/*! @file   concurrency_unit.cpp
 *  @author Louis-Quentin Noé (noe.louis-quentin@hotmail.fr)
 *  @brief
 *  @date   10-03-2023
 *
 *  @copyright Copyright (c) 2023 - Pulsar Software
 *
 *  @since 0.1.4
 */

// Include: Pulsar
#include "pulsar/pulsar.hpp"
#include "pulsar/allocator.hpp"

// Include: Pulsar -> Tester
#include "pulsar_tester/pulsar_tester.hpp"

// Pulsar
namespace pul
{
	// SAMD Ring Allocator
	pt_pack(samd_ring_allocator)
	{
		// Unit
		pt_unit(samd_ring_allocator_test)
		{
			samd_ring_allocator<magnifier_quadratic> all(65536);
			for (size_t i = 0; i < 128; ++i)
			{
				void *p1 = all.allocate(4096);
				void *p2 = all.allocate(4096);
				void *p3 = all.allocate(4096);
				void *p4 = all.allocate(4096);
				all.deallocate(p1);
				all.deallocate(p2);
				all.deallocate(p3);
				all.deallocate(p4);
			}

		}

		// Benchmarks
		pt_benchmark(samd_ring_allocator_benchmark_t1, __bvn, 65536, 1)
		{

		}
		pt_benchmark(samd_ring_allocator_benchmark_t4, __bvn, 65536, 4)
		{

		}
		pt_benchmark(samd_ring_allocator_benchmark_t8, __bvn, 65536, 8)
		{

		}
	}

	// MPSC Queue
	// TODO

	// SPMC Queue
	// TODO

	// MPSC Singly List
	// TODO


}
