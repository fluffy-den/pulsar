/*! @file   debug_win.hpp
 *  @author Louis-Quentin Noé (noe.louis-quentin@hotmail.fr)
 *  @brief
 *  @date   21-02-2023
 *
 *  @copyright Copyright (c) 2023 - Pulsar Software
 *
 *  @since 0.1.4
 */

#ifndef PULSAR_SRC_DEBUG_WIN_HPP
#define PULSAR_SRC_DEBUG_WIN_HPP 1

// Include: Pulsar -> Src
#include "internal.hpp"

// Include: Pulsar
#include "pulsar/debug.hpp"

// Include: Windows
#include <dbghelp.h>
#ifdef PF_OS_WINDOWS

// Include: C++
#include <exception>
#include <system_error>
#include <future>

// Pulsar
namespace pul
{
	/// DEBUG: Win -> Symbols
	pf_hint_nodiscard char_t*
	__dbg_format_walk_to_win(
		char_t *__where,
		PCONTEXT __ctx,
		size_t __ignore)
	{
		// 1. Symbol Init
		HANDLE thdl = GetCurrentThread();
		HANDLE phdl = GetCurrentProcess();
		SymSetOptions(SYMOPT_DEFERRED_LOADS);
		pf_throw_if(
			!SymInitialize(phdl, nullptr, TRUE),
			dbg_category_system(),
			GetLastError(),
			dbg_flags::dump_with_handle_data,
			"SymInitialize failed! handle={}",
			union_cast<size_t>(phdl));

		// 2. StackFrame
		// per platform stack frame initialisation
		STACKFRAME64 sf;
		std::memset(&sf, 0, sizeof(sf));
		// https://docs.microsoft.com/en-us/windows/win32/api/dbghelp/ns-dbghelp-stackframe
		// x86 platforms
# if defined(PF_ARCHITECTURE_I386)
		DWORD machineType = IMAGE_FILE_MACHINE_I386;
		sf.AddrPC.Offset		= __ctx->Eip;
		sf.AddrPC.Mode			= AddrModeFlat;
		sf.AddrFrame.Offset = __ctx->Ebp;
		sf.AddrFrame.Mode		= AddrModeFlat;
		sf.AddrStack.Offset = __ctx->Esp;
		sf.AddrStack.Mode		= AddrModeFlat;
		// intel itanium
# elif defined(PF_ARCHITECTURE_IA64)
		DWORD machineType = IMAGE_FILE_MACHINE_IA64;
		sf.AddrPC.Offset		 = __ctx->StIIP;
		sf.AddrPC.Mode			 = AddrModeFlat;
		sf.AddrFrame.Offset	 = __ctx->IntSp;
		sf.AddrFrame.Mode		 = AddrModeFlat;
		sf.AddrBStore.Offset = __ctx->RsBSP;
		sf.AddrBStore.Mode	 = AddrModeFlat;
		sf.AddrStack.Offset	 = __ctx->IntSp;
		sf.AddrStack.Mode		 = AddrModeFlat;
		// x64 platforms
# elif defined(PF_ARCHITECTURE_AMD64)
		DWORD machineType = IMAGE_FILE_MACHINE_AMD64;
		sf.AddrPC.Offset		= __ctx->Rip;
		sf.AddrPC.Mode			= AddrModeFlat;
		sf.AddrFrame.Offset = __ctx->Rsp;
		sf.AddrFrame.Mode		= AddrModeFlat;
		sf.AddrStack.Offset = __ctx->Rsp;
		sf.AddrStack.Mode		= AddrModeFlat;
# else
#   error "Platform unsupported!"
# endif
		// symbol handling
		union
		{
			_IMAGEHLP_SYMBOL64 as_symbol;
			BYTE as_buffer[sizeof(as_symbol) + DBG_FMT_NAME_LEN * sizeof(CHAR)];
		};
		std::memset(union_cast<byte_t*>(&as_symbol), 0, sizeof(as_symbol) + sizeof(as_buffer) - 1);
		as_symbol.SizeOfStruct	= sizeof(as_symbol);
		as_symbol.MaxNameLength = DBG_FMT_NAME_LEN;
		// module of the symbol
		IMAGEHLP_MODULE64 m;
		std::memset(&m, 0, sizeof(m));
		m.SizeOfStruct = sizeof(m);
		// number of trace
		uint32_t cn = DBG_FMT_STACK_FRAMES - __ignore;

		// 3. "Walk" the stacktrace
		while (
			StackWalk64(
				machineType, phdl, thdl, &sf, __ctx, nullptr,
				SymFunctionTableAccess64, SymGetModuleBase64, nullptr)
			&& cn > 0)
		{
			// i. Retrieve
			// ignore
			if (__ignore > 0)
			{
				--__ignore;
				continue;
			}

			// ii.format
		#ifdef PF_DEBUG
			// line, file of the symbol
			IMAGEHLP_LINE64 l;
			std::memset(&l, 0, sizeof(l));
			l.SizeOfStruct = sizeof(l);
			DWORD64 sd = 0;
			char_t name[DBG_FMT_NAME_LEN];
			std::memset(&name[0], 0, DBG_FMT_NAME_LEN);
			SymGetSymFromAddr64(phdl, sf.AddrPC.Offset, &sd, &as_symbol);
			UnDecorateSymbolName(&as_symbol.Name[0], &name[0], DBG_FMT_NAME_LEN, UNDNAME_COMPLETE);
			SymGetModuleInfo64(phdl, sf.AddrPC.Offset, &m);
			DWORD ld = 0;
			SymGetLineFromAddr64(phdl, sf.AddrPC.Offset, &ld, &l);
			--cn;
			__where = fmt::format_to(
				__where, "                  at function={}, in module={} at file={}:{}\n",
				&as_symbol.Name[0],
				&m.ModuleName[0],
				&l.FileName[0] ? &l.FileName[0] : "???",
				l.LineNumber);
		#else
			DWORD64 sd = 0;
			char_t name[DBG_FMT_NAME_LEN];
			std::memset(&name[0], 0, DBG_FMT_NAME_LEN);
			SymGetSymFromAddr64(phdl, sf.AddrPC.Offset, &sd, &as_symbol);
			SymGetModuleInfo64(phdl, sf.AddrPC.Offset, &m);
			__where = fmt::format_to(
				__where, "                  at function={}, in module={}\n",
				&as_symbol.Name[0],
				&m.ModuleName[0]);
		#endif
		}

		// 3. Clean
		pf_throw_if(
			!SymCleanup(phdl),
			dbg_category_system(),
			GetLastError(),
			dbg_flags::dump_with_handle_data,
			"SymCleanup failed! handle={}",
			union_cast<size_t>(phdl));

		// 4. Return
		return __where;
	}

	/// DEBUG: Win -> Exception Handler
	class __dbg_exception_handler_win
	{
	// SEH -> Handle
	pf_decl_static LONG WINAPI __vectored_exception_handler(
		PEXCEPTION_POINTERS __info) pf_attr_noexcept
	{
		if (__info->ExceptionRecord->ExceptionInformation[1] == 0
				&& __info != ex_)
		{
			// Exception -> Ex
			ex_ = __info;
		}
		return EXCEPTION_CONTINUE_SEARCH;
	}

	public:
		/// Constructors
		__dbg_exception_handler_win()																		 = delete;
		__dbg_exception_handler_win(__dbg_exception_handler_win const &) = delete;
		__dbg_exception_handler_win(__dbg_exception_handler_win &&)			 = delete;

		/// Operator =
		__dbg_exception_handler_win& operator=(__dbg_exception_handler_win const &) = delete;
		__dbg_exception_handler_win& operator=(__dbg_exception_handler_win &&)			= delete;

		/// Destructor
		~__dbg_exception_handler_win() = delete;

		/// Initializers
		pf_decl_static void __init() pf_attr_noexcept
		{
			sehex_ = AddVectoredExceptionHandler(0, __vectored_exception_handler);
			pf_assert(sehex_, "[WIN] AddVectoredExceptionHandler for printing stacktrace failed! handle={}", union_cast<void*>(sehex_));
		}
		pf_decl_static void __terminate() pf_attr_noexcept
		{
			if (sehex_) RemoveVectoredExceptionHandler(sehex_);
		}

		/// Current Exception
		pf_hint_nodiscard pf_decl_static pf_decl_inline
		DWORD thrower_thread_id() pf_attr_noexcept
		{
			return exID_;
		}
		pf_hint_nodiscard pf_decl_static pf_decl_inline
		PEXCEPTION_POINTERS current_exception() pf_attr_noexcept
		{
			return ex_;
		}

	private:
		// SEH -> Last Local Exception
		pf_decl_static pf_decl_inline PEXCEPTION_POINTERS ex_;
		pf_decl_static pf_decl_inline DWORD exID_;
		pf_decl_static pf_decl_inline PVOID sehex_;
	};

	/// DEBUG: Message
	pf_decl_static dbg_u8string __dbg_generate_error_message_win(
		DWORD __error) pf_attr_noexcept
	{
		// 1. UTF16 message

		// 2. Cast to UTF8

		// 3. Returns
	}

	/// DEBUG: MiniDump
	pf_decl_static void __dbg_generate_popup_message(
		dbg_u8string_view __title,
		dbg_u8string_view __msg) pf_attr_noexcept
	{
		// TODO
	}
	pf_decl_static void __dbg_generate_exception_dumpbin(
		dbg_category const *__cat,
		uint32_t __code,
		uint32_t __flags,
		dbg_u8string_view __what) pf_attr_noexcept
	{
		// 1. Privileges

		// 2. File (locallow/pulsar/(processname).dmp)
		HFILE file = nullptr;	// TODO

		// 3. Replace Last DumpBin
		// Flags
		DWORD flags = MINIDUMP_TYPE::MiniDumpNormal;
		switch(__flags)
		{

		};

		// Exception
		MINIDUMP_EXCEPTION_INFORMATION mei;
		std::memset(&mei, 0, sizeof(mei));
		mei.ThreadId					= __dbg_exception_handler_win::thrower_thread_id();
		mei.ExceptionPointers = __dbg_exception_handler_win::current_exception();
		mei.ClientPointers		= TRUE;

		// Generate
		if(MiniDumpWriteDump(
				 GetCurrentProcess(), GetCurrentProcessId(), file,
				 flags, &mei, nullptr, nullptr) == HRESULT_FROM_WIN32(ERROR_CANCELLED))
		{
			pf_print(
				dbg_type::warning, dbg_level::high,
				"[WIN] Failed to generate dumpbin at path={}, code={}, reason={}",
				filepath.begin(), GetLastError(),
				__dbg_generate_error_message_win().begin());
		}

	}
	pf_decl_static void __dbg_generate_dumpbin(
		uint32_t __flags,
		dbg_u8string_view __what) pf_attr_noexcept
	{
		// TODO
	}

	/// DEBUG: Win -> Terminate Handler
	class __dbg_initializer_win
	{
	/// Terminate
	pf_decl_static void __terminate_handle() pf_attr_noexcept
	{
		/// Dumpbin
		#if defined(PF_RELEASE)
		/// Exception?
		std::exception_ptr p = std::current_exception();
		if (p)
		{
			try
			{
				std::rethrow_exception(p);
			}
			catch(auto const &__e)
			{
				// It's an exception. Generate dumpbin
				__dbg_generate_exception_dumpbin();
			}
		}
		else
		{
			__dbg_generate_dumpbin();
		}
		#endif

		/// Exit
		std::exit(SIGABRT);
	}

	public:
		/// Constructors
		__dbg_initializer_win()															= delete;
		__dbg_initializer_win(__dbg_initializer_win const&) = delete;
		__dbg_initializer_win(__dbg_initializer_win &&)			= delete;

		/// Operator =
		__dbg_initializer_win &operator=(__dbg_initializer_win const &) = delete;
		__dbg_initializer_win &operator=(__dbg_initializer_win &&)			= delete;

		/// Destructor
		~__dbg_initializer_win() = delete;

		/// Initializers
		pf_decl_static void __init() pf_attr_noexcept
		{
			__dbg_exception_handler_win::__init();
			std::set_terminate(__terminate_handle);
		}
		pf_decl_static void __terminate() pf_attr_noexcept
		{
			__dbg_exception_handler_win::__terminate();
		}
	};

	/// DEBUG: Win -> Format
	pf_hint_nodiscard pf_decl_static char_t*
	__dbg_format_stacktrace_of_exception_to_win(
		char_t *__where,
		size_t __ignore)
	{
		// 1. Init
		PEXCEPTION_POINTERS p = __dbg_exception_handler_win::current_exception();
		// 2. Call
		return __dbg_format_walk_to_win(__where, p->ContextRecord, __ignore);
	}
}

#endif // !PF_OS_WINDOWS

#endif // !PULSAR_SRC_DEBUG_WIN_HPP