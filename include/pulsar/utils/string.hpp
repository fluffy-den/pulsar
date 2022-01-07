/**
 * @file    string.hpp
 * @author  Fluffy (noe.louis-quentin@hotmail.fr)
 * @brief
 * @date    02-01-2022
 *
 * @copyright Copyright (c) 2022 - Pulsar Software
 *
 */

// Include: Pulsar
#include "pulsar/debug/debug_category.hpp"

// Include: C
#include <cstdarg> // va_*
#include <cstdio>	 // vsnprintf

// Include: C++
#include <string>
#include <string_view>

// Pulsar
namespace pulsar
{

	/**
	 * @category String
	 */
	// Types
	using string			= std::basic_string<char, std::char_traits<char>, allocator<char>>;
	using string_view = std::basic_string_view<char, std::char_traits<char>>;

	// Format
	/**
	 * @brief Creates a pulsar::string from a pulsar::string_view f formatted by the variadic
	 * 				arguments based on std::vsnprintf function.
	 *
	 * @ref See https://en.cppreference.com/w/cpp/io/c/fprintf to know how to use this function
	 * 			(same arguments as std::fprintf)
	 *
	 * @param (pulsar::string_view) f The formatting string
	 * @param (va) ...								The variadic arguments sent to f
	 *
	 * @throw error_errno()
	 *
	 * @return (pulsar::string) The formatted string
	 */
	pf_hint_nodiscard std::string string_format(string_view f, ...)
	{
		va_list va;
		va_start(va, f);
		size_t n = std::vsnprintf(nullptr, 0, f.data(), va) + 1;
		string s(n, '\0');
		if (std::vsnprintf(s.data(), n, f.data(), va) < 0)
			throw(debug::error_errno());
		va_end(va);
		return s;
	}

} // Pulsar