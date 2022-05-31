/*! @file    pulsar.hpp
 * 	@author  Fluffy (noe.louis-quentin@hotmail.fr)
 * 	@brief   This file verifies that compilation is possible, by the presence of compatible
 * 					operating systems and compilers and defines all the macros, types, of these for their
 * 					use through the pulsar framework. Pulsar framework requires at least C++20!
 * 	@date    23-11-2021
 *
 * 	@copyright Copyright (c) 2021 - Pulsar Software
 *
 * 	@since 0.1.0
 */

#ifndef PULSAR_HPP
#define PULSAR_HPP 1

// C++20 is required!
#if !defined(__cplusplus) || __cplusplus < 201703L
#	error "[PULSAR]: At least C++ 20 is required!"
#endif // !__cplusplus || __cplusplus < 201703L

// The computer must run on one of the following OS
// OS: Windows
#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32) || defined(_WIN64) || defined(__WIN64__) || defined(WIN64)
#	define PF_OS_WINDOWS 1

// Reduces size of windows.h inclusion
#	define WIN32_LEAN_AND_MEAN
#	define NOMINMAX	1

// Call
#	define pf_oscall __stdcall

#	if defined(_WIN64) || defined(__WIN64__) || defined(WIN64)
#		define PF_64BIT 1
#	else // ^^^ WIN64 ^^^ / vvv WIN32 vvv
#		define PF_32BIT 1
#	endif // _WIN64

// OS: Linux
#elif defined(__linux) || defined(__linux__) || defined(linux) // ^^^ Windows ^^^ / vvv Linux vvv
#	define PF_OS_LINUX 1

// Call
#	define pf_oscall

#else // ^^^ Linux ^^^ / vvv Unknown / Unsupported vvv
#	error "[pulsar]: OS isn't supported yet!"
#endif // Not supported OS!

// Architecture
#if defined(_M_IX86) || defined(__i386) || defined(__i386__) || defined(i386) || defined(_X86_) // vvv i386 vvv
#	define PF_ARCHITECTURE_I386 1
#elif defined(_M_X64) || defined(__amd64__) || defined(__amd64) || defined(__x86_64__) || defined(__x86_64) // ^^^ i386 ^^^ / vvv AMD 64 vvv
#	define PF_ARCHITECTURE_AMD64 1
#elif defined(_M_IA64) || defined(__ia64__) || defined(_IA64) || defined(__IA64__) // ^^^ AMD64 ^^^ /  vvv IA64 vvv
#	define PF_ARCHITECTURE_IA64 1
#else // ^^^ IA64 ^^^
#	error "[pulsar]: Architecture isn't supported!"
#endif // ^^^ Architecture ^^^

// Debug
#ifdef _DEBUG
#	define PF_DEBUG 1
#endif // _DEBUG

// Static Assert
#define pf_static_assert(...) static_assert(__VA_ARGS__)

// Assert
#ifdef PF_DEBUG
#	define pf_assert(val, message) assert(val &&message)
#else
#	define pf_assert(val, message)
#endif

// Include: C
#include <cstdint>

// Pulsar
namespace pul
{

	/// Types
	// Integers: Signed
	using int8_t	= signed char;
	using int16_t = signed short;
	using int32_t = signed int;
	using int64_t = signed long long;

	// Integers: Unsigned
	using uint8_t	 = unsigned char;
	using uint16_t = unsigned short;
	using uint32_t = unsigned int;
	using uint64_t = unsigned long long;

	// Floats
	using float32_t = float;
	using float64_t = long double;

	// Byte
	using byte_t = unsigned char;

	// Varying
	using size_t			= std::size_t;
	using align_val_t = std::align_val_t;
#ifdef PF_64BIT
	using diff_t = int64_t;
#else	 // ^^^ PF_64_BIT ^^^ / vvv PF_32_BIT vvv
	using diff_t = int32_t;
#endif // PF_64_BIT

	// GNU / MSVC compiler
#if defined(_MSC_VER) || defined(__MINGW32__) || defined(__MINGW64__)
#	define pf_pragma(pragma)		_pragma(pragma)

// Declspec
#	define pf_decl_safebuffers __declspec(safebuffers)
#	define pf_decl_noinline		__declspec(noinline)
#	define pf_decl_novtable		__declspec(novtable)
#	define pf_decl_noalias			__declspec(noalias)
#	define pf_decl_export			__declspec(dllexport)
#	define pf_decl_import			__declspec(dllimport)
#	define pf_decl_malloc			__declspec(restrict)

// GNU
#else
#	if defined(__MINGW64__)
#		define PF_64BIT 1
#	elif defined(__MINGW32__) // ^^^ 64BIT ^^^ / vvv 32BIT vvv
#		define PF_32BIT 1
#	else
#		error "[Pulsar]: Compiler not supported yet!"
#	endif // __MINGW64__
#endif	 // _MSC_VER || defined(__MINGW32__) || defined(__MINGW64__)

// CPP decl
#define pf_decl_constexpr																										constexpr
#define pf_decl_consteval																										consteval
#define pf_decl_friend																											friend
#define pf_decl_extern																											extern
#define pf_decl_static																											static
#define pf_decl_inline																											inline
#define pf_decl_virtual																											virtual
#define pf_decl_explicit																										explicit
#define pf_decl_thread_local																								thread_local

// Calls
#define pf_defcall																													__cdecl
#define pf_fastcall																													__fastcall

// Attribs
#define pf_attr_noexcept																										noexcept
#define pf_attr_override																										override
#define pf_attr_final																												final

// Hints
#define pf_hint_likely																											[[likely]]
#define pf_hint_unlikely																										[[unlikely]]
#define pf_hint_fallthrough																									[[fallthrough]]
#define pf_hint_nodiscard																										[[nodiscard]]
#define pf_hint_deprecated																									[[deprecated]]

// Tools
#define pf_stringize_helper(token)																					#token;
#define pf_stringize(str)																										pf_stringize_helper(str)
#define pf_join_helper(x, y)																								x##y
#define pf_join(x, y)																												pf_join_helper(x, y)
#define pf_va_args_counter(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, N, ...) N
#define pf_va_args_num(...)																									pf_va_args_counter(__VA_ARGS__, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1)
}

// Version
#include "pulsar/pulsar_version.hpp"

#ifdef PULSAR_CONSTRUCT
#	define pulsar_api pf_decl_export
#else // ^^^ PULSAR_CONSTRUCT ^^^ // vvv !PULSAR_CONSTRUCT vvv
#	define pulsar_api pf_decl_import
#endif // PULSAR_CONSTRUCT

#endif // !PULSAR_HPP