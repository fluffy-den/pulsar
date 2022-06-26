/*! @file   singly_linked_unit.cpp
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

// Pulsar
namespace pul
{
	// Singly Node
	TEST_CASE("SinglyNodeConstructTest")
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
	TEST_CASE("SinglyNodeDeferenceTest")
	{
		int32_t i			 = 1;
		singly_node in = i;
		REQUIRE(*in == i);
	}
	TEST_CASE("SinglyNodeArrowDeferenceTest")
	{
		struct __by_arrow_t
		{
			int32_t i;
		};
		singly_node in = __by_arrow_t{ 1 };
		REQUIRE(in->i == 1);
	}
	TEST_CASE("SinglyNodeEqualityTest")
	{
		singly_node in1 = 1;
		singly_node in2 = 2;
		singly_node in3 = 1;
		REQUIRE(in1 != in2);
		REQUIRE(in1 == in3);
		REQUIRE(in2 != in3);
	}

	// Singly Iterator
	TEST_CASE("SinglyIteratorConstructTest")
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
	TEST_CASE("SinglyIteratorEqualityTest")
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
	TEST_CASE("SinglyIteratorBaseTest")
	{
		singly_node in1			= 1;
		singly_iterator it1 = &in1;
		REQUIRE(it1.base() == &in1);
	}

	// Singly Linked-List
	TEST_CASE("SinglyLinkedListConstructTest")
	{
		singly_linked_list<int32_t> ill;
		singly_linked_list<const byte_t[]> bll;
		REQUIRE(ill.empty());
		REQUIRE(bll.empty());
	}
	TEST_CASE("SinglyLinkedListTest1")
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
	TEST_CASE("SinglyLinkedListTest2")
	{
		singly_linked_list<byte_t[]> bll;
		singly_node<byte_t[]> n1, n2;
		bll.insert_tail(&n1);
		bll.insert_tail(&n2);
		REQUIRE(bll.begin() == bll.parent_of(&n2));
	}
	TEST_CASE("SinglyLinkedListTest3")
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
	TEST_CASE("SinglyLinkedListSTLTest1")
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
	TEST_CASE("SinglyLinkedListSTLTest2")
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
	TEST_CASE("SinglyRotativeListConstructTest")
	{
		singly_rotative_linked_list<int32_t> rll;
		REQUIRE(rll.empty());
		singly_rotative_linked_list<const int32_t> crll;
		REQUIRE(crll.empty());
	}
	TEST_CASE("SinglyRotativeListTest1")
	{
		singly_rotative_linked_list<int32_t> rll;
		singly_node n1 = 1, n2 = 0;
		rll.insert_head(&n1);
		rll.insert_tail(&n2);
		REQUIRE(rll.head() == &n1);
		REQUIRE(rll.tail() == &n2);
	}
	TEST_CASE("SinglyRotativeListTest2")
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

	// Singly initialize range
	TEST_CASE("SinglyNodeRangeInitializeTest")
	{
		singly_node<int32_t> buf[8] = { 0, 1, 2, 3, 4, 5, 6, 7 };
		singly_link(std::begin(buf), std::end(buf));
	}
}