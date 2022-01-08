/**
 * @file    string_format.hpp
 * @author  Fluffy (noe.louis-quentin@hotmail.fr)
 * @brief		String format function definition file.
 * @date    08-01-2022
 *
 * @copyright Copyright (c) 2022 - Pulsar Software
 *
 */

#ifndef PULSAR_STRING_FORMAT_HPP
#define PULSAR_STRING_FORMAT_HPP 1

// Include: Pulsar
#include "pulsar/utils/string.hpp"

// Include: C
#include <cstdarg>

// Pulsar
namespace pulsar
{
	/// Format
	/**
	 * @brief Creates a pulsar::string from a pulsar::string_view @a __format formatted by the
	 * 				variadic arguments based on std::vsnprintf function.
	 *
	 * @ref See https://en.cppreference.com/w/cpp/io/c/fprintf to know how to use this function
	 * 			(For @a __format and variadic arguments).
	 *
	 * @param __format Formatting string.
	 * @param ... 		 Variadic arguments sent to @a __format.
	 *
	 * @return The formatted string.
	 */
	string string_format(string_view __format, ...)
	{
		va_list va;
		va_start(va, __format);
		size_t n = std::vsnprintf(nullptr, 0, __format.data(), va) + 1;
		string s(n, '\0');
		if (std::vsnprintf(s.data(), n, __format.data(), va) < 0)
			return ""; // TODO: Exception
		va_end(va);
		return s;
	}

} // Pulsar

#endif // !PULSAR_STRING_FORMAT_HPP