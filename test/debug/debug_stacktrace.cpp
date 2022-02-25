/*! @file   debug_trace.cpp
 *  @author Fluffy (noe.louis-quentin@hotmail.fr)
 *  @brief  Test the debug_stacktrace function.
 *  @date   20-02-2022
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
	const auto start = std::chrono::high_resolution_clock::now();
	while (std::chrono::high_resolution_clock::now() - start < std::chrono::seconds(10))
	{
		pul::dbgstacktrace(0);
	}

	// Success
	return 0;
}