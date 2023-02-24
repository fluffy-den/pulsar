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
#include "pulsar/memory.hpp"
#include "pulsar/function.hpp"
#include "pulsar/chrono.hpp"

// Include: Fmt
#include "fmt/format.h"

// Include: C++
#include <thread>

// Pulsar
namespace pul
{
	/// DEBUG: Constants
	// Error Values
	enum class errv : uint32_t {
		invalid_argument,
		domain_error,
		length_error,
		logic_error,
		out_of_range,
		runtime_error,
		range_error,
		future_error,
		overflow_error,
		underflow_error,
		bad_alloc,
		unknown = static_cast<uint32_t>(-1)
	};

	// Types
	enum class dbg_type : char_t
	{
		info = 'I',
		warning = 'W',
		extension = '\0'
	};

	// Levels
	enum class dbg_level : uint32_t
	{
		low = 0,
		medium = 1,
		high = 2
	};

	// Flags
namespace dbg_flags
{
	pf_decl_constexpr uint32_t none = 0x00000000;

	// NOTE: Dumpfile MUST be only generated in release mode!
	pf_decl_constexpr uint32_t dump_with_data_segs										= 0x00000001;	// Dump with data sections of all modules (global variables).
	pf_decl_constexpr uint32_t dump_with_full_memory									= 0x00000002;	// Dump with all accessible memory of the process.
	pf_decl_constexpr uint32_t dump_with_handle_data									= 0x00000004;	// Dump with high-level information of the OS.
	pf_decl_constexpr uint32_t dump_with_filter_memory								= 0x00000008;	// Dump with stack and backing store memory filtered.
	pf_decl_constexpr uint32_t dump_with_scan_memory									= 0x00000010;	// Dump with stack and backing store memory scanned for reference on modules.
	pf_decl_constexpr uint32_t dump_with_unloaded_modules							= 0x00000020;	// Dump with unloaded modules information.
	pf_decl_constexpr uint32_t dump_with_indirectly_referenced_memory = 0x00000040;	// Dump with pages referenced by locals or other stack memory.
	pf_decl_constexpr uint32_t dump_with_filter_module_paths					= 0x00000080;	// Dump with module path filtering such as user names or important directories.
	pf_decl_constexpr uint32_t dump_with_process_thread_data					= 0x00000100;	// Dump with complete per-process and per-thread information from OS.
	pf_decl_constexpr uint32_t dump_with_private_readwrite_memory			= 0x00000200;	// Dump with scan of virtual address space with read and write access.
	pf_decl_constexpr uint32_t dump_without_auxiliary_state						= 0x00000400;	// Dump without auxiliary-supported memory gathering.
	pf_decl_constexpr uint32_t dump_with_full_auxiliary_state					= 0x00000800;	// Dump with full auxiliary data.
	pf_decl_constexpr uint32_t dump_with_private_write								= 0x00001000;	// Dump with virtual address with write only access.
	pf_decl_constexpr uint32_t dump_without_inaccessible_memory				= 0x00002000;	// Dump without inaccessible memory.
	pf_decl_constexpr uint32_t dump_with_token_information						= 0x00004000;	// Dump with security tokens for user data.
	pf_decl_constexpr uint32_t dump_with_module_headers								= 0x00008000;	// Dump with module header related data.
	pf_decl_constexpr uint32_t dump_with_filter_triage								= 0x00010000;	// Dump with filter triage related data.
	pf_decl_constexpr uint32_t dump_with_avx_state_context						= 0x00020000;	// Dump with avx state context.
	pf_decl_constexpr uint32_t dump_with_ipt_trace										= 0x00040000;	// Dump with Intel processor trace related data.
	pf_decl_constexpr uint32_t dump_with_inaccessible_partial_pages		= 0x00080000;	// Dump with inaccessible partial pages.
	pf_decl_constexpr uint32_t dump_with_valid_type_flags							= 0x00100000;	// Dump with valid type flags.
}

	// Constants
	pf_decl_constexpr uint32_t DBG_FMT_NAME_LEN						 = 1024;
	pf_decl_constexpr uint32_t DBG_FMT_STACK_FRAMES				 = 63;
	pf_decl_constexpr uint32_t DBG_FMT_STACK_FRAMES_IGNORE = 2;
	pf_decl_constexpr uint32_t DBG_FMT_WRITE_OFFSET				 = sizeof("[HHHH:MM:SS:MMMM]");
	pf_decl_constexpr uint32_t DBG_FMT_BUFFER_SIZE				 = 65536;

	/// DEBUG: UTF8 -> Types
	// String View
	class dbg_u8string_view
	{
	public:
		/// Constructors
		pf_decl_constexpr dbg_u8string_view()
		pf_attr_noexcept
		: store_(nullptr)
		, count_(0)
		{}
		pf_decl_constexpr dbg_u8string_view(
			const char_t * __str,
			size_t __count) pf_attr_noexcept
		: store_(__str)
		, count_(__count)
		{}
		pf_decl_constexpr dbg_u8string_view(
			const char_t * __str) pf_attr_noexcept
		: store_(__str)
		, count_(0)
		{
			for (; this->store_[this->count_] != '\0'; ++this->count_);
		}

		/// Operator =
		pf_decl_constexpr dbg_u8string_view(
			dbg_u8string_view const &) = default;

		/// Operator[]
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const char_t &
		operator[](
			size_t __index) const pf_attr_noexcept
		{
			return this->store_[__index];
		}

		/// Begin/End
		pf_hint_nodiscard pf_decl_constexpr pf_decl_inline const char_t*
		begin() const pf_attr_noexcept
		{
			return &this->store_[0];
		}
		pf_hint_nodiscard pf_decl_constexpr pf_decl_inline const char_t*
		end() const pf_attr_noexcept
		{
			return &this->store_[0] + this->count_;
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
		const char_t *store_;
		size_t count_;
	};

	// String
	class dbg_u8string
	{
	/// __assign_view
	pf_decl_constexpr void __assign_view(
		dbg_u8string_view __v) pf_attr_noexcept
	{
		for (size_t i = 0; i < this->count_; ++i) this->store_[i] = __v[i];
	}

	public:
		/// Constructors
		pf_decl_constexpr dbg_u8string() pf_attr_noexcept
		: store_(nullptr)
		, count_(0)
		{}
		pf_decl_constexpr dbg_u8string(
			size_t __count,
			char_t __val) pf_attr_noexcept
		: store_(union_cast<char_t*>(allocate(__count, align_val_t(32), 0)))
		, count_(__count)
		{
			std::memset(this->store_, __val, __count);
		}
		dbg_u8string(
			dbg_u8string &&) = default;
		dbg_u8string(
			dbg_u8string const &) = default;
		pf_decl_constexpr dbg_u8string(
			dbg_u8string_view __v) pf_attr_noexcept
		: store_(union_cast<char_t*>(allocate(__v.count(), align_val_t(32), 0)))
		, count_(__v.count())
		{
			this->__assign_view(__v);
		}

		/// Destructor
		pf_decl_constexpr ~dbg_u8string() pf_attr_noexcept
		{
			deallocate(this->store_);
		}

		/// Operator =
		pf_decl_constexpr dbg_u8string &operator=(
			dbg_u8string const &) = default;
		pf_decl_constexpr dbg_u8string &operator=(
			dbg_u8string &&) = default;
		pf_decl_constexpr dbg_u8string &operator=(
			dbg_u8string_view __v) pf_attr_noexcept
		{
			this->count_ = 0;
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

		/// Shrink
		pf_decl_constexpr void shrink(
			size_t __c) pf_attr_noexcept
		{
			this->store_ = union_cast<char_t*>(reallocate(this->store_, this->count_, __c, align_val_t(32), 0));
		}

		/// Begin/End
		pf_hint_nodiscard pf_decl_constexpr pf_decl_inline char_t*
		begin() pf_attr_noexcept
		{
			return &this->store_[0];
		}
		pf_hint_nodiscard pf_decl_constexpr pf_decl_inline const char_t*
		begin() const pf_attr_noexcept
		{
			return &this->store_[0];
		}
		pf_hint_nodiscard pf_decl_constexpr pf_decl_inline char_t*
		end() pf_attr_noexcept
		{
			return &this->store_[0] + this->count_;
		}
		pf_hint_nodiscard pf_decl_constexpr pf_decl_inline const char_t*
		end() const pf_attr_noexcept
		{
			return &this->store_[0] + this->count_;
		}

		/// Operator (View)
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr
		operator dbg_u8string_view() const pf_attr_noexcept
		{
			return this->view();
		}

	private:
		char_t *store_;
		size_t count_;
	};

	// Format
	template <typename ... _Args>
	using dbg_u8string_format = fmt::basic_format_string<char_t, fmt::type_identity_t<_Args>...>;


	/// DEBUG: Category
	class dbg_category
	{
	public:
		/// Constructor
		pf_decl_constexpr dbg_category() pf_attr_noexcept = default;

		/// Name
		pf_decl_virtual dbg_u8string_view
		name() const pf_attr_noexcept = 0;

		/// Message
		pf_decl_virtual dbg_u8string_view
		message(
			uint32_t __val) const pf_attr_noexcept = 0;
	};


	/// DEBUG: Category -> Generic
	class dbg_category_generic_t :
		public dbg_category
	{
	public:
		/// Constructors
		pf_decl_constexpr dbg_category_generic_t() pf_attr_noexcept = default;

		/// Name
		pf_decl_constexpr pf_decl_inline dbg_u8string_view
		name() const pf_attr_noexcept
		{
			return "generic";
		}

		/// Message
		pf_decl_constexpr pf_decl_inline dbg_u8string_view
		message(
			uint32_t __val) const pf_attr_noexcept pf_attr_override
		{
			switch(union_cast<errv>(__val))
			{
			case errv::invalid_argument:  return "Invalid argument";
			case errv::domain_error:      return "Domain error";
			case errv::length_error:      return "Length error";
			case errv::out_of_range:      return "Out of range";
			// TODO: Add missing
			case errv::future_error:      return "Future error";
			case errv::overflow_error:    return "Overflow error";
			case errv::underflow_error:   return "Underflow error";
			case errv::bad_alloc:         return "Bad alloc error";
			default: return "Unknown";
			};
		}
	};

	/// Generic -> Instance
	pf_decl_static pf_decl_inline dbg_category_generic_t dbg_category_generic_instance;

	/// Generic -> Retrieve
	pf_decl_inline pf_decl_constexpr dbg_category*
	dbg_category_generic() pf_attr_noexcept
	{
		return &dbg_category_generic_instance;
	}


	/// DEBUG: Category -> System
	/// System -> Type
	class dbg_category_system_t :
		public dbg_category
	{
	public:
		/// Constructors
		pf_decl_constexpr dbg_category_system_t() pf_attr_noexcept = default;

		/// Name
		pf_decl_constexpr pf_decl_inline dbg_u8string_view
		name() const pf_attr_noexcept
		{
			return "system";
		}

		/// Message
		pf_decl_constexpr pf_decl_inline dbg_u8string_view
		message(
			uint32_t __val) const pf_attr_noexcept pf_attr_override
		{
			switch(__val)
			{
			// TODO: Windows errors
			default: return "Unknown";
			};
		}
	};

	/// System -> Instance
	pf_decl_static pf_decl_inline dbg_category_system_t dbg_category_system_instance;

	/// System -> Retrieve
	pf_decl_inline pf_decl_constexpr dbg_category*
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
			dbg_exception &&) = default;

		/// What
		pf_hint_nodiscard pf_decl_inline const char_t*
		what() const pf_attr_noexcept pf_attr_override
		{
			return this->msg_.begin();
		}
		pf_hint_nodiscard pf_decl_inline const char_t*
		type() const pf_attr_noexcept
		{
			return this->cat_->message(this->code_).begin();
		}

		/// Category
		pf_hint_nodiscard pf_decl_inline const dbg_category*
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

	/// DEBUG: Logger
	class dbg_logger
	{
	public:
		using callback_t = fun_ptr<void (dbg_u8string_view)>;

		/// Constructor
		pf_decl_inline dbg_logger() pf_attr_noexcept
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

		/// Callback
		pf_decl_inline callback_t get_callback()
		{
			return this->c_;
		}
		pf_decl_inline void set_callback(
			callback_t __c)
		{
			this->c_ = __c;
		}

		/// Level
		pf_hint_nodiscard pf_decl_inline dbg_level
		level() const pf_attr_noexcept
		{
			return this->lvl_;
		}
		pf_hint_nodiscard pf_decl_inline dbg_level
		set_level(
			dbg_level __lvl) pf_attr_noexcept
		{
			dbg_level lvl = this->lvl_;
			this->lvl_ = __lvl;
			return lvl;
		}

		/// Timer
		pf_hint_nodiscard pf_decl_inline nanosecond_t
		elapsed_time() const pf_attr_noexcept
		{
			return high_resolution_clock_t::now() - this->timer_;
		}

		/// Write
		pf_decl_inline void write(
			dbg_u8string_view __msg)
		{
			fmt::print("{}", __msg.begin());
			if (this->c_) this->c_(__msg);
		}
		pf_decl_inline void write(
			dbg_level __lvl,
			dbg_u8string_view __msg)
		{
			if (__lvl > this->lvl_)
			{
				this->write(__msg);
			}
		}

	private:
		callback_t c_;
		dbg_level lvl_;
		const high_resolution_point_t timer_;
	};

	// Instance
	pf_decl_extern dbg_logger logger;

	/// DEBUG: Format
	pf_hint_nodiscard pulsar_api char_t*
	__dbg_format_chrono_to(
		char_t *__w) pf_attr_noexcept;
	pf_hint_nodiscard pulsar_api char_t*
	__dbg_format_stacktrace_to(
		char_t *__w,
		size_t __ignore) pf_attr_noexcept;
	pf_hint_nodiscard pulsar_api char_t*
	__dbg_reformat_newlines_to(
		char_t *__w) pf_attr_noexcept;
	pulsar_api void
	dbg_print_exception(
		dbg_category const *__cat,
		uint32_t __code,
		dbg_u8string_view __msg) pf_attr_noexcept;
	template <typename ..._Args>
	pf_decl_static void
	__dbg_print(
		dbg_type __type,
		dbg_level __level,
		dbg_u8string_format<_Args...> __fmt,
		_Args &&... __args)
	{
		// 1. Level
		if (__level < logger.level()) return;

		// 2. Print
		const char_t *lvl;
		switch(__level)
		{
		case dbg_level::low:    { lvl = "LOW";    break; }
		case dbg_level::medium: { lvl = "MEDIUM"; break; }
		case dbg_level::high:   { lvl = "HIGH";   break; }
		};
		dbg_u8string prt(DBG_FMT_BUFFER_SIZE, '\0');
		char_t *p = prt.begin();
		if (__type == dbg_type::extension)
		{
			p = fmt::format_to(p, "                  ");
		}
		else
		{
			p = __dbg_format_chrono_to(p);
			p = fmt::format_to(p, " /{}/ /{}/ message=", union_cast<char_t>(__type), lvl);
		}
		char_t *k = p;
		p			 = fmt::format_to(p, __fmt, std::forward<_Args>(__args)...);
		p			 = __dbg_reformat_newlines_to(k);
		*(p++) = '\n';
		prt.shrink(union_cast<size_t>(p) - union_cast<size_t>(prt.begin()) + 1);
		logger.write(prt);
	}
	template<typename ..._Args>
	pf_hint_nodiscard pf_decl_static dbg_u8string
	__dbg_format_message(	// Message
		dbg_u8string_format<_Args...> __fmt,
		_Args &&... __args) pf_attr_noexcept
	{
		// 1. Format
		dbg_u8string str(fmt::formatted_size(__fmt, std::forward<_Args>(__args)...) + 1, '\0');
		char_t *p = str.begin();
		p = fmt::format_to(p, __fmt, std::forward<_Args>(__args)...);

		// 2. Return
		return str;
	}
	template<typename ..._Args>
	pf_hint_nodiscard pf_decl_static dbg_u8string
	__dbg_format_error_message(	// Assertion
		dbg_u8string_format<_Args...> __fmt,
		_Args &&... __args) pf_attr_noexcept
	{
		// 1. Format
		dbg_u8string str(DBG_FMT_BUFFER_SIZE, '\0');
		char_t *p				= str.begin();
		const char_t *b = p;
		p = __dbg_format_chrono_to(p);
		p = fmt::format_to(p, " T={} /A/ message=", union_cast<size_t>(std::this_thread::get_id()));
		char_t *k = p;
		p	 = fmt::format_to(p, __fmt, std::forward<_Args>(__args)...);
		p	 = __dbg_reformat_newlines_to(k);
		*p = '\n';
		++p;
		p = __dbg_format_stacktrace_to(p, DBG_FMT_STACK_FRAMES_IGNORE);
		str.shrink(union_cast<size_t>(p) - union_cast<size_t>(b) - 1);

		// 2. Return
		return str;
	}

	/// DEBUG: Utility
	pf_hint_noreturn pf_decl_static pf_decl_inline void __dbg_throw(
		dbg_category const *__cat,
		uint32_t __code,
		uint32_t __flags,
		dbg_u8string_view __msg)
	{
		throw(dbg_exception(__cat, __code, __flags, __msg));
	}
	pf_hint_noreturn pf_decl_static pf_decl_inline void __dbg_assert(
		dbg_u8string_view __msg) pf_attr_noexcept
	{
		logger.write(__msg);
		std::exit(SIGABRT);
	}

	/// DEBUG: Initializer
	// Type
	class __dbg_initializer
	{
	public:
		/// Constructors
		pulsar_api __dbg_initializer() pf_attr_noexcept;
		__dbg_initializer(__dbg_initializer const &) = delete;
		__dbg_initializer(__dbg_initializer &&)			 = delete;

		/// Destructor
		pulsar_api ~__dbg_initializer() pf_attr_noexcept;

		/// Operator =
		__dbg_initializer &operator=(__dbg_initializer const&) = delete;
		__dbg_initializer &operator=(__dbg_initializer &&)		 = delete;
	};

	// Instance
	pf_decl_extern __dbg_initializer __dbg_initializer_instance;
}

#define pf_print(type, level, format, ...) pul::__dbg_print(type, level, format __VA_OPT__( ,) __VA_ARGS__)
#define pf_throw(cat, val, bit, format, ...) pul::__dbg_throw(cat, pul::union_cast<uint32_t>(val), bit, pul::__dbg_format_message(format __VA_OPT__( ,) __VA_ARGS__))
#define pf_throw_if(c, cat, val, bit, format, ...) if(pf_unlikely(c)) pf_throw(cat, val, bit, format __VA_OPT__( ,) __VA_ARGS__)
#ifdef PF_DEBUG
#define pf_assert(c, ...) if(pf_unlikely(!c)) pul::__dbg_assert(pul::__dbg_format_error_message(__VA_ARGS__))
#define pf_assert_nodiscard(c, ...) pf_assert(c __VA_OPT__( ,) __VA_ARGS__)
#else
#define pf_assert(c, ...)
#define pf_assert_nodiscard(c, ...) c
#endif
#define pf_assert_static(...) static_assert(__VA_ARGS__)

#endif // !PULSAR_DEBUG_HPP

// TODO: Ring Buffer for Logger ?
