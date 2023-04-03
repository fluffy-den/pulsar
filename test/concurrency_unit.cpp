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
#include "pulsar/iterable.hpp"

// Include: Pulsar -> Tester
#include "pulsar_tester/pulsar_tester.hpp"

// Pulsar
namespace pul
{
	// IDX benchmarks
	pt_pack(idx_pack)
	{
		pt_benchmark(idx_get_t1, __bvn, 16192, 1)
		{
			__bvn.measure([&](size_t __index)
			{
				ignore = __index;
				return this_thread::get_id();
			});
		}
		pt_benchmark(idx_get_t8, __bvn, 16192, 8)
		{
			__bvn.measure([&](size_t __index)
			{
				ignore = __index;
				return this_thread::get_id();
			});
		}
	}

	// MPMC Ring Allocator
	pt_pack(mpmc_ring_allocator)
	{
		pt_unit(alloc_free_loop_unit)
		{
			allocator_mamd_ring_buffer all(65536, 65536);
			for (size_t i = 0; i < 16; ++i)
			{
				void *buf[2048] = {nullptr};
				bool anynull		= false;
				for (size_t j = 0; j < 2048; ++j)
				{
					buf[j] = all.allocate(8);
					if (!buf[j]) anynull = true;
				}
				pt_check(anynull == false);
				for (size_t j = 0; j < 2048; ++j)
				{
					all.deallocate(buf[j]);
				}
			}
		}
	}

	// MPMC Queue2
	pt_pack(mpmc_lifo2_pack)
	{
		pt_benchmark(pop2_t8, __bvn, 16192, 8)
		{
			mpmc_lifo2<size_t> queue(16192 * 128);
			size_t *buf = new_construct_array<size_t>(__bvn.num_iterations());
			for (size_t i = 0; i < __bvn.num_iterations(); ++i)
			{
				buf[i] = i;
				queue.try_enqueue(&buf[i]);
			}
			__bvn.measure([&](size_t __index)
			{
				ignore = __index;
				return queue.try_dequeue();
			});
			destroy_delete_array(buf);
		}
		pt_benchmark(push2_t8, __bvn, 16192, 8)
		{
			mpmc_lifo2<size_t> queue(16192 * 128);
			size_t *buf = new_construct_array<size_t>(__bvn.num_iterations());
			for (size_t i = 0; i < __bvn.num_iterations(); ++i)
			{
				buf[i] = i;
			}
			__bvn.measure([&](size_t __index)
			{
				queue.try_enqueue(&buf[__index]);
				return __index;
			});
			destroy_delete_array(buf);
		}
		pt_benchmark(pop2_empty_t8, __bvn, 16192, 8)
		{
			mpmc_lifo2<size_t> queue(16192 * 128);
			__bvn.measure([&](size_t __index)
			{
				ignore = __index;
				return queue.try_dequeue();
			});
		}
	}
}
