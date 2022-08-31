/*! @file   allocator_cds_ring.hpp
 *  @author Fluffy (noe.louis-quentin@hotmail.fr)
 *  @brief
 *  @date   13-06-2022
 *
 *  @copyright Copyright (c) 2022 - Pulsar Software
 *
 *  @since 0.1.0
 */

#ifndef PULSAR_ALLOCATOR_CDS_RING_HPP
#define PULSAR_ALLOCATOR_CDS_RING_HPP 1

// Include: Pulsar
#include "pulsar/memory.hpp"

// Include: C++
#include <tuple>

// Pulsar
namespace pul
{
	/// MEMORY: Allocator CDS Ring
	/*! @brief Concurrent ring memory allocator.
	 */
	class allocator_cds_ring: public allocator_cds_linear
	{
		using propagate_on_container_copy_assignment = std::true_type;
		using propagate_on_container_move_assignment = std::false_type;
		using propagate_on_container_swap						 = std::true_type;

	public:
		/// Constructors
		/*! @brief Default constructor.
		 */
		pf_decl_inline pf_decl_constexpr allocator_cds_ring() pf_attr_noexcept
		{}
		/*! @brief Constructor.
		 *
		 *  @param[in] __size 		Size of the memory buffer.
		 *  @param[in] __bufalign Alignment of the memory buffer.
		 */
		pf_decl_inline pf_decl_constexpr allocator_cds_ring(
				size_t __size,
				align_val_t __bufalign = MAX_ALIGN)
				: allocator_cds_linear(__size, __bufalign)
		{}
		/*! @brief Copy constructor.
		 *
		 *  @param[in] __r Other linear allocator to copy from.
		 */
		pf_decl_inline allocator_cds_ring(
				allocator_cds_linear const &__r) pf_attr_noexcept
				: allocator_cds_linear(__r)
		{}
		/*! @brief Copy constructor.
		 *
		 *  @param[in] __r 				Other linear allocator to copy from.
		 *  @param[in] __bufalign Alignment of the buffer.
		 */
		pf_decl_inline allocator_cds_ring(
				allocator_cds_linear const &__r,
				align_val_t __bufalign) pf_attr_noexcept
				: allocator_cds_linear(__r, __bufalign)
		{}
		/*! @brief Move constructor.
		 *
		 *	@param[in] __r Other linear allocator to copy from.
		 */
		pf_decl_inline allocator_cds_ring(
				allocator_cds_linear &&__r) pf_attr_noexcept
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
		pf_hint_nodiscard pf_decl_inline void *allocate(
				size_t __size,
				align_val_t __align = MAX_ALIGN,
				size_t __offset			= 0)
		{
			void *all = allocator_cds_linear::allocate(__size, __align, __offset);
			if (!all)
			{
				allocator_cds_linear::clear(std::memory_order::release);
				all = allocator_cds_linear::allocate(__size, __align, __offset);
			}
			return all;
		}
	};
}

#endif // !PULSAR_ALLOCATOR_CDS_RING_HPP