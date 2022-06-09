/*! @file   string_win.hpp
 *  @author Fluffy (noe.louis-quentin@hotmail.fr)
 *  @brief	Defines functions for conversions between strings wchar <-> char.
 *  @date   03-06-2022
 *
 *  @copyright Copyright (c) 2022 - Pulsar Software
 *
 *  @since 0.1.1
 */

#ifndef PULSAR_UTILITY_STRING_WIN_HPP
#define PULSAR_UTILITY_STRING_WIN_HPP 1

// Include: Pulsar
#include "pulsar/debug.hpp"
#include "pulsar/pulsar.hpp"

// Include: C++
#include <string>
#include <string_view>

// Include: C
#include <cstdlib>

// Include: Windows
#ifdef PF_OS_WINDOWS

// Pulsar
namespace pul
{
	// Wchar <=> Char
	/*! @brief Converts a multibyte wchar string to an ANSI string.
	 *
	 *  @param[in] __sv Multibyte wchar string.
	 *  @return ANSI string.
	 */
	pf_hint_nodiscard pf_decl_inline std::string strUNItoANSI(
			std::wstring_view __sv)
	{
		size_t sn = __sv.length();
		std::string s(sn, '\0');
		errno_t err = wcstombs_s(
				&sn,
				s.data(),
				sn,
				__sv.data(),
				sn);
		if (sn > 0 && err == std::numeric_limits<size_t>::max())
			throw(exception(std::generic_category(), err));
		return s;
	}
	/*! @brief Converts a ANSI string to an multibyte wchar string.
	 *
	 *  @param[in] __sv ANSI string.
	 *  @return Multibyte wchar string.
	 */
	pf_hint_nodiscard pf_decl_inline std::wstring strANSItoUNI(
			std::string_view __sv)
	{
		size_t sn = __sv.length();
		std::wstring s(sn, L'\0');
		errno_t err = mbstowcs_s(
				&sn,
				s.data(),
				sn,
				__sv.data(),
				sn);
		if (sn > 0 && err == std::numeric_limits<size_t>::max())
			throw(exception(std::generic_category(), err));
		return s;
	}
}

#endif // PF_OS_WINDOWS

#endif // !PULSAR_UTILITY_STRING_WIN_HPP