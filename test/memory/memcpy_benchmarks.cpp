/*! @file   memcpy_benchmarks.cpp
 *  @author Fluffy (noe.louis-quentin@hotmail.fr)
 *  @brief
 *  @date   07-08-2022
 *
 *  @copyright Copyright (c) 2022 - Pulsar Software
 */

// Include: Pulsar
#include "pulsar/memory.hpp"

// Include: Catch2
#include "catch2/catch_all.hpp"

// Pulsar
namespace pul
{
	/// CPP: Memcpy Benchmark
	TEST_CASE("CppMemcpyBenchmark")
	{
		const size_t n = 65536;
		BENCHMARK_ADVANCED("Copy1x")
		(Catch::Benchmark::Chronometer __m)
		{
			std::unique_ptr<int32_t[]> from = std::make_unique<int32_t[]>(n);
			std::unique_ptr<int32_t[]> to		= std::make_unique<int32_t[]>(n);
			__m.measure([&]()
									{ return std::memcpy(&to[0], &from[0], sizeof(int32_t) * n); });
		};
		BENCHMARK_ADVANCED("Copy4x")
		(Catch::Benchmark::Chronometer __m)
		{
			const int32_t quartsize					 = n / 4;
			std::unique_ptr<int32_t[]> from1 = std::make_unique<int32_t[]>(quartsize);
			std::unique_ptr<int32_t[]> from2 = std::make_unique<int32_t[]>(quartsize);
			std::unique_ptr<int32_t[]> from3 = std::make_unique<int32_t[]>(quartsize);
			std::unique_ptr<int32_t[]> from4 = std::make_unique<int32_t[]>(quartsize);
			std::unique_ptr<int32_t[]> to		 = std::make_unique<int32_t[]>(n);

			__m.measure([&]()
									{ std::memcpy(&to[0], &from1[0], (quartsize - 1) * sizeof(int32_t));
				std::memcpy(&to[quartsize], &from2[0], (quartsize - 1) * sizeof(int32_t));
				std::memcpy(&to[2 * quartsize], &from3[0], (quartsize - 1) * sizeof(int32_t));
				return std::memcpy(&to[3 * quartsize], &from4[0], (quartsize - 1) * sizeof(int32_t)); });
		};
	}
}
