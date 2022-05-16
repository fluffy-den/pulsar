/*! @file   memory_benchmarks.cpp
 *  @author Fluffy (noe.louis-quentin@hotmail.fr)
 *  @brief
 *  @date   10-04-2022
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
	/// MEMORY: Allocator Linear Benchmark
	TEST_CASE("AllocatorLinearBenchmark", "[allocator][memory]")
	{
		BENCHMARK_ADVANCED("Allocation")
		(Catch::Benchmark::Chronometer __m)
		{
			const int32_t n = __m.runs();
			memory::allocator_linear all(n * static_cast<size_t>(memory::MAX_ALIGN));
			__m.measure([&all]()
									{ return all.allocate(static_cast<size_t>(memory::MAX_ALIGN)); });
		};
	}

	/// MEMORY: Allocator Stack Benchmark
	TEST_CASE("AllocatorStackBenchmark", "[allocator][memory]")
	{
		BENCHMARK_ADVANCED("Allocation")
		(Catch::Benchmark::Chronometer __m)
		{
			const int32_t n = __m.runs();
			memory::allocator_stack all(n * static_cast<size_t>(memory::MAX_ALIGN));
			__m.measure([&all]()
									{ return all.allocate(static_cast<size_t>(memory::MAX_ALIGN)); });
		};
		BENCHMARK_ADVANCED("Deallocation")
		(Catch::Benchmark::Chronometer __m)
		{
			const int32_t n = __m.runs();
			memory::allocator_stack all(n * static_cast<size_t>(memory::MAX_ALIGN));
			auto b = std::make_unique<void *[]>(n);
			for (int32_t i = 0; i < n; ++i)
			{
				b[i] = all.allocate(static_cast<size_t>(memory::MAX_ALIGN));
			}
			__m.measure([&all, &b, &n](int32_t i)
									{ return all.deallocate(b[n - 1 - i]); });
		};
	}

	/// MEMORY: Allocator Pool Benchmark
	TEST_CASE("AllocatorPoolBenchmark", "[allocator][memory]")
	{
		BENCHMARK_ADVANCED("Allocation")
		(Catch::Benchmark::Chronometer __m)
		{
			const int32_t n = __m.runs();
			memory::allocator_pool all(
					static_cast<size_t>(memory::MAX_ALIGN),
					n,
					memory::MAX_ALIGN);
			__m.measure([&all]()
									{ return all.allocate(static_cast<size_t>(memory::MAX_ALIGN)); });
		};
		BENCHMARK_ADVANCED("Deallocation")
		(Catch::Benchmark::Chronometer __m)
		{
			const int32_t n = __m.runs();
			memory::allocator_pool all(
					static_cast<size_t>(memory::MAX_ALIGN),
					n,
					memory::MAX_ALIGN);
			auto b = std::make_unique<void *[]>(n);
			for (int32_t i = 0; i < n; ++i)
			{
				b[i] = all.allocate(static_cast<size_t>(memory::MAX_ALIGN));
			}
			__m.measure([&all, &b](int32_t i)
									{ return all.deallocate(b[i]); });
		};
	}

	/// MEMORY: Allocator CDS Linear Benchmark
	TEST_CASE("AllocatorCDSLinearBenchmark", "[allocator][memory][cds]")
	{
		BENCHMARK_ADVANCED("Allocation")
		(Catch::Benchmark::Chronometer __m)
		{
			const int32_t n = __m.runs();
			memory::allocator_cds_linear all(n * static_cast<size_t>(memory::MAX_ALIGN));
			__m.measure([&all]()
									{ return all.allocate(static_cast<size_t>(memory::MAX_ALIGN)); });
		};
	}

	/// MEMORY: Allocator CDS Stack Benchmark
	TEST_CASE("AllocatorCDSStackBenchmark", "[allocator][memory][cds]")
	{
		BENCHMARK_ADVANCED("Allocation")
		(Catch::Benchmark::Chronometer __m)
		{
			const int32_t n = __m.runs();
			memory::allocator_cds_stack all(n * static_cast<size_t>(memory::MAX_ALIGN));
			__m.measure([&all]()
									{ return all.allocate(static_cast<size_t>(memory::MAX_ALIGN)); });
		};
		BENCHMARK_ADVANCED("Deallocation")
		(Catch::Benchmark::Chronometer __m)
		{
			const int32_t n = __m.runs();
			memory::allocator_cds_stack all(n * static_cast<size_t>(memory::MAX_ALIGN));
			auto b = std::make_unique<void *[]>(n);
			for (int32_t i = 0; i < n; ++i)
			{
				b[i] = all.allocate(static_cast<size_t>(memory::MAX_ALIGN));
			}
			__m.measure([&all, &b, &n](int32_t i)
									{ return all.deallocate(b[n - 1 - i]); });
		};
	}

	/// MEMORY: Allocator CDS Pool Benchmark
	TEST_CASE("AllocatorCDSPoolBenchmark", "[allocator][memory][cds]")
	{
		BENCHMARK_ADVANCED("Allocation")
		(Catch::Benchmark::Chronometer __m)
		{
			const int32_t n = __m.runs();
			memory::allocator_cds_pool all(
					static_cast<size_t>(memory::MAX_ALIGN),
					n,
					memory::MAX_ALIGN);
			__m.measure([&all]()
									{ return all.allocate(static_cast<size_t>(memory::MAX_ALIGN)); });
		};
		BENCHMARK_ADVANCED("Deallocation")
		(Catch::Benchmark::Chronometer __m)
		{
			const int32_t n = __m.runs();
			memory::allocator_cds_pool all(
					static_cast<size_t>(memory::MAX_ALIGN),
					n,
					memory::MAX_ALIGN);
			auto b = std::make_unique<void *[]>(n);
			for (int32_t i = 0; i < n; ++i)
			{
				b[i] = all.allocate(static_cast<size_t>(memory::MAX_ALIGN));
			}
			__m.measure([&all, &b, &n](int32_t i)
									{ return all.deallocate(b[i]); });
		};
	}

}