/*! @file   mpsc_singly_lifo.hpp
 *  @author Louis-Quentin Noé (noe.louis-quentin@hotmail.fr)
 *  @brief
 *  @date   17-03-2023
 *
 *  @copyright Copyright (c) 2023 - Pulsar Software
 *
 *  @since 0.1.4
 */

#ifndef PULSAR_MPMC_SINGLY_LIFO_HPP
#define PULSAR_MPMC_SINGLY_LIFO_HPP 1

// Include: Pulsar
#include "pulsar/debug.hpp"
#include "pulsar/memory.hpp"
#include "pulsar/utility.hpp"
#include "pulsar/concurrency.hpp"

// Pulsar
namespace pul
{
	/// SINGLY: MPSC List
	template <typename _Ty>
	class mpsc_singly_lifo
	{
	private:
		/// Type -> Node
		struct __node_t
		{
			// Flexible Arrays -> Disable warning
	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wpedantic"

			__node_t *next = nullptr;
			_Ty store;

			// Flexible Arrays
	#pragma GCC diagnostic pop
		};

		/// Type -> Cache
		struct __list_t
		{
			/// Store
			pf_alignas(CCY_ALIGN) atomic<__node_t*> tail																								= nullptr;
			__node_t *head																																							= nullptr;
			byte_t padd[static_cast<size_t>(CCY_ALIGN) - sizeof(atomic<__node_t*>) - sizeof(__node_t*)] = {0};
		};

		/// Type -> Buffer
		struct __buffer_t
		{
			/// Constructors
			__buffer_t() pf_attr_noexcept
			{
				for (size_t i = 0; i < CCY_NUM_THREADS; ++i)
				{
					construct(this->__get_list(i));
				}
			}
			__buffer_t(__buffer_t const &) = delete;
			__buffer_t(__buffer_t &&)			 = delete;

			/// Destructor
			~__buffer_t() pf_attr_noexcept
			{
				for (size_t i = 0; i < CCY_NUM_THREADS; ++i)
				{
					destroy(this->__get_list(i));
				}
			}

			/// Operator =
			__buffer_t &operator=(
				__buffer_t const &) = delete;
			__buffer_t &operator=(
				__buffer_t &&) = delete;

			/// Get k cache
			pf_hint_nodiscard __list_t*
			__get_list(
				size_t __k) pf_attr_noexcept
			{
				return union_cast<__list_t*>(&this->store[0] + __k * sizeof(__list_t));
			}

			/// Insert Tail
			pf_hint_nodiscard bool
			__insert_tail(
				__node_t *__n) pf_attr_noexcept
			{
				size_t i = counter.fetch_add(1, atomic_order::relaxed) % CCY_NUM_THREADS;
				while(true)
				{
					__list_t *l = this->__get_list(i);
					__node_t *b = l->tail.load(atomic_order::relaxed);
					__node_t *t = b;
					if (l->tail.compare_exchange_weak(t, __n, atomic_order::release, atomic_order::relaxed))
					{
						if (pf_unlikely(!t))
						{
							std::atomic_thread_fence(atomic_order::relaxed);
							l->head = __n;
						}
						else
						{
							b->next = __n;
						}
						return true;
					}
					else
					{
						i = counter.fetch_add(1, atomic_order::relaxed) % CCY_NUM_THREADS;
					}
				}
			}

			/// Remove Head
			pf_hint_nodiscard __node_t*
			__remove_head() pf_attr_noexcept
			{
				__node_t *h = nullptr;
				__node_t *t = nullptr;
				size_t i		= 0;

				// Initialize list
				do
				{
					__list_t *l = this->__get_list(i);
					h = l->head;
					if (l->tail.load(atomic_order::relaxed))
					{
						std::atomic_thread_fence(atomic_order::relaxed);
						l->head = nullptr;
						t				= l->tail.exchange(nullptr, atomic_order::release);
					}
					++i;
				} while (!t && i < CCY_NUM_THREADS);

				// Return lists
				while (i < CCY_NUM_THREADS)
				{
					__list_t *l = this->__get_list(i);
					if (l->tail.load(atomic_order::relaxed))
					{
						std::atomic_thread_fence(atomic_order::relaxed);
						t->next = l->head;
						l->head = nullptr;
						t				= l->tail.exchange(nullptr, atomic_order::relaxed);
						++i;
					}
				}
				if (pf_likely(t)) t->next = nullptr;// Security
				return h;
			}

			// Flexible Arrays -> Disable warning
	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wpedantic"

			/// Store
			pf_alignas(CCY_ALIGN) atomic<size_t> counter;
			pf_alignas(CCY_ALIGN) byte_t store[1];

			// Flexible Arrays
	#pragma GCC diagnostic pop
		};

		/// Buffer -> New
		pf_hint_nodiscard __buffer_t*
		__new_buffer()
		{
			return new_construct_ex<__buffer_t>(sizeof(__buffer_t) + sizeof(__list_t) * CCY_NUM_THREADS);
		}

		/// Buffer -> Delete
		void
		__delete_buffer(
			__buffer_t *__buf) pf_attr_noexcept
		{
			destroy_delete(__buf);
		}

	public:
		using node_t = __node_t;

		/// Constructors
		mpsc_singly_lifo()
			: buf_(this->__new_buffer())
		{}
		mpsc_singly_lifo(
			mpsc_singly_lifo const&)
			: buf_(this->__new_buffer())
		{}
		mpsc_singly_lifo(
			mpsc_singly_lifo &&__r)
			: buf_(__r.buf_)
		{
			__r.buf_ = nullptr;
		}

		/// Destructor
		~mpsc_singly_lifo() pf_attr_noexcept
		{
			if (this->buf_)
			{
				this->__delete_buffer(this->buf_);
			}
		}

		/// Operator =
		mpsc_singly_lifo<_Ty> &
		operator=(
			mpsc_singly_lifo<_Ty> const &__r) pf_attr_noexcept
		{
			return *this;
		}
		mpsc_singly_lifo<_Ty> &
		operator=(
			mpsc_singly_lifo<_Ty> &&__r) pf_attr_noexcept
		{
			if (pf_likely(__r != this))
			{
				this->buf_ = __r.buf_;
				__r.buf_	 = nullptr;
			}
			return *this;
		}

		/// Insert Tail
		bool
		insert_tail(
			node_t *__n) pf_attr_noexcept
		{
			return this->buf_->__insert_tail(__n);
		}

		/// Remove Head
		pf_hint_nodiscard node_t*
		remove_head() pf_attr_noexcept
		{
			return this->buf_->__remove_head();
		}

	private:
		__buffer_t *buf_;
	};
}


#endif // !PULSAR_MPMC_SINGLY_LIFO_HPP