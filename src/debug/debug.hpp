/*! @file   debug.hpp
 *  @author Fluffy (noe.louis-quentin@hotmail.fr)
 *  @brief
 *  @date   16-07-2022
 *
 *  @copyright Copyright (c) 2022 - Pulsar Software
 *
 *  @since 0.1.2
 */

#ifndef PULSAR_INTERNAL_DEBUG_HPP
#define PULSAR_INTERNAL_DEBUG_HPP 1

// Include: Debugger


// Include: Pulsar
#include "pulsar/debug.hpp"

// Pulsar
namespace pul
{
	/// Debugger -> Internal
	struct pulsar_api __debugger_internal
	{
		/// Constructor
		__debugger_internal() pf_attr_noexcept;

		/// Writter -> Default
		pf_decl_static void __default_writter(
				std::string_view __message) pf_attr_noexcept;

		/// Writter -> Write
		void __log_write(
				debug_level __level,
				debug_filter __filter,
				std::string_view __message = "");

		/// Internal -> Store
		shared_isignal<void(std::string_view)> loggerDefaultReceiver_;
		shared_osignal<void(std::string_view)> loggerSignal_;
		std::filesystem::path dumpbinpath_;
		std::chrono::high_resolution_clock::time_point loggerStart_;
		debug_filter loggerFilter_;

		pf_decl_static pf_decl_inline std::unique_ptr<__debugger_internal> instance_ = nullptr;
	};
}

#endif // !PULSAR_INTERNAL_DEBUG_HPP
