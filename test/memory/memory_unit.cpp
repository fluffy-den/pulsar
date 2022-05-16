/*! @file   memory_unit.cpp
 *  @author Fluffy (noe.louis-quentin@hotmail.fr)
 *  @brief  Test unit for memory objects (such as allocators) / functions.
 *  @date   29-03-2022
 *
 *  @copyright Copyright (c) 2022 - Pulsar Software
 *
 *  @since 0.1.1
 */

// Include: Pulsar
#include "pulsar/memory.hpp"

// Include: Catch2
#include "catch2/catch_all.hpp"

// Pulsar
namespace pul
{
	/// MEMORY: Buffer
	TEST_CASE("MemoryBufferTest1")
	{
		memory::buffer buf1(1024, memory::MAX_ALIGN);
		REQUIRE(buf1.capacity() == 1024);
		memory::buffer buf2 = buf1;
		REQUIRE(buf2.capacity() == 1024);
		memory::buffer buf3 = std::move(buf1);
		REQUIRE(buf3.capacity() == 1024);
		REQUIRE(buf1.capacity() == 0);
		buf1 = buf2;
		REQUIRE(buf1.capacity() == 1024);
		buf2 = std::move(buf1);
		REQUIRE(buf2.capacity() == 1024);
		REQUIRE(buf1.capacity() == 0);
	}
	TEST_CASE("MemoryBufferTest2")
	{
		memory::buffer buf1(1024, memory::MAX_ALIGN);
		REQUIRE(buf1.capacity() == 1024);
		buf1.resize(2048, memory::MAX_ALIGN);
		REQUIRE(buf1.capacity() == 2048);
	}

	/// MEMORY: Allocators Wrappers
	TEST_CASE("AllocatorLinearWrap")
	{
		memory::allocator_linear all(1024);
		memory::allocator_wrapper<byte_t, memory::allocator_linear> wrp1 = std::move(all);

		byte_t *obj1 = new (wrp1) byte_t('\1');

		memory::allocator_wrapper wrp2 = std::move(wrp1);
		memory::allocator_wrapper wrp3 = wrp2;

		byte_t *obj2 = new (wrp3) byte_t('\3');

		::operator delete(obj1, wrp2);
		::operator delete(obj2, wrp3);
	}
	TEST_CASE("AllocatorStackWrap")
	{
		memory::allocator_stack all(1024);
		memory::allocator_wrapper<byte_t, memory::allocator_stack> wrp1 = std::move(all);

		byte_t *obj1 = new (wrp1) byte_t('\1');

		memory::allocator_wrapper wrp2 = std::move(wrp1);
		memory::allocator_wrapper wrp3 = wrp2;

		byte_t *obj2 = new (wrp3) byte_t('\3');

		::operator delete(obj1, wrp2);
		::operator delete(obj2, wrp3);
	}
	TEST_CASE("AllocatorPoolWrap")
	{
		memory::allocator_pool all(1, 1024, align_val_t(1));
		memory::allocator_wrapper<byte_t, memory::allocator_pool> wrp1 = std::move(all);

		byte_t *obj1 = new (wrp1) byte_t('\1');

		memory::allocator_wrapper wrp2 = std::move(wrp1);
		memory::allocator_wrapper wrp3 = wrp2;

		byte_t *obj2 = new (wrp3) byte_t('\3');

		::operator delete(obj1, wrp2);
		::operator delete(obj2, wrp3);
	}
}