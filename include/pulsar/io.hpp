/*! @file   io.hpp
 *  @author Louis-Quentin NOE (noe.louis-quentin@hotmail.fr)
 *  @brief
 *  @date   17-09-2022
 *
 *  @copyright Copyright (c) 2022 - Pulsar Software
 *
 *  @since 0.1.2
 */

#ifndef PULSAR_IO_HPP
#define PULSAR_IO_HPP 1

// Include: Pulsar
#include "pulsar/string.hpp"

// Pulsar
namespace pul
{
	// // https://github.com/fmtlib/fmt
	template <typename ..._Args>
	pf_decl_inline pf_decl_constexpr void
	fprint(
		c8string_span_t __fmt,
		_Args && ... __args)
	{
		if(std::is_constant_evaluated())
		{
			// Static Print
		}
		else
		{
			// Print
		}
	}

	template <typename ..._Args>
	pf_decl_inline pf_decl_constexpr c8string_t
	format(
		c8string_span_t __fmt,
		_Args && ... __args)
	{
		// TODO: Format
		return nullptr;
	}

	template <typename _OutIterator, typename ..._Args>
	pf_decl_inline pf_decl_constexpr c8string_t
	format_to(
		_OutIterator __dest,
		c8string_span_t __fmt,
		_Args && ... __args)
	{
		// TODO: Format
		return nullptr;
	}
}

#endif // !PULSAR_IO_HPP