/*! @file   allocator_default.hpp
 *  @author Louis-Quentin Noé (noe.louis-quentin@hotmail.fr)
 *  @brief
 *  @date   02-01-2023
 *
 *  @copyright Copyright (c) 2023 - Pulsar Software
 *
 *  @since 0.1.3
 */

#ifndef PULSAR_ALLOCATOR_DEFAULT_HPP
#define PULSAR_ALLOCATOR_DEFAULT_HPP 1

// Include: Pulsar
#include "pulsar/allocator.hpp"

// Pulsar
namespace pul
{
	/// MEMORY: Allocator -> Default
	class allocator_default pf_attr_final
	{
	public:
		/// Allocate
		pf_hint_nodiscard pf_decl_inline void *
		allocate(
		 size_t __s,
		 align_val_t __align,
		 size_t __o) pf_attr_noexcept
		{
			return mi_malloc_aligned_at(__s, union_cast<size_t>(__align), __o);
		}

		/// Reallocate
		pf_hint_nodiscard pf_decl_inline void *
		reallocate(
		 void *__p,
		 size_t __s,
		 align_val_t __align,
		 size_t __o) pf_attr_noexcept
		{
			return mi_realloc_aligned_at(__p, __s, union_cast<size_t>(__align), __o);
		}

		/// Deallocate
		pf_decl_inline void
		deallocate(
		 void *__p) pf_attr_noexcept
		{
			mi_free(__p);
		}
	};
}	 // namespace pul

#endif	// !PULSAR_ALLOCATOR_DEFAULT_HPP
