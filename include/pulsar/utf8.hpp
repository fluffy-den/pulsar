/*! @file   utf8.hpp
 *  @author Louis-Quentin Noé (noe.louis-quentin@hotmail.fr)
 *  @brief
 *  @date   24-03-2023
 *
 *  @copyright Copyright (c) 2023 - Pulsar Software
 *
 *  @since 0.1.4
 */

#ifndef PULSAR_UTF8_HPP
#define PULSAR_UTF8_HPP 1

// Include: Pulsar
#include "pulsar/pulsar.hpp"
#include "pulsar/debug.hpp"

// Pulsar
namespace pul
{
	// https://github.com/DuffsDevice/tiny-utf8/blob/master/include/tinyutf8/tinyutf8.h

	/// UTF8: Iterator
	template <typename _StringTy>
	class u8iterator
	{
	// TODO: u8iterator
	};

	/// UTF8: String
	template <
		typename _Magnifier = magnifier_default,
		typename _Allocator = allocator_default>
	class u8string
	{
	// TODO: u8string
	};
	using u8string_t = u8string<magnifier_default, allocator_default>;
	template <typename ... _Args>
	using u8string_format = fmt::format_string<_Args...>;


	/// UTF8: Format
	template <typename ... _Args>
	pf_hint_nodiscard u8string_t
	u8formatted_size(
		u8string_format<_Args...> __fmt,
		_Args && ... __args) pf_attr_noexcept
	{
		return fmt::formatted_size(__fmt, std::forward<_Args>(__args)...);
	}
	template <typename ... _Args>
	pf_hint_nodiscard u8string_t
	u8format(
		u8string_format<_Args...> __fmt,
		_Args && ... __args) pf_attr_noexcept
	{
		// TODO: u8format
	}
	template <
		typename _IteratorTy,
		typename ... _Args>
	pf_hint_nodiscard _IteratorTy
	u8format_to(
		_IteratorTy __w,
		u8string_format<_Args...> __fmt,
		_Args && ... __args) pf_attr_noexcept
	{
		// TODO: u8format_to
	}
}

#endif // !PULSAR_UTF8_HPP