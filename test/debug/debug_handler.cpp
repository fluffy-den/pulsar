/*! @file   debug_handler.cpp
 *  @author Fluffy (noe.louis-quentin@hotmail.fr)
 *  @brief  Test the debugging terminate handler.
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
	// Fake exception
	throw pul::exception(std::system_category(), 0, "This is fake error.");

	// Success
	return 0;
}