/*! @file   allocator_ring.hpp
 *  @author Louis-Quentin Noé (noe.louis-quentin@hotmail.fr)
 *  @brief  Thread local lock-free ring buffer.
 *  @date   05-03-2023
 *
 *  @copyright Copyright (c) 2023 - Pulsar Software
 *
 *  @since 0.1.4
 */

#ifndef PULSAR_RING_HPP
#define PULSAR_RING_HPP 1

// Include: Pulsar
#include "pulsar/concurrency.hpp"
#include "pulsar/memory.hpp"
#include "pulsar/utility.hpp"

// Pulsar
namespace pul
{

	/// ALLOCATOR: Ring Buffer -> MAMD
	class allocator_mamd_ring_buffer
	{
	/// Type -> Header
	struct __header_t
	{
		int32_t marked : 2;
		int32_t next : sizeof(int32_t) - 2;
	};

	/// Type -> Buffer
	struct __buffer_t
	{
		/// Constructor
		__buffer_t() pf_attr_noexcept
			: head(nullptr)
			, tail(nullptr)
		{}
		__buffer_t(__buffer_t const&) = delete;
		__buffer_t(__buffer_t &&)			= delete;

		/// Destructor
		~__buffer_t() pf_attr_noexcept = default;

		/// Operator =
		__buffer_t &operator=(__buffer_t const &) = delete;
		__buffer_t &operator=(__buffer_t &&)			= delete;

		/// Allocate
		pf_hint_nodiscard byte_t*
		__realign_allocation(
			size_t __seqsize,
			byte_t *__p,
			size_t __size,
			align_val_t __align,
			size_t __offset) pf_attr_noexcept
		{
			__p = union_cast<byte_t*>(align_top(__p, __align, sizeof(__header_t) + __offset));
			if (__p + __size > &this->seq[0] + __seqsize)
			{
				__p = union_cast<byte_t*>(align_top(&this->seq[0], __align, sizeof(__header_t) + __offset));
			}
			return __p;
		}
		pf_hint_nodiscard void*
		__allocate(
			size_t __seqsize,
			size_t __size,
			align_val_t __align,
			size_t __offset) pf_attr_noexcept
		{
			__size += sizeof(__header_t);

			union
			{
				__header_t *as_header;
				byte_t *as_byte;
			};

			// Update head if necessary
			// __header_t *h = this->head.load(atomic_order::relaxed);
			// if (this->head.compare_exchange_weak(h, k, atomic_order::relaxed, atomic_order::relaxed)) h = k;

			// Allocate
			as_header = this->tail;
			if (pf_unlikely(!this->head))
			{
				// Allocate
				as_byte						= this->__realign_allocation(__seqsize, as_byte, __size, __align, __offset);
				as_header->marked = 0;
				as_header->next		= __size;

				// Store
				this->tail = as_header;
				// this->head.store(as_header, atomic_order::relaxed);
				this->head = as_header;
			}
			else
			{
				// Allocate
				as_byte += as_header->next;
				as_byte	 = this->__realign_allocation(__seqsize, as_byte, __size, __align, __offset);

				// Check if good allocation
				byte_t *nb = as_byte + __size;
				byte_t *hb = union_cast<byte_t*>(this->head);
				byte_t *tb = union_cast<byte_t*>(this->tail);
				bool bad	 = (this->head > this->tail && nb > hb) || (nb > hb && as_byte <= tb);
				if (pf_unlikely(bad))
				{
					size_t reclaimed = 0;
					while (as_header != this->tail && as_header->marked)
					{
						const size_t n = as_header->next;
						as_byte += n;
						if (n > 0) reclaimed += n;
					}
					if(this->head == this->tail)
					{
						// Store
						this->tail = as_header;
						// this->head.store(as_header, atomic_order::relaxed);
						this->head = as_header;
					}
					else if (pf_unlikely(reclaimed < __size))
					{
						return nullptr;
					}
				}

				// Construct
				as_header->marked = 0;
				as_header->next		= __size;

				// Store
				this->tail->next = diffof(this->tail, as_byte);
				this->tail			 = as_header;
			}
			return (++as_header);
		}

		/// Deallocate
		pf_decl_static void
		__deallocate(
			void *__p) pf_attr_noexcept
		{
			union
			{
				__header_t *as_header;
				byte_t *as_byte;
				void *as_void;
			};
			as_void = __p;
			--as_header;
			as_header->marked = 1;
		}

		/// Store
		__header_t *head;
		__header_t *tail;
		pf_alignas(CCY_ALIGN) byte_t seq[];
	};

	/// Buffer -> New
	void
	__new_buffer(
		size_t __seqsize) pf_attr_noexcept
	{
		this->buffer_ = union_cast<byte_t*>(halloc((sizeof(__buffer_t) + __seqsize) * CCY_NUM_THREADS, align_val_t(64), sizeof(__buffer_t)));
		for (size_t i = 0; i < CCY_NUM_THREADS; ++i)
		{
			construct(this->__get_buffer(i));
		}
	}

	/// Buffer -> Delete
	void
	__delete_buffer(
		byte_t *__buffer) pf_attr_noexcept
	{
		union
		{
			byte_t *as_byte;
			__buffer_t *as_buffer;
		};
		as_byte = __buffer;
		for (size_t i = 0; i < CCY_NUM_THREADS; ++i)
		{
			destroy(this->__get_buffer(i));
		}
		hfree(this->buffer_);
	}

	/// Buffer -> Get
	pf_hint_nodiscard pf_decl_inline __buffer_t*
	__get_buffer(
		size_t __index) pf_attr_noexcept
	{
		return union_cast<__buffer_t*>(this->buffer_ + (sizeof(__buffer_t) + this->seqsize_) * __index);
	}

	public:
		/// Constructors
		allocator_mamd_ring_buffer(
			size_t __seqsize) pf_attr_noexcept
			: seqsize_(__seqsize)
		{
			this->__new_buffer(__seqsize);
		}
		allocator_mamd_ring_buffer(
			allocator_mamd_ring_buffer const &__r)
			: seqsize_(__r.seqsize_)
		{
			this->__new_buffer(__r.seqsize_);
		}
		allocator_mamd_ring_buffer(
			allocator_mamd_ring_buffer &&__r)
			: seqsize_(__r.seqsize_)
		{
			this->buffer_ = __r.buffer_;
			__r.__new_buffer(__r.seqsize_);
		}

		/// Destructor
		~allocator_mamd_ring_buffer() pf_attr_noexcept
		{
			this->__delete_buffer(this->buffer_);
		}

		/// Operator =
		allocator_mamd_ring_buffer &operator=(
			allocator_mamd_ring_buffer const &__r)
		{
			if (pf_likely(this != &__r))
			{
				this->__delete_buffer(this->buffer_);
				this->seqsize_ = __r.seqsize_;
				this->__new_buffer(__r.seqsize_);
			}
			return *this;
		}
		allocator_mamd_ring_buffer &operator=(
			allocator_mamd_ring_buffer &&__r)
		{
			if (pf_likely(this != &__r))
			{
				this->__delete_buffer(this->buffer_);
				this->seqsize_ = __r.seqsize_;
				this->buffer_	 = __r.buffer_;
				__r.__new_buffer(__r.seqsize_);
			}
			return *this;
		}

		/// Allocate
		pf_hint_nodiscard void*
		allocate(
			size_t __size,
			align_val_t __align = ALIGN_DEFAULT,
			size_t __offset			= 0) pf_attr_noexcept
		{
			return this->__get_buffer(this_thread::get_id())->__allocate(this->seqsize_, __size, __align, __offset);
		}

		/// Deallocate
		void deallocate(
			void *__buffer) pf_attr_noexcept
		{
			__buffer_t::__deallocate(__buffer);
		}

	private:
		size_t seqsize_;
		byte_t *buffer_;
	};
}

#endif // !PULSAR_RING_HPP