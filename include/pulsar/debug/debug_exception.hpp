/**
 * @file    debug_exception.hpp
 * @author  Fluffy (noe.louis-quentin@hotmail.fr)
 * @brief
 * @date    01-01-2022
 *
 * @copyright Copyright (c) 2022 - Pulsar Software
 *
 */

#ifndef PULSAR_DEBUG_EXCEPTION_HPP
#define PULSAR_DEBUG_EXCEPTION_HPP 1

// Include: Pulsar
#include "pulsar/debug/debug.hpp"

// Pulsar
namespace pulsar
{
	// Debug
	namespace debug
	{
		/**
		 * @category Debugging
		 */

		/**
		 * @brief
		 *
		 *
		 */
		class exception pf_attr_final
		{
		public:
			/// Constructors
			/**
			 * @brief
			 *
			 * @param r
			 */
			exception(error_code c,
								string_view file = __FILE__,
								string_view func = __FUNCSIG__) pf_attr_noexcept
			{}
			/**
			 * @brief
			 *
			 * @param r
			 */
			exception(exception const &r,
								string_view file = __FILE__,
								string_view func = __FUNCSIG__) pf_attr_noexcept
			{}

			/// Destructor
			/**
			 * @brief
			 */
			pf_decl_virtual ~exception() pf_attr_noexcept
			{}

			/// Assignment Operator
			/**
			 *
			 */
			exception &operator=(exception const &r) pf_attr_noexcept
			{
				return *this;
			}

			/**
			 * @brief
			 *
			 * @return
			 */
			pf_decl_virtual const char *what() const pf_attr_noexcept pf_attr_override
			{
				// TODO: What [pulsar_module] (CODE) -> Message in __FUNCSIG__, at FILENAME, LINE
				return nullptr;
			}

		private:
			error_code code;
		};
	} // Debugging
} // Pulsar

#endif // !PULSAR_DEBUG_EXCEPTION_HPP