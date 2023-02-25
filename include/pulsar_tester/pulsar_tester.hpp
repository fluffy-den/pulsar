/*! @file   tester.hpp
 *  @author Louis-Quentin Noé (noe.louis-quentin@hotmail.fr)
 *  @brief
 *  @date   12-09-2022
 *
 *  @copyright Copyright (c) 2023 - Pulsar Software
 *
 *  @since 0.1.2
 */

#ifndef PULSAR_TESTER_HPP
#define PULSAR_TESTER_HPP 1

// Include: Pulsar
#include "pulsar/debug.hpp"

// Pulsar Tester 
namespace pul
{
	/// TESTER: Types
	class __tester_unit;
	class __tester_pack;
	class __tester_engine;
	class __tester_benchmark;
	class __tester_exception_require;

	/// TESTER: Unit
	class __tester_unit
	{
	public:
		/// Constructors
		__tester_unit(
			dbg_u8string_view __name) pf_attr_noexcept;
		__tester_unit(__tester_unit const &) = delete;
		__tester_unit(__tester_unit &&)			 = delete;

		/// Run
		pf_decl_virtual void __run() = 0;

		/// Destructor
		pf_decl_virtual ~__tester_unit() = default;

		/// Store
		dbg_u8string_view name_;
		__tester_unit *next_;
	};


	/// TESTER: Benchmark
	class __tester_benchmark
	{
	public:
		/// Constructor
    __tester_benchmark(
        dbg_u8string __name,
        size_t __itc) pf_attr_noexcept;

    /// Destructor
    ~__tester_benchmark() = default;

    /// Proc
    pf_decl_virtual void
    __proc(__tester_benchmark &__b) = 0;

		/// Run
    void
    __run();
    void
    bench(
      fun_buf<void(size_t)> __benchFun);

		/// Store
    dbg_u8string_view name_;
    fun_buf<void(size_t)> benchFun_;
    __tester_benchmark *next_;
    size_t itc_;
  };

	/// TESTER: Unit -> Exception Require
	class __tester_exception_require
	{
	public:
		/// Constructors
		__tester_exception_require(
			dbg_u8string_view __file,
			uint32_t __line) pf_attr_noexcept;
		__tester_exception_require(__tester_exception_require const&) = default;
		__tester_exception_require(__tester_exception_require &&)			= default;

		/// Destructor
		~__tester_exception_require() pf_attr_noexcept = default;

		/// Operator =
		__tester_exception_require &operator=(__tester_exception_require const &) = default;
		__tester_exception_require &operator=(__tester_exception_require &&)			= default;

		/// File
		pf_hint_nodiscard pf_decl_inline dbg_u8string_view
		file() const pf_attr_noexcept
		{
			return this->file_;
		}

		/// Line
		pf_hint_nodiscard pf_decl_inline uint32_t
		line() const pf_attr_noexcept
		{
			return this->line_;
		}

		/// Store
		dbg_u8string_view file_;
		uint32_t line_;
	};

	/// TESTER: Unit -> Pack
	class __tester_pack
	{
	public:
		/// Constructors
		__tester_pack(
			dbg_u8string_view __name) pf_attr_noexcept;
		__tester_pack(__tester_pack const &) = delete;
		__tester_pack(__tester_pack &&)			 = delete;

		/// Destructor
		~__tester_pack() pf_attr_noexcept = default;

		/// Operator =
		__tester_pack &operator=(__tester_pack const &) = delete;
		__tester_pack &operator=(__tester_pack &&)			= delete;

		/// Run
		void
		__run() pf_attr_noexcept;

		/// Unit
		void
		__add_unit(
			__tester_unit *__u) pf_attr_noexcept;
    void
    __add_benchmark(
      __tester_benchmark *__b) pf_attr_noexcept;
    void
		__add_result(
			bool __c,
			dbg_u8string_view __file,
			uint32_t __line) pf_attr_noexcept;

		/// Store
		dbg_u8string_view name_;
		__tester_pack *next_;
		__tester_unit *unitHead_;
		__tester_unit *unitTail_;
    __tester_benchmark *benchHead_;
    __tester_benchmark *benchTail_;
    size_t numTests_;
		size_t numFailed_;
	};

	/// TESTER: Engine
	// Type
	class __tester_engine pf_attr_final
	{
	public:
		/// Constructors
		__tester_engine() pf_attr_noexcept;
		__tester_engine(__tester_engine const &) = delete;
		__tester_engine(__tester_engine &&)			 = delete;

		/// Destructor
		~__tester_engine() pf_attr_noexcept = default;

		/// Run
		pf_hint_nodiscard int32_t
		run() pf_attr_noexcept;

		/// Operator =
		__tester_engine &operator=(
			__tester_engine const &) = delete;
		__tester_engine &operator=(
			__tester_engine &&) = delete;

		/// Test
		void
		__test(
			bool __c,
			dbg_u8string_view __file,
			uint32_t __line) pf_attr_noexcept;

		/// Pack
		void
		__add_pack(
			__tester_pack *__p) pf_attr_noexcept;
		pf_hint_nodiscard __tester_pack*
		__cur_pack() pf_attr_noexcept;

		/// Store
		__tester_pack unscoppedPack_;
		__tester_pack *packHead_;
		__tester_pack *packTail_;
	};

	/// TESTER: Instances
	pf_decl_extern __tester_engine tester_engine;

	/// TESTER: Functions
	pf_hint_noreturn pf_decl_static void __test_require(
		bool __c,
		dbg_u8string_view __file,
		uint32_t __line)
	{
		if(!__c) throw(__tester_exception_require(__file, __line));
	}
}

/// TESTER: Macro -> Functions
#define pt_check(cond) pul::tester_engine.__test(cond, __FILE__, __LINE__)
#define pt_require(cond) pul::__test_require(cond, __FILE__, __LINE__)

/// TESTER: Macro -> Unit
#define __pt_generate_unit_name(name) pf_stringize(name)
#define __pt_generate_unit_type(name) pf_concatenate(pf_concatenate(__pt_unit_, name), _t)
#define __pt_generate_unit_instance_name(name) __pt_generate_unit_type(name) pf_concatenate(pf_concatenate(__pt_unit_, name), _instance)

#define pt_unit(name)                                                   \
class __pt_generate_unit_type(name)	                                    \
		: public pul::__tester_unit			                                    \
{                                                                       \
public:																	                                \
  __pt_generate_unit_type(name)() pf_attr_noexcept                      \
    : pul::__tester_unit(__pt_generate_unit_name(name))                 \
  {}                                                                    \
  void                                                                  \
  __run();                                                              \
};                                                                      \
pf_decl_static pf_decl_inline __pt_generate_unit_instance_name(name);   \
void                                                                    \
__pt_generate_unit_type(name)::__run()                                


/// TESTER: Macro -> Pack
#define __pt_generate_pack_name(name) pf_stringize(name)
#define __pt_generate_pack_type(name) pf_concatenate(pf_concatenate(__pt_pack_, name), _t)
#define __pt_generate_pack_instance_name(name) __pt_generate_pack_type(name) pf_concatenate(pf_concatenate(__pt_pack_, name), _instance)

#define pt_pack(name)                                                   \
class __pt_generate_pack_type(name)                                     \
	: public pul::__tester_pack                                           \
{                                                                       \
public:                                                                 \
  __pt_generate_pack_type(name)() pf_attr_noexcept                      \ 
    : pul::__tester_pack(__pt_generate_pack_name(name))                 \
  {}                                                                    \
};                                                                      \
pf_decl_static pf_decl_inline __pt_generate_pack_instance_name(name);


/// TESTER: Macro -> Benchmark
#define __pt_generate_benchmark_name(name) pf_stringize(name)
#define __pt_generate_benchmark_type(name) pf_concatenate(pf_concatenate(__pt_benchmark_, name), _t)
#define __pt_generate_benchmark_instance_name(name) __pt_generate_benchmark_type(name) pf_concatenate(pf_concatenate(__pt_benchmark_, name), _instance)

#define pt_benchmark(name, bvn, itc)														               \  
class __pt_generate_benchmark_type(name)									                     \
	: public pul::__tester_benchmark                                             \
{																													                     \
public:																											                   \
  __pt_generate_benchmark_type(name)() pf_attr_noexcept                        \
    : pul::__tester_benchmark(__pt_generate_benchmark_name(name), itc)         \
  {}                                                                           \
  void                                                                         \
  __proc(pul::__tester_benchmark &bvn) pf_attr_override;                       \
};                                                                             \
pf_decl_static pf_decl_inline __pt_generate_benchmark_instance_name(name);     \
void __pt_generate_benchmark_type(name)::__proc(pul::__tester_benchmark &bvn)



/*
 * [HHHH:MM:SS:MMMM] /I/ Using Pulsar Tester
 *                   Running Pack "Global"
 *                   <Test>::<line>
 *                   ... X Times
 *                   (A) Success | (B) Failed
 *                   ...
 *
 *                   Pack "<PackName X>"
 *                   Benchmark "<BenchName>"
 *                   iterations   X        ...
 *                   Min (ns)   1438 ns    ...
 *                   Max (ns)   2000 ns    ...
 *                   Avg (ns)   1639 ns    ...
 *                   Var (ns)   1550 ns    ...
 *                   Sig (ns)   725  ns    ...
 *                   Q1  (ns)   1250 ns    ...
 *                   Q3  (ns)   1500 ns    ...
 *                   Total (ns)   N         M
 *
 *                   (X) Pack(s) ran
 *                   (U) Unit(s) ran
 *                   (B) Benchmark(s) ran
 *                   (A) Total Success | (B) Total Fails
 *                or All tests passed!
 */

// TODO: Move all to static library (better compilation time)

#endif // !PULSAR_TESTER_HPP