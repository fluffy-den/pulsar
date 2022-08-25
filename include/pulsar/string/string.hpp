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
}

#endif // !PULSAR_STRING_HPP