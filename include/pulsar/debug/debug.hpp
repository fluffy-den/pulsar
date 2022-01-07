/**
 * @file    debug.hpp
 * @author  Fluffy (noe.louis-quentin@hotmail.fr)
 * @brief
 * @date    01-01-2022
 *
 * @copyright Copyright (c) 2022 - Pulsar Software
 *
 */

#ifndef PULSAR_DEBUG_HPP
#define PULSAR_DEBUG_HPP 1

// Include: Pulsar
#include "pulsar/pulsar.hpp"
#include "pulsar/utils/string.hpp"

// Include: C
#include <cassert>

// Pulsar
namespace pulsar
{
	/**
	 * @category Debugging
	 */

	// Debug
#ifdef _DEBUG
#	define pf_debug 1
#else
#	define pf_debug 0
#endif // _DEBUG

	// Debug: Static Assert
#define pf_static_assert(val, message) static_assert(val, message)

	// Debug: Assert
#define pf_assert(val, message)				 assert(val, message)

} // Pulsar

#endif // !PULSAR_DEBUG_HPP