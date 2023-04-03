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
		size_t __offset) pf_attr_noexcept
	{
		return __internal.cache.allocate(__size, __align, __offset);
	}
	void
	cfree(
		void *__ptr) pf_attr_noexcept
	{
		return __internal.cache.deallocate(__ptr);
	}
}