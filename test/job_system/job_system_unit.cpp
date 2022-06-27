/*! @file   job_system_unit.cpp
 *  @author Fluffy (noe.louis-quentin@hotmail.fr)
 *  @brief
 *  @date   13-06-2022
 *
 *  @copyright Copyright (c) 2022 - Pulsar Software
 *
 *  @since 0.1.2
 */

// Include: Pulsar
#include "pulsar/thread.hpp"

// Include: Catch2
#include "catch2/catch_all.hpp"

// Include: C++
#include <random>

// Include: C
#include <cstdio>
#include <iostream>

// REMARK: Fails in debug mode / Work fine in release!

// Pulsar
namespace pul
{
	/// JOB-SYSTEM: Steal tests
	TEST_CASE("JobSystemStealTest2")
	{
		fun_ptr fun1 = [](void *) -> void {};
		for (size_t i = 0; i < 1024; ++i)
		{
			job_system::submit({ fun1 });
		}
		job_system::process_with_workers();
		job_system::process_0();
	}
	TEST_CASE("JobSystemStealTest3")
	{
		job_future<void> fut1;
		fun_ptr fun1 = [](void *) -> void {};
		job_system::submit(fut1, { fun1 });
		job_system::process_0();
		job_system::process_with_workers();
	}
	TEST_CASE("JobSystemStealTest4")
	{
		job j;
		fun_ptr fun1 = [](void *) -> void {};
		for (size_t i = 0; i < 1024; ++i)
		{
			job_system::submit(j, { { fun1 } });
		}
		while (!j->future().is_finished())
		{
			job_system::process_0();
			job_system::process_with_workers();
		}
	}
	TEST_CASE("JobSystemStealTest5")
	{
		fun_ptr fun1 = [](void *) -> void {};
		job_future<void> f1, f2;
		job_system::submit({
				{f1,	fun1, nullptr},
				{ f2, fun1, nullptr}
		});
		while (!f1.is_finished() || !f2.is_finished())
		{
			job_system::process_0();
			job_system::process_with_workers();
		}
	}
	TEST_CASE("JobSystemStealTest6")
	{
		fun_ptr fun1 = [](int32_t a, int32_t b) -> int32_t
		{
			return a + b;
		};
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<int32_t> r(
				std::numeric_limits<int32_t>::min(),
				std::numeric_limits<int32_t>::max());
		job_future<int32_t> f1, f2;
		int32_t a = r(gen), b = r(gen), c = r(gen), d = r(gen);
		job_system::submit({
				{f1,	fun1, a, b},
				{ f2, fun1, c, d}
		 });
		while (!f1.is_finished() || !f2.is_finished())
		{
			job_system::process_0();
			job_system::process_with_workers();
		}
		REQUIRE(f1.value() == a + b);
		REQUIRE(f2.value() == c + d);
	}

	/// JOB-SYSTEM: Main tests
	TEST_CASE("JobSystemMainTest1")
	{
		fun_ptr fun1 = [](void *) -> void {};
		fun_ptr fun2 = [](void *) -> void {};
		job_system::submit_0({ { fun1 }, { fun2 } });
		job_system::process_0();
	}
	TEST_CASE("JobSystemMainTest2")
	{
		fun_ptr fun1 = [](void *) -> void {};
		job_future<void> fut1;
		job_system::submit_0(fut1, { fun1 });
		job_system::process_0();
	}
	TEST_CASE("JobSystemMainTest3")
	{
		job j;
		fun_ptr fun1 = [](void *) -> void {};
		for (size_t i = 0; i < 16; ++i)
		{
			job_system::submit_0(j, { { fun1 } });
		}
		job_system::process_0();
	}
	TEST_CASE("JobSystemMainTest4")
	{
		job j;
		fun_ptr fun1 = [](void *) -> void {};
		job_future<void> fut1, fut2;
		job_system::submit_0({
				{fut1,	fun1, nullptr},
				{ fut2, fun1, nullptr}
		});
		job_system::process_0();
	}
	TEST_CASE("JobSystemMainTest5")
	{
		fun_ptr fun1 = [](int32_t a, int32_t b) -> int32_t
		{
			return a + b;
		};
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<int32_t> r(
				std::numeric_limits<int32_t>::min(),
				std::numeric_limits<int32_t>::max());
		job_future<int32_t> f1, f2;
		int32_t a = r(gen), b = r(gen), c = r(gen), d = r(gen);
		job_system::submit_0({
				{f1,	fun1, a, b},
				{ f2, fun1, c, d}
		 });
		job_system::process_0();
		REQUIRE(f1.value() == a + b);
		REQUIRE(f2.value() == c + d);
	}
}