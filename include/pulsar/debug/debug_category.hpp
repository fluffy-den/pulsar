/**
 * @file    debug_category.hpp
 * @author  Fluffy (noe.louis-quentin@hotmail.fr)
 * @brief
 * @date    05-01-2022
 *
 * @copyright Copyright (c) 2022 - Pulsar Software
 *
 */

#ifndef PULSAR_DEBUG_CATEGORY_HPP
#define PULSAR_DEBUG_CATEGORY_HPP 1

// Include: Pulsar
#include "pulsar/debug/debug.hpp"

// Include: C++
#include <system_error>

// Include: C
#include <cerrno>

// Pulsar
namespace pulsar
{
	// Debug
	namespace debug
	{

		/**
		 * @category Debugging
		 */

		// Code
		using error_code = std::error_code;

		// Category
		/**
		 * @brief
		 *
		 */
		class pf_decl_novtable pulsar_category pf_attr_final: public std::error_category
		{
			// Constructors

			// Operator=

			// Destructor

			// Name
			/**
			 * @brief
			 *
			 * @return pf_hint_nodiscard const*
			 */
			pf_hint_nodiscard pf_decl_virtual const char *name() const pf_attr_noexcept pf_attr_override
			{
				return "pulsar";
			}
		};

		// Errno
		/**
		 * @brief
		 *
		 * @return error_code
		 */
		error_code error_errno()
		{
			return error_code(errno, std::generic_category());
		}

	} // Debug
} // Pulsar

#endif // !PF_DEBUG_CATEGORY_HPP