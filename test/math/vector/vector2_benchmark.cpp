/*! @file   vector2_benchmark.cpp
 *  @author Fluffy (noe.louis-quentin@hotmail.fr)
 *  @brief	Benchmark for size 2 vectors.
 *  @date   26-05-2022
 *
 *  @copyright Copyright (c) 2022 - Pulsar Software
 *
 *  @since 0.1.1
 */

// Include: Pulsar
#include "pulsar/mathematics.hpp"

// Include: Catch2
#include "catch2/catch_all.hpp"

// Include: C++
#include <numeric>
#include <random>

// Pulsar
namespace pul
{
	/// MATH: Vec2 Benchmark
	TEST_CASE("MathVec2Benchmarks")
	{
		BENCHMARK_ADVANCED("SumBenchmark")
		(Catch::Benchmark::Chronometer __m)
		{
			const size_t n = __m.runs() * 2;
			std::random_device rd;
			std::mt19937 gen(rd());
			std::uniform_real_distribution<float32_t> r(
					std::numeric_limits<float32_t>::min(),
					std::numeric_limits<float32_t>::max());
			auto buf = std::make_unique<f32vec2u_t[]>(n);
			for (auto b = &buf[0], e = &buf[n - 1]; b != e; ++b)
			{
				*b = { r(gen), r(gen) };
			}
			__m.measure([&buf](int32_t __index)
									{ return buf[2 * __index] + buf[2 * __index + 1]; });
		};
		BENCHMARK_ADVANCED("SumBenchmarkSIMD")
		(Catch::Benchmark::Chronometer __m)
		{
			const size_t n = __m.runs() * 2;
			std::random_device rd;
			std::mt19937 gen(rd());
			std::uniform_real_distribution<float32_t> r(
					std::numeric_limits<float32_t>::min(),
					std::numeric_limits<float32_t>::max());
			auto buf = std::make_unique<f32vec2_t[]>(n);
			for (auto b = &buf[0], e = &buf[n - 1]; b != e; ++b)
			{
				*b = { r(gen), r(gen) };
			}
			__m.measure([&buf](int32_t __index)
									{ return buf[2 * __index] + buf[2 * __index + 1]; });
		};
		BENCHMARK_ADVANCED("MulBenchmark")
		(Catch::Benchmark::Chronometer __m)
		{
			const size_t n = __m.runs();
			std::random_device rd;
			std::mt19937 gen(rd());
			std::uniform_real_distribution<float32_t> r(
					std::numeric_limits<float32_t>::min(),
					std::numeric_limits<float32_t>::max());
			auto rbuf = std::make_unique<f32vec2u_t[]>(n);
			auto cbuf = std::make_unique<f32vec2u_t[]>(n);
			for (auto b = &rbuf[0], e = &rbuf[n - 1]; b != e; ++b)
			{
				*b = { r(gen), r(gen) };
			}
			for (auto b = &cbuf[0], e = &cbuf[n - 1]; b != e; ++b)
			{
				*b = { r(gen), r(gen) };
			}
			__m.measure([&rbuf, &cbuf](int32_t __index)
									{ return rbuf[__index] * cbuf[__index]; });
		};
		BENCHMARK_ADVANCED("MulBenchmarkSIMD")
		(Catch::Benchmark::Chronometer __m)
		{
			const size_t n = __m.runs();
			std::random_device rd;
			std::mt19937 gen(rd());
			std::uniform_real_distribution<float32_t> r(
					std::numeric_limits<float32_t>::min(),
					std::numeric_limits<float32_t>::max());
			auto rbuf = std::make_unique<f32vec2_t[]>(n);
			auto cbuf = std::make_unique<f32vec2_t[]>(n);
			for (auto b = &rbuf[0], e = &rbuf[n - 1]; b != e; ++b)
			{
				*b = { r(gen), r(gen) };
			}
			for (auto b = &cbuf[0], e = &cbuf[n - 1]; b != e; ++b)
			{
				*b = { r(gen), r(gen) };
			}
			__m.measure([&rbuf, &cbuf](int32_t __index)
									{ return rbuf[__index] * cbuf[__index]; });
		};
	}
}