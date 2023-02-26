/*! @file   debug_unit.cpp
 *  @author Louis-Quentin Noé (noe.louis-quentin@hotmail.fr)
 *  @brief
 *  @date   25-02-2023
 *
 *  @copyright Copyright (c) 2023 - Pulsar Software
 *
 *  @since 0.1.4
 */

// Include: Pulsar
#include "pulsar/debug.hpp"

// Include: Pulsar -> Tester
#include "pulsar_tester/pulsar_tester.hpp"

// Pulsar
namespace pul
{
	// Debug -> Logger
	pt_pack(logger)
	pt_unit(writing)
	{
		int32_t i = 0;
		throw(std::runtime_error("Blah"));
		pt_require(i > 0);
	}
	pt_benchmark(writing_bench, __b, 1024)
	{


	}
}