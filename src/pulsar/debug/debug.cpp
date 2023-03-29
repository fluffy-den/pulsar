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
	/// DEBUG: Allocate / Deallocate
	pulsar_api void*
	__dbg_allocate(
		size_t __size)
	{
		return __internal.cache.allocate(__size, align_val_t(8), 0);
	}
	pulsar_api void
	__dbg_deallocate(
		void *__ptr) pf_attr_noexcept
	{
		return __internal.cache.deallocate(__ptr);
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

	/// DEBUG: Format
	pulsar_api char_t*
	dbg_format_chrono_to(
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
	pulsar_api char_t*
	dbg_reformat_newlines_to(
		char_t *__w) pf_attr_noexcept
	{
		// 1. Length
		char_t *e = __w;
		while (*e != '\0')
		{
			++e;
		}
		--e;
		size_t n = 0;
		while (*e == '\n')
		{
			++n;
			--e;
		}
		e++;

		// 2. '\n' -> " ... "
		for (; __w != e; ++__w)
		{
			if (*__w == '\n')
			{
				++__w;
				for (char_t *k = e + 17; k != __w + 16; --k)
				{
					*k = *(k - 17);
				}
				std::memset(__w, ' ', 17);
				__w += 17;
				e		+= 17;
			}
		}
		e += n;

		// Returns
		return e;
	}
}