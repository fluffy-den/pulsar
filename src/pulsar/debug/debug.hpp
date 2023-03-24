/*! @file   debug.hpp
 *  @author Louis-Quentin Noé (noe.louis-quentin@hotmail.fr)
 *  @brief
 *  @date   19-03-2023
 *
 *  @copyright Copyright (c) 2023 - Pulsar Software
 *
 *  @since 0.1.5
 */

// Include: Pulsar
#include "pulsar/debug.hpp"
#include "pulsar/allocator.hpp"
#include "pulsar/concurrency.hpp"

#ifndef PULSAR_SRC_DEBUG_HPP
#define PULSAR_SRC_DEBUG_HPP 1

// Pulsar
namespace pul
{
	/// DEBUG: Internal Allocator -> Constant
	pf_decl_constexpr size_t DBG_ALLOCATOR_SIZE = 65536;

	/// DEBUG: Internal Allocator -> Instance
	pf_decl_extern allocator_mamd_ring_buffer __dbg_internal_allocator;
}

#endif // !PULSAR_SRC_DEBUG_HPP