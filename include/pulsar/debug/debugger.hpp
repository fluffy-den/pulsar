/*! @file   debugger.hpp
 *  @author Fluffy (noe.louis-quentin@hotmail.fr)
 *  @brief  Implements debugger.
 *  @date   11-08-2022
 *
 *  @copyright Copyright (c) 2022 - Pulsar Software
 *
 *  @since 0.1.2
 */

#ifndef PULSAR_DEBUGGER_HPP
#define PULSAR_DEBUGGER_HPP 1

// Include: Pulsar -> Debug
#include "pulsar/debug/debug_exception.hpp"

// Include: Pulsar
#include "pulsar/function.hpp"

// Include: C++
#include <filesystem>

// Pulsar
namespace pul
{
	class pulsar_api debugger
	{
	public:
		using exception = exception;

		/// Constructors
		debugger();
		debugger(debugger const &) = delete;
		debugger(debugger &&)			 = delete;

		/// Destructor
		~debugger() pf_attr_noexcept;

		/// StackTrace -> Get List
		/*! @brief Generates the current call stack trace and ignores the first @a __numToIgnore traces.
		 *
		 *  @param[in] __numToIgnore Number of trace to ignore.
		 *  @return Trace list of the call stack.
		 */
		pf_hint_nodiscard pf_decl_static debug_trace_list_t stacktrace(
				uint32_t __numToIgnore = 1);

		/// Writter -> Signals
		/*! @brief Add signal receiver to logger. (Function that receives next message to write)
		 *
		 *  @param[in] __wr Signal receiver to add.
		 */
		pf_decl_static void add_log_signal(
				shared_isignal<void(std::string_view)> &__wr) pf_attr_noexcept;
		/*! @brief Remove signal receiver from logger.
		 *
		 *  @param[in] __wr Signal receiver to remove.
		 */
		pf_decl_static void rem_log_signal(
				shared_isignal<void(std::string_view)> &__wr)
				pf_attr_noexcept;

		/// Writter -> Filters
		/*! @brief Gets the current log filters.
		 *
		 *  @return Log Filter.
		 */
		pf_hint_nodiscard pf_decl_static debug_filter log_filter() pf_attr_noexcept;
		/*! @brief Set the log filters.
		 *
		 *  @param[in] __f New log filters.
		 */
		pf_decl_static void set_log_filter(
				debug_filter __f) pf_attr_noexcept;

		/// Writter -> Write
		/*! @brief Writes a message.
		 *
		 *  @param[in] __level   Level.
		 *  @param[in] __filter  Importance.
		 *  @param[in] __message Message.
		 */
		pf_decl_static void log(
				debug_level __level,
				debug_filter __filter,
				std::string_view __message);

		/// Generic -> Code Cast
		/*! @brief Converts a generic error code to an integer.
		 *
		 *  @param[in] __code The standard error code.
		 *  @return Converted error code as integer.
		 */
		pf_hint_nodiscard pf_decl_static pf_decl_constexpr int32_t generic_code(
				std::errc __code) pf_attr_noexcept
		{
			union
			{
				std::errc as_code_t;
				std::int32_t as_int32_t;
			};
			as_code_t = __code;
			return as_int32_t;
		}

		/// Dumpbin -> Path
		/*! @brief Returns the path to the directory where the dump files are generated.
		 *				 generation.
		 *
		 *  @return Reference on the path.
		 */
		pf_hint_nodiscard pf_decl_static std::filesystem::path &dumpfile_creation_path() pf_attr_noexcept;
		/*! @brief Changes the path of the directory where the dump files are generated when an error
		 *				 is caught.
		 *
		 *  @param[in] __p New directory.
		 *  @return Old path.
		 */
		pf_decl_static void set_dumpfile_creation_path(
				std::filesystem::path &&__p) pf_attr_noexcept;

		/// Dumpbin -> Generate
		/*! @brief Generates a dumpbin.
		 *
		 *  @param[in] __p		 Filepath where the dumpbin should be generated.
		 *  @param[in] __flags Dumpbin flags.
		 *  @return Path to the dumpbin.
		 */
		pf_hint_nodiscard pulsar_api pf_decl_static std::filesystem::path generate_dumpbin(
				std::filesystem::path const &__p,
				uint32_t __flags);

		/// Message -> Box
		/*! @brief Generates a message box according to @a __level argument.
		 *
		 *  @param[in] __level	 Level of the message.
		 *  @param[in] __title	 Title of the box.
		 *  @param[in] __message Message.
		 *
		 *  @warning Blocks the main thread.
		 */
		pulsar_api pf_decl_static void generate_messagebox(
				debug_level __level,
				std::string_view __title,
				std::string_view __message);
	};

	/// DEBUG: Exception
	using exception = debugger::exception;

	/// DEBUG: Initializer
	pulsar_api pf_decl_extern const debugger __debugger;
}

#endif // !PULSAR_DEBUGGER_HPP