/*! @file   intrin.hpp
 *  @author Louis-Quentin Noé (noe.louis-quentin@hotmail.fr)
 *  @brief  Implementation of intrin utilitaries.
 *  @date   14-08-2022
 *
 *  @copyright Copyright (c) 2023 - Pulsar Software
 *
 *  @since 0.1.2
 */

#ifndef PULSAR_INTRIN_HPP
#define PULSAR_INTRIN_HPP 1

// Include: Pulsar
#include "pulsar/pulsar.hpp"

// Include: C++
#include <algorithm>
#include <type_traits>

// Include: C
#include <immintrin.h>
#include <intrin.h>

// Pulsar
namespace pul
{
	/*
	 * !      !
	 *   SIMD
	 * !      !
	 */

	/// SIMD: SSE2 -> Types
	using m128i64_t	 = __m128i;
	using m128i64u_t = __m128i_u;
	using m128f32_t	 = __m128;
	using m128f32u_t = __m128_u;
	using m128f64_t	 = __m128d;
	using m128f64u_t = __m128d_u;

	/// SIMD: AVX2 -> Types
	using m256i64_t	 = __m256i;
	using m256i64u_t = __m256i_u;
	using m256f32_t	 = __m256;
	using m256f32u_t = __m256_u;
	using m256f64_t	 = __m256d;
	using m256f64u_t = __m256d_u;

	/// SIMD: AVX512 -> Types
	using m512i64_t	 = __m512i;
	using m512i64u_t = __m512i_u;
	using m512f32_t	 = __m512;
	using m512f32u_t = __m512_u;
	using m512f64_t	 = __m512d;
	using m512f64u_t = __m512d_u;
}	 // namespace pul

/// SIMD: Alignas -> Vector
#define pf_alignas_v(type, num) \
	pf_alignas(alignof(type) * num > 16 ? alignof(type) * num : 16)

#endif	// !PULSAR_INTRIN_HPP
