/*! @file   singly_cds_linked_benchmarks.cpp
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
	// Singly CDS LIFO
	TEST_CASE("SinglyCDSLIFOBenchmark")
	{
		BENCHMARK_ADVANCED("STInsertTail")
		(Catch::Benchmark::Chronometer __m)
		{
			const int32_t n = __m.runs();
			auto buf				= std::make_unique<singly_node<byte_t[]>[]>(n);
			singly_cds_linked_lifo<byte_t[]> conclifo;
			// insert benchmark
			__m.measure([&buf, &conclifo](int32_t i)
									{ return conclifo.insert_tail(&buf[i]); });
		};
		BENCHMARK_ADVANCED("STRemoveHead")
		(Catch::Benchmark::Chronometer __m)
		{
			const int32_t n = __m.runs();
			auto buf				= std::make_unique<singly_node<byte_t[]>[]>(n);
			singly_cds_linked_lifo<byte_t[]> conclifo;
			// fill
			for (int32_t i = 0; i < n; ++i)
			{
				conclifo.insert_tail(&buf[i]);
			}
			// remove benchmark
			__m.measure([&conclifo]
									{ return conclifo.remove_head(); });
		};
		/* NOTE: Multithreading benchmarking for LIFO
		BENCHMARK_ADVANCED("MTInsertHead")
		(Catch::Benchmark::Chronometer __m)
		{
			const int32_t n = __m.runs();
		};
		BENCHMARK_ADVANCED("MTRemoveHead")
		(Catch::Benchmark::Chronometer __m)
		{
			const int32_t n = __m.runs();
		};
		BENCHMARK_ADVANCED("MTInsertAndRemove")
		(Catch::Benchmark::Chronometer __m)
		{
			const int32_t n = __m.runs();
		};
		*/
	}

	// Singly CDS FIFO
	TEST_CASE("SinglyCDSFIFOBenchmark")
	{
		BENCHMARK_ADVANCED("STInsertHead")
		(Catch::Benchmark::Chronometer __m)
		{
			const int32_t n = __m.runs();
			auto buf				= std::make_unique<singly_node<byte_t[]>[]>(n);
			singly_cds_linked_fifo<byte_t[]> concfifo;
			// insert benchmark
			__m.measure([&buf, &concfifo](int32_t i)
									{ return concfifo.insert_head(&buf[i]); });
		};
		BENCHMARK_ADVANCED("STRemoveHead")
		(Catch::Benchmark::Chronometer __m)
		{
			const int32_t n = __m.runs();
			auto buf				= std::make_unique<singly_node<byte_t[]>[]>(n);
			singly_cds_linked_fifo<byte_t[]> concfifo;
			// fill
			for (int32_t i = 0; i < n; ++i)
			{
				concfifo.insert_head(&buf[i]);
			}
			// remove benchmark
			__m.measure([&concfifo]
									{ return concfifo.remove_head(); });
		};
		/* NOTE: Multithreading benchmarking for FIFO
		BENCHMARK_ADVANCED("MTInsertHead")
		(Catch::Benchmark::Chronometer __m)
		{
			const int32_t n = __m.runs();
		};
		BENCHMARK_ADVANCED("MTRemoveHead")
		(Catch::Benchmark::Chronometer __m)
		{
			const int32_t n = __m.runs();
		};
		BENCHMARK_ADVANCED("MTInsertAndRemove")
		(Catch::Benchmark::Chronometer __m)
		{
			const int32_t n = __m.runs();
		};
		*/
	}
}