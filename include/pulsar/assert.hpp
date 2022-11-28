/*! @file   assert.hpp
 *  @author Fluffy (noe.louis-quentin@hotmail.fr)
 *  @brief
 *  @date   12-09-2022
 *
 *  @copyright Copyright (c) 2022 - Pulsar Software
 *
 *  @since 0.1.2
 */

#ifndef PULSAR_ASSERT_HPP
#define PULSAR_ASSERT_HPP 1

// Include: Pulsar
#include "pulsar/pulsar.hpp"

// Include: C++
#include <type_traits>
#include <utility>

// Pulsar
namespace pul
{

	// https://github.com/saarraz/static-print

	/// ASSERT: Static Print
	template <typename ..._Args>
	pf_decl_constexpr void
	__assert_print
	(
		const char *__file,
		const char *__func,
		size_t __line,
		const char *__fmt,
		_Args && ... __args) pf_attr_noexcept
	{
		// TODO: Constexpr Assert Print
	}

	/// ASSERT: Warning
	template <typename ..._Args>
	pf_decl_constexpr void
	assert_no_abort(
		bool __cond,
		const char *__file,
		const char *__func,
		size_t __line,
		const char *__fmt,
		_Args && ... __args) pf_attr_noexcept
	{
		if(!__cond)
		{
			__assert_print(__file, __func, __line, __fmt, std::forward<_Args>(__args)...);
		}
	}

	/// ASSERT: Abort
	template <typename ..._Args>
	pf_decl_constexpr void
	assert(
		bool __cond,
		const char *__file,
		const char *__func,
		size_t __line,
		const char *__fmt,
		_Args && ... __args) pf_attr_noexcept
	{
		if(!__cond)
		{
			__assert_print(__file, __func, __line, __fmt, std::forward<_Args>(__args)...);
			std::abort();
		}
	}

	// pf_assert
#ifdef PF_DEBUG
	// pf_assert
# define pf_assert(cond, ...)  pul::assert(cond, pf_file, pf_function, pf_line, __VA_ARGS__)
	// pf_assert_no_abort
# define pf_warning(cond, ...) pul::assert_no_abort(cond, pf_file, pf_function, pf_line, __VA_ARGS__)
#else // ^^^ PF_DEBUG ^^^ / vvv !PF_DEBUG vvv
	// pf_assert
# define pf_assert(...)
	// pf_assert_no_abort
# define pf_warning(...)
#endif // PF_DEBUG
	// pf_static_assert
#define pf_assert_s(...) static_assert(__VA_ARGS__)
}


#endif // !PULSAR_ASSERT_HPP