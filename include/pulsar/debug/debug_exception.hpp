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
			const char *what() const pf_attr_noexcept
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