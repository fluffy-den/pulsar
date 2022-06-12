/*! @file   allocator_ring.hpp
 *  @author Fluffy (noe.louis-quentin@hotmail.fr)
 *  @brief
 *  @date   12-06-2022
 *
 *  @copyright Copyright (c) 2022 - Pulsar Software
 *
 *  @since 0.1.0
 */

#ifndef PULSAR_ALLOCATOR_RING_HPP
#define PULSAR_ALLOCATOR_RING_HPP 1

// Include: Pulsar
#include "pulsar/memory.hpp"

// Pulsar
namespace pul
{
	// Memory
	namespace memory
	{
		/// MEMORY: Allocator Ring
		class allocator_ring: public allocator_linear
		{
		public:
			using propagate_on_container_copy_assignment = std::true_type;
			using propagate_on_container_move_assignment = std::false_type;
			using propagate_on_container_swap						 = std::true_type;

			/// Constructors
			/*! @brief
			 *
			 *  @param[in] __size
			 *  @param[in] __bufalign
			 *  @return pf_decl_constexpr
			 */
			pf_decl_constexpr allocator_ring(
					size_t __size,
					align_val_t __bufalign = MAX_ALIGN) pf_attr_noexcept
					: allocator_linear(__size, __bufalign)
			{}
			/*! @brief
			 *
			 *  @param[in] __r
			 *  @return pf_decl_constexpr
			 */
			pf_decl_constexpr allocator_ring(
					allocator_ring const &__r) pf_attr_noexcept
					: allocator_linear(__r)
			{}
			/*! @brief
			 *
			 *  @param[in] __r
			 *  @return pf_decl_constexpr
			 */
			pf_decl_constexpr allocator_ring(
					allocator_ring const &__r,
					align_val_t __bufalign) pf_attr_noexcept
					: allocator_linear(__r, __bufalign)
			{}
			/*! @brief
			 *
			 *  @param[in] __r
			 *  @return pf_decl_consteval
			 */
			pf_decl_consteval allocator_ring(
					allocator_ring &&__r) pf_attr_noexcept
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
				void *all = allocator_linear::allocate(__size, __align, __offset);
				if (!all)
				{
					allocator_linear::clear();
					all = allocator_linear::allocate(__size, __align, __offset);
				}
				return all;
			}
		};
	}
}

#endif // !PULSAR_ALLOCATOR_RING_HPP