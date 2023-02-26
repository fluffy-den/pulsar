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

// Include: C++
#include <functional>

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
        dbg_u8string_view __name,
        size_t __itc,
        size_t __ntc) pf_attr_noexcept;

    /// Destructor
    ~__tester_benchmark() = default;

    /// Proc
    pf_decl_virtual void
    process(__tester_benchmark &__b) = 0;

		/// Run
    struct __results_t
    {
      nanoseconds_t min;
      nanoseconds_t max;
      nanoseconds_t avg;
      nanoseconds_t var;
      nanoseconds_t ect;
      nanoseconds_t q1;
      nanoseconds_t q2;
      nanoseconds_t q3;
      nanoseconds_t total;
    };

    /// Computation
    pulsar_api void
    __display_results(
        __results_t const &__r) pf_attr_noexcept;

    template <typename _Ty>
    void
    __compute_results(
      std::function<_Ty(size_t)> __measureFun,
      size_t __itc,
      size_t __off,
      nanoseconds_t *__results)
    {
      for(size_t i = __off, e = __off + __itc; i != e; ++i)
      {
        // 1. Measure
        high_resolution_point_t n		= high_resolution_clock_t::now();
        pf_hint_maybe_unused auto k = __measureFun(i);
        __results[i] = high_resolution_clock_t::now() - n;
        // 2. Anti-Optimize
        byte_t buf[sizeof(k)];
        std::memcpy(&buf[0], &k, sizeof(k)); // Anti-optimize!
      }
    }
    template <typename _Ty>
    void
    measure(
      std::function<_Ty(size_t)> __measureFun)
    {
      // 1. Measure
      const size_t ni				 = this->num_iterations();
      nanoseconds_t *results = union_cast<nanoseconds_t*>(allocate(ni * sizeof(nanoseconds_t), align_val_t(32), 0));
      std::thread *workers	 = union_cast<std::thread*>(allocate(ni * this->ntt_, align_val_t(alignof(std::thread)), 0));
      for (size_t i = 1; i < this->ntt_; ++i)
      {
        workers[i - 1] = std::thread([&]() { 
          __compute_results(__measureFun, this->itc_, this->itc_ * i, results); });
      }
      __compute_results(__measureFun, this->itc_, 0, results);
      for (size_t i = 0; i < this->ntt_; ++i)
      {
        if(workers[i - 1].joinable())
        {
          workers[i - 1].join();
        }
      }
      deallocate(workers);

      // 2. Compute Results
      __results_t r = { 
        nanoseconds_t(union_cast<size_t>(-1)), 
        nanoseconds_t(0), 
        nanoseconds_t(0), 
        nanoseconds_t(0), 
        nanoseconds_t(0), 
        nanoseconds_t(0), 
        nanoseconds_t(0), 
        nanoseconds_t(0), 
        nanoseconds_t(0) };
      nanoseconds_t avg2	 = nanoseconds_t(0);	// E[X^2]
      for (size_t i = 0; i < ni; ++i)
      {
        if (results[i] < r.min) r.min = results[i];
        if (results[i] > r.max) r.max = results[i];
        r.avg   += results[i];
        r.total += results[i];
      }
      r.avg  = nanoseconds_t(r.avg.count() / ni);
      for (size_t i = 0; i < ni; ++i)
      {
        const nanoseconds_t k = (results[i] - r.avg);
        r.var += nanoseconds_t(k.count() * k.count());
      }
      r.var  = nanoseconds_t(r.var.count() / ni);
      r.ect  = nanoseconds_t(static_cast<int64_t>(std::sqrt(static_cast<float64_t>(r.var.count()))));
      r.q1   = results[ni / 4];
      r.q2   = results[ni / 2];
      r.q3   = results[ni * 3 / 4];

      // 3. Display
      __display_results(r);
    }

    /// Name
    pf_decl_inline pf_decl_constexpr dbg_u8string_view
    name() const pf_attr_noexcept
    {
      return this->name_;
    }

    /// Num Iteration
    pf_decl_inline pf_decl_constexpr size_t 
    num_iterations() const pf_attr_noexcept
    {
      return this->itc_ * this->ntt_;
    }

    /// Num Threads
    pf_decl_inline pf_decl_constexpr size_t
    num_threads() const pf_attr_noexcept
    {
      return this->ntt_;
    }

		/// Store
    __tester_benchmark *next_;
    dbg_u8string_view name_;
    size_t itc_;
    size_t ntt_;
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

#define pt_benchmark(name, bvn, itc, ntt)                                      \  
class __pt_generate_benchmark_type(name)									                     \
	: public pul::__tester_benchmark                                             \
{																													                     \
public:																											                   \
  __pt_generate_benchmark_type(name)() pf_attr_noexcept                        \
    : pul::__tester_benchmark(__pt_generate_benchmark_name(name), itc, ntt)    \
  {}                                                                           \
  void                                                                         \
  process(pul::__tester_benchmark &bvn) pf_attr_override;                      \
};                                                                             \
pf_decl_static pf_decl_inline __pt_generate_benchmark_instance_name(name);     \
void __pt_generate_benchmark_type(name)::process(pul::__tester_benchmark &bvn)



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