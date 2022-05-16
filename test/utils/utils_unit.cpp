/*! @file   utils_singly_unit.cpp
 *  @author Fluffy (noe.louis-quentin@hotmail.fr)
 *  @brief  Test unit for utility objects / functions.
 *  @date   12-03-2022
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
#include <tuple>
#include <vector>


// Pulsar
namespace pul
{

	// String Format
	TEST_CASE("StringFormatTest", "[utility][string]")
	{
		const char *fmt = "%s";
		const char *msg = "test";
		std::string str = strfmt(fmt, msg);
		REQUIRE(str == "test");
		REQUIRE(str.back() != '\0');
	}
	TEST_CASE("StringFormatRefTest", "[utility][string]")
	{
		const char *fmt = " %s";
		const char *msg = "test";
		std::string str = "str with";
		strfmt(fmt, str, str.end(), msg);
		REQUIRE(str == "str with test");
		REQUIRE(str.back() != '\0');
	}
	TEST_CASE("StringTimeFormatTest", "[utility][string]")
	{
		const char *fmt = "%c";
		std::time_t t		= std::time(0);
		std::tm *k			= std::localtime(&t);
		std::string str = strfmt(fmt, *k);
		REQUIRE(!str.empty());
		REQUIRE(str.back() != '\0');
	}
	TEST_CASE("StringTimeFormatRefTest", "[utility][string]")
	{
		const char *fmt = " %c";
		std::string str = "str date";
		std::time_t t		= std::time(0);
		std::tm *k			= std::localtime(&t);
		strfmt(fmt, str, str.end(), *k);
		REQUIRE(str.length() != 8);
		REQUIRE(str.back() != '\0');
	}

	// String Utilities
	TEST_CASE("StringTrimLeftTest", "[utility][string]")
	{
		std::string str = "			message";
		strtriml(str);
		REQUIRE(str == "message");
	}
	TEST_CASE("StringTrimRightTest", "[utility][string]")
	{
		std::string str = "message			";
		strtrimr(str);
		REQUIRE(str == "message");
	}
	TEST_CASE("StringToLowerTest", "[utility][string]")
	{
		std::string str = "MeSSaGE";
		strtolower(str);
		REQUIRE(str == "message");
	}
	TEST_CASE("StringToUpperTest", "[utility][string]")
	{
		std::string str = "MeSSaGE";
		strtoupper(str);
		REQUIRE(str == "MESSAGE");
	}

	// Singly Node
	TEST_CASE("SinglyNodeConstructTest", "[utility][linked_list]")
	{
		singly_node<int32_t> in1;
		std::ignore			= in1;
		singly_node in2 = 1;
		std::ignore			= in2;
		singly_node<byte_t[]> bn;
		std::ignore = bn;
		singly_node<const byte_t[]> cbn;
		std::ignore = cbn;
	}
	TEST_CASE("SinglyNodeDeferenceTest", "[utility][linked_list]")
	{
		int32_t i			 = 1;
		singly_node in = i;
		REQUIRE(*in == i);
	}
	TEST_CASE("SinglyNodeArrowDeferenceTest", "[utility][linked_list]")
	{
		struct __by_arrow_t
		{
			int32_t i;
		};
		singly_node in = __by_arrow_t{ 1 };
		REQUIRE(in->i == 1);
	}
	TEST_CASE("SinglyNodeEqualityTest", "[utility][linked_list]")
	{
		singly_node in1 = 1;
		singly_node in2 = 2;
		singly_node in3 = 1;
		REQUIRE(in1 != in2);
		REQUIRE(in1 == in3);
		REQUIRE(in2 != in3);
	}

	// Singly Iterator
	TEST_CASE("SinglyIteratorConstructTest", "[utility][linked_list]")
	{
		singly_node in1							 = 1;
		singly_iterator it1					 = &in1;
		singly_iterator<int32_t> it2 = nullptr;
		const singly_node in2				 = 1;
		singly_iterator it3					 = &in2;
		// remove warnings
		std::ignore									 = it1;
		std::ignore									 = it2;
		std::ignore									 = it3;
	}
	TEST_CASE("SinglyIteratorEqualityTest", "[utility][linked_list]")
	{
		const singly_node in1 = 1;
		singly_iterator it1		= &in1;
		singly_iterator it2		= it1;
		REQUIRE(it2 == &in1);
		singly_node in2										 = 1;
		singly_const_iterator<int32_t> it3 = static_cast<const singly_node<int32_t> *>(&in2);
		REQUIRE(it3 == &in2);
		singly_node in3			= 1;
		singly_iterator it4 = &in3;
		REQUIRE(it4 != it1);
	}
	TEST_CASE("SinglyIteratorBaseTest", "[utility][linked_list]")
	{
		singly_node in1			= 1;
		singly_iterator it1 = &in1;
		REQUIRE(it1.base() == &in1);
	}

	// Singly Linked-List
	TEST_CASE("SinglyLinkedListConstructTest", "[utility][linked_list]")
	{
		singly_linked_list<int32_t> ill;
		singly_linked_list<const byte_t[]> bll;
		REQUIRE(ill.empty());
		REQUIRE(bll.empty());
	}
	TEST_CASE("SinglyLinkedListTest1", "[utility][linked_list]")
	{
		singly_linked_list<byte_t[]> bll;
		singly_node<byte_t[]> n1;
		bll.insert_head(&n1);
		singly_node<byte_t[]> n2;
		bll.insert_tail(&n2);
		bll.remove_head();
		REQUIRE(bll.begin() == &n2);
		bll.remove(&n2);
		REQUIRE(bll.empty());
	}
	TEST_CASE("SinglyLinkedListTest2", "[utility][linked_list]")
	{
		singly_linked_list<byte_t[]> bll;
		singly_node<byte_t[]> n1, n2;
		bll.insert_tail(&n1);
		bll.insert_tail(&n2);
		REQUIRE(bll.begin() == bll.parent_of(&n2));
	}
	TEST_CASE("SinglyLinkedListTest3", "[utility][linked_list]")
	{
		singly_linked_list<byte_t[]> bll;
		singly_node<byte_t[]> n1, n2, n3;
		singly_iterator i1 = &n1;
		bll.insert_head(i1);
		// [n1]
		bll.insert_tail(&n3);
		// [n1, n3]
		bll.insert_next(&n1, &n2);
		// [n1, n2, n3]
		REQUIRE(++bll.begin() == &n2);
		singly_node<byte_t[]> n4;
		bll.insert_prev(&n2, &n4);
		// [n1, n4, n2, n3]
		REQUIRE(++bll.begin() == &n4);
		REQUIRE(bll.contains(&n4));
		REQUIRE(bll.size() == 4);
		bll.remove_next(&n1);
		// [n1, n2, n3]
		REQUIRE(++bll.begin() == &n2);
		bll.remove_prev(&n3);
		// [n1, n3]
		REQUIRE(++bll.begin() == &n3);
	}
	TEST_CASE("SinglyLinkedListSTLTest1", "[utility][linked_list]")
	{
		singly_linked_list<int32_t> vll;
		const int32_t n = 10;
		auto buf				= std::make_unique<singly_node<int32_t>[]>(n);
		for (int32_t i = 0; i < n; ++i)
		{
			*buf[i] = i;
			vll.insert_tail(&buf[i]);
		}
		std::vector cpy(vll.cbegin(), vll.cend());
		auto it = vll.cbegin();
		for (int32_t i = 0; i < n; ++i)
		{
			REQUIRE(cpy[i] == *it);
			++it;
		}
		vll.clear();
	}
	TEST_CASE("SinglyLinkedListSTLTest2, [utility][linked_list]")
	{
		singly_linked_list<int32_t> vll;
		const int32_t n = 10;
		auto buf				= std::make_unique<singly_node<int32_t>[]>(n);
		for (int32_t i = 0; i < n; ++i)
		{
			*buf[i] = i;
			vll.insert_tail(&buf[i]);
		}
		std::transform(vll.cbegin(), vll.cend(), vll.begin(), [](int32_t __i)
									 { if (__i % 2 == 0) return 0; return __i; });
		auto beg = vll.cbegin();
		for (int32_t i = 0; i < n; ++i)
		{
			if (i % 2 == 0)
			{
				REQUIRE(*beg == 0);
			}
			else
			{
				REQUIRE(*beg == i);
			}
			++beg;
		}
		vll.clear();
	}

	// Singly Rotative Linked-List
	TEST_CASE("SinglyRotativeListConstructTest", "[utility][linked_list]")
	{
		singly_rotative_linked_list<int32_t> rll;
		REQUIRE(rll.empty());
		singly_rotative_linked_list<const int32_t> crll;
		REQUIRE(crll.empty());
	}
	TEST_CASE("SinglyRotativeListTest1", "[utility][linked_list]")
	{
		singly_rotative_linked_list<int32_t> rll;
		singly_node n1 = 1, n2 = 0;
		rll.insert_head(&n1);
		rll.insert_tail(&n2);
		REQUIRE(rll.head() == &n1);
		REQUIRE(rll.tail() == &n2);
	}
	TEST_CASE("SinglyRotativeListTest2", "[utility][linked_list]")
	{
		singly_rotative_linked_list<int32_t> rll;
		singly_node n1 = 1, n2 = 0, n3 = 2, n4 = 3;
		rll.insert_next(nullptr, &n1);
		// [n1]
		REQUIRE(rll.head() == &n1);
		rll.insert_next(&n1, &n2);
		// [n1, n2]
		REQUIRE(++rll.begin() == &n2);
		rll.insert_prev(nullptr, &n3);
		// [n1, n2, n3]
		REQUIRE(rll.tail() == &n3);
		// [n1, n4, n2, n3]
		rll.insert_prev(&n2, &n4);
		REQUIRE(++rll.begin() == &n4);
		singly_iterator it = &n3;
		REQUIRE(++it == rll.begin());
		// [n4, n2, n3]
		rll.remove_head();
		// [n4, n2]
		rll.remove_tail();
		REQUIRE(rll.head() == &n4);
		REQUIRE(rll.tail() == &n2);
		REQUIRE(rll.size() == 2);
		REQUIRE(!rll.empty());
		// [n2]
		rll.remove_prev(&n2);
		REQUIRE(rll.head() == &n2);
		// [n4, n2]
		rll.insert_prev(&n2, &n4);
		rll.remove_next(&n4);
		REQUIRE(rll.tail() == &n4);
	}

	// Singly CDS LIFO
	TEST_CASE("CDSLIFOTest1", "[utility][linked_list][cds]")
	{
		// Insert
		singly_cds_linked_lifo<int32_t> cll;
		std::atomic_flag start;
		start.clear(std::memory_order::release);
		const size_t n								 = 4096;
		const size_t threadcount			 = std::thread::hardware_concurrency();
		const size_t totalsize				 = n * (threadcount + 1);
		auto buf											 = std::make_unique<singly_node<int32_t>[]>(totalsize);
		const singly_node<int32_t> *cb = &buf[0],
															 *ce = &buf[totalsize - 1] + 1;
		size_t i											 = 0;
		for (auto *b = &buf[0], *e = &buf[totalsize - 1] + 1; b != e; ++b)
		{
			*b = i;
			++i;
		}
		auto cfp = [&buf, &cll, &start](size_t id)
		{
			start.wait(false, std::memory_order::acquire);
			for (size_t i = 0; i < n; ++i)
			{
				cll.insert_tail(&buf[i + id * n]);
			}
		};
		std::vector<std::thread> threads;
		threads.reserve(threadcount);
		for (size_t i = 0; i < threadcount; ++i)
		{
			threads.emplace_back(cfp, i + 1);
		}
		start.test_and_set(std::memory_order::release);
		start.notify_all();
		cfp(0);
		// Stop the threads
		for (auto &it : threads)
		{
			if (it.joinable()) it.join();
		}
		threads.clear();
		// Unicity of nodes?
		for (size_t i = 0; i < totalsize; ++i)
		{
			REQUIRE(std::count(cb, ce, i) == 1);
		}
		// Iterator is well-formed?
		for (size_t i = 0; i < totalsize; ++i)
		{
			REQUIRE(std::count(cll.cbegin(), cll.cend(), i) == 1);
		}
		// Remove
		start.clear(std::memory_order::release);
		auto cfr = [&cll, &start]()
		{
			start.wait(false, std::memory_order::acquire);
			while (cll.remove_head() != nullptr)
				;
		};
		for (size_t i = 0; i < threadcount; ++i)
		{
			threads.emplace_back(cfr);
		}
		start.test_and_set(std::memory_order::release);
		start.notify_all();
		cfr();
		// Stop the threads
		for (auto &it : threads)
		{
			if (it.joinable()) it.join();
		}
		threads.clear();
		cll.clear();
		// Nodes mustn't be in the list
		REQUIRE(cll.empty());
	}

	// Singly CDS FIFO
	TEST_CASE("CDSFIFOTest1", "[utility][linked_list][cds]")
	{
		// Insert
		singly_cds_linked_fifo<int32_t> cll;
		std::atomic_flag start;
		start.clear(std::memory_order::release);
		const size_t n								 = 4096;
		const size_t threadcount			 = std::thread::hardware_concurrency();
		const size_t totalsize				 = n * (threadcount + 1);
		auto buf											 = std::make_unique<singly_node<int32_t>[]>(totalsize);
		const singly_node<int32_t> *cb = &buf[0],
															 *ce = &buf[totalsize - 1] + 1;
		size_t i											 = 0;
		for (auto *b = &buf[0], *e = &buf[totalsize - 1] + 1; b != e; ++b)
		{
			*b = i;
			++i;
		}
		auto cfp = [&buf, &cll, &start](size_t id)
		{
			start.wait(false, std::memory_order::acquire);
			for (size_t i = 0; i < n; ++i)
			{
				cll.insert_head(&buf[i + id * n]);
			}
		};
		std::vector<std::thread> threads;
		threads.reserve(threadcount);
		for (size_t i = 0; i < threadcount; ++i)
		{
			threads.emplace_back(cfp, i + 1);
		}
		start.test_and_set(std::memory_order::release);
		start.notify_all();
		cfp(0);
		// Stop the threads
		for (auto &it : threads)
		{
			if (it.joinable()) it.join();
		}
		threads.clear();
		// Unicity of nodes?
		for (size_t i = 0; i < totalsize; ++i)
		{
			REQUIRE(std::count(cb, ce, i) == 1);
		}
		// Iterator is well-formed?
		for (size_t i = 0; i < totalsize; ++i)
		{
			REQUIRE(std::count(cll.cbegin(), cll.cend(), i) == 1);
		}
		// Remove
		start.clear(std::memory_order::release);
		auto cfr = [&cll, &start]()
		{
			start.wait(false, std::memory_order::acquire);
			while (cll.remove_head() != nullptr)
				;
		};
		for (size_t i = 0; i < threadcount; ++i)
		{
			threads.emplace_back(cfr);
		}
		start.test_and_set(std::memory_order::release);
		start.notify_all();
		cfr();
		// Stop the threads
		for (auto &it : threads)
		{
			if (it.joinable()) it.join();
		}
		threads.clear();
		// Nodes mustn't be in the list
		REQUIRE(cll.empty());
	}

	// Doubly Node
	TEST_CASE("DoublyNodeConstructTest", "[utility][linked_list]")
	{
		doubly_node<int32_t> in1;
		std::ignore			= in1;
		doubly_node in2 = 1;
		std::ignore			= in2;
		doubly_node<byte_t[]> bn;
		std::ignore = bn;
		doubly_node<const byte_t[]> cbn;
		std::ignore = cbn;
	}
	TEST_CASE("DoublyNodeDeferenceTest", "[utility][linked_list]")
	{
		int32_t i			 = 1;
		doubly_node in = i;
		REQUIRE(*in == i);
	}
	TEST_CASE("DoublyNodeArrowDeferenceTest", "[utility][linked_list]")
	{
		struct __by_arrow_t
		{
			int32_t i;
		};
		doubly_node in = __by_arrow_t{ 1 };
		REQUIRE(in->i == 1);
	}
	TEST_CASE("DoublyNodeEqualityTest", "[utility][linked_list]")
	{
		doubly_node in1 = 1;
		doubly_node in2 = 2;
		doubly_node in3 = 1;
		REQUIRE(in1 != in2);
		REQUIRE(in1 == in3);
		REQUIRE(in2 != in3);
	}

	// Doubly Iterator
	TEST_CASE("DoublyIteratorConstructTest", "[utility][linked_list]")
	{
		doubly_node in1							 = 1;
		doubly_iterator it1					 = &in1;
		doubly_iterator<int32_t> it2 = nullptr;
		const doubly_node in2				 = 1;
		doubly_iterator it3					 = &in2;
		// remove warnings
		std::ignore									 = it1;
		std::ignore									 = it2;
		std::ignore									 = it3;
	}
	TEST_CASE("DoublyIteratorEqualityTest", "[utility][linked_list]")
	{
		const doubly_node in1 = 1;
		doubly_iterator it1		= &in1;
		doubly_iterator it2		= it1;
		REQUIRE(it2 == &in1);
		doubly_node in2										 = 1;
		doubly_const_iterator<int32_t> it3 = static_cast<const doubly_node<int32_t> *>(&in2);
		REQUIRE(it3 == &in2);
		doubly_node in3			= 1;
		doubly_iterator it4 = &in3;
		REQUIRE(it4 != it1);
	}
	TEST_CASE("DoublyIteratorBaseTest", "[utility][linked_list]")
	{
		doubly_node in1			= 1;
		doubly_iterator it1 = &in1;
		REQUIRE(it1.base() == &in1);
	}

	// Doubly Linked List
	TEST_CASE("DoublyLinkedListConstructTest", "[utility][linked_list]")
	{
		doubly_linked_list<int32_t> ill;
		doubly_linked_list<const byte_t[]> bll;
		REQUIRE(ill.empty());
		REQUIRE(bll.empty());
	}
	TEST_CASE("DoublyLinkedListTest1", "[utility][linked_list]")
	{
		doubly_linked_list<byte_t[]> bll;
		doubly_node<byte_t[]> n1;
		bll.insert_head(&n1);
		doubly_node<byte_t[]> n2;
		bll.insert_tail(&n2);
		bll.remove_head();
		REQUIRE(bll.begin() == &n2);
		bll.remove(&n2);
		REQUIRE(bll.empty());
	}
	TEST_CASE("DoublyLinkedListTest2", "[utility][linked_list]")
	{
		doubly_linked_list<byte_t[]> bll;
		doubly_node<byte_t[]> n1, n2;
		bll.insert_tail(&n1);
		bll.insert_tail(&n2);
		REQUIRE(bll.begin() == &n1);
	}
	TEST_CASE("DoublyLinkedListTest3", "[utility][linked_list]")
	{
		doubly_linked_list<byte_t[]> bll;
		doubly_node<byte_t[]> n1, n2, n3;
		doubly_iterator i1 = &n1;
		bll.insert_head(i1);
		// [n1]
		bll.insert_tail(&n3);
		// [n1, n3]
		bll.insert_next(&n1, &n2);
		// [n1, n2, n3]
		REQUIRE(++bll.begin() == &n2);
		doubly_node<byte_t[]> n4;
		bll.insert_prev(&n2, &n4);
		// [n1, n4, n2, n3]
		REQUIRE(++bll.begin() == &n4);
		REQUIRE(bll.contains(&n4));
		REQUIRE(bll.size() == 4);
		bll.remove_next(&n1);
		// [n1, n2, n3]
		REQUIRE(++bll.begin() == &n2);
		bll.remove_prev(&n3);
		// [n1, n3]
		REQUIRE(++bll.begin() == &n3);
	}
	TEST_CASE("DoublyLinkedListSTLTest1", "[utility][linked_list]")
	{
		doubly_linked_list<int32_t> vll;
		const int32_t n = 10;
		auto buf				= std::make_unique<doubly_node<int32_t>[]>(n);
		for (int32_t i = 0; i < n; ++i)
		{
			*buf[i] = i;
			vll.insert_tail(&buf[i]);
		}
		std::vector cpy(vll.cbegin(), vll.cend());
		auto it = vll.cbegin();
		for (int32_t i = 0; i < n; ++i)
		{
			REQUIRE(cpy[i] == *it);
			++it;
		}
		vll.clear();
	}
	TEST_CASE("DoublyLinkedListSTLTest2", "[utility][linked_list]")
	{
		doubly_linked_list<int32_t> vll;
		const int32_t n = 10;
		auto buf				= std::make_unique<doubly_node<int32_t>[]>(n);
		for (int32_t i = 0; i < n; ++i)
		{
			*buf[i] = i;
			vll.insert_tail(&buf[i]);
		}
		std::transform(vll.cbegin(), vll.cend(), vll.begin(), [](int32_t __i)
									 { if (__i % 2 == 0) return 0; return __i; });
		auto beg = vll.cbegin();
		for (int32_t i = 0; i < n; ++i)
		{
			if (i % 2 == 0)
			{
				REQUIRE(*beg == 0);
			}
			else
			{
				REQUIRE(*beg == i);
			}
			++beg;
		}
		vll.clear();
	}

	// Doubly Rotative Linked-List
	TEST_CASE("DoublyRotativeListConstructTest", "[utility][linked_list]")
	{
		doubly_rotative_linked_list<int32_t> rll;
		REQUIRE(rll.empty());
		doubly_rotative_linked_list<const int32_t> crll;
		REQUIRE(crll.empty());
	}
	TEST_CASE("DoublyRotativeListTest1", "[utility][linked_list]")
	{
		doubly_rotative_linked_list<int32_t> rll;
		doubly_node n1 = 1, n2 = 0;
		rll.insert_head(&n1);
		rll.insert_tail(&n2);
		REQUIRE(rll.head() == &n1);
		REQUIRE(rll.tail() == &n2);
	}
	TEST_CASE("DoublyRotativeListTest2", "[utility][linked_list]")
	{
		doubly_rotative_linked_list<int32_t> rll;
		doubly_node n1 = 1, n2 = 0, n3 = 2, n4 = 3;
		rll.insert_next(nullptr, &n1);
		// [n1]
		REQUIRE(rll.head() == &n1);
		rll.insert_next(&n1, &n2);
		// [n1, n2]
		REQUIRE(++rll.begin() == &n2);
		rll.insert_prev(nullptr, &n3);
		// [n1, n2, n3]
		REQUIRE(rll.tail() == &n3);
		// [n1, n4, n2, n3]
		rll.insert_prev(&n2, &n4);
		REQUIRE(++rll.begin() == &n4);
		doubly_iterator it = &n3;
		REQUIRE(++it == rll.begin());
		// [n4, n2, n3]
		rll.remove_head();
		// [n4, n2]
		rll.remove_tail();
		REQUIRE(rll.head() == &n4);
		REQUIRE(rll.tail() == &n2);
		REQUIRE(rll.size() == 2);
		REQUIRE(!rll.empty());
		// [n2]
		rll.remove_prev(&n2);
		REQUIRE(rll.head() == &n2);
		// [n4, n2]
		rll.insert_prev(&n2, &n4);
		rll.remove_next(&n4);
		REQUIRE(rll.tail() == &n4);
	}


	// Function Pointer Tests
	TEST_CASE("FunctionPointerTest", "[utility][fun_ptr]")
	{
		auto lambda = [](int32_t __i)
		{ return __i + 1; };
		fun_ptr p = lambda;
		REQUIRE(p(1) == 2);
	}
}