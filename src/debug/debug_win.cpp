/*! @file   debug_win.cpp
 *  @author Louis-Quentin Noé (noe.louis-quentin@hotmail.fr)
 *  @brief
 *  @date   19-02-2023
 *
 *  @copyright Copyright (c) 2023 - Pulsar Software
 *
 *  @since 0.1.4
 */

// Include: Pulsar
#include "debug/debug_win.hpp"

// Windows
#ifdef PF_OS_WINDOWS
#include <windows.h>
#include <dbghelp.h>

// Pulsar
namespace pul
{
	/// DEBUG: Win -> Stack Trace
	pulsar_api char_t*
	__dbg_format_stacktrace_to(
		char_t *__where,
		size_t __ignore) pf_attr_noexcept
	{
		// 1. Init
		// for now, stacktrace is used only on this process
		// !! no need to get stacktrace of another thread, or another process (for now) !!
		CONTEXT ctx;
		std::memset(&ctx, 0, sizeof(ctx));
		RtlCaptureContext(&ctx);
		// 2. Call
		return __dbg_format_walk_to_win(__where, &ctx, __ignore);
	}

	/// DEBUG: Win -> Print
	pulsar_api void
	dbg_print_exception(
		dbg_category const *__cat,
		uint32_t __code,
		dbg_u8string_view __msg) pf_attr_noexcept
	{
		// 1. Format
		dbg_u8string_view catn = __cat->name();
		dbg_u8string_view catm = __cat->message(__code);
		dbg_u8string str(DBG_FMT_BUFFER_SIZE, '\0');
		char_t *p = str.begin();
		p = __dbg_format_chrono_to(p);
		p = fmt::format_to(p, " T={} /{}/ /{}/ ({}) message={} | ",
											 union_cast<size_t>(std::this_thread::get_id()),
											 fmt::styled('E', fmt::fg(fmt::color::red) | fmt::bg(fmt::color::black)),
											 fmt::styled(catn.begin(), fmt::fg(fmt::color::orange) | fmt::bg(fmt::color::black)),
											 fmt::styled(__code, fmt::fg(fmt::color::red) | fmt::bg(fmt::color::black)),
											 fmt::styled(catm.begin(), fmt::fg(fmt::color::orange) | fmt::bg(fmt::color::black)));
		char_t *k = p;
		p			 = fmt::format_to(p, "{}", __msg.begin());
		p			 = __dbg_reformat_newlines_to(k);
		p			 = __dbg_format_stacktrace_of_exception_to_win(p, 0);
		*(p++) = '\n';
		str.shrink(union_cast<size_t>(p) - union_cast<size_t>(str.begin()) + 1);

		// 2. Print
		logger.write(str);
	}

	/// DEBUG: Initializer
	pulsar_api __dbg_initializer::__dbg_initializer() pf_attr_noexcept
	{
		__dbg_initializer_win::__init();
	}
	pulsar_api __dbg_initializer::~__dbg_initializer() pf_attr_noexcept
	{
		__dbg_initializer_win::__terminate();
	}
}

#endif // !PF_WINDOWSs