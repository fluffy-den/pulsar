/**
 * @file    string_format.cpp
 * @author  Fluffy (noe.louis-quentin@hotmail.fr)
 * @brief		Test of string format function
 * @date    05-01-2022
 *
 * @copyright Copyright (c) 2022 - Pulsar Software
 *
 */

// Include: Pulsar
#include "pulsar/utils/string_format.hpp"

// Include: C++
#include <iostream>

// Main: Test
pulsar::int32_t main()
{
	// #1.
	std::cout << pulsar::string_format(
			"The sum of %u and %u is equal to : %u\n",
			1,
			2,
			3);

	// #2.
	std::cout << pulsar::string_format(
			"Some different radices: %d %x %o %#x %#o \n",
			100,
			100,
			100,
			100,
			100);

	// #End
	return 0;
}