/*! @file   thread_pool_unit.cpp
 *  @author Louis-Quentin Noé (noe.louis-quentin@hotmail.fr)
 *  @brief
 *  @date   12-09-2022
 *
 *  @copyright Copyright (c) 2023 - Pulsar Software
 *
 *  @since 0.1.4
 */

// Include: Pulsar
#include "pulsar/thread_pool.hpp"

// Include: Pulsar -> Tester
#include "pulsar_tester/pulsar_tester.hpp"

// Pulsar
namespace pul
{
	// Thread Pool
	pt_pack(thread_pool)
	{
		// Task
		pt_unit(task_submit)
		{
			submit_task([](size_t __i){ return __i;}, 10);
		}
		pt_unit(task_submit_0)
		{
			submit_task_0([](size_t __i){ return __i;}, 10);
		}
		pt_benchmark(task_submit_benchmark_t1, __bvn, 1024, 1)
		{
			__bvn.measure([&](size_t __i)
			{
				submit_task([](size_t __k){ return __k;}, __i);
				return __i;
			});
		}
		// pt_benchmark(task_submit_benchmark_t8, __bvn, 4096, 8)
		// {
		// 	__bvn.measure([&](size_t __i)
		// 	{
		// 		submit_task([](size_t __k){ return __k;}, __i);
		// 		return __i;
		// 	});
		// }
		//
		// pt_unit(future_task_submit)
		// {
		// 	auto f = submit_future_task([](size_t __i){ return __i;}, 10);
		// 	f.wait();
		// }
		// pt_unit(future_task_submit_0)
		// {
		// 	auto f = submit_future_task_0([](size_t __i){ return __i;}, 10);
		// 	process_tasks_0();
		// 	f.wait();
		// }
		//
		// pt_unit(task_pool)
		// {
		// 	task_pool_t pool;
		// 	pool.submit_task([](size_t __i){ return __i; }, 10);
		// 	auto f1 = pool.submit_future_task([](){ return this_thread::get_id();});
		// 	auto f2 = pool.submit_future_task([](){ return this_thread::get_id();});
		// 	pt_check(f1.value() == f2.value());
		// }
		// pt_unit(task_pool_0)
		// {
		// 	task_pool_0_t pool;
		// 	pool.submit_task_0([](size_t __i){ return __i; }, 10);
		// 	auto f1 = pool.submit_future_task_0([](){ return this_thread::get_id();});
		// 	auto f2 = pool.submit_future_task_0([](){ return this_thread::get_id();});
		// 	pt_check(f1.value() == f2.value());
		// }
	}
}