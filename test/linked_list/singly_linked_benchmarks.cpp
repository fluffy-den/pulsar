/*! @file   singly_linked_benchmarks.cpp
 *  @author Fluffy (noe.louis-quentin@hotmail.fr)
 *  @brief
 *  @date   16-06-2022
 *
 *  @copyright Copyright (c) 2022 - Pulsar Software
 *
 *  @since 0.1.2
 */

// Include: Pulsar
#include "pulsar/linked_list.hpp"

// Include: Catch2
#include "catch2/catch_all.hpp"

// Include: C++
#include <memory.h>

// Pulsar
namespace pul
{
	// Singly List
	TEST_CASE("SinglyLinkedListBenchmark")
	{
		BENCHMARK_ADVANCED("InsertHead")
		(Catch::Benchmark::Chronometer __m)
		{
			const int32_t n = __m.runs();
			auto buf				= std::make_unique<singly_node<byte_t[]>[]>(n);
			singly_linked_list<byte_t[]> slt;
			// insert benchmark
			__m.measure([&buf, &slt](int32_t i)
									{ return slt.insert_head(&buf[i]); });
		};
		BENCHMARK_ADVANCED("RemoveHead")
		(Catch::Benchmark::Chronometer __m)
		{
			const int32_t n = __m.runs();
			auto buf				= std::make_unique<singly_node<byte_t[]>[]>(n);
			singly_linked_list<byte_t[]> slt;
			// fill
			for (int32_t i = 0; i < n; ++i)
			{
				slt.insert_head(&buf[i]);
			}
			// remove benchmark
			__m.measure([&slt]
									{ return slt.remove_head(); });
		};
		BENCHMARK_ADVANCED("InsertTail")
		(Catch::Benchmark::Chronometer __m)
		{
			const int32_t n = __m.runs();
			auto buf				= std::make_unique<singly_node<byte_t[]>[]>(n);
			singly_linked_list<byte_t[]> slt;
			// insert benchmark
			__m.measure([&buf, &slt](int32_t i)
									{ return slt.insert_tail(&buf[i]); });
		};
		BENCHMARK_ADVANCED("RemoveTail")
		(Catch::Benchmark::Chronometer __m)
		{
			const int32_t n = __m.runs();
			auto buf				= std::make_unique<singly_node<byte_t[]>[]>(n);
			singly_linked_list<byte_t[]> slt;
			// fill
			for (int32_t i = 0; i < n; ++i)
			{
				slt.insert_tail(&buf[i]);
			}
			// remove benchmark
			__m.measure([&slt]
									{ return slt.remove_tail(); });
		};
	}

	// Singly Rotative List
	TEST_CASE("SinglyRotativeLinkedListBenchmark")
	{
		BENCHMARK_ADVANCED("InsertHead")
		(Catch::Benchmark::Chronometer __m)
		{
			const int32_t n = __m.runs();
			auto buf				= std::make_unique<singly_node<byte_t[]>[]>(n);
			singly_rotative_linked_list<byte_t[]> rlt;
			// insert benchmark
			__m.measure([&buf, &rlt](int32_t i)
									{ return rlt.insert_head(&buf[i]); });
		};
		BENCHMARK_ADVANCED("RemoveHead")
		(Catch::Benchmark::Chronometer __m)
		{
			const int32_t n = __m.runs();
			auto buf				= std::make_unique<singly_node<byte_t[]>[]>(n);
			singly_rotative_linked_list<byte_t[]> rlt;
			// fill
			for (int32_t i = 0; i < n; ++i)
			{
				rlt.insert_head(&buf[i]);
			}
			// remove benchmark
			__m.measure([&rlt]
									{ return rlt.remove_head(); });
		};
		BENCHMARK_ADVANCED("InsertTail")
		(Catch::Benchmark::Chronometer __m)
		{
			const int32_t n = __m.runs();
			auto buf				= std::make_unique<singly_node<byte_t[]>[]>(n);
			singly_rotative_linked_list<byte_t[]> rlt;
			// insert benchmark
			__m.measure([&buf, &rlt](int32_t i)
									{ return rlt.insert_tail(&buf[i]); });
		};
		BENCHMARK_ADVANCED("RemoveTail")
		(Catch::Benchmark::Chronometer __m)
		{
			const int32_t n = __m.runs();
			auto buf				= std::make_unique<singly_node<byte_t[]>[]>(n);
			singly_rotative_linked_list<byte_t[]> rlt;
			// fill
			for (int32_t i = 0; i < n; ++i)
			{
				rlt.insert_tail(&buf[i]);
			}
			// remove benchmark
			__m.measure([&rlt]
									{ return rlt.remove_tail(); });
		};
	}
}