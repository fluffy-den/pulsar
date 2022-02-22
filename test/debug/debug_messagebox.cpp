/*! @file   debug_messagebox.cpp
 *  @author Fluffy (noe.louis-quentin@hotmail.fr)
 *  @brief  Test the debug_messagebox function.
 *  @date   21-02-2022
 *
 *  @copyright Copyright (c) 2022 - Pulsar Software
 *
 *  @since 0.1.0
 */

// Include: Pulsar
#include "pulsar/debug.hpp"

// Include: C
#include <cstdio>

// Main: Test
pul::int32_t main()
{
	// Test#1 -> Info message box
	pul::debug_messagebox(
			pul::debug_level::info,
			"Info message box",
			"This is an info message box.");

	// Test#2 -> Warning message box
	pul::debug_messagebox(
			pul::debug_level::warning,
			"Warning message box",
			"This is a warning message box.");

	// Test#3 -> Error message box
	pul::debug_messagebox(
			pul::debug_level::error,
			"Error message box",
			"This is an error message box.");

	// Success
	return 0;
}