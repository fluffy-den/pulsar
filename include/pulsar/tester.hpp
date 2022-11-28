/*! @file   tester.hpp
 *  @author Fluffy (noe.louis-quentin@hotmail.fr)
 *  @brief
 *  @date   12-09-2022
 *
 *  @copyright Copyright (c) 2022 - Pulsar Software
 *
 *  @since 0.1.2
 */

#ifndef PULSAR_TESTER_HPP
#define PULSAR_TESTER_HPP 1

// Include: Pulsar
#include "pulsar/pulsar.hpp"
#include "pulsar/function.hpp"
#include "pulsar/memory.hpp"
#include "pulsar/ratio.hpp"
#include "pulsar/string.hpp"
#include "pulsar/list.hpp"

// Pulsar
namespace pul
{

	/// TEST: Result
	struct __test_result_t
	{
		nanosecond_t total;
		nanosecond_t average;
		nanosecond_t min;
		nanosecond_t max;
	};

	/// TEST: Benchmarker
	class __test_benchmarker
	{
	public:
		/// TODO: Test Benchmarker

	private:
		unique_ptr<nanosecond_t[], deleter_default<unique_store<nanosecond_t[]>>> results_;
	};

	/// TEST: Unit
	class __test_unit
	{
	public:

	private:
		uspan_t name_;
		fun_ptr<void(void)> unit_;
	};

	/// TEST: Engine
	class __test_engine
	{
	public:
		/// TODO: Test Engine

	private:
		pf_decl_static pf_decl_thread_local pf_decl_inline list<__test_unit> units_;
	};

}

#define pf_test(cond)
#define pf_require(cond)
#define pf_catch(...)
#define pf_unit(name)
#define pf_benchmark(name)

#endif // !PULSAR_TESTER_HPP