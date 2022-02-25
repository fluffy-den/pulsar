/*! @file   debug.hpp
 *  @author Fluffy (noe.louis-quentin@hotmail.fr)
 *  @brief  Defines all utilities to facilitate debugging of applications using this library.
 *  @date   01-01-2022
 *
 *  @copyright Copyright (c) 2022 - Pulsar Software
 *
 *  @since 0.1
 */

#ifndef PULSAR_DEBUG_HPP
#define PULSAR_DEBUG_HPP 1

// Include: Pulsar
#include "pulsar/memory.hpp"
#include "pulsar/pulsar.hpp"
#include "pulsar/utility.hpp"

// Include: C
#include <cassert>

// Include: C++
#include <chrono>
#include <filesystem>
#include <functional>
#include <string>
#include <string_view>
#include <system_error>
#include <vector>

// Pulsar
namespace pul
{

	/// Types
	class debug;
	class exception;
	class debug_trace_t;
	class debug_logger;

	/*! @brief
	 *
	 */
	namespace debug_flags
	{

		pf_decl_constexpr uint32_t none					 = 0x00000000;
		pf_decl_constexpr uint32_t write_in_logs = 0x00000001; // Write in logs when constructor is called.

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
	struct pf_decl_alignas(memory::max_align) debug_trace_t
	{
		const std::string undname;
		const std::string name;
		const std::string filename;
		const std::string modulename;
		const uint32_t fileline;
	};

	/*! @brief Generates the current call stack trace and ignores the first @a __numToIgnore traces.
	 *
	 *  @param[in] __numToIgnore Number of trace to ignore.
	 *  @return Trace list of the call stack.
	 */
	pulsar_api std::vector<debug_trace_t> dbgstacktrace(
			uint32_t __numToIgnore = 1);

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
	enum class debug_filter
	{
		important,
		helpful,
		detail,
	};

	/// Debug Dumpbin
	/*! @brief Generates a dump file in the @a __p folder, checking the @a __flags. The file created
	 *				 has the date of its creation as its name.
	 *
	 *  @param[in] __p 		 Directory where to create the dump file. If it does not exist, is
	 * 								 		 automatically created.
	 *  @param[in] __flags See the flags for generating a dump file.
	 *  @return Absolute location of the created dump file.
	 */
	pulsar_api std::filesystem::path dbggenbin(
			std::filesystem::path const &__p,
			uint32_t __flags);

	/// Debug message box
	/*! @brief Create a pop-up message.
	 *
	 *  @param[in] __level   Message level.
	 *  @param[in] __title	 Title of the pop-up window.
	 *  @param[in] __message Message of the pop-up window.
	 */
	pulsar_api void dbgpopbox(
			debug_level __level,
			std::string_view __title,
			std::string_view __message);

	/// Debug TP
	pf_decl_static pf_decl_inline const auto DEBUG_TP = std::chrono::high_resolution_clock::now();

	/// Debug Logger
	/*! @brief Defines the log system of the Pulsar Framework.
	 */
	class pulsar_api debug_logger pf_attr_final
	{
		pf_decl_friend class __debug_logger;
		pf_decl_friend class exception;

	public:
		/*! @brief Type of writing function.
		 */
		using writer_t = std::function<void(std::string_view __message)>;

		/// Constructors
		/*! @brief Default constructor.
		 */
		debug_logger()												= delete;
		/*! @brief Copy constructor.
		 *
		 *  @param[in] __r Another instance from which to copy.
		 */
		debug_logger(debug_logger const &__r) = delete;

		/// Operators=
		/*! @brief Copy assignment operator.
		 *
		 *  @param[in] __r Another instance from which to copy.
		 *  @return Reference to this current instance.
		 */
		debug_logger &operator=(debug_logger const &__r) = delete;

		/// Destructor
		/*! @brief Destructor.
		 */
		~debug_logger() pf_attr_noexcept = delete;

		/// Set
		/*! @brief Set the new writing function.
		 *
		 *  @param[in] __fun New writing function.
		 *  @return Old writing function.
		 */
		pf_decl_static writer_t set_writter(
				writer_t &&__fun) pf_attr_noexcept;

		/// Get
		/*! @brief Get the current writing function.
		 *
		 *  @return Reference on the current writting function.
		 */
		pf_hint_nodiscard pf_decl_static writer_t &get_writter() pf_attr_noexcept;

		/// Filter
		/*! @brief Get the he current message filter.
		 *
		 *  @return Current message filter.
		 */
		pf_hint_nodiscard pf_decl_static debug_filter get_filter() pf_attr_noexcept;
		/*! @brief Set the message filter.
		 *
		 *  @param[in] __filter New message filter.
		 *  @return Current message filter.
		 */
		pf_decl_static debug_filter set_filter(
				debug_filter __filter) pf_attr_noexcept;

		/// Write
		/*! @brief Sends the message to the log system.
		 *
		 *  @param[in] __level	 Level of the message.
		 *  @param[in] __filter	 Importance of the message.
		 *  @param[in] __message (optional) Message to write.
		 *  @param[in] __flags	 (optional) @see debug_flags
		 */
		pf_decl_static void write(
				debug_level __level,
				debug_filter __filter,
				std::string_view __message,
				uint32_t __flags = debug_flags::none) pf_attr_noexcept;

		/// Default
		/*! @brief Default writing function of the log system. Sends the message to std::puts.
		 *
		 *  @param[in] __message Formatted message to write.
		 */
		pf_decl_static void default_writter(
				std::string_view __message) pf_attr_noexcept;
	};

	/// Exception
	/*! @brief Class defining unexpected errors.
	 */
	class pulsar_api exception pf_attr_final: public std::exception
	{
	public:
		/// Constructors
		/*! @brief Constructor.
		 *
		 *  @param[in] __cat		 Category of the error.
		 *  @param[in] __code		 Code of the error.
		 *  @param[in] __message (optional) Message explaining the error.
		 *  @param[in] __flags	 (optional) @see debug_flags.
		 */
		exception(
				std::error_category const &__cat,
				int32_t __code,
				std::string_view __message = "",
				uint32_t __flags					 = debug_flags::default_flags) pf_attr_noexcept;
		/*! @brief Copy constructor.
		 *
		 *  @param[in] __r Another instance from wich to copy.
		 */
		exception(exception const &__r) pf_attr_noexcept = delete;

		/// Operator=
		/*! @brief Copy assignment operator.
		 *
		 *  @param[in] __r Another instance from wich to copy.
		 *  @return Reference on this instance.
		 */
		exception &operator=(exception const &__r) pf_attr_noexcept = delete;

		/// Code
		/*! @brief Gets the associated error_code with this exception.
		 *
		 *  @return Constant reference on this associated error_code.
		 */
		pf_hint_nodiscard const std::error_code &code() const pf_attr_noexcept;

		/// What
		/*! @brief Gets the formatted message explaining the exception.
		 *
		 *  @return Constant pointer on this associated formatted message explaining the error.
		 */
		pf_hint_nodiscard const char *what() const pf_attr_noexcept;

		/// Flags
		/*! @brief Gets the current associated flags with this exception.
		 *
		 *  @return @see debug_flags.
		 */
		pf_hint_nodiscard uint32_t get_flags() const pf_attr_noexcept;

		/*! @brief Set the new flags for this exception.
		 *
		 *  @param[in] __flags @see debug_flags.
		 *  @return Old flags of this exception.
		 */
		uint32_t set_flags(uint32_t __flags) pf_attr_noexcept;

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
				std::string_view __message = "") pf_attr_noexcept;

	private:
		std::error_code code_;
		std::string message_;
		uint32_t flags_;
	};

	/*! @brief Converts a generic error code to an integer.
	 *
	 *  @param[in] __code The standard error code.
	 *  @return Converted error code as integer.
	 */
	pf_hint_nodiscard pf_decl_constexpr pf_decl_static int32_t dbgerrc(
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

	/// Debug
	class pulsar_api debug pf_attr_final
	{
		pf_decl_friend class __debug;

		/// Constructors
		/*! @brief Default constructor.
		 */
		debug() pf_attr_noexcept;
		/*! @brief Copy constructor.
		 *
		 *  @param[in] __r Another instance to copy from.
		 */
		debug(debug const &__r) pf_attr_noexcept = delete;

		/// Destructor
		/*! @brief Destructor.
		 */
		~debug() pf_attr_noexcept;

		/// Operator=
		/*! @brief Assignment operator.
		 *
		 *  @param[in] __r Another instance to copy from.
		 *  @return Reference on this instance.
		 */
		debug &operator=(debug const &__r) pf_attr_noexcept = delete;

	public:
		/// Dumpbin
		/*! @brief Returns the path to the directory where the dump files are generated.
		 *				 generation.
		 *
		 *  @return Reference on the path.
		 */
		pf_hint_nodiscard pf_decl_static std::filesystem::path &get_dumpfile_creation_path() pf_attr_noexcept;
		/*! @brief Changes the path of the directory where the dump files are generated.
		 *
		 *  @param[in] __p New directory.
		 *  @return Old path.
		 */
		pf_decl_static std::filesystem::path set_dumpfile_creation_path(
				std::filesystem::path &&__p) pf_attr_noexcept;
	};
}

#endif // !PULSAR_DEBUG_HPP