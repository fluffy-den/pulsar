/*! @file   internal_allocator.hpp
 *  @author Louis-Quentin Noé (noe.louis-quentin@hotmail.fr)
 *  @brief
 *  @date   28-06-2023
 *
 *  @copyright Copyright (c) 2023 - Pulsar Software
 *
 *  @since 0.1.6
 */

#ifndef PULSAR_INTERNAL_ALLOCATORS_HPP
#define PULSAR_INTERNAL_ALLOCATORS_HPP 1

// Include: Pulsar
#include "pulsar/magnifier.hpp"
#include "pulsar/memory.hpp"
#include "pulsar/algorithm.hpp"

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
			pf_hint_nodiscard pf_decl_always_inline byte_t *
			__realign_allocation(
			 size_t __seqsize,
			 byte_t *__p,
			 size_t __size,
			 align_val_t __align,
			 size_t __offset) pf_attr_noexcept
			{
				__p = union_cast<byte_t *>(align_top(__p, __align, __offset));
				if(__p + __size > &this->seq[0] + __seqsize)
				{
					__p = union_cast<byte_t *>(align_top(&this->seq[0], __align, __offset));
				}
				return __p;
			}

			/// Constructors
			pf_decl_inline
			__buffer_t() pf_attr_noexcept
				: head(nullptr)
				, tail(union_cast<__header_t *>(&this->seq[0]))
			{}
			__buffer_t(__buffer_t const &) = delete;
			__buffer_t(__buffer_t &&)			 = delete;

			/// Destructor
			pf_decl_inline ~__buffer_t() pf_attr_noexcept = default;

			/// Operator =
			__buffer_t &
			operator=(__buffer_t const &) = delete;
			__buffer_t &
			operator=(__buffer_t &&) = delete;

			/// Allocate
			pf_hint_nodiscard void *
			__allocate(
			 size_t __seqsize,
			 size_t __size,
			 align_val_t __align,
			 size_t __offset) pf_attr_noexcept
			{
				pf_assert(__offset < __size, "__offset is greater or equal to __size!");
				__size	 += sizeof(__header_t);
				__offset += sizeof(__header_t);
				union
				{
					__header_t *as_header;
					byte_t *as_byte;
				};
				// Allocate
				as_header = this->tail;
				if(pf_unlikely(!this->head))	// !head = empty list
																			//  head = first to dealloc
				{
					// Allocate
					as_byte						= this->__realign_allocation(__seqsize, as_byte, __size, __align, __offset);
					as_header->marked = 0;
					as_header->next		= union_cast<int32_t>(__size);

					// Store
					this->head = as_header;
					this->tail = as_header;
				}
				else
				{
					// Allocate
					as_byte += as_header->next;
					as_byte	 = this->__realign_allocation(__seqsize, as_byte, __size, __align, __offset);

					// Check if good allocation
					byte_t *n = this->__realign_allocation(__seqsize, as_byte + __size, 0, __align, __offset);
					byte_t *h = union_cast<byte_t *>(this->head);
					byte_t *t = union_cast<byte_t *>(this->tail);

					// [o][o][t][o][o][o][h][o][o]
					// [o][o][h][o][o][o][t][o][o]
					if(pf_unlikely(h != t && (as_byte <= h && n > h)))
					{
						size_t reclaimed = 0;
						__header_t *lst	 = as_header;
						as_header				 = this->head;
						while(as_header != this->tail && as_header->marked)
						{
							const int32_t k = as_header->next;
							as_byte				 += k;
							reclaimed			 += (k >= 0 ? k : distof(as_header, this->seq + __seqsize));
						}
						this->head = as_header;
						if(this->head == this->tail)
						{
							// Store
							this->tail = as_header;
						}
						else if(pf_unlikely(reclaimed < __size))
						{
							return nullptr;
						}
						as_header = lst;
					}

					// Construct
					as_header->marked = 0;
					as_header->next		= union_cast<int32_t>(__size);

					// Store
					this->tail->next = union_cast<int32_t>(diffof(this->tail, as_byte));
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
		 size_t __seqsize)
		{
			this->buffer_ = union_cast<byte_t *>(halloc((sizeof(__buffer_t) - 1) * (CCY_NUM_WORKERS * __seqsize + __seqsize0), align_val_t(32), sizeof(__buffer_t)));
			pf_throw_if(
			 !this->buffer_,
			 dbg_category_generic(),
			 dbg_code::runtime_error,
			 dbg_flags::dump_with_handle_data,
			 "Failed to create new heap buffer!");
			for(size_t i = 0; i < CCY_NUM_THREADS; ++i)
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
			for(size_t i = 0; i < CCY_NUM_THREADS; ++i)
			{
				destroy(this->__get_buffer(i));
			}
			hfree(this->buffer_);
		}

		/// Buffer -> Get
		pf_hint_nodiscard pf_decl_inline __buffer_t *
		__get_buffer(
		 size_t __index) pf_attr_noexcept
		{
			if(__index == 0) return union_cast<__buffer_t *>(this->buffer_);
			return union_cast<__buffer_t *>(this->buffer_ + (sizeof(__buffer_t) + this->seqsize0_) + (sizeof(__buffer_t) + this->seqsize_) * (__index - 1));
		}

	public:
		/// Constructors
		pf_decl_inline
		allocator_mamd_ring_buffer(
		 size_t __seqsize0,
		 size_t __seqsize)
			: seqsize0_(__seqsize0)
			, seqsize_(__seqsize)
		{
			this->__new_buffer(__seqsize0, __seqsize);
		}
		allocator_mamd_ring_buffer(
		 allocator_mamd_ring_buffer const &__r) = delete;
		allocator_mamd_ring_buffer(
		 allocator_mamd_ring_buffer &&__r) = delete;

		/// Destructor
		~allocator_mamd_ring_buffer() pf_attr_noexcept
		{
			this->__delete_buffer(this->buffer_);
		}

		/// Operator =
		allocator_mamd_ring_buffer &
		operator=(
		 allocator_mamd_ring_buffer const &__r) = delete;
		allocator_mamd_ring_buffer &
		operator=(
		 allocator_mamd_ring_buffer &&__r) = delete;

		/// Allocate
		pf_hint_nodiscard pf_decl_inline void *
		allocate(
		 size_t __size,
		 align_val_t __align = ALIGN_DEFAULT,
		 size_t __offset		 = 0) pf_attr_noexcept
		{
			thread_id_t ID = this_thread::get_idx();
			return this->__get_buffer(ID)->__allocate(ID == 0 ? this->seqsize0_ : this->seqsize_, __size, __align, __offset);
		}

		/// Deallocate
		pf_decl_inline void
		deallocate(
		 void *__buffer) pf_attr_noexcept
		{
			__buffer_t::__deallocate(__buffer);
		}

	private:
		/// Data
		size_t seqsize0_;
		size_t seqsize_;
		byte_t *buffer_;
	};


	/// ALLOCATOR: Stack Buffer -> MAMD
	template<typename _Magnifier = magnifier_default>
	class allocator_mamd_stack_buffer
	{
		struct __header_t;
		struct __controller_t;
		struct __buffer_t;

		/// Type -> Header
		struct __header_t
		{
			uint32_t marked : 1;
			uint32_t prev		: 31;
			uint32_t offset;
		};

		/// Type -> Buffer
		struct __buffer_t
		{
			/// Constructors
			pf_decl_inline
			__buffer_t(
			 size_t __size,
			 __controller_t *__ctrl) pf_attr_noexcept
				: size(__size)
				, ctrl(__ctrl)
				, next(nullptr)
				, tail(union_cast<__header_t *>(&this->seq[0]))
				, last(tail)
			{}
			__buffer_t(__buffer_t const &) = delete;
			__buffer_t(__buffer_t &&)			 = delete;

			/// Destructor
			pf_decl_inline ~__buffer_t() pf_attr_noexcept = default;

			/// Operator =
			__buffer_t &
			operator=(__buffer_t const &) = delete;
			__buffer_t &
			operator=(__buffer_t &&) = delete;

			/// Process Marks
			pf_decl_inline void
			__process_marks() pf_attr_noexcept
			{
				union
				{
					byte_t *as_byte;
					__header_t *as_header;
				};
				while(this->last->marked)
				{
					this->tail = this->last;
					as_header	 = this->last;
					as_byte		-= this->last->prev;
					this->last = as_header;
				}
			}

			/// Allocate
			pf_hint_nodiscard void *
			__allocate(
			 size_t __size,
			 align_val_t __align,
			 size_t __offset) pf_attr_noexcept
			{
				__size += sizeof(__header_t);
				union
				{
					byte_t *as_byte;
					__header_t *as_header;
				};
				this->__process_marks();
				as_byte				= union_cast<byte_t *>(align_top(this->tail, __align, sizeof(__header_t) + __offset));
				__header_t *p = as_header;
				as_byte			 += __size;
				if(pf_unlikely(as_byte > &this->seq[0] + this->size)) return nullptr;
				p->marked	 = 0;
				p->prev		 = union_cast<uint32_t>(distof(this->last, p));
				p->offset	 = union_cast<uint32_t>(distof(this, p));
				this->tail = as_header;
				this->last = p;
				return ++p;
			}

			/// Deallocate
			bool
			__deallocate(
			 thread_id_t __ID,
			 __header_t *__header) pf_attr_noexcept
			{
				if(pf_unlikely(__ID != this_thread::get_idx()))
				{
					__header->marked = 1;
					return false;
				}
				else
				{
					this->__process_marks();
					union
					{
						byte_t *as_byte;
						__header_t *as_header;
					};
					as_header = __header;
					if(pf_likely(as_header == this->last))
					{
						this->tail = this->last;
						as_byte		-= this->last->prev;
						this->last = as_header;
						return this->last == union_cast<__header_t *>(&seq[0]);
					}
					else
					{
						__header_t *prev = union_cast<__header_t *>(as_byte - as_header->prev);
						as_header->prev += prev->prev;
						return false;
					}
				}
			}

			/// Data
			const size_t size;
			__controller_t *ctrl;
			__buffer_t *next;
			__header_t *tail;
			__header_t *last;
			byte_t seq[];
		};

		/// Type -> Controllers
		struct __controller_t	 // NOTE: May never grow
		{
			/// Buffer -> New
			pf_hint_nodiscard pf_decl_inline __buffer_t *
			__new_buffer(
			 size_t __size) pf_attr_noexcept
			{
				return new_construct_ex<__buffer_t>(__size, __size, this);
			}

			/// Buffer -> Delete
			pf_decl_inline void
			__delete_buffer(
			 __buffer_t *__buf) pf_attr_noexcept
			{
				destroy_delete(__buf);
			}

			/// Header Of
			pf_hint_nodiscard pf_decl_inline __header_t *
			__header_of(
			 void *__ptr) pf_attr_noexcept
			{
				union
				{
					void *as_void;
					__header_t *as_header;
				};
				as_void = __ptr;
				--as_header;
				return as_header;
			}

			/// Buffer Of
			pf_hint_nodiscard pf_decl_inline __buffer_t *
			__buffer_of(
			 __header_t *__h) pf_attr_noexcept
			{
				union
				{
					__buffer_t *as_buffer;
					__header_t *as_header;
					byte_t *as_byte;
				};
				as_header = __h;
				as_byte	 -= as_header->offset;
				return as_buffer;
			}

			/// Constructors
			pf_decl_inline
			__controller_t(
			 thread_id_t __ID,
			 size_t __start) pf_attr_noexcept
				: ID(__ID)
				, head(this->__new_buffer(__start))
				, tail(this->head)
			{}
			__controller_t(__controller_t const &) = delete;
			__controller_t(__controller_t &&)			 = delete;

			/// Destructor
			pf_decl_inline ~__controller_t() pf_attr_noexcept
			{
				while(this->head)
				{
					__buffer_t *next = this->head->next;
					this->__delete_buffer(this->head);
					this->head = next;
				}
			}

			/// Operator =
			__controller_t &
			operator=(__controller_t const &) = delete;
			__controller_t &
			operator=(__controller_t &&) = delete;

			/// Allocate
			pf_hint_nodiscard void *
			__allocate(
			 _Magnifier const &__magnifier,
			 size_t __size,
			 align_val_t __align,
			 size_t __offset) pf_attr_noexcept
			{
				void *p = this->tail->__allocate(__size, __align, __offset);
				if(pf_unlikely(!p))
				{
					const size_t ns	 = __size > this->tail->size ? __magnifier(__size) : __magnifier(this->tail->size);
					this->tail->next = this->__new_buffer(__magnifier(ns));
					this->tail			 = this->tail->next;
					p								 = this->tail->__allocate(__size, __align, __offset);
				}
				return p;
			}

			/// Deallocate
			void
			__deallocate(
			 void *__ptr) pf_attr_noexcept
			{
				__header_t *h = this->__header_of(__ptr);
				__buffer_t *b = this->__buffer_of(h);
				b->__deallocate(this->ID, h);
			}
			pf_hint_nodiscard size_t
			__deallocate_and_purge(
			 size_t __start,
			 void *__ptr) pf_attr_noexcept
			{
				__header_t *h = this->__header_of(__ptr);
				__buffer_t *b = this->__buffer_of(h);
				if(pf_unlikely(b->__deallocate(this->ID, h)))
				{
					if(b == this->head)
					{
						if(this->head == this->tail)
						{
							if(this->head->size != __start && this->head->last == union_cast<__header_t *>(&this->head->seq[0]))
							{
								const size_t diff = union_cast<size_t>(this->head->size) - __start;
								this->head				= union_cast<__buffer_t *>(hrealloc(this->head, sizeof(__buffer_t) + __start, ALIGN_DEFAULT, 0));
								this->tail				= this->head;
								construct(this->head, __start, this);
								return diff;
							}
						}
						else
						{
							const size_t diff = union_cast<size_t>(this->head->size);
							__buffer_t *next	= this->head->next;
							this->__delete_buffer(this->head);
							this->head = next;
							return diff;
						}
					}
					else
					{
						__buffer_t *parent = this->head;
						while(parent->next != b)
						{
							parent = parent->next;
						}
						__buffer_t *next	= b->next;
						const size_t diff = union_cast<size_t>(b->size);
						this->__delete_buffer(b);
						parent->next = next;
						return diff;
					}
				}
				return 0;
			}

			/// Purge
			pf_hint_nodiscard size_t
			__purge(
			 size_t __start) pf_attr_noexcept
			{
				size_t diff = 0;
				if(this->head != this->tail)
				{
					while(this->head != this->tail && this->head->last == &this->head->seq[0])
					{
						diff						+= this->head->size;
						__buffer_t *next = this->head->next;
						this->__delete_buffer(this->head);
						this->head = next;
					}
				}
				if(this->head == this->tail && this->head.size != __start && this->head->last == &this->head->seq[0])
				{
					diff			+= this->head.size + __start;
					this->head = union_cast<__buffer_t *>(hrealloc(this->head, sizeof(__buffer_t) + __start, ALIGN_DEFAULT, 0));
					this->tail = this->head;
				}
				return diff;
			}

			/// Data
			thread_id_t ID;
			__buffer_t *head;
			__buffer_t *tail;
		};

	public:
		/// Constructors
		pf_decl_inline
		allocator_mamd_stack_buffer(
		 size_t __start0,
		 size_t __start,
		 _Magnifier &&__magnifier) pf_attr_noexcept
			: start0_(__start0)
			, start_(__start)
			, magnifier_(std::move(__magnifier))
		{
			this->ctrls_ = union_cast<__controller_t *>(halloc(sizeof(__controller_t) * CCY_NUM_THREADS, ALIGN_DEFAULT, 0));
			construct(&this->ctrls_[0], 0, this->start0_);
			for(uint32_t i = 1; i < CCY_NUM_THREADS; ++i)
			{
				construct(&this->ctrls_[i], i, this->start_);
			}
		}
		allocator_mamd_stack_buffer(allocator_mamd_stack_buffer const &) = delete;
		allocator_mamd_stack_buffer(allocator_mamd_stack_buffer &&)			 = delete;

		/// Destructor
		pf_decl_inline ~allocator_mamd_stack_buffer() pf_attr_noexcept
		{
			for(uint32_t i = 0; i < CCY_NUM_THREADS; ++i)
			{
				destroy(&this->ctrls_[i]);
			}
			hfree(this->ctrls_);
		}

		/// Operator =
		allocator_mamd_stack_buffer &
		operator=(allocator_mamd_stack_buffer const &) = delete;
		allocator_mamd_stack_buffer &
		operator=(allocator_mamd_stack_buffer &&) = delete;

		/// Allocate
		pf_hint_nodiscard pf_decl_inline void *
		allocate(
		 size_t __size,
		 align_val_t __align = ALIGN_DEFAULT,
		 size_t __offset		 = 0) pf_attr_noexcept
		{
			return this->ctrls_[this_thread::get_idx()].__allocate(this->magnifier_, __size, __align, __offset);
		}

		/// Deallocate
		pf_decl_inline void
		deallocate(
		 void *__p) pf_attr_noexcept
		{
			union
			{
				void *as_void;
				byte_t *as_byte;
				__header_t *as_header;
				__buffer_t *as_buffer;
			};
			as_void = __p;
			--as_header;
			as_byte -= as_header->offset;
			return as_buffer->ctrl->__deallocate(__p);
		}
		pf_decl_inline size_t
		deallocate_and_purge(
		 void *__p) pf_attr_noexcept
		{
			union
			{
				void *as_void;
				byte_t *as_byte;
				__header_t *as_header;
				__buffer_t *as_buffer;
			};
			as_void = __p;
			--as_header;
			as_byte -= as_header->offset;
			return as_buffer->ctrl->__deallocate_and_purge(this_thread::get_idx() == 0 ? this->start0_ : this->start_, __p);
		}

		/// Purge
		pf_decl_inline size_t
		purge() pf_attr_noexcept
		{
			const auto ID = this_thread::get_idx();
			return this->ctrl_[ID].__purge(ID == 0 ? this->start0_ : this->start_);
		}

	private:
		/// Data
		__controller_t *ctrls_;
		align_val_t align_;
		const size_t start0_;
		const size_t start_;
		pf_hint_nounique_address _Magnifier magnifier_;
	};


}	 // namespace pul

#endif	// !PULSAR_INTERNAL_ALLOCATORS_HPP
