/*! @file   allocator.hpp
 *  @author Louis-Quentin Noé (noe.louis-quentin@hotmail.fr)
 *  @brief
 *  @date   05-03-2023
 *
 *  @copyright Copyright (c) 2023 - Pulsar Software
 *
 *  @since 0.1.4
 */

#ifndef PULSAR_ALLOCATOR_HPP
#define PULSAR_ALLOCATOR_HPP 1

// Include: Pulsar
#include "pulsar/heap.hpp"
#include "pulsar/magnifier.hpp"
#include "pulsar/utility.hpp"

// Pulsar
namespace pul
{
	/// ALLOCATOR: Concept
	template <typename _Allocator>
	concept __allocator_c = requires(
		_Allocator & __all)
	{
		{ __all.allocate(std::declval<size_t>(), std::declval<align_val_t>(), std::declval<size_t>()) }->std::convertible_to<void*>;
		{ __all.reallocate(std::declval<void*>(), std::declval<size_t>(), std::declval<align_val_t>(), std::declval<size_t>()) }->std::convertible_to<void*>;
		__all.deallocate(std::declval<void*>());
	};
	template <typename _Allocator>
	struct is_allocator : std::false_type
	{};
	template <__allocator_c _Allocator>
	struct is_allocator<_Allocator> : std::true_type
	{};
	template <typename _Allocator>
	pf_decl_static pf_decl_constexpr bool is_allocator_v = is_allocator<_Allocator>::value;

	/// ALLOCATOR: Allocate
	template <typename _Allocator>
	pf_hint_nodiscard pf_decl_inline pf_decl_constexpr void*
	allocate(
		_Allocator &__all,
		size_t __size,
		align_val_t __align = ALIGN_DEFAULT,
		size_t __offset			= 0) pf_attr_noexcept
	requires(is_allocator_v<_Allocator>)
	{
		if(std::is_constant_evaluated())
		{
			return new byte_t[__size];
		}
		else
		{
			return __all.allocate(
				__size,
				union_cast<size_t>(__align),
				__offset);
		}
	}

	/// ALLOCATOR: Reallocate
	template <typename _Allocator>
	pf_decl_inline pf_decl_constexpr void*
	reallocate(
		_Allocator &__all,
		void *__ptr,
		size_t __osize,
		size_t __nsize,
		align_val_t __align = ALIGN_DEFAULT,
		size_t __offset			= 0) pf_attr_noexcept
	{
		if (std::is_constant_evaluated())
		{
			byte_t *p = new byte_t[__nsize];
			if (!__ptr) return p;
			std::memcpy(p, __ptr, std::min(__osize, __nsize));
			deallocate(__all, __ptr);
			return p;
		}
		else
		{
			return __all.reallocate(__ptr, __nsize, __align, __offset);	// TODO: Note is it correct?
		}
	}

	/// ALLOCATOR: Deallocate
	template <typename _Allocator>
	pf_decl_inline pf_decl_constexpr void
	deallocate(
		_Allocator &__all,
		void *__ptr) pf_attr_noexcept
	{
		if (std::is_constant_evaluated())
		{
			delete[] union_cast<byte_t*>(__ptr);
		}
		else
		{
			__all.deallocate(__ptr);
		}
	}
}

// Include: Pulsar -> Allocator
#include "pulsar/allocator/allocator_default.hpp"
#include "pulsar/allocator/allocator_samd_ring.hpp"

#endif // !PULSAR_ALLOCATOR_HPP