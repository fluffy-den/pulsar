/**
 * @file    atomic.hpp
 * @author  Fluffy (noe.louis-quentin@hotmail.fr)
 * @brief   Defines atomic objects from the standard libraries.
 * @date    08-01-2022
 *
 * @copyright Copyright (c) 2022 - Pulsar Software
 *
 */

#ifndef PULSAR_ATOMIC_HPP
#define PULSAR_ATOMIC_HPP 1

// Include: C++
#include <atomic>

// Pulsar
namespace pulsar
{
	/// Types
	template <typename _Ty>
	using atomic			 = std::atomic<_Ty>;
	using atomic_flag	 = std::atomic_flag;
	using memory_order = std::memory_order;
}

#endif // !PULSAR_ATOMIC_HPP