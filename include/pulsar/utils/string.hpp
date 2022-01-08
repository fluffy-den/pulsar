/**
 * @file    string.hpp
 * @author  Fluffy (noe.louis-quentin@hotmail.fr)
 * @brief   Defines string objects from the standard libraries.
 * @date    08-01-2022
 *
 * @copyright Copyright (c) 2022 - Pulsar Software
 *
 */

#ifndef PULSAR_STRING_HPP
#define PULSAR_STRING_HPP 1

// Include: C++
#include <string>
#include <string_view>

// Pulsar
namespace pulsar
{
	/// Types
	using string			= std::string;
	using string_view = std::string_view;
}

#endif // !PULSAR_STRING_HPP