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
	/// DEBUG: Exception
	pulsar_api dbg_exception::dbg_exception(
		dbg_category const *__cat,
		uint32_t __code,
		uint32_t __flags,
		dbg_u8string_view __msg) pf_attr_noexcept
		: cat_(__cat)
		, msg_(__msg)
		, code_(__code)
		, flags_(__flags)
	{
		// Size
		auto st	 = __dbg_retrieve_stacktrace();
		size_t s = DBG_FMT_WRITE_OFFSET
							 + fmt::formatted_size(" /{}/ category={}, code={}, message={}|{}",
																		 dbg_styled('E', fmt::fg(fmt::color::red)),
																		 dbg_styled(this->cat_->name().data(), fmt::fg(fmt::color::fire_brick)),
																		 dbg_styled(this->code_, fmt::fg(fmt::color::green_yellow)),
																		 this->cat_->message(this->code_).data(),
																		 this->msg_.data())
							 + __dbg_formatted_stacktrace_size(st) + 1;

		// Message
		dbg_u8string str(s, '\0');
		char_t *k = str.data();
		k = __dbg_format_chrono_to(k);
		k = fmt::format_to(k, " /{}/ category={}, code={}, message={}|{}\n",
											 dbg_styled('E', fmt::fg(fmt::color::red)),
											 dbg_styled(this->cat_->name().data(), fmt::fg(fmt::color::fire_brick)),
											 dbg_styled(this->code_, fmt::fg(fmt::color::green_yellow)),
											 this->cat_->message(this->code_).data(),
											 this->msg_.data());
		k = __dbg_format_stacktrace_to(k, st);
		__dbg_print("{}", str.data());
	}

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
	pf_hint_nodiscard pulsar_api size_t
	__dbg_formatted_stacktrace_size(
		__dbg_stacktrace_t const &__st) pf_attr_noexcept
	{
		size_t c = 0;
		for (size_t i = 0; i < __st.available; ++i)
		{
			c += strlen(&__st.trace[DBG_FMT_NAME_LEN * i]) + 2;
		}
		return c;
	}
	pf_hint_nodiscard pulsar_api char_t*
	__dbg_format_stacktrace_to(
		char_t *__w,
		__dbg_stacktrace_t const &__st) pf_attr_noexcept
	{
		for (size_t i = 0; i < __st.available; ++i)
		{
			__w = fmt::format_to(__w, "\t{}\n", &__st.trace[DBG_FMT_NAME_LEN * i]);
		}
		return __w;
	}
}