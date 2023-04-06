/*! @file   debug.cpp
 *  @author Louis-Quentin Noé (noe.louis-quentin@hotmail.fr)
 *  @brief
 *  @date   19-02-2023
 *
 *  @copyright Copyright (c) 2023 - Pulsar Software
 *
 *  @since 0.1.4
 */

// Include: Pulsar
#include "pulsar/internal.hpp"

// Pulsar
namespace pul
{
	/// DEBUG: Logger
	pulsar_api dbg_level
	dbg_log_filter() pf_attr_noexcept
	{
		return __internal.dbg_logger.__get_filter();
	}
	pulsar_api dbg_logger_callback_t
	dbg_log_callback() pf_attr_noexcept
	{
		return __internal.dbg_logger.__get_callback();
	}
	pulsar_api void
	dbg_log_set_filter(
		dbg_level __level) pf_attr_noexcept
	{
		return __internal.dbg_logger.__set_filter(__level);
	}
	pulsar_api void
	dbg_log_set_callback(
		dbg_logger_callback_t __callback) pf_attr_noexcept
	{
		return __internal.dbg_logger.__set_callback(__callback);
	}

	pulsar_api nanoseconds_t
	dbg_elapsed_time() pf_attr_noexcept
	{
		return __internal.dbg_logger.__get_elapsed_time();
	}

	pulsar_api void dbg_log(
		dbg_u8string &&__msg)
	{
		return __internal.dbg_logger.__write(std::move(__msg));
	}
	pulsar_api void dbg_log(
		dbg_level __level,
		dbg_u8string &&__msg)
	{
		return __internal.dbg_logger.__write(__level, std::move(__msg));
	}

	/// DEBUG: Format -> Chrono
	pulsar_api char_t*
	__dbg_format_chrono_to(
		char_t *__where) pf_attr_noexcept
	{
		uint64_t elapsed = union_cast<uint64_t>(dbg_elapsed_time().count());
		return dbg_u8format_to(
			__where,
			"[{:0>4}:{:0>2}:{:0>2}:{:0>3}]",
			(elapsed / 3600000000000ull),
			(elapsed / 60000000000ull) % 100ull,
			(elapsed / 1000000000ull) % 100ull,
			(elapsed / 1000000ull) % 1000ull);
	}

	/// DEBUG: Stacktrace
	pf_hint_nodiscard pulsar_api char_t*
	__dbg_append_stacktrace_to(
		char_t *__w,
		__dbg_stacktrace_formatted_t const &__st) pf_attr_noexcept
	{
		return fmt::format_to(__w, "{}", &__st.message[0]);
	}

	/// DEBUG: Terminate
	pulsar_api void
	dbg_set_terminate_handle(
		dbg_terminate_handle_t __handle) pf_attr_noexcept
	{
		__internal.dbg_internal.__set_terminate(__handle);
	}
	pf_hint_nodiscard pulsar_api dbg_terminate_handle_t
	dbg_get_terminate_handle() pf_attr_noexcept
	{
		return __internal.dbg_internal.__get_terminate();
	}
}