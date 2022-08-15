/*! @file   fun_signal_unit.cpp
 *  @author Fluffy (noe.louis-quentin@hotmail.fr)
 *  @brief  Test function signals.
 *  @date   11-08-2022
 *
 *  @copyright Copyright (c) 2022 - Pulsar Software
 *
 *  @since 0.1.2
 */

// Include: Pulsar
#include "pulsar/function.hpp"

// Include: Catch2
#include "catch2/catch_all.hpp"

// Pulsar
namespace pul
{
	// Function Signal Tests
	TEST_CASE("FunctionSharedSignalTest")
	{
		pf_decl_constexpr int32_t val = 3;
		shared_osignal<void(int32_t), doubly_linked_list> s1;
		shared_isignal<void(int32_t), doubly_linked_list> r1 =
				fun_ptr([](int32_t __i) -> void
								{ REQUIRE(__i == val); });

		shared_isignal<void(int32_t), doubly_linked_list> r2 =
				fun_ptr([](int32_t __i) -> void
								{ REQUIRE(__i == val); });
		s1.signal_add(r1);
		s1.signal_add(r2);
		s1(val);
	}
	TEST_CASE("FunctionUniqueSignalTest")
	{
		pf_decl_constexpr int32_t val = 3;
		unique_osignal<void(int32_t)> s1;
		unique_isignal<void(int32_t)> r1 =
				fun_ptr([](int32_t __i) -> void
								{ REQUIRE(__i == val); });
		s1.signal_set(r1);
		s1(val);
	}
}