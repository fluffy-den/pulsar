/**
 * @file    source_location.hpp
 * @author  Fluffy (noe.louis-quentin@hotmail.fr)
 * @brief
 * @date    05-01-2022
 *
 * @copyright Copyright (c) 2022 - Pulsar Software
 *
 */

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
		// Constructors
		/**
		 * @brief
		 *
		 * @param file
		 * @param func
		 * @return pf_decl_constexpr
		 */
		pf_decl_constexpr source_location(string_view file = __FILE__,
																			string_view func = __FUNCSIG__) pf_attr_noexcept
				: file(file)
				, func(func)
		{}
		/**
		 * @brief
		 *
		 * @param r
		 * @return pf_decl_constexpr
		 */
		pf_decl_constexpr source_location(source_location &&r) pf_attr_noexcept
				: file(r.file)
				, func(r.file)
		{}
		/**
		 * @brief
		 *
		 * @param r
		 * @return pf_decl_constexpr
		 */
		pf_decl_constexpr source_location(source_location const &r) pf_attr_noexcept
				: file(std::move(r.file))
				, func(std::move(r.func))
		{}

		// Operators=
		/**
		 * @brief
		 *
		 * @param r
		 * @return pf_decl_constexpr&
		 */
		pf_decl_constexpr source_location &operator=(source_location &&r) pf_attr_noexcept
		{
			this.file = r.file;
			this.func = r.func;
			return *this;
		}
		/**
		 * @brief
		 *
		 * @param r
		 * @return pf_decl_constexpr&
		 */
		pf_decl_constexpr source_location &operator=(source_location const &r) pf_attr_noexcept
		{
			this.file = std::move(r.file);
			this.func = std::move(r.func);
			return *this;
		}

		// Static Source
		/**
		 * @brief
		 *
		 * @param file
		 * @param func
		 * @return pf_decl_static
		 */
		pf_decl_static source_location current_location(string_view file = __FILE__,
																										string_view func = __FUNCSIG__) pf_attr_noexcept
		{
			return source_location(file, func);
		}

		// Data
		const string_view file;
		const string_view func;
		const uint64_t line;
	};
}