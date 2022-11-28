/*! @file   allocator.hpp
 *  @author Louis-Quentin NOE (noe.louis-quentin@hotmail.fr)
 *  @brief
 *  @date   22-09-2022
 *
 *  @copyright Copyright (c) 2022 - Pulsar Software
 *
 *  @since 0.1.2
 */

#ifndef PULSAR_ALLOCATOR_HPP
#define PULSAR_ALLOCATOR_HPP 1

// Include: Pulsar -> Memory -> Allocators
#include "pulsar/memory/allocator/allocator_malloc.hpp"

// Include: C++
#include <concepts>

// Pulsar
namespace pul
{
	/// MEMORY: Allocator Concept
	template <typename _Allocator>
	concept allocator_c = requires(
		_Allocator & __all,
		void *__p,
		size_t __s,
		align_val_t __a,
		size_t __o)
	{
		{ __all.allocate(__s, __a, __o) }->std::convertible_to<void *>;
		{ __all.reallocate(__p, __s, __a, __o) }->std::convertible_to<void *>;
		{ __all.deallocate(__p) }->std::convertible_to<void>;
	};

	/// MEMORY: Is Allocator
	template <typename _Allocator>
	struct is_allocator : std::false_type
	{};
	template <allocator_c _Allocator>
	struct is_allocator<_Allocator> : std::true_type
	{};
	template <typename _Allocator>
	pf_decl_constexpr pf_decl_inline bool is_allocator_v = is_allocator<_Allocator>::value;
}

#endif // !PULSAR_ALLOCATOR_HPP