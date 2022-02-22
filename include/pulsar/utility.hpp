/*! @file   utility.hpp
 *  @author Fluffy (noe.louis-quentin@hotmail.fr)
 *  @brief
 *  @date   09-02-2022
 *
 *  @copyright Copyright (c) 2022 - Pulsar Software
 *
 *  @since 0.1.0
 */

#ifndef PULSAR_UTILITY_HPP
#define PULSAR_UTILITY_HPP 1

// Include: Pulsar
#include "pulsar/pulsar.hpp"

// Include: C++
#include <algorithm>
#include <string>
#include <string_view>
#include <utility>

// Include: C
#include <cstdarg>
#include <ctime>

// Pulsar
namespace pul
{

	/// Format
	/*! @brief Creates a pul::string from a pul::string_view @a __format formatted by the
	 * 				 variadic arguments based on std::vsnprintf function.
	 *
	 *  @ref See https://en.cppreference.com/w/cpp/io/c/fprintf to know how to use this function
	 * 			 (For @a __format and variadic arguments).
	 *
	 *  @param __format Formatting string.
	 *  @param ... 		  Variadic arguments sent to @a __format.
	 *
	 *  @return The formatted string.
	 */
	pf_decl_inline std::string strfmt(
			std::string_view __format,
			...)
	{
		va_list va;
		va_start(va, __format);
		size_t n = std::vsnprintf(nullptr, 0, __format.data(), va) + 1;
		std::string s(n, ' ');
		if (std::vsnprintf(s.data(), s.length(), __format.data(), va) < 0)
			return "";
		va_end(va);
		s.pop_back(); // Remove '\0'
		return s;
	}

	/// Trim
	/*! @brief
	 *
	 *  @param __s
	 *  @return pf_decl_inline
	 */
	pf_decl_inline std::string::iterator strtriml(
			std::string &__s)
	{
		return __s.erase(__s.begin(),
										 std::find_if(__s.begin(), __s.end(), [](char c)
																	{ return !std::isspace(c) && c != '\0'; }));
	}
	/*! @brief
	 *
	 *  @param __s
	 *  @return pf_decl_inline
	 */
	pf_decl_inline std::string::iterator strtrimr(
			std::string &__s)
	{
		return __s.erase(std::find_if(__s.rbegin(), __s.rend(), [](char c)
																	{ return !std::isspace(c) && c != '\0'; })
												 .base(),
										 __s.end());
	}

	/// ToLower / Upper
	pf_decl_inline std::string::iterator strtolower(std::string &__s)
	{
		return std::transform(__s.cbegin(), __s.cend(), __s.begin(), [](char c)
													{ return std::tolower(c); });
	}
	pf_decl_inline std::string::iterator strtoupper(std::string &__s)
	{
		return std::transform(__s.cbegin(), __s.cend(), __s.begin(), [](char c)
													{ return std::toupper(c); });
	}

	/// Date
	/*! @brief
	 *
	 *  @param __format
	 *  @param __tm
	 *  @return pf_decl_inline
	 */
	pf_decl_inline std::string strdate(std::string_view __format, const std::tm *__tm)
	{
		std::string s(128, ' ');
		if (!std::strftime(s.data(), s.length(), __format.data(), __tm))
			return "";
		strtrimr(s);
		return s;
	}
}

#endif // PULSAR_UTILITY_HPP