/*! @file   tuple_benchmark.cpp
 *  @author Fluffy (noe.louis-quentin@hotmail.fr)
 *  @brief
 *  @date   17-06-2022
 *
 *  @copyright Copyright (c) 2022 - Pulsar Software
 *
 *  @since 0.1.2
 */

// Include: Pulsar -> Tuple
#include "pulsar/tuple.hpp"

// Include: Catch2
#include "catch2/catch_all.hpp"

// Include: C++
#include <memory>
#include <numeric>
#include <random>

// Pulsar
namespace pul
{
	/// TUPLE: Benchmarks
	TEST_CASE("TupleBenchmark")
	{
		BENCHMARK_ADVANCED("InitializationTestFor10")
		(Catch::Benchmark::Chronometer __m)
		{
			const size_t n = __m.runs();
			std::random_device rd;
			std::mt19937 gen(rd());
			std::uniform_int_distribution<int32_t> r(
					std::numeric_limits<int32_t>::min(),
					std::numeric_limits<int32_t>::max());
			using i32tuple10_t =
					tuple<int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t>;
			auto buf1 = std::make_unique<i32tuple10_t[]>(n);
			auto buf2 = std::make_unique<i32tuple10_t[]>(n);
			for (auto b = &buf1[0], e = &buf1[n - 1]; b != e; ++b)
			{
				*b = {
					r(gen),
					r(gen),
					r(gen),
					r(gen),
					r(gen),
					r(gen),
					r(gen),
					r(gen),
					r(gen),
					r(gen)
				};
			}

			__m.measure([&buf1, &buf2](int32_t __index)
									{ return buf2[__index] = buf1[__index]; });
		};
		BENCHMARK_ADVANCED("GetIndex9")
		(Catch::Benchmark::Chronometer __m)
		{
			const size_t n = __m.runs();
			std::random_device rd;
			std::mt19937 gen(rd());
			std::uniform_int_distribution<int32_t> r(
					std::numeric_limits<int32_t>::min(),
					std::numeric_limits<int32_t>::max());
			using i32tuple10_t =
					tuple<int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t>;
			auto buf = std::make_unique<i32tuple10_t[]>(n);
			__m.measure([&buf](int32_t __index)
									{ return get<9>(buf[__index]); });
		};
	}

}