/*! @file   debug_exception.hpp
 *  @author Fluffy (noe.louis-quentin@hotmail.fr)
 *  @brief  Implements exception.
 *  @date   11-08-2022
 *
 *  @copyright Copyright (c) 2022 - Pulsar Software
 *
 *  @since 0.1.2
 */

#ifndef PULSAR_DEBUG_EXCEPTION_HPP
#define PULSAR_DEBUG_EXCEPTION_HPP 1

// Include: Pulsar -> Debug
#include "pulsar/debug/debug_types.hpp"

// Include: C++
#include <system_error>

// Pulsar
namespace pul
{
	/// Exception
	/*! @brief Class defining unexpected errors.
	 */
	class pulsar_api exception pf_attr_final: public std::exception
	{
		/// Message -> Initializer
		pf_decl_static std::string __exception_message(
				std::error_category const &__cat,
				int32_t __code,
				std::string_view __message) pf_attr_noexcept;

		/// Message -> Add end point
		pf_decl_static std::string &__add_end_point_to_message(
				std::string &__str) pf_attr_noexcept;

	public:
		/// Constructors
		/*! @brief Constructor.
		 *
		 *  @param[in] __cat		 Category of the error.
		 *  @param[in] __code		 Code of the error.
		 *  @param[in] __message (optional) Message explaining the error.
		 *  @param[in] __flags	 (optional) @see debug_flags.
		 */
		exception(
				std::error_category const &__cat,
				int32_t __code,
				std::string_view __message = "",
				uint32_t __flags					 = debug_flags::default_flags) pf_attr_noexcept;
		/*! @brief Copy constructor.
		 *
		 *  @param[in] __r Another instance from wich to copy.
		 */
		exception(exception const &__r) pf_attr_noexcept = default;

		/// Operator=
		/*! @brief Copy assignment operator.
		 *
		 *  @param[in] __r Another instance from wich to copy.
		 *  @return Reference on this instance.
		 */
		exception &operator=(exception const &__r) pf_attr_noexcept = default;

		/// Code
		/*! @brief Gets the associated error_code with this exception.
		 *
		 *  @return Constant reference on this associated error_code.
		 */
		pf_hint_nodiscard const std::error_code &code() const pf_attr_noexcept;

		/// What
		/*! @brief Gets the formatted message explaining the exception.
		 *
		 *  @return Constant pointer on this associated formatted message explaining the error.
		 */
		pf_hint_nodiscard const char *what() const pf_attr_noexcept pf_attr_override;

		/// Flags
		/*! @brief Gets the current associated flags with this exception.
		 *
		 *  @return @see debug_flags.
		 */
		pf_hint_nodiscard uint32_t get_flags() const pf_attr_noexcept;

		/*! @brief Set the new flags for this exception.
		 *
		 *  @param[in] __flags @see debug_flags.
		 *  @return Old flags of this exception.
		 */
		void set_flags(uint32_t __flags) pf_attr_noexcept;

		/// Formatters
		/*! @brief Generates a formatted message of an error.
		 *
		 *  @param[in] __cat	   Category of the error.
		 *  @param[in] __code		 Code of the error.
		 *  @param[in] __message (optional) Message explaining the error.
		 *  @return Formatted error message.
		 */
		pf_hint_nodiscard pf_decl_static std::string format(
				std::error_category const &__cat,
				int32_t __code,
				std::string_view __message = "") pf_attr_noexcept;

	private:
		std::error_code code_;
		std::string message_;
		uint32_t flags_;
	};
}

#endif // !PULSAR_DEBUG_EXCEPTION_HPP