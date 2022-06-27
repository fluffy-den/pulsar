/*! @file   debug_win.cpp
 *  @author Fluffy (noe.louis-quentin@hotmail.fr)
 *  @brief	Implements debugging utilities under Windows.
 *  @date   22-01-2022
 *
 *  @copyright Copyright (c) 2022 - Pulsar Software
 *
 *  @since 0.1.0
 */

// Include: Pulsar
#include "pulsar/debug.hpp"
#include "pulsar/string.hpp"

// Include: Windows
#ifdef PF_OS_WINDOWS
#	include <windows.h>
#	include <dbghelp.h>

// Include: C
#	include <cstring>

// Pulsar
namespace pul
{

	/// StackTrace
	// Constants
	pf_decl_constexpr uint32_t MAX_FRAMES_NUM = 63;
	pf_decl_constexpr uint32_t MAX_NAME_LEN		= 1024;

	// Debug Trace
	pf_hint_nodiscard pulsar_api std::vector<debug_trace_t> debugger::stacktrace(
			uint32_t __ignoreNum)
	{
		// for now, stacktrace is used only on this process
		// !! no need to get stacktrace of another thread, or another process (for now) !!
		HANDLE phdl = GetCurrentProcess();
		HANDLE thdl = GetCurrentThread();
		// symbol initialisation
		SymSetOptions(SYMOPT_UNDNAME | SYMOPT_DEFERRED_LOADS);
		if (!SymInitialize(phdl, nullptr, TRUE))
			throw exception(
					std::system_category(),
					GetLastError(),
					"",
					debug_flags::write_in_logs | debug_flags::dump_with_handle_data);
		// current context retrieval
		CONTEXT ctx;
		std::memset(&ctx, 0, sizeof(ctx));
		RtlCaptureContext(&ctx);
		// per platform stack frame initialisation
		STACKFRAME64 sf;
		std::memset(&sf, 0, sizeof(sf));
		// https://docs.microsoft.com/en-us/windows/win32/api/dbghelp/ns-dbghelp-stackframe
		// x86 platforms
#	if defined(PF_ARCHITECTURE_I386)
		DWORD machineType		= IMAGE_FILE_MACHINE_I386;
		sf.AddrPC.Offset		= ctx.Eip;
		sf.AddrPC.Mode			= AddrModeFlat;
		sf.AddrFrame.Offset = ctx.Ebp;
		sf.AddrFrame.Mode		= AddrModeFlat;
		sf.AddrStack.Offset = ctx.Esp;
		sf.AddrStack.Mode		= AddrModeFlat;
		// intel itanium
#	elif defined(PF_ARCHITECTURE_IA64)
		DWORD machineType		 = IMAGE_FILE_MACHINE_IA64;
		sf.AddrPC.Offset		 = ctx.StIIP;
		sf.AddrPC.Mode			 = AddrModeFlat;
		sf.AddrFrame.Offset	 = ctx.IntSp;
		sf.AddrFrame.Mode		 = AddrModeFlat;
		sf.AddrBStore.Offset = ctx.RsBSP;
		sf.AddrBStore.Mode	 = AddrModeFlat;
		sf.AddrStack.Offset	 = ctx.IntSp;
		sf.AddrStack.Mode		 = AddrModeFlat;
		// x64 platforms
#	elif defined(PF_ARCHITECTURE_AMD64)
		DWORD machineType		= IMAGE_FILE_MACHINE_AMD64;
		sf.AddrPC.Offset		= ctx.Rip;
		sf.AddrPC.Mode			= AddrModeFlat;
		sf.AddrFrame.Offset = ctx.Rsp;
		sf.AddrFrame.Mode		= AddrModeFlat;
		sf.AddrStack.Offset = ctx.Rsp;
		sf.AddrStack.Mode		= AddrModeFlat;
#	else
#		error "Platform unsupported!"
#	endif
		// symbol handling
		union
		{
			_IMAGEHLP_SYMBOL64 as_symbol;
			BYTE as_buffer[sizeof(as_symbol) + MAX_NAME_LEN * sizeof(CHAR)];
		} s;
		std::memset(&s.as_buffer[0], 0, sizeof(s));
		s.as_symbol.SizeOfStruct	= sizeof(s.as_symbol);
		s.as_symbol.MaxNameLength = MAX_NAME_LEN;
		// line, file of the symbol
		IMAGEHLP_LINE64 l;
		std::memset(&l, 0, sizeof(l));
		l.SizeOfStruct = sizeof(l);
		// module of the symbol
		IMAGEHLP_MODULE64 m;
		std::memset(&m, 0, sizeof(m));
		m.SizeOfStruct = sizeof(m);
		// number of trace
		uint32_t cn		 = MAX_FRAMES_NUM - __ignoreNum;
		// walk on the trace
		std::vector<void *> tL(cn);
		std::vector<debug_trace_t> sL;
		sL.reserve(cn);
		while (StackWalk64(
							 machineType,
							 phdl,
							 thdl,
							 &sf,
							 &ctx,
							 nullptr,
							 SymFunctionTableAccess64,
							 SymGetModuleBase64,
							 nullptr)
					 && cn > 0)
		{
			// ignore
			if (__ignoreNum > 0)
			{
				--__ignoreNum;
				continue;
			}
			// get symbol
			DWORD64 sd = 0;
			std::string name(MAX_NAME_LEN, ' '), undname(MAX_NAME_LEN, ' ');
			if (!SymGetSymFromAddr64(phdl, sf.AddrPC.Offset, &sd, &s.as_symbol))
				debugger::log(
						debug_level::warning,
						debug_filter::detail,
						exception::format(
								std::system_category(),
								GetLastError(),
								strfmt("SymGetSymFromAddr64 failed for addr=0x%llx", sf.AddrPC.Offset)));
			// undecorated Name
			if (!UnDecorateSymbolName(&s.as_symbol.Name[0], name.data(), name.length(), UNDNAME_NAME_ONLY))
				debugger::log(
						debug_level::warning,
						debug_filter::detail,
						exception::format(
								std::system_category(),
								GetLastError(),
								strfmt("UnDecorateSymbolName failed for addr=0x%llx", sf.AddrPC.Offset)));
			strtriml(name);
			strtrimr(name);
			strtolower(name);
			// fully Undecorated Name
			if (!UnDecorateSymbolName(&s.as_symbol.Name[0], undname.data(), undname.length(), UNDNAME_COMPLETE))
				debugger::log(
						debug_level::warning,
						debug_filter::detail,
						exception::format(
								std::system_category(),
								GetLastError(),
								strfmt("UnDecorateSymbolName failed for addr=0x%llx", sf.AddrPC.Offset)));
			strtriml(undname);
			strtrimr(undname);
			strtolower(undname);
			// module name
			if (!SymGetModuleInfo64(phdl, sf.AddrPC.Offset, &m))
				debugger::log(
						debug_level::warning,
						debug_filter::detail,
						exception::format(
								std::system_category(),
								GetLastError(),
								strfmt("SymGetModuleInfo64 failed for addr=0x%llx", sf.AddrPC.Offset)));
			std::string modname = m.ModuleName;
			strtriml(modname);
			strtrimr(modname);
			strtolower(modname);
			// line
			DWORD ld = 0;
			if (!SymGetLineFromAddr64(phdl, sf.AddrPC.Offset, &ld, &l))
				debugger::log(
						debug_level::warning,
						debug_filter::detail,
						exception::format(
								std::system_category(),
								GetLastError(),
								strfmt("SymGetLineFromAddr64 failed for addr=0x%llx", sf.AddrPC.Offset)));
			// trace
			sL.emplace_back(
					std::move(undname),
					std::move(name),
					l.FileName == nullptr ? "??" : l.FileName,
					std::move(modname),
					l.LineNumber);
			// remaining trace to walk on
			--cn;
		}
		// clean-up
		if (!SymCleanup(phdl))
			throw exception(
					std::system_category(),
					GetLastError(),
					"",
					debug_flags::write_in_logs | debug_flags::dump_with_handle_data);
		return sL;
	}

	/// DumpBin
	pulsar_api pf_hint_nodiscard std::filesystem::path debugger::generate_dumpbin(
			std::filesystem::path const &__p,
			uint32_t __flags)
	{
		// __p must be a directory!
		if (__p.has_extension())
			throw exception(
					std::generic_category(),
					debugger::generic_code(std::errc::not_a_directory),
					"__p must be a directory!");
		// What flags to generate the dumpbin?
		union
		{
			MINIDUMP_TYPE as_minidump_t;
			uint32_t as_uint32_t;
		};
		as_minidump_t = MINIDUMP_TYPE::MiniDumpNormal;
		if ((__flags & debug_flags::dump_with_data_segs) == debug_flags::dump_with_data_segs)
			as_uint32_t |= MINIDUMP_TYPE::MiniDumpWithDataSegs;
		if ((__flags & debug_flags::dump_with_full_memory) == debug_flags::dump_with_full_memory)
			as_uint32_t |= MINIDUMP_TYPE::MiniDumpWithFullMemory;
		if ((__flags & debug_flags::dump_with_handle_data) == debug_flags::dump_with_handle_data)
			as_uint32_t |= MINIDUMP_TYPE::MiniDumpWithHandleData;
		if ((__flags & debug_flags::dump_with_filter_memory) == debug_flags::dump_with_filter_memory)
			as_uint32_t |= MINIDUMP_TYPE::MiniDumpFilterMemory;
		if ((__flags & debug_flags::dump_with_scan_memory) == debug_flags::dump_with_scan_memory)
			as_uint32_t |= MINIDUMP_TYPE::MiniDumpScanMemory;
		if ((__flags & debug_flags::dump_with_unloaded_modules) == debug_flags::dump_with_unloaded_modules)
			as_uint32_t |= MINIDUMP_TYPE::MiniDumpWithUnloadedModules;
		if ((__flags & debug_flags::dump_with_indirectly_referenced_memory) == debug_flags::dump_with_indirectly_referenced_memory)
			as_uint32_t |= MINIDUMP_TYPE::MiniDumpWithIndirectlyReferencedMemory;
		if ((__flags & debug_flags::dump_with_filter_module_paths) == debug_flags::dump_with_filter_module_paths)
			as_uint32_t |= MINIDUMP_TYPE::MiniDumpFilterModulePaths;
		if ((__flags & debug_flags::dump_with_process_thread_data) == debug_flags::dump_with_process_thread_data)
			as_uint32_t |= MINIDUMP_TYPE::MiniDumpWithProcessThreadData;
		if ((__flags & debug_flags::dump_with_private_readwrite_memory) == debug_flags::dump_with_private_readwrite_memory)
			as_uint32_t |= MINIDUMP_TYPE::MiniDumpWithPrivateReadWriteMemory;
		if ((__flags & debug_flags::dump_without_auxiliary_state) == debug_flags::dump_without_auxiliary_state)
			as_uint32_t |= MINIDUMP_TYPE::MiniDumpWithoutAuxiliaryState;
		if ((__flags & debug_flags::dump_with_full_auxiliary_state) == debug_flags::dump_with_full_auxiliary_state)
			as_uint32_t |= MINIDUMP_TYPE::MiniDumpWithFullAuxiliaryState;
		if ((__flags & debug_flags::dump_with_private_write) == debug_flags::dump_with_private_write)
			as_uint32_t |= MINIDUMP_TYPE::MiniDumpWithPrivateWriteCopyMemory;
		if ((__flags & debug_flags::dump_without_inaccessible_memory) == debug_flags::dump_without_inaccessible_memory)
			as_uint32_t |= MINIDUMP_TYPE::MiniDumpIgnoreInaccessibleMemory;
		if ((__flags & debug_flags::dump_with_token_information) == debug_flags::dump_with_token_information)
			as_uint32_t |= MINIDUMP_TYPE::MiniDumpWithTokenInformation;
		if ((__flags & debug_flags::dump_with_module_headers) == debug_flags::dump_with_module_headers)
			as_uint32_t |= 0x00080000; // MINIDUMP_TYPE::MiniDumpWithModuleHeaders;
		if ((__flags & debug_flags::dump_with_filter_triage) == debug_flags::dump_with_filter_triage)
			as_uint32_t |= 0x00100000; // MINIDUMP_TYPE::MiniDumpWithFilterTriage;
		if ((__flags & debug_flags::dump_with_avx_state_context) == debug_flags::dump_with_avx_state_context)
			as_uint32_t |= 0x00200000; // MINIDUMP_TYPE::MiniDumpWithAvxXStateContext;
		if ((__flags & debug_flags::dump_with_ipt_trace) == debug_flags::dump_with_ipt_trace)
			as_uint32_t |= 0x00400000; // MINIDUMP_TYPE::MiniDumpWithIptTrace;
		if ((__flags & debug_flags::dump_with_inaccessible_partial_pages) == debug_flags::dump_with_inaccessible_partial_pages)
			as_uint32_t |= 0x00800000; // MINIDUMP_TYPE::MiniDumpScanInaccessiblePartialPages;
		if ((__flags & debug_flags::dump_with_valid_type_flags) == debug_flags::dump_with_valid_type_flags)
			as_uint32_t |= 0x01ffffff; // MINIDUMP_TYPE ::MiniDumpValidTypeFlags;
		// generate directories
		try
		{
			if (!std::filesystem::exists(__p))
				std::filesystem::create_directories(__p);
		}
		catch (std::system_error const &__err)
		{
			throw exception(
					std::system_category(),
					__err.code().value(),
					"",
					debug_flags::write_in_logs | debug_flags::dump_with_handle_data);
		}
		// generate filename
		std::time_t t = std::chrono::high_resolution_clock::to_time_t(
				std::chrono::high_resolution_clock::now());
		std::filesystem::path p = __p / (strfmt("%d%b%y-%H%M%S", std::gmtime(&t)) + ".dmp");
		// generate file
		HANDLE fhdl							= CreateFileW(
				p.wstring().c_str(),
				GENERIC_READ | GENERIC_WRITE,
				0,
				nullptr,
				CREATE_ALWAYS,
				FILE_ATTRIBUTE_NORMAL,
				nullptr);
		if (fhdl == INVALID_HANDLE_VALUE)
			throw exception(std::system_category(), GetLastError());
		// generate dumpbin
		if (!MiniDumpWriteDump(
						GetCurrentProcess(),
						GetCurrentProcessId(),
						fhdl,
						as_minidump_t,
						nullptr,
						nullptr,
						nullptr))
			throw exception(
					std::system_category(),
					GetLastError(),
					"",
					debug_flags::write_in_logs | debug_flags::dump_with_handle_data);
		// close the file
		CloseHandle(fhdl);
		// success
		return p;
	}

	/// Debug Message box
	pulsar_api void debugger::generate_messagebox(
			debug_level __level,
			std::string_view __title,
			std::string_view __message)
	{
		// type of pop-up message
		UINT type = 0;
		switch (__level)
		{
			case debug_level::warning:
				type = MB_ICONWARNING;
				break;
			case debug_level::info:
				type = MB_ICONINFORMATION;
				break;
			default:
			case debug_level::error:
				type = MB_ICONERROR;
				break;
		}
		// generate the pop-up message
		// !! pop-up isn't owned by any handles !
		if (!MessageBoxA(nullptr, __message.data(), __title.data(), type))
		{
			throw exception(
					std::system_category(),
					GetLastError(),
					"",
					debug_flags::write_in_logs | debug_flags::dump_with_handle_data);
		}
	}
}

#endif // PF_OS_WINDOWS