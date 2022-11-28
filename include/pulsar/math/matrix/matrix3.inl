/*! @file   matrix3.inl
 *  @author Fluffy (noe.louis-quentin@hotmail.fr)
 *  @brief	Specialization for square matrices of size 3.
 *  @date   31-05-2022
 *
 *  @copyright Copyright (c) 2022 - Pulsar Software
 *
 *  @since 0.1.1
 */

#ifndef PULSAR_MATH_MATRIX3_INL
#define PULSAR_MATH_MATRIX3_INL 1

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

	/// MATH: bmat3x3_t
	using bmat3_t
	pf_alignas(bool) = matrix<bool, 3, 3>;

	/// MATH: umat3x3_t
	using u8mat3_t pf_alignas(uint8_t)	 = matrix<uint8_t, 3, 3>;
	using u16mat3_t pf_alignas(uint16_t) = matrix<uint16_t, 3, 3>;
	using u32mat3_t pf_alignas(uint32_t) = matrix<uint32_t, 3, 3>;
	using u64mat3_t pf_alignas(uint64_t) = matrix<uint64_t, 3, 3>;

	/// MATH: imat3x3_t
	using i8mat3_t pf_alignas(int8_t)		= matrix<int8_t, 3, 3>;
	using i16mat3_t pf_alignas(int16_t) = matrix<int16_t, 3, 3>;
	using i32mat3_t pf_alignas(int32_t) = matrix<int32_t, 3, 3>;
	using i64mat3_t pf_alignas(int64_t) = matrix<int64_t, 3, 3>;

	/// MATH: fmat3x3_t
	using f32mat3_t pf_alignas(float32_t) = matrix<float32_t, 3, 3>;
	using f64mat3_t pf_alignas(float64_t) = matrix<float64_t, 3, 3>;
}

#endif // !PULSAR_MATH_MATRIX3_INL