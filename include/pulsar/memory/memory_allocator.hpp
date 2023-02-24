/*! @file   memory_allocator.hpp
 *  @author Louis-Quentin Noé (noe.louis-quentin@hotmail.fr)
 *  @brief
 *  @date   19-02-2023
 *
 *  @copyright Copyright (c) 2023 - Pulsar Software
 *
 *  @since 0.1.4
 */

#ifndef PULSAR_MEMORY_ALLOCATOR_HPP
#define PULSAR_MEMORY_ALLOCATOR_HPP 1

// Include: Pulsar
#include "pulsar/memory.hpp"

// Pulsar
namespace pul
{

	/// MEMORY: Allocator -> Concept
	template <typename _Allocator>
	concept __allocator_c = requires(
		_Allocator & __all)
	{
		{ __all.allocate(std::declval<size_t>(), std::declval<align_val_t>(), std::declval<size_t>()) }->std::convertible_to<void*>;
		{ __all.reallocate(std::declval<void*>(), std::declval<size_t>(), std::declval<align_val_t>(), std::declval<size_t>()) }->std::convertible_to<void*>;
		__all.deallocate(std::declval<void*>());
	};

	/// MEMORY: Allocator -> Is*
	template <typename _Allocator>
	struct is_allocator : std::false_type
	{};
	template <__allocator_c _Allocator>
	struct is_allocator<_Allocator> : std::true_type
	{};
	template <typename _Allocator>
	pf_decl_static pf_decl_constexpr bool is_allocator_v = is_allocator<_Allocator>::value;

}

// Include: Pulsar -> Memory -> Allocators
#include "pulsar/memory/allocator/allocator_default.hpp"

#endif // !PULSAR_MEMORY_ALLOCATOR_HPP