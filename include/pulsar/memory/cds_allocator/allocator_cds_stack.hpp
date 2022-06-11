/*! @file   allocator_cds_stack.hpp
 *  @author Fluffy (noe.louis-quentin@hotmail.fr)
 *  @brief
 *  @date   11-06-2022
 *
 *  @copyright Copyright (c) 2022 - Pulsar Software
 *
 *  @since 0.1.1
 */

#ifndef PULSAR_MEMORY_ALLOCATOR_CDS_STACK_HPP
#define PULSAR_MEMORY_ALLOCATOR_CDS_STACK_HPP

// Include: Pulsar
#include "pulsar/memory.hpp"

// Pulsar
namespace pul
{
	// Memory
	namespace memory
	{
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
	}
}

#endif // !PULSAR_MEMORY_ALLOCATOR_CDS_STACK_HPP