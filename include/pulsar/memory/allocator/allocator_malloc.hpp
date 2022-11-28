/*! @file   allocator_malloc.hpp
 *  @author Louis-Quentin NOE (noe.louis-quentin@hotmail.fr)
 *  @brief
 *  @date   22-09-2022
 *
 *  @copyright Copyright (c) 2022 - Pulsar Software
 *
 *  @since 0.1.2
 */

#ifndef PULSAR_ALLOCATOR_MALLOC_HPP
#define PULSAR_ALLOCATOR_MALLOC_HPP 1

// Include: Pulsar -> Memory
#include "pulsar/pulsar.hpp"

// Pulsar
namespace pul
{
	/// ALLOCATOR: X
	struct allocator_malloc pf_attr_final
	{
		/// Constructors
		pf_decl_constexpr
		allocator_malloc() pf_attr_noexcept = default;
		pf_decl_constexpr
		allocator_malloc(
			allocator_malloc const &) = default;
		pf_decl_constexpr
		allocator_malloc(
			allocator_malloc &&) = default;

		/// Allocate
		pf_hint_nodiscard void *
		allocate(
			size_t __s,
			align_val_t __a,
			size_t __o)
		{
			return mi_malloc_aligned_at(__s, static_cast<size_t>(__a), __o);
		}

		/// Reallocate
		pf_hint_nodiscard void *
		reallocate(
			void *__p,
			size_t __s,
			align_val_t __a,
			size_t __o)
		{
			return mi_realloc_aligned_at(__p, __s,  static_cast<size_t>(__a), __o);
		}

		/// Deallocate
		void
		deallocate(
			void *__p) pf_attr_noexcept
		{
			mi_free(__p);
		}
	};
}

#endif // !PULSAR_ALLOCATOR_MALLOC_HPP