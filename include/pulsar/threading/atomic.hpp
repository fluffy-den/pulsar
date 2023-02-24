/*! @file   atomic.hpp
 *  @author Louis-Quentin Noé (noe.louis-quentin@hotmail.fr)
 *  @brief
 *  @date   15-01-2023
 *
 *  @copyright Copyright (c) 2023 - Pulsar Software
 *
 *  @since 0.1.3
 */

#ifndef PULSAR_ATOMIC_HPP
#define PULSAR_ATOMIC_HPP 1

// Include: Pulsar
#include "pulsar/pulsar.hpp"

// Include: C++
#include <atomic>

// Pulsar
namespace pul
{
	/// ATOMIC:
	template <typename _Ty>
	using atomic			 = std::atomic<_Ty>;
	using atomic_flag	 = std::atomic_flag;
	using atomic_order = std::memory_order;
}

#endif // !PULSAR_ATOMIC_HPP