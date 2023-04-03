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
#include "pulsar/internal.hpp"

// Windows
#ifdef PF_OS_WINDOWS
#include <windows.h>
#include <dbghelp.h>

// Pulsar
namespace pul
{
	/// DEBUG: Win -> Stack Trace
	__dbg_stacktrace_t
	__dbg_capture_stacktrace(
		uint32_t __ignore) pf_attr_noexcept
	{
		// Symbol Init
		HANDLE thdl = GetCurrentThread();
		HANDLE phdl = GetCurrentProcess();
		SymSetOptions(SYMOPT_DEFERRED_LOADS);
		if (!SymInitializeW(phdl, nullptr, TRUE))
		{
			pf_print(dbg_type::warning, dbg_level::high, "[WIN] SymInitialize failed! handle={}", phdl);
			return { 0, 0 };
		}

		// StackFrame
		CONTEXT ctx;
		memset(&ctx, 0, sizeof(ctx));
		RtlCaptureContext(&ctx);
		// per platform stack frame initialisation
		STACKFRAME64 sf;
		memset(&sf, 0, sizeof(sf));
		// https://docs.microsoft.com/en-us/windows/win32/api/dbghelp/ns-dbghelp-stackframe
		// x86 platforms
# if defined(PF_ARCHITECTURE_I386)
		DWORD machineType = IMAGE_FILE_MACHINE_I386;
		sf.AddrPC.Offset		= ctx.Eip;
		sf.AddrPC.Mode			= AddrModeFlat;
		sf.AddrFrame.Offset = ctx.Ebp;
		sf.AddrFrame.Mode		= AddrModeFlat;
		sf.AddrStack.Offset = ctx.Esp;
		sf.AddrStack.Mode		= AddrModeFlat;
		// intel itanium
# elif defined(PF_ARCHITECTURE_IA64)
		DWORD machineType = IMAGE_FILE_MACHINE_IA64;
		sf.AddrPC.Offset		 = ctx.StIIP;
		sf.AddrPC.Mode			 = AddrModeFlat;
		sf.AddrFrame.Offset	 = ctx.IntSp;
		sf.AddrFrame.Mode		 = AddrModeFlat;
		sf.AddrBStore.Offset = ctx.RsBSP;
		sf.AddrBStore.Mode	 = AddrModeFlat;
		sf.AddrStack.Offset	 = ctx.IntSp;
		sf.AddrStack.Mode		 = AddrModeFlat;
		// x64 platforms
# elif defined(PF_ARCHITECTURE_AMD64)
		DWORD machineType = IMAGE_FILE_MACHINE_AMD64;
		sf.AddrPC.Offset		= ctx.Rip;
		sf.AddrPC.Mode			= AddrModeFlat;
		sf.AddrFrame.Offset = ctx.Rsp;
		sf.AddrFrame.Mode		= AddrModeFlat;
		sf.AddrStack.Offset = ctx.Rsp;
		sf.AddrStack.Mode		= AddrModeFlat;
# else
#   error "Platform unsupported!"
# endif
		// symbol handling
		union
		{
			IMAGEHLP_SYMBOL64 as_symbol;
			BYTE as_buffer[sizeof(as_symbol) + DBG_FMT_NAME_LEN * sizeof(CHAR)];
		};
		memset(union_cast<byte_t*>(&as_buffer), 0, sizeof(as_buffer));
		as_symbol.SizeOfStruct	= sizeof(as_symbol);
		as_symbol.MaxNameLength = DBG_FMT_NAME_LEN;
		// module of the symbol
		IMAGEHLP_MODULE64 m;
		memset(&m, 0, sizeof(m));
		m.SizeOfStruct = sizeof(m);
		// number of trace
		uint32_t cn = DBG_FMT_STACK_FRAMES - __ignore;

		// "Walk" the stacktrace
		__dbg_stacktrace_t st = { 0 };
		while (
			StackWalk64(
				machineType, phdl, thdl, &sf, &ctx, nullptr,
				SymFunctionTableAccess64, SymGetModuleBase64, nullptr)
			&& cn > 0)
		{
			// Retrieve
			// ignore
			if (__ignore > 0)
			{
				--__ignore;
				continue;
			}

			// format
		#ifdef PF_DEBUG
			// line, file of the symbol
			IMAGEHLP_LINE64 l;
			memset(&l, 0, sizeof(l));
			l.SizeOfStruct = sizeof(l);
			DWORD64 sd = 0;
			char_t name[DBG_FMT_NAME_LEN];
			SymGetSymFromAddr64(phdl, sf.AddrPC.Offset, &sd, &as_symbol);
			UnDecorateSymbolName(&as_symbol.Name[0], &name[0], DBG_FMT_NAME_LEN, UNDNAME_COMPLETE);
			SymGetModuleInfo64(phdl, sf.AddrPC.Offset, &m);
			DWORD ld = 0;
			SymGetLineFromAddr64(phdl, sf.AddrPC.Offset, &ld, &l);
			auto *w = &st.trace[DBG_FMT_NAME_LEN * (st.available++)];
			fmt::format_to(
				w, "at function={}, in module={} at file={}:{}",
				&as_symbol.Name[0],
				&m.ModuleName[0],
				&l.FileName[0] ? &l.FileName[0] : "???",
				l.LineNumber);
			--cn;
		#else
			DWORD64 sd = 0;
			char_t name[DBG_FMT_NAME_LEN];
			SymGetSymFromAddr64(phdl, sf.AddrPC.Offset, &sd, &as_symbol);
			SymGetModuleInfo64(phdl, sf.AddrPC.Offset, &m);
			auto *w = &st.trace[DBG_FMT_NAME_LEN * (st.available++)];
			fmt::format_to(
				w, "at function={}, in module={}",
				&as_symbol.Name[0],
				&m.ModuleName[0]);
			--cn;
		#endif
		}

		// Clean
		SymCleanup(phdl);

		// Return
		return st;
	}
	pulsar_api __dbg_stacktrace_t
	__dbg_retrieve_stacktrace() pf_attr_noexcept
	{
		return __dbg_capture_stacktrace(DBG_FMT_STACK_FRAMES_IGNORE);
	}

	/// DEBUG: Win -> Category -> System
	pulsar_api dbg_u8string
	dbg_category_system_t::message(
		uint32_t __val) const pf_attr_noexcept
	{
		return __dbg_generate_error_message_win(__val);
	}

	/// DEBUG: Win -> Print
	pulsar_api void
	dbg_print_exception(
		dbg_category const *__cat,
		uint32_t __code,
		dbg_u8string_view __msg) pf_attr_noexcept
	{
		// Initialisation
		auto st = __dbg_retrieve_stacktrace();
		auto ID = this_thread::get_id();

		// Size
		const size_t s = DBG_FMT_WRITE_OFFSET
										 + fmt::formatted_size(" T={} /{}/ /{}/ ({}) message={} | ",
																					 ID,
																					 dbg_styled('E', dbg_style_fg(dbg_color::red)),
																					 dbg_styled(__cat->name().data(), dbg_style_fg(dbg_color::orange)),
																					 dbg_styled(__code, dbg_style_fg(dbg_color::red)), dbg_styled(__cat->message(__code).data(), dbg_style_fg(dbg_color::orange)))
										 + __dbg_formatted_stacktrace_size(st) + fmt::formatted_size("{}\n", __msg.begin()) + 2;// 2 == '\0' + '\n'

		// Format
		dbg_u8string str(s, '\0');
		char_t *p = str.begin();
		p = __dbg_format_chrono_to(p);
		p = dbg_u8format_to(p,
												" T={} /{}/ /{}/ ({}) message={} | ",
												ID,
												dbg_styled('E', dbg_style_fg(dbg_color::red)),
												dbg_styled(__cat->name().data(), dbg_style_fg(dbg_color::orange)),
												dbg_styled(__code, dbg_style_fg(dbg_color::red)),
												dbg_styled(__cat->message(__code).data(), dbg_style_fg(dbg_color::orange)));
		p			 = dbg_u8format_to(p, "{}\n", __msg.begin());
		p			 = __dbg_format_stacktrace_to(p, st);
		*(p++) = '\n';

		// 2. Print
		dbg_log(std::move(str));
	}

	/// DEBUG: Exception Context -> Retrieve
	pulsar_api dbg_exception_context
	__dbg_retrieve_current_exception_context()
	{
		return dbg_exception_context(
			std::current_exception(),
			__internal.dbg_ex_ctx.__get_current_exception_pointer(),
			__internal.dbg_ex_ctx.__get_ID());
	}

	/// DEBUG: Win -> Exception Handler
	/// Handle
	LONG WINAPI __dbg_exception_context_t::__vectored_exception_handler(
		EXCEPTION_POINTERS *__info) pf_attr_noexcept
	{
		const uint32_t ID = this_thread::get_id();
		__buffer_t *b			= &__internal.dbg_ex_ctx.buffer_[ID];
		if (__info->ExceptionRecord->ExceptionInformation[1] == 0
				&& __info != b->exp)
		{
			// Exception -> Ex
			b->ID	 = ID;
			b->exp = __info;
		}
		return EXCEPTION_CONTINUE_SEARCH;
	}

	/// Constructors
	__dbg_exception_context_t::__dbg_exception_context_t() pf_attr_noexcept
		: buffer_(union_cast<__buffer_t*>(halloc(sizeof(__buffer_t) * CCY_NUM_THREADS)))
	{
		this->handle_ = AddVectoredExceptionHandler(0, __vectored_exception_handler);
		pf_assert(this->handle_, "[WIN] AddVectoredExceptionHandler for printing stacktrace failed! handle={}", this->handle_);
		std::set_terminate(__dbg_terminate_win);
	}

	/// Destructor
	__dbg_exception_context_t::~__dbg_exception_context_t() pf_attr_noexcept
	{
		hfree(this->buffer_);
		if (this->handle_ && !RemoveVectoredExceptionHandler(this->handle_))
		{
			pf_print(dbg_type::warning, dbg_level::low, "[WIN] RemoveVectoredExceptionHandler failed for handle={}!", this->handle_);
		}
	}

	/// DEBUG: Exception -> Context
	void
	dbg_exception_context::rethrow() pf_attr_noexcept
	{
		if (this->ptr_)
		{
			__internal.dbg_ex_ctx.__move_exception_to(this_thread::get_id(), this);
			std::rethrow_exception(this->ptr_);
		}
	}

	/// DEBUG: Win -> Convert
	__dbg_wsstring __dbg_convert_u8_to_wide_win(
		const char_t *__buf,
		size_t __size)
	{
		// 1. Buffer
		if (!__buf || !__size) return __dbg_wsstring();
		const size_t r = MultiByteToWideChar(CP_UTF8, 0, __buf, __size, nullptr, 0);
		__dbg_wsstring str(r, '\0');
		if (pf_unlikely(!MultiByteToWideChar(CP_UTF8, 0, __buf, __size, str.begin(), str.size())))
			pf_throw(
				dbg_category_system(), GetLastError(), dbg_flags::none,
				"[WIN] MultiByteToWideChar bad convertion! buffer={}, size={}",
				union_cast<void*>(__buf), __size);

		// 2. Returns
		return str;
	}
	dbg_u8string
	__dbg_convert_wide_to_u8_win(
		const wchar_t *__buf,
		size_t __count)
	{
		// 1. Buffer
		if (!__buf || !__count) return dbg_u8string();
		const size_t r = WideCharToMultiByte(CP_UTF8, 0, __buf, __count, nullptr, 0, 0, 0);
		dbg_u8string str(r, '\0');
		if (pf_unlikely(!WideCharToMultiByte(CP_UTF8, 0, __buf, __count, str.begin(), str.size(), 0, 0)))
			pf_throw(
				dbg_category_system(), GetLastError(), dbg_flags::none,
				"[WIN] WideCharToMultiByte bad convertion! buffer={}, count={}",
				union_cast<void*>(__buf), __count);

		// 2. Returns
		return str;
	}

	/// DEBUG: Win -> Generate
	MINIDUMP_TYPE
	__dbg_flags_to_minidump_type_win(
		uint32_t __flags) pf_attr_noexcept
	{
		DWORD flags = MINIDUMP_TYPE::MiniDumpNormal;
		if (__flags & dbg_flags::dump_with_data_segs)
			flags |= MINIDUMP_TYPE::MiniDumpWithDataSegs;
		if (__flags & dbg_flags::dump_with_full_memory)
			flags |= MINIDUMP_TYPE::MiniDumpWithFullMemory;
		if (__flags & dbg_flags::dump_with_handle_data)
			flags |= MINIDUMP_TYPE::MiniDumpWithHandleData;
		if (__flags & dbg_flags::dump_filter_memory)
			flags |= MINIDUMP_TYPE::MiniDumpFilterMemory;
		if (__flags & dbg_flags::dump_with_scan_memory)
			flags |= MINIDUMP_TYPE::MiniDumpScanMemory;
		if (__flags & dbg_flags::dump_with_unloaded_modules)
			flags |= MINIDUMP_TYPE::MiniDumpWithUnloadedModules;
		if (__flags & dbg_flags::dump_with_indirectly_referenced_memory)
			flags |= MINIDUMP_TYPE::MiniDumpWithIndirectlyReferencedMemory;
		if (__flags & dbg_flags::dump_with_filter_module_paths)
			flags |= MINIDUMP_TYPE::MiniDumpFilterModulePaths;
		if (__flags & dbg_flags::dump_with_process_thread_data)
			flags |= MINIDUMP_TYPE::MiniDumpWithProcessThreadData;
		if (__flags & dbg_flags::dump_with_private_read_write_memory)
			flags |= MINIDUMP_TYPE::MiniDumpWithPrivateReadWriteMemory;
		if (__flags & dbg_flags::dump_without_auxiliary_state)
			flags |= MINIDUMP_TYPE::MiniDumpWithoutAuxiliaryState;
		if (__flags & dbg_flags::dump_with_full_auxiliary_state)
			flags |= MINIDUMP_TYPE::MiniDumpWithFullAuxiliaryState;
		if (__flags & dbg_flags::dump_with_private_read_write_memory)
			flags |= MINIDUMP_TYPE::MiniDumpWithPrivateReadWriteMemory;
		if (__flags & dbg_flags::dump_ignore_inaccessible_memory)
			flags |= MINIDUMP_TYPE::MiniDumpIgnoreInaccessibleMemory;
		if (__flags & dbg_flags::dump_with_token_information)
			flags |= MINIDUMP_TYPE::MiniDumpWithTokenInformation;
		if (__flags & dbg_flags::dump_with_module_headers)
			flags |= MINIDUMP_TYPE::MiniDumpWithModuleHeaders;
		if (__flags & dbg_flags::dump_filter_triage)
			flags |= MINIDUMP_TYPE::MiniDumpFilterTriage;
		if (__flags & dbg_flags::dump_with_avx_state_context)
			flags |= MINIDUMP_TYPE::MiniDumpWithAvxXStateContext;
		if (__flags & dbg_flags::dump_with_ipt_trace)
			flags |= MINIDUMP_TYPE::MiniDumpWithIptTrace;
		if (__flags & dbg_flags::dump_scan_inaccessible_partial_pages)
			flags |= MINIDUMP_TYPE::MiniDumpScanInaccessiblePartialPages;
		if (__flags & dbg_flags::dump_valid_type_flags)
			flags |= MINIDUMP_TYPE::MiniDumpValidTypeFlags;
		return union_cast<MINIDUMP_TYPE>(flags);
	}
	dbg_u8string __dbg_generate_error_message_win(
		DWORD __error)
	{
		// 1. Buffer
		PWSTR msg			 = nullptr;
		const size_t c = FormatMessageW(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			0, __error, MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), union_cast<PWSTR>(&msg), 0, nullptr);
		if (pf_unlikely(!msg || c == 0))
			pf_throw(
				dbg_category_system(), GetLastError(), dbg_flags::none,
				"[WIN] FormatMessageW failed for code={}!", __error);
		LocalFree(msg);

		// 2. Returns
		return __dbg_convert_wide_to_u8_win(msg, c);
	}
	void __dbg_generate_error_popup_win(
		dbg_u8string_view __title,
		dbg_u8string_view __msg)
	{
		// 1. Convert u8 -> wide
		__dbg_wsstring wstitle = __dbg_convert_u8_to_wide_win(__title.data(), __title.size());
		__dbg_wsstring wsmsg	 = __dbg_convert_u8_to_wide_win(__msg.data(), __msg.size());

		// 2. Display
		if (!MessageBoxW(nullptr, wsmsg.data(), wstitle.data(), MB_OK | MB_ICONERROR))
			pf_throw(
				dbg_category_system(), GetLastError(), dbg_flags::none,
				"[WIN] MessageBoxW failed! title={}, message={}",
				__title.data(), __msg.data());
	}
	dbg_u8string __dbg_generate_dumpbin_win(
		MINIDUMP_EXCEPTION_INFORMATION *__mei,
		MINIDUMP_TYPE __type)
	{
		// 1. Path
		PWSTR known = nullptr;
		BOOL ret		= SHGetKnownFolderPath(FOLDERID_LocalAppDataLow, 0, 0, &known);
		if (ret != S_OK)
		{
			CoTaskMemFree(known);
			pf_throw(
				dbg_category_system(), GetLastError(), dbg_flags::none,
				"[WIN] SHGetKnownFolderPath failed with FOLDERID_LocalAppDataLow!");
		}
		// 2. Directory
		__dbg_wsstring str(DBG_FMT_NAME_LEN, '\0');
		str.insert_back(known);
		str.insert_back(L"\\Pulsar\\");
		CoTaskMemFree(known);
		if(!CreateDirectoryW(str.data(), nullptr))
		{
			DWORD err = GetLastError();
			if (err != ERROR_ALREADY_EXISTS)
			{
				dbg_u8string u8path = __dbg_convert_wide_to_u8_win(str.data(), std::wcslen(str.data()));
				pf_throw(
					dbg_category_system(), GetLastError(), dbg_flags::none,
					"[WIN] CreateDirectoryW failed for path={}", u8path.data());
			}
		}

		// 3. File
		str.insert_back(L"minidump.dmp");
		HANDLE file					= CreateFileW(str.data(), GENERIC_READ | GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
		dbg_u8string u8path = __dbg_convert_wide_to_u8_win(str.data(), std::wcslen(str.data()));
		if (!file)
		{
			pf_throw(
				dbg_category_system(), GetLastError(), dbg_flags::none,
				"[WIN] CreateFileW failed at path={}", u8path.data());
			return dbg_u8string();
		}

		// 4. Generate
		if(!MiniDumpWriteDump(
				 GetCurrentProcess(), GetCurrentProcessId(), file,
				 __type, __mei, nullptr, nullptr))
		{
			DWORD result = HRESULT_FROM_WIN32(GetLastError());
			pf_throw(
				dbg_category_system(), result, dbg_flags::none,
				"[WIN] Failed to generate dumpbin at path={}", u8path.data());
			return dbg_u8string();
		}

		// 5. Close File
		if (!CloseHandle(file))
		{
			pf_throw(
				dbg_category_system(), GetLastError(), dbg_flags::none,
				"[WIN] Failed closing file handle for file={}, at path={}",
				union_cast<void*>(file), u8path.data());
			return dbg_u8string();
		}

		// 6. Info
		pf_print(
			dbg_type::info, dbg_level::high,
			"[WIN] Dumpfile written at path={}.", u8path.data());

		// 7. Returns
		return u8path;
	}
	void __dbg_generate_exception_dumpbin_win(
		dbg_category const *__cat,
		uint32_t __code,
		uint32_t __flags,
		dbg_u8string_view __what)
	{
		// Print
		pf_print(dbg_type::info, dbg_level::high, "[WIN] Catching unhandled exception of category={}, code={}, message={}. Generating dumpbin...",
						 __cat->name().data(), __code, __what.data());
		auto st	 = __dbg_retrieve_stacktrace();
		size_t s = __dbg_formatted_stacktrace_size(st);
		dbg_u8string str(s, '\0');
		__dbg_format_stacktrace_to(str.begin(), st);
		__dbg_print("{}", str.begin());

		// Generating Dumpbin
		MINIDUMP_EXCEPTION_INFORMATION mei;
		memset(&mei, 0, sizeof(mei));
		CONTEXT c;
		memset(&c, 0, sizeof(c));
		c.ContextFlags = CONTEXT_ALL;
		HANDLE thread = OpenThread(THREAD_ALL_ACCESS, FALSE, __internal.dbg_ex_ctx.__get_ID());
		GetThreadContext(thread, &c);
		mei.ThreadId												 = __internal.dbg_ex_ctx.__get_ID();
		mei.ExceptionPointers								 = __internal.dbg_ex_ctx.__get_current_exception_pointer();
		mei.ExceptionPointers->ContextRecord = &c;
		mei.ClientPointers									 = FALSE;
		dbg_u8string u8path = __dbg_generate_dumpbin_win(&mei, __dbg_flags_to_minidump_type_win(__flags));
		// 3. Popup
		__dbg_generate_error_popup_win(
			"Pulsar", dbg_u8format(
				"An exception has been thrown of category={}, code={}, message={} | {}.\n"
				"A dumpfile has been generated at location={}.\n"
				"Press ok to terminate the program.",
				__cat->name().data(), __code, __cat->message(__code).data(), __what.data(), u8path.data()).data());
	}
	void __dbg_terminate_unknown_dumpbin_win() pf_attr_noexcept
	{
		// 1. Print
		pf_print(dbg_type::info, dbg_level::high, "[WIN] Unknown error caught. Generating dumpbin...");

		// 2. Generate dumpbin
		try
		{
			dbg_u8string u8path = __dbg_generate_dumpbin_win(nullptr, MINIDUMP_TYPE::MiniDumpNormal);
			__dbg_generate_error_popup_win(
				"Pulsar", dbg_u8format(
					"An unknown error has been caught.\n"
					"A dumpfile has been generated at location={}.\n"
					"Press ok to terminate the program.",
					u8path.begin()));
		}
		catch(dbg_exception const &__e)
		{
			pf_print(
				dbg_type::warning, dbg_level::high,
				"[WIN] Failed to generate unknown dumpbin! category={}, code={}, message={}",
				__e.category()->name().data(), __e.code(), __e.what());
		}

		// 3. Ends
		pf_print(dbg_type::warning, dbg_level::high, "Aborting...");
		std::abort();
	}
	void __dbg_terminate_exception_dumpbin_win(
		dbg_category const *__cat,
		uint32_t __code,
		uint32_t __flags,
		dbg_u8string_view __what) pf_attr_noexcept
	{
		// Generate
		try
		{
			__dbg_generate_exception_dumpbin_win(__cat, __code, __flags, __what);
		}
		catch (dbg_exception const &__e)
		{
			pf_print(
				dbg_type::warning, dbg_level::high,
				"[WIN] Failed to generate exception dumpbin!");
			pf_print(
				dbg_type::warning, dbg_level::high,
				"[WIN] Aborting with exception category={}, code={}, message={} | {}",
				__e.category()->name().data(),
				__e.code(),
				__e.category()->message(__e.code()).data(),
				__e.what());
		}

		// Aborting
		pf_print(
			dbg_type::warning, dbg_level::high,
			"Aborting...");
		std::abort();
	}
	void __dbg_terminate_win() pf_attr_noexcept
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
			catch (dbg_exception const &__e)
			{
				__dbg_terminate_exception_dumpbin_win(
					__e.category(),
					__e.code(),
					__e.flags(),
					__e.what());
			}
			catch (std::system_error const &__e)
			{
				__dbg_terminate_exception_dumpbin_win(
					dbg_category_system(),
					__e.code().value(),
					dbg_flags::none,
					__e.what());
			}
			catch (std::invalid_argument const &__e)
			{
				__dbg_terminate_exception_dumpbin_win(
					dbg_category_generic(),
					union_cast<uint32_t>(dbg_code::invalid_argument),
					dbg_flags::none,
					__e.what());
			}
			catch (std::domain_error const &__e)
			{
				__dbg_terminate_exception_dumpbin_win(
					dbg_category_generic(),
					union_cast<uint32_t>(dbg_code::domain_error),
					dbg_flags::none,
					__e.what());
			}
			catch (std::length_error const &__e)
			{
				__dbg_terminate_exception_dumpbin_win(
					dbg_category_generic(),
					union_cast<uint32_t>(dbg_code::length_error),
					dbg_flags::none,
					__e.what());
			}
			catch (std::out_of_range const &__e)
			{
				__dbg_terminate_exception_dumpbin_win(
					dbg_category_generic(),
					union_cast<uint32_t>(dbg_code::out_of_range),
					dbg_flags::none,
					__e.what());
			}
			catch (std::logic_error const &__e)
			{
				__dbg_terminate_exception_dumpbin_win(
					dbg_category_generic(),
					union_cast<uint32_t>(dbg_code::logic_error),
					dbg_flags::none,
					__e.what());
			}
			catch (std::range_error const &__e)
			{
				__dbg_terminate_exception_dumpbin_win(
					dbg_category_generic(),
					union_cast<uint32_t>(dbg_code::range_error),
					dbg_flags::none,
					__e.what());
			}
			catch (std::overflow_error const &__e)
			{
				__dbg_terminate_exception_dumpbin_win(
					dbg_category_generic(),
					union_cast<uint32_t>(dbg_code::overflow_error),
					dbg_flags::none,
					__e.what());
			}
			catch (std::underflow_error const &__e)
			{
				__dbg_terminate_exception_dumpbin_win(
					dbg_category_generic(),
					union_cast<uint32_t>(dbg_code::underflow_error),
					dbg_flags::none,
					__e.what());
			}
			catch (std::runtime_error const &__e)
			{
				__dbg_terminate_exception_dumpbin_win(
					dbg_category_generic(),
					union_cast<uint32_t>(dbg_code::runtime_error),
					dbg_flags::none,
					__e.what());
			}
			catch (std::bad_alloc const &__e)
			{
				__dbg_terminate_exception_dumpbin_win(
					dbg_category_generic(),
					union_cast<uint32_t>(dbg_code::bad_alloc),
					dbg_flags::none,
					__e.what());
			}
			catch (std::exception const &__e)
			{
				__dbg_terminate_exception_dumpbin_win(
					dbg_category_generic(),
					union_cast<uint32_t>(dbg_code::unknown),
					dbg_flags::none,
					__e.what());
			}
		}
		else
		{
			__dbg_terminate_unknown_dumpbin_win();
		}
		#else // ^^^ PF_RELEASE ^^^ / vvv PF_DEBUG vvv
		/// Exit
		std::abort();
		#endif
	}
}

#endif // !PF_WINDOWSs