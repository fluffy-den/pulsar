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
	pt_benchmark(writing_bench, __b, 1024, 16)
	{
		__b.measure(std::function([&](size_t __i)
															{ return __i; }));
	}
}