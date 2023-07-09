/*! @file   cache.cpp
 *  @author Louis-Quentin Noé (noe.louis-quentin@hotmail.fr)
 *  @brief
 *  @date   31-03-2023
 *
 *  @copyright Copyright (c) 2023 - Pulsar Software
 *
 *  @since 0.1.5
 */

// Include: Pulsar
#include "pulsar/internal.hpp"

// Pulsar
namespace pul
{
	/// MALLOC: Heap
	pulsar_api void *
	__halloc(
	 size_t __size,
	 align_val_t __align,
	 size_t __offset)
	{
		void *p = mi_malloc_aligned_at(
		 __size,
		 union_cast<size_t>(__align),
		 __offset);
		pf_throw_if(
		 !p,
		 dbg_category_generic(),
		 dbg_code::bad_alloc,
		 dbg_flags::dump_with_handle_data,
		 "Failed to allocate heap of size={}, align={}, offset={}",
		 __size,
		 union_cast<size_t>(__align),
		 __offset);
		return p;
	}
	pf_decl_inline pulsar_api void
	__hfree(
	 void *__ptr) pf_attr_noexcept
	{
		mi_free(__ptr);
	}
	pf_decl_inline pulsar_api void *
	__hrealloc(
	 void *__ptr,
	 size_t __nsize,
	 align_val_t __nalign,
	 size_t __noffset)
	{
		void *p = mi_realloc_aligned_at(__ptr, __nsize, union_cast<size_t>(__nalign), __noffset);
		pf_throw_if(
		 !p && !__nsize,
		 dbg_category_generic(),
		 dbg_code::bad_alloc,
		 dbg_flags::dump_with_data_segs | dbg_flags::dump_with_handle_data,
		 "Failed to reallocate heap at ptr={}, nsize={}, nalign={}, noffset={}",
		 __ptr,
		 __nsize,
		 union_cast<size_t>(__nalign),
		 __noffset);
		return p;
	}

	/// MALLOC: Cache
	pulsar_api void *
	__calloc(
	 size_t __size,
	 align_val_t __align,
	 size_t __offset)
	{
		void *p = __internal.cmem.allocate(__size, __align, __offset);
		pf_throw_if(
		 !p,
		 dbg_category_generic(),
		 dbg_code::bad_alloc,
		 dbg_flags::dump_with_data_segs | dbg_flags::dump_with_handle_data,
		 "Failed to allocate cache of size={}, align={}, offset={}",
		 __size,
		 union_cast<size_t>(__align),
		 __offset);
		return p;
	}
	pulsar_api void *
	__crealloc(
	 void *__ptr,
	 size_t __size,
	 align_val_t __align,
	 size_t __offset)
	{
		if(pf_likely(__ptr)) __internal.cmem.deallocate(__ptr);
		return __internal.cmem.allocate(__size, __align, __offset);
	}
	pulsar_api void
	__cfree(
	 void *__ptr) pf_attr_noexcept
	{
		if(pf_likely(__ptr)) __internal.cmem.deallocate(__ptr);
	}

	/// MALLOC: Stack
	pulsar_api void *
	__salloc(
	 size_t __size,
	 align_val_t __align,
	 size_t __offset)
	{
		void *p = __internal.smem.allocate(__size, __align, __offset);
		pf_throw_if(
		 !p,
		 dbg_category_generic(),
		 dbg_code::bad_alloc,
		 dbg_flags::dump_with_data_segs | dbg_flags::dump_with_handle_data,
		 "Failed to allocate stack of size={}, align={}, offset={}",
		 __size,
		 union_cast<size_t>(__align),
		 __offset);
		return p;
	}
	pulsar_api void *
	__srealloc(
	 void *__ptr,
	 size_t __size,
	 align_val_t __align,
	 size_t __offset)
	{
		if(pf_likely(__ptr)) __internal.smem.deallocate(__ptr);
		return __internal.smem.allocate(__size, __align, __offset);
	}
	pulsar_api void
	__sfree(
	 void *__ptr) pf_attr_noexcept
	{
		if(pf_likely(__ptr)) __internal.smem.deallocate_and_purge(__ptr);
	}


}	 // namespace pul
