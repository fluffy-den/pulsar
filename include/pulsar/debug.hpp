/*! @file   debug.hpp
 *  @author Louis-Quentin Noé (noe.louis-quentin@hotmail.fr)
 *  @brief
 *  @date   27-06-2022
 *
 *  @copyright Copyright (c) 2023 - Pulsar Software
 *
 *  @since 0.1.4
 */

#ifndef PULSAR_DEBUG_HPP
#define PULSAR_DEBUG_HPP 1

// Include: Pulsar
#include "pulsar/pulsar.hpp"
#include "pulsar/malloc.hpp"
#include "pulsar/function.hpp"
#include "pulsar/chrono.hpp"
#include "pulsar/concurrency.hpp"

// Include: Fmt
#include "fmt/color.h"
#include "fmt/ranges.h"
#include "fmt/chrono.h"
#include "fmt/format.h"

// Include: C++
#include <thread>

// Pulsar
namespace pul
{
	/// DEBUG: Constants
	// Error Values
	enum class dbg_code : uint32_t
	{
		invalid_argument,
		domain_error,
		length_error,
		out_of_range,
		logic_error,
		range_error,
		overflow_error,
		underflow_error,
		runtime_error,
		bad_alloc,
		unknown = static_cast<uint32_t>(-1)
	};

	// Types
	enum class dbg_type : char_t
	{
		info			= 'I',
		warning		= 'W',
		extension = '\0'
	};

	// Levels
	enum class dbg_level : uint32_t
	{
		low		 = 0,
		medium = 1,
		high	 = 2
	};

	// Flags
	namespace dbg_flags
	{
		pf_decl_constexpr uint32_t none = 0x00'00'00'00;

		// NOTE: Dumpfile MUST be only generated in release mode!
		pf_decl_constexpr uint32_t dump_with_data_segs										= 0x00'00'00'01;	// Dump with data sections of all modules (global variables).
		pf_decl_constexpr uint32_t dump_with_full_memory									= 0x00'00'00'02;	// Dump with all accessible memory of the process.
		pf_decl_constexpr uint32_t dump_with_handle_data									= 0x00'00'00'04;	// Dump with high-level information of the OS.
		pf_decl_constexpr uint32_t dump_filter_memory											= 0x00'00'00'08;	// Dump with stack and backing store memory filtered.
		pf_decl_constexpr uint32_t dump_with_scan_memory									= 0x00'00'00'10;	// Dump with stack and backing store memory scanned for reference on modules.
		pf_decl_constexpr uint32_t dump_with_unloaded_modules							= 0x00'00'00'20;	// Dump with unloaded modules information.
		pf_decl_constexpr uint32_t dump_with_indirectly_referenced_memory = 0x00'00'00'40;	// Dump with pages referenced by locals or other stack memory.
		pf_decl_constexpr uint32_t dump_with_filter_module_paths					= 0x00'00'00'80;	// Dump with module path filtering such as user names or important directories.
		pf_decl_constexpr uint32_t dump_with_process_thread_data					= 0x00'00'01'00;	// Dump with complete per-process and per-thread information from OS.
		pf_decl_constexpr uint32_t dump_with_private_read_write_memory		= 0x00'00'02'00;	// Dump with scan of virtual address space with read and write access.
		pf_decl_constexpr uint32_t dump_without_auxiliary_state						= 0x00'00'04'00;	// Dump without auxiliary-supported memory gathering.
		pf_decl_constexpr uint32_t dump_with_full_auxiliary_state					= 0x00'00'08'00;	// Dump with full auxiliary data.
		pf_decl_constexpr uint32_t dump_with_private_read_write						= 0x00'00'10'00;	// Dump with virtual address with write only access.
		pf_decl_constexpr uint32_t dump_ignore_inaccessible_memory				= 0x00'00'20'00;	// Dump without inaccessible memory.
		pf_decl_constexpr uint32_t dump_with_token_information						= 0x00'00'40'00;	// Dump with security tokens for user data.
		pf_decl_constexpr uint32_t dump_with_module_headers								= 0x00'00'80'00;	// Dump with module header related data.
		pf_decl_constexpr uint32_t dump_filter_triage											= 0x00'01'00'00;	// Dump with filter triage related data.
		pf_decl_constexpr uint32_t dump_with_avx_state_context						= 0x00'02'00'00;	// Dump with avx state context.
		pf_decl_constexpr uint32_t dump_with_ipt_trace										= 0x00'04'00'00;	// Dump with Intel processor trace related data.
		pf_decl_constexpr uint32_t dump_scan_inaccessible_partial_pages		= 0x00'08'00'00;	// Dump with inaccessible partial pages.
		pf_decl_constexpr uint32_t dump_valid_type_flags									= 0x00'10'00'00;	// Dump with valid type flags.
	}																																											// namespace dbg_flags

	// Constants
	pf_decl_constexpr uint32_t DBG_FMT_NAME_LEN							 = 1'024;
	pf_decl_constexpr uint32_t DBG_FMT_STACK_FRAMES					 = 63;
	pf_decl_constexpr uint32_t DBG_FMT_WRITE_OFFSET					 = sizeof("[HHHH:MM:SS:MMMM]");
	pf_decl_constexpr uint32_t DBG_FMT_STACKTRACE_MAX_LENGTH = 8'192;

	/// DEBUG: UTF8 -> Types
	// String View
	class dbg_u8string_view
	{
	public:
		/// Constructors
		pf_decl_constexpr pf_decl_inline
		dbg_u8string_view() pf_attr_noexcept
			: count_(0)
			, store_(nullptr)
		{}
		pf_decl_constexpr pf_decl_inline
		dbg_u8string_view(
		 nullptr_t)
			: dbg_u8string_view()
		{}
		pf_decl_constexpr pf_decl_inline
		dbg_u8string_view(
		 const char_t *__str,
		 size_t __count) pf_attr_noexcept
			: count_(__count)
			, store_(__str)
		{}
		pf_decl_constexpr pf_decl_inline
		dbg_u8string_view(
		 const char_t *__str) pf_attr_noexcept
			: count_(std::strlen(__str))
			, store_(__str)
		{}
		dbg_u8string_view(
		 dbg_u8string_view const &) = default;
		dbg_u8string_view(
		 dbg_u8string_view &&) = default;

		/// Operator =
		dbg_u8string_view &
		operator=(
		 dbg_u8string_view const &) = default;
		dbg_u8string_view &
		operator=(
		 dbg_u8string_view &&) = default;

		/// Operator[]
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const char_t &
		operator[](
		 size_t __index) const pf_attr_noexcept
		{
			return this->store_[__index];
		}

		/// Begin
		pf_hint_nodiscard pf_decl_constexpr pf_decl_inline const char_t *
		begin() const pf_attr_noexcept
		{
			return &this->store_[0];
		}
		pf_hint_nodiscard pf_decl_constexpr pf_decl_inline const char_t *
		cbegin() const pf_attr_noexcept
		{
			return &this->store_[0];
		}

		/// End
		pf_hint_nodiscard pf_decl_constexpr pf_decl_inline const char_t *
		end() const pf_attr_noexcept
		{
			return &this->store_[0] + this->count_;
		}
		pf_hint_nodiscard pf_decl_constexpr pf_decl_inline const char_t *
		cend() const pf_attr_noexcept
		{
			return &this->store_[0] + this->count_;
		}

		/// Data
		pf_hint_nodiscard pf_decl_constexpr pf_decl_inline const char_t *
		data() const pf_attr_noexcept
		{
			return &this->store_[0];
		}


		/// Size
		pf_hint_nodiscard pf_decl_constexpr pf_decl_inline size_t
		size() pf_attr_noexcept
		{
			return this->count_;
		}

		/// Count
		pf_hint_nodiscard pf_decl_constexpr pf_decl_inline size_t
		count() pf_attr_noexcept
		{
			return this->count_;
		}

	private:
		size_t count_;
		const char_t *store_;
	};

	// String
	class dbg_u8string
	{
		/// __assign_view
		pf_decl_constexpr void
		__assign_view(
		 dbg_u8string_view __v) pf_attr_noexcept
		{
			memcpy(this->data(), __v.data(), this->count_);
			*(this->data() + this->count_) = '\0';
		}

	public:
		/// Constructors
		pf_decl_constexpr pf_decl_inline
		dbg_u8string() pf_attr_noexcept
			: count_(0)
			, store_(nullptr)
		{}
		pf_decl_constexpr pf_decl_inline
		dbg_u8string(
		 nullptr_t) pf_attr_noexcept
			: dbg_u8string()
		{}
		pf_decl_inline
		dbg_u8string(
		 const char_t *__str,
		 size_t __count)
			: count_(__count + 1)
			, store_(union_cast<char_t *>(calloc(this->count_ + 1)))
		{
			memcpy(this->store_, __str, __count);
			*(this->data() + this->count_) = '\0';
		}
		pf_decl_inline
		dbg_u8string(
		 const char_t *__str)
			: count_(std::strlen(__str))
			, store_(union_cast<char_t *>(calloc(this->count_ + 1)))
		{
			memcpy(this->store_, __str, this->count_);
			*(this->data() + this->count_) = '\0';
		}
		pf_decl_inline
		dbg_u8string(
		 size_t __count,
		 char_t __val) pf_attr_noexcept
			: count_(__count)
			, store_(union_cast<char_t *>(calloc(this->count_ + 1)))
		{
			memset(this->store_, __val, __count);
			*(this->data() + this->count_) = '\0';
		}
		pf_decl_inline
		dbg_u8string(
		 dbg_u8string &&__r) pf_attr_noexcept
			: count_(__r.count_)
			, store_(__r.store_)
		{
			__r.count_ = 0;
			__r.store_ = nullptr;
		}
		pf_decl_inline
		dbg_u8string(
		 dbg_u8string const &__r) pf_attr_noexcept
			: dbg_u8string(__r.data())
		{}
		pf_decl_inline
		dbg_u8string(
		 dbg_u8string_view __v) pf_attr_noexcept
			: count_(__v.count())
			, store_(union_cast<char_t *>(calloc(__v.count() + 1)))
		{
			this->__assign_view(__v);
		}

		/// Destructor
		pf_decl_constexpr pf_decl_inline ~dbg_u8string() pf_attr_noexcept
		{
			if(this->store_) cfree(this->store_);
		}

		/// Operator =
		pf_decl_constexpr pf_decl_inline dbg_u8string &
		operator=(
		 dbg_u8string const &__r)
		{
			this->count_ = 0;
			this->__assign_view(__r.view());
			return *this;
		}
		pf_decl_constexpr pf_decl_inline dbg_u8string &
		operator=(
		 dbg_u8string &&__r)
		{
			if(this->store_) cfree(this->store_);
			this->count_ = __r.count_;
			__r.count_	 = 0;
			this->store_ = __r.store_;
			__r.store_	 = 0;
			return *this;
		}
		pf_decl_constexpr pf_decl_inline dbg_u8string &
		operator=(
		 dbg_u8string_view __v) pf_attr_noexcept
		{
			this->__assign_view(__v);
			return *this;
		}

		/// Operator[]
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr char_t &
		operator[](
		 size_t __index) pf_attr_noexcept
		{
			return this->store_[__index];
		}

		/// View
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr dbg_u8string_view
		view() const pf_attr_noexcept
		{
			return dbg_u8string_view(&this->store_[0], this->count_);
		}

		/// Begin/End
		pf_hint_nodiscard pf_decl_constexpr pf_decl_inline char_t *
		begin() pf_attr_noexcept
		{
			return &this->store_[0];
		}
		pf_hint_nodiscard pf_decl_constexpr pf_decl_inline const char_t *
		begin() const pf_attr_noexcept
		{
			return &this->store_[0];
		}
		pf_hint_nodiscard pf_decl_constexpr pf_decl_inline char_t *
		end() pf_attr_noexcept
		{
			return &this->store_[0] + this->count_ - 1;
		}
		pf_hint_nodiscard pf_decl_constexpr pf_decl_inline const char_t *
		end() const pf_attr_noexcept
		{
			return &this->store_[0] + this->count_ - 1;
		}

		/// Data
		pf_hint_nodiscard pf_decl_constexpr pf_decl_inline char_t *
		data() pf_attr_noexcept
		{
			return &this->store_[0];
		}
		pf_hint_nodiscard pf_decl_constexpr pf_decl_inline const char_t *
		data() const pf_attr_noexcept
		{
			return &this->store_[0];
		}

		/// Size
		pf_hint_nodiscard pf_decl_constexpr pf_decl_inline size_t
		size() const pf_attr_noexcept
		{
			return this->count_;
		}
		pf_hint_nodiscard pf_decl_constexpr pf_decl_inline size_t
		count() const pf_attr_noexcept
		{
			return this->count_ - 1;
		}

		/// Operator (View)
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr
		operator dbg_u8string_view() const pf_attr_noexcept
		{
			return this->view();
		}

	private:
		size_t count_;
		char_t *store_;
	};

	// Format
	template<typename... _Args>
	using dbg_u8string_format = fmt::basic_format_string<char_t, fmt::type_identity_t<_Args>...>;
	using dbg_color						= fmt::color;
	using dbg_style						= fmt::text_style;
	using dbg_emphasis				= fmt::emphasis;

	/// DEBUG: Category
	class dbg_category
	{
	public:
		/// Constructor
		pf_decl_constexpr
		dbg_category() pf_attr_noexcept = default;

		/// Name
		pf_hint_nodiscard pf_decl_virtual dbg_u8string_view
		name() const pf_attr_noexcept = 0;

		/// Message
		pf_hint_nodiscard pf_decl_virtual dbg_u8string
		message(
		 uint32_t __val) const pf_attr_noexcept = 0;
	};


	/// DEBUG: Category -> Generic
	class dbg_category_generic_t : public dbg_category
	{
	public:
		/// Constructors
		pf_decl_constexpr
		dbg_category_generic_t() pf_attr_noexcept = default;

		/// Name
		pf_hint_nodiscard pf_decl_inline dbg_u8string_view
		name() const pf_attr_noexcept
		{
			return "generic";
		}

		/// Message
		pf_hint_nodiscard pf_decl_inline dbg_u8string
		message(
		 uint32_t __val) const pf_attr_noexcept pf_attr_override
		{
			switch(union_cast<dbg_code>(__val))
			{
				case dbg_code::invalid_argument: return "Invalid argument";
				case dbg_code::domain_error: return "Domain error";
				case dbg_code::length_error: return "Length error";
				case dbg_code::out_of_range: return "Out of range";
				case dbg_code::logic_error: return "Logic error";
				case dbg_code::overflow_error: return "Overflow error";
				case dbg_code::underflow_error: return "Underflow error";
				case dbg_code::runtime_error: return "Runtime error";
				case dbg_code::bad_alloc: return "Bad alloc";
				default: return "Unknown";
			};
		}
	};

	/// Generic -> Instance
	pf_decl_static pf_decl_inline dbg_category_generic_t dbg_category_generic_instance;

	/// Generic -> Retrieve
	pf_decl_inline dbg_category *
	dbg_category_generic() pf_attr_noexcept
	{
		return &dbg_category_generic_instance;
	}


	/// DEBUG: Category -> System
	/// System -> Type
	class dbg_category_system_t : public dbg_category
	{
	public:
		/// Constructors
		pf_decl_constexpr
		dbg_category_system_t() pf_attr_noexcept = default;

		/// Name
		pf_hint_nodiscard pf_decl_inline dbg_u8string_view
		name() const pf_attr_noexcept
		{
			return "system";
		}

		/// Message
		pf_hint_nodiscard pulsar_api dbg_u8string
		message(
		 uint32_t __val) const pf_attr_noexcept pf_attr_override;
	};

	/// System -> Instance
	pf_decl_static pf_decl_inline dbg_category_system_t dbg_category_system_instance;

	/// System -> Retrieve
	pf_decl_inline pf_decl_constexpr dbg_category *
	dbg_category_system() pf_attr_noexcept
	{
		return &dbg_category_system_instance;
	}

	/// DEBUG: Exception
	class dbg_exception
		: public std::exception
	{
	public:
		/// Constructors
		dbg_exception(
		 dbg_category const *__cat,
		 uint32_t __code,
		 uint32_t __flags,
		 dbg_u8string_view __msg) pf_attr_noexcept
			: cat_(__cat)
			, msg_(__msg)
			, code_(__code)
			, flags_(__flags)
		{}
		dbg_exception(
		 dbg_exception const &) = delete;
		dbg_exception(
		 dbg_exception &&__r)
			: cat_(__r.cat_)
			, msg_(std::move(__r.msg_))
			, code_(__r.code_)
			, flags_(__r.flags_)
		{}

		/// Destructor
		~dbg_exception() pf_attr_noexcept = default;

		/// What
		pf_hint_nodiscard pf_decl_inline const char_t *
		what() const pf_attr_noexcept pf_attr_override
		{
			return this->msg_.begin();
		}
		pf_hint_nodiscard pf_decl_inline const char_t *
		type() const pf_attr_noexcept
		{
			return this->cat_->message(this->code_).begin();
		}

		/// Category
		pf_hint_nodiscard pf_decl_inline const dbg_category *
		category() const pf_attr_noexcept
		{
			return this->cat_;
		}

		/// Code
		pf_hint_nodiscard pf_decl_inline uint32_t
		code() const pf_attr_noexcept
		{
			return this->code_;
		}

		/// Flags
		pf_hint_nodiscard pf_decl_inline uint32_t
		flags() const pf_attr_noexcept
		{
			return this->flags_;
		}

	private:
		dbg_category const *cat_;
		dbg_u8string msg_;
		uint32_t code_;
		uint32_t flags_;
	};


	/// DEBUG: Exception -> Record -> Make
	pf_hint_nodiscard pulsar_api void *
	__dbg_get_current_exception_record() pf_attr_noexcept;
	pulsar_api void
	__dbg_set_current_exception_record(
	 void *__record) pf_attr_noexcept;

	/// DEBUG: Exception -> Record
	class __dbg_exception_record_t
	{
	public:
		/// Constructors
		__dbg_exception_record_t() pf_attr_noexcept
			: ptr_(nullptr)
			, record_(nullptr)
		{}
		__dbg_exception_record_t(__dbg_exception_record_t const &) = delete;
		__dbg_exception_record_t(__dbg_exception_record_t &&)			 = delete;

		/// Destructor
		~__dbg_exception_record_t() pf_attr_noexcept
		{
			this->rethrow();	// NOTE: If user forget to check for exception, and one is handled, the program will crash!
		}

		/// Operator =
		__dbg_exception_record_t &
		operator=(
		 __dbg_exception_record_t const &) = delete;
		__dbg_exception_record_t &
		operator=(
		 __dbg_exception_record_t &&) = delete;

		/// Rethrow
		pulsar_api void
		rethrow();

		/// Operator (Bool)
		pf_hint_nodiscard operator bool() const pf_attr_noexcept
		{
			return this->ptr_ != nullptr;
		}

		/// Set
		void
		set_record(
		 std::exception_ptr &&__ptr,
		 atomic<bool> *__control)
		{
			this->ptr_		= std::move(__ptr);
			this->ctrl_		= __control;
			this->record_ = __dbg_get_current_exception_record();
		}

	private:
		std::exception_ptr ptr_;
		atomic<bool> *ctrl_;
		void *record_;
	};

	/// DEBUG: Context Record -> Switcher
	// Function
	pulsar_api void
	__dbg_move_exception_record_to_0() pf_attr_noexcept;

	// Type
	class __dbg_exception_record_switcher_t pf_attr_final
	{
	public:
		/// Constructors
		__dbg_exception_record_switcher_t() pf_attr_noexcept
			: ptr_(nullptr)
			, ctrl_(nullptr)
		{}
		__dbg_exception_record_switcher_t(
		 std::exception_ptr &&__ptr,
		 atomic<bool> *__ctrl) pf_attr_noexcept
			: ptr_(__ptr)
			, ctrl_(__ctrl)
		{}
		__dbg_exception_record_switcher_t(__dbg_exception_record_switcher_t const &) = delete;
		__dbg_exception_record_switcher_t(__dbg_exception_record_switcher_t &&)			 = delete;

		/// Destructor
		~__dbg_exception_record_switcher_t() pf_attr_noexcept
		{
			if(this->ctrl_) this->ctrl_->store(true, atomic_order::relaxed);
		}

		/// Operator =
		__dbg_exception_record_switcher_t &
		operator=(__dbg_exception_record_switcher_t const &) = delete;
		__dbg_exception_record_switcher_t &
		operator=(__dbg_exception_record_switcher_t &&) = delete;

		/// Rethrow
		pf_hint_noreturn void
		__rethrow() const
		{
			std::rethrow_exception(this->ptr_);
		}

	private:
		std::exception_ptr ptr_;
		atomic<bool> *ctrl_;
	};

	/// DEBUG: Format -> Functions
	template<typename... _Args>
	pf_decl_inline char_t *
	dbg_u8format_to(
	 char_t *__to,
	 dbg_u8string_format<_Args...> __fmt,
	 _Args &&...__args) pf_attr_noexcept
	{
		// Returns
		return fmt::format_to(__to, __fmt, std::forward<_Args>(__args)...);
	}
	template<typename... _Args>
	pf_hint_nodiscard pf_decl_inline dbg_u8string
	dbg_u8format(	 // Message
	 dbg_u8string_format<_Args...> __fmt,
	 _Args &&...__args) pf_attr_noexcept
	{
		// Format
		dbg_u8string str(fmt::formatted_size(__fmt, std::forward<_Args>(__args)...), '\0');
		dbg_u8format_to(str.data(), __fmt, std::forward<_Args>(__args)...);

		// Return
		return str;
	}
	template<typename... _Args>
	pf_decl_constexpr void
	dbg_u8print(
	 dbg_u8string_format<_Args...> __fmt,
	 _Args &&...__args)
	{
		// Print
		fmt::print(__fmt, std::forward<_Args>(__args)...);
	}
	template<typename _Ty>
	pf_decl_constexpr pf_decl_inline auto
	dbg_styled(
	 const _Ty &__val,
	 dbg_style __ts) pf_attr_noexcept
	{
		// Styled
		return fmt::styled(__val, __ts);
	}
	template<typename _Ty>
	pf_decl_constexpr pf_decl_inline auto
	dbg_style_fg(
	 _Ty __ts) pf_attr_noexcept
	{
		// Styled
		return fmt::fg(__ts);
	}
	template<typename _Ty>
	pf_decl_constexpr pf_decl_inline auto
	dbg_style_bg(
	 _Ty __ts) pf_attr_noexcept
	{
		// Styled
		return fmt::bg(__ts);
	}

	/// DEBUG: Logger
	using dbg_logger_callback_t = fun_ptr<void(dbg_u8string_view)>;

	/// Parameters
	pf_hint_nodiscard pulsar_api dbg_level
	dbg_log_filter() pf_attr_noexcept;
	pf_hint_nodiscard pulsar_api dbg_logger_callback_t
	dbg_log_callback() pf_attr_noexcept;
	pulsar_api void
	dbg_log_set_filter(
	 dbg_level __level) pf_attr_noexcept;
	pulsar_api void
	dbg_log_set_callback(
	 dbg_logger_callback_t __callback) pf_attr_noexcept;

	/// Elapsed Time
	pf_hint_nodiscard pulsar_api nanoseconds_t
	dbg_elapsed_time() pf_attr_noexcept;

	/// Logger
	pulsar_api void
	dbg_log(
	 dbg_u8string &&__msg);
	pulsar_api void
	dbg_log(
	 dbg_level __level,
	 dbg_u8string &&__msg);

	/// DEBUG: Format -> Chrono
	pf_hint_nodiscard pulsar_api char_t *
	__dbg_format_chrono_to(
	 char_t *__w) pf_attr_noexcept;

	/// DEBUG: StackTrace
	struct __dbg_stacktrace_formatted_t
	{
		size_t length;
		char_t message[DBG_FMT_STACKTRACE_MAX_LENGTH];
	};
	pf_hint_nodiscard pulsar_api __dbg_stacktrace_formatted_t
	__dbg_retrieve_stacktrace() pf_attr_noexcept;
	pulsar_api char_t *
	__dbg_append_stacktrace_to(
	 char_t *__w,
	 __dbg_stacktrace_formatted_t const &__st) pf_attr_noexcept;

	/// DEBUG: Print
	template<typename... _Args>
	pulsar_api void
	dbg_print_current_exception() pf_attr_noexcept;
	template<typename... _Args>
	pf_decl_static void
	__dbg_print(
	 dbg_type __type,
	 dbg_level __level,
	 dbg_u8string_format<_Args...> __fmt,
	 _Args &&...__args)
	{
		// Level
		if(__level < dbg_log_filter()) return;

		// By Type
		if(__type == dbg_type::extension)
		{
			dbg_u8print(__fmt, std::forward<_Args>(__args)...);
		}
		else
		{
			// Initialisation
			const char_t *lvl = nullptr;
			fmt::text_style style;
			switch(__level)
			{
				case dbg_level::low:
				{
					lvl		= "low";
					style = fmt::bg(fmt::color::midnight_blue);
					break;
				}
				case dbg_level::medium:
				{
					lvl		= "medium";
					style = fmt::bg(fmt::color::green);
					break;
				}
				case dbg_level::high:
				{
					lvl		= "high";
					style = fmt::bg(fmt::color::indian_red);
					break;
				}
			};

			// Size
			const size_t s =
			 DBG_FMT_WRITE_OFFSET
			 + fmt::formatted_size(
				" /{}/ /{}/ message=",
				fmt::styled(union_cast<char_t>(__type), __type == dbg_type::info ? fmt::fg(fmt::color::sky_blue) : fmt::fg(fmt::color::orange_red)),
				fmt::styled(lvl, style))
			 + fmt::formatted_size(__fmt, std::forward<_Args>(__args)...) + 56;

			// Print
			dbg_u8string prt(s, '\0');
			auto *p = prt.begin();
			p				= __dbg_format_chrono_to(p);
			p				= dbg_u8format_to(p, " /{}/ /{}/ message=", fmt::styled(union_cast<char_t>(__type), __type == dbg_type::info ? fmt::fg(fmt::color::sky_blue) : fmt::fg(fmt::color::orange_red)), fmt::styled(lvl, style));
			p				= dbg_u8format_to(p, __fmt, std::forward<_Args>(__args)...);
			*(p++)	= '\n';
			dbg_log(std::move(prt));
		}
	}
	template<typename... _Args>
	pf_decl_static void
	__dbg_print(
	 dbg_level __level,
	 dbg_u8string_format<_Args...> __fmt,
	 _Args &&...__args)
	{
		// Level
		if(__level < dbg_log_filter()) return;

		// Print
		dbg_u8string prt = dbg_u8format(__fmt, std::forward<_Args>(__args)...);
		dbg_log(std::move(prt));
	}
	/*!
	 * @brief Print a message with high debug level
	 *
	 * @tparam _Args Variadic template paramater
	 * @param[in] __fmt The input format for printing
	 * @param[in] __args The variadic arguments for printing
	 */
	template<typename... _Args>
	pf_decl_static void
	__dbg_print(
	 dbg_u8string_format<_Args...> __fmt,
	 _Args &&...__args)
	{
		// Print
		return __dbg_print(dbg_level::high, __fmt, std::forward<_Args>(__args)...);
	}
	/*!
	 * @brief Format an error message into a u8string
	 *
	 * @param[in] __fmt The format string for the message
	 * @param[in] __args Variadic list of parameters for the format string
	 *
	 * @return dbg_u8string The formatted message with thread ID and stack trace appended
	 */
	template<typename... _Args>
	pf_hint_nodiscard pf_decl_static dbg_u8string
	__dbg_format_error_message(	 // Assertion
	 dbg_u8string_format<_Args...> __fmt,
	 _Args &&...__args) pf_attr_noexcept
	{
		// Initialisation
		auto st = __dbg_retrieve_stacktrace();
		auto ID = this_thread::get_id();

		// Size
		const size_t s = DBG_FMT_WRITE_OFFSET + fmt::formatted_size(" T={} /{}/ message=", ID, fmt::styled('A', fmt::fg(fmt::color::orange) | fmt::bg(fmt::color::black))) + fmt::formatted_size(__fmt, std::forward<_Args>(__args)...) + st.length;

		// Format
		dbg_u8string str(s, '\0');
		char_t *p = str.data();
		p					= __dbg_format_chrono_to(p);
		p					= dbg_u8format_to(p, " T={} /{}/ message=", ID, fmt::styled('A', fmt::fg(fmt::color::orange) | fmt::bg(fmt::color::black)));
		p					= dbg_u8format_to(p, __fmt, std::forward<_Args>(__args)...);
		*p				= '\n';
		++p;
		p = __dbg_append_stacktrace_to(p, st);

		// Return
		return str;
	}

	/// DEBUG: Utility
	/*! @brief This function throws a dbg_exception with details provided in the
	 *  arguments.
	 *
	 *  @param[in] __cat   A pointer to a debug category instance.
	 *  @param[in] __code  An error code value.
	 * 	@param[in] __flags (optional) Additionnal parameters for the exception.
	 *                     See dbg_flags.
	 *  @param[in] __msg   Message
	 *
	 *  @note no return (optimized)
	 *  @note thread safe
	 */
	pf_hint_noreturn pf_decl_static pf_decl_inline void
	__dbg_throw(
	 dbg_category const *__cat,
	 uint32_t __code,
	 uint32_t __flags,
	 dbg_u8string &&__msg)
	{
		throw(dbg_exception(__cat, __code, __flags, std::move(__msg)));
	}
	/*! @brief This function will take a dbg_u8string as an input and prints it
	 *  on the standard output then the program aborts.
	 *
	 *  @param[in] __msg The message to log with the assertion.
	 *
	 *  @note no return (optimized)
	 *  @note thread safe
	 */
	pf_hint_noreturn pf_decl_static pf_decl_inline void
	__dbg_assert(
	 dbg_u8string &&__msg) pf_attr_noexcept
	{
		dbg_log(std::move(__msg));
		std::abort();
	}

	/// DEBUG: Terminate
	// Handle Type
	using dbg_terminate_handle_t = fun_ptr<void(void)>;

	/*! @brief This function is used to set a `dbg_terminate_handle_t` as the
	 *  current handle for termination.
	 *
	 *  @param[in] __handle The handle for termination.
	 */
	pulsar_api void
	dbg_set_terminate_handle(
	 dbg_terminate_handle_t __handle) pf_attr_noexcept;
	/*! @brief This function is used to get the current handle for termination.
	 *
	 *  @return A `dbg_terminate_handle_t` that holds the current handle for
	 *  termination.
	 */
	pf_hint_nodiscard pulsar_api dbg_terminate_handle_t
	dbg_get_terminate_handle() pf_attr_noexcept;
	/*! @brief This function is used to terminate the process.
	 */
	pf_hint_noreturn pulsar_api void
	dbg_terminate() pf_attr_noexcept;
}	 // namespace pul

#define pf_print(...)												 pul::__dbg_print(__VA_ARGS__)
#define pf_throw(cat, val, bit, format, ...) pul::__dbg_throw(cat, pul::union_cast<uint32_t>(val), bit, pul::dbg_u8format(format __VA_OPT__(, ) __VA_ARGS__))
#define pf_throw_if(c, cat, val, bit, format, ...) \
	if(pf_unlikely(c)) pf_throw(cat, val, bit, format __VA_OPT__(, ) __VA_ARGS__)
#ifdef PF_DEBUG
 #define pf_assert(c, ...) \
	 if(!(c)) pul::__dbg_assert(pul::__dbg_format_error_message(__VA_ARGS__))
 #define pf_assert_nodiscard(c, ...) pf_assert(c __VA_OPT__(, ) __VA_ARGS__)
#else
 #define pf_assert(c, ...)
 #define pf_assert_nodiscard(c, ...) c
#endif
#define pf_assert_static(...) static_assert(__VA_ARGS__)

#endif	// !PULSAR_DEBUG_HPP
