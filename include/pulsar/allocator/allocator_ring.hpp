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
		uint32_t marked;
		int32_t next;
	};

	/// Type -> Buffer
	struct __buffer_t
	{
		/// Constructor
		__buffer_t() pf_attr_noexcept
			: head(nullptr)
			, tail(union_cast<__header_t*>(&this->seq[0]))
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

			pf_assert(__size <= __seqsize, "__size is greater than __seqsize!");
			pf_assert(is_power_of_two(union_cast<size_t>(__align)), "__align isn't power of two!");

			union
			{
				__header_t *as_header;
				byte_t *as_byte;
			};
			// Allocate
			as_header = this->tail;
			if (pf_unlikely(!this->head))				// !head = empty list
																					//  head = first to dealloc
			{
				// Allocate
				as_byte						= this->__realign_allocation(__seqsize, as_byte, __size, __align, __offset);
				as_header->marked = 0;
				as_header->next		= __size;

				// Store
				this->tail = as_header;
				this->head = as_header;
			}
			else
			{
				// Allocate
				as_byte += as_header->next;
				as_byte	 = this->__realign_allocation(__seqsize, as_byte, __size, __align, __offset);

				// Check if good allocation
				byte_t *n = this->__realign_allocation(__seqsize, as_byte + __size, 0, __align, __offset);
				byte_t *h = union_cast<byte_t*>(this->head);
				byte_t *t = union_cast<byte_t*>(this->tail);

				// [o][o][t][o][o][o][h][o][o]
				// [o][o][h][o][o][o][t][o][o]
				if (pf_unlikely(h != t && (as_byte <= h && n > h)))
				{
					size_t reclaimed = 0;
					__header_t *lst	 = as_header;
					as_header = this->head;
					while (as_header != this->tail && as_header->marked)
					{
						const int32_t k = as_header->next;
						as_byte		+= k;
						reclaimed += (k >= 0 ? k : distof(as_header, this->seq + __seqsize));
					}
					this->head = as_header;
					if(this->head == this->tail)
					{
						// Store
						this->tail = as_header;
					}
					else if (pf_unlikely(reclaimed < __size))
					{
						return nullptr;
					}
					as_header = lst;
				}

				// Construct
				as_header->marked = 0;
				as_header->next		= union_cast<int32_t>(__size);

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
		pf_alignas(CCY_ALIGN) byte_t seq[1];
	};

	/// Buffer -> New
	void
	__new_buffer(
		size_t __seqsize0,
		size_t __seqsize) pf_attr_noexcept
	{
		this->buffer_ = union_cast<byte_t*>(halloc(sizeof(__buffer_t) * (CCY_NUM_WORKERS * __seqsize + __seqsize0), align_val_t(32), sizeof(__buffer_t)));
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
		if (__index == 0) return union_cast<__buffer_t*>(this->buffer_);
		return union_cast<__buffer_t*>(this->buffer_ + (sizeof(__buffer_t) + this->seqsize0_) + (sizeof(__buffer_t) + this->seqsize_) * (__index - 1));
	}

	public:
		/// Constructors
		allocator_mamd_ring_buffer(
			size_t __seqsize0,
			size_t __seqsize) pf_attr_noexcept
			: seqsize0_(__seqsize0)
			, seqsize_(__seqsize)
		{
			this->__new_buffer(__seqsize0, __seqsize);
		}
		allocator_mamd_ring_buffer(
			allocator_mamd_ring_buffer const &__r)
			: seqsize0_(__r.seqsize0_)
			, seqsize_(__r.seqsize_)
		{
			this->__new_buffer(__r.seqsize0_, __r.seqsize_);
		}
		allocator_mamd_ring_buffer(
			allocator_mamd_ring_buffer &&__r)
			: seqsize0_(__r.seqsize0_)
			, seqsize_(__r.seqsize_)
		{
			this->buffer_ = __r.buffer_;
			__r.__new_buffer(__r.seqsize0_, __r.seqsize_);
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
				this->seqsize0_ = __r.seqsize0_;
				this->seqsize_	= __r.seqsize_;
				this->__new_buffer(__r.seqsize0_, __r.seqsize_);
			}
			return *this;
		}
		allocator_mamd_ring_buffer &operator=(
			allocator_mamd_ring_buffer &&__r)
		{
			if (pf_likely(this != &__r))
			{
				this->__delete_buffer(this->buffer_);
				this->seqsize0_ = __r.seqsize0_;
				this->seqsize_	= __r.seqsize_;
				this->buffer_		= __r.buffer_;
				__r.__new_buffer(__r.seqsize0_, __r.seqsize_);
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
			thread_id_t ID = this_thread::get_id();
			void *p				 = this->__get_buffer(ID)->__allocate(ID == 0 ? this->seqsize0_ : this->seqsize_, __size, __align, __offset);
			pf_assert(p, "p is nullptr!");
			return p;
		}

		/// Deallocate
		void deallocate(
			void *__buffer) pf_attr_noexcept
		{
			__buffer_t::__deallocate(__buffer);
		}

	private:
		size_t seqsize0_;
		size_t seqsize_;
		byte_t *buffer_;
	};
}

#endif // !PULSAR_RING_HPP