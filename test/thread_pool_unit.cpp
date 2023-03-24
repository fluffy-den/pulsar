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
		pt_unit(task_submit0)
		{
			submit_task_0([](size_t __i){ return __i;}, 10);
		}
		pt_unit(future_task_submit)
		{
			auto f = submit_future_task([](size_t __i){ return __i;}, 10);
			f.wait();
			auto r = f.value();
		}
		pt_unit(future_task_submit_0)
		{
			auto f = submit_future_task_0([](size_t __i){ return __i;}, 10);
			process_0();
			f.wait();
			auto r = f.value();
		}
	}
}