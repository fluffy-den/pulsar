/*! @file   pulsar.hpp
 * 	@author Louis-Quentin Noé (noe.louis-quentin@hotmail.fr)
 * 	@brief  This file verifies that compilation is possible, by the presence
 *          of compatible operating systems and compilers and defines all the macros,
 *          types, of these for their use through the pulsar framework. Pulsar framework
 *          requires at least C++20!
 *
 * 	@date 23-11-2021
 *
 * 	@copyright Copyright (c) 2023 - Pulsar Software
 *
 * 	@since 0.1.0
 */

#ifndef PULSAR_HPP
#define PULSAR_HPP 1

// Include: C
#include <cassert>	// assert
#include <cstdio>		// __VA_ARGS__

// C++20 is required!
#if !defined(__cplusplus) || __cplusplus < 202002L
#error "[PULSAR]: At least C++ 20 is required!"
#endif  // !__cplusplus || __cplusplus < 201703L

// The computer must run on one of the following OS
// OS: Windows
#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32) || \
	defined(_WIN64) || defined(__WIN64__) || defined(WIN64)
#define PF_OS_WINDOWS 1

// Reduces size of windows.h inclusion
#define WIN32_LEAN_AND_MEAN 1
#define VC_EXTRALEAN 1
#define NOMINMAX 1

// Call
#define pf_oscall __stdcall

#if defined(_WIN64) || defined(__WIN64__) || defined(WIN64)
#define PF_64BIT 1
#else  // ^^^ WIN64 ^^^ / vvv WIN32 vvv
#define PF_32BIT 1
#endif  // _WIN64

// OS: Linux
#elif defined(__linux) || defined(__linux__) ||	\
	defined(linux)		// ^^^ Windows ^^^ / vvv Linux vvv
#define PF_OS_LINUX 1

// Call
#define pf_oscall

#else  // ^^^ Linux ^^^ / vvv Unknown / Unsupported vvv
#error "[pulsar]: OS isn't supported yet!"
#endif  // Not supported OS!

// Architecture
#if defined(_M_IX86) || defined(__i386) || defined(__i386__) ||	\
	defined(i386) || defined(_X86_)		// vvv i386 vvv
#define PF_ARCHITECTURE_I386 1
#elif defined(_M_X64) || defined(__amd64__) || defined(__amd64) || \
	defined(__x86_64__) || defined(__x86_64)		// ^^^ i386 ^^^ / vvv AMD 64 vvv
#define PF_ARCHITECTURE_AMD64 1
#elif defined(_M_IA64) || defined(__ia64__) || defined(_IA64) || \
	defined(__IA64__)		// ^^^ AMD64 ^^^ /  vvv IA64 vvv
#define PF_ARCHITECTURE_IA64 1
#else  // ^^^ IA64 ^^^
#error "[pulsar]: Architecture isn't supported!"
#endif  // ^^^ Architecture ^^^

// Debug
#ifndef NDEBUG
#define PF_DEBUG 1
#endif  // !NDEBUG

// Release
#ifndef PF_DEBUG
#define PF_RELEASE 1
#endif  // !PF_DEBUG

// Compiler
#if defined(__MINGW32__) || defined(__MINGW64__)
#define PF_GCC 1
#endif
#if defined(__clang__)
#define PF_CLANG 1
#endif
#if defined(_MSC_VER)
#define PF_MSVC 1
#endif

// Decl -> Pragma
#if defined(PF_MSVC) || defined(PF_GCC)
#define pf_pragma(pragma) _pragma(pragma)

// GNU
#else
#if defined(__MINGW64__)
#define PF_64BIT 1
#elif defined(__MINGW32__)	// ^^^ 64BIT ^^^ / vvv 32BIT vvv
#define PF_32BIT 1
#else
#error "[Pulsar]: Compiler not supported yet!"
#endif  // __MINGW64__
#endif  // _MSC_VER || defined(__MINGW32__) || defined(__MINGW64__)

// Decl -> Standard
#define pf_decl_safebuffers __declspec(safebuffers)
#define pf_decl_noinline    __declspec(noinline)
#define pf_decl_novtable    __declspec(novtable)
#define pf_decl_export      __declspec(dllexport)
#define pf_decl_import      __declspec(dllimport)
#define pf_decl_constexpr     constexpr
#define pf_decl_consteval     consteval
#define pf_decl_constinit     constinit
#define pf_decl_friend        friend
#define pf_decl_extern        extern
#define pf_decl_static        static
#define pf_decl_inline        inline
#define pf_decl_virtual       virtual
#define pf_decl_explicit      explicit
#define pf_decl_thread_local  thread_local

// Calls
#define pf_defcall __cdecl
#define pf_fastcall __fastcall

// Attribs
#define pf_attr_noexcept       noexcept
#define pf_attr_noexcept_if(c) noexcept(c)
#define pf_attr_override       override
#define pf_attr_final          final
#define pf_attr_malloc         __attribute__((malloc))

// Hints
#define pf_hint_fallthrough      [[fallthrough]]
#define pf_hint_nodiscard        [[nodiscard]]
#define pf_hint_deprecated       [[deprecated]]
#define pf_hint_maybe_unused     [[maybe_unused]]
#define pf_hint_noreturn         [[noreturn]]
#define pf_hint_nounique_address [[no_unique_address]]

// Tools
#define pf_unlikely(x)            __builtin_expect(!!(x), false)
#define pf_likely(x)              __builtin_expect(!!(x), true)
#define pf_offsetof(type, member) offsetof(type, member)
#define pf_line                   __LINE__
#if defined(PF_CLANG) || defined(PF_GCC)
#define pf_function               __PRETTY_FUNCTION__
#else
#define pf_function               __FUNCTION__
#endif
#define pf_file                   __FILE__
#define pf_va_args                __VA_ARGS__	// TODO: Find unused __VA_ARGS__
#define pf_alignas(a)             alignas(a)
#define pf_alignas_n(type, a)					 \
				alignas(alignof(type) * a > 32 \
							? 32										 \
							: (alignof(type) * a < 16 ? 16 : alignof(type) * a))
#define pf_concatenate_helper(a, b) a ## b
#define pf_concatenate(a, b) pf_concatenate_helper(a, b)
#define pf_stringize_helper(a) #a
#define pf_stringize(a) pf_stringize_helper(a)
#define pf_parent_scope 1

// Include: Pulsar -> Version
#include "pulsar/pulsar_version.hpp"

#if defined(PULSAR_SHARED)
 #if defined(PULSAR_BUILD)
	#define pulsar_api pf_decl_export
 #else  // ^^^ PULSAR_SHARED ^^^ // vvv !PULSAR_SHARED vvv
	#define pulsar_api pf_decl_import
 #endif
#else
 #define pulsar_api
#endif

// Include: C++
#include <type_traits>

// Pulsar
namespace pul
{
	// Integral: Signed
	using int8_t	= char;
	using int16_t = short;
	using int32_t = int;
	using int64_t = long long;

	// Integral: Unsigned
	using uint8_t	 = unsigned char;
	using uint16_t = unsigned short;
	using uint32_t = unsigned int;
	using uint64_t = unsigned long long;

	// Floating-Points
	using float32_t = float;
	using float64_t = double;

	// Byte
	using ubyte_t = unsigned char;
	using byte_t	= char;
	using uchar_t = ubyte_t;
	using char_t	= char;

	// Enum
	using enum_t = uint32_t;

	// Varying
	using size_t			= std::size_t;
	using align_val_t = std::align_val_t;
#ifdef PF_64BIT
	using diff_t = int64_t;
#else   // ^^^ PF_64_BIT ^^^ / vvv PF_32_BIT vvv
	using diff_t = int32_t;
#endif  // PF_64_BIT

	// Nullptr
	typedef decltype(nullptr) nullptr_t;
}

#endif  // !PULSAR_HPP