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
#include <algorithm>
#include <atomic>
#include <memory>
#include <type_traits>

// Include: C
#include <cstring>

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

	}
}

// Include: Pulsar -> Memory Buffers
#include "pulsar/memory/buffer.hpp"
#include "pulsar/memory/buffer_virtual.hpp"

// Include: Pulsar -> Memory Allocators
#include "pulsar/memory/allocator/allocator_linear.hpp"
#include "pulsar/memory/allocator/allocator_pool.hpp"
#include "pulsar/memory/allocator/allocator_stack.hpp"

// Include: Pulsar -> Memory CDS Allocators
#include "pulsar/memory/cds_allocator/allocator_cds_linear.hpp"
#include "pulsar/memory/cds_allocator/allocator_cds_pool.hpp"
#include "pulsar/memory/cds_allocator/allocator_cds_stack.hpp"

// Include: Pulsar -> Memory Allocator Wrapper
#include "pulsar/memory/allocator_wrapper.hpp"

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
		_Allocator &__all) pf_attr_noexcept
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