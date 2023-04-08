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
	pf_decl_constexpr size_t DBG_UNIT_NUM_ITERATIONS = 32'768;

	/// DBG: Test -> Allocator
	pt_pack(dbg_allocator)
	{
		pt_benchmark(mimalloc_allocate_08t, __bvn, DBG_UNIT_NUM_ITERATIONS, 8)
		{
			const size_t n = __bvn.num_iterations();
			void **p			 = new_construct_array<void *>(n);
			__bvn.measure([&](size_t __i)
										{ return p[__i] = halloc(8); });
			for(size_t i = 0; i < n; ++i)
			{
				hfree(p[i]);
			}
			destroy_delete_array(p);
		}
		pt_benchmark(ring_buffer_08t, __bvn, DBG_UNIT_NUM_ITERATIONS, 8)
		{
			const size_t n = __bvn.num_iterations();
			void **p			 = new_construct_array<void *>(n);
			__bvn.measure([&](size_t __i)
										{ return p[__i] = calloc(8); });
			for(size_t i = 0; i < n; ++i)
			{
				cfree(p[i]);
			}
			destroy_delete_array(p);
		}
	}

	pt_pack(exception)
	{
		pt_unit(throwing)
		{
			pf_throw(dbg_category_generic(), dbg_code::invalid_argument, dbg_flags::none, "Throwed!");
		}
	}
}	 // namespace pul
