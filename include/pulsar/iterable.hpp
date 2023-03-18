/*! @file   iterable.hpp
 *  @author Louis-Quentin Noé (noe.louis-quentin@hotmail.fr)
 *  @brief  Implementation of iterables.
 *  @date   31-12-2022
 *
 *  @copyright Copyright (c) 2023 - Pulsar Software
 *
 *  @since 0.1.4
 */

#ifndef PULSAR_ITERABLE_HPP
#define PULSAR_ITERABLE_HPP 1

// Include: Pulsar
#include "pulsar/pulsar.hpp"
#include "pulsar/memory.hpp"
#include "pulsar/iterator.hpp"
#include "pulsar/allocator.hpp"
#include "pulsar/algorithm.hpp"

// Include: Pulsar -> Iterable
#include "pulsar/iterable/array.hpp"
#include "pulsar/iterable/sequence.hpp"
#include "pulsar/iterable/mpmc_queue.hpp"
#include "pulsar/iterable/mpsc_singly_lifo.hpp"

#endif // !PULSAR_CONTAINER_HPP