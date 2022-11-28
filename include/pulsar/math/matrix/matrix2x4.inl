/*! @file   matrix2x4.inl
 *  @author Fluffy (noe.louis-quentin@hotmail.fr)
 *  @brief	Specialization for matrices of 2 rows and 4 columns.
 *  @date   31-05-2022
 *
 *  @copyright Copyright (c) 2022 - Pulsar Software
 *
 *  @since 0.1.1
 */

#ifndef PULSAR_MATH_MATRIX2X4_INL
#define PULSAR_MATH_MATRIX2X4_INL 1

// Include: Pulsar
#include "pulsar/intrin.hpp"

// Pulsar
namespace pul
{
	/// MATH: Matrix -> Definition
	template <typename _Ty, size_t _RowNum, size_t _ColNum>
	requires(
		std::is_arithmetic_v<_Ty>
		&& _RowNum > 1 && _ColNum > 1)
	union matrix;

	/// MATH: bmat2x4_t
	using bmat2x4_t
	pf_alignas(bool) = matrix<bool, 2, 4>;

	/// MATH: umat2x4_t
	using u8mat2x4_t	= pf_alignas(uint8_t) matrix<uint8_t, 2, 4>;
	using u16mat2x4_t = pf_alignas(uint16_t) matrix<uint16_t, 2, 4>;
	using u32mat2x4_t = pf_alignas(uint32_t) matrix<uint32_t, 2, 4>;
	using u64mat2x4_t = pf_alignas(uint64_t) matrix<uint64_t, 2, 4>;

	/// MATH: s_umat2x4_t
	using s_u16mat2x4_t = pf_alignas_n(uint16_t, 4) matrix<uint16_t, 2, 4>;
	using s_u32mat2x4_t = pf_alignas_n(uint32_t, 4) matrix<uint32_t, 2, 4>;
	using s_u64mat2x4_t = pf_alignas_n(uint64_t, 4) matrix<uint64_t, 2, 4>;

	/// MATH: imat2x4_t
	using i8mat2x4_t	= pf_alignas(int8_t) matrix<int8_t, 2, 4>;
	using i16mat2x4_t = pf_alignas(int16_t) matrix<int16_t, 2, 4>;
	using i32mat2x4_t = pf_alignas(int32_t) matrix<int32_t, 2, 4>;
	using i64mat2x4_t = pf_alignas(int64_t) matrix<int64_t, 2, 4>;

	/// MATH: s_imat2x4_t
	using s_i16mat2x4_t = pf_alignas_n(int16_t, 4) matrix<int16_t, 2, 4>;
	using s_i32mat2x4_t = pf_alignas_n(int32_t, 4) matrix<int32_t, 2, 4>;
	using s_i64mat2x4_t = pf_alignas_n(int64_t, 4) matrix<int64_t, 2, 4>;

	/// MATH: fmat2x4_t
	using f32mat2x4_t = pf_alignas(float32_t) matrix<float32_t, 2, 4>;
	using f64mat2x4_t = pf_alignas(float64_t) matrix<float64_t, 2, 4>;

	/// MATH: s_fmat2x4_t
	using s_f32mat2x4_t = pf_alignas_n(float32_t, 4) matrix<float32_t, 2, 4>;
	using s_f64mat2x4_t = pf_alignas_n(float64_t, 4) matrix<float64_t, 2, 4>;
}

#endif // !PULSAR_MATH_MATRIX2X4_INL