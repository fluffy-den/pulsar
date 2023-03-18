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
				pf_assert(this->maxcount > CCY_NUM_THREADS * 64, "maxcount_ must be greater than {}. maxcount_={}", CCY_NUM_THREADS * 64,  this->maxcount);
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

			/// Enqueue
			bool __try_enqueue(
				_Ty *__p) pf_attr_noexcept
			{
				// Initialisation
				size_t h = this->head.load(atomic_order::relaxed) % this->maxcount;
				size_t t = this->tail.load(atomic_order::acquire) % this->maxcount;
				size_t c = writers.fetch_add(1, atomic_order::relaxed);

				// Verify
				size_t d			 = h <= t ? this->maxcount - h + t : t - h;
				const size_t n = c + 1;
				if ((t + n % this->maxcount) > d)
				{
					this->writers.fetch_sub(1, atomic_order::relaxed);
					return false;
				}

				// Add
				t = this->tail.fetch_add(1, atomic_order::relaxed);
				this->writers.fetch_sub(1, atomic_order::relaxed);
				t									= this->__shuffle(t);
				this->elements[t] = __p;
				return false;
			}
			void __enqueue(
				_Ty *__p)
			{
				// Add
				size_t t = this->tail.fetch_add(1, atomic_order::relaxed);
				this->elements[this->__shuffle(t)] = __p;
			}

			/// Dequeue
			pf_hint_nodiscard _Ty*
			__try_dequeue() pf_attr_noexcept
			{
				size_t h = this->head.load(atomic_order::acquire) % this->maxcount;
				size_t t = this->tail.load(atomic_order::relaxed) % this->maxcount;
				if (h == t) return nullptr;
				size_t c = readers.fetch_add(1, atomic_order::relaxed);

				// Verify
				size_t d			 = h <= t ? t - h : this->maxcount - h + t;
				const size_t n = c + 1;
				if ((t + n % this->maxcount) > d)
				{
					this->readers.fetch_sub(1, atomic_order::relaxed);
					return nullptr;
				}

				// Add
				h = this->head.fetch_add(1, atomic_order::relaxed);
				this->readers.fetch_sub(1, atomic_order::relaxed);
				h = this->__shuffle(h);
				return this->elements[h];
			}

			/// Empty
			pf_hint_nodiscard bool
			__empty() const pf_attr_noexcept
			{
				return this->head.load(atomic_order::relaxed) == this->tail.load(atomic_order::relaxed);
			}

			/// Store
			// Flexible Arrays -> Disable warning
	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wpedantic"

			pf_alignas(CCY_ALIGN) atomic<size_t> head;
			pf_alignas(CCY_ALIGN) atomic<size_t> tail;
			pf_alignas(CCY_ALIGN) atomic<size_t> writers;
			pf_alignas(CCY_ALIGN) atomic<size_t> readers;
			const size_t maxcount;
			const size_t seqcount;
			pf_alignas(CCY_ALIGN) _Ty *elements[];

			// Flexible Arrays
	#pragma GCC diagnostic pop
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
			: buf_(this->__new_buffer(__r.buf_->maxcount))
		{}
		mpmc_queue(mpmc_queue<_Ty> && __r)
			: buf_(__r.buf_)
		{
			__r.buf_ = this->__new_buffer(__r.buf_->maxcount);
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
			if (pf_likely(this != __r))
			{
				pf_assert(this->__empty(), "Deleting not empty buffer!");
				this->__delete_buffer(this->buf_);
				this->buf_ = this->__new_buffer(__r.buf_->seqcount);
			}
			return *this;
		}
		mpmc_queue<_Ty> &operator=(
			mpmc_queue<_Ty> &&__r) pf_attr_noexcept
		{
			if (pf_likely(this != __r))
			{
				pf_assert(this->__empty(), "Deleting not empty buffer!");
				this->__delete_buffer(this->buf_);
				this->buf_ = __r.buf_;
				__r.buf_	 = this->__new_buffer(__r.buf_->seqcount);
			}
			return *this;
		}

		/// enqueue
		bool try_enqueue(
			_Ty *__p) pf_attr_noexcept
		{
			return this->buf_->__try_enqueue(__p);
		}
		void enqueue(
			_Ty *__p) pf_attr_noexcept
		{
			return this->buf_->__enqueue(__p);
		}

		/// dequeue
		pf_hint_nodiscard _Ty*
		try_dequeue() pf_attr_noexcept
		{
			return this->buf_->__try_dequeue();
		}

	private:
		// Store
		__buffer_t *buf_;
	};

	/// QUEUE: MPMC 2 (Multi-CAS)
	template <typename _Ty>
	class mpmc_queue2
	{
	private:
		/// Type -> Header
		struct __header_t
		{
			/// Constructors
			__header_t() pf_attr_noexcept
				: head(0)
				, padd1{ 0 }
				, tail(0)
				, padd2{ 0 }
			{}
			__header_t(__header_t const &) = delete;
			__header_t(__header_t &&)			 = delete;

			/// Destructor
			~__header_t() pf_attr_noexcept = default;

			/// Operator =
			__header_t &operator=(__header_t const &) = delete;
			__header_t &operator=(__header_t &&)			= delete;

			/// Store
			pf_alignas(CCY_ALIGN) atomic<uint32_t> head;
			byte_t padd1[static_cast<size_t>(CCY_ALIGN) - sizeof(atomic<size_t>)];
			pf_alignas(CCY_ALIGN) atomic<uint32_t> tail;
			byte_t padd2[static_cast<size_t>(CCY_ALIGN) - sizeof(atomic<size_t>)];
		};

		// [b ][H1][H2][..][HN][L1][L2][..][LN]

		/// Type -> Buffer
		struct __buffer_t
		{
			/// Constructors
			__buffer_t(
				size_t __seqcount) pf_attr_noexcept
				: seqcount(__seqcount)
			{
				pf_assert(this->seqcount > CCY_NUM_THREADS * 64, "seqcount_ must be greater than {}. seqcount_={}", CCY_NUM_THREADS * 64,  this->seqcount);
				pf_assert(is_power_of_two(this->seqcount), "seqcount_ must be a power of two!");

				// Construct lists
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
				// Destroy lists
				for (size_t i = 0; i < CCY_NUM_THREADS; ++i)
				{
					destroy(this->__get_list(i));
				}
			}

			/// Operator =
			__buffer_t &operator=(__buffer_t const &) = delete;
			__buffer_t &operator=(__buffer_t &&)			= delete;

			/// Get Header
			pf_hint_nodiscard pf_decl_inline __header_t*
			__get_header(
				uint32_t __k) pf_attr_noexcept
			{

				return union_cast<__header_t*>(
					&this->store[0] + __k * sizeof(__header_t));
			}

			/// Get list
			pf_hint_nodiscard pf_decl_inline _Ty**
			__get_list(
				uint32_t __k) pf_attr_noexcept
			{
				return union_cast<_Ty**>(
					&this->store[0] + CCY_NUM_THREADS * sizeof(__header_t) + __k * this->seqcount * sizeof(_Ty*));
			}

			/// Enqueue
			bool
			__try_enqueue(
				_Ty *__ptr) pf_attr_noexcept
			{
				const size_t id = (this_thread::id - 1) % CCY_NUM_THREADS;
				uint32_t i			= id % CCY_NUM_THREADS;
				do
				{
					__header_t *c = this->__get_header(i);
					uint32_t h		= c->head.load(atomic_order::relaxed);
					uint32_t t		= c->tail.load(atomic_order::acquire);
					if (t == h - 1
							|| !c->tail.compare_exchange_strong(
								t, t + 1, atomic_order::release, atomic_order::relaxed))
					{
						i = (i + 1) % CCY_NUM_THREADS;
					}
					else
					{
						std::atomic_thread_fence(atomic_order::relaxed);
						this->__get_list(i)[t % this->seqcount] = __ptr;
						return true;
					}
				} while (i != id);
				return false;
			}

			/// Dequeue
			pf_hint_nodiscard _Ty*
			__try_dequeue() pf_attr_noexcept
			{
				const size_t id = (this_thread::id - 1) % CCY_NUM_THREADS;
				uint32_t i			= id % CCY_NUM_THREADS;
				do
				{
					__header_t *c = this->__get_header(i);
					uint32_t h		= c->head.load(atomic_order::acquire);
					uint32_t t		= c->tail.load(atomic_order::relaxed);
					if (h == t
							|| !c->head.compare_exchange_strong(
								h, h + 1, atomic_order::release, atomic_order::relaxed))
					{
						i = (i + 1) % CCY_NUM_THREADS;
					}
					else
					{
						std::atomic_thread_fence(atomic_order::relaxed);
						return this->__get_list(i)[h % this->seqcount];
					}
				} while (i != id);
				return nullptr;
			}

			/// Empty
			pf_hint_nodiscard bool
			__empty() const pf_attr_noexcept
			{
				for (size_t i = 0; i < CCY_NUM_THREADS; ++i)
				{
					__header_t *c = this->__get_header(i);
					uint32_t h		= c->head.load(atomic_order::relaxed);
					uint32_t t		= c->tail.load(atomic_order::relaxed);
					if (h != t) return false;
				}
				return true;
			}

			/// Store
			// Flexible Arrays -> Disable warning
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"

			const size_t seqcount;
			pf_alignas(CCY_ALIGN) byte_t store[];

			// Flexible Arrays
#pragma GCC diagnostic pop
		};

		/// Buffer -> New
		pf_hint_nodiscard __buffer_t*
		__new_buffer(
			size_t __seqcount) pf_attr_noexcept
		{
			return new_construct_ex<__buffer_t>(sizeof(__buffer_t) + CCY_NUM_THREADS * (sizeof(__header_t) + __seqcount * sizeof(_Ty*)), __seqcount);
		}

		/// Buffer -> Delete
		void
		__delete_buffer(
			__buffer_t *__buffer) pf_attr_noexcept
		{
			destroy_delete(__buffer);
		}

	public:
		/// Constructors
		mpmc_queue2(
			size_t __seqcount)
			: buf_(this->__new_buffer(__seqcount))
		{}
		mpmc_queue2(
			mpmc_queue2<_Ty> const& __r)
			: buf_(this->__new_buffer(__r.buf_->seqcount))
		{}
		mpmc_queue2(
			mpmc_queue2<_Ty> && __r)
			: buf_(__r.buf_)
		{
			__r.buf_ = this->__new_buffer(__r.buf_->seqcount);
		}

		/// Operator =
		mpmc_queue2<_Ty>&
		operator=(
			mpmc_queue2<_Ty> const &__r)
		{
			if (pf_likely(this != __r))
			{
				pf_assert(this->__empty(), "Deleting not empty buffer!");
				this->__delete_buffer(this->buf_);
				this->buf_ = this->__new_buffer(__r.buf_->seqcount);
			}
			return *this;
		}
		mpmc_queue2<_Ty>&
		operator=(
			mpmc_queue2<_Ty> &&__r)
		{
			if (pf_likely(this != __r))
			{
				pf_assert(this->__empty(), "Deleting not empty buffer!");
				this->__delete_buffer(this->buf_);
				this->buf_ = __r.buf_;
				__r.buf_	 = this->__new_buffer(__r.buf_->seqcount);
			}
			return *this;
		}

		/// Destructor
		~mpmc_queue2() pf_attr_noexcept
		{
			this->__delete_buffer(this->buf_);
		}

		/// Enqueue
		bool
		try_enqueue(
			_Ty *__ptr) pf_attr_noexcept
		{
			return this->buf_->__try_enqueue(__ptr);
		}

		/// Dequeue
		pf_hint_nodiscard _Ty*
		try_dequeue() pf_attr_noexcept
		{
			return this->buf_->__try_dequeue();
		}

	private:
		/// Store
		__buffer_t *buf_;
	};
}

#endif // !PULSAR_MPMC_QUEUE_HPP