/*! @file   char_traits_format.hpp
 *  @author Louis-Quentin Noé (noe.louis-quentin@hotmail.fr)
 *  @brief
 *  @date   02-01-2023
 *
 *  @copyright Copyright (c) 2023 - Pulsar Software
 *
 *  @since 0.1.3
 */

#ifndef PULSAR_CHAR_TRAITS_FORMAT_HPP
#define PULSAR_CHAR_TRAITS_FORMAT_HPP 1

// Include: Pulsar
#include "pulsar/encoding.hpp"
// Pulsar
namespace pul
{
	/*
	 * !        !
	 *   Traits
	 * !        !
	 */
	/// FORMAT
	// https://github.com/fmtlib/fmt

	/// TRAITS: Sring -> Format
	template<typename _Traits,
					 typename _Magnifier = magnifier_default,
					 typename _Allocator = allocator_default,
					 typename ... _Args>
	pf_hint_nodiscard pf_decl_constexpr char_traits_string<_Traits, _Magnifier, _Allocator>
	format(
		char_traits_string_view<_Traits> __fmt,
		_Args &&... __args) pf_attr_noexcept
	{
		// TODO: strfmt
		return "";
	}

	/// TRAITS: String -> Format To
	template<typename _Iterator, typename ... _Args>
	pf_decl_constexpr void
	format_to(
		_Iterator __b,
		char_traits_string_view<typename _Iterator::traits_t> __fmt,
		_Args &&... __args) pf_attr_noexcept
	{
		// TODO: strfmt_to
	}

	/// TRAITS: String -> Print To
	template<typename _Stream, typename _Traits, typename ... _Args>
	pf_decl_constexpr void
	print(
		_Stream &__stream,
		char_traits_string_view<_Traits> __fmt,
		_Args &&... __args) pf_attr_noexcept
	{
		// TODO: strprint
	}
}

#endif // !PULSAR_CHAR_TRAITS_FORMAT_HPP