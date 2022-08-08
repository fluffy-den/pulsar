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
#define pf_decl_constexpr																																										 constexpr
#define pf_decl_consteval																																										 consteval
#define pf_decl_friend																																											 friend
#define pf_decl_extern																																											 extern
#define pf_decl_static																																											 static
#define pf_decl_inline																																											 inline
#define pf_decl_virtual																																											 virtual
#define pf_decl_explicit																																										 explicit
#define pf_decl_thread_local																																								 thread_local

// Calls
#define pf_defcall																																													 __cdecl
#define pf_fastcall																																													 __fastcall

// Attribs
#define pf_attr_noexcept																																										 noexcept
#define pf_attr_override																																										 override
#define pf_attr_final																																												 final

// Hints
#define pf_hint_likely																																											 [[likely]]
#define pf_hint_unlikely																																										 [[unlikely]]
#define pf_hint_fallthrough																																									 [[fallthrough]]
#define pf_hint_nodiscard																																										 [[nodiscard]]
#define pf_hint_deprecated																																									 [[deprecated]]
// Tools
#define pf_stringize_helper(token)																																					 #token;
#define pf_stringize(str)																																										 pf_stringize_helper(str)
#define pf_nargs_pass4(x)																																										 x
#define pf_nargs_pass3(_unused, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _val, ...) _val
#define pf_nargs_pass2(...)																																									 pf_nargs_pass4(pf_nargs_pass3(__VA_ARGS__, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0))
#define pf_nargs_pass1(...)																																									 unused, __VA_ARGS__
#define pf_nargs(...)																																												 pf_nargs_pass2(pf_nargs_pass1(__VA_ARGS__))
#define pf_cat_tokens(x, y)																																									 pf_cat_pass1((x, y))
#define pf_primitive_cat(x, y)																																							 x##y
#define pf_pass1(x)																																													 pf_primitive_cat x
#define pf_rem(...)																																													 __VA_ARGS__
#define pf_eat(...)
#define pf_arg_pair(x)						pf_rem x
#define pf_arg_type_pass4(x, ...) pf_rem x
#define pf_arg_type_pass3(x)			pf_arg_type_pass4 x
#define pf_arg_type_pass2(...)		pf_arg_type_pass3((__VA_ARGS__))
#define pf_arg_type_pass1(...)		(__VA_ARGS__)
#define pf_arg_type(x)						pf_arg_type_pass2(pf_arg_type_pass1 x, )
#define pf_arg_name(x)						pf_stringize(x)

// Tools -> Do For Each Semicolon
#define pf_do_for_each_semicolon_0(m)
#define pf_do_for_each_semicolon_1(m, x1)																																 , m(x1)
#define pf_do_for_each_semicolon_2(m, x1, x2)																														 , m(x1), m(x2)
#define pf_do_for_each_semicolon_3(m, x1, x2, x3)																												 , m(x1), m(x2), m(x3)
#define pf_do_for_each_semicolon_4(m, x1, x2, x3, x4)																										 , m(x1), m(x2), m(x3), m(x4)
#define pf_do_for_each_semicolon_5(m, x1, x2, x3, x4, x5)																								 , m(x1), m(x2), m(x3), m(x4), m(x5)
#define pf_do_for_each_semicolon_6(m, x1, x2, x3, x4, x5, x6)																						 , m(x1), m(x2), m(x3), m(x4), m(x5), m(x6)
#define pf_do_for_each_semicolon_7(m, x1, x2, x3, x4, x5, x6, x7)																				 , m(x1), m(x2), m(x3), m(x4), m(x5), m(x6), m(x7)
#define pf_do_for_each_semicolon_8(m, x1, x2, x3, x4, x5, x6, x7, x8)																		 , m(x1), m(x2), m(x3), m(x4), m(x5), m(x6), m(x7), m(x8)
#define pf_do_for_each_semicolon_9(m, x1, x2, x3, x4, x5, x6, x7, x8, x9)																 , m(x1), m(x2), m(x3), m(x4), m(x5), m(x6), m(x7), m(x8), m(x9)
#define pf_do_for_each_semicolon_10(m, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10)													 , m(x1), m(x2), m(x3), m(x4), m(x5), m(x6), m(x7), m(x8), m(x9), m(x10)
#define pf_do_for_each_semicolon_11(m, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11)										 , m(x1), m(x2), m(x3), m(x4), m(x5), m(x6), m(x7), m(x8), m(x9), m(x10), m(x11)
#define pf_do_for_each_semicolon_12(m, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12)								 , m(x1), m(x2), m(x3), m(x4), m(x5), m(x6), m(x7), m(x8), m(x9), m(x10), m(x11), m(x12)
#define pf_do_for_each_semicolon_13(m, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13)					 , m(x1), m(x2), m(x3), m(x4), m(x5), m(x6), m(x7), m(x8), m(x9), m(x10), m(x11), m(x12), m(x13)
#define pf_do_for_each_semicolon_14(m, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13, x14)			 , m(x1), m(x2), m(x3), m(x4), m(x5), m(x6), m(x7), m(x8), m(x9), m(x10), m(x11), m(x12), m(x13), m(x14)
#define pf_do_for_each_semicolon_15(m, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13, x14, x15) , m(x1), m(x2), m(x3), m(x4), m(x5), m(x6), m(x7), m(x8), m(x9), m(x10), m(x11), m(x12), m(x13), m(x14), m(x15)
#define pf_do_for_each_semicolon(macro, ...)																														 pf_do_for_each_pass1(pf_cat_tokens(pf_do_for_each_semicolon_, pf_nargs(__VA_ARGS__)), (macro, __VA_ARGS__))
}

// Version
#include "pulsar/pulsar_version.hpp"

#ifdef PULSAR_CONSTRUCT
#	define pulsar_api pf_decl_export
#else // ^^^ PULSAR_CONSTRUCT ^^^ // vvv !PULSAR_CONSTRUCT vvv
#	define pulsar_api pf_decl_import
#endif // PULSAR_CONSTRUCT

#endif // !PULSAR_HPP