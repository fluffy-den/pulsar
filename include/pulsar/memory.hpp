/*! @file   memory.hpp
 *  @author Fluffy (noe.louis-quentin@hotmail.fr)
 *  @brief	Defines the memory utilities such as memory functions, allocators and operators.
 *  @date   09-01-2022
 *
 *  @copyright Copyright (c) 2022 - Pulsar Software
 *
 *  @since 0.1.0
 */

#ifndef PULSAR_MEMORY_HPP
#define PULSAR_MEMORY_HPP 1

// Include: Pulsar
#include "pulsar/debug.hpp"
#include "pulsar/pulsar.hpp"
#include "pulsar/utility.hpp"

// Include: C++
#include <memory>
#include <type_traits>

// Pulsar
namespace pul
{

	// Memory
	namespace memory
	{

		/// MEMORY: Alignment
		pf_decl_constexpr align_val_t MAX_ALIGN = align_val_t(alignof(void *));

		/*! @brief Calculates the offset of a memory sequence starting at address @a __addr needed to
		 * 				 align the sequence to the value of @a __align.
		 *
		 *  @param[in] __addr	 Memory address of the beginning of the sequence.
		 *  @param[in] __align Desired alignment of the memory.
		 *
		 *  @return 0 if @a __align is equal to 0.
		 * 				  The offset required to align the memory sequence otherwise.
		 */
		pf_hint_nodiscard pf_decl_constexpr size_t padding_of(
				size_t __addr,
				align_val_t __align) pf_attr_noexcept
		{
			if (__align == align_val_t(0))
				return 0;
			return (static_cast<size_t>(__align) - (__addr & (static_cast<size_t>(__align) - 1))) & (static_cast<size_t>(__align) - 1);
		}
		/*! @brief Computes from the base @a __ptr a new pointer aligned to the value of @a __align by
		 *  			 an	offset of @a __offset.
		 *
		 *  @param[in] __ptr		Pointer to a memory sequence.
		 *  @param[in] __align	Desired alignment of the memory.
		 *  @param[in] __offset Head offset of the new pointer (optional).
		 *
		 *  @return Aligned pointer.
		 */
		pf_hint_nodiscard pf_decl_constexpr void *align_top(
				void *__ptr,
				align_val_t __align,
				size_t __offset = 0) pf_attr_noexcept
		{
			union
			{
				void *as_voidptr;
				size_t as_addr;
			};

			as_voidptr = __ptr;
			as_addr += padding_of(as_addr + __offset, __align);
			return as_voidptr;
		}
		/*! @brief Checks if a block of memory pointed to by @a __ptr whose head is aligned by
		 *				 @a __align by an @a __offset.
		 *
		 *  @param[in] __ptr		Pointer to a memory sequence.
		 *  @param[in] __align	Alignment of this memory sequence.
		 *  @param[in] __offset Offset of the alignment.
		 *  @return True if the block is aligned.
		 * 					False otherwise.
		 */
		pf_hint_nodiscard pf_decl_constexpr bool is_aligned(
				const void *__ptr,
				align_val_t __align,
				size_t __offset = 0) pf_attr_noexcept
		{
			union
			{
				const void *as_void;
				size_t as_addr;
			};

			as_void = __ptr;

			return padding_of(as_addr + __offset, __align) == 0;
		}

		/// MEMORY: Buffer
		/*! @brief Memory buffer.
		 */
		class buffer
		{
		public:
			/// Constructors
			/*! @brief Constructor. Does not build dynamic memory.
			 */
			pf_decl_constexpr buffer() pf_attr_noexcept
					: align_(std::align_val_t(0))
					, beg_(nullptr)
					, end_(nullptr)
			{}
			/*! @brief Constructor. Builds a memory of size @a __size and aligned to @a __bufalign.
			 *
			 *  @param[in] __size			Size of the memory to build.
			 *  @param[in] __bufalign Alignment of this memory.
			 */
			pf_decl_constexpr buffer(
					size_t __size,
					align_val_t __bufalign = MAX_ALIGN) pf_attr_noexcept
					: align_(std::align_val_t(__bufalign))
					, beg_(new (align_) byte_t[(__size += padding_of(__size, __bufalign))])
					, end_(this->beg_ + __size)
			{
				pf_assert(this->size() % __bufalign == 0,
									"Buffer size isn't aligned to __bufalign!");
				pf_assert(is_aligned(this->beg_, __bufalign),
									"Buffer starting address isn't aligned to __bufalign!");
				this->fill();
			}
			/*! @brief Copy constructor. Builds a memory of the same size as the other buffer @a __r.
			 *				 Doesn't copy memory, as types may uses their own operator= or copy constructors.
			 *
			 *  @param[in] __r				Other memory buffer to copy from.
			 */
			pf_decl_constexpr buffer(
					buffer const &__r) pf_attr_noexcept
					: buffer(__r.capacity(), __r.align_)
			{}
			/*! @brief Copy constructor. Builds a memory of the same size as the other buffer @a __r.
			 *				 Doesn't copy memory, as types may uses their own operator= or copy constructors.
			 *				 The newly created buffer is aligned to @a __bufalign.
			 *
			 *  @param[in] __r				Other memory buffer to copy from.
			 *  @param[in] __bufalign New alignment of this buffer.
			 */
			pf_decl_constexpr buffer(
					buffer const &__r,
					align_val_t __bufalign) pf_attr_noexcept
					: buffer(__r.capacity(), __bufalign)
			{}
			/*! @brief Move constructor. Moves a memory.
			 *
			 *  @param[in] __r Other memory buffer to move from.
			 */
			pf_decl_constexpr buffer(
					buffer &&__r) pf_attr_noexcept
					: align_(__r.align_)
					, beg_(__r.beg_)
					, end_(__r.end_)
			{
				__r.align_ = std::align_val_t(0);
				__r.beg_	 = nullptr;
				__r.end_	 = nullptr;
			}

			/// Destructor
			/*! @brief Destructor. Free the memory.
			 */
			pf_decl_constexpr ~buffer() pf_attr_noexcept
			{
				this->__free();
			}

			/// Operators=
			/*! @brief Copy assignment operator. Resize the memory at the same size as the other buffer
			 *				 @a __r. Doesn't copy memory, as types may uses their own operator= or copy
			 *				 constructors. Cleans owned memory before resizing. See @ref resize.
			 *
			 *  @param[in] __r Other memory buffer to copy from.
			 *  @return Reference on this memory buffer.
			 */
			pf_decl_constexpr buffer &operator=(
					buffer const &__r) pf_attr_noexcept
			{
				if (&__r == this) return *this;
				this->resize(__r.capacity(), MAX_ALIGN);
				return *this;
			}
			/*! @brief Move assignment operator. Moves a buffer memory. Cleans owned memory before
			 *				 resizing. See @ref resize.
			 *
			 *  @param[in] __r Other memory buffer to move from.
			 *  @return Reference on this memory buffer.
			 */
			pf_decl_constexpr buffer &operator=(
					buffer &&__r) pf_attr_noexcept
			{
				if (&__r == this) return *this;
				this->__free();
				this->align_ = __r.align_;
				this->beg_	 = __r.beg_;
				this->end_	 = __r.end_;
				__r.align_	 = std::align_val_t(0);
				__r.beg_		 = nullptr;
				__r.end_		 = nullptr;
				return *this;
			}

			/// Capacity
			/*! @brief Returns the maximum capacity of this buffer.
			 *
			 *  @return Capacity in bytes of this buffer.
			 */
			pf_decl_constexpr size_t capacity() const pf_attr_noexcept
			{
				return std::distance(this->beg_, this->end_);
			}

			/// Resize
			/*! @brief Resizes the memory buffer to a new size @a __newsize, aligned to @a __newalign,
			 *				 and fill the new memory to @a __val.
			 *
			 * 	@warning All the previous content are discarded (no internal copy!).
			 *
			 *  @param[in] __newsize	The new size of this buffer.
			 *  @param[in] __newalign	The new alignment of this buffer.
			 *  @param[in] __val			The filled value of the new memory.
			 *  @return Difference of size between the last memory size and the new one.
			 */
			pf_decl_constexpr diff_t resize(
					size_t __newsize,
					align_val_t __newalign = MAX_ALIGN,
					int32_t __val					 = '\0') pf_attr_noexcept
			{
				size_t s = this->capacity();
				if (__newsize == s)
					return 0;
				this->__free();
				if (__newsize == 0)
				{
					return -s;
				}
				this->align_ = std::align_val_t(__newalign);
				this->beg_	 = new (this->align_) byte_t[__newsize];
				this->end_	 = this->beg_ + __newsize;
				this->fill(__val);
				pf_assert(this->size() % __newalign == 0,
									"Buffer size isn't aligned to __newalign!");
				pf_assert(is_aligned(this->beg_, __newalign),
									"Buffer starting address isn't aligned to __newalign!");
				return static_cast<diff_t>(this->capacity()) - static_cast<diff_t>(s);
			}

			/// Alignment
			/*! @brief Returns the current alignment of this buffer.
			 *
			 *  @return Alignment in power of two of this buffer.
			 */
			pf_decl_constexpr align_val_t alignment() const pf_attr_noexcept
			{
				return this->align_;
			}

			/// Fill
			/*! @brief Fill the memory with @a __val.
			 *
			 *  @param[in] __val Value to fill with.
			 */
			pf_decl_constexpr void fill(
					int32_t __val = '\0') const pf_attr_noexcept
			{
				std::memset(this->beg_, __val, this->capacity());
			}

			/// Begin
			/*! @brief Returns the beginning of the memory sequence.
			 *
			 *  @return Pointer on the beginning of the memory sequence.
			 */
			pf_decl_constexpr byte_t *begin() pf_attr_noexcept
			{
				return this->beg_;
			}

			/// End
			/*! @brief Returns the end of the memory sequence.
			 *
			 *  @return Pointer on the end of the memory sequence.
			 */
			pf_decl_constexpr byte_t *end() pf_attr_noexcept
			{
				return this->end_;
			}

		private:
			/// Free
			/*! @brief Free the memory buffer.
			 */
			pf_decl_constexpr void __free() pf_attr_noexcept
			{
				if (this->beg_)
				{
					::operator delete[](this->beg_, this->align_);
					this->beg_ = nullptr;
					this->end_ = nullptr;
				}
			}

			std::align_val_t align_;
			byte_t *beg_;
			byte_t *end_;
		};

		/*! @brief
		 *
		 */
		class buffer_virtual
		{
		public:
			// TODO: Buffer Virtual

		private:
			byte_t *virtBeg_;
			byte_t *virtEnd_;
			byte_t *physBeg_;
			byte_t *physEnd_;
		};

		/// MEMORY: Allocators
		class allocator_linear;
		class allocator_stack;
		class allocator_pool;

		/// MEMORY: Allocator Linear
		/*! @brief Linear memory allocator.
		 */
		class allocator_linear
		{
			pf_decl_friend class allocator_stack;

		public:
			using propagate_on_container_copy_assignment = std::true_type;
			using propagate_on_container_move_assignment = std::false_type;
			using propagate_on_container_swap						 = std::true_type;

			/// Constructors
			/*! @brief Constructor.
			 *
			 *  @param[in] __size 		Size of the memory buffer.
			 *  @param[in] __bufalign Alignment of the memory buffer.
			 */
			pf_decl_constexpr allocator_linear(
					size_t __size,
					align_val_t __bufalign = MAX_ALIGN) pf_attr_noexcept
					: buf_(__size, __bufalign)
					, off_(this->buf_.begin())
			{}
			/*! @brief Copy constructor.
			 *
			 *  @param[in] __r Other linear allocator to copy from.
			 */
			pf_decl_constexpr allocator_linear(
					allocator_linear const &__r) pf_attr_noexcept
					: buf_(__r.buf_)
					, off_(__r.off_)
			{}
			/*! @brief Copy constructor.
			 *
			 *  @param[in] __r 				Other linear allocator to copy from.
			 *  @param[in] __bufalign Alignment of the buffer.
			 */
			pf_decl_constexpr allocator_linear(
					allocator_linear const &__r,
					align_val_t __bufalign) pf_attr_noexcept
					: buf_(__r.buf_, __bufalign)
					, off_(__r.off_)
			{}
			/*! @brief Move constructor.
			 *
			 *	@param[in] __r Other linear allocator to copy from.
			 */
			pf_decl_constexpr allocator_linear(
					allocator_linear &&__r) pf_attr_noexcept
					: buf_(std::move(__r.buf_))
					, off_(__r.off_)
			{
				__r.off_ = nullptr;
			}

			/// Operators=
			/*! @brief Copy assignment operator.
			 *
			 *  @param[in] __r Other linear allocator to copy from.
			 *  @return Reference on this allocator.
			 */
			pf_decl_constexpr allocator_linear &operator=(
					allocator_linear const &__r) pf_attr_noexcept
			{
				if (&__r == this) return *this;
				this->buf_ = __r.buf_;
				this->off_ = this->buf_.begin();
				return *this;
			}
			/*! @brief Move assignment operator.
			 *
			 *  @param[in] __r Other linear allocator to move from.
			 *  @return Reference on this allocator.
			 */
			pf_decl_constexpr allocator_linear &operator=(
					allocator_linear &&__r) pf_attr_noexcept
			{
				if (&__r == this) return *this;
				this->buf_ = __r.buf_;
				this->off_ = __r.off_;
				return *this;
			}

			/// Operator==
			/*! @brief Equality operator.
			 *
			 *  @param[in] __r Other Allocator to compare with.
			 *  @return False, can't be equal with any allocator (unique memory).
			 */
			template <typename _Allocator>
			pf_decl_constexpr bool operator==(
					_Allocator const &__r) pf_attr_noexcept
			{
				return false;
			}

			/// Allocate
			/*! @brief Allocates a memory of size @a __size, aligned to @a __align at the head of
			 *				 @a __offset.
			 *
			 *  @param[in] __size 	Size of the allocation.
			 *  @param[in] __align	Alignment of the allocation.
			 *  @param[in] __offset	Offset to alignment.
			 *  @return Pointer on a allocated memory.
			 */
			pf_hint_nodiscard pf_decl_constexpr void *allocate(
					size_t __size,
					align_val_t __align = MAX_ALIGN,
					size_t __offset			= 0) pf_attr_noexcept
			{
				__size += padding_of(__size + __offset, __align);
				union
				{
					byte_t *as_byte;
					size_t as_addr;
				};
				as_byte = this->off_;
				as_addr += padding_of(as_addr + __offset, __align);
				byte_t *e = as_byte + __size;
				if (e > this->buf_.end())
					return nullptr;
				this->off_ = e;
				return as_byte;
			}

			/// Deallocate
			/*! @brief Deallocates a memory pointed by @a __ptr.
			 *
			 *  @param[in] __ptr Pointer referring to a memory to be deallocated.
			 */
			pf_decl_constexpr void deallocate(
					void *__ptr) pf_attr_noexcept
			{
				std::ignore = __ptr;
			}

			/// Clear
			/*! @brief Resets the allocator to zero.
			 *
			 *  @warning Must not be used with types using this memory that use dynamic allocations
			 *  				 themselves!
			 */
			pf_decl_constexpr void clear() pf_attr_noexcept
			{
				this->buf_.fill();
				this->off_ = this->buf_.begin();
			}

		private:
			buffer buf_;
			byte_t *off_;
		};

		/// MEMORY: Allocator Stack
		/*! @brief Stack memory allocator.
		 */
		class allocator_stack: public allocator_linear
		{
			struct __alloc_h
			{
				byte_t *prev_;
			};

		public:
			using propagate_on_container_copy_assignment = std::true_type;
			using propagate_on_container_move_assignment = std::false_type;
			using propagate_on_container_swap						 = std::true_type;

			/// Constructors
			/*! @brief Constructor.
			 *
			 *  @param[in] __size		  Size of the memory buffer.
			 *  @param[in] __bufalign Alignment of the memory buffer.
			 */
			pf_decl_constexpr allocator_stack(
					size_t __size,
					align_val_t __bufalign = MAX_ALIGN) pf_attr_noexcept
					: allocator_linear(__size, __bufalign)
			{}
			/*! @brief Copy constructor.
			 *
			 *  @param[in] __r Other stack allocator to copy from.
			 */
			pf_decl_constexpr allocator_stack(
					allocator_stack const &__r) pf_attr_noexcept
					: allocator_linear(__r)
			{}
			/*! @brief Copy constructor.
			 *
			 *  @param[in] __r 				Other stack allocator to copy from.
			 * 	@param[in] __bufalign Alignment of the memory buffer.
			 */
			pf_decl_constexpr allocator_stack(
					allocator_stack const &__r,
					align_val_t __bufalign) pf_attr_noexcept
					: allocator_linear(__r, __bufalign)
			{}
			/*! @brief Move constructor
			 *
			 *  @param[in] __r Other stack allocator to move from.
			 */
			pf_decl_constexpr allocator_stack(
					allocator_stack &&__r) pf_attr_noexcept
					: allocator_linear(std::move(__r))
			{}

			/// Allocate
			/*! @brief Allocates a memory of size @a __size, aligned to @a __align at the head of
			 *				 @a __offset.
			 *
			 *  @param[in] __size 	Size of the allocation.
			 *  @param[in] __align	Alignment of the allocation.
			 *  @param[in] __offset	Offset to alignment.
			 *  @return Pointer on a allocated memory.
			 */
			pf_decl_constexpr void *allocate(
					size_t __size,
					align_val_t __align = MAX_ALIGN,
					size_t __offset			= 0) pf_attr_noexcept
			{
				__size += padding_of(__size + __offset, __align);
				union
				{
					byte_t *as_byte;
					__alloc_h *as_header;
					size_t as_addr;
				};
				as_byte		= this->off_;
				byte_t *l = as_byte;
				as_addr += padding_of(as_addr + __offset, __align);
				byte_t *e = as_byte + sizeof(__alloc_h) + __size;
				if (e > this->buf_.end())
					return nullptr;
				this->off_			 = e;
				as_header->prev_ = l;
				return ++as_header;
			}

			/// Deallocate
			/*! @brief Deallocates a memory pointed by @a __ptr.
			 *
			 *  @param[in] __ptr Pointer referring to a memory to be deallocated.
			 */
			pf_decl_constexpr void deallocate(
					void *__ptr) pf_attr_noexcept
			{
				if (!__ptr) return;
				union
				{
					void *as_void;
					__alloc_h *as_header;
				};
				as_void = __ptr;
				--as_header;
				if (as_header->prev_ < this->off_)
					this->off_ = as_header->prev_;
			}
		};

		/// MEMORY: Allocator Pool
		/*! @brief Pool memory allocator.
		 */
		class allocator_pool
		{
			using __node_t = singly_node<byte_t[]>;
			using __list_t = singly_linked_list<byte_t[]>;

			/// Initialize
			/*! @brief Initializes the allocator by initializing the allocation nodes.
			 */
			pf_decl_constexpr void __initialize() pf_attr_noexcept
			{
				union
				{
					byte_t *as_byte;
					__node_t *as_node;
					size_t as_addr;
				};
				as_byte = this->buf_.begin();
				as_byte += padding_of(as_addr + sizeof(__node_t), this->elemalign_);
				for (size_t i = 0; i < this->elemcount_; ++i)
				{
					this->list_.insert_tail(as_node);
					as_byte += this->elemsize_;
				}
			}

		public:
			using propagate_on_container_copy_assignment = std::true_type;
			using propagate_on_container_move_assignment = std::false_type;
			using propagate_on_container_swap						 = std::true_type;

			/// Constructors
			/*! @brief Constructor.
			 *
			 *  @param[in] __elemsize	 Maximum size of an element.
			 *  @param[in] __elemcount Maximum allocator's element number.
			 *  @param[in] __maxalign  Maximum alignment of an allocation.
			 */
			pf_decl_constexpr allocator_pool(
					size_t __elemsize,
					size_t __elemcount,
					align_val_t __maxalign = MAX_ALIGN)
					: buf_(
							(padding_of(sizeof(__node_t), __maxalign) + (__elemsize += padding_of(sizeof(__node_t) + __elemsize, __maxalign) + sizeof(__node_t))) * __elemcount,
							__maxalign)
					, elemsize_(__elemsize)
					, elemcount_(__elemcount)
					, elemalign_(__maxalign)
			{
				this->__initialize();
			}
			/*! @brief Copy constructor.
			 *
			 *  @param[in] __r Other pool allocator to copy from.
			 */
			pf_decl_constexpr allocator_pool(
					allocator_pool const &__r) pf_attr_noexcept
					: allocator_pool(__r.elemsize_, __r.elemcount_, __r.elemalign_)
			{}
			/*! @brief Copy constructor.
			 *
			 *  @param[in] __r 				Other pool allocator to copy from.
			 *  @param[in] __bufalign Alignment of the buffer.
			 */
			pf_decl_constexpr allocator_pool(
					allocator_pool const &__r,
					align_val_t __maxalign) pf_attr_noexcept
					: allocator_pool(__r.elemsize_, __r.elemcount_, __maxalign)
			{}
			/*! @brief Move constructor.
			 *
			 *	@param[in] __r Other pool allocator to move from.
			 */
			pf_decl_constexpr allocator_pool(
					allocator_pool &&__r) pf_attr_noexcept
					: buf_(std::move(__r.buf_))
					, list_(std::move(__r.list_))
					, elemsize_(__r.elemsize_)
					, elemcount_(__r.elemcount_)
					, elemalign_(__r.elemalign_)
			{}

			/// Destructor
			/*! @brief Destructor.
			 */
			pf_decl_constexpr ~allocator_pool() pf_attr_noexcept
			{
				this->list_.clear();
			}

			/// Operators=
			/*! @brief Copy assignment operator.
			 *
			 *  @param[in] __r Other pool allocator to copy from.
			 *  @return Reference on this allocator.
			 */
			pf_decl_constexpr allocator_pool &operator=(
					allocator_pool const &__r) pf_attr_noexcept
			{
				if (&__r == this) return *this;
				this->buf_			 = __r.buf_;
				this->elemsize_	 = __r.elemsize_;
				this->elemcount_ = __r.elemcount_;
				this->elemalign_ = __r.elemalign_;
				this->__initialize();
				return *this;
			}
			/*! @brief Move assignment operator.
			 *
			 *  @param[in] __r Other linear allocator to move from.
			 *  @return Reference on this allocator.
			 */
			pf_decl_constexpr allocator_pool &operator=(
					allocator_pool &&__r) pf_attr_noexcept
			{
				if (&__r == this) return *this;
				this->buf_			 = std::move(__r.buf_);
				this->list_			 = std::move(__r.list_);
				this->elemsize_	 = __r.elemsize_;
				this->elemcount_ = __r.elemcount_;
				this->elemalign_ = __r.elemalign_;
				return *this;
			}

			/// Operator==
			/*! @brief Equality operator.
			 *
			 *  @param[in] __r Other Allocator to compare with.
			 *  @return False, can't be equal with any allocator (unique memory).
			 */
			template <typename _Allocator>
			pf_decl_constexpr bool operator==(
					_Allocator const &__r) pf_attr_noexcept
			{
				return false;
			}

			/// Allocate
			/*! @brief Allocates a memory of size @a __size, aligned to @a __align at the head of
			 *				 @a __offset.
			 *
			 *  @param[in] __size 	Size of the allocation.
			 *  @param[in] __align	Alignment of the allocation.
			 *  @param[in] __offset	Offset to alignment.
			 *  @return Pointer on a allocated memory.
			 */
			pf_hint_nodiscard pf_decl_constexpr void *allocate(
					size_t __size,
					align_val_t __align = MAX_ALIGN,
					size_t __offset			= 0) pf_attr_noexcept
			{
				std::ignore = __size;
				std::ignore = __align;
				std::ignore = __offset;
				__node_t *n = this->list_.remove_head();
				return ++n;
			}

			/// Deallocate
			/*! @brief Deallocates a memory pointed by @a __ptr.
			 *
			 *  @param[in] __ptr Pointer referring to a memory to be deallocated.
			 */
			pf_decl_constexpr void deallocate(
					void *__ptr) pf_attr_noexcept
			{
				if (!__ptr) return;
				union
				{
					void *as_void;
					__node_t *as_node;
				};
				as_void = __ptr;
				--as_node;
				pf_assert(!this->list_.contains(as_node), "Pool allocator cannot deallocate a free block!");
				this->list_.insert_tail(as_node);
			}

		private:
			buffer buf_;
			__list_t list_;
			size_t elemsize_;
			size_t elemcount_;
			align_val_t elemalign_;
		};

		/// MEMORY: CDS Allocators
		class allocator_cds_linear;
		class allocator_cds_stack;
		class allocator_cds_pool;

		/// MEMORY: CDS Allocator Linear
		/*! @brief Concurrent linear memory allocator.
		 */
		class allocator_cds_linear
		{
			pf_decl_friend class allocator_cds_stack;

		public:
			using propagate_on_container_copy_assignment = std::true_type;
			using propagate_on_container_move_assignment = std::false_type;
			using propagate_on_container_swap						 = std::true_type;

			/// Constructors
			/*! @brief Constructor.
			 *
			 *  @param[in] __size 		Size of the memory buffer.
			 *  @param[in] __bufalign Alignment of the memory buffer.
			 */
			pf_decl_constexpr allocator_cds_linear(
					size_t __size,
					align_val_t __bufalign = MAX_ALIGN) pf_attr_noexcept
					: buf_(__size, __bufalign)
					, off_(this->buf_.begin())
			{}
			/*! @brief Copy constructor.
			 *
			 *  @param[in] __r Other linear allocator to copy from.
			 */
			allocator_cds_linear(
					allocator_cds_linear const &__r) pf_attr_noexcept
					: buf_(__r.buf_)
					, off_(__r.off_.load(std::memory_order_acquire))
			{}
			/*! @brief Copy constructor.
			 *
			 *  @param[in] __r Other linear allocator to copy from.
			 *  @param[in] __bufalign Alignment of the buffer.
			 */
			allocator_cds_linear(
					allocator_cds_linear const &__r,
					align_val_t __bufalign) pf_attr_noexcept
					: buf_(__r.buf_, __bufalign)
					, off_(__r.off_.load(std::memory_order_acquire))
			{}
			/*! @brief Move constructor.
			 *
			 *	@param[in] __r Other linear allocator to copy from.
			 */
			allocator_cds_linear(
					allocator_cds_linear &&__r) pf_attr_noexcept
					: buf_(std::move(__r.buf_))
					, off_(__r.off_.exchange(nullptr, std::memory_order::acq_rel))
			{}

			/// Operators=
			/*! @brief Copy assignment operator.
			 *
			 *  @param[in] __r Other linear allocator to copy from.
			 *  @return Reference on this allocator.
			 */
			allocator_cds_linear &operator=(
					allocator_cds_linear const &__r) pf_attr_noexcept
			{
				if (&__r == this) return *this;
				this->buf_ = __r.buf_;
				this->off_.store(this->buf_.begin(), std::memory_order::release);
				return *this;
			}
			/*! @brief Move assignment operator.
			 *
			 *  @param[in] __r Other linear allocator to move from.
			 *  @return Reference on this allocator.
			 */
			allocator_cds_linear &operator=(
					allocator_cds_linear &&__r) pf_attr_noexcept
			{
				if (&__r == this) return *this;
				this->buf_ = __r.buf_;
				this->off_.store(__r.off_.exchange(nullptr, std::memory_order::acq_rel), std::memory_order::release);
				return *this;
			}

			/// Operator==
			/*! @brief Equality operator.
			 *
			 *  @param[in] __r Other Allocator to compare with.
			 *  @return False, can't be equal with any allocator (unique memory).
			 */
			template <typename _Allocator>
			pf_decl_constexpr bool operator==(
					_Allocator const &__r) pf_attr_noexcept
			{
				return false;
			}

			/// Allocate
			/*! @brief Allocates a memory of size @a __size, aligned to @a __align at the head of
			 *				 @a __offset.
			 *
			 *  @param[in] __size 	Size of the allocation.
			 *  @param[in] __align	Alignment of the allocation.
			 *  @param[in] __offset	Offset to alignment.
			 *  @return Pointer on a allocated memory.
			 */
			pf_hint_nodiscard void *allocate(
					size_t __size,
					align_val_t __align = MAX_ALIGN,
					size_t __offset			= 0) pf_attr_noexcept
			{
				__size += padding_of(__size + __offset, __align);
				union
				{
					byte_t *as_byte;
					size_t as_addr;
				};
				as_byte		= this->off_.load(std::memory_order::relaxed);
				byte_t *e = nullptr;
				do
				{
					as_addr += padding_of(as_addr + __offset, __align);
					e = as_byte + __size;
					if (e > this->buf_.end())
						return nullptr;
				} while (!this->off_.compare_exchange_weak(
						as_byte,
						e,
						std::memory_order::acquire,
						std::memory_order::release));
				return as_byte;
			}

			/// Deallocate
			/*! @brief Deallocates a memory pointed by @a __ptr.
			 *
			 *  @param[in] __ptr Pointer referring to a memory to be deallocated.
			 */
			void deallocate(
					void *__ptr) pf_attr_noexcept
			{
				std::ignore = __ptr;
			}

			/// Clear
			/*! @brief Resets the allocator to zero.
			 *
			 *  @warning Must not be used with types using this memory that use dynamic allocations
			 *  				 themselves!
			 */
			size_t clear(
					std::memory_order __order = std::memory_order::relaxed) pf_attr_noexcept
			{
				this->buf_.fill();
				return std::distance(
						this->buf_.begin(),
						this->off_.exchange(this->buf_.begin(), __order));
			}

		private:
			buffer buf_;
			std::atomic<byte_t *> off_;
		};

		/// MEMORY: Allocator Stack
		/*! @brief Concurrent stack memory allocator.
		 */
		class allocator_cds_stack: public allocator_cds_linear
		{
			struct __alloc_h
			{
				byte_t *prev_;
			};

		public:
			using propagate_on_container_copy_assignment = std::true_type;
			using propagate_on_container_move_assignment = std::false_type;
			using propagate_on_container_swap						 = std::true_type;

			/// Constructors
			/*! @brief Constructor.
			 *
			 *  @param[in] __size		  Size of the memory buffer.
			 *  @param[in] __bufalign Alignment of the memory buffer.
			 */
			pf_decl_constexpr allocator_cds_stack(
					size_t __size,
					align_val_t __bufalign = MAX_ALIGN) pf_attr_noexcept
					: allocator_cds_linear(__size, __bufalign)
			{}
			/*! @brief Copy constructor.
			 *
			 *  @param[in] __r Other stack allocator to copy from.
			 */
			allocator_cds_stack(
					allocator_cds_stack const &__r) pf_attr_noexcept
					: allocator_cds_linear(__r)
			{}
			/*! @brief Copy constructor.
			 *
			 *  @param[in] __r Other stack allocator to copy from.
			 *  @param[in] __bufalign Alignment of the memory buffer.
			 */
			allocator_cds_stack(
					allocator_cds_stack const &__r,
					align_val_t __bufalign) pf_attr_noexcept
					: allocator_cds_linear(__r, __bufalign)
			{}
			/*! @brief Move constructor
			 *
			 *  @param[in] __r Other stack allocator to move from.
			 */
			allocator_cds_stack(
					allocator_cds_stack &&__r) pf_attr_noexcept
					: allocator_cds_linear(std::move(__r))
			{}

			/// Allocate
			/*! @brief Allocates a memory of size @a __size, aligned to @a __align at the head of
			 *				 @a __offset.
			 *
			 *  @param[in] __size 	Size of the allocation.
			 *  @param[in] __align	Alignment of the allocation.
			 *  @param[in] __offset	Offset to alignment.
			 *  @return Pointer on a allocated memory.
			 */
			pf_hint_nodiscard void *allocate(
					size_t __size,
					align_val_t __align = MAX_ALIGN,
					size_t __offset			= 0) pf_attr_noexcept
			{
				__size += padding_of(__size + __offset, __align);
				union
				{
					byte_t *as_byte;
					__alloc_h *as_header;
					size_t as_addr;
				};
				as_byte		= this->off_.load(std::memory_order::relaxed);
				byte_t *e = nullptr, *l = as_byte;
				do
				{
					as_addr += padding_of(as_addr + __offset, __align);
					e = as_byte + sizeof(__alloc_h) + __size;
					if (e > this->buf_.end())
						return nullptr;
				} while (!this->off_.compare_exchange_weak(
						as_byte,
						e,
						std::memory_order::acquire,
						std::memory_order::release));
				as_header->prev_ = l;
				return ++as_header;
			}

			/// Deallocate
			/*! @brief Deallocates a memory pointed by @a __ptr.
			 *
			 *  @param[in] __ptr Pointer referring to a memory to be deallocated.
			 */
			pf_decl_constexpr void deallocate(
					void *__ptr) pf_attr_noexcept
			{
				if (!__ptr) return;
				union
				{
					void *as_void;
					__alloc_h *as_header;
				};
				as_void = __ptr;
				--as_header;
				if (as_header->prev_ < this->off_.load(std::memory_order::acquire))
					this->off_.store(as_header->prev_, std::memory_order::release);
			}
		};

		/// MEMORY: CDS Allocator Pool
		/*! @brief Concurrent pool memory allocator.
		 */
		class allocator_cds_pool: public buffer
		{
			using __node_t = singly_node<byte_t[]>;
			using __list_t = singly_cds_linked_lifo<byte_t[]>;

			/// Initialize
			/*! @brief Initializes the allocator by initializing the allocation nodes.
			 */
			void __initialize() pf_attr_noexcept
			{
				union
				{
					byte_t *as_byte;
					__node_t *as_node;
					size_t as_addr;
				};
				as_byte = this->buf_.begin();
				as_byte += padding_of(as_addr + sizeof(__node_t), this->elemalign_);
				iterator_sequence b(as_node, this->elemsize_);
				as_byte += this->elemsize_ * this->elemcount_;
				iterator_sequence e(as_node, this->elemsize_);
				this->list_.insert_tail_bulk(b, e);
			}

		public:
			using propagate_on_container_copy_assignment = std::true_type;
			using propagate_on_container_move_assignment = std::false_type;
			using propagate_on_container_swap						 = std::true_type;

			/// Constructors
			/*! @brief Constructor.
			 *
			 *  @param[in] __elemsize	 Maximum size of an element.
			 *  @param[in] __elemcount Maximum allocator's element number.
			 *  @param[in] __maxalign  Maximum alignment of an allocation.
			 */
			pf_decl_constexpr allocator_cds_pool(
					size_t __elemsize,
					size_t __elemcount,
					align_val_t __maxalign = MAX_ALIGN)
					: buf_(
							(padding_of(sizeof(__node_t), __maxalign) + (__elemsize += padding_of(sizeof(__node_t) + __elemsize, __maxalign) + sizeof(__node_t))) * __elemcount,
							__maxalign)
					, elemsize_(__elemsize)
					, elemcount_(__elemcount)
					, elemalign_(__maxalign)
			{
				this->__initialize();
			}
			/*! @brief Copy constructor.
			 *
			 *  @param[in] __r Other pool allocator to copy from.
			 */
			pf_decl_constexpr allocator_cds_pool(
					allocator_cds_pool const &__r) pf_attr_noexcept
					: allocator_cds_pool(__r.elemsize_, __r.elemcount_, __r.elemalign_)
			{}
			/*! @brief Copy constructor.
			 *
			 *  @param[in] __r 				Other pool allocator to copy from.
			 *  @param[in] __bufalign Alignment of the buffer.
			 */
			allocator_cds_pool(
					allocator_cds_pool const &__r,
					align_val_t __maxalign) pf_attr_noexcept
					: allocator_cds_pool(__r.elemsize_, __r.elemcount_, __maxalign)
			{}
			/*! @brief Move constructor.
			 *
			 *	@param[in] __r Other pool allocator to move from.
			 */
			allocator_cds_pool(
					allocator_cds_pool &&__r) pf_attr_noexcept
					: buffer(std::move(__r))
					, list_(std::move(__r.list_))
					, elemsize_(__r.elemsize_)
					, elemcount_(__r.elemcount_)
			{}

			/// Destructor
			/*! @brief Destructor.
			 */
			~allocator_cds_pool() pf_attr_noexcept
			{
				this->list_.clear();
			}

			/// Operators=
			/*! @brief Copy assignment operator.
			 *
			 *  @param[in] __r Other pool allocator to copy from.
			 *  @return Reference on this allocator.
			 */
			allocator_cds_pool &operator=(
					allocator_cds_pool const &__r) pf_attr_noexcept
			{
				if (&__r == this) return *this;
				this->buf_			 = __r.buf_;
				this->elemsize_	 = __r.elemsize_;
				this->elemcount_ = __r.elemcount_;
				this->elemalign_ = __r.elemalign_;
				this->__initialize();
				return *this;
			}
			/*! @brief Move assignment operator.
			 *
			 *  @param[in] __r Other linear allocator to move from.
			 *  @return Reference on this allocator.
			 */
			allocator_cds_pool &operator=(
					allocator_cds_pool &&__r) pf_attr_noexcept
			{
				if (&__r == this) return *this;
				this->buf_			 = std::move(__r.buf_);
				this->list_			 = std::move(__r.list_);
				this->elemsize_	 = __r.elemsize_;
				this->elemcount_ = __r.elemcount_;
				this->elemalign_ = __r.elemalign_;
				return *this;
			}

			/// Operator==
			/*! @brief Equality operator.
			 *
			 *  @param[in] __r Other Allocator to compare with.
			 *  @return False, can't be equal with any allocator (unique memory).
			 */
			template <typename _Allocator>
			pf_decl_constexpr bool operator==(
					_Allocator const &__r) pf_attr_noexcept
			{
				return false;
			}

			/// Allocate
			/*! @brief Allocates a memory of size @a __size, aligned to @a __align at the head of
			 *				 @a __offset.
			 *
			 *  @param[in] __size 	Size of the allocation.
			 *  @param[in] __align	Alignment of the allocation.
			 *  @param[in] __offset	Offset to alignment.
			 *  @return Pointer on a allocated memory.
			 */
			pf_hint_nodiscard void *allocate(
					size_t __size,
					align_val_t __align = MAX_ALIGN,
					size_t __offset			= 0) pf_attr_noexcept
			{
				std::ignore = __size;
				std::ignore = __align;
				std::ignore = __offset;
				__node_t *n = this->list_.remove_head();
				return ++n;
			}

			/// Deallocate
			/*! @brief Deallocates a memory pointed by @a __ptr.
			 *
			 *  @param[in] __ptr Pointer referring to a memory to be deallocated.
			 */
			void deallocate(
					void *__ptr) pf_attr_noexcept
			{
				if (!__ptr) return;
				union
				{
					void *as_void;
					__node_t *as_node;
				};
				as_void = __ptr;
				--as_node;
				pf_assert(!this->list_.contains(as_node), "Pool allocator cannot deallocate a free block!");
				this->list_.insert_tail(as_node);
			}

		private:
			buffer buf_;
			__list_t list_;
			size_t elemsize_;
			size_t elemcount_;
			align_val_t elemalign_;
		};

		/// MEMORY: Allocator Wrapper
		/*! @brief Type of allocator acting as an interface to the standard library. Takes as parameter
		 *				 another allocator.
		 *
		 *  @tparam _Ty				 Type of object to allocate.
		 *  @tparam _Allocator Type of allocator.
		 */
		template <typename _Ty, typename _Allocator>
		class allocator_wrapper
		{
		public:
			using value_type														 = _Ty;
			using size_type															 = size_t;
			using difference_type												 = diff_t;
			using propagate_on_container_copy_assignment = _Allocator::propagate_on_container_copy_assignment;
			using propagate_on_container_move_assignment = _Allocator::propagate_on_container_move_assignment;
			using propagate_on_container_swap						 = _Allocator::propagate_on_container_swap;

			/// Constructors
			/*! @brief Constructor.
			 *
			 *  @param[in] __all Allocator to encapsulate.
			 */
			pf_decl_constexpr allocator_wrapper(
					_Allocator &&__all) pf_attr_noexcept
					: all_(std::move(__all))
			{}
			/*! @brief Copy constructor.
			 *
			 *  @param[in] __allwrp Wrapper to copy from.
			 */
			pf_decl_constexpr allocator_wrapper(
					allocator_wrapper<_Ty, _Allocator> const &__allwrp) pf_attr_noexcept requires(std::is_copy_constructible_v<_Allocator>)
					: all_(__allwrp.all_)
			{}
			/*! @brief Move constructor.
			 *
			 *  @param[in] __allwrp Wrapper to move from.
			 */
			pf_decl_constexpr allocator_wrapper(
					allocator_wrapper<_Ty, _Allocator> &&__allwrp) pf_attr_noexcept requires(std::is_move_constructible_v<_Allocator>)
					: all_(std::move(__allwrp.all_))
			{}

			/// Operators=
			/*! @brief Copy assignment operator.
			 *
			 *  @param[in] __allwrp Wrapper to copy from.
			 *  @return Reference on this.
			 */
			pf_decl_constexpr allocator_wrapper<_Ty, _Allocator> &operator=(
					allocator_wrapper<_Ty, _Allocator> const &__allwrp) pf_attr_noexcept requires(std::is_copy_assignable_v<_Allocator>)
			{
				if (&__allwrp == this) return *this;
				this->all_ = __allwrp.all_;
				return *this;
			}
			/*! @brief Move assignment operator.
			 *
			 *  @param[in] __allwrp Wrapper to move from.
			 *  @return Reference on this.
			 */
			pf_decl_constexpr allocator_wrapper<_Ty, _Allocator> &operator=(
					allocator_wrapper<_Ty, _Allocator> &&__allwrp) pf_attr_noexcept requires(std::is_move_assignable_v<_Allocator>)
			{
				if (&__allwrp == this) return *this;
				this->all_ = std::move(__allwrp.all_);
				return *this;
			}

			/// Operator==
			/*! @brief Equality operator.
			 *
			 *  @param[in] __r Wrapper to compare with.
			 *  @return True if equals.
			 *  @return False otherwise.
			 */
			pf_hint_nodiscard pf_decl_constexpr bool operator==(
					allocator_wrapper<_Ty, _Allocator> const &__r) const pf_attr_noexcept
			{
				return this->all_ == __r.all_;
			}

			/// Allocate
			/*! @brief Allocates a memory of size @a __size, aligned to @a __align at the head of
			 *				 @a __offset.
			 *
			 *  @param[in] __size 	Size of the allocation.
			 *  @param[in] __align	Alignment of the allocation.
			 *  @param[in] __offset	Offset to alignment.
			 *  @return Pointer on a allocated memory.
			 */
			pf_hint_nodiscard pf_decl_constexpr void *allocate(
					size_t __count,
					align_val_t __align = align_val_t(alignof(_Ty)),
					size_t __offset			= 0)
			{
				return this->all_.allocate(__count + padding_of(__count + __offset, __align), __align);
			}

			/// Deallocate
			/*! @brief Deallocates a memory pointed by @a __ptr.
			 *
			 *  @param[in] __ptr Pointer referring to a memory to be deallocated.
			 */
			pf_decl_constexpr void deallocate(
					void *__ptr) pf_attr_noexcept
			{
				this->all_.deallocate(__ptr);
			}

		private:
			_Allocator all_;
		};

	}
}

/// NEW: Allocator Allocate
/*! @brief Operator new for allocators.
 *
 *  @tparam _Allocator Type of allocator.
 *  @param[in] __size Size of allocation.
 *  @param[in] __all  Allocator instance.
 *  @return @ref __all.allocate(__size).
 */
template <typename _Allocator>
pf_hint_nodiscard pf_decl_inline void *operator new(
		pul::size_t __size,
		_Allocator &__all) pf_attr_noexcept
{
	return __all.allocate(__size);
}
/*! @brief Operator new for allocators.
 *
 *  @tparam _Allocator Type of allocator.
 *  @param[in] __size  Size of allocation.
 *  @param[in] __align Alignment of the allocation.
 *  @param[in] __all   Allocator instance.
 *  @return @ref __all.allocate(__size, __align).
 */
template <typename _Allocator>
pf_hint_nodiscard pf_decl_inline void *operator new(
		pul::size_t __size,
		pul::align_val_t __align,
		_Allocator &__all) pf_attr_noexcept
{
	return __all.allocate(__size, __align);
}
/*! @brief Operator new for allocators.
 *
 *  @tparam _Allocator Type of allocator.
 *  @param[in] __size   Size of allocation.
 *  @param[in] __align  Alignment of the allocation.
 *  @param[in] __offset Alignment offset of the allocation.
 *  @param[in] __all    Allocator instance.
 *  @return @ref __all.allocate(__size, __align, __offset).
 */
template <typename _Allocator>
pf_hint_nodiscard pf_decl_inline void *operator new(
		pul::size_t __size,
		pul::align_val_t __align,
		pul::size_t __offset,
		_Allocator &__all) pf_attr_noexcept
{
	return __all.allocate(__size, __align, __offset);
}
/*! @brief Operator new[] for allocators.
 *
 *  @tparam _Allocator Type of allocator.
 *  @param[in] __size Size of allocation.
 *  @param[in] __all  Allocator instance.
 *  @return @ref __all.allocate(__size).
 */
template <typename _Allocator>
pf_hint_nodiscard pf_decl_inline void *operator new[](
		pul::size_t __size,
		_Allocator &__all) pf_attr_noexcept
{
	return __all.allocate(__size);
}
/*! @brief Operator new[] for allocators.
 *
 *  @tparam _Allocator Type of allocator.
 *  @param[in] __size  Size of allocation.
 *  @param[in] __align Alignment of the allocation.
 *  @param[in] __all   Allocator instance.
 *  @return @ref __all.allocate(__size, __align).
 */
template <typename _Allocator>
pf_hint_nodiscard pf_decl_inline void *operator new[](
		pul::size_t __size,
		pul::align_val_t __align,
		_Allocator &__all) pf_attr_noexcept
{
	return __all.allocate(__size, __align);
}
/*! @brief Operator new[] for allocators.
 *
 *  @tparam _Allocator Type of allocator.
 *  @param[in] __size   Size of allocation.
 *  @param[in] __align  Alignment of the allocation.
 *  @param[in] __offset Alignment offset of the allocation.
 *  @param[in] __all    Allocator instance.
 *  @return @ref __all.allocate(__size, __align, __offset).
 */
template <typename _Allocator>
pf_hint_nodiscard pf_decl_inline void *operator new[](
		pul::size_t __size,
		pul::align_val_t __align,
		pul::size_t __offset,
		_Allocator &__all) pf_attr_noexcept
{
	return __all.allocate(__size, __align, __offset);
}

/// DELETE: Allocator Deallocate
/*! @brief Operator delete for allocators.
 *
 *  @tparam _Allocator Type of allocator.
 *  @param[in] __ptr Pointer on memory ressource allocated by @a __all.
 *  @param[in] __all Allocator instance.
 *  @return @ref __all.deallocate(__ptr).
 */
template <typename _Allocator>
pf_decl_inline void operator delete(
		void *__ptr,
		_Allocator &__all)pf_attr_noexcept
{
	__all.deallocate(__ptr);
}
/*! @brief Operator delete[] for allocators.
 *
 *  @tparam _Allocator Type of allocator.
 *  @param[in] __ptr Pointer on memory ressource allocated by @a __all.
 *  @param[in] __all Allocator instance.
 *  @return @ref __all.deallocate(__ptr).
 */
template <typename _Allocator>
pf_decl_inline void operator delete[](
		void *__ptr,
		_Allocator &__all) pf_attr_noexcept
{
	__all.deallocate(__ptr);
}

#endif // !PULSAR_MEMORY_HPP