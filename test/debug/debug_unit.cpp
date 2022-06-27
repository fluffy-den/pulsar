/*! @file 	debug_unit.cpp
 *  @author Fluffy (noe.louis-quentin@hotmail.fr)
 *  @brief	Test unit for the debugging tools.
 *  @date 	02-03-2022
 *
 * 	@copyright Copyright (c) 2022 - Pulsar Software
 *
 *  @since 0.1.0
 */

// Include: Pulsar
#include "pulsar/debug.hpp"

// Include: Catch2
#include "catch2/catch_all.hpp"

// Pulsar
namespace pul
{
	// Debug Logger
	TEST_CASE("DebugLoggerInfoTest", "[debug]")
	{
		debugger::log(
				debug_level::info,
				debug_filter::important,
				"This is an info message...");
	}
	TEST_CASE("DebugLoggerWarningTest", "[debug]")
	{
		debugger::log(
				debug_level::warning,
				debug_filter::important,
				"This is a warning message...");
	}
	TEST_CASE("DebugLoggerErrorTest", "[debug]")
	{
		debugger::log(
				debug_level::error,
				debug_filter::important,
				"This is an error message...");
	}

	// Debug Message Box
	TEST_CASE("DebugMessageBoxInfo", "[debug]")
	{
		debugger::generate_messagebox(
				debug_level::info,
				"Info message box",
				"This is an info message box.");
	}
	TEST_CASE("DebugMessageBoxWarning", "[debug]")
	{
		debugger::generate_messagebox(
				debug_level::warning,
				"Warning message box",
				"This is a warning message box.");
	}
	TEST_CASE("DebugMessageBoxError", "[debug]")
	{
		debugger::generate_messagebox(
				debug_level::error,
				"Error message box",
				"This is an error message box.");
	}

	// Debug StackTrace
	TEST_CASE("DebugStacktraceTest", "[debug]")
	{
		auto stl = debugger::stacktrace(0);
		REQUIRE(!stl.empty());
	}
}