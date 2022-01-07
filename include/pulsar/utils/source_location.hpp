/**
 * @file    source_location.hpp
 * @author  Fluffy (noe.louis-quentin@hotmail.fr)
 * @brief
 * @date    05-01-2022
 *
 * @copyright Copyright (c) 2022 - Pulsar Software
 *
 */

#ifndef PULSAR_SOURCE_LOCATION_HPP
#define PULSAR_SOURCE_LOCATION_HPP 1

// Include: Pulsar
#include "pulsar/utils/string.hpp"

// Pulsar
namespace pulsar
{
	/**
	 * @brief
	 *
	 */
	struct source_location
	{
	public:
		pf_decl_explicit pf_decl_constexpr source_location(const char *__file = __FILE__,
																											 const char *__func = __FUNCTION__,
																											 size_t __line			= __LINE__) pf_attr_noexcept
				: file_(__file)
				, func_(__func)
				, line_(__line)
		{}

	private:
		string_view file_;
		string_view func_;
		size_t line_;
	};

}; // Pulsar

#endif // !PULSAR_SOURCE_LOCATION_HPP