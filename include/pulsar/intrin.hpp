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

	/// SIMD: SSE2 -> Functions
	pf_hint_nodiscard pf_decl_inline m128f64_t _mm_cvtepu64_ps_mystical(
		m128i64_t __x) pf_attr_noexcept	// Mystical's int64_t -> float64_t
	{
		__x = _mm_add_epi64(__x, _mm_castpd_si128(_mm_set1_pd(0x0018000000000000)));
		return _mm_sub_pd(_mm_castsi128_pd(__x), _mm_set1_pd(0x0018000000000000));
	}
	pf_hint_nodiscard pf_decl_inline m128f64_t _mm_cvtepi64_ps_mystical(
		m128i64_t __x) pf_attr_noexcept	// Mystical's uint64_t -> float64_t
	{
		__x = _mm_or_si128(__x, _mm_castpd_si128(_mm_set1_pd(0x0010000000000000)));
		return _mm_sub_pd(_mm_castsi128_pd(__x), _mm_set1_pd(0x0010000000000000));
	}
	pf_hint_nodiscard pf_decl_inline m128f64_t _mm_cvtepi64_ps(m128i64_t __x)
	pf_attr_noexcept {
		m128i64_t m	 = _mm_set1_epi64x(0xFFFFFFFF);
		m128f64_t c	 = _mm_set1_pd(4294967296.0);	/* 2^32 */
		m128i64_t lo =
			_mm_and_si128(__x, m);/* extract the 32 lowest significant bits of __x */
		m128i64_t hi = _mm_srli_epi64(
			__x, 32);	/* 32 most significant bits of __x. srai_epi64 doesn't exist */
		m128i64_t sign = _mm_srai_epi32(
			__x, 32);	/* broadcast sign bit to the 32 most significant bits */
		hi = _mm_blend_epi32(hi, sign,
												 0b10101010);	/* restore the correct sign of hi */
		m128f64_t lo_mystical = _mm_cvtepu64_ps_mystical(
			lo);/* lo is within specified range of int64_to_double */
		m128f64_t hi_mystical = _mm_cvtepu64_ps_mystical(
			hi);/* hi is within specified range of int64_to_double */
		hi_mystical =
			_mm_mul_pd(c, hi_mystical);	/* _mm_mul_pd and _mm_add_pd may compile to a
																	   single fma instruction  */
		return _mm_add_pd(hi_mystical, lo_mystical);/* rounding occurs if the integer
																								   doesn't exist as a double */
	}
	pf_hint_nodiscard pf_decl_inline m128f64_t _mm_cvtepu64_ps(m128i64_t __x)
	pf_attr_noexcept// converts uint64_t -> float64_t
	{
		m128i64_t m	 = _mm_set1_epi64x(0xFFFFFFFF);
		m128f64_t c	 = _mm_set1_pd(4294967296.0);	/* 2^32 */
		m128i64_t lo =
			_mm_and_si128(__x, m);/* extract the 32 lowest significant bits of __x */
		m128i64_t hi					= _mm_srli_epi64(__x, 32);/* 32 most significant bits of __x */
		m128f64_t lo_mystical = _mm_cvtepi64_ps_mystical(
			lo);/* lo is within specified range of uint64_to_double */
		m128f64_t hi_mystical = _mm_cvtepi64_ps_mystical(
			hi);/* hi is within specified range of uint64_to_double */
		hi_mystical = _mm_mul_pd(c, hi_mystical);
		return _mm_add_pd(hi_mystical,
											lo_mystical);	/* rounding may occur for inputs > 2^52 */
	}

	/// SIMD: AVX2 -> Types
	using m256i64_t	 = __m256i;
	using m256i64u_t = __m256i_u;
	using m256f32_t	 = __m256;
	using m256f32u_t = __m256_u;
	using m256f64_t	 = __m256d;
	using m256f64u_t = __m256d_u;

	/// SIMD: AVX2 -> Cast: int*_t
	/// url =
	/// https://stackoverflow.com/questions/41144668/how-to-efficiently-perform-double-int64-conversions-with-sse-avx
	pf_hint_nodiscard pf_decl_inline m256f64_t _mm256_cvtepu64_ps_mystical(
		m256i64_t __x) pf_attr_noexcept	// Mystical's int64_t -> float64_t
	{
		__x = _mm256_add_epi64(
			__x, _mm256_castpd_si256(_mm256_set1_pd(0x0018000000000000)));
		return _mm256_sub_pd(_mm256_castsi256_pd(__x),
												 _mm256_set1_pd(0x0018000000000000));
	}
	pf_hint_nodiscard pf_decl_inline m256f64_t _mm256_cvtepi64_ps_mystical(
		m256i64_t __x) pf_attr_noexcept	// Mystical's uint64_t -> float64_t
	{
		__x = _mm256_or_si256(
			__x, _mm256_castpd_si256(_mm256_set1_pd(0x0010000000000000)));
		return _mm256_sub_pd(_mm256_castsi256_pd(__x),
												 _mm256_set1_pd(0x0010000000000000));
	}
	pf_hint_nodiscard pf_decl_inline m256f64_t _mm256_cvtepi64_ps(m256i64_t __x)
	pf_attr_noexcept// converts int64_t -> float64_t
	{
		m256i64_t m	 = _mm256_set1_epi64x(0xFFFFFFFF);
		m256f64_t c	 = _mm256_set1_pd(4294967296.0);/* 2^32 */
		m256i64_t lo = _mm256_and_si256(
			__x, m);/* extract the 32 lowest significant bits of v */
		m256i64_t hi = _mm256_srli_epi64(
			__x, 32);	/* 32 most significant bits of v. srai_epi64 doesn't exist */
		m256i64_t sign = _mm256_srai_epi32(
			__x, 32);	/* broadcast sign bit to the 32 most significant bits */
		hi = _mm256_blend_epi32(hi, sign,
														0b10101010);/* restore the correct sign of hi */
		m256f64_t lo_mystical = _mm256_cvtepu64_ps_mystical(
			lo);/* lo is within specified range of int64_to_double */
		m256f64_t hi_mystical = _mm256_cvtepu64_ps_mystical(
			hi);/* hi is within specified range of int64_to_double */
		hi_mystical =
			_mm256_mul_pd(c, hi_mystical);/* _mm256_mul_pd and _mm256_add_pd may
																		   compile to a single fma instruction */
		return _mm256_add_pd(hi_mystical,
												 lo_mystical);/* rounding occurs if the integer doesn't
																			   exist as a double                */
	}
	pf_hint_nodiscard pf_decl_inline m256f64_t _mm_cvtepu64_ps(m256i64_t __x)
	pf_attr_noexcept// converts uint64_t -> float64_t
	{
		m256i64_t m	 = _mm256_set1_epi64x(0xFFFFFFFF);
		m256f64_t c	 = _mm256_set1_pd(4294967296.0);/* 2^32 */
		m256i64_t lo = _mm256_and_si256(
			__x, m);/* extract the 32 lowest significant bits of v */
		m256i64_t hi					= _mm256_srli_epi64(__x, 32);	/* 32 most significant bits of v */
		m256f64_t lo_mystical = _mm256_cvtepi64_ps_mystical(
			lo);/* lo is within specified range of uint64_to_double */
		m256f64_t hi_mystical = _mm256_cvtepi64_ps_mystical(
			hi);/* hi is within specified range of uint64_to_double */
		hi_mystical = _mm256_mul_pd(c, hi_mystical);
		return _mm256_add_pd(hi_mystical,
												 lo_mystical);/* rounding may occur for inputs > 2^52 */
	}

	/// SIMD: AVX512 -> Types
	using m512i64_t	 = __m512i;
	using m512i64u_t = __m512i_u;
	using m512f32_t	 = __m512;
	using m512f32u_t = __m512_u;
	using m512f64_t	 = __m512d;
	using m512f64u_t = __m512d_u;
}	// namespace pul

/// SIMD: Alignas -> Vector
#define pf_alignas_v(type, num)                                                \
				pf_alignas(alignof(type) * num > 16 ? alignof(type) * num : 16)

#endif // !PULSAR_INTRIN_HPP