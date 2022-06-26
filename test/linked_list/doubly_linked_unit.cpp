/*! @file   doubly_linked_unit.cpp
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
#include <vector>

// Pulsar
namespace pul
{
	// Doubly Node
	TEST_CASE("DoublyNodeConstructTest")
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
	TEST_CASE("DoublyNodeDeferenceTest")
	{
		int32_t i			 = 1;
		doubly_node in = i;
		REQUIRE(*in == i);
	}
	TEST_CASE("DoublyNodeArrowDeferenceTest")
	{
		struct __by_arrow_t
		{
			int32_t i;
		};
		doubly_node in = __by_arrow_t{ 1 };
		REQUIRE(in->i == 1);
	}
	TEST_CASE("DoublyNodeEqualityTest")
	{
		doubly_node in1 = 1;
		doubly_node in2 = 2;
		doubly_node in3 = 1;
		REQUIRE(in1 != in2);
		REQUIRE(in1 == in3);
		REQUIRE(in2 != in3);
	}

	// Doubly Iterator
	TEST_CASE("DoublyIteratorConstructTest")
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
	TEST_CASE("DoublyIteratorEqualityTest")
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
	TEST_CASE("DoublyIteratorBaseTest")
	{
		doubly_node in1			= 1;
		doubly_iterator it1 = &in1;
		REQUIRE(it1.base() == &in1);
	}

	// Doubly Linked List
	TEST_CASE("DoublyLinkedListConstructTest")
	{
		doubly_linked_list<int32_t> ill;
		doubly_linked_list<const byte_t[]> bll;
		REQUIRE(ill.empty());
		REQUIRE(bll.empty());
	}
	TEST_CASE("DoublyLinkedListTest1")
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
	TEST_CASE("DoublyLinkedListTest2")
	{
		doubly_linked_list<byte_t[]> bll;
		doubly_node<byte_t[]> n1, n2;
		bll.insert_tail(&n1);
		bll.insert_tail(&n2);
		REQUIRE(bll.begin() == &n1);
	}
	TEST_CASE("DoublyLinkedListTest3")
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
	TEST_CASE("DoublyLinkedListSTLTest1")
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
	TEST_CASE("DoublyLinkedListSTLTest2")
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
	TEST_CASE("DoublyRotativeListConstructTest")
	{
		doubly_rotative_linked_list<int32_t> rll;
		REQUIRE(rll.empty());
		doubly_rotative_linked_list<const int32_t> crll;
		REQUIRE(crll.empty());
	}
	TEST_CASE("DoublyRotativeListTest1")
	{
		doubly_rotative_linked_list<int32_t> rll;
		doubly_node n1 = 1, n2 = 0;
		rll.insert_head(&n1);
		rll.insert_tail(&n2);
		REQUIRE(rll.head() == &n1);
		REQUIRE(rll.tail() == &n2);
	}
	TEST_CASE("DoublyRotativeListTest2")
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

	// Doubly initialize range
	TEST_CASE("DoublyNodeRangeInitializeTest")
	{
		doubly_node<int32_t> buf[8] = { 0, 1, 2, 3, 4, 5, 6, 7 };
		doubly_link(std::begin(buf), std::end(buf));
	}
}