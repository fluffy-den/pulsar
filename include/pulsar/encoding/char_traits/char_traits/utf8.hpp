/*! @file   traits_utf8.hpp
 *  @author Louis-Quentin Noé (noe.louis-quentin@hotmail.fr)
 *  @brief
 *  @date   02-01-2023
 *
 *  @copyright Copyright (c) 2023 - Pulsar Software
 *
 *  @since 0.1.3
 */

#ifndef PULSAR_CHAR_TRAITS_UTF8_HPP
#define PULSAR_CHAR_TRAITS_UTF8_HPP 1

// Include: Pulsar
#include "pulsar/encoding/char_traits/char_traits.hpp"

// Pulsar
namespace pul
{
	// https://github.com/DuffsDevice/tiny-utf8
	// https://github.com/lemire/fastvalidate-utf-8/blob/master/include/simdasciicheck.h
	// https://github.com/simdutf/simdutf
	// https://dirtyhandscoding.github.io/posts/utf8lut-vectorized-utf-8-converter-introduction.html

	// Char Traits
namespace char_traits
{
	/// TRAITS: UTF8
	class utf8
	{
	public:
		using value_t			 = char_t;
		using code_value_t = uint32_t;

		// Len Of
		pf_hint_nodiscard pf_decl_static pf_decl_constexpr size_t
		lenof(
			const value_t * __str) pf_attr_noexcept
		{
			/*
			   if(!__str) throw(exception(
			                   std::errc::invalid_argument,
			                   "__str is nullptr!"));
			 */
			size_t len = 0;
			while(true)
			{
				if(*__str == '\0') return len;
				size_t c = codelenof(union_cast<char_t>(*__str));
				__str		+= c;
				++len;
			}
			return 0;
		}

		// Code Len
		pf_hint_nodiscard pf_decl_static pf_decl_constexpr size_t
		codelenof(
			code_value_t __v) pf_attr_noexcept
		{
			if((__v & 0xE0) == 0xC0) return 1;
			else if((__v & 0xF0) == 0xE0) return 2;
			else if((__v & 0xF8) == 0xF0) return 3;
			else if((__v & 0xFC) == 0xF8) return 4;
			/*
			   throw(exception(
			        std::errc::invalid_argument,
			        strfmt<char_traits_ascii>("__v = {} is invalid utf-8 code value!", __v)));
			 */
			return 0;
		}

		// Next Of
		pf_hint_nodiscard pf_decl_static pf_decl_constexpr value_t*
		nextof(
			value_t * __p) pf_attr_noexcept
		{
			return __p += codelenof(to_code(__p));
		}
		pf_hint_nodiscard pf_decl_static pf_decl_constexpr const value_t*
		nextof(
			const value_t * __p) pf_attr_noexcept
		{
			return __p += codelenof(to_code(__p));
		}

		// Prev Of
		pf_hint_nodiscard pf_decl_static pf_decl_constexpr value_t*
		prevof(
			value_t * __p) pf_attr_noexcept
		{
			// TODO: UTF8 -> Prev
			return nullptr;
		}
		pf_hint_nodiscard pf_decl_static pf_decl_constexpr const value_t*
		prevof(
			const value_t * __p) pf_attr_noexcept
		{
			// TODO: UTF8 -> Prev
			return nullptr;
		}

		// Is*
		pf_hint_nodiscard pf_decl_static pf_decl_constexpr bool
		is_valid(
			const value_t * __str) pf_attr_noexcept
		{
			if(!__str)
				return true;
			while(*__str != '\0')
			{
				size_t n;
				size_t cp;
				// Check code point length
				if((*__str & 0x80) == 0x00)
				{
					cp = (*__str & 0x7F);
					n	 = 1;
				}
				else if((*__str & 0xE0) == 0xC0)
				{
					cp = (*__str & 0x1F);
					n	 = 2;
				}
				else if((*__str & 0xF0) == 0xE0)
				{
					cp = (*__str & 0x0F);
					n	 = 3;
				}
				else if((*__str & 0xF8) == 0xF0)
				{
					cp = (*__str & 0x07);
					n	 = 4;
				}
				else return false;
				// Is valid code point sequence?
				++__str;
				for(size_t i = 1; i < n; ++n)
				{
					if((*__str & 0xC0) != 0x80)
						return false;
					++__str;
				}
				if((cp > 0x10FFFF)
					 || ((cp >= 0xD800) && (cp <= 0xDFFF))
					 || ((cp <= 0x007F) && (n != 1))
					 || ((cp >= 0x0080) && (cp <= 0x07FF) && (n != 2))
					 || ((cp >= 0x0800) && (cp <= 0xFFFF) && (n != 3))
					 || ((cp >= 0x10000) && (cp <= 0x1FFFFF) && (n != 4)))
					return false;
			}
			return true;

			// https://github.com/lemire/fastvalidate-utf-8/blob/master/include/simdutf8check.h
		}
		pf_hint_nodiscard pf_decl_static pf_decl_constexpr bool
		is_alnum(
			code_value_t __v) pf_attr_noexcept
		{
			// TODO: UTF8 -> is_alnum
			return false;
		}
		pf_hint_nodiscard pf_decl_static bool
		is_alpha(
			code_value_t __v) pf_attr_noexcept
		{
			// TODO: UTF8 -> is_alpha
			return false;
		}
		pf_hint_nodiscard pf_decl_static pf_decl_constexpr bool
		is_lower(
			code_value_t __v) pf_attr_noexcept
		{
			// TODO: UTF8 -> is_lower
			return false;
		}
		pf_hint_nodiscard pf_decl_static pf_decl_constexpr bool
		is_upper(
			code_value_t __v) pf_attr_noexcept
		{
			// TODO: UTF8 -> is_upper
			return false;
		}
		pf_hint_nodiscard pf_decl_static pf_decl_constexpr bool
		is_digit(
			code_value_t __v) pf_attr_noexcept
		{
			// TODO: UTF8 -> is_digit
			return false;
		}
		pf_hint_nodiscard pf_decl_static pf_decl_constexpr bool
		is_xdigit(
			code_value_t __v) pf_attr_noexcept
		{
			// TODO: UTF8 -> is_xdigit
			return false;
		}
		pf_hint_nodiscard pf_decl_static pf_decl_constexpr bool
		is_cntrl(
			code_value_t __v) pf_attr_noexcept
		{
			// TODO: UTF8 -> is_cntrl
			return false;
		}
		pf_hint_nodiscard pf_decl_static pf_decl_constexpr bool
		is_graph(
			code_value_t __v) pf_attr_noexcept
		{
			// TODO: UTF8 -> is_graph
			return false;
		}
		pf_hint_nodiscard pf_decl_static pf_decl_constexpr bool
		is_space(
			code_value_t __v) pf_attr_noexcept
		{
			// TODO: UTF8 -> is_space
			return false;
		}
		pf_hint_nodiscard pf_decl_static pf_decl_constexpr bool
		is_blank(
			code_value_t __v) pf_attr_noexcept
		{
			// TODO: UTF8 -> is_blank
			return false;
		}
		pf_hint_nodiscard pf_decl_static pf_decl_constexpr bool
		is_print(
			code_value_t __v) pf_attr_noexcept
		{
			// TODO: UTF8 -> is_print
			return false;
		}
		pf_hint_nodiscard pf_decl_static pf_decl_constexpr bool
		is_punct(
			code_value_t __v) pf_attr_noexcept
		{
			// TODO: UTF8 -> is_punct
			return false;
		}
		pf_hint_nodiscard pf_decl_static pf_decl_constexpr bool
		is_ascii(
			code_value_t __v) pf_attr_noexcept
		{
			// TODO: UTF8 -> is_ascii
			return false;
		}

		// To*
		pf_hint_nodiscard pf_decl_static pf_decl_constexpr code_value_t
		to_code(
			const value_t * __p) pf_attr_noexcept
		{
			code_value_t code = 0;
			// TODO: UTF8 -> to_code
			return code;
		}
		pf_hint_nodiscard pf_decl_static pf_decl_constexpr code_value_t
		to_upper(
			code_value_t __v) pf_attr_noexcept
		{
			// TODO: UTF8 -> to_upper
			return __v;
		}
		pf_hint_nodiscard pf_decl_static pf_decl_constexpr code_value_t
		to_lower(
			code_value_t __v) pf_attr_noexcept
		{
			// TODO: UTF8 -> to_lower
			return __v;
		}
		pf_hint_nodiscard pf_decl_static pf_decl_constexpr int8_t
		to_int8(
			const value_t * __b,
			size_t __maxlen) pf_attr_noexcept
		{
			// TODO: UTF8 -> to_int8
			return 0;
		}
		pf_hint_nodiscard pf_decl_static pf_decl_constexpr int16_t
		to_int16(
			const value_t * __b,
			size_t __maxlen) pf_attr_noexcept
		{
			// TODO: UTF8 -> to_int16
			return 0;
		}
		pf_hint_nodiscard pf_decl_static pf_decl_constexpr int32_t
		to_int32(
			const value_t * __b,
			size_t __maxlen) pf_attr_noexcept
		{
			// TODO: UTF8 -> to_int32
			return 0;
		}
		pf_hint_nodiscard pf_decl_static pf_decl_constexpr int64_t
		to_int64(
			const value_t * __b,
			size_t __maxlen) pf_attr_noexcept
		{
			// TODO: UTF8 -> to_int64
			return 0;
		}
		pf_hint_nodiscard pf_decl_static pf_decl_constexpr uint8_t
		to_uint8(
			const value_t * __b,
			size_t __maxlen) pf_attr_noexcept
		{
			// TODO: UTF8 -> to_uint8
			return 0;
		}
		pf_hint_nodiscard pf_decl_static pf_decl_constexpr uint16_t
		to_uint16(
			const value_t * __b,
			size_t __maxlen) pf_attr_noexcept
		{
			// TODO: UTF8 -> to_uint16
			return 0;
		}
		pf_hint_nodiscard pf_decl_static pf_decl_constexpr uint32_t
		to_uint32(
			const value_t * __b,
			size_t __maxlen) pf_attr_noexcept
		{
			// TODO: UTF8 -> to_uint32
			return 0;
		}
		pf_hint_nodiscard pf_decl_static pf_decl_constexpr uint64_t
		to_uint64(
			const value_t * __b,
			size_t __maxlen) pf_attr_noexcept
		{
			// TODO: UTF8 -> to_uint64
			return 0;
		}
		pf_hint_nodiscard pf_decl_static pf_decl_constexpr float32_t
		to_float32(
			const value_t * __b,
			size_t __maxlen) pf_attr_noexcept
		{
			// TODO: UTF8 -> to_float32
			return 0;
		}
		pf_hint_nodiscard pf_decl_static pf_decl_constexpr float64_t
		to_float64(
			const value_t * __b,
			size_t __maxlen) pf_attr_noexcept
		{
			// TODO: UTF8 -> to_float64
			return 0;
		}

		// Compare
		pf_decl_static pf_decl_constexpr diff_t
		compare(
			const value_t * __p1,
			const value_t * __p2,
			size_t __maxlen) pf_attr_noexcept
		{
			// TODO: UTF8 -> Compare
			return 0;
		}
	};
}
}

#endif // !PULSAR_TRAITS_UTF8_HPP