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
	auto sL = pul::debug_stacktrace();

	std::printf("Test: Debug StackTrace\n");
	size_t k = 0;
	for (auto &i : sL)
	{
		std::printf(
				"[%zu] undName=%s, name=%s, filename=%s, modulename=%s, fileline=%zu\n",
				k,
				i.undname.c_str(),
				i.name.c_str(),
				i.filename.c_str(),
				i.modulename.c_str(),
				i.fileline);
		++k;
	}

	// Success
	return 0;
}