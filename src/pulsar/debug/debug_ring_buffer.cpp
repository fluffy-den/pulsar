/*! @file   debug_ring_buffer.cpp
 *  @author Louis-Quentin Noé (noe.louis-quentin@hotmail.fr)
 *  @brief
 *  @date   04-03-2023
 *
 *  @copyright Copyright (c) 2023 - Pulsar Software
 *
 *  @since 0.1.3
 */

// Include: Pulsar
#include "pulsar/debug/debug_ring_buffer.hpp"

// Pulsar
namespace pul
{
	/// DEBUG: Ring Buffer -> Local Instance
	__dbg_ring_buffer_local_t __dbg_ring_buffer_local;

	/// DEBUG: Allocator
	pulsar_api void*
	allocate(
		size_t __size) pf_attr_noexcept
	{
		return __dbg_ring_buffer_local.allocate(__size);
	}
	pulsar_api void
	deallocate(
		void *__ptr) pf_attr_noexcept
	{
		__dbg_ring_buffer_local.deallocate(__ptr);
	}
}