/**
 * @file    pulsar_common.hpp
 * @author  Fluffy (noe.louis-quentin@hotmail.fr)
 * @brief   This file verifies that compilation is possible, by the presence of compatible operating
 * 			systems and compilers and defines all the macros, types, of these for their use through
 * 			the pulsar framework. Pulsar framework requires at least C++17!
 * @date    23-11-2021
 *
 * @copyright Copyright (c) 2021 - Pulsar Software
 */

#ifndef PULSAR_HPP
#define PULSAR_HPP 1

// C++ 17 is required!
#if !defined(__cplusplus) || __cplusplus < 201703L
#	error "[PULSAR]: At least C++ 20 is required!"
#endif // !__cplusplus || __cplusplus < 201703L

// The computer must run on one of the following OS
// OS: Windows
#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32) || defined(_WIN64) \
		|| defined(__WIN64__) || defined(WIN64)
#	define PF_OS_WINDOWS 1

// Call
#	define pf_oscall			__stdcall

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
#	error "[PULSAR]: OS isn't supported yet!"
#endif // Not supported OS!

// Include: C++
#include <memory>

// Pulsar
namespace pulsar
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
#ifdef PF_64BIT
	using size_t		= uint64_t;
	using diff_t		= int64_t;
	using ptrsize_t = size_t;
	using ptrdiff_t = diff_t;
#else	 // ^^^ PF_64BIT ^^^ / vvv PF_32BIT vvv
	using size_t		= uint32_t;
	using diff_t		= int32_t;
	using ptrsize_t = size_t;
	using ptrdiff_t = diff_t;
#endif // PF_64BIT

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
#	define pf_decl_allocator		__declspec(allocator)
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
#define pf_decl_alignas(alignment)																					alignas(alignment)
#define pf_decl_constexpr																										constexpr
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
#define pf_hint(hint)																												_assume(hint)
#define pf_hint_likely																											[[likely]]
#define pf_hint_unlikely																										[[unlikely]]
#define pf_hint_fallthrough																									[[fallthrough]]
#define pf_hint_nodiscard																										[[nodiscard]]
#define pf_hint_deprecated																									[[deprecated]]
#define pf_hint_no_switch_default																						_assume(0)

// Tools
#define pf_stringize_helper(token)																					#token;
#define pf_stringize(str)																										pf_stringize_helper(str)
#define pf_join_helper(x, y)																								x##y
#define pf_join(x, y)																												pf_join_helper(x, y)
#define pf_va_args_counter(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, N, ...) N
#define pf_va_args_num(...)																									pf_va_args_counter(__VA_ARGS__, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1)

	// Function
	/**
	 * @brief Alias that defines function pointers more easily
	 *
	 * @tparam _Ret  The type of return
	 * @tparam _Args (Variadic) Types of arguments
	 */
	template <typename _Ret, typename... _Args>
	using ptrfun = _Ret (*)(_Args...);

	// Allocator
#ifndef PF_ALLOCATOR
	template <typename _Ty>
	using allocator = std::allocator<_Ty>;
#endif // !PF_CUSTOM_ALLOCATOR

} // Pulsar

// Version
#include "pulsar/pulsar_version.hpp"

#endif // !PULSAR_HPP