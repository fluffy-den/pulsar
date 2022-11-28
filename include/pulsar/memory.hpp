/*! @file   memory.hpp
 *  @author Fluffy (noe.louis-quentin@hotmail.fr)
 *  @brief	Includes memory headers.
 *  @date   09-01-2022
 *
 *  @copyright Copyright (c) 2022 - Pulsar Software
 *
 *  @since 0.1.0
 */

#ifndef PULSAR_MEMORY_HPP
#define PULSAR_MEMORY_HPP 1

// Include: C
#include <cstring>// memcpy, memmove...

// Include: MiMalloc
#include "mimalloc.h"

// Include: Pulsar -> Memory
#include "pulsar/memory/utility.hpp"
#include "pulsar/memory/allocator.hpp"
#include "pulsar/memory/unique_ptr.hpp"
#include "pulsar/memory/shared_ptr.hpp"

#endif // !PULSAR_MEMORY_HPP