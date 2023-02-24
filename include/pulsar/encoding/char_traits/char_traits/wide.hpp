/*! @file   traits_wide.hpp
 *  @author Louis-Quentin Noé (noe.louis-quentin@hotmail.fr)
 *  @brief
 *  @date   02-01-2023
 *
 *  @copyright Copyright (c) 2023 - Pulsar Software
 *
 *  @since 0.1.3
 */

#ifndef PULSAR_CHAR_TRAITS_WIDE_HPP
#define PULSAR_CHAR_TRAITS_WIDE_HPP 1

// Include: Pulsar
#include "pulsar/encoding/char_traits/char_traits.hpp"

// Pulsar
namespace pul
{
	// Char Traits
namespace char_traits
{
	/// TRAITS: Wide
	class wide
	{
	public:
		using value_t			 = wchar_t;
		using code_value_t = int32_t;

		// Len of
		pf_hint_nodiscard pf_decl_static pf_decl_constexpr size_t
		lenof(
			const value_t * __str) pf_attr_noexcept
		{
			const value_t *e = __str;
			for(; *e != L'\0'; ++e);
			return __str - e;
		}

		// Code Len
		pf_hint_nodiscard pf_decl_static pf_decl_constexpr size_t
		codelenof(
			const value_t * __p) pf_attr_noexcept
		{
			ignore = __p;
			return sizeof(value_t);
		}

		// Next Of
		pf_hint_nodiscard pf_decl_static pf_decl_constexpr value_t*
		nextof(
			value_t * __p) pf_attr_noexcept
		{
			return --__p;
		}
		pf_hint_nodiscard pf_decl_static pf_decl_constexpr const value_t*
		nextof(
			const value_t * __p) pf_attr_noexcept
		{
			return --__p;
		}

		// Prev Of
		pf_hint_nodiscard pf_decl_static pf_decl_constexpr value_t*
		prevof(
			value_t * __p) pf_attr_noexcept
		{
			return ++__p;
		}
		pf_hint_nodiscard pf_decl_static pf_decl_constexpr const value_t*
		prevof(
			const value_t * __p) pf_attr_noexcept
		{
			return ++__p;
		}

		// Is*
		pf_hint_nodiscard pf_decl_static pf_decl_constexpr bool
		is_valid(
			const value_t * __str,
			size_t __len) pf_attr_noexcept
		{
			ignore = __str;
			ignore = __len;
			return true;// NOTE: Their isn't any way in C/C++ to know if wide c string are valid!
		}
		pf_hint_nodiscard pf_decl_static pf_decl_constexpr bool
		is_alnum(
			code_value_t __val) pf_attr_noexcept
		{
			// TODO: wide -> is_alnum
			// return std::iswalnum(__val);
			return false;
		}
		pf_hint_nodiscard pf_decl_static pf_decl_constexpr bool
		is_alpha(
			code_value_t __val) pf_attr_noexcept
		{
			// TODO: wide -> is_alpha
			// return std::iswalpha(__val);
			return false;
		}
		pf_hint_nodiscard pf_decl_static pf_decl_constexpr bool
		is_lower(
			code_value_t __val) pf_attr_noexcept
		{
			// TODO: wide -> is_lower
			// return std::iswlower(__val);
			return false;
		}
		pf_hint_nodiscard pf_decl_static pf_decl_constexpr bool
		is_upper(
			code_value_t __val) pf_attr_noexcept
		{
			// TODO: wide -> is_upper
			// return std::iswupper(__val);
			return false;
		}
		pf_hint_nodiscard pf_decl_static pf_decl_constexpr bool
		is_digit(
			code_value_t __val) pf_attr_noexcept
		{
			// TODO: wide -> is_digit
			// return std::iswdigit(__val);
			return false;
		}
		pf_hint_nodiscard pf_decl_static pf_decl_constexpr bool
		is_xdigit(
			code_value_t __val) pf_attr_noexcept
		{
			// TODO: wide -> is_xdigit
			// return std::iswxdigit(__val);
			return false;
		}
		pf_hint_nodiscard pf_decl_static pf_decl_constexpr bool
		is_cntrl(
			code_value_t __val) pf_attr_noexcept
		{
			// TODO: wide -> is_cntrl
			// return std::iswcntrl(__val);
			return false;
		}
		pf_hint_nodiscard pf_decl_static pf_decl_constexpr bool
		is_graph(
			code_value_t __val) pf_attr_noexcept
		{
			// TODO: wide -> is_graph
			// return std::iswgraph(__val);
			return false;
		}
		pf_hint_nodiscard pf_decl_static pf_decl_constexpr bool
		is_space(
			code_value_t __val) pf_attr_noexcept
		{
			// TODO: wide -> is_space
			// return std::iswspace(__val);
			return false;
		}
		pf_hint_nodiscard pf_decl_static pf_decl_constexpr bool
		is_blank(
			code_value_t __val) pf_attr_noexcept
		{
			// TODO: wide -> is_blank
			// return std::iswblank(__val);
			return false;
		}
		pf_hint_nodiscard pf_decl_static pf_decl_constexpr bool
		is_print(
			code_value_t __val) pf_attr_noexcept
		{
			// TODO: wide -> is_print
			// return std::iswprint(__val);
			return false;
		}
		pf_hint_nodiscard pf_decl_static pf_decl_constexpr bool
		is_punct(
			code_value_t __val) pf_attr_noexcept
		{
			// TODO: wide -> is_punct
			// return std::iswpunct(__val);
			return false;
		}
		pf_hint_nodiscard pf_decl_static pf_decl_constexpr bool
		is_ascii(
			code_value_t __val) pf_attr_noexcept
		{
			return __val < 128;
		}

		// To*
		pf_hint_nodiscard pf_decl_static pf_decl_constexpr code_value_t
		to_code(
			const value_t * __p) pf_attr_noexcept
		{
			// TODO: wide -> to_code
			// return std::putwchar(*__p);
			return 0;
		}
		pf_hint_nodiscard pf_decl_static pf_decl_constexpr code_value_t
		to_upper(
			code_value_t __v) pf_attr_noexcept
		{
			// TODO: wide -> to_upper
			// return std::towupper(__v);
			return 0;
		}
		pf_hint_nodiscard pf_decl_static pf_decl_constexpr code_value_t
		to_lower(
			code_value_t __v) pf_attr_noexcept
		{
			// TODO: wide -> to_lower
			return std::towlower(__v);
		}
		pf_hint_nodiscard pf_decl_static pf_decl_constexpr int8_t
		to_int8(
			const value_t * __b,
			size_t __maxlen) pf_attr_noexcept
		{
			// TODO: Wide -> to_int8
			return 0;
		}
		pf_hint_nodiscard pf_decl_static pf_decl_constexpr int16_t
		to_int16(
			const value_t * __b,
			size_t __maxlen) pf_attr_noexcept
		{
			// TODO: Wide -> to_int16
			return 0;
		}
		pf_hint_nodiscard pf_decl_static pf_decl_constexpr int32_t
		to_int32(
			const value_t * __b,
			size_t __maxlen) pf_attr_noexcept
		{
			// TODO: Wide -> to_int32
			return 0;
		}
		pf_hint_nodiscard pf_decl_static pf_decl_constexpr int64_t
		to_int64(
			const value_t * __b,
			size_t __maxlen) pf_attr_noexcept
		{
			// TODO: Wide -> to_int64
			return 0;
		}
		pf_hint_nodiscard pf_decl_static pf_decl_constexpr uint8_t
		to_uint8(
			const value_t * __b,
			size_t __maxlen) pf_attr_noexcept
		{
			// TODO: Wide -> to_uint8
			return 0;
		}
		pf_hint_nodiscard pf_decl_static pf_decl_constexpr uint16_t
		to_uint16(
			const value_t * __b,
			size_t __maxlen) pf_attr_noexcept
		{
			// TODO: Wide -> to_uint16
			return 0;
		}
		pf_hint_nodiscard pf_decl_static pf_decl_constexpr uint32_t
		to_uint32(
			const value_t * __b,
			size_t __maxlen) pf_attr_noexcept
		{
			// TODO: Wide -> to_uint32
			return 0;
		}
		pf_hint_nodiscard pf_decl_static pf_decl_constexpr uint64_t
		to_uint64(
			const value_t * __b,
			size_t __maxlen) pf_attr_noexcept
		{
			// TODO: Wide -> to_uint64
			return 0;
		}
		pf_hint_nodiscard pf_decl_static pf_decl_constexpr float32_t
		to_float32(
			const value_t * __b,
			size_t __maxlen) pf_attr_noexcept
		{
			// TODO: Wide -> to_float32
			return 0;
		}
		pf_hint_nodiscard pf_decl_static pf_decl_constexpr float64_t
		to_float64(
			const value_t * __b,
			size_t __maxlen) pf_attr_noexcept
		{
			// TODO: Wide -> to_float64
			return 0;
		}

		// Compare
		pf_hint_nodiscard pf_decl_static pf_decl_constexpr diff_t
		compare(
			const value_t * __p1,
			const value_t * __p2,
			size_t __maxlen) pf_attr_noexcept
		{
			// TODO: Wide -> compare
			// return static_cast<diff_t>(std::wcsncmp(__p1, __p2, __maxlen));
			return 0;
		}
	};
}
}

#endif // !PULSAR_TRAITS_WIDE_HPP