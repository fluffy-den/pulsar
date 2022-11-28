/*! @file   charcode.hpp
 *  @author Louis-Quentin NOE (noe.louis-quentin@hotmail.fr)
 *  @brief
 *  @date   21-11-2022
 *
 *  @copyright Copyright (c) 2022 - Pulsar Software
 *
 *  @since 0.1.2
 */

#ifndef PULSAR_CHARCODE_HPP
#define PULSAR_CHARCODE_HPP 1

// Include: Pulsar
#include "pulsar/sequence.hpp"

// Include: C++
#include <locale>
#include <byte>
#include <cwchar>

// Pulsar
namespace pul
{
	// TODO: toatoi / l / ll etc...

	// https://utf8everywhere.org/
	// https://github.com/DuffsDevice/tiny-utf8

	/// CHAR: String
	template <
		typename _CharTraits,
		typename _Magnifier = magnifier_default,
		typename _Allocator = allocator_malloc>
	class charcode_string;

	/// CHAR: String View
	template <
		typename _CharTraits>
	class charcode_string_view;

	/// CHAR: Iterator
	template <typename _CharTraits>
	class charcode_iterator;
	template <typename _CharTraits>
	class charcode_const_iterator;
	template <typename _CharTraits>
	class charcode_reverse_iterator;
	template <typename _CharTraits>
	class charcode_const_reverse_iterator;
	template <typename _CharTraits, typename _String>
	class charcode_iterator_variable;
	template <typename _CharTraits, typename _String>
	using charcode_const_iterator_variable = charcode_const_iterator<_CharTraits>;
	template <typename _CharTraits, typename _String>
	class charcode_reverse_iterator_variable;
	template <typename _CharTraits, typename _String>
	using charcode_const_reverse_iterator_variable = charcode_const_reverse_iterator<_CharTraits>;




	/*
	 * !       !
	 *   ASCII
	 * !       !
	 */
	/// ASCII: Lenof
	pf_hint_nodiscard pf_decl_constexpr size_t
	asciilenof(
		const char *__str) pf_attr_noexcept
	{
		return std::strlen(__str);
	}

	/// ASCII: Is
	pf_hint_nodiscard pf_decl_constexpr bool
	asciisalnum(
		int32_t __val) pf_attr_noexcept
	{
		return std::isalnum(__val, std::locale::classic());
	}
	pf_hint_nodiscard pf_decl_constexpr bool
	asciisalpha(
		int32_t __val) pf_attr_noexcept
	{
		return std::isalpha(__val, std::locale::classic());
	}
	pf_hint_nodiscard pf_decl_constexpr bool
	asciislower(
		int32_t __val) pf_attr_noexcept
	{
		return std::islower(__val, std::locale::classic());
	}
	pf_hint_nodiscard pf_decl_constexpr bool
	asciisupper(
		int32_t __val) pf_attr_noexcept
	{
		return std::isupper(__val, std::locale::classic());
	}
	pf_hint_nodiscard pf_decl_constexpr bool
	asciisdigit(
		int32_t __val) pf_attr_noexcept
	{
		return std::isdigit(__val, std::locale::classic());
	}
	pf_hint_nodiscard pf_decl_constexpr bool
	asciisxdigit(
		int32_t __val) pf_attr_noexcept
	{
		return std::isxdigit(__val, std::locale::classic());
	}
	pf_hint_nodiscard pf_decl_constexpr bool
	asciiscntrl(
		int32_t __val) pf_attr_noexcept
	{
		return std::iscntrl(__val, std::locale::classic());
	}
	pf_hint_nodiscard pf_decl_constexpr bool
	asciisgraph(
		int32_t __val) pf_attr_noexcept
	{
		return std::isgraph(__val, std::locale::classic());
	}
	pf_hint_nodiscard pf_decl_constexpr bool
	asciisspace(
		int32_t __val) pf_attr_noexcept
	{
		return std::isspace(__val, std::locale::classic());
	}
	pf_hint_nodiscard pf_decl_constexpr bool
	asciisblank(
		int32_t __val) pf_attr_noexcept
	{
		return std::isblank(__val, std::locale::classic());
	}
	pf_hint_nodiscard pf_decl_constexpr bool
	asciisprint(
		int32_t __val) pf_attr_noexcept
	{
		return std::isprint(__val, std::locale::classic());
	}
	pf_hint_nodiscard pf_decl_constexpr bool
	asciispunct(
		int32_t __val) pf_attr_noexcept
	{
		return std::ispunct(__val, std::locale::classic());
	}

	/// ASCII: To*
	pf_hint_nodiscard pf_decl_constexpr int32_t
	asciitoupper(
		int32_t __v) pf_attr_noexcept
	{
		return std::toupper(__v, std::locale::classic());
	}
	pf_hint_nodiscard pf_decl_constexpr int32_t
	asciitolower(
		int32_t __v) pf_attr_noexcept
	{
		return std::tolower(__v, std::locale::classic());
	}

	/// ASCII: Traits
	struct ascii_traits_t
	{
		using value_t					 = char_t;
		using code_value_t		 = int32_t;
		using iterator_t			 = charcode_iterator<ascii_traits_t>;
		using const_iterator_t = charcode_const_iterator<ascii_traits_t>;
		using iterator_t			 = charcode_reverse_iterator<ascii_traits_t>;
		using const_iterator_t = charcode_const_reverse_iterator<ascii_traits_t>;

		// To code
		pf_decl_static pf_decl_constexpr code_value_t
		to_code(
			value_t *__p) pf_attr_noexcept
		{
			return static_cast<code_value_t>(*__p);
		}

		// Next
		pf_decl_static pf_decl_constexpr value_t *
		next(
			value_t *__p) pf_attr_noexcept
		{
			return ++__p;
		}

		// Prev
		pf_decl_static pf_decl_constexpr value_t *
		prev(
			value_t *__p) pf_attr_noexcept
		{
			return --__p;
		}

		// Code Len
		pf_decl_static pf_decl_constexpr size_t
		code_len(
			const value_t *__p) pf_attr_noexcept
		{
			ignore = __p;
			return sizeof(value_t);
		}

		// Is Valid
		pf_decl_static pf_decl_constexpr bool
		is_valid(
			const value_t *__str) pf_attr_noexcept
		{
			ignore = __str;
			return true;
		}

		// Compare
		pf_decl_static pf_decl_constexpr diff_t
		compare(
			const value_t *__p1,
			const value_t *__p2,
			size_t __maxlen) pf_attr_noexcept
		{
			// TODO: Lexicographic comparison
		}
	};




	/*
	 * !      !
	 *   UTF8
	 * !      !
	 */
	/// UTF8: Code Len
	pf_hint_nodiscard pf_decl_constexpr size_t
	utf8codelen(
		uint32_t __v) pf_attr_noexcept
	{
		if((__v & 0xE0) == 0xC0) return 1;
		else if((__v & 0xF0) == 0xE0) return 2;
		else if((__v & 0xF8) == 0xF0) return 3;
		else if((__v & 0xFC) == 0xF8) return 4;
		// else if((__v & 0xFE) == 0xFC) return 5;
		pf_assert(true, "__v is invalid utf8 code value!");	// TODO: UTF8 code value exception
		return 0;
	}

	/// UTF8: Len
	pf_hint_nodiscard pf_decl_constexpr size_t
	utf8len(
		const char_t *__str) pf_attr_noexcept
	{
		size_t len = 0, i = 0;
		while
		{
			char_t c = __str[i];
			if(c == '\0') return len;
			i += utf8codelen(static_cast<uint32_t>(c));
			else return 0;
			++len;
		}
	}

	/// UTF8: Compare
	pf_hint_nodiscard pf_decl_constexpr diff_t
	utf8cmp(
		uint32_t __v1,
		uint32_t __v2) pf_attr_noexcept
	{
		// TODO: utf8cmp
	}

	/// UTF8: is
	pf_hint_nodiscard pf_decl_constexpr bool
	utf8isalnum(
		uint32_t __v) pf_attr_noexcept
	{
		// TODO: utf8isalnum
	}
	pf_hint_nodiscard pf_decl_constexpr bool
	utf8isalpha(
		uint32_t __v) pf_attr_noexcept
	{
		// TODO: utf8isalpha
	}
	pf_hint_nodiscard pf_decl_constexpr bool
	utf8islower(
		uint32_t __v) pf_attr_noexcept
	{
		// TODO: utf8islower
	}
	pf_hint_nodiscard pf_decl_constexpr bool
	utf8isupper(
		uint32_t __v) pf_attr_noexcept
	{
		// TODO: utf8isupper
	}
	pf_hint_nodiscard pf_decl_constexpr bool
	utf8isdigit(
		uint32_t __v) pf_attr_noexcept
	{
		// TODO: utf8isdigit
	}
	pf_hint_nodiscard pf_decl_constexpr bool
	utf8isxdigit(
		uint32_t __v) pf_attr_noexcept
	{
		// TODO: utf8isxdigit
	}
	pf_hint_nodiscard pf_decl_constexpr bool
	utf8iscntrl(
		uint32_t __v) pf_attr_noexcept
	{
		// TODO: utf8iscntrl
	}
	pf_hint_nodiscard pf_decl_constexpr bool
	utf8isgraph(
		uint32_t __v) pf_attr_noexcept
	{
		// TODO: utf8isgraph
	}
	pf_hint_nodiscard pf_decl_constexpr bool
	utf8isspace(
		uint32_t __v) pf_attr_noexcept
	{
		// TODO: utf8isspace
	}
	pf_hint_nodiscard pf_decl_constexpr bool
	utf8isblank(
		uint32_t __v) pf_attr_noexcept
	{
		// TODO: utf8isblank
	}
	pf_hint_nodiscard pf_decl_constexpr bool
	utf8isprint(
		uint32_t __v) pf_attr_noexcept
	{
		// TODO: utf8isprint
	}
	pf_hint_nodiscard pf_decl_constexpr bool
	utf8ispunct(
		uint32_t __v) pf_attr_noexcept
	{
		// TODO: utf8ispunct
	}

	/// UTF8: ->
	pf_hint_nodiscard pf_decl_constexpr uint32_t
	utf8toupper(
		uint32_t __v) pf_attr_noexcept
	{
		// TODO: utf8toupper
	}
	pf_hint_nodiscard pf_decl_constexpr uint32_t
	utf8tolower(
		uint32_t __v) pf_attr_noexcept
	{
		// TODO: utf8tolower
	}

	/// UTF8: Traits
	struct utf8_traits_t
	{
		using value_t									 = char_t;
		using code_value_t						 = uint32_t;
		using iterator_t							 = charcode_iterator_variable<utf8_traits_t>;
		using const_iterator_t				 = charcode_const_iterator_variable<utf8_traits_t>;
		using reverse_iterator_t			 = charcode_reverse_iterator_variable<utf8_traits_t>;
		using const_reverse_iterator_t = charcode_const_reverse_iterator_variable<utf8_traits_t>;

		// To code
		pf_decl_static pf_decl_constexpr code_value_t
		to_code(
			value_t *__p) pf_attr_noexcept
		{
			code_value_t code = 0;

		}

		// Code Len
		pf_decl_static pf_decl_constexpr size_t
		code_len(
			const value_t *__str) pf_attr_noexcept
		{}

		// Next
		pf_decl_static pf_decl_constexpr value_t *
		next(
			value_t *__p) pf_attr_noexcept
		{}

		// Prev
		pf_decl_static pf_decl_constexpr value_t *
		prev(
			value_t *__p) pf_attr_noexcept
		{}

		// Is Valid
		pf_decl_static pf_decl_constexpr bool
		is_valid(
			const value_t *__str) pf_attr_noexcept
		{
			// https://lemire.me/blog/2018/05/09/how-quickly-can-you-check-that-a-string-is-valid-unicode-utf-8/
		}

		// Compare
		pf_decl_static pf_decl_constexpr diff_t
		compare(
			code_value_t __p1,
			code_value_t __p2) pf_attr_noexcept
		{}

	};




	/*
	 * !      !
	 *   Wide
	 * !      !
	 */
	/// WIDE: Lenof
	pf_hint_nodiscard pf_decl_constexpr size_t
	widelenof(
		const wchar_t *__str) pf_attr_noexcept
	{
		return std::wcslen(__str);
	}

	/// WIDE: Is*
	pf_hint_nodiscard pf_decl_constexpr bool
	wideisalnum(
		int32_t __val) pf_attr_noexcept
	{
		return std::iswalnum(static_cast<wint_t>(__val));
	}
	pf_hint_nodiscard pf_decl_constexpr bool
	wideisalpha(
		int32_t __val) pf_attr_noexcept
	{
		return std::iswalpha(static_cast<wint_t>(__val));
	}
	pf_hint_nodiscard pf_decl_constexpr bool
	wideislower(
		int32_t __val) pf_attr_noexcept
	{
		return std::iswlower(static_cast<wint_t>(__val));
	}
	pf_hint_nodiscard pf_decl_constexpr bool
	wideisupper(
		int32_t __val) pf_attr_noexcept
	{
		return std::iswupper(static_cast<wint_t>(__val));
	}
	pf_hint_nodiscard pf_decl_constexpr bool
	wideisdigit(
		int32_t __val) pf_attr_noexcept
	{
		return std::iswdigit(static_cast<wint_t>(__val));
	}
	pf_hint_nodiscard pf_decl_constexpr bool
	wideisxdigit(
		int32_t __val) pf_attr_noexcept
	{
		return std::iswxdigit(static_cast<wint_t>(__val));
	}
	pf_hint_nodiscard pf_decl_constexpr bool
	wideiscntrl(
		int32_t __val) pf_attr_noexcept
	{
		return std::iswcntrl(static_cast<wint_t>(__val));
	}
	pf_hint_nodiscard pf_decl_constexpr bool
	wideisgraph(
		int32_t __val) pf_attr_noexcept
	{
		return std::iswgraph(static_cast<wint_t>(__val));
	}
	pf_hint_nodiscard pf_decl_constexpr bool
	wideisspace(
		int32_t __val) pf_attr_noexcept
	{
		return std::iswspace(static_cast<wint_t>(__val));
	}
	pf_hint_nodiscard pf_decl_constexpr bool
	wideisblank(
		int32_t __val) pf_attr_noexcept
	{
		return std::iswblank(static_cast<wint_t>(__val));
	}
	pf_hint_nodiscard pf_decl_constexpr bool
	wideisprint(
		int32_t __val) pf_attr_noexcept
	{
		return std::iswprint(static_cast<wint_t>(__val));
	}
	pf_hint_nodiscard pf_decl_constexpr bool
	wideispunct(
		int32_t __val) pf_attr_noexcept
	{
		return std::iswpunct(static_cast<wint_t>(__val));
	}
	pf_hint_nodiscard pf_decl_constexpr bool
	wideisascii(
		int32_t __val) pf_attr_noexcept
	{
		return std::iswascii(static_cast<wint_t>(__val));
	}
	pf_hint_nodiscard pf_decl_constexpr bool
	wideisleadbyte(
		int32_t __val) pf_attr_noexcept
	{
		return std::isleadbyte(__val);
	}

	/// WIDE: Traits
	struct wide_traits_t
	{
		using value_t					 = wchar_t;
		using code_value_t		 = int32_t;
		using iterator_t			 = charcode_iterator<wide_traits_t>;
		using const_iterator_t = charcode_const_iterator<wide_traits_t>;
		using iterator_t			 = charcode_reverse_iterator<wide_traits_t>;
		using const_iterator_t = charcode_const_reverse_iterator<wide_traits_t>;

		// To code
		pf_decl_static pf_decl_constexpr code_value_t
		to_code(
			value_t *__p) pf_attr_noexcept
		{}

		// Next
		pf_decl_static pf_decl_constexpr value_t *
		next(
			value_t *__p) pf_attr_noexcept
		{
			return --__p;
		}

		// Prev
		pf_decl_static pf_decl_constexpr value_t *
		prev(
			value_t *__p) pf_attr_noexcept
		{
			return ++__p;
		}

		// Code Len
		pf_decl_static pf_decl_constexpr size_t
		code_len(
			const value_t *__str) pf_attr_noexcept
		{}

		// Is Valid
		pf_decl_static pf_decl_constexpr bool
		is_valid(
			const value_t *__str) pf_attr_noexcept
		{}

		// Compare
		pf_decl_static pf_decl_constexpr diff_t
		compare(
			code_value_t __p1,
			code_value_t __p2) pf_attr_noexcept
		{}
	};

	/*
	 * !           !
	 *   Char Code
	 * !           !
	 */

	/// CHAR: String
	template <
		typename _CharTraits,
		typename _Magnifier,
		typename _Allocator>
	class charcode_string
	{
		using value_t									 = typename _CharTraits::value_t;
		using iterator_t							 = typename _CharTraits::iterator_t;
		using const_iterator_t				 = typename _CharTraits::const_iterator_t;
		using reverse_iterator_t			 = typename _CharTraits::reverse_iterator_t;
		using const_reverse_iterator_t = typename _CharTraits::const_reverse_iterator_t;
		using magnifier_t							 = _Magnifier;
		using allocator_t							 = _Allocator;

	private:
		forward_sequence<value_t, _Magnifier, _Allocator> seq_;
	};

	/// CHAR: String -> Alias
	template <
		typename _Magnifier = magnifier_default,
		typename _Allocator = allocator_malloc>
	using asciistring_t = charcode_string<ascii_traits_t, _Magnifier, _Allocator>;
	template <
		typename _Magnifier = magnifier_default,
		typename _Allocator = allocator_malloc>
	using widestring_t = charcode_string<wide_traits_t, _Magnifier, _Allocator>;
	template <
		typename _Magnifier = magnifier_default,
		typename _Allocator = allocator_malloc>
	using utf8string_t = charcode_string<utf8_traits_t, _Magnifier, _Allocator>;

	/// CHAR: String View
	template <typename _CharTraits>
	class charcode_string_view
	{
		using value_t									 = typename _CharTraits::value_t;
		using const_iterator_t				 = typename _CharTraits::const_iterator_t;
		using const_reverse_iterator_t = typename _CharTraits::const_reverse_iterator_t;

	private:
		span<value_t> span_;
	};

	/// CHAR: Iterator
	template <typename _CharTraits>
	class charcode_iterator
	{
		// TODO: static_assert not dynamic code point!

		using value_t				= value_type_t<_CharTraits>;
		using char_traits_t = _CharTraits;

	private:
		value_t *ptr_;
	};

	/// CHAR: Const Iterator
	template <typename _CharTraits>
	class charcode_const_iterator
	{
		using value_t				= value_type_t<_CharTraits>;
		using char_traits_t = _CharTraits;

	private:
		const value_t *ptr_;
	};

	/// CHAR: Reverse Iterator
	template <typename _CharTraits>
	class charcode_reverse_iterator
	{
		// TODO: static_assert not dynamic code point!

		using value_t				= value_type_t<_CharTraits>;
		using char_traits_t = _CharTraits;

	private:
		value_t *ptr_;
	};

	/// CHAR: Const Reverse Iterator
	template <typename _CharTraits>
	class charcode_const_reverse_iterator
	{
		using value_t				= value_type_t<_CharTraits>;
		using char_traits_t = _CharTraits;

	private:
		const value_t *ptr_;
	};

	/// CHAR: Iterator -> Variable
	template <typename _CharTraits, typename _String>
	class charcode_iterator_variable
	{
		using value_t				= value_type<_CharTraits>;
		using char_traits_t = _CharTraits;
		using string_t			= _String;

	private:
		value_t *ptr_;
		string_t *str_;
	};

	/// CHAR: Reverse Iterator -> Variable
	template <typename _CharTraits, typename _String>
	class charcode_reverse_iterator_variable
	{
		using value_t				= value_type<_CharTraits>;
		using char_traits_t = _CharTraits;
		using string_t			= _String;

	private:
		value_t *ptr_;
		string_t *str_;
	};

	/// CHAR: Cast
	template <
		typename _StringOut,
		typename _StringIn>
	pf_hint_nodiscard pf_decl_constexpr _StringOut
	char_cast(
		_StringIn const &__str) pf_attr_noexcept
	{
		// TODO: cast ascii -> wide
		// TODO: cast ascii -> utf8
		// TODO: cast utf8 -> wide
		// TODO: cast wide -> utf8
	}
	template <
		typename _CharTraitsOut,
		typename _CharTraitsIn>
	pf_hint_nodiscard pf_decl_constexpr size_t
	char_cast(
		charcode_string_view<_CharTraitsIn> __in,
		typename _CharTraitsOut::value_t *__buf,
		size_t __size) pf_attr_noexcept
	{
		// TODO: buffer cast
	}

	/// CHAR: Format
	template <
		typename _StringOut,
		typename ..._Args>
	pf_hint_nodiscard pf_decl_constexpr _StringOut
	format(
		_Args && ... __args) pf_attr_noexcept
	{}
	template <
		typename _StringOut,
		typename ..._Args>
	pf_hint_nodiscard pf_decl_constexpr size_t
	format(
		_StringOut &__str,
		_Args && ... __args) pf_attr_noexcept
	{}
	template <
		typename _IteratorOut,
		typename ..._Args>
	pf_hint_nodiscard pf_decl_constexpr _IteratorOut
	format(
		_IteratorOut __it,
		_Args && ... __args) pf_attr_noexcept
	{}

}

#endif // !PULSAR_CHARCODE_HPP