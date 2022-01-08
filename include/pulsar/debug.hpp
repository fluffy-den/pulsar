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
#include "pulsar/utils/string_format.hpp"

// Include: C
#include <cassert>

// Include: C++
#include <source_location>
#include <system_error>

// Pulsar
namespace pulsar
{
	/// Macro
#ifdef _DEBUG
#	define pf_debug 1
#else
#	define pf_debug 0
#endif // _DEBUG

	/// Static Assert
#define pf_static_assert(val, message) static_assert(val, message)

	/// Assert
#define pf_assert(val, message)				 assert(val, message)

	/// Types
	using error_code		 = std::error_code;
	using error_category = std::error_category;
} // Pulsar

#endif // !PULSAR_DEBUG_HPP