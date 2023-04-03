/*! @file   queue.hpp
 *  @author Louis-Quentin Noé (noe.louis-quentin@hotmail.fr)
 *  @brief
 *  @date   09-03-2023
 *
 *  @copyright Copyright (c) 2023 - Pulsar Software
 *
 *  @since 0.1.4
 */

#ifndef PULSAR_QUEUE_HPP
#define PULSAR_QUEUE_HPP 1

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
	class mpmc_lifo
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
				size_t t = this->tail.load(atomic_order::relaxed) % this->maxcount;
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
				size_t h = this->head.load(atomic_order::relaxed) % this->maxcount;
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
		mpmc_lifo(
			size_t __maxcount)
			: buf_(this->__new_buffer(__maxcount))
		{}
		mpmc_lifo(mpmc_lifo<_Ty> const &__r)
			: buf_(this->__new_buffer(__r.buf_->maxcount))
		{}
		mpmc_lifo(mpmc_lifo<_Ty> && __r)
			: buf_(__r.buf_)
		{
			__r.buf_ = this->__new_buffer(__r.buf_->maxcount);
		}

		/// Destructor
		~mpmc_lifo() pf_attr_noexcept
		{
			this->__delete_buffer(this->buf_);
		}

		/// Operator =
		mpmc_lifo<_Ty> &operator=(
			mpmc_lifo<_Ty> const &__r) pf_attr_noexcept
		{
			if (pf_likely(this != __r))
			{
				pf_assert(this->__empty(), "Deleting not empty buffer!");
				this->__delete_buffer(this->buf_);
				this->buf_ = this->__new_buffer(__r.buf_->seqcount);
			}
			return *this;
		}
		mpmc_lifo<_Ty> &operator=(
			mpmc_lifo<_Ty> &&__r) pf_attr_noexcept
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
	class mpmc_lifo2
	{
	private:
		/// Type -> Header
		struct __header_t
		{
			/// Constructors
			__header_t() pf_attr_noexcept
				: head(0)
				, tail(0)
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
			pf_alignas(CCY_ALIGN) atomic<uint32_t> writer;
			pf_alignas(CCY_ALIGN) atomic<uint32_t> tail;
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
			size_t
			__try_enqueue(
				_Ty *__ptr) pf_attr_noexcept
			{
				const thread_id_t id = this_thread::get_id();
				uint32_t i					 = id;
				do
				{
					__header_t *c		 = this->__get_header(i);
					const uint32_t h = c->head.load(atomic_order::acquire);
					uint32_t t			 = c->tail.load(atomic_order::acquire);
					if (t == h - 1
							|| !c->tail.compare_exchange_strong(
								t, t + 1, atomic_order::release, atomic_order::relaxed))
					{
						i = (i + 1) % CCY_NUM_THREADS;
					}
					else
					{
						auto l = this->__get_list(i);
						l[t % this->seqcount] = __ptr;
						c->writer.fetch_add(1, atomic_order::relaxed);
						return 1;
					}
				} while (i != id);
				return 0;
			}
			template <typename _IteratorIn>
			size_t
			__try_enqueue_bulk(
				_IteratorIn __beg,
				_IteratorIn __end) pf_attr_noexcept
			requires(is_iterator_v<_IteratorIn>
							 && std::is_same_v<value_type_t<_IteratorIn>, _Ty*>)
			{
				const uint32_t count = countof(__beg, __end);
				if (pf_unlikely(count == 0 || count > this->seqcount)) return 0;
				const thread_id_t id = this_thread::get_id();
				uint32_t i					 = id;
				do
				{
					__header_t *c		 = this->__get_header(i);
					const uint32_t h = c->head.load(atomic_order::acquire);
					uint32_t t			 = c->tail.load(atomic_order::acquire);
					const uint32_t n = (t + count);
					if ((n > h - 1 && n > union_cast<uint32_t>(-1) - count)
							|| !c->tail.compare_exchange_strong(
								t, t + count, atomic_order::release, atomic_order::relaxed))
					{
						i = (i + 1) % CCY_NUM_THREADS;
					}
					else
					{
						auto l = this->__get_list(i);
						for (uint32_t k = 0; k < count; ++k)
						{
							l[(t + k) % this->seqcount] = __beg[k];
						}
						c->writer.fetch_add(count, atomic_order::relaxed);
						return count;
					}
				} while (i != id);
				return 0;
			}

			/// Dequeue
			pf_hint_nodiscard _Ty*
			__try_dequeue() pf_attr_noexcept
			{
				const thread_id_t id = this_thread::get_id();
				uint32_t i					 = id;
				do
				{
					__header_t *c		 = this->__get_header(i);
					uint32_t h			 = c->head.load(atomic_order::acquire);
					const uint32_t t = c->writer.load(atomic_order::acquire);
					if (h == t
							|| !c->head.compare_exchange_strong(
								h, h + 1, atomic_order::release, atomic_order::relaxed))
					{
						i = (i + 1) % CCY_NUM_THREADS;
					}
					else
					{
						auto l = this->__get_list(i);
						return l[h % this->seqcount];
					}
				} while (i != id);
				return nullptr;
			}
			template <typename _IteratorOut>
			pf_hint_nodiscard size_t
			__try_dequeue_bulk(
				_IteratorOut __beg,
				_IteratorOut __end) pf_attr_noexcept
			requires(is_iterator_v<_IteratorOut>
							 && std::is_same_v<value_type_t<_IteratorOut>, _Ty*>)
			{
				const thread_id_t id = this_thread::get_id();
				uint32_t count			 = countof(__beg, __end);
				if (pf_unlikely(count == 0)) return 0;
				uint32_t i = id;
				do
				{
					__header_t *c						 = this->__get_header(i);
					uint32_t h							 = c->head.load(atomic_order::acquire);
					const uint32_t t				 = c->writer.load(atomic_order::acquire);
					const uint32_t available = t - h;
					if (count > available)
						count = available;
					if (count == 0
							|| !c->head.compare_exchange_strong(
								h, h + count, atomic_order::release, atomic_order::relaxed))
					{
						i = (i + 1) % CCY_NUM_THREADS;
					}
					else
					{
						auto l = this->__get_list(i);
						for (size_t k = 0; k < count; ++k)
						{
							__beg[k] = l[(h + k) % this->seqcount];
						}
						return count;
					}
				} while (i != id);
				return 0;
			}

			/// Empty
			pf_hint_nodiscard bool
			__empty() const pf_attr_noexcept
			{
				for (size_t i = 0; i < CCY_NUM_THREADS; ++i)
				{
					__header_t *c = this->__get_header(i);
					uint32_t h		= c->head.load(atomic_order::acquire);
					uint32_t t		= c->tail.load(atomic_order::acquire);
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
			__buffer_t *b = new_construct_ex<__buffer_t>(CCY_NUM_THREADS * (sizeof(__header_t) + __seqcount * sizeof(_Ty*)), __seqcount);
			std::memset(&b->store[0] + CCY_NUM_THREADS * sizeof(__header_t), 0, __seqcount * sizeof(_Ty*) * CCY_NUM_THREADS);
			return b;
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
		mpmc_lifo2(
			size_t __seqcount)
			: buf_(this->__new_buffer(__seqcount))
		{}
		mpmc_lifo2(
			mpmc_lifo2<_Ty> const& __r)
			: buf_(this->__new_buffer(__r.buf_->seqcount))
		{}
		mpmc_lifo2(
			mpmc_lifo2<_Ty> && __r)
			: buf_(__r.buf_)
		{
			__r.buf_ = this->__new_buffer(__r.buf_->seqcount);
		}

		/// Operator =
		mpmc_lifo2<_Ty>&
		operator=(
			mpmc_lifo2<_Ty> const &__r)
		{
			if (pf_likely(this != __r))
			{
				pf_assert(this->__empty(), "Deleting not empty buffer!");
				this->__delete_buffer(this->buf_);
				this->buf_ = this->__new_buffer(__r.buf_->seqcount);
			}
			return *this;
		}
		mpmc_lifo2<_Ty>&
		operator=(
			mpmc_lifo2<_Ty> &&__r)
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
		~mpmc_lifo2() pf_attr_noexcept
		{
			this->__delete_buffer(this->buf_);
		}

		/// Enqueue
		pf_decl_inline bool
		try_enqueue(
			_Ty *__ptr) pf_attr_noexcept
		{
			return this->buf_->__try_enqueue(__ptr);
		}
		template <typename _IteratorIn>
		pf_decl_inline bool
		try_enqueue_bulk(
			_IteratorIn __beg,
			_IteratorIn __end) pf_attr_noexcept
		requires(is_iterator_v<_IteratorIn>
						 && std::is_same_v<value_type_t<_IteratorIn>, _Ty*>)
		{
			return this->buf_->__try_enqueue_bulk(__beg, __end);
		}

		/// Dequeue
		pf_hint_nodiscard pf_decl_inline _Ty*
		try_dequeue() pf_attr_noexcept
		{
			return this->buf_->__try_dequeue();
		}
		template <typename _IteratorOut>
		pf_hint_nodiscard pf_decl_inline size_t
		try_dequeue_bulk(
			_IteratorOut __beg,
			_IteratorOut __end) pf_attr_noexcept
		requires(is_iterator_v<_IteratorOut>
						 && std::is_same_v<value_type_t<_IteratorOut>, _Ty*>)
		{
			return this->buf_->__try_dequeue_bulk(__beg, __end);
		}

	private:
		/// Store
		__buffer_t *buf_;
	};

	/// SINGLY: MPSC List
	template <typename _NodeTy>
	class mpsc_singly_lifo
	{
	pf_assert_static(!std::is_const_v<_NodeTy>, "_NodeTy is a constant type!");
	pf_assert_static(!std::is_pointer_v<_NodeTy>, "_NodeTy is a pointer type!");

	private:
		/// Type -> Cache
		struct __list_t
		{
			/// Constructors
			__list_t() pf_attr_noexcept
				: head(nullptr)
				, tail(nullptr)
			{}
			__list_t(__list_t const &) = delete;
			__list_t(__list_t &&)			 = delete;

			/// Destructor
			~__list_t() pf_attr_noexcept = default;

			/// Operator =
			__list_t &operator=(__list_t const &) = delete;
			__list_t &operator=(__list_t &&)			= delete;

			/// Store
			pf_alignas(CCY_ALIGN) atomic<_NodeTy*> head;
			pf_alignas(CCY_ALIGN) atomic<_NodeTy*> tail;
			byte_t padd[static_cast<size_t>(CCY_ALIGN) - sizeof(atomic<_NodeTy*>) - sizeof(_NodeTy*)] = {0};
		};

		/// Type -> Buffer
		struct __buffer_t
		{
			/// Constructors
			__buffer_t() pf_attr_noexcept
			{
				for (size_t i = 0; i != CCY_NUM_THREADS; ++i)
				{
					auto l = this->__get_list(i);
					construct(l);
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
				uint32_t __k) pf_attr_noexcept
			{
				return union_cast<__list_t*>(&this->store[0] + __k * sizeof(__list_t));
			}

			/// Enqueue
			void
			__enqueue_bulk(
				_NodeTy *__b,
				_NodeTy *__e) pf_attr_noexcept
			{
				// Initialization
				uint32_t i = this_thread::get_id();
				auto *l		 = this->__get_list(i);
				_NodeTy *t = l->tail.load(atomic_order::relaxed);

				// Loop
				while (!l->tail.compare_exchange_weak(t, __e, atomic_order::release, atomic_order::relaxed));
				if (t)
				{
					std::atomic_thread_fence(atomic_order::release);
					t->next = __b;
				}
				else
				{
					_NodeTy *h = l->head.load(atomic_order::relaxed);
					while (h != nullptr) h = l->head.load(atomic_order::relaxed);	// SPSC dequeue
					while (!l->head.compare_exchange_weak(h, __b, atomic_order::release, atomic_order::relaxed));
				}
			}


			/// Dequeue
			pf_hint_nodiscard _NodeTy*
			__dequeue_all() pf_attr_noexcept
			{
				// Initialization
				uint32_t i = 0;
				_NodeTy *b = nullptr;
				_NodeTy *e = nullptr;

				// Create 1. list
				while (!b && i < CCY_NUM_THREADS)
				{
					auto *l		 = this->__get_list(i);
					_NodeTy *t = l->tail.load(atomic_order::relaxed);
					while (!l->tail.compare_exchange_weak(t, nullptr, atomic_order::release, atomic_order::relaxed));
					if (t)
					{
						_NodeTy *h = nullptr;
						do
						{
							h = l->head.exchange(nullptr, atomic_order::relaxed);	// SPSC dequeue
						} while (!h);
						b = h;
						e = t;
					}
					++i;
				}

				// Link to 1.
				while (i < CCY_NUM_THREADS)
				{
					auto *l		 = this->__get_list(i);
					_NodeTy *t = l->tail.load(atomic_order::relaxed);
					while (!l->tail.compare_exchange_weak(t, nullptr, atomic_order::release, atomic_order::relaxed));
					if (t)
					{
						_NodeTy *h = nullptr;
						do
						{
							h = l->head.exchange(nullptr, atomic_order::relaxed);	// SPSC dequeue
						} while (!h);
						std::atomic_thread_fence(atomic_order::release);
						e->next = t;
						e				= h;
					}
					++i;
				}

				// Return 1.
				return b;
			}

			/// Store
			pf_alignas(CCY_ALIGN) byte_t store[1];
		};

	public:
		using node_t = _NodeTy;

		/// Constructors
		mpsc_singly_lifo()
			: buf_(new_construct_ex<__buffer_t>(sizeof(__list_t) * CCY_NUM_THREADS))
		{}
		mpsc_singly_lifo(
			mpsc_singly_lifo<_NodeTy> const&)
			: buf_(new_construct_ex<__buffer_t>(sizeof(__list_t) * CCY_NUM_THREADS))
		{}
		mpsc_singly_lifo(
			mpsc_singly_lifo<_NodeTy> &&__r)
			: buf_(__r.buf_)
		{
			__r.buf_ = nullptr;
		}

		/// Destructor
		~mpsc_singly_lifo() pf_attr_noexcept
		{
			if (this->buf_) destroy_delete(this->buf_);
		}

		/// Operator =
		mpsc_singly_lifo<_NodeTy> &
		operator=(
			mpsc_singly_lifo<_NodeTy> const &__r) pf_attr_noexcept
		{
			return *this;
		}
		mpsc_singly_lifo<_NodeTy> &
		operator=(
			mpsc_singly_lifo<_NodeTy> &&__r) pf_attr_noexcept
		{
			if (pf_likely(__r != this))
			{
				this->buf_ = __r.buf_;
				__r.buf_	 = nullptr;
			}
			return *this;
		}

		/// Insert Tail
		pf_decl_inline void
		enqueue_bulk(
			node_t *__b,
			node_t *__e) pf_attr_noexcept
		{
	#ifdef PF_DEBUG

			node_t *l = __b;
			while (l->next) l = l->next;
			pf_assert(l == __e, "__b to __e isn't well formed!");

	#endif // PF_DEBUG

			return this->buf_->__enqueue_bulk(__b, __e);
		}
		pf_decl_inline void
		enqueue(
			node_t *__n) pf_attr_noexcept
		{
			this->enqueue_bulk(__n, __n);
		}

		/// Remove Head
		pf_hint_nodiscard pf_decl_inline node_t*
		dequeue() pf_attr_noexcept
		{
			return this->buf_->__dequeue_all();
		}

	private:
		__buffer_t *buf_;
	};
}

#endif // !PULSAR_QUEUE_HPP