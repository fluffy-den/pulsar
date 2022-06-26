/*! @file   singly_cds_linked_unit.cpp
 *  @author Fluffy (noe.louis-quentin@hotmail.fr)
 *  @brief
 *  @date   16-06-2022
 *
 *  @copyright Copyright (c) 2022 - Pulsar Software
 *
 *  @since 0.1.2
 */

// Include: Pulsar
#include "pulsar/function.hpp"
#include "pulsar/linked_list.hpp"

// Include: Catch2
#include "catch2/catch_all.hpp"

// Include: C++
#include <thread>
#include <vector>

// REMARK: Fails in debug mode / Work fine in release!

// Pulsar
namespace pul
{
	// Singly CDS LIFO

	// TEST -> N Insertion Bulk
	//			-> N removal with K threads
	TEST_CASE("CDSLIFOTest1")
	{
		// Insert
		singly_cds_linked_lifo<size_t> cll;
		const size_t n	= 2048;
		const size_t tc = std::thread::hardware_concurrency() - 1;
		auto wb					= std::make_unique<std::vector<singly_node<size_t> *>[]>(std::thread::hardware_concurrency());
		auto mb					= std::make_unique<singly_node<size_t>[]>(n);
		for (size_t i = 0; i < n; ++i)
		{
			mb[i] = i;
		}
		cll.insert_tail_bulk(&mb[0], &mb[n]);
		// Proc
		auto proc = [&cll, &wb](size_t __index)
		{
			auto c = cll.remove_head();
			wb[__index].reserve(n);
			while (c)
			{
				wb[__index].emplace_back(c);
				c = cll.remove_head();
			}
		};
		// Launch
		std::vector<std::thread> workers;
		workers.reserve(tc);
		for (size_t i = 0; i < tc; ++i)
		{
			workers.emplace_back(proc, i + 1);
		}
		proc(0);
		for (auto &w : workers)
		{
			if (w.joinable())
				w.join();
		}
		std::vector<singly_node<size_t> *> res;
		res.reserve(n);
		for (size_t i = 0; i < tc; ++i)
		{
			res.insert(res.end(), wb[i].begin(), wb[i].end());
		}
		// Verify Unicity
		for (size_t i = 0; i < n; ++i)
		{
			REQUIRE(std::count(res.begin(), res.end(), &mb[i]) == 1);
		}
	}
}