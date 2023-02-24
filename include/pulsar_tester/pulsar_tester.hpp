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
#include "pulsar/container.hpp"

// Pulsar
namespace pul
{
	struct __test_result_t;
	class __test_engine;
	class __test_runner;
	class __test_unit;
	class __test_pack;

	/// TEST: Engine
	class __test_engine
	{
	public:
	// TODO:

	private:
		forward_list<__test_pack> packs_;
	};

	/// TEST: Pack
	class __test_pack
	{
	public:
	// TODO:

	private:
		forward_list<__test_unit> units_;
	};

  #define __pf_test_class_name(name) pf_concatenate("__ptf", name);

	/// TEST: Unit
	class __test_unit: public __test_pack
	{
	public:
	// TODO:

	private:
	};
}

#define pf_suite(name) 																						    \
   pf_static_assert(sizeof(sn) > 1, "Suite name mustn't be empty!");	\
   class __pf_test_class_name(sn) : public pul::__test_suite					\
   {																																	\
   public:																														\
    pf_decl_constexpr __pf_test_class_name(sn)() pf_attr_noexcept		  \
    : pul::__test_suite(sn)																					  \
    {}																															  \
   };

/* namespace pul
   {

   struct __test_result_t;
   class __test_engine;
   class __test_runner;
   class __test_unit;
   class __test_suite;

   /// TEST: Engine
   class __test_engine
   {
   public:
    /// Constructors
    pf_decl_constexpr __test_engine() pf_attr_noexcept
    {
      pf_assert(this->suites_.is_empty(), "Suites aren't empty!");
      // TODO: Print Testing Start
    }
    __test_engine(__test_engine const &) = delete;
    __test_engine(__test_engine &&)			 = delete;

    /// Destructor
    pf_decl_constexpr ~__test_engine() pf_attr_noexcept
    {
      // TODO: Print Results
      this->suites_.clear();
    }

    /// Operator =
    pf_decl_constexpr
    __test_engine &
    operator =(__test_engine const &) = delete;
    pf_decl_constexpr
    __test_engine &
    operator =(__test_engine &&) = delete;

    /// Suite -> Add
    pf_decl_constexpr void
    __add_suite(
      const __test_suite *__suite) pf_attr_noexcept
    {
      // TODO: Check if suite name already exist
      this->suites_.insert_back(__suite);
    }

    /// Suite -> Process All
    pf_decl_constexpr void
    __process_all() pf_attr_noexcept
    {
      for(auto &it : this->suites_)
      {
        it.__process();
      }
    }

   private:
    list_forward<__test_suite *> suites_;
   };

   /// TEST: Result
   struct __test_result_t
   {
    nanosecond_t total;
    nanosecond_t average;
    nanosecond_t min;
    nanosecond_t max;
   };

   /// TEST: Runner
   class __test_runner
   {
   public:
    /// Constructors
    pf_decl_constexpr
    __test_runner()											 = default;
    pf_decl_constexpr
    __test_runner(__test_runner const &) = delete;
    pf_decl_constexpr
    __test_runner(__test_runner &&)			 = delete;

    /// Destructor
    pf_decl_constexpr
      pf_decl_virtual
    ~__test_runner() pf_attr_noexcept = default;

    /// Operator
    __test_runner &
    operator =(__test_runner const &) = delete;
    __test_runner &
    operator =(__test_runner &&)			= delete;

    /// Execute
    pf_decl_virtual void
    __execute() pf_attr_noexcept = 0;
   };

   /// TEST: Suite
   class __test_suite
   {
   public:
    /// Constructors
    pf_decl_constexpr
    __test_suite(
      const char_t *__name) pf_attr_noexcept
      : name_(__name)
    {
      __test_engine::__add_suite(this);
    }
    __test_suite(__test_suite const &) = delete;
    __test_suite(__test_suite &&)			 = delete;

    /// Destructor
    pf_decl_constexpr
    ~__test_suite() pf_attr_noexcept = default;

    /// Operator=
    pf_decl_constexpr __test_suite &
    operator =(__test_suite const &) = delete;
    pf_decl_constexpr __test_suite &
    operator =(__test_suite &&)			 = delete;

    /// Runner -> Process
    pf_decl_constexpr void
    __process() pf_attr_noexcept
    {
      for(auto &it : this->runners_)
      {
        it->a->__execute();
      }
    }

    /// Runner -> Add
    pf_decl_constexpr void
    __add_runner(
      const __test_runner *__runner) pf_attr_noexcept
    {
      // TODO: Check if runner's name already exist.
      this->runners_.insert_back(__runner, true);
    }

   private:
    const char_t *name_;
    list_forward<pair<__test_runner *, bool>> runners_;
   };

   /// TEST: Unit
   class __test_unit : public __test_runner
   {
   public:
    /// Constructors
    pf_decl_constexpr
    __test_unit(
      string_span<char_traits_ascii> __name)
      : name_(__name)
    {
      // TODO: Link to __test_engine
    }
    __test_unit(__test_unit const &) = delete;
    __test_unit(__test_unit &&)			 = delete;

    /// Destructor
    pf_decl_constexpr
    ~__test_unit() pf_attr_noexcept = default;

    /// Operator =
    __test_unit &
    operator =(__test_unit const &) = delete;
    __test_unit &
    operator =(__test_unit &&)			= delete;

    /// Execute
    pf_decl_virtual
    void
    __execute() pf_attr_noexcept = 0;

   private:
    const char_t *name_;
   };

   /// TEST: Unit -> Benchmark
   class test_benchmark pf_attr_final : public __test_runner
   {
   public:
    /// Measure
    enum measure_t
    {
      measure_each,
      measure_all
    };

    /// Constructors
    pf_decl_constexpr
    test_benchmark(
      const char_t *__name,
      size_t __i)
    {}
    test_benchmark(test_benchmark const &) = delete;
    test_benchmark(test_benchmark &&)			 = delete;

    /// Destructor
    pf_decl_constexpr
    test_benchmark() pf_attr_noexcept = default;

    /// Execute
    void
    execute() pf_attr_noexcept
    {}

    /// Add Runner
    template <typename _Ret>
    void
    add_runner(
      fun_buf<_Ret(size_t)> __proc);

   private:
    size_t num_;
   };
   }

   /// TEST: Macros

   // TODO: Check if their isn't any parent
 #define pf_test_suite(sn)																						\
   pf_static_assert(sizeof(sn) > 1, "Suite name mustn't be empty!");	\
   class __pf_test_class_name(sn) : public pul::__test_suite					\
   {																																	\
   public:																														\
    pf_decl_constexpr __pf_test_class_name(sn)() pf_attr_noexcept		\
    : pul::__test_suite(sn)																					\
    {}																															\
   };

   // TODO: Check if parent scope is a suite
 #define pf_test_unit(un)																					 \
   pf_static_assert(sizeof(un) > 1, "Unit name mustn't be empty!"); \
   class __pf_test_class_name(sn) : public pul::__test_unit				 \
   {																																 \
   public:																													 \
    pf_decl_constexpr __pf_test_class_name(un)() pf_attr_noexcept	 \
    : pul::__test_unit(un)																				 \
    {}																														 \
 \
    pf_decl_constexpr void																				 \
    __execute();																									 \
 \
   };																															 \
   void __pf_test_class_name(sn)::__execute()

   // TODO: Check if parent scope is a suite
 #define pf_test_benchmark(bn)
   pf_static_assert(sizeof(bn) > 1, "Benchmark name mustn't be empty!");	\
   class __pf_test_class_name(bn) : public pul::test_benchmark					\
   {																																		\
   public:																																\
   };																																	\
   void __pf_test_class_name(bn)::execute()														\

   // TODO: Check if parent scope is a unit
 #define pf_require(assert) \
   if(!(assert))						 \
   {												 \
 \
   }

 #define pf_require_catch(function, exception)	\

 #define pf_require_no_catch(function)	\

   // TODO: Check if parent scope is a unit
 #define pf_check(assert) \
   if(!(assert))					 \
   {											 \
 \
   }

 #define pf_check_catch(function, exception)	\

 #define pf_check_no_catch(function)	\ */

#endif // !PULSAR_TESTER_HPP