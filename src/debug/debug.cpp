/*! @file   debug.cpp
 *  @author Fluffy (noe.louis-quentin@hotmail.fr)
 *  @brief	Implements debug objects.
 *  @date   15-07-2022
 *
 *  @copyright Copyright (c) 2022 - Pulsar Software
 *
 *  @since 0.1.2
 */

// Include: Pulsar
#include "pulsar/debug.hpp"

// Include: Src
#include "debug/debug.hpp"

// Pulsar
namespace pul
{
	/// DEBUG: Debugger
	__debugger_internal::__debugger_internal() pf_attr_noexcept
			: loggerDefaultReceiver_(__default_writter)
			, dumpbinpath_(std::filesystem::current_path())
			, loggerStart_(std::chrono::high_resolution_clock::now())
			, loggerFilter_(debug_filter::detail)
	{
		// Transmitter
		this->loggerSignal_.add_signal(this->loggerDefaultReceiver_);
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
				catch (exception const &e)
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
					auto dbp = debugger::generate_dumpbin(instance_->dumpbinpath_, flags);
					debugger::generate_messagebox(
							debug_level::error,
							"Pulsar - Framework",
							strfmt("Unexpected error! %s\n\nGenerated dumbfile"
										 "code=%u, at path=%s\n"
										 "Press ok to terminate the process...",
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
			// using the standard c abort function
			std::abort();
		};
		// using the standard library for error handling
		std::set_terminate(lbd);
	}

	/// Writter -> Default
	void __debugger_internal::__default_writter(
			std::string_view __message) pf_attr_noexcept
	{
		std::puts(__message.data());
	}

	/// Writter -> Write
	void __debugger_internal::__log_write(
			debug_level __level,
			debug_filter __filter,
			std::string_view __message)
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
		this->loggerSignal_(msg);
	}

	/// Exception -> Message
	std::string debugger::exception::__exception_message(
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

	// Exception -> Add End Point to Message
	std::string &exception::__add_end_point_to_message(
			std::string &__str) pf_attr_noexcept
	{
		if (__str.back() != '.')
			__str += '.';
		return __str;
	}

	// Exception -> Constructor
	exception::exception(
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
			debugger::log(
					debug_level::error,
					debug_filter::important,
					this->message_);
		}
	}

	// Exception -> Code
	pf_hint_nodiscard const std::error_code &exception::code() const pf_attr_noexcept
	{
		return this->code_;
	}

	// Exception -> What
	pf_hint_nodiscard const char *exception::what() const pf_attr_noexcept
	{
		return this->message_.c_str();
	}

	// Exception -> Flags
	pf_hint_nodiscard uint32_t exception::get_flags() const pf_attr_noexcept
	{
		return this->flags_;
	}

	void exception::set_flags(
			uint32_t __flags) pf_attr_noexcept
	{
		this->flags_ = __flags;
	}

	// Exception -> Formatters
	pf_hint_nodiscard std::string exception::format(
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

	/// Debugger -> Constructors
	debugger::debugger()
	{
		if (__debugger_internal::instance_)
			return;
		__debugger_internal::instance_ = std::make_unique<__debugger_internal>();
	}

	/// Debugger -> Destructor
	debugger::~debugger()
	{
		__debugger_internal::instance_.reset();
	}

	/// Debugger -> Log Signals
	void debugger::add_log_signal(
			shared_isignal<void(std::string_view)> &__wr)
			pf_attr_noexcept
	{
		__debugger_internal::instance_->loggerSignal_.add_signal(__wr);
	}
	void debugger::rem_log_signal(
			shared_isignal<void(std::string_view)> &__wr) pf_attr_noexcept
	{
		__debugger_internal::instance_->loggerSignal_.rem_signal(__wr);
	}

	/// Writter -> Filters
	pf_hint_nodiscard debug_filter debugger::log_filter() pf_attr_noexcept
	{
		return __debugger_internal::instance_->loggerFilter_;
	}
	void debugger::set_log_filter(
			debug_filter __f) pf_attr_noexcept
	{
		__debugger_internal::instance_->loggerFilter_ = __f;
	}

	/// Debugger -> Log
	void debugger::log(
			debug_level __level,
			debug_filter __filter,
			std::string_view __message)
	{
		__debugger_internal::instance_->__log_write(__level, __filter, __message);
	}

	/// Debugger -> Dumpbin Path
	pf_hint_nodiscard std::filesystem::path &debugger::dumpfile_creation_path() pf_attr_noexcept
	{
		return __debugger_internal::instance_->dumpbinpath_;
	}
	void debugger::set_dumpfile_creation_path(
			std::filesystem::path &&__p) pf_attr_noexcept
	{
		__debugger_internal::instance_->dumpbinpath_ = std::move(__p);
	}

	/// Debugger -> Initializer
	const debugger __debugger;
}