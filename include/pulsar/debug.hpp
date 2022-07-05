/*! @file   debugger.hpp
 *  @author Fluffy (noe.louis-quentin@hotmail.fr)
 *  @brief
 *  @date   27-06-2022
 *
 *  @copyright Copyright (c) 2022 - Pulsar Software
 *
 *  @since 0.1.1
 */

#ifndef PULSAR_DEBUG_HPP
#define PULSAR_DEBUG_HPP 1

// Include: Pulsar
#include "pulsar/function.hpp"
#include "pulsar/memory.hpp"
#include "pulsar/pulsar.hpp"
#include "pulsar/static_initializer.hpp"

// Include: Pulsar -> Thread ID
#include "pulsar/thread/thread_identifier.hpp"

// Include: Pulsar -> String
#include "pulsar/string/string.hpp"

// Include: C++
#include <chrono>
#include <filesystem>
#include <vector>

// Pulsar
namespace pul
{
	/*! @brief
	 *
	 */
	namespace debug_flags
	{

		pf_decl_constexpr uint32_t none					 = 0x00000000;
		pf_decl_constexpr uint32_t write_in_logs = 0x00000001; // Write in logs when exception is called.

		// ! Dumpfile is only generated in release mode !
		pf_decl_constexpr uint32_t generate_dumpfile											= 0x00000002;											// Generate dump file with default data.
		pf_decl_constexpr uint32_t dump_with_data_segs										= generate_dumpfile | 0x00000004; // Dump with data sections of all modules (global variables).
		pf_decl_constexpr uint32_t dump_with_full_memory									= generate_dumpfile | 0x00000008; // Dump with all accessible memory of the process.
		pf_decl_constexpr uint32_t dump_with_handle_data									= generate_dumpfile | 0x00000010; // Dump with high-level information of the OS.
		pf_decl_constexpr uint32_t dump_with_filter_memory								= generate_dumpfile | 0x00000020; // Dump with stack and backing store memory filtered.
		pf_decl_constexpr uint32_t dump_with_scan_memory									= generate_dumpfile | 0x00000040; // Dump with stack and backing store memory scanned for reference on modules.
		pf_decl_constexpr uint32_t dump_with_unloaded_modules							= generate_dumpfile | 0x00000080; // Dump with unloaded modules information.
		pf_decl_constexpr uint32_t dump_with_indirectly_referenced_memory = generate_dumpfile | 0x00000100; // Dump with pages referenced by locals or other stack memory.
		pf_decl_constexpr uint32_t dump_with_filter_module_paths					= generate_dumpfile | 0x00000200; // Dump with module path filtering such as user names or important directories.
		pf_decl_constexpr uint32_t dump_with_process_thread_data					= generate_dumpfile | 0x00000400; // Dump with complete per-process and per-thread information from OS.
		pf_decl_constexpr uint32_t dump_with_private_readwrite_memory			= generate_dumpfile | 0x00000800; // Dump with scan of virtual address space with read and write access.
		pf_decl_constexpr uint32_t dump_without_auxiliary_state						= generate_dumpfile | 0x00001000; // Dump without auxiliary-supported memory gathering.
		pf_decl_constexpr uint32_t dump_with_full_auxiliary_state					= generate_dumpfile | 0x00002000; // Dump with full auxiliary data.
		pf_decl_constexpr uint32_t dump_with_private_write								= generate_dumpfile | 0x00004000; // Dump with virtual address with write only access.
		pf_decl_constexpr uint32_t dump_without_inaccessible_memory				= generate_dumpfile | 0x00008000; // Dump without inaccessible memory.
		pf_decl_constexpr uint32_t dump_with_token_information						= generate_dumpfile | 0x00010000; // Dump with security tokens for user data.
		pf_decl_constexpr uint32_t dump_with_module_headers								= generate_dumpfile | 0x00020000; // Dump with module header related data.
		pf_decl_constexpr uint32_t dump_with_filter_triage								= generate_dumpfile | 0x00040000; // Dump with filter triage related data.
		pf_decl_constexpr uint32_t dump_with_avx_state_context						= generate_dumpfile | 0x00080000; // Dump with avx state context.
		pf_decl_constexpr uint32_t dump_with_ipt_trace										= generate_dumpfile | 0x00100000; // Dump with Intel processor trace related data.
		pf_decl_constexpr uint32_t dump_with_inaccessible_partial_pages		= generate_dumpfile | 0x00200000; // Dump with inaccessible partial pages.
		pf_decl_constexpr uint32_t dump_with_valid_type_flags							= generate_dumpfile | 0x00400000; // Dump with valid type flags.
		// ! Dumpfile is only generated in release mode !

		pf_decl_constexpr uint32_t default_flags = write_in_logs
																						 | generate_dumpfile;
	}

	/// Debug Trace
	/*! @brief Structure containing the trace of a symbol.
	 */
	struct debug_trace_t
	{
		const std::string undname;
		const std::string name;
		const std::string filename;
		const std::string modulename;
		const uint32_t fileline;
	};

	/// Debug Trace -> List
	using debug_trace_list_t = std::vector<debug_trace_t>;

	/// Levels
	/*! @brief Structure listing the levels of the messages.
	 */
	enum class debug_level : char
	{
		info		= 'I',
		warning = 'W',
		error		= 'E'
	};

	/// Filter
	/*! @brief Structure listing the message filters.
	 */
	enum class debug_filter : byte_t
	{
		important,
		helpful,
		detail,
	};

	/// Debugger
	class debugger
	{
		pf_static_initializer_allow(debugger);

		/// Internal
		struct __internal
		{
			/// Constructor
			__internal() pf_attr_noexcept
					: loggerDefaultReceiver_(__default_writter)
					, dumpbinpath_(std::filesystem::current_path())
					, loggerStart_(std::chrono::high_resolution_clock::now())
					, loggerFilter_(debug_filter::detail)
			{
				// Transmitter
				this->loggerTransmitter_.add_signal(this->loggerDefaultReceiver_);
				// Exception not handled function
				auto lbd = []() pf_attr_noexcept
				{
					std::exception_ptr ep = std::current_exception();
					if (ep)
					{
						std::string cat, msg;
						uint32_t flags = 0;
						// get the last exception, and process it.
						try
						{
							std::rethrow_exception(ep);
						}
						catch (__exception const &e)
						{
							msg		= e.what();
							flags = e.get_flags();
						}
						catch (std::exception const &e)
						{
							msg		= e.what();
							flags = debug_flags::generate_dumpfile;
						}
						// then generate a dumpbin with exception information
						try
						{
							auto dbp = generate_dumpbin(instance_->dumpbinpath_, flags);
							generate_messagebox(
									debug_level::error,
									"Pulsar - Framework",
									strfmt("Unexpected error! %s\n\nGenerated dumbfile"
												 "code=%u, at path=%s\n"
												 "Press ok to terminate the process...",
												 msg.c_str(),
												 flags,
												 dbp.string().c_str()));
						}
						catch (__exception const &e)
						{
							// ignore to avoid creating an error loop
							std::ignore = e;
							return;
						}
					}
					// using the standard c abort function
					std::abort();
				};
				// using the standard library for error handling
				std::set_terminate(lbd);
			}

			/// Writter -> Default
			pf_decl_static void __default_writter(
					std::string_view __message) pf_attr_noexcept
			{
				std::puts(__message.data());
			}

			/// Writter -> Write
			void __log_write(
					debug_level __level,
					debug_filter __filter,
					std::string_view __message = "")
			{
				// checks if message isn't filtered (displayable)
				if (this->loggerFilter_ <= __filter)
					return;
				// reservation
				const size_t headersize = 27;
				std::string msg;
				msg.reserve(headersize + 5
										+ __message.length() + (__message.length() / headersize + 1) * headersize);
				// chrono header
				auto now			= std::chrono::high_resolution_clock::now() - this->loggerStart_;
				const auto h	= std::chrono::duration_cast<std::chrono::hours>(now);
				const auto m	= std::chrono::duration_cast<std::chrono::minutes>(now - h);
				const auto s	= std::chrono::duration_cast<std::chrono::seconds>(now - h - m);
				const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now - h - m - s);
				// fmt chrono
				strfmt(
						"[%c] - [%.4lli:%.2lli:%.2lli:%.4lli] - T%.2zu -> ",
						msg,
						msg.end(),
						__level,
						h.count(),
						m.count(),
						s.count(),
						ms.count(),
						this_thread::ID());
				// fmt msg
				if (__message.empty())
				{
					msg = "No message.";
				}
				else
				{
					for (auto i : __message)
					{
						msg += i;
						if (i == '\n')
						{
							msg.append(headersize, ' ');
						}
					}
				}
				// write
				this->loggerTransmitter_(msg);
			}

			fun_receiver<void(std::string_view)> loggerDefaultReceiver_;
			fun_transmitter<void(std::string_view)> loggerTransmitter_;
			std::filesystem::path dumpbinpath_;
			std::chrono::high_resolution_clock::time_point loggerStart_;
			debug_filter loggerFilter_;
		};

		/// Exception
		/*! @brief Class defining unexpected errors.
		 */
		class __exception pf_attr_final: public std::exception
		{
			/// Message -> Initializer
			pf_decl_static std::string __exception_message(
					std::error_category const &__cat,
					int32_t __code,
					std::string_view __message) pf_attr_noexcept
			{
				std::vector<debug_trace_t> dtl = debugger::stacktrace(2);
				std::string msg;
				size_t rsv = __message.length() * 2
									 + dtl.size() * (4 * 1024 + 32);
				rsv += memory::padding_of(rsv, memory::MAX_ALIGN);
				msg.reserve(rsv);
				msg += __exception::format(__cat, __code, __message);
				msg += '\n';
				for (auto &tr : dtl)
				{
					msg += "at file=";
					msg += tr.filename;
					if (tr.fileline != 0)
					{
						msg += ':';
						msg += std::to_string(tr.fileline);
					}
					msg += " in module=";
					msg += tr.modulename;
					msg += ", func=";
					msg += tr.undname;
					msg += '\n';
				}
				return msg;
			}

			// Message -> Add end point
			pf_decl_static std::string &__add_end_point_to_message(
					std::string &__str) pf_attr_noexcept
			{
				if (__str.back() != '.')
					__str += '.';
				return __str;
			}

		public:
			/// Constructors
			/*! @brief Constructor.
			 *
			 *  @param[in] __cat		 Category of the error.
			 *  @param[in] __code		 Code of the error.
			 *  @param[in] __message (optional) Message explaining the error.
			 *  @param[in] __flags	 (optional) @see debug_flags.
			 */
			__exception(
					std::error_category const &__cat,
					int32_t __code,
					std::string_view __message = "",
					uint32_t __flags					 = debug_flags::default_flags) pf_attr_noexcept
					: code_(__code, __cat)
					, message_(__exception_message(__cat, __code, __message))
					, flags_(__flags)
			{
				if ((this->flags_ & debug_flags::write_in_logs) == debug_flags::write_in_logs)
				{
					debugger::log(
							debug_level::error,
							debug_filter::important,
							this->message_);
				}
			}
			/*! @brief Copy constructor.
			 *
			 *  @param[in] __r Another instance from wich to copy.
			 */
			__exception(__exception const &__r) pf_attr_noexcept = default;

			/// Operator=
			/*! @brief Copy assignment operator.
			 *
			 *  @param[in] __r Another instance from wich to copy.
			 *  @return Reference on this instance.
			 */
			__exception &operator=(__exception const &__r) pf_attr_noexcept = default;

			/// Code
			/*! @brief Gets the associated error_code with this exception.
			 *
			 *  @return Constant reference on this associated error_code.
			 */
			pf_hint_nodiscard const std::error_code &code() const pf_attr_noexcept
			{
				return this->code_;
			}

			/// What
			/*! @brief Gets the formatted message explaining the exception.
			 *
			 *  @return Constant pointer on this associated formatted message explaining the error.
			 */
			pf_hint_nodiscard const char *what() const pf_attr_noexcept pf_attr_override
			{
				return this->message_.c_str();
			}

			/// Flags
			/*! @brief Gets the current associated flags with this exception.
			 *
			 *  @return @see debug_flags.
			 */
			pf_hint_nodiscard uint32_t get_flags() const pf_attr_noexcept
			{
				return this->flags_;
			}

			/*! @brief Set the new flags for this exception.
			 *
			 *  @param[in] __flags @see debug_flags.
			 *  @return Old flags of this exception.
			 */
			void set_flags(
					uint32_t __flags) pf_attr_noexcept
			{
				this->flags_ = __flags;
			}

			/// Formatters
			/*! @brief Generates a formatted message of an error.
			 *
			 *  @param[in] __cat	   Category of the error.
			 *  @param[in] __code		 Code of the error.
			 *  @param[in] __message (optional) Message explaining the error.
			 *  @return Formatted error message.
			 */
			pf_hint_nodiscard pf_decl_static std::string format(
					std::error_category const &__cat,
					int32_t __code,
					std::string_view __message = "") pf_attr_noexcept
			{
				std::string catmsg = __cat.message(__code).c_str();
				__add_end_point_to_message(catmsg);
				strtriml(catmsg);
				strtrimr(catmsg);
				std::string fmtmsg;
				fmtmsg.resize(__message.size());
				__message.copy(fmtmsg.data(), fmtmsg.size());
				strtriml(fmtmsg);
				strtrimr(fmtmsg);
				__add_end_point_to_message(fmtmsg);
				return strfmt(
						__message.empty()
								? "category=%s, code=%i, message=%s"
								: "category=%s, code=%i, message=%s %s",
						__cat.name(),
						__code,
						catmsg.data(),
						fmtmsg.data());
			}

		private:
			std::error_code code_;
			std::string message_;
			uint32_t flags_;
		};

		/// Initializers
		pf_decl_static void __init()
		{
			instance_ = std::make_unique<__internal>();
		}
		pf_decl_static void __terminate()
		{
			instance_.reset();
		}

	public:
		using exception = __exception;

		/// Constructors
		debugger() = delete;

		/// StackTrace -> Get List
		/*! @brief Generates the current call stack trace and ignores the first @a __numToIgnore traces.
		 *
		 *  @param[in] __numToIgnore Number of trace to ignore.
		 *  @return Trace list of the call stack.
		 */
		pf_hint_nodiscard pulsar_api pf_decl_static debug_trace_list_t stacktrace(
				uint32_t __numToIgnore = 1);

		/// Writter -> Signals
		pf_decl_static void add_log_receiver(fun_receiver<void(std::string_view)> &__wr)
				pf_attr_noexcept
		{
			instance_->loggerTransmitter_.add_signal(__wr);
		}
		pf_decl_static void rem_log_receiver(
				fun_receiver<void(std::string_view)> &__wr) pf_attr_noexcept
		{
			instance_->loggerTransmitter_.rem_signal(__wr);
		}

		/// Writter -> Filters
		pf_hint_nodiscard pf_decl_static debug_filter log_filter() pf_attr_noexcept
		{
			return instance_->loggerFilter_;
		}
		pf_decl_static void set_log_filter(
				debug_filter __f) pf_attr_noexcept
		{
			instance_->loggerFilter_ = __f;
		}

		/// Writter -> Write
		pf_decl_static void log(
				debug_level __level,
				debug_filter __filter,
				std::string_view __message)
		{
			instance_->__log_write(__level, __filter, __message);
		}

		/// Generic -> Code Cast
		/*! @brief Converts a generic error code to an integer.
		 *
		 *  @param[in] __code The standard error code.
		 *  @return Converted error code as integer.
		 */
		pf_hint_nodiscard pf_decl_static pf_decl_constexpr int32_t generic_code(
				std::errc __code) pf_attr_noexcept
		{
			union
			{
				std::errc as_code_t;
				std::int32_t as_int32_t;
			};
			as_code_t = __code;
			return as_int32_t;
		}

		/// Dumpbin -> Path
		/*! @brief Returns the path to the directory where the dump files are generated.
		 *				 generation.
		 *
		 *  @return Reference on the path.
		 */
		pf_hint_nodiscard pf_decl_static std::filesystem::path &dumpfile_creation_path() pf_attr_noexcept
		{
			return instance_->dumpbinpath_;
		}
		/*! @brief Changes the path of the directory where the dump files are generated.
		 *
		 *  @param[in] __p New directory.
		 *  @return Old path.
		 */
		pf_decl_static void set_dumpfile_creation_path(
				std::filesystem::path &&__p) pf_attr_noexcept
		{
			instance_->dumpbinpath_ = std::move(__p);
		}

		/// Dumpbin -> Generate
		pf_hint_nodiscard pulsar_api pf_decl_static std::filesystem::path generate_dumpbin(
				std::filesystem::path const &__p,
				uint32_t __flags);

		/// Message -> Box
		pulsar_api pf_decl_static void generate_messagebox(
				debug_level __level,
				std::string_view __title,
				std::string_view __message);

		/// Initialized
		pf_decl_static bool is_initialized() pf_attr_noexcept
		{
			return instance_ != nullptr;
		}

	private:
		pf_decl_static pf_decl_inline std::unique_ptr<__internal> instance_;
		pf_static_initializer(debugger)
	};

	/// DEBUG: Exception
	using exception = debugger::exception;
}

// Include: Pulsar -> Thread
#include "pulsar/thread.hpp"

#endif // !PULSAR_DEBUG_HPP