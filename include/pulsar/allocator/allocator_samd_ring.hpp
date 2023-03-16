/*! @file   allocator_samd_ring.hpp
 *  @author Louis-Quentin Noé (noe.louis-quentin@hotmail.fr)
 *  @brief  Thread local lock-free ring buffer.
 *  @date   05-03-2023
 *
 *  @copyright Copyright (c) 2023 - Pulsar Software
 *
 *  @since 0.1.4
 */

#ifndef PULSAR_SAMD_RING_HPP
#define PULSAR_SAMD_RING_HPP 1

// Include: Pulsar
#include "pulsar/allocator.hpp"
#include "pulsar/concurrency.hpp"
#include "pulsar/debug.hpp"
#include "pulsar/memory.hpp"
#include "pulsar/utility.hpp"

// Pulsar
namespace pul
{

	class allocator_samd_ring_buffer
	{
	private:
		struct __header_t
		{
			diff_t marked : 2;
			diff_t next : sizeof(diff_t) - 2;
		};
		struct __buffer_t
		{
			/// Constructors
			__buffer_t(
				size_t __size) pf_attr_noexcept
				: head(nullptr)
				, tail(union_cast<__header_t*>(&this->seq[0]))
				, size(__size)
			{}
			__buffer_t(__buffer_t const &) = delete;
			__buffer_t(__buffer_t &&)			 = delete;

			/// Destructor
			~__buffer_t() pf_attr_noexcept
			{
				// Remove remaining
				// __header_t *h = this->head.load(atomic_order::relaxed);
				if (this->head)
				{
					union
					{
						__header_t *as_header;
						byte_t *as_byte;
					};
					as_header = this->head;
					while (as_header != this->tail && as_header->marked)
					{
						as_byte += as_header->next;
					}
					this->head = as_header;
					if(pf_likely(this->head == this->tail)) this->head = nullptr;
				}
				// this->head.store(h);

				// Problem
				pf_assert(
					!this->head,
					"Head isn't equal to tail! head={}, tail={}",
					union_cast<void*>(this->head),
					union_cast<void*>(this->tail));
			}

			/// Operator =
			__buffer_t &operator=(__buffer_t const &) = delete;
			__buffer_t &operator=(__buffer_t &&)			= delete;

			/// Allocate
			pf_hint_nodiscard byte_t*
			__realign_allocation(
				byte_t *__p,
				size_t __size,
				align_val_t __align,
				size_t __offset) pf_attr_noexcept
			{
				__p = union_cast<byte_t*>(align_top(__p, __align, sizeof(__header_t) + __offset));
				if (__p + __size > &this->seq[0] + this->size)
				{
					__p = union_cast<byte_t*>(align_top(&this->seq[0], __align, sizeof(__header_t) + __offset));
				}
				return __p;
			}
			pf_hint_nodiscard void*
			__allocate(
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
					as_byte						= this->__realign_allocation(as_byte, __size, __align, __offset);
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
					as_byte	 = this->__realign_allocation(as_byte, __size, __align, __offset);

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
			void
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
			const size_t size;
			byte_t seq[];
		};

		// Buffer -> Create
		pf_hint_nodiscard __buffer_t*
		__create_buffer(
			size_t __size)
		{
			return new_construct_ex<__buffer_t>(__size, __size);
		}

		// Buffer -> Delete
		void
		__remove_buffer(
			__buffer_t *__b) pf_attr_noexcept
		{
			destroy_delete(__b);
		}

	public:
		/// Constructors
		allocator_samd_ring_buffer(
			size_t __size)
			: buffer_(this->__create_buffer(__size))
		{}
		allocator_samd_ring_buffer(
			allocator_samd_ring_buffer const& __r)
			: buffer_(this->__create_buffer(__r.buffer_->size))
		{}
		allocator_samd_ring_buffer(
			allocator_samd_ring_buffer &&__r)
			: buffer_(__r.buffer_)
		{
			__r.buffer_ = this->__create_buffer(this->buffer_->size);
		}

		/// Operator =
		allocator_samd_ring_buffer &operator=(
			allocator_samd_ring_buffer const& __r) pf_attr_noexcept
		{
			if (pf_likely(this != &__r))
			{
				this->__remove_buffer(this->buffer_);
				this->buffer_ = this->__create_buffer(__r.buffer_->size);
			}
			return *this;
		}
		allocator_samd_ring_buffer &operator=(
			allocator_samd_ring_buffer && __r) pf_attr_noexcept
		{
			if (pf_likely(this != &__r))
			{
				this->buffer_ = __r.buffer_;
				__r.buffer_		= this->__create_buffer(__r.buffer_->size);
			}
			return *this;
		}

		/// Destructor
		~allocator_samd_ring_buffer() pf_attr_noexcept
		{
			this->__remove_buffer(this->buffer_);
		}

		/// Allocate
		pf_hint_nodiscard void*
		allocate(
			size_t __size,
			align_val_t __align = ALIGN_DEFAULT,
			size_t __offset			= 0) pf_attr_noexcept
		{
			return this->buffer_->__allocate(__size, __align, __offset);
		}

		/// Deallocate
		void
		deallocate(
			void *__ptr) pf_attr_noexcept
		{
			return this->buffer_->__deallocate(__ptr);
		}

		/// Store
		__buffer_t *buffer_;
	};
}

#endif // !PULSAR_SAMD_RING_HPP