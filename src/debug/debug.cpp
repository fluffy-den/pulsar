/*! @file   debug.cpp
 *  @author Fluffy (noe.louis-quentin@hotmail.fr)
 *  @brief
 *  @date   22-02-2022
 *
 *  @copyright Copyright (c) 2022 - Pulsar Software
 *
 *  @since 0.1.0
 */

// Include: Pulsar
#include "pulsar/debug.hpp"

// Pulsar
namespace pul
{

	/// Debug Logger
	/// Internal
	class __debug_logger
	{
		pf_decl_friend class debug_logger;

		/// Filter
		pf_decl_static pf_decl_inline debug_filter FILTER = debug_filter::detail; // All message are authorized

		/// Callbacks
		pf_decl_static pf_decl_inline debug_logger::writter_t WRITTER_CLBK		 = &debug_logger::default_writter;
		pf_decl_static pf_decl_inline debug_logger::formatter_t FORMATTER_CLBK = &debug_logger::default_format;
	};

	/// Set
	pulsar_api debug_logger::formatter_t debug_logger::set_formatter(
			formatter_t &&__fun) pf_attr_noexcept
	{
		formatter_t clbk = __debug_logger::FORMATTER_CLBK;
		if (!__fun)
			__fun = &default_format;
		__debug_logger::FORMATTER_CLBK = std::forward<formatter_t>(__fun);
		return clbk;
	}
	pulsar_api debug_logger::writter_t debug_logger::set_writer(
			writter_t &&__fun) pf_attr_noexcept
	{
		writter_t clbk = __debug_logger::WRITTER_CLBK;
		if (!__fun)
			__fun = &default_writter;
		__debug_logger::WRITTER_CLBK = std::forward<writter_t>(__fun);
		return clbk;
	}

	/// Get
	pulsar_api debug_logger::formatter_t &debug_logger::get_formatter() pf_attr_noexcept // TODO: Remove formatter.
	{
		return __debug_logger::FORMATTER_CLBK;
	}
	pulsar_api debug_logger::writter_t &debug_logger::get_writer() pf_attr_noexcept
	{
		return __debug_logger::WRITTER_CLBK;
	}

	/// Filter
	pulsar_api debug_filter debug_logger::get_filter() pf_attr_noexcept
	{
		return __debug_logger::FILTER;
	}
	pulsar_api debug_filter debug_logger::set_filter(
			debug_filter __filter) pf_attr_noexcept
	{
		debug_filter cpy			 = __debug_logger::FILTER;
		__debug_logger::FILTER = __filter;
		return cpy;
	}

	/// Write
	pulsar_api void debug_logger::write(
			debug_level __level,
			debug_filter __filter,
			std::string_view __message,
			uint32_t __flags) pf_attr_noexcept
	{
		if (__debug_logger::FILTER <= __filter) return;

		std::vector<debug_trace_t> stL;

		if (__flags & debug_flags::write_in_logs_stacktrace) stL = debug_stacktrace(1);

		__debug_logger::WRITTER_CLBK(__debug_logger::FORMATTER_CLBK(
				__level,
				std::chrono::high_resolution_clock::now() - DEBUG_TP,
				stL,
				__message));

		// TODO: Multithreading writting, buffering, filtering.
	}

	/// Default
	pulsar_api std::string debug_logger::default_format(
			debug_level __level,
			std::chrono::nanoseconds __when,
			std::vector<debug_trace_t> const &__trace,
			std::string_view __message) pf_attr_noexcept
	{
		const auto h	= std::chrono::duration_cast<std::chrono::hours>(__when);
		const auto m	= std::chrono::duration_cast<std::chrono::minutes>(__when - h);
		const auto s	= std::chrono::duration_cast<std::chrono::seconds>(__when - h - m);
		const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(__when - h - s);

		// headerfmt
		std::string msg = strfmt("[%c] - [%.4lli:%.2lli:%.2lli:%.4lli] -> ",
														 __level,
														 h.count(),
														 m.count(),
														 s.count(),
														 ms.count());

		const size_t headersize = msg.length();

		// reserve
		msg.reserve(__message.length()
								+ memory::padding_of(__message.length(), memory::max_align)
								+ __trace.size() * (1024 + 32 + 4 + memory::padding_of(1024 + 32 + 4, memory::max_align) * 3));

		// __message
		if (__message.empty())
		{
			msg += "unknown.";
		}
		else
		{
			__message.data();
			for (auto i : __message)
			{
				msg += i;
				if (i == '\n')
				{
					msg.append(headersize, ' ');
				}
			}
		}
		msg += '\n';
		msg.append(headersize, ' ');

		// __trace fmt
		for (auto &i : __trace)
		{
			// at file=pulsar/bla.hpp:525 in module=module, func=int sum(int a, int b) noexcept

			// space + at file=
			msg += "at file=";
			// filename
			msg += i.filename;
			// line
			if (i.fileline != 0)
			{
				msg += ':';
				msg += std::to_string(i.fileline);
			}
			// " in module="
			msg += " in module=";
			// module
			msg += i.modulename;
			// ", func="
			msg += ", func=";
			msg += i.undname;
			msg += '\n';
			msg.append(headersize, ' ');
		}

		strtriml(msg);
		strtrimr(msg);

		// end
		return msg;
	}
	/*! @brief
	 *
	 *  @param[in] __message
	 *  @return pf_decl_static
	 */
	pulsar_api void debug_logger::default_writter(
			std::string_view __message) pf_attr_noexcept
	{
		std::puts(__message.data());
	}


	/// Exception
	/// Constructors
	pulsar_api exception::exception(
			std::error_category const &__cat,
			int32_t __code,
			std::string_view __message,
			uint32_t __flags) pf_attr_noexcept
			: code_(__code, __cat)
			, message_(format(__cat, __code, __message))
			, flags_(__flags)
	{
		if (this->message_.back() != '.')
		{
			this->message_ += '.';
		}
		if (this->flags_ & debug_flags::write_in_logs)
		{
			debug_logger::write(debug_level::error,
													debug_filter::important,
													this->message_,
													this->flags_);
		}
	}

	/// Code
	pulsar_api pf_hint_nodiscard const std::error_code &exception::code() const pf_attr_noexcept
	{
		return this->code_;
	}

	/// What
	pulsar_api pf_hint_nodiscard const char *exception::what() const pf_attr_noexcept
	{
		return this->message_.c_str();
	}

	/// Flags
	pulsar_api pf_hint_nodiscard uint32_t exception::get_flags() const pf_attr_noexcept
	{
		return this->flags_;
	}
	pulsar_api uint32_t exception::set_flags(
			uint32_t __flags) pf_attr_noexcept
	{
		uint32_t flags = this->flags_;
		this->flags_	 = __flags;
		return flags;
	}

	/// Formatters
	pulsar_api pf_hint_nodiscard std::string exception::format(
			std::error_category const &__cat,
			int32_t __code,
			std::string_view __message) pf_attr_noexcept
	{
		return strfmt(__message.empty() ? "category=%s, code=%i, message=%s" : "category=%s, code=%i, message=%s, additional=%s",
									__cat.name(),
									__code,
									__cat.message(__code).c_str(),
									__message.data());
	}
	pulsar_api pf_hint_nodiscard std::string exception::system_format(
			int32_t __code,
			std::string_view __message) pf_attr_noexcept
	{
		return format(std::system_category(), __code, __message);
	}
	pulsar_api pf_hint_nodiscard std::string exception::generic_format(
			std::errc __code,
			std::string_view __message) pf_attr_noexcept
	{
		return format(std::generic_category(), debug_errc(__code), __message);
	}

	/// Makers
	pulsar_api pf_hint_nodiscard exception exception::system_error(
			int32_t __code,
			std::string_view __message,
			uint32_t __flags) pf_attr_noexcept
	{
		return exception(std::system_category(), __code, __message, __flags);
	}
	pulsar_api pf_hint_nodiscard exception exception::generic_error(
			int32_t __code,
			std::string_view __message,
			uint32_t __flags) pf_attr_noexcept
	{
		return exception(std::generic_category(), __code, __message, __flags);
	}

	/// Debug
	/// Internal
	class __debug
	{
		pf_decl_friend class debug;

		/// Instance
		pf_decl_static pf_decl_inline debug INSTANCE;

		/// Dumpbin Path
		pf_decl_static pf_decl_inline auto DUMPBIN_PATH = std::filesystem::current_path();
	};

	/// Constructor
	pulsar_api debug::debug() pf_attr_noexcept
	{
		auto lbd = []()
		{
#ifndef PF_DEBUG
			std::exception_ptr ep = std::current_exception();

			if (ep)
			{
				std::string cat, msg;
				uint32_t flags = 0;

				try
				{
					std::rethrow_exception(ep);
				}
				catch (exception const &e)
				{
					msg		= e.what();
					flags = e.get_flags();
				}
				catch (std::exception const &e)
				{
					msg		= "unknown.";
					flags = debug_flags::generate_dumpfile;
				}

				auto dbp = debug_dumpbin(__debug::DUMPBIN_PATH, flags);

				debug_messagebox(
						debug_level::error,
						"Pulsar - Framework Library",
						strfmt("Unexpected error! %s\n\nGenerated dumbfile with "
									 "code=%u, at path=%s\n"
									 "The process will now terminate...",
									 msg.c_str(),
									 flags,
									 dbp.string().c_str()));
			}
#endif

			std::abort();
		};

		std::set_terminate(lbd);
	}

	/// Destructor
	pulsar_api debug::~debug() = default;

	/// Dumpbin
	pulsar_api pf_hint_nodiscard std::filesystem::path &debug::get_dumpfile_creation_path() pf_attr_noexcept
	{
		return __debug::DUMPBIN_PATH;
	}
	pulsar_api std::filesystem::path debug::set_dumpfile_creation_path(
			std::filesystem::path &&__p) pf_attr_noexcept
	{
		std::filesystem::path cpy = __debug::DUMPBIN_PATH;
		__debug::DUMPBIN_PATH			= std::forward<std::filesystem::path>(__p);
		return cpy;
	}
}