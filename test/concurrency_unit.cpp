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
		pt_benchmark(samd_ring_allocator_t1, __bvn, 65536, 1)
		{
			samd_ring_allocator<magnifier_quadratic> all(2 * 65536 * (8 + sizeof(__samd_ring_buffer_header_t)));
			__bvn.measure([&](size_t __index)
			{
				void *a = all.allocate(
					8, align_val_t(16));
				all.deallocate(a);
				return a;
			});
		}
		pt_benchmark(mimalloc_allocator_t1, __bvn, 65536, 1)
		{
			__bvn.measure([&](size_t __index)
			{
				void *a = heap_allocate(
					8, align_val_t(16));
				heap_deallocate(a);
				return a;
			});
		}
	}

	// MPSC Queue
	// TODO

	// SPMC Queue
	// TODO

	// MPSC Unbuffered Queue
	// TODO


}
