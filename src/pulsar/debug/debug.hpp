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
#include "pulsar/thread_pool.hpp"

#ifndef PULSAR_SRC_DEBUG_HPP
#define PULSAR_SRC_DEBUG_HPP 1

// Pulsar
namespace pul
{
	/// DEBUG: Internal Allocator -> Constant
	pf_decl_constexpr size_t DBG_ALLOCATOR_SIZE = 65536;

	/// DEBUG: Internal Allocator -> Instance
	pf_decl_extern allocator_mamd_ring_buffer __dbg_internal_allocator;
	pf_decl_extern task_pool_t __dbg_logger_task_pool;
}

#endif // !PULSAR_SRC_DEBUG_HPP