/*! @file	 	string.hpp
 *  @author Fluffy (noe.louis-quentin@hotmail.fr)
 *  @brief	Defines utility functions for string manipulation.
 *  @date 	23-05-2022
 *
 *  @copyright Copyright (c) 2022 - Pulsar Software
 *
 *  @since 0.1.0
 */

#ifndef PULSAR_STRING_HPP
#define PULSAR_STRING_HPP 1

// Include: Pulsar
#include "pulsar/pulsar.hpp"

// Include: C++
#include <algorithm>
#include <array>
#include <string>
#include <string_view>
#include <utility>

// Include: C
#include <cstdarg>
#include <cstring>
#include <ctime>

// Pulsar
namespace pul
{
	/// Trim
	/*! @brief Deletes the spaces to the right of the last character of the string.
	 *
	 *  @param[in, out] __s String to modify.
	 *  @return Iterator referencing location of the first removal.
	 */
	pf_decl_inline std::string::iterator strtriml(
			std::string &__s)
	{
		return __s.erase(__s.begin(),
										 std::find_if(__s.begin(), __s.end(), [](char c)
																	{ return !std::isspace(c); }));
	}
	/*! @brief Deletes the spaces to the left of the first character of the string.
	 *
	 *  @param[in, out] __s String to modify.
	 *  @return Iterator referencing location of the first removal.
	 */
	pf_decl_inline std::string::iterator strtrimr(
			std::string &__s)
	{
		return __s.erase(std::find_if(__s.rbegin(), __s.rend(), [](char c)
																	{ return !std::isspace(c); })
												 .base(),
										 __s.end());
	}

	/// ToLower / Upper
	/*! @brief Converts all characters in a string to lower case.
	 *
	 *  @param[in, out] __s String to modify.
	 *  @return Output iterator equal to the last modified element.
	 */
	pf_decl_inline std::string::iterator strtolower(
			std::string &__s)
	{
		return std::transform(__s.cbegin(), __s.cend(), __s.begin(), [](char c)
													{ return std::tolower(c); });
	}
	/*! @brief Converts all characters in a string to upper case.
	 *
	 *  @param[in, out] __s String to modify.
	 *  @return Output iterator equal to the last modified element.
	 */
	pf_decl_inline std::string::iterator strtoupper(
			std::string &__s)
	{
		return std::transform(__s.cbegin(), __s.cend(), __s.begin(), [](char c)
													{ return std::toupper(c); });
	}

	/// Format
	/*! @brief Creates a string from  formatting specification @a __format by the variadic arguments
	 *				 based on std::vsnprintf function.
	 *
	 *  @ref See https://en.cppreference.com/w/cpp/io/c/fprintf to know how to use this function
	 * 			 (For @a __format and variadic arguments).
	 *
	 *  @param[in] __format Formatting specification.
	 *  @param[in] ... 		  Variadic arguments sent to @a __format.
	 *
	 *  @return Formatted string.
	 */
	pf_hint_nodiscard pf_decl_inline std::string strfmt(
			std::string_view __format,
			...)
	{
		std::va_list va;
		va_start(va, __format);
		const size_t n = std::vsnprintf(nullptr, 0, __format.data(), va) + 1;
		std::string s(n, ' ');
		if (std::vsnprintf(s.data(), s.length(), __format.data(), va) < 0)
			return "";
		va_end(va);
		s.pop_back(); // remove null-terminating
		return s;
	}
	/*! @brief Inserts into a string @a __str, a formatted string at position @a __w from a
	 * 				 formatting specification @a __format.
	 *
	 *  @param[in] __format   Formatting specification.
	 *  @param[in, out] __str String to modify.
	 *  @param[in] __w				Location of the insertion.
	 *  @param[in] ...				Variadic arguments sent to @a __format.
	 *  @return Reference to the input string.
	 */
	pf_decl_inline std::string &strfmt(
			std::string_view __format,
			std::string &__str,
			std::string::iterator __w,
			...)
	{
		std::va_list va;
		va_start(va, __w);
		const size_t n = std::vsnprintf(nullptr, 0, __format.data(), va) + 1;
		__str.resize(__str.size() + n);
		char *w = __w.base();
		char *d = w + n;
		std::memcpy(d, w, n);
		if (std::vsnprintf(w, n, __format.data(), va) < 0)
		{
			std::memcpy(w, d, n);
			__str.resize(__str.size() - n);
		}
		else
		{
			__str.erase(__w + n - 1);
		}
		va_end(va);
		return __str;
	}

	/// Time format
	/*! @brief Formats from a specification @a __format, a date @a __tm.
	 *
	 *  @param[in] __format Format of the string.
	 *  @param[in] __tm  		Calendar time.
	 *  @return Character string containing a formatted time.
	 */
	pf_hint_nodiscard pf_decl_inline std::string strfmt(
			std::string_view __format,
			const std::tm &__tm)
	{
		std::array<char, 128> buf{ ' ' };
		size_t n = std::strftime(buf.data(), buf.size(), __format.data(), &__tm) + 1;
		if (n == 1)
			return "";
		if (n <= buf.size())
			--n;
		return std::string(buf.begin(), buf.begin() + n);
	}
	/*! @brief Inserts into a string @a __str, a string formatted at the position @a __w, a
	 *				 formatting specification @a __format, a date @a __tm.
	 *
	 *	@param[in] __format 	Formatting specification.
	 * 	@param[in, out] __str String to modify.
	 * 	@param[in] __w				Location of the string.
	 * 	@param[in] __tm				Calendar time.
	 * 	@return Reference to the input string.
	 */
	pf_decl_inline std::string &strfmt(
			std::string_view __format,
			std::string &__str,
			std::string::iterator __w,
			const std::tm &__tm)
	{
		std::array<char, 128> buf{ ' ' };
		size_t n = std::strftime(buf.data(), buf.size(), __format.data(), &__tm) + 1;
		if (n == 1)
			return __str;
		if (n <= buf.size())
			--n;
		__str.insert(__w, buf.data(), buf.data() + n);
		return __str;
	}

	// TODO: strfmt -> fmt::format
}

#endif // !PULSAR_STRING_HPP