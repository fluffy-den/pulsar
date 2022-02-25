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
		pf_decl_static pf_decl_inline debug_filter FILTER = debug_filter::detail; // All messages are authorized.

		/// Callbacks
		pf_decl_static pf_decl_inline debug_logger::writer_t WRITTER_CLBK = &debug_logger::default_writter;
	};

	/// Set
	pulsar_api debug_logger::writer_t debug_logger::set_writter(
			writer_t &&__fun) pf_attr_noexcept
	{
		writer_t clbk = __debug_logger::WRITTER_CLBK;
		if (!__fun)
			__fun = &default_writter;
		__debug_logger::WRITTER_CLBK = std::forward<writer_t>(__fun);
		return clbk;
	}

	/// Get
	pulsar_api debug_logger::writer_t &debug_logger::get_writter() pf_attr_noexcept
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
		// checks if message isn't filtered (displayable)
		if (__debug_logger::FILTER <= __filter)
			return;
		// reservation
		const size_t headersize = 27;
		std::string msg;
		msg.reserve(headersize + 5
								+ __message.length() + (__message.length() / headersize + 1) * headersize);
		// chrono header
		auto now			= std::chrono::high_resolution_clock::now() - DEBUG_TP;
		const auto h	= std::chrono::duration_cast<std::chrono::hours>(now);
		const auto m	= std::chrono::duration_cast<std::chrono::minutes>(now - h);
		const auto s	= std::chrono::duration_cast<std::chrono::seconds>(now - h - m);
		const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now - h - s);
		// fmt chrono
		strfmt(
				"[%c] - [%.4lli:%.2lli:%.2lli:%.4lli] -> ",
				msg,
				msg.end(),
				__level,
				h.count(),
				m.count(),
				s.count(),
				ms.count());
		// fmt msg
		if (__message.empty())
		{
			msg += "unknown.";
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
		__debug_logger::WRITTER_CLBK(msg);
	}

	/// Default
	pulsar_api void debug_logger::default_writter(
			std::string_view __message) pf_attr_noexcept
	{
		std::puts(__message.data());
	}


	/// Exception
	/// message initializer
	std::string __exception_message(
			std::error_category const &__cat,
			int32_t __code,
			std::string_view __message) pf_attr_noexcept
	{
		std::vector<debug_trace_t> dtl = dbgstacktrace(2);
		std::string msg;
		size_t rsv = __message.length() * 2
							 + dtl.size() * (4 * 1024 + 32);
		rsv += memory::padding_of(rsv, memory::max_align);
		msg.reserve(rsv);
		msg += exception::format(__cat, __code, __message);
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
	// message point to end
	std::string &__add_end_point_to_message(
			std::string &__str) pf_attr_noexcept
	{
		if (__str.back() != '.')
			__str += '.';
		return __str;
	}

	/// Constructors
	pulsar_api exception::exception(
			std::error_category const &__cat,
			int32_t __code,
			std::string_view __message,
			uint32_t __flags) pf_attr_noexcept
			: code_(__code, __cat)
			, message_(__exception_message(__cat, __code, __message))
			, flags_(__flags)
	{
		if ((this->flags_ & debug_flags::write_in_logs) == debug_flags::write_in_logs)
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
				// get the last exception, and process it.
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
				// then generate a dumpbin with exception information
				try
				{
					auto dbp = dbggenbin(__debug::DUMPBIN_PATH, flags);
					dbgpopbox(
							debug_level::error,
							"Pulsar - Framework Library",
							strfmt("Unexpected error! %s\n\nGenerated dumbfile with "
										 "code=%u, at path=%s\n"
										 "The process will now terminate...",
										 msg.c_str(),
										 flags,
										 dbp.string().c_str()));
				}
				catch (exception const &e)
				{
					// ignore to avoid creating an error loop
					std::ignore = e;
					return;
				}
			}
#endif
			// using the standard c abort function
			std::abort();
		};

		// using the standard library for error handling
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