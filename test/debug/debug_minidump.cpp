/*! @file   debug_minidump.cpp
 *  @author Fluffy (noe.louis-quentin@hotmail.fr)
 *  @brief  Test the debug_dumpbin function.
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
	auto dbp = pul::dbggenbin(
			std::filesystem::current_path(),
			pul::debug_flags::generate_dumpfile);
	if (dbp.empty())
		return -1;

	// Success
	return 0;
}