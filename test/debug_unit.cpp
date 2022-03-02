/*! @file 	debug_unit.cpp
 *  @author Fluffy (noe.louis-quentin@hotmail.fr)
 *  @brief	Unit test of the debugging tools.
 *  @date 	02-03-2022
 *
 * 	@copyright Copyright (c) 2022 - Pulsar Software
 *
 *  @since 0.1.0
 */

// Include: Pulsar
#include "pulsar/debug.hpp"

// Include: Google Test
#include "gtest/gtest.h"

/// Debug Logger
GTEST_TEST(DebugLoggerTest, DebugLoggerInfoTest)
{
	pul::debug_logger::write(
			pul::debug_level::info,
			pul::debug_filter::important,
			"This is an info message...");
}
GTEST_TEST(DebugLoggerTest, DebugLoggerWarningTest)
{
	pul::debug_logger::write(
			pul::debug_level::warning,
			pul::debug_filter::important,
			"This is a warning message...");
}
GTEST_TEST(DebugLoggerTest, DebugLoggerErrorTest)
{
	pul::debug_logger::write(
			pul::debug_level::error,
			pul::debug_filter::important,
			"This is an error message...");
}

/// Debug Message Box
GTEST_TEST(DebugMessageBox, DebugMessageBoxInfo)
{
	pul::debug_messagebox(
			pul::debug_level::info,
			"Info message box",
			"This is an info message box.");
}
GTEST_TEST(DebugMessageBox, DebugMessageBoxWarning)
{
	pul::debug_messagebox(
			pul::debug_level::warning,
			"Warning message box",
			"This is a warning message box.");
}
GTEST_TEST(DebugMessageBox, DebugMessageBoxError)
{
	pul::debug_messagebox(
			pul::debug_level::error,
			"Error message box",
			"This is an error message box.");
}

/// Debug StackTrace
GTEST_TEST(DebugStacktraceTest, DebugStacktraceTest)
{
	auto stl = pul::debug_stacktrace(0);
	GTEST_ASSERT_FALSE(stl.empty());
}