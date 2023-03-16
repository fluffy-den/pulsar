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
			allocator_samd_ring_buffer all(65536);
			for (size_t i = 0; i < 128; ++i)
			{
				void *p1 = all.allocate(4096);
				pt_require(p1);
				void *p2 = all.allocate(4096);
				pt_require(p2 && p2 != p1);
				void *p3 = all.allocate(4096);
				pt_require(p3 && p3 != p2);
				void *p4 = all.allocate(4096);
				pt_require(p4 && p4 != p3);
				all.deallocate(p1);
				all.deallocate(p2);
				all.deallocate(p3);
				all.deallocate(p4);
			}
		}

		// Benchmarks
		pt_benchmark(samd_ring_deal_t1, __bvn, 262144, 1)
		{
			allocator_samd_ring_buffer all(2ull * 262144 * (64 + 8));
			void **buf = union_cast<void**>(halloc(262144 * 1 * sizeof(void*), align_val_t(8)));
			for (size_t i = 0; i < 262144 * 1; ++i)
			{
				buf[i] = all.allocate(64);
			}
			__bvn.measure([&](size_t __index)
			{
				all.deallocate(buf[__index]);
				return __index;
			});
			hfree(buf);
		}
		pt_benchmark(samd_ring_all_and_deal_t1, __bvn, 262144, 1)
		{
			allocator_samd_ring_buffer all(2ull * 262144 * (64 + 8));
			__bvn.measure([&](size_t __index)
			{
				void *a = all.allocate(64);
				all.deallocate(a);
				return a;
			});
		}
		pt_benchmark(samd_ring_deal_t2, __bvn, 262144, 2)
		{
			allocator_samd_ring_buffer all(4ull * 262144 * (64 + 8));
			void **buf = union_cast<void**>(halloc(262144 * 2 * sizeof(void*), align_val_t(32)));
			for (size_t i = 0; i < 262144 * 2; ++i)
			{
				buf[i] = all.allocate(64);
			}
			__bvn.measure([&](size_t __index)
			{
				all.deallocate(buf[__index]);
				return buf[__index];
			});
			hfree(buf);
		}
		pt_benchmark(samd_ring_deal_t8, __bvn, 262144, 8)
		{
			allocator_samd_ring_buffer all(16ull * 262144 * (64 + 8));
			void **buf = union_cast<void**>(halloc(262144 * 8 * sizeof(void*), align_val_t(32)));
			for (size_t i = 0; i < 262144 * 8; ++i)
			{
				buf[i] = all.allocate(64);
			}
			__bvn.measure([&](size_t __index)
			{
				all.deallocate(buf[__index]);
				return buf[__index];
			});
			hfree(buf);
		}
		pt_benchmark(mimalloc_all_and_deal_t1, __bvn, 262144, 1)
		{
			__bvn.measure([&](size_t __index)
			{
				void *a = halloc(64);
				hfree(a);
				return a;
			});
		}
		pt_benchmark(mimalloc_all_and_deal_t2, __bvn, 262144, 2)
		{
			__bvn.measure([&](size_t __index)
			{
				void *a = halloc(64);
				hfree(a);
				return a;
			});
		}
		pt_benchmark(mimalloc_all_and_deal_t8, __bvn, 262144, 8)
		{
			__bvn.measure([&](size_t __index)
			{
				void *a = halloc(64);
				hfree(a);
				return a;
			});
		}
	}

	// MPSC Queue
	// TODO CAS vs Load + Store using fetch_add
	// TODO

	// SPMC Queue
	// TODO

	// MPSC Unbuffered Queue
	// TODO


}
