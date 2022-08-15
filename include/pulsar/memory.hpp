/*! @file   memory.hpp
 *  @author Fluffy (noe.louis-quentin@hotmail.fr)
 *  @brief	Defines the memory utilities such as memory functions, allocators and operators.
 *  @date   09-01-2022
 *
 *  @copyright Copyright (c) 2022 - Pulsar Software
 *
 *  @since 0.1.0
 */

#ifndef PULSAR_MEMORY_HPP
#define PULSAR_MEMORY_HPP 1

// Include: Pulsar -> Memory Utils
#include "pulsar/memory/memory_buffer.hpp"
#include "pulsar/memory/memory_utils.hpp"

// Include: Pulsar -> Memory Allocators
#include "pulsar/memory/allocator/allocator_linear.hpp"
#include "pulsar/memory/allocator/allocator_pool.hpp"
#include "pulsar/memory/allocator/allocator_ring.hpp"
#include "pulsar/memory/allocator/allocator_stack.hpp"

// Include: Pulsar -> Memory CDS Allocators
#include "pulsar/memory/cds_allocator/allocator_cds_linear.hpp"
#include "pulsar/memory/cds_allocator/allocator_cds_pool.hpp"
#include "pulsar/memory/cds_allocator/allocator_cds_ring.hpp"
#include "pulsar/memory/cds_allocator/allocator_cds_stack.hpp"

// Include: Pulsar -> Memory Allocator Wrapper
#include "pulsar/memory/allocator/allocator_wrapper.hpp"

#endif // !PULSAR_MEMORY_HPP