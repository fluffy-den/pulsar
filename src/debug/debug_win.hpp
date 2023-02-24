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
#ifdef PF_OS_WINDOWS
#include <dbghelp.h>
#include <shlobj.h>

// Include: C++
#include <exception>
#include <system_error>
#include <future>

// Pulsar
namespace pul
{
	/// DEBUG: Wide -> Types
	// String View
	class __dbg_wsstring_view
	{
	public:
		/// Constructors
		pf_decl_inline __dbg_wsstring_view() pf_attr_noexcept
			: store_(nullptr)
			, count_(0)
		{}
		pf_decl_inline __dbg_wsstring_view(
			const wchar_t *__str,
			size_t __count) pf_attr_noexcept
			: store_(__str)
			, count_(__count)
		{}
		pf_decl_inline __dbg_wsstring_view(
			const wchar_t *__str) pf_attr_noexcept
			: store_(__str)
			, count_(std::wcslen(__str))
		{}
		__dbg_wsstring_view(
			__dbg_wsstring_view const&) = default;
		__dbg_wsstring_view(
			__dbg_wsstring_view &&) = default;

		/// Operator =
		__dbg_wsstring_view &operator=(
			__dbg_wsstring_view const &) = default;
		__dbg_wsstring_view &operator=(
			__dbg_wsstring_view &&) = default;

		/// Operator []
		pf_hint_nodiscard pf_decl_inline const wchar_t &
		operator[](
			size_t __index) const pf_attr_noexcept
		{
			return this->store_[__index];
		}

		/// Begin
		pf_hint_nodiscard pf_decl_inline const wchar_t*
		begin() const pf_attr_noexcept
		{
			return &this->store_[0];
		}
		pf_hint_nodiscard pf_decl_inline const wchar_t*
		cbegin() const pf_attr_noexcept
		{
			return &this->store_[0];
		}

		/// End
		pf_hint_nodiscard pf_decl_inline const wchar_t*
		end() const pf_attr_noexcept
		{
			return &this->store_[0] + this->count_;
		}
		pf_hint_nodiscard pf_decl_inline const wchar_t*
		cend() const pf_attr_noexcept
		{
			return &this->store_[0] + this->count_;
		}

		/// Data
		pf_hint_nodiscard pf_decl_inline const wchar_t*
		data() const pf_attr_noexcept
		{
			return &this->store_[0];
		}

		/// Size
		pf_hint_nodiscard pf_decl_inline size_t
		size() const pf_attr_noexcept
		{
			return this->count_ * sizeof(wchar_t);
		}

		/// Count
		pf_hint_nodiscard pf_decl_inline size_t
		count() const pf_attr_noexcept
		{
			return this->count_;
		}

	private:
		const wchar_t *store_;
		size_t count_;
	};

	// String
	class __dbg_wsstring
	{
	/// __assign_view
	void __assign_view(
		__dbg_wsstring_view __v) pf_attr_noexcept
	{
		this->shrink(__v.size());
		std::memcpy(this->data(), __v.data(), this->count_);
	}

	public:
		/// Constructors
		pf_decl_inline __dbg_wsstring() pf_attr_noexcept
			: store_(nullptr)
			, count_(0)
		{}
		pf_decl_inline __dbg_wsstring(
			nullptr_t) pf_attr_noexcept
			: __dbg_wsstring()
		{}
		pf_decl_inline __dbg_wsstring(
			size_t __count,
			char_t __val) pf_attr_noexcept
			: store_(union_cast<wchar_t*>(allocate(__count, align_val_t(32), 0)))
			, count_(__count)
		{
			std::memset(this->store_, __val, __count);
		}
		__dbg_wsstring(
			__dbg_wsstring &&) = default;
		__dbg_wsstring(
			__dbg_wsstring const &) = default;
		pf_decl_inline __dbg_wsstring(
			__dbg_wsstring_view __v) pf_attr_noexcept
			: store_(union_cast<wchar_t*>(allocate(__v.count(), align_val_t(32), 0)))
			, count_(__v.count())
		{
			this->__assign_view(__v);
		}

		/// Destructor
		pf_decl_inline ~__dbg_wsstring() pf_attr_noexcept
		{
			deallocate(this->store_);
		}

		/// Operator =
		pf_decl_inline __dbg_wsstring &operator=(
			__dbg_wsstring const &) = default;
		pf_decl_inline __dbg_wsstring &operator=(
			__dbg_wsstring &&) = default;
		pf_decl_inline __dbg_wsstring &operator=(
			__dbg_wsstring_view __v) pf_attr_noexcept
		{
			this->count_ = 0;
			this->__assign_view(__v);
			return *this;
		}

		/// Operator[]
		pf_hint_nodiscard pf_decl_inline wchar_t &
		operator[](
			size_t __index) pf_attr_noexcept
		{
			return this->store_[__index];
		}

		/// View
		pf_hint_nodiscard pf_decl_inline __dbg_wsstring_view
		view() const pf_attr_noexcept
		{
			return __dbg_wsstring_view(&this->store_[0], this->count_);
		}

		/// Shrink
		pf_decl_inline void shrink(
			size_t __c) pf_attr_noexcept
		{
			this->store_ = union_cast<wchar_t*>(reallocate(this->store_, this->count_, __c, align_val_t(32), 0));
		}

		/// Begin/End
		pf_hint_nodiscard pf_decl_inline wchar_t*
		begin() pf_attr_noexcept
		{
			return &this->store_[0];
		}
		pf_hint_nodiscard pf_decl_inline const wchar_t*
		begin() const pf_attr_noexcept
		{
			return &this->store_[0];
		}
		pf_hint_nodiscard pf_decl_inline wchar_t*
		end() pf_attr_noexcept
		{
			return &this->store_[0] + this->count_;
		}
		pf_hint_nodiscard pf_decl_inline const wchar_t*
		end() const pf_attr_noexcept
		{
			return &this->store_[0] + this->count_;
		}

		/// Data
		pf_hint_nodiscard pf_decl_inline wchar_t*
		data() pf_attr_noexcept
		{
			return &this->store_[0];
		}
		pf_hint_nodiscard pf_decl_inline const wchar_t*
		data() const pf_attr_noexcept
		{
			return &this->store_[0];
		}

		/// Size
		pf_hint_nodiscard pf_decl_inline size_t
		size() const pf_attr_noexcept
		{
			return this->count_ * sizeof(wchar_t);
		}
		pf_hint_nodiscard pf_decl_inline size_t
		count() const pf_attr_noexcept
		{
			return this->count_;
		}

		/// Operator (View)
		pf_hint_nodiscard pf_decl_inline
		operator __dbg_wsstring_view() const pf_attr_noexcept
		{
			return this->view();
		}

	private:
		wchar_t *store_;
		size_t count_;
	};

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
			exThreadID_ = GetCurrentThreadId();
			ex_					= __info;
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
			return exThreadID_;
		}
		pf_hint_nodiscard pf_decl_static pf_decl_inline
		PEXCEPTION_POINTERS current_exception() pf_attr_noexcept
		{
			return ex_;
		}

	private:
		// SEH -> Last Local Exception
		pf_decl_static pf_decl_inline PEXCEPTION_POINTERS ex_;
		pf_decl_static pf_decl_inline DWORD exThreadID_;
		pf_decl_static pf_decl_inline PVOID sehex_;
	};


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
			std::memset(&name[0], 0, sizeof(name));
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
			std::memset(&name[0], 0, sizeof(name));
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
	/// DEBUG: Win -> Format
	pf_hint_nodiscard pf_decl_static char_t*
	__dbg_format_stacktrace_of_exception_to_win(
		char_t *__where,
		size_t __ignore)
	{
		// 1. Call
		PEXCEPTION_POINTERS p = __dbg_exception_handler_win::current_exception();
		return __dbg_format_walk_to_win(__where, p->ContextRecord, __ignore);
	}


	/// DEBUG: Message
	pf_hint_nodiscard pf_decl_static __dbg_wsstring __dbg_convert_u8_to_wide_win(
		const char_t *__buf,
		size_t __size) pf_attr_noexcept
	{
		// 1. Buffer
		const size_t r = MultiByteToWideChar(CP_UTF8, WC_ERR_INVALID_CHARS, __buf, __size, nullptr, 0);
		__dbg_wsstring str(r, '\0');
		DWORD err = GetLastError();
		if (pf_unlikely(!MultiByteToWideChar(CP_UTF8, WC_ERR_INVALID_CHARS, __buf, __size, str.begin(), str.size())))
			pf_throw(dbg_category_system(), err, dbg_flags::none, "[WIN] MultiByteToWideChar failed!");

		// 2. Returns
		return str;
	}
	pf_hint_nodiscard pf_decl_static dbg_u8string __dbg_convert_wide_to_u8_win(
		const wchar_t *__buf,
		size_t __size) pf_attr_noexcept
	{
		// 1. Buffer
		const size_t r = WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, __buf, __size, nullptr, 0, 0, 0);
		dbg_u8string str(r, '\0');
		DWORD err = GetLastError();
		if (pf_unlikely(!WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, __buf, __size, str.begin(), str.size(), 0, 0)))
			pf_throw(dbg_category_system(), err, dbg_flags::none, "[WIN] WideCharToMultiByte failed!");

		// 2. Returns
		return str;
	}
	pf_hint_nodiscard pf_decl_static dbg_u8string __dbg_generate_error_message_win(
		DWORD __error) pf_attr_noexcept
	{
		// 1. Buffer
		PWSTR msg			 = nullptr;
		const size_t c = FormatMessageW(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			0, __error, MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), msg, 0, nullptr);
		if (pf_unlikely(!msg || c == 0))
			pf_throw(dbg_category_system(), __error, dbg_flags::none, "[WIN] FormatMessageW failed!");
		LocalFree(msg);

		// 2. Returns
		return __dbg_convert_wide_to_u8_win(msg, c);
	}

	/// DEBUG: MiniDump
	pf_decl_static MINIDUMP_TYPE __dbg_flags_to_minidump_type_win(
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
	pf_decl_static void __dbg_generate_error_popup_win(
		dbg_u8string_view __title,
		dbg_u8string_view __msg) pf_attr_noexcept
	{
		// 1. Display
		__dbg_wsstring wstitle = __dbg_convert_u8_to_wide_win(__title.begin(), __title.size());
		__dbg_wsstring wsmsg	 = __dbg_convert_u8_to_wide_win(__msg.begin(), __msg.size());
		if (!MessageBoxExW(nullptr, wstitle.begin(), wsmsg.begin(), MB_OK | MB_ICONERROR, MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US)))
			pf_print(dbg_type::warning, dbg_level::high, "[WIN] Failed to generate error popup!");
	}
	pf_decl_static dbg_u8string __dbg_generate_dumpbin(
		MINIDUMP_EXCEPTION_INFORMATION *__mei,
		MINIDUMP_TYPE __type)
	{
		// 1. Path
		PWSTR *known = nullptr;
		BOOL ret		 = SHGetKnownFolderPath(FOLDERID_LocalAppDataLow, 0, 0, known);
		if (!ret)
		{
			CoTaskMemFree(known);
			pf_throw(dbg_category_system(), GetLastError(), dbg_flags::none, "[WIN] SHGetKnownFolderPath failed to find path=\"*user*/appdata/locallow/\"!");
		}
		WCHAR buffer[DBG_FMT_NAME_LEN];
		WCHAR *p = &buffer[0];
		size_t s = std::wcslen(*known) * sizeof(wchar_t);
		std::memcpy(p, known, s);
		CoTaskMemFree(known);
		p += s;
		WCHAR wpath[] = L"\\pulsar\\crash.dmp\0";
		std::memcpy(p, &wpath[0], sizeof(wpath));
		dbg_u8string u8path = __dbg_convert_wide_to_u8_win(&buffer[0], std::wcslen(&buffer[0]));

		// 2. File
		HANDLE file = CreateFileW(&buffer[0], GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
		if (!file)
		{
			pf_throw(dbg_category_system(), GetLastError(), 0, "[WIN] CreateFileW failed at path={}", u8path.data());
		}

		// 3. Generate
		if(!MiniDumpWriteDump(
				 GetCurrentProcess(), GetCurrentProcessId(), file,
				 __type, __mei, nullptr, nullptr))
		{
			DWORD result = HRESULT_FROM_WIN32(GetLastError());
			pf_print(
				dbg_type::warning, dbg_level::high,
				"[WIN] Failed to generate dumpbin at path={}, code={}, reason={}",
				u8path.data(), result, __dbg_generate_error_message_win(result).begin());
		}

		// 4. Close File
		if (!CloseHandle(file))
		{
			pf_print(
				dbg_type::warning, dbg_level::high,
				"[WIN] Failed closing file handle for file={}, at path={}", union_cast<void*>(file), u8path.data());
		}

		// 5. Info
		pf_print(
			dbg_type::info, dbg_level::high,
			"[WIN] Dumpfile written at path={}.", u8path.begin());

		// 6. Returns
		return u8path;
	}
	pf_decl_static void __dbg_generate_unknown_dumpbin()
	{
		// 1. Print
		pf_print(dbg_type::info, dbg_level::high, "[WIN] Unknown error caught. Generating dumpbin...");

		// 2. Generating Dumpbin
		dbg_u8string u8path = __dbg_generate_dumpbin(nullptr, MINIDUMP_TYPE::MiniDumpNormal);

		// 3. Popup
		__dbg_generate_error_popup_win(
			"Pulsar", __dbg_format_message(
				"An unknown error has been caught.\n"
				"A dumpfile has been generated at location={}.\n"
				"Press ok to terminate the program.",
				u8path.begin()));
	}
	pf_decl_static void __dbg_generate_exception_dumpbin(
		dbg_category const *__cat,
		uint32_t __code,
		uint32_t __flags,
		dbg_u8string_view __what)
	{
		// 1. Print
		pf_print(dbg_type::info, dbg_level::high, "[WIN] Catching unhandled exception of category={}, code={}, message={}. Generating dumpbin...",
						 __cat->name().data(),
						 __code,
						 __what.data());

		// 2. Generating Dumpbin
		MINIDUMP_EXCEPTION_INFORMATION mei;
		std::memset(&mei, 0, sizeof(mei));
		mei.ThreadId					= __dbg_exception_handler_win::thrower_thread_id();
		mei.ExceptionPointers = __dbg_exception_handler_win::current_exception();
		mei.ClientPointers		= TRUE;
		dbg_u8string u8path = __dbg_generate_dumpbin(&mei, __dbg_flags_to_minidump_type_win(__flags));

		// 3. Popup
		__dbg_generate_error_popup_win(
			"Pulsar", __dbg_format_message(
				"An exception has been thrown of category={}, code={}, message={} | {}.\n"
				"A dumpfile has been generated at location={}.\n"
				"Press ok to terminate the program.",
				__cat->name().data(),
				__code,
				__cat->message(__code).data(),
				__what.data(),
				u8path.data()));
	}

	/// DEBUG: Win -> Category -> System
	pulsar_api dbg_u8string dbg_category_system_t::message(
		uint32_t __val) const pf_attr_noexcept
	{
		return __dbg_generate_error_message_win(__val);
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
			catch(dbg_exception const &__e)
			{
				// It's an exception. Generate dumpbin
				__dbg_generate_exception_dumpbin(
					__e.category(),
					__e.code(),
					__e.flags(),
					__e.what());
			}
			catch(std::system_error const &__e)
			{
				__dbg_generate_exception_dumpbin(
					dbg_category_system(),
					__e.code().value(),
					dbg_flags::none,
					__e.what());
			}
			catch(std::invalid_argument const &__e)
			{
				__dbg_generate_exception_dumpbin(
					dbg_category_generic(),
					union_cast<uint32_t>(errv::invalid_argument),
					dbg_flags::none,
					__e.what());
			}
			catch(std::domain_error const &__e)
			{
				__dbg_generate_exception_dumpbin(
					dbg_category_generic(),
					union_cast<uint32_t>(errv::domain_error),
					dbg_flags::none,
					__e.what());
			}
			catch(std::length_error const &__e)
			{
				__dbg_generate_exception_dumpbin(
					dbg_category_generic(),
					union_cast<uint32_t>(errv::length_error),
					dbg_flags::none,
					__e.what());
			}
			catch(std::out_of_range const &__e)
			{
				__dbg_generate_exception_dumpbin(
					dbg_category_generic(),
					union_cast<uint32_t>(errv::out_of_range),
					dbg_flags::none,
					__e.what());
			}
			catch(std::logic_error const &__e)
			{
				__dbg_generate_exception_dumpbin(
					dbg_category_generic(),
					union_cast<uint32_t>(errv::logic_error),
					dbg_flags::none,
					__e.what());
			}
			catch(std::range_error const &__e)
			{
				__dbg_generate_exception_dumpbin(
					dbg_category_generic(),
					union_cast<uint32_t>(errv::range_error),
					dbg_flags::none,
					__e.what());
			}
			catch(std::overflow_error const &__e)
			{
				__dbg_generate_exception_dumpbin(
					dbg_category_generic(),
					union_cast<uint32_t>(errv::overflow_error),
					dbg_flags::none,
					__e.what());
			}
			catch(std::underflow_error const &__e)
			{
				__dbg_generate_exception_dumpbin(
					dbg_category_generic(),
					union_cast<uint32_t>(errv::underflow_error),
					dbg_flags::none,
					__e.what());
			}
			catch(std::runtime_error const &__e)
			{
				__dbg_generate_exception_dumpbin(
					dbg_category_generic(),
					union_cast<uint32_t>(errv::runtime_error),
					dbg_flags::none,
					__e.what());
			}
			catch(std::bad_alloc const &__e)
			{
				__dbg_generate_exception_dumpbin(
					dbg_category_generic(),
					union_cast<uint32_t>(errv::bad_alloc),
					dbg_flags::none,
					__e.what());
			}
			catch(std::exception const &__e)
			{
				__dbg_generate_exception_dumpbin(
					dbg_category_generic(),
					union_cast<uint32_t>(errv::unknown),
					dbg_flags::none,
					__e.what());
			}
			// TODO: Other exceptions ?
		}
		else
		{
			__dbg_generate_unknown_dumpbin();
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
}


#endif // !PF_OS_WINDOWS
#endif // !PULSAR_SRC_DEBUG_WIN_HPP

// TODO: Move PEXCEPTION_POINTERS + threadID to another thread