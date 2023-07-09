/*! @file   __internal.hpp
 *  @author Louis-Quentin Noé (noe.louis-quentin@hotmail.fr)
 *  @brief
 *  @date   26-03-2023
 *
 *  @copyright Copyright (c) 2023 - Pulsar Software
 *
 *  @since 0.1.5
 */

#ifndef PULSAR_SRC_INTERNAL_HPP
#define PULSAR_SRC_INTERNAL_HPP 1

// Include: Pulsar
#include "pulsar/pulsar.hpp"
#include "pulsar/debug.hpp"
#include "pulsar/memory.hpp"
#include "pulsar/algorithm.hpp"
#include "pulsar/iterator.hpp"
#include "pulsar/iterable.hpp"
#include "pulsar/allocator.hpp"
#include "pulsar/utility.hpp"
#include "pulsar/char.hpp"
#include "pulsar/concurrency.hpp"
#include "pulsar/thread_pool.hpp"
#include "pulsar/filesystem.hpp"

// Include: Pulsar -> Src
#include "pulsar/internal_allocator.hpp"

// Include: Pulsar -> Src -> Debug
#include "pulsar/debug/debug.hpp"

// Include: Pulsar -> Src -> Thread Pool
#include "pulsar/concurrency/thread_pool.hpp"

// Pulsar
namespace pul
{
	/// INTERNAL:
	/// Type
	struct __internal_t
	{
		/// Constructors
		__internal_t();
		__internal_t(__internal_t const &) = delete;
		__internal_t(__internal_t &&)			 = delete;

		/// Destructor
		~__internal_t() pf_attr_noexcept = default;

		/// Operator =
		__internal_t &
		operator=(__internal_t const &) = delete;
		__internal_t &
		operator=(__internal_t &&) = delete;

		/// Module -> Local Allocators
		allocator_mamd_ring_buffer cmem;
		allocator_mamd_stack_buffer<magnifier_linear> smem;

		/// Module -> Debug
		__dbg_internal_t dbg_internal;
		__dbg_logger_t dbg_logger;

		/// Module -> Thread Pool
		__thread_pool_t thread_pool;

		/// Module -> IO Async Controller
		task_pool_t io_async_controller;
	};

	pf_decl_extern __internal_t __internal;
}	 // namespace pul

#endif	// !PULSAR_SRC_INTERNAL_HPP
