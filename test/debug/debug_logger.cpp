/*! @file   debug_logger.cpp
 *  @author Fluffy (noe.louis-quentin@hotmail.fr)
 *  @brief  Test the debug_logger function.
 *  @date   21-02-2022
 *
 *  @copyright Copyright (c) 2022 - Pulsar Software
 *
 *  @since 0.1.0
 */

// Include: Pulsar
#include "pulsar/debug.hpp"

// Main: Test
pul::int32_t main()
{
	// Test#1 -> Info write
	pul::debug_logger::write(
			pul::debug_level::info,
			pul::debug_filter::important,
			"This is an info message...");

	// Test#2 -> Warning write
	pul::debug_logger::write(
			pul::debug_level::warning,
			pul::debug_filter::important,
			"This is a warning message...");

	// Test#3 -> Error write
	pul::debug_logger::write(
			pul::debug_level::error,
			pul::debug_filter::important,
			"This is an error message...");

	// Success
	return 0;
}