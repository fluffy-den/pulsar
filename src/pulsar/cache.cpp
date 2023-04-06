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
	/// CACHE: Alloc / Free
	void*
	calloc(
		size_t __size,
		align_val_t __align,
		size_t __offset)
	{
		void *p = __internal.cache.allocate(__size, __align, __offset);
		if (pf_unlikely(!p))
			pf_throw(dbg_category_generic(), dbg_code::bad_alloc, dbg_flags::dump_with_data_segs | dbg_flags::dump_with_handle_data, "No more cache!");
		return p;
	}
	void
	cfree(
		void *__ptr) pf_attr_noexcept
	{
		return __internal.cache.deallocate(__ptr);
	}
}