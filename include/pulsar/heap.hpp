/*! @file   heap.hpp
 *  @author Louis-Quentin Noé (noe.louis-quentin@hotmail.fr)
 *  @brief
 *  @date   05-03-2023
 *
 *  @copyright Copyright (c) 2023 - Pulsar Software
 *
 *  @since 0.1.4
 */

#ifndef PULSAR_HEAP_HPP
#define PULSAR_HEAP_HPP 1

// Include: Pulsar
#include "pulsar/pulsar.hpp"
#include "pulsar/utility.hpp"

// Include: MiMalloc
#include <mimalloc.h>

// Include: C
#include <cstring>// memcpy, memmove...

// Pulsar
namespace pul
{
	/// HEAP: Constants
	pf_decl_constexpr align_val_t ALIGN_DEFAULT = align_val_t(alignof(void*));
	pf_decl_constexpr align_val_t ALIGN_MAX			= align_val_t(1024);

	/// HEAP: Allocate
	pf_hint_nodiscard pf_decl_inline pf_decl_constexpr void*
	heap_allocate(
		size_t __size,
		align_val_t __align = ALIGN_DEFAULT,
		size_t __offset			= 0) pf_attr_noexcept
	{
		if(std::is_constant_evaluated())
		{
			return new byte_t[__size];
		}
		else
		{
			return mi_malloc_aligned_at(
				__size,
				union_cast<size_t>(__align),
				__offset);
		}
	}

	/// HEAP: Reallocate
	pf_decl_inline pf_decl_constexpr void*
	heap_reallocate(
		void *__ptr,
		size_t __osize,
		size_t __nsize,
		align_val_t __nalign = ALIGN_DEFAULT,
		size_t __noffset		 = 0) pf_attr_noexcept
	{
		if (std::is_constant_evaluated())
		{
			byte_t *p = new byte_t[__nsize];
			if (!__ptr) return p;
			const size_t m = std::min(__osize, __nsize);
			std::memcpy(p, __ptr, m);
			delete[] union_cast<byte_t*>(__ptr);
			return p;
		}
		else
		{
			return mi_realloc_aligned_at(__ptr, __nsize, union_cast<size_t>(__nalign), __noffset);
		}
	}

	/// HEAP: Deallocate
	pf_decl_inline pf_decl_constexpr void
	heap_deallocate(
		void *__ptr) pf_attr_noexcept
	{
		if (std::is_constant_evaluated())
		{
			delete[] union_cast<byte_t*>(__ptr);
		}
		else
		{
			mi_free(__ptr);
		}
	}

	/// HEAP: Usable Size
	pf_decl_inline size_t
	heap_usable_size(
		void *__ptr) pf_attr_noexcept
	{
		return mi_usable_size(__ptr);
	}

	/// HEAP: Good Size
	pf_decl_inline size_t
	heap_good_size(
		size_t __size)
	{
		return mi_good_size(__size);
	}
}

#endif // !PULSAR_HEAP_HPP