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
	// NOTE: SAMD -> Single Allocation Multiple Deallocation
	struct __samd_ring_buffer_header_t;
	template <typename _Magnifier>
	struct __samd_ring_buffer;
	template <typename _Magnifier>
	class samd_ring_allocator;

	/// ALLOCATOR: SAMD Ring Header
	struct __samd_ring_buffer_header_t
	{
		/// Constructor
		__samd_ring_buffer_header_t(
			int32_t __offset,
			int32_t __next) pf_attr_noexcept
			: ismarked(false)
			, offset(__offset)
			, next(__next)
		{}
		__samd_ring_buffer_header_t(__samd_ring_buffer_header_t const &) = delete;
		__samd_ring_buffer_header_t(__samd_ring_buffer_header_t &&)			 = delete;

		/// Destructor
		~__samd_ring_buffer_header_t() pf_attr_noexcept = default;

		/// Operator =
		__samd_ring_buffer_header_t &operator=(__samd_ring_buffer_header_t const&) = delete;
		__samd_ring_buffer_header_t &operator=(__samd_ring_buffer_header_t &&)		 = delete;

		/// Store
		pf_alignas(8) atomic_flag_t ismarked;
		int32_t offset;
		int32_t next;
		byte_t store[];
	};

	/// ALLOCATOR: SAMD Ring Buffer
	template <typename _Magnifier>
	struct __samd_ring_buffer
	{
		/// Constructors
		__samd_ring_buffer(
			const samd_ring_allocator<_Magnifier> *__allocator,
			size_t __size) pf_attr_noexcept
			: allocator(__allocator)
			, size(__size)
			, next(nullptr)
			, head(union_cast<__samd_ring_buffer_header_t*>(&this->store[0]))
			, tail(head)
			, count(0)
			, ismarked(false)
		{}
		__samd_ring_buffer(__samd_ring_buffer<_Magnifier> const &) = delete;
		__samd_ring_buffer(__samd_ring_buffer<_Magnifier> &&)			 = delete;

		/// Operator =
		__samd_ring_buffer<_Magnifier> &operator=(__samd_ring_buffer<_Magnifier> const &) = delete;
		__samd_ring_buffer<_Magnifier> &operator=(__samd_ring_buffer<_Magnifier> &&)			= delete;

		/// Destructor
		~__samd_ring_buffer() pf_attr_noexcept = default;

		/// Allocate
		pf_hint_nodiscard void*
		__allocate(
			size_t __size,
			align_val_t __align,
			size_t __offset) pf_attr_noexcept
		{
			// Size
			__size += sizeof(__samd_ring_buffer_header_t);
			if (pf_unlikely(__size > this->size)) return nullptr;

			// Dealloc marked
			this->__process_dealloc();

			// Caster
			union
			{
				byte_t *as_byte;
				__samd_ring_buffer_header_t *as_header;
			};

			// Empty
			if (pf_unlikely(this->__empty()))
			{
				// Validity
				as_header = this->tail;
				as_byte		= union_cast<byte_t*>(align_top(as_byte, __align, sizeof(__samd_ring_buffer_header_t) + __offset));
				if (pf_unlikely(as_byte + __size > &this->store[0] + this->size))
				{
					as_byte = &this->store[0];
					as_byte = union_cast<byte_t*>(align_top(as_byte, __align, sizeof(__samd_ring_buffer_header_t) + __offset));
					if (pf_unlikely(as_byte + __size > union_cast<byte_t*>(this->head)))
						return nullptr;
				}

				// Allocate
				this->head = as_header;
				this->tail = as_header;
				construct(as_header, diffof(as_header, this), union_cast<int32_t>(__size));
				++this->count;
				return (as_header + 1);
			}
			else
			{
				// Validity
				byte_t *t = union_cast<byte_t*>(this->tail);
				as_byte = t + this->tail->next;
				byte_t *h = union_cast<byte_t*>(this->head);
				if (pf_unlikely(h > t && as_byte + __size > h))
					return nullptr;
				as_byte = union_cast<byte_t*>(align_top(as_byte, __align, sizeof(__samd_ring_buffer_header_t) + __offset));
				if (pf_unlikely(as_byte + __size > &this->store[0] + this->size))
				{
					as_byte = &this->store[0];
					as_byte = union_cast<byte_t*>(align_top(as_byte, __align, sizeof(__samd_ring_buffer_header_t) + __offset));
					if (pf_unlikely(as_byte + __size > h))
						return nullptr;
				}

				// Allocate
				this->tail->next = diffof(t, as_header);
				construct(as_header, diffof(as_header, this), union_cast<int32_t>(__size));
				this->tail = as_header;
				++this->count;
				return (as_header + 1);
			}
		}

		/// Deallocate
		bool
		__deallocate(
			void *__ptr) pf_attr_noexcept
		{
			// Caster
			union
			{
				void *as_void;
				byte_t *as_byte;
				__samd_ring_buffer_header_t *as_header;
			};
			as_void = __ptr;
			--as_header;

			// Owner?
			if (this->__is_owner_thread())
			{
				if (as_header == this->head)
				{
					as_byte		 = as_byte + as_header->next;
					this->head = as_header;
					--this->count;
				}
				else
				{
					as_header->ismarked.test_and_set(atomic_order::relaxed);
				}
				this->__process_dealloc();
				return this->__empty();
			}
			else
			{
				as_header->ismarked.test_and_set(atomic_order::relaxed);
				return false;
			}
		}

		/// __empty
		pf_hint_nodiscard pf_decl_inline bool
		__empty() const pf_attr_noexcept
		{
			return this->count == 0;
		}

		/// __is_owner_thread
		pf_hint_nodiscard pf_decl_inline bool
		__is_owner_thread() const pf_attr_noexcept
		{
			return (this->allocator->ownerID_ == this_thread::id);
		}

		/// __process_dealloc
		void
		__process_dealloc() pf_attr_noexcept
		{
			union
			{
				byte_t *as_byte;
				__samd_ring_buffer_header_t *as_header;
			};
			as_header = this->head;
			while (this->count > 0 && as_header->ismarked.test(atomic_order::relaxed))
			{
				--this->count;
				as_byte += as_header->next;
			}
			this->head = as_header;
		}

		/// Store
		const samd_ring_allocator<_Magnifier> *allocator;
		const size_t size;
		__samd_ring_buffer<_Magnifier> *next;
		__samd_ring_buffer_header_t *head;
		__samd_ring_buffer_header_t *tail;
		size_t count;
		pf_alignas(8) bool ismarked;
		byte_t store[];
	};

	/// ALLOCATOR: SAMD Ring Allocator
	template <typename _Magnifier>
	class samd_ring_allocator
	{
	pf_decl_friend __samd_ring_buffer<_Magnifier>;

	/// __new_buffer
	pf_hint_nodiscard pf_decl_inline __samd_ring_buffer<_Magnifier>*
	__new_buffer(
		size_t __size)
	{
		return new_construct_aligned_at_ex<__samd_ring_buffer<_Magnifier>>(
			sizeof(__samd_ring_buffer<_Magnifier>) + __size,
			align_val_t(64),
			sizeof(__samd_ring_buffer<_Magnifier>),
			this,
			__size);
	}

	/// __delete_buffer
	pf_decl_inline void
	__delete_buffer(
		__samd_ring_buffer<_Magnifier> *__ptr)
	{
		destroy_delete(__ptr);
	}

	/// __purge_grow
	void
	__purge_grow()
	{
		if (this->head_ == this->tail_
				&& this->startsize_ != 0
				&& this->startsize_ != this->tail_->size)
		{
			__samd_ring_buffer<_Magnifier> *buf = __new_buffer(this->startsize_);
			this->tail_->next = buf;
			this->tail_				= buf;
		}
	}

	public:
		/// Constructors
		samd_ring_allocator(
			size_t __wantedstart,
			_Magnifier &&__magnifier = _Magnifier())
			: head_(nullptr)
			, tail_(nullptr)
			, ownerID_(this_thread::id)
			, startsize_(this->magnifier_(__wantedstart))
			, magnifier_(std::move(__magnifier))
		{
			if (startsize_ != 0)
			{
				this->head_ = this->__new_buffer(this->startsize_);
				this->tail_ = this->head_;
			}
		}

		/// Destructor
		~samd_ring_allocator() pf_attr_noexcept
		{
			while(this->head_)
			{
				__samd_ring_buffer<_Magnifier> *buf = this->head_->next;
				this->__delete_buffer(this->head_);
				this->head_ = buf;
			}
		}

		/// Operator =
		samd_ring_allocator<_Magnifier> &operator=(samd_ring_allocator<_Magnifier> const &) = delete;
		samd_ring_allocator<_Magnifier> &operator=(samd_ring_allocator<_Magnifier> &&)			= delete;

		/// Allocate
		pf_hint_nodiscard void*
		allocate(
			size_t __size,
			align_val_t __align = ALIGN_DEFAULT,
			size_t __offset			= 0)
		{
			// Owner
			pf_assert(this_thread::id == this->ownerID_, "Only owning thread can allocate!");

			// Direct
			void *alloc = this->tail_->__allocate(__size, __align, __offset);

			// Needs a grow!
			if (pf_unlikely(!alloc))
			{
				__samd_ring_buffer<_Magnifier> *buf = __new_buffer(__size > this->tail_->size ? __size : this->tail_->size + 1);
				this->tail_->next = buf;
				this->tail_				= buf;
				alloc							= this->tail_->__allocate(__size, __align, __offset);
			}
			else
			{
				return alloc;	// NOTE: Balancing load (max value)
			}

			// Check for empty marked buffers
			if (pf_unlikely(this->head_ != this->tail_))
			{
				do
				{
					this->head_->__process_dealloc();
					if (!this->head_->__empty()) break;
					__samd_ring_buffer<_Magnifier> *buf = this->head_->next;
					this->__delete_buffer(this->head_);
					this->head_ = buf;
				} while (this->head_ != this->tail_);
				// Empty
				this->__purge_grow();
			}

			// Handle
			return alloc;
		}

		/// Deallocate
		void
		deallocate(
			void *__ptr)
		{
			union
			{
				void *as_void;
				__samd_ring_buffer_header_t *as_header;
				__samd_ring_buffer<_Magnifier> *as_buffer;
				byte_t *as_byte;
			};
			as_void = __ptr;
			--as_header;
			as_byte += as_header->offset;

			// Is owner?
			if (pf_unlikely(as_buffer->__deallocate(__ptr) && this->head_ != this->tail_))	// NOTE: True only for owning thread!
			{
				if (as_buffer == this->head_)
				{
					__samd_ring_buffer<_Magnifier> *buf = this->head_->next;
					this->__delete_buffer(this->head_);
					this->head_ = buf;
					// Delete marked buffers
					while(this->head_ && this->head_->ismarked)
					{
						__samd_ring_buffer<_Magnifier> *k = this->head_->next;
						this->__delete_buffer(this->head_);
						this->head_ = k;
					}
					// One?
					this->__purge_grow();
				}
				else
				{
					as_buffer->ismarked = true;
				}
			}
		}

	private:
		__samd_ring_buffer<_Magnifier> *head_;
		__samd_ring_buffer<_Magnifier> *tail_;
		thread_id_t ownerID_;
		const size_t startsize_;
		pf_hint_nounique_address _Magnifier magnifier_;
	};
}

#endif // !PULSAR_SAMD_RING_HPP