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
#include "pulsar/debug/debug_win.hpp"

// Include: Pulsar -> Tester
#include "pulsar_tester/pulsar_tester.hpp"

// Pulsar
namespace pul
{
	/// DBG: Test -> Constants
	pf_decl_constexpr size_t DBG_UNIT_NUM_ITERATIONS = 65536;

	/// DBG: Test -> Allocator
	pt_pack(dbg_allocator)
	{
		pt_unit(ring_buffer)
		{

		}
		pt_benchmark(ring_buffer_01t, __bvn, DBG_UNIT_NUM_ITERATIONS, 1)
		{

		}
		pt_benchmark(ring_buffer_08t, __bvn, DBG_UNIT_NUM_ITERATIONS, 8)
		{

		}
		pt_benchmark(ring_buffer_16t, __bvn, DBG_UNIT_NUM_ITERATIONS, 16)
		{

		}
	}

	/// DBG: Test -> Types
	pt_pack(dbg_types)
	{
		pt_unit(u8string_view)
		{

		}
		pt_unit(u8string)
		{

		}

		#ifdef PF_OS_WINDOWS
		pt_unit(wsstring_view)
		{

		}
		pt_unit(wsstring)
		{

		}
		#endif // PF_OS_WINDOWS
	}
	// TODO: u8string_view
	// TODO: u8string
	// TODO: Benchmark -> Ring Buffer
	// TODO: Benchmark -> Formatting Message

}