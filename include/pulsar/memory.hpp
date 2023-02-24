/*! @file   memory.hpp
 *  @author Louis-Quentin Noé (noe.louis-quentin@hotmail.fr)
 *  @brief
 *  @date   19-02-2023
 *
 *  @copyright Copyright (c) 2023 - Pulsar Software
 *
 *  @since 0.1.4
 */

#ifndef PULSAR_MEMORY_HPP
#define PULSAR_MEMORY_HPP 1

// Include: Pulsar
#include "pulsar/pulsar.hpp"

// Include: C++
#include <concepts>

// Include: C
#include <cstring>// memcpy

// Include: MiMalloc
#include "mimalloc.h"

// Pulsar
namespace pul
{
	/// MEMORY: Constants
	pf_decl_constexpr align_val_t ALIGN_DEFAULT = align_val_t(alignof(void*));
	pf_decl_constexpr align_val_t ALIGN_MAX			= align_val_t(1024);
}

// Include: Pulsar -> Memory
#include "pulsar/memory/memory_allocator.hpp"
#include "pulsar/memory/memory_utility.hpp"

#endif // !PULSAR_MEMORY_HPP