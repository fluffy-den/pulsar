/*! @file   allocator_pool.hpp
 *  @author Fluffy (noe.louis-quentin@hotmail.fr)
 *  @brief
 *  @date   11-06-2022
 *
 *  @copyright Copyright (c) 2022 - Pulsar Software
 *
 *  @since 0.1.1
 */

#ifndef PULSAR_MEMORY_ALLOCATOR_POOL_HPP
#define PULSAR_MEMORY_ALLOCATOR_POOL_HPP 1

// Include: Pulsar
#include "pulsar/memory.hpp"

// Pulsar
namespace pul
{
	// Memory
	namespace memory
	{
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
	}
}

#endif // !PULSAR_MEMORY_ALLOCATOR_POOL_HPP