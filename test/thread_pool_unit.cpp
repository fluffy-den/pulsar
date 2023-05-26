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
	// Thread Pool but for Paien
	pt_pack(thread_pool_but_for_paien)
	{
		// Task
		pt_unit(task_he_dont_understand_that_a_tool)
		{
			for(size_t i = 0; i < 10'000; ++i)
			{
				submit_task(
				 []()
				 {
					pf_print("THREAD POOL IS A TOOL! THREAD ID =", this_thread::get_id());
				});
			}
		}
	}

	// Thread Pool
	pt_pack(thread_pool)
	{
		// Task
		pt_unit(task_submit)
		{
			submit_task(
			 [](size_t __i)
			 { return __i; },
			 10);
		}
		pt_unit(task_submit_0)
		{
			submit_task_0(
			 [](size_t __i)
			 { return __i; },
			 10);
		}

		pt_benchmark(task_submit_benchmark_t1, __bvn, 2'048, 1)
		{
			__bvn.measure(
			 [&](size_t __i)
			 {
				submit_task([](size_t __k)
										{ return __k; },
										__i);
				return __i;
			});
		}
		pt_benchmark(task_submit_benchmark_t8, __bvn, 2'048, 8)
		{
			__bvn.measure(
			 [&](size_t __i)
			 {
				submit_task([](size_t __k)
										{ return __k; },
										__i);
				return __i;
			});
		}
		pt_unit(future_task_submit)
		{
			auto f = submit_future_task([](size_t __i)
																	{ return __i; },
																	10);
			f.wait();
		}
		pt_unit(future_task_submit_0)
		{
			auto f = submit_future_task_0([](size_t __i)
																		{ return __i; },
																		10);
			process_tasks_0();
			f.wait();
		}

		pt_unit(task_pool)
		{
			task_pool_t pool;
			pool.submit_task([](size_t __i)
											 { return __i; },
											 10);
			auto f1 = pool.submit_future_task([]()
																				{ return this_thread::get_idx(); });
			auto f2 = pool.submit_future_task([]()
																				{ return this_thread::get_idx(); });
		}
		pt_unit(task_pool_0)
		{
			task_pool_0_t pool;
			pool.submit_task_0([](size_t __i)
												 { return __i; },
												 10);
			auto f1 = pool.submit_future_task_0([]()
																					{ return this_thread::get_idx(); });
			auto f2 = pool.submit_future_task_0([]()
																					{ return this_thread::get_idx(); });
		}

		pt_unit(task_throw)
		{
			auto f1 = submit_future_task(
			 []()
			 { pf_throw(dbg_category_generic(), dbg_code::invalid_argument, dbg_flags::none, "A C++ exception from thread={}!", this_thread::get_id()); });
			try
			{
				f1.check_exception();
			} catch(const dbg_exception &__e)
			{
				pt_check(__e.category() == dbg_category_generic());
			} catch(std::exception const &__e)
			{
				pt_check(false);
			}
		}
		pt_unit(task_throw_0)
		{
			auto f1 = submit_future_task_0(
			 []()
			 { pf_throw(dbg_category_generic(), dbg_code::invalid_argument, dbg_flags::none, "A C++ exception from thread={}!", this_thread::get_id()); });
			try
			{
				f1.check_exception();
			} catch(const dbg_exception &__e)
			{
				pt_check(__e.category() == dbg_category_generic());
			} catch(std::exception const &__e)
			{
				pt_check(false);
			}
		}
	}
}	 // namespace pul
