/*! @file   utils_singly_benchmark.cpp
 *  @author Fluffy (noe.louis-quentin@hotmail.fr)
 *  @brief  Benchmark for singly_* objects.
 *  @date   20-03-2022
 *
 *  @copyright Copyright (c) 2022 - Pulsar Software
 *
 *  @since 0.1.1
 */

// Include: Pulsar
#include "pulsar/utility.hpp"

// Include: Catch2
#include "catch2/catch_all.hpp"

// Include: C++
#include <thread>

// Pulsar
namespace pul
{
	// Singly List
	TEST_CASE("SinglyLinkedListBenchmark", "[utility][list]")
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
	TEST_CASE("SinglyRotativeLinkedListBenchmark", "[list][utility]")
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

	// Singly CDS LIFO
	TEST_CASE("SinglyCDSLIFOBenchmark", "[utility][list][cds]")
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
	TEST_CASE("SinglyCDSFIFOBenchmark", "[utility][list][cds]")
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

	// Doubly List
	TEST_CASE("DoublyLinkedListBenchmark", "[utility][list]")
	{
		BENCHMARK_ADVANCED("InsertHead")
		(Catch::Benchmark::Chronometer __m)
		{
			const int32_t n = __m.runs();
			auto buf				= std::make_unique<doubly_node<byte_t[]>[]>(n);
			doubly_linked_list<byte_t[]> slt;
			// insert benchmark
			__m.measure([&buf, &slt](int32_t i)
									{ return slt.insert_head(&buf[i]); });
		};
		BENCHMARK_ADVANCED("RemoveHead")
		(Catch::Benchmark::Chronometer __m)
		{
			const int32_t n = __m.runs();
			auto buf				= std::make_unique<doubly_node<byte_t[]>[]>(n);
			doubly_linked_list<byte_t[]> slt;
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
			auto buf				= std::make_unique<doubly_node<byte_t[]>[]>(n);
			doubly_linked_list<byte_t[]> slt;
			// insert benchmark
			__m.measure([&buf, &slt](int32_t i)
									{ return slt.insert_tail(&buf[i]); });
		};
		BENCHMARK_ADVANCED("RemoveTail")
		(Catch::Benchmark::Chronometer __m)
		{
			const int32_t n = __m.runs();
			auto buf				= std::make_unique<doubly_node<byte_t[]>[]>(n);
			doubly_linked_list<byte_t[]> slt;
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

	// Doubly Rotative List
	TEST_CASE("DoublyRotativeLinkedListBenchmark", "[list][utility]")
	{
		BENCHMARK_ADVANCED("InsertHead")
		(Catch::Benchmark::Chronometer __m)
		{
			const int32_t n = __m.runs();
			auto buf				= std::make_unique<doubly_node<byte_t[]>[]>(n);
			doubly_rotative_linked_list<byte_t[]> rlt;
			// insert benchmark
			__m.measure([&buf, &rlt](int32_t i)
									{ return rlt.insert_head(&buf[i]); });
		};
		BENCHMARK_ADVANCED("RemoveHead")
		(Catch::Benchmark::Chronometer __m)
		{
			const int32_t n = __m.runs();
			auto buf				= std::make_unique<doubly_node<byte_t[]>[]>(n);
			doubly_rotative_linked_list<byte_t[]> rlt;
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
			auto buf				= std::make_unique<doubly_node<byte_t[]>[]>(n);
			doubly_rotative_linked_list<byte_t[]> rlt;
			// insert benchmark
			__m.measure([&buf, &rlt](int32_t i)
									{ return rlt.insert_tail(&buf[i]); });
		};
		BENCHMARK_ADVANCED("RemoveTail")
		(Catch::Benchmark::Chronometer __m)
		{
			const int32_t n = __m.runs();
			auto buf				= std::make_unique<doubly_node<byte_t[]>[]>(n);
			doubly_rotative_linked_list<byte_t[]> rlt;
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