/*! @file   mpmc_queue.hpp
 *  @author Louis-Quentin Noé (noe.louis-quentin@hotmail.fr)
 *  @brief
 *  @date   09-03-2023
 *
 *  @copyright Copyright (c) 2023 - Pulsar Software
 *
 *  @since 0.1.4
 */

#ifndef PULSAR_MPMC_QUEUE_HPP
#define PULSAR_MPMC_QUEUE_HPP 1

// Include: Pulsar
#include "pulsar/debug.hpp"
#include "pulsar/memory.hpp"
#include "pulsar/utility.hpp"
#include "pulsar/concurrency.hpp"

// Pulsar
namespace pul
{
	// https://github.com/max0x7ba/atomic_queue/blob/master/include/atomic_queue/atomic_queue.h

	/// QUEUE: MPMC
	template <typename _Ty>
	class mpmc_queue
	{
	private:
		// Type -> Buffer
		struct __buffer_t
		{
			/// Constructors
			__buffer_t(
				size_t __maxcount) pf_attr_noexcept
				: head(0)
				, tail(0)
				, writers(0)
				, readers(0)
				, maxcount(__maxcount)
				, seqcount(__maxcount / CCY_NUM_THREADS)
			{
				pf_assert(this->maxcount > 1024, "maxcount_ must be greater than 1024. maxcount_={}", this->maxcount);
				pf_assert(is_power_of_two(this->maxcount), "maxcount_ must be a power of two!");
			}
			__buffer_t(__buffer_t const &) = delete;
			__buffer_t(__buffer_t &&)			 = delete;

			/// Destructor
			~__buffer_t() pf_attr_noexcept = default;

			/// Operator =
			__buffer_t &operator=(__buffer_t const &) = delete;
			__buffer_t &operator=(__buffer_t &&)			= delete;

			/// Shuffle
			size_t __shuffle(
				size_t __index) pf_attr_noexcept
			{
				return (__index % CCY_NUM_THREADS) * this->seqcount + (__index / CCY_NUM_THREADS);
			}

			/// Push
			bool __try_push(
				_Ty *__p) pf_attr_noexcept
			{
				// Initialisation
				size_t h = this->head.load(atomic_order::acquire) % this->maxcount;
				size_t t = this->tail.load(atomic_order::acquire) % this->maxcount;
				size_t c = writers.fetch_add(1, atomic_order::acq_rel);

				// Verify
				size_t d			 = h <= t ? d = this->maxcount - h + t : t - h;
				const size_t n = c + 1;
				if ((t + n % this->maxcount) > d)
				{
					this->writers.fetch_sub(1, atomic_order::release);
					return false;
				}

				// Add
				t = this->tail.fetch_add(1, atomic_order::acq_rel);
				this->writers.fetch_sub(1, atomic_order::release);
				t									= this->__shuffle(t);
				this->elements[t] = __p;
				return false;
			}
			void __push(
				_Ty *__p)
			{
				// Add
				size_t t = this->tail.fetch_add(1, atomic_order::acq_rel);
				this->elements[this->__shuffle(t)] = __p;
			}

			/// Pop
			pf_hint_nodiscard _Ty*
			__try_pop() pf_attr_noexcept
			{
				size_t h = this->head.load(atomic_order::acquire) % this->maxcount;
				size_t t = this->tail.load(atomic_order::acquire) % this->maxcount;
				if (pf_unlikely(h == t)) return nullptr;
				size_t c = readers.fetch_add(1, atomic_order::acquire);

				// Verify
				size_t d			 = h <= t ? t - h : d = this->maxcount - h + t;
				const size_t n = c + 1;
				if ((t + n % this->maxcount) > d)
				{
					this->writers.fetch_sub(1, atomic_order::release);
					return nullptr;
				}

				// Add
				t = this->head.fetch_add(1, atomic_order::acq_rel);
				this->readers.fetch_sub(1, atomic_order::release);
				t = this->__shuffle(t);
				return this->elements[t];
			}

			/// Store
			pf_alignas(CCY_ALIGN) atomic<size_t> head;
			pf_alignas(CCY_ALIGN) atomic<size_t> tail;
			pf_alignas(CCY_ALIGN) atomic<size_t> writers;
			pf_alignas(CCY_ALIGN) atomic<size_t> readers;
			const size_t maxcount;
			const size_t seqcount;
			pf_alignas(CCY_ALIGN) _Ty *elements[];
		};

		/// Buffer -> New
		pf_hint_nodiscard __buffer_t*
		__new_buffer(
			size_t __maxcount)
		{
			return new_construct_ex<__buffer_t>(sizeof(_Ty*) * __maxcount, __maxcount);
		}

		/// Buffer -> Delete
		void __delete_buffer(
			__buffer_t *__buffer)
		{
			destroy_delete(__buffer);
		}

	public:
		/// Constructors
		mpmc_queue(
			size_t __maxcount)
			: buf_(this->__new_buffer(__maxcount))
		{}
		mpmc_queue(mpmc_queue<_Ty> const &__r)
		{

		}
		mpmc_queue(mpmc_queue<_Ty> && __r)
		{

		}

		/// Destructor
		~mpmc_queue() pf_attr_noexcept
		{
			this->__delete_buffer(this->buf_);
		}

		/// Operator =
		mpmc_queue<_Ty> &operator=(
			mpmc_queue<_Ty> const &__r) pf_attr_noexcept
		{

		}
		mpmc_queue<_Ty> &operator=(
			mpmc_queue<_Ty> &&__r) pf_attr_noexcept
		{

		}

		/// Push
		bool try_push_front(
			_Ty *__p) pf_attr_noexcept
		{
			return this->buf_->__try_push(__p);
		}
		void push_front(
			_Ty *__p) pf_attr_noexcept
		{
			return this->buf_->__push(__p);
		}

		/// Pop
		pf_hint_nodiscard _Ty*
		try_pop_back() pf_attr_noexcept
		{
			return this->buf_->__try_pop();
		}

	private:
		// Store
		__buffer_t *buf_;
	};
}

#endif // !PULSAR_MPMC_QUEUE_HPP