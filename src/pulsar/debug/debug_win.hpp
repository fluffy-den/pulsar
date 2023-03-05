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

// Include: Pulsar
#include "pulsar/debug.hpp"

// Include: Windows
#ifdef PF_OS_WINDOWS
#include <windows.h>
#include <initguid.h>
#include <knownfolders.h>
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
	class __dbg_wsstring_view	// TODO: Upgrade
	{
	public:
		/// Constructors
		pf_decl_inline __dbg_wsstring_view() pf_attr_noexcept
			: count_(0)
			, store_(nullptr)
		{}
		pf_decl_inline __dbg_wsstring_view(
			const wchar_t *__str,
			size_t __count) pf_attr_noexcept
			: count_(__count)
			, store_(__str)

		{}
		pf_decl_inline __dbg_wsstring_view(
			const wchar_t *__str) pf_attr_noexcept
			: count_(std::wcslen(__str))
			, store_(__str)
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
		size_t count_;
		const wchar_t *store_;
	};

	// String
	class __dbg_wsstring// TODO: Upgrade
	{
	/// __assign_view
	void __assign_view(
		__dbg_wsstring_view __v) pf_attr_noexcept
	{
		this->shrink(__v.size());
		std::memcpy(this->data(), __v.data(), this->count_ * sizeof(wchar_t));
		*(this->data() + this->count_) = L'\0';
	}

	public:
		using iterator_t			 = wchar_t*;
		using const_iterator_t = const wchar_t*;

		/// Constructors
		pf_decl_inline __dbg_wsstring() pf_attr_noexcept
			: count_(0)
			, store_(nullptr)
		{}
		pf_decl_inline __dbg_wsstring(
			nullptr_t) pf_attr_noexcept
			: __dbg_wsstring()
		{}
		pf_decl_inline __dbg_wsstring(
			size_t __count,
			wchar_t __val) pf_attr_noexcept
			: count_(__count)
			, store_(union_cast<wchar_t*>(heap_allocate((this->count_ + 1) * sizeof(wchar_t), align_val_t(32), 0)))
		{
			std::memset(this->store_, __val, __count * sizeof(wchar_t));
			*(this->data() + this->count_) = L'\0';
		}
		__dbg_wsstring(
			__dbg_wsstring &&__r) pf_attr_noexcept
			: count_(__r.count_)
			, store_(__r.store_)
		{
			__r.count_ = 0;
			__r.store_ = nullptr;
		}
		__dbg_wsstring(
			__dbg_wsstring const &__r) pf_attr_noexcept
			: __dbg_wsstring(__r.data())
		{}
		pf_decl_inline __dbg_wsstring(
			__dbg_wsstring_view __v) pf_attr_noexcept
			: count_(__v.count())
			, store_(union_cast<wchar_t*>(heap_allocate((count_ + 1) * sizeof(wchar_t), align_val_t(32), 0)))
		{
			std::memcpy(this->store_, __v.data(), __v.size());
			*(this->data() + this->count_) = L'\0';
		}

		/// Destructor
		pf_decl_inline ~__dbg_wsstring() pf_attr_noexcept
		{
			if (this->store_) heap_deallocate(this->store_);
		}

		/// Operator =
		pf_decl_inline __dbg_wsstring &operator=(
			__dbg_wsstring const &__r) pf_attr_noexcept
		{
			this->count_ = 0;
			this->__assign_view(__r.view());
			return *this;
		}
		pf_decl_inline __dbg_wsstring &operator=(
			__dbg_wsstring &&__r) pf_attr_noexcept
		{
			if (this->store_) heap_deallocate(this->store_);
			this->count_ = __r.count_;
			__r.count_	 = 0;
			this->store_ = __r.store_;
			__r.store_	 = 0;
			return *this;
		}
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

		/// Insert
		pf_decl_inline iterator_t
		insert_back(
			const wchar_t *__str)
		{
			const size_t c = std::wcslen(__str);
			iterator_t p	 = this->data() + std::wcslen(this->data());
			std::memcpy(p, __str, std::wcslen(__str) * sizeof(wchar_t));
			this->count_ += c;
			p						 += c;
			*p = '\0';
			return p;
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
			if (__c == 0)
			{
				heap_deallocate(this->store_);
			}
			else
			{
				this->store_ = union_cast<wchar_t*>(heap_reallocate(this->store_, this->count_, __c + sizeof(wchar_t), align_val_t(32), 0));
				this->count_ = __c + sizeof(wchar_t);
			}
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
			return &this->store_[0] + std::wcslen(this->data());
		}
		pf_hint_nodiscard pf_decl_inline const wchar_t*
		end() const pf_attr_noexcept
		{
			return &this->store_[0] + std::wcslen(this->data());
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
		size_t count_;
		wchar_t *store_;
	};

	/// DEBUG: Win -> Internal
	// Type
	struct pf_alignas(32) __dbg_internal_win_t
	{
		// Exception Handler
		PEXCEPTION_POINTERS exp;
		DWORD exid;
		PVOID sehex;
	};

	// Static
	pf_decl_extern __dbg_internal_win_t __dbg_internal_win;

	/// DEBUG: Win -> Exception Handler
	class __dbg_exception_handler_win
	{
	/// Handle
	pf_hint_nodiscard pf_decl_static LONG WINAPI __vectored_exception_handler(
		PEXCEPTION_POINTERS __info) pf_attr_noexcept;

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
		pf_decl_static void __init() pf_attr_noexcept;
		pf_decl_static void __terminate() pf_attr_noexcept;

		/// Current Exception
		pf_hint_nodiscard pf_decl_static pf_decl_inline
		DWORD thrower_thread_id() pf_attr_noexcept
		{
			return __dbg_internal_win.exid;
		}
		pf_hint_nodiscard pf_decl_static pf_decl_inline
		PEXCEPTION_POINTERS current_exception() pf_attr_noexcept
		{
			return __dbg_internal_win.exp;
		}
	};

	/// DEBUG: Win -> StackTrace
	pf_hint_nodiscard char_t*
	__dbg_format_walk_to_win(
		char_t *__where,
		PCONTEXT __ctx,
		size_t __ignore);
	pf_hint_nodiscard char_t*
	__dbg_format_stacktrace_of_exception_to_win(
		char_t *__where,
		size_t __ignore);

	/// DEBUG: Win -> Convert
	pf_hint_nodiscard __dbg_wsstring __dbg_convert_u8_to_wide_win(
		const char_t *__buf,
		size_t __size);
	pf_hint_nodiscard dbg_u8string __dbg_convert_wide_to_u8_win(
		const wchar_t *__buf,
		size_t __count);

	/// DEBUG: Win -> Generate
	pf_hint_nodiscard dbg_u8string __dbg_generate_error_message_win(
		DWORD __error);

	/// DEBUG: Win -> MiniDump
	pf_hint_nodiscard MINIDUMP_TYPE __dbg_flags_to_minidump_type_win(
		uint32_t __flags) pf_attr_noexcept;
	void __dbg_generate_error_popup_win(
		dbg_u8string_view __title,
		dbg_u8string_view __msg);
	pf_hint_nodiscard dbg_u8string __dbg_generate_dumpbin_win(
		MINIDUMP_EXCEPTION_INFORMATION *__mei,
		MINIDUMP_TYPE __type);
	void __dbg_generate_exception_dumpbin_win(
		dbg_category const *__cat,
		uint32_t __code,
		uint32_t __flags,
		dbg_u8string_view __what);
	pf_hint_noreturn void __dbg_terminate_unknown_dumpbin_win() pf_attr_noexcept;
	pf_hint_noreturn void __dbg_terminate_exception_dumpbin_win(
		dbg_category const *__cat,
		uint32_t __code,
		uint32_t __flags,
		dbg_u8string_view __what) pf_attr_noexcept;
	pf_hint_noreturn void __dbg_terminate_win() pf_attr_noexcept;

	/// DEBUG: Win -> Terminate Handler
	class __dbg_initializer_win
	{
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
		pf_decl_static void __init() pf_attr_noexcept;
		pf_decl_static void __terminate() pf_attr_noexcept;
	};
}

#endif // !PF_OS_WINDOWS
#endif // !PULSAR_SRC_DEBUG_WIN_HPP

// TODO: Move PEXCEPTION_POINTERS + threadID to another thread