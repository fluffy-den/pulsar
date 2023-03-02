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
#include "pulsar/debug.hpp"

// Pulsar
namespace pul
{
	/// DEBUG: Logger
	pulsar_api dbg_logger::dbg_logger() pf_attr_noexcept
		: c_(nullptr)
		, timer_(high_resolution_clock_t::now())
	{
		dbg_u8string_view logo =
			R"(
        ██████╗ ██╗   ██╗██╗     ███████╗ █████╗ ██████╗ 
        ██╔══██╗██║   ██║██║     ██╔════╝██╔══██╗██╔══██╗
        ██████╔╝██║   ██║██║     ███████╗███████║██████╔╝
        ██╔═══╝ ██║   ██║██║     ╚════██║██╔══██║██╔══██╗
        ██║     ╚██████╔╝███████╗███████║██║  ██║██║  ██║
        ╚═╝      ╚═════╝ ╚══════╝╚══════╝╚═╝  ╚═╝╚═╝  ╚═╝  )";
		fmt::print("{}", logo.begin());
		char_t buf[64] = {'\0'};
		fmt::format_to(
			&buf[0], "v{0}.{1}.{2}\n\n",
			PULSAR_VERSION_MAJOR,
			PULSAR_VERSION_MINOR,
			PULSAR_VERSION_PATCH);
		fmt::print("{}", &buf[0]);
	}

	/// DEBUG: Format
	pulsar_api char_t*
	dbg_format_chrono_to(
		char_t *__where) pf_attr_noexcept
	{
		uint64_t elapsed = union_cast<uint64_t>(logger.elapsed_time().count());
		return fmt::format_to(
			__where,
			"[{:0>4}:{:0>2}:{:0>2}:{:0>4}]",
			(elapsed / 3600000000000ull),
			(elapsed / 60000000000ull) % 100ull,
			(elapsed / 1000000000ull) % 100ull,
			(elapsed / 1000000ull) % 10000ull);
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
				for (char_t *k = e + 18; k != __w + 17; --k)
				{
					*k = *(k - 18);
				}
				std::memset(__w, ' ', 18);
				__w += 18;
				e		+= 18;
			}
		}
		e += n;

		// Returns
		return e;
	}

	/// DEBUG: Instance
	pulsar_api dbg_logger logger;
	pulsar_api __dbg_initializer initializer;
}