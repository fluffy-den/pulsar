/*! @file   atomic_queue.hpp
 *  @author Louis-Quentin Noé (noe.louis-quentin@hotmail.fr)
 *  @brief
 *  @date   02-01-2023
 *
 *  @copyright Copyright (c) 2023 - Pulsar Software
 *
 *  @since 0.1.3
 */

#ifndef PULSAR_ATOMIC_QUEUE_HPP
#define PULSAR_ATOMIC_QUEUE_HPP 1

// Include: Pulsar
#include "pulsar/pulsar.hpp"

// Pulsar
namespace pul
{
	/*
	 * !              !
	 *   Atomic Queue
	 * !              !
	 */
	// https://github.com/cameron314/concurrentqueue
	// https://github.com/cameron314/readerwriterqueue

	/// CDS: SCMP Queue
	class atomic_scmp_queue
	{};

	/// CDS: MCMP Queue
	class atomic_mcmp_queue
	{};

}

#endif // !PULSAR_ATOMIC_QUEUE_HPP