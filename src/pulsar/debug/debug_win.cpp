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
	pf_hint_nodiscard __dbg_stacktrace_formatted_t
	__dbg_capture_stacktrace_win(
	 CONTEXT *__ctx,
	 uint32_t __ignore) pf_attr_noexcept
	{
		// Symbol Init
		HANDLE thdl = GetCurrentThread();
		HANDLE phdl = GetCurrentProcess();
		SymSetOptions(SYMOPT_DEFERRED_LOADS);
		if(!SymInitializeW(phdl, nullptr, TRUE))
		{
			pf_print(dbg_type::warning, dbg_level::high, "[WIN] SymInitialize failed! process={}", phdl);
			return { 0, '\0' };
		}

		// StackFrame
		// per platform stack frame initialisation
		STACKFRAME64 sf;
		memset(&sf, 0, sizeof(sf));
		// https://docs.microsoft.com/en-us/windows/win32/api/dbghelp/ns-dbghelp-stackframe
		// x86 platforms
 #if defined(PF_ARCHITECTURE_I386)
		DWORD machineType		= IMAGE_FILE_MACHINE_I386;
		sf.AddrPC.Offset		= __ctx->Eip;
		sf.AddrPC.Mode			= AddrModeFlat;
		sf.AddrFrame.Offset = __ctx->Ebp;
		sf.AddrFrame.Mode		= AddrModeFlat;
		sf.AddrStack.Offset = __ctx->Esp;
		sf.AddrStack.Mode		= AddrModeFlat;
			// intel itanium
 #elif defined(PF_ARCHITECTURE_IA64)
		DWORD machineType		 = IMAGE_FILE_MACHINE_IA64;
		sf.AddrPC.Offset		 = __ctx->StIIP;
		sf.AddrPC.Mode			 = AddrModeFlat;
		sf.AddrFrame.Offset	 = __ctx->IntSp;
		sf.AddrFrame.Mode		 = AddrModeFlat;
		sf.AddrBStore.Offset = __ctx->RsBSP;
		sf.AddrBStore.Mode	 = AddrModeFlat;
		sf.AddrStack.Offset	 = __ctx->IntSp;
		sf.AddrStack.Mode		 = AddrModeFlat;
			 // x64 platforms
 #elif defined(PF_ARCHITECTURE_AMD64)
		DWORD machineType		= IMAGE_FILE_MACHINE_AMD64;
		sf.AddrPC.Offset		= __ctx->Rip;
		sf.AddrPC.Mode			= AddrModeFlat;
		sf.AddrFrame.Offset = __ctx->Rsp;
		sf.AddrFrame.Mode		= AddrModeFlat;
		sf.AddrStack.Offset = __ctx->Rsp;
		sf.AddrStack.Mode		= AddrModeFlat;
 #else
	#error "Platform unsupported!"
 #endif
		// symbol handling
		union
		{
			IMAGEHLP_SYMBOL64 as_symbol;
			BYTE as_buffer[sizeof(as_symbol) + DBG_FMT_NAME_LEN * sizeof(CHAR)];
		};
		memset(union_cast<byte_t *>(&as_buffer), 0, sizeof(as_buffer));
		as_symbol.SizeOfStruct	= sizeof(as_symbol);
		as_symbol.MaxNameLength = DBG_FMT_NAME_LEN;
		// module of the symbol
		IMAGEHLP_MODULE64 m;
		memset(&m, 0, sizeof(m));
		m.SizeOfStruct = sizeof(m);
		// number of trace
		uint32_t cn		 = DBG_FMT_STACK_FRAMES - __ignore;

		// "Walk" the stacktrace
		__dbg_stacktrace_formatted_t st = { 0, '\0' };
		auto *w													= &st.message[0];
		while(
		 StackWalk64(
			machineType, phdl, thdl, &sf, __ctx, nullptr, SymFunctionTableAccess64, SymGetModuleBase64, nullptr)
		 && cn > 0)
		{
			// Retrieve
			// ignore
			if(__ignore > 0)
			{
				--__ignore;
			}
			else
			{
				// format
				// line, file of the symbol
				IMAGEHLP_LINE64 l;
				memset(&l, 0, sizeof(l));
				l.SizeOfStruct = sizeof(l);
				DWORD64 sd		 = 0;
				char_t name[DBG_FMT_NAME_LEN];
				SymGetSymFromAddr64(phdl, sf.AddrPC.Offset, &sd, &as_symbol);
				UnDecorateSymbolName(&as_symbol.Name[0], &name[0], DBG_FMT_NAME_LEN, UNDNAME_COMPLETE);
				SymGetModuleInfo64(phdl, sf.AddrPC.Offset, &m);
				DWORD ld = 0;
				SymGetLineFromAddr64(phdl, sf.AddrPC.Offset, &ld, &l);
				const size_t s = fmt::formatted_size(
													"\nat function={}, in module={} at file={}:{}\n",
													&as_symbol.Name[0],
													&m.ModuleName[0],
													&l.FileName[0] ? &l.FileName[0] : "???",
													l.LineNumber)
											 + 11;
				if(w + s > &st.message[0] + DBG_FMT_STACKTRACE_MAX_LENGTH)
				{
					w = fmt::format_to(w, "\nat ...\n");
					break;
				}
				else
				{
					w = fmt::format_to(
					 w, "\tat function={}, in module={} at file={}:{}\n", &as_symbol.Name[0], &m.ModuleName[0], &l.FileName[0] ? &l.FileName[0] : "???", l.LineNumber);
				}
				--cn;
			}
		}

		// Clean
		SymCleanup(phdl);

		// Return
		st.length = distof(&st.message[0], w);
		return st;
	}
	pf_hint_nodiscard __dbg_stacktrace_formatted_t
	__dbg_retrieve_exception_stacktrace_win() pf_attr_noexcept
	{
		return __dbg_capture_stacktrace_win(__internal.dbg_internal.__retrieve_current_context()->exp->ContextRecord, 2);
	}
	pf_hint_nodiscard pulsar_api __dbg_stacktrace_formatted_t
	__dbg_retrieve_stacktrace() pf_attr_noexcept
	{
		CONTEXT ctx;
		memset(&ctx, 0, sizeof(ctx));
		RtlCaptureContext(&ctx);
		return __dbg_capture_stacktrace_win(&ctx, 2);
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
		auto *b = __internal.dbg_internal.__retrieve_current_context();
		auto st = __dbg_retrieve_exception_stacktrace_win();

		// Size
		const size_t s = DBG_FMT_WRITE_OFFSET
									 + fmt::formatted_size(" T={} /{}/ /{}/ ({}) message={} | ", b->ID, dbg_styled('E', dbg_style_fg(dbg_color::red)), dbg_styled(__cat->name().data(), dbg_style_fg(dbg_color::orange)), dbg_styled(__code, dbg_style_fg(dbg_color::red)), dbg_styled(__cat->message(__code).data(), dbg_style_fg(dbg_color::orange)))
									 + st.length + fmt::formatted_size("{}\n", __msg.begin()) + 2;	// 2 == '\0' + '\n'

		// Format
		dbg_u8string str(s, '\0');
		char_t *p = str.begin();
		p					= __dbg_format_chrono_to(p);
		p					= dbg_u8format_to(p, " T={} /{}/ /{}/ ({}) message={} | ", b->ID, dbg_styled('E', dbg_style_fg(dbg_color::red)), dbg_styled(__cat->name().data(), dbg_style_fg(dbg_color::orange)), dbg_styled(__code, dbg_style_fg(dbg_color::red)), dbg_styled(__cat->message(__code).data(), dbg_style_fg(dbg_color::orange)));
		p					= dbg_u8format_to(p, "{}\n", __msg.begin());
		p					= __dbg_append_stacktrace_to(p, st);
		*(p++)		= '\n';

		// Print
		dbg_log(std::move(str));
	}

	/// DEBUG: Win -> Exception Handler
	/// Handle
	LONG WINAPI
	__dbg_internal_t::__vectored_exception_handler(
	 EXCEPTION_POINTERS *__info) pf_attr_noexcept
	{
		const thread_id_t ID = this_thread::get_idx();
		auto *b							 = __internal.dbg_internal.__retrieve_current_context();
		if(!b->exp || !b->exp->ExceptionRecord || !b->exp->ContextRecord || __info->ExceptionRecord->ExceptionAddress != b->exp->ExceptionRecord->ExceptionAddress)
		{
			// Exception -> Ex
			b->exp = __info;
			b->ID	 = ID;
		}
		return EXCEPTION_CONTINUE_SEARCH;
	}

	/// Constructors
	__dbg_internal_t::__dbg_internal_t() pf_attr_noexcept
		: buffer_(new_construct<__dbg_record_win_t[]>(CCY_NUM_THREADS))
	{
		this->handle_ = AddVectoredExceptionHandler(0, __vectored_exception_handler);
		pf_assert(this->handle_, "[WIN] AddVectoredExceptionHandler for printing stacktrace failed! handle={}", this->handle_);
		std::set_terminate(__dbg_terminate_win);
	}

	/// Destructor
	__dbg_internal_t::~__dbg_internal_t() pf_attr_noexcept
	{
		destroy_delete<__dbg_record_win_t[]>(this->buffer_);
		if(this->handle_ && !RemoveVectoredExceptionHandler(this->handle_))
		{
			pf_print(dbg_type::warning, dbg_level::low, "[WIN] RemoveVectoredExceptionHandler failed for handle={}!", this->handle_);
		}
	}

	// Retrieve
	__dbg_record_win_t *
	__dbg_internal_t::__retrieve_current_context() pf_attr_noexcept
	{
		return &this->buffer_[this_thread::get_idx()];
	}

	// Terminate
	void
	__dbg_internal_t::__set_terminate(
	 dbg_terminate_handle_t __handle) pf_attr_noexcept
	{
		this->terminate_ = __handle;
	}
	dbg_terminate_handle_t
	__dbg_internal_t::__get_terminate() const pf_attr_noexcept
	{
		return this->terminate_;
	}
	void
	__dbg_internal_t::__call_terminate() const pf_attr_noexcept
	{
		if(this->terminate_) this->terminate_();
	}

	/// DEBUG: Win -> Convert
	__dbg_wsstring
	__dbg_convert_u8_to_wide_win(
	 const char_t *__buf,
	 size_t __size)
	{
		// Buffer
		if(!__buf || !__size) return __dbg_wsstring();
		const size_t r = utf8_to_utf16le::required_count(__buf, __size);
		__dbg_wsstring str(r, L'\0');
		const auto res = utf8_to_utf16le::convert(__buf, __size, union_cast<u16char_t *>(&str[0]));
		pf_throw_if(res.code != char_error_code::success, dbg_category_char(), res.code, 0, "[WIN] utf8 to utf16 transcoding failed!");

		// Returns
		return str;
	}
	dbg_u8string
	__dbg_convert_wide_to_u8_win(
	 const wchar_t *__buf,
	 size_t __count)
	{
		// Buffer
		if(!__buf || !__count) return dbg_u8string();
		const size_t r = utf16le_to_utf8::required_count(union_cast<const u16char_t *>(__buf), __count);
		dbg_u8string str(r, '\0');
		const auto res = utf16le_to_utf8::convert(union_cast<const u16char_t *>(__buf), __count, &str[0]);
		pf_throw_if(res.code != char_error_code::success, dbg_category_char(), res.code, 0, "[WIN] utf16le to utf8 transcoding failed!");

		// Returns
		return str;
	}

	/// DEBUG: Win -> Generate
	MINIDUMP_TYPE
	__dbg_flags_to_minidump_type_win(
	 uint32_t __flags) pf_attr_noexcept
	{
		DWORD flags = MINIDUMP_TYPE::MiniDumpNormal;
		if(__flags & dbg_flags::dump_with_data_segs)
			flags |= MINIDUMP_TYPE::MiniDumpWithDataSegs;
		if(__flags & dbg_flags::dump_with_full_memory)
			flags |= MINIDUMP_TYPE::MiniDumpWithFullMemory;
		if(__flags & dbg_flags::dump_with_handle_data)
			flags |= MINIDUMP_TYPE::MiniDumpWithHandleData;
		if(__flags & dbg_flags::dump_filter_memory)
			flags |= MINIDUMP_TYPE::MiniDumpFilterMemory;
		if(__flags & dbg_flags::dump_with_scan_memory)
			flags |= MINIDUMP_TYPE::MiniDumpScanMemory;
		if(__flags & dbg_flags::dump_with_unloaded_modules)
			flags |= MINIDUMP_TYPE::MiniDumpWithUnloadedModules;
		if(__flags & dbg_flags::dump_with_indirectly_referenced_memory)
			flags |= MINIDUMP_TYPE::MiniDumpWithIndirectlyReferencedMemory;
		if(__flags & dbg_flags::dump_with_filter_module_paths)
			flags |= MINIDUMP_TYPE::MiniDumpFilterModulePaths;
		if(__flags & dbg_flags::dump_with_process_thread_data)
			flags |= MINIDUMP_TYPE::MiniDumpWithProcessThreadData;
		if(__flags & dbg_flags::dump_with_private_read_write_memory)
			flags |= MINIDUMP_TYPE::MiniDumpWithPrivateReadWriteMemory;
		if(__flags & dbg_flags::dump_without_auxiliary_state)
			flags |= MINIDUMP_TYPE::MiniDumpWithoutAuxiliaryState;
		if(__flags & dbg_flags::dump_with_full_auxiliary_state)
			flags |= MINIDUMP_TYPE::MiniDumpWithFullAuxiliaryState;
		if(__flags & dbg_flags::dump_with_private_read_write_memory)
			flags |= MINIDUMP_TYPE::MiniDumpWithPrivateReadWriteMemory;
		if(__flags & dbg_flags::dump_ignore_inaccessible_memory)
			flags |= MINIDUMP_TYPE::MiniDumpIgnoreInaccessibleMemory;
		if(__flags & dbg_flags::dump_with_token_information)
			flags |= MINIDUMP_TYPE::MiniDumpWithTokenInformation;
		if(__flags & dbg_flags::dump_with_module_headers)
			flags |= MINIDUMP_TYPE::MiniDumpWithModuleHeaders;
		if(__flags & dbg_flags::dump_filter_triage)
			flags |= MINIDUMP_TYPE::MiniDumpFilterTriage;
		if(__flags & dbg_flags::dump_with_avx_state_context)
			flags |= MINIDUMP_TYPE::MiniDumpWithAvxXStateContext;
		if(__flags & dbg_flags::dump_with_ipt_trace)
			flags |= MINIDUMP_TYPE::MiniDumpWithIptTrace;
		if(__flags & dbg_flags::dump_scan_inaccessible_partial_pages)
			flags |= MINIDUMP_TYPE::MiniDumpScanInaccessiblePartialPages;
		if(__flags & dbg_flags::dump_valid_type_flags)
			flags |= MINIDUMP_TYPE::MiniDumpValidTypeFlags;
		return union_cast<MINIDUMP_TYPE>(flags);
	}
	dbg_u8string
	__dbg_generate_error_message_win(
	 DWORD __error)
	{
		// Buffer
		PWSTR msg			 = nullptr;
		const size_t c = FormatMessageW(
		 FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		 0,
		 __error,
		 MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),
		 union_cast<PWSTR>(&msg),
		 0,
		 nullptr);
		if(pf_unlikely(!msg || c == 0))
			pf_throw(
			 dbg_category_system(), GetLastError(), dbg_flags::none, "[WIN] FormatMessageW failed for code={}!", __error);
		LocalFree(msg);

		// Returns
		return __dbg_convert_wide_to_u8_win(msg, c);
	}
	void
	__dbg_generate_error_popup_win(
	 dbg_u8string_view __title,
	 dbg_u8string_view __msg)
	{
		// Convert u8 -> wide
		__dbg_wsstring wstitle = __dbg_convert_u8_to_wide_win(__title.data(), __title.size());
		__dbg_wsstring wsmsg	 = __dbg_convert_u8_to_wide_win(__msg.data(), __msg.size());

		// Display
		if(!MessageBoxW(nullptr, wsmsg.data(), wstitle.data(), MB_OK | MB_ICONERROR))
			pf_throw(
			 dbg_category_system(), GetLastError(), dbg_flags::none, "[WIN] MessageBoxW failed! title={}, message={}", __title.data(), __msg.data());
	}
	dbg_u8string
	__dbg_generate_dumpbin_win(
	 MINIDUMP_EXCEPTION_INFORMATION *__mei,
	 MINIDUMP_TYPE __type)
	{
		// Path
		PWSTR known = nullptr;
		BOOL ret		= SHGetKnownFolderPath(FOLDERID_LocalAppDataLow, 0, 0, &known);
		if(ret != S_OK)
		{
			CoTaskMemFree(known);
			pf_throw(
			 dbg_category_system(), GetLastError(), dbg_flags::none, "[WIN] SHGetKnownFolderPath failed with FOLDERID_LocalAppDataLow!");
		}
		// Directory
		__dbg_wsstring str(DBG_FMT_NAME_LEN, '\0');
		str.insert_back(known);
		str.insert_back(L"\\Pulsar\\");
		CoTaskMemFree(known);
		if(!CreateDirectoryW(str.data(), nullptr))
		{
			DWORD err = GetLastError();
			if(err != ERROR_ALREADY_EXISTS)
			{
				dbg_u8string u8path = __dbg_convert_wide_to_u8_win(str.data(), std::wcslen(str.data()));
				pf_throw(
				 dbg_category_system(), GetLastError(), dbg_flags::none, "[WIN] CreateDirectoryW failed for path={}", u8path.data());
			}
		}

		// File
		str.insert_back(L"minidump.dmp");
		HANDLE file					= CreateFileW(str.data(), GENERIC_READ | GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
		dbg_u8string u8path = __dbg_convert_wide_to_u8_win(str.data(), std::wcslen(str.data()));
		if(!file)
		{
			pf_throw(
			 dbg_category_system(), GetLastError(), dbg_flags::none, "[WIN] CreateFileW failed at path={}", u8path.data());
		}

		// Generate
		if(!MiniDumpWriteDump(
				GetCurrentProcess(), GetCurrentProcessId(), file, __type, __mei, nullptr, nullptr))
		{
			DWORD result = HRESULT_FROM_WIN32(GetLastError());
			pf_throw(
			 dbg_category_system(), result, dbg_flags::none, "[WIN] Failed to generate dumpbin at path={}", u8path.data());
		}

		// Close File
		if(!CloseHandle(file))
		{
			pf_throw(
			 dbg_category_system(), GetLastError(), dbg_flags::none, "[WIN] Failed closing file handle for file={}, at path={}", union_cast<void *>(file), u8path.data());
		}

		// Print
		pf_print(
		 dbg_type::info, dbg_level::high, "[WIN] Dumpfile written at path={}.", u8path.data());

		// Returns
		return u8path;
	}
	void
	__dbg_generate_exception_dumpbin_win(
	 dbg_category const *__cat,
	 uint32_t __code,
	 uint32_t __flags,
	 dbg_u8string_view __what)
	{
		// Print
		pf_print(dbg_type::info, dbg_level::high, "[WIN] Caught unhandled exception of category={}, code={}, message={}", __cat->name().data(), __code, __what.data());
		auto st	 = __dbg_retrieve_stacktrace();
		size_t s = st.length;
		dbg_u8string str(s, '\0');
		__dbg_append_stacktrace_to(str.begin(), st);
		dbg_log(std::move(str));
		pf_print(dbg_type::info, dbg_level::high, "[WIN] Generating dumpbin...");

		// Generating Dumpbin
		MINIDUMP_EXCEPTION_INFORMATION mei;
		memset(&mei, 0, sizeof(mei));
		CONTEXT c;
		memset(&c, 0, sizeof(c));
		c.ContextFlags				= CONTEXT_ALL;
		const auto *b					= __internal.dbg_internal.__retrieve_current_context();
		mei.ThreadId					= b->ID + 1;
		mei.ExceptionPointers = b->exp;
		mei.ClientPointers		= FALSE;
		dbg_u8string u8path		= __dbg_generate_dumpbin_win(&mei, __dbg_flags_to_minidump_type_win(__flags));

		// Popup
		__dbg_generate_error_popup_win(
		 "Pulsar", dbg_u8format("An exception has been thrown of category={}, code={}, message={} | {}.\n"
														"A dumpfile has been generated at location={}.\n"
														"Press ok to terminate the program.",
														__cat->name().data(),
														__code,
														__cat->message(__code).data(),
														__what.data(),
														u8path.data())
								.data());
	}
	void
	__dbg_terminate_exception_dumpbin_win(
	 dbg_category const *__cat,
	 uint32_t __code,
	 uint32_t __flags,
	 dbg_u8string_view __what) pf_attr_noexcept
	{
		// Generate
		try
		{
			__dbg_generate_exception_dumpbin_win(__cat, __code, __flags, __what);
		} catch(dbg_exception const &__e)
		{
			pf_print(
			 dbg_type::warning, dbg_level::high, "[WIN] Failed to generate exception dumpbin! Reason: category={}, code={}, message={} | {}", __e.category()->name().data(), __e.code(), __e.category()->message(__e.code()).data(), __e.what());
		}
	}
	void
	__dbg_terminate_win() pf_attr_noexcept
	{
		/// Exception?
		std::exception_ptr p = std::current_exception();
		if(p)
		{
			try
			{
				std::rethrow_exception(p);
			} catch(dbg_exception const &__e)
			{
				__dbg_terminate_exception_dumpbin_win(
				 __e.category(),
				 __e.code(),
				 __e.flags(),
				 __e.what());
			} catch(std::exception const &__e)
			{
				__dbg_terminate_exception_dumpbin_win(
				 dbg_category_generic(),
				 union_cast<uint32_t>(dbg_code::unknown),
				 dbg_flags::none,
				 __e.what());
			}
		}

		/// Hook
		__internal.dbg_internal.__call_terminate();

		/// Abort
		process_tasks();	// Write down messages
		std::abort();
	}

	/// DEBUG: Exception -> Record
	pf_hint_noreturn void
	__dbg_exception_record_switch_task_win(
	 std::exception_ptr &&__ptr,
	 void *__record,
	 atomic<bool> *__ctrl)
	{
		// Update local thread EXCEPTION_POINTERS + ID of thread thrower
		__dbg_set_current_exception_record(__record);

		// Rethrow exception. Since it's thrown inside a try - catch block, it'll
		// be destroyed only if it exit it, unblocking the throwing thread.
		try
		{
			throw(__dbg_exception_record_switcher_t(std::move(__ptr), __ctrl));
		} catch(__dbg_exception_record_switcher_t const &__e)
		{
			__ptr = nullptr;
			__e.__rethrow();	// NOTE: Lifetime => Deleted when exit a catch block
		}
	}
	void
	__dbg_exception_record_t::rethrow()
	{
		if(this->ptr_)
		{
			auto p		 = std::move(this->ptr_);
			this->ptr_ = nullptr;
			__dbg_exception_record_switch_task_win(std::move(p), this->record_, this->ctrl_);
		}
	}

	/// DEBUG: Record
	void *
	__dbg_get_current_exception_record() pf_attr_noexcept
	{
		auto *ctx = __internal.dbg_internal.__retrieve_current_context();
		return ctx;
	}
	void
	__dbg_set_current_exception_record(
	 void *__record) pf_attr_noexcept
	{
		__dbg_record_win_t *r	 = __internal.dbg_internal.__retrieve_current_context();
		__dbg_record_win_t *in = union_cast<__dbg_record_win_t *>(__record);
		r->exp								 = in->exp;
		r->ID									 = in->ID;
	}

	/// DEBUG: Record -> Switcher
	pulsar_api void
	__dbg_move_exception_record_to_0() pf_attr_noexcept
	{
		pf_assert(
		 this_thread::get_idx() != 0,
		 "[WIN] Can't move exception record from main to main thread!");
		pf_alignas(CCY_ALIGN) atomic<bool> ctrl = false;
		submit_task_0(
		 __dbg_exception_record_switch_task_win,
		 std::current_exception(),
		 __dbg_get_current_exception_record(),
		 &ctrl);
		while(!ctrl.load(atomic_order::relaxed)) this_thread::yield();
	}
	// TODO: Maybe we'll need different way of getting exception record in the future!
}	 // namespace pul

#endif	// !PF_WINDOWS
