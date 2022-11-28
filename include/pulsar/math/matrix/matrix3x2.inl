/*! @file   matrix3x2.inl
 *  @author Fluffy (noe.louis-quentin@hotmail.fr)
 *  @brief	Specialization for matrices of 3 rows and 2 columns.
 *  @date   31-05-2022
 *
 *  @copyright Copyright (c) 2022 - Pulsar Software
 *
 *  @since 0.1.1
 */

#ifndef PULSAR_MATH_MATRIX3X2_INL
#define PULSAR_MATH_MATRIX3X2_INL 1

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

	/// MATH: bmat3x2_t
	using bmat3x2_t
	pf_alignas(bool) = matrix<bool, 3, 2>;

	/// MATH: umat3x2_t
	using u8mat3x2_t pf_alignas(uint8_t)	 = matrix<uint8_t, 3, 2>;
	using u16mat3x2_t pf_alignas(uint16_t) = matrix<uint16_t, 3, 2>;
	using u32mat3x2_t pf_alignas(uint32_t) = matrix<uint32_t, 3, 2>;
	using u64mat3x2_t pf_alignas(uint64_t) = matrix<uint64_t, 3, 2>;

	/// MATH: s_umat3x2_t
	using s_u32mat3x2_t pf_alignas_n(uint32_t, 2) = matrix<uint32_t, 3, 2>;
	using s_u64mat3x2_t pf_alignas_n(uint64_t, 2) = matrix<uint64_t, 3, 2>;

	/// MATH: imat3x2_t
	using i8mat3x2_t pf_alignas(int8_t)		= matrix<int8_t, 3, 2>;
	using i16mat3x2_t pf_alignas(int16_t) = matrix<int16_t, 3, 2>;
	using i32mat3x2_t pf_alignas(int32_t) = matrix<int32_t, 3, 2>;
	using i64mat3x2_t pf_alignas(int64_t) = matrix<int64_t, 3, 2>;

	/// MATH: s_imat3x2_t
	using s_i32mat3x2_t pf_alignas_n(int32_t, 2) = matrix<int32_t, 3, 2>;
	using s_i64mat3x2_t pf_alignas_n(int64_t, 2) = matrix<int64_t, 3, 2>;

	/// MATH: fmat3x2_t
	using f32mat3x2_t pf_alignas(float32_t) = matrix<float32_t, 3, 2>;
	using f64mat3x2_t pf_alignas(float64_t) = matrix<float64_t, 3, 2>;

	/// MATH: s_fmat3x2_t
	using s_f32mat3x2_t pf_alignas_n(float32_t, 2) = matrix<float32_t, 3, 2>;
	using s_f64mat3x2_t pf_alignas_n(float64_t, 2) = matrix<float64_t, 3, 2>;
}

#endif // !PULSAR_MATH_MATRIX3X2_INL