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
			samd_ring_allocator<magnifier_quadratic> all(1 * 65536 * (8 + sizeof(__samd_ring_buffer_header_t)));
			void **buf = union_cast<void**>(heap_allocate(65536 * 256 * sizeof(void*), align_val_t(8)));
			for (size_t i = 0; i < 65536 * 1; ++i)
			{
				buf[i] = all.allocate(256);
			}
			__bvn.measure([&](size_t __index)
			{
				all.deallocate(buf[__index]);
				return __index;
			});
			heap_deallocate(buf);
		}
		// pt_benchmark(samd_ring_allocator_t2, __bvn, 65536, 2)
		// {
		// 	samd_ring_allocator<magnifier_quadratic> all(2 * 65536 * (8 + sizeof(__samd_ring_buffer_header_t)));
		// 	void **buf = union_cast<void**>(heap_allocate(65536 * 2 * sizeof(void*), align_val_t(32)));
		// 	for (size_t i = 0; i < 65536 * 2; ++i)
		// 	{
		// 		buf[i] = all.allocate(8, align_val_t(16));
		// 	}
		// 	__bvn.measure([&](size_t __index)
		// 	{
		// 		all.deallocate(buf[__index]);
		// 		return buf[__index];
		// 	});
		// 	heap_deallocate(buf);
		// }
		// pt_benchmark(mimalloc_allocator_t1, __bvn, 65536, 1)
		// {
		// 	__bvn.measure([&](size_t __index)
		// 	{
		// 		void *a = heap_allocate(
		// 			8, align_val_t(16));
		// 		heap_deallocate(a);
		// 		return a;
		// 	});
		// }
		// pt_benchmark(mimalloc_allocator_t2, __bvn, 65536, 2)
		// {
		// 	__bvn.measure([&](size_t __index)
		// 	{
		// 		void *a = heap_allocate(
		// 			8, align_val_t(16));
		// 		heap_deallocate(a);
		// 		return a;
		// 	});
		// }
	}

	// MPSC Queue
	// TODO CAS vs Load + Store using fetch_add
	// TODO

	// SPMC Queue
	// TODO

	// MPSC Unbuffered Queue
	// TODO


}
