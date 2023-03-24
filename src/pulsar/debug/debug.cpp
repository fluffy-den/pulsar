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
#include "pulsar/debug/debug.hpp"

// Pulsar
namespace pul
{
	/// DEBUG: Internal Allocator -> Instance
	allocator_mamd_ring_buffer __dbg_internal_allocator = allocator_mamd_ring_buffer(65536);

	/// DEBUG: Allocate / Deallocate
	pulsar_api void*
	__dbg_allocate(
		size_t __size)
	{
		return __dbg_internal_allocator.allocate(__size, align_val_t(32), 0);
	}
	pulsar_api void
	__dbg_deallocate(
		void *__ptr) pf_attr_noexcept
	{
		return __dbg_internal_allocator.deallocate(__ptr);
	}

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
		dbg_u8print("{}", logo.begin());
		char_t buf[64] = {'\0'};
		dbg_u8format_to(
			&buf[0], "v{0}.{1}.{2}\n\n",
			PULSAR_VERSION_MAJOR,
			PULSAR_VERSION_MINOR,
			PULSAR_VERSION_PATCH);
		dbg_u8print("{}", &buf[0]);
	}

	/// DEBUG: Format
	pulsar_api char_t*
	dbg_format_chrono_to(
		char_t *__where) pf_attr_noexcept
	{
		uint64_t elapsed = union_cast<uint64_t>(logger.elapsed_time().count());
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

	/// DEBUG: Instance
	pulsar_api dbg_logger logger;
	pulsar_api __dbg_initializer initializer;
}