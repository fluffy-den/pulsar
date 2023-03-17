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
#include "pulsar/function.hpp"
#include "pulsar/concurrency.hpp"

// Include: C++
#include <thread>

// Include: C
#include <intrin.h>

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
		pulsar_api __tester_unit(
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
		/// Type
    using __tester_thread_t = std::thread;

  public:
		/// Constructor
    pulsar_api __tester_benchmark(
        dbg_u8string_view __name,
        size_t __itc,
        size_t __ntc = 1) pf_attr_noexcept;

    /// Destructor
    pulsar_api ~__tester_benchmark() = default;

    /// Proc
    pf_decl_virtual void
    process(
      __tester_benchmark &__b) = 0;

    /// Display
    pulsar_api void
    __display_measures(
        uint64_t *__rts,
        const size_t __c) pf_attr_noexcept; 

    /// Computation
    template <typename _FunTy>
    void
    pf_decl_inline pf_decl_static __measure_proc(
      _FunTy &&__measureFun,
      size_t __itc,
      size_t __off,
      uint64_t *__results)
    {
      // Measure
      for (size_t i = __off, e = __off + __itc; i != e; ++i)
      {
        uint64_t s = __rdtsc();
        pf_hint_maybe_unused pf_decl_volatile auto k = __measureFun(i);
        __results[i] = __rdtsc() - s;
      }
    }
    template <typename _FunTy>
    void
    measure(
      _FunTy &&__measureFun)
    {
      // Anti-Boil (Bandwith Overhead + Throll Overhead)
      this_thread::sleep_for(seconds_t(1));

      // Measure
      const size_t ni = this->num_iterations();
      const size_t rs = sizeof(uint64_t) * ni;
      const size_t ss = sizeof(__tester_thread_t) * this->ntt_;
      const size_t ts = rs + ss;
      byte_t *store = union_cast<byte_t *>(halloc(ts, align_val_t(32)));
      std::memset(store, 0, ts);
      __tester_thread_t *workers = union_cast<__tester_thread_t*>(&store[0]);
      uint64_t *results = union_cast<uint64_t*>(&store[0] + ss);
      for (size_t i = 1; i < this->ntt_; ++i)
      {
        workers[i - 1] = __tester_thread_t(
          this->__measure_proc<_FunTy>,
          std::move(__measureFun), 
          this->itc_, 
          this->itc_ * i, 
          results);
      }
      this->__measure_proc(std::move(__measureFun), this->itc_, 0, results);

      // End Threads
      for (size_t i = 1; i < this->ntt_; ++i)
      {
        if(workers[i - 1].joinable()) workers[i - 1].join();
      }

      // Compute
      __display_measures(results, ni);

      // Deallocate
      hfree(store);
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
		__tester_exception_require() pf_attr_noexcept                 = default;
		__tester_exception_require(__tester_exception_require const&) = default;
		__tester_exception_require(__tester_exception_require &&)			= default;

		/// Destructor
		~__tester_exception_require() pf_attr_noexcept = default;

		/// Operator =
		__tester_exception_require &operator=(__tester_exception_require const &) = default;
		__tester_exception_require &operator=(__tester_exception_require &&)			= default;
	};

	/// TESTER: Unit -> Pack
	class __tester_pack
	{
	public:
		/// Constructors
		pulsar_api __tester_pack(
			dbg_u8string_view __name) pf_attr_noexcept;
		__tester_pack(__tester_pack const &) = delete;
		__tester_pack(__tester_pack &&)			 = delete;

		/// Destructor
		pulsar_api ~__tester_pack() pf_attr_noexcept = default;

		/// Operator =
		__tester_pack &operator=(__tester_pack const &) = delete;
		__tester_pack &operator=(__tester_pack &&)			= delete;

		/// Run
		pulsar_api void
		__run() pf_attr_noexcept;

		/// Unit
		pulsar_api void
		__add_unit(
			__tester_unit *__u) pf_attr_noexcept;
    pulsar_api void
    __add_benchmark(
      __tester_benchmark *__b) pf_attr_noexcept;
    pulsar_api void
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
		pulsar_api __tester_engine() pf_attr_noexcept;
		__tester_engine(__tester_engine const &) = delete;
		__tester_engine(__tester_engine &&)			 = delete;

		/// Destructor
		pulsar_api ~__tester_engine() pf_attr_noexcept = default;

		/// Run
		pf_hint_nodiscard pulsar_api int32_t
		run() pf_attr_noexcept;

		/// Operator =
		__tester_engine &operator=(
			__tester_engine const &) = delete;
		__tester_engine &operator=(
			__tester_engine &&) = delete;

		/// Test
		pulsar_api void
		__test(
			bool __c,
			dbg_u8string_view __file,
			uint32_t __line) pf_attr_noexcept;

		/// Pack
		pulsar_api void
		__add_pack(
			__tester_pack *__p) pf_attr_noexcept;
		pf_hint_nodiscard pulsar_api __tester_pack*
		__cur_pack() pf_attr_noexcept;

		/// Store
		__tester_pack unscoppedPack_;
		__tester_pack *packHead_;
    __tester_pack *packTail_;
    __tester_pack *packCurr_;
	};

	/// TESTER: Instances
	pf_decl_extern __tester_engine tester_engine;

	/// TESTER: Functions
	pf_decl_static pulsar_api void __test_require(
		bool __cond,
		dbg_u8string_view __file,
		uint32_t __line)
	{
    pul::tester_engine.__test(__cond, __file, __line);
    if(!__cond) throw(__tester_exception_require());
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
pf_decl_static pf_decl_inline __pt_generate_pack_instance_name(name);   \
namespace


/// TESTER: Macro -> Benchmark
#define __pt_generate_benchmark_name(name) pf_stringize(name)
#define __pt_generate_benchmark_type(name) pf_concatenate(pf_concatenate(__pt_benchmark_, name), _t)
#define __pt_generate_benchmark_instance_name(name) __pt_generate_benchmark_type(name) pf_concatenate(pf_concatenate(__pt_benchmark_, name), _instance)

#define pt_benchmark(name, bvn, ...)                                           \  
class __pt_generate_benchmark_type(name)									                     \
	: public pul::__tester_benchmark                                             \
{																													                     \
public:																											                   \
  __pt_generate_benchmark_type(name)() pf_attr_noexcept                        \
    : pul::__tester_benchmark(__pt_generate_benchmark_name(name), __VA_ARGS__) \
  {}                                                                           \
  void                                                                         \
  process(pul::__tester_benchmark &bvn) pf_attr_override;                      \
};                                                                             \
pf_decl_static pf_decl_inline __pt_generate_benchmark_instance_name(name);     \
void __pt_generate_benchmark_type(name)::process(pul::__tester_benchmark &bvn)

// TODO: Write down second to greater!

#endif // !PULSAR_TESTER_HPP