/*! @file   matrix4x3.inl
 *  @author Louis-Quentin Noé (noe.louis-quentin@hotmail.fr)
 *  @brief	Specialization for matrices of 4 rows and 3 columns.
 *  @date   31-05-2022
 *
 *  @copyright Copyright (c) 2023 - Pulsar Software
 *
 *  @since 0.1.1
 */

#ifndef PULSAR_MATH_MATRIX4X3_INL
#define PULSAR_MATH_MATRIX4X3_INL 1

// Include: Pulsar
#include "pulsar/pulsar.hpp"

// Pulsar
namespace pul
{
	/// MATH: Matrix -> Definition
	template <typename _Ty, size_t _RowNum, size_t _ColNum>
	requires(
		std::is_arithmetic_v<_Ty>
		&& _RowNum > 1 && _ColNum > 1)
	union matrix;

	/// MATH: bmat4x3_t
	using bmat4x3_t
	pf_alignas(bool) = matrix<bool, 4, 3>;

	/// MATH: umat4x3_t
	using u8mat4x3_t pf_alignas(uint8_t)	 = matrix<uint8_t, 4, 3>;
	using u16mat4x3_t pf_alignas(uint16_t) = matrix<uint16_t, 4, 3>;
	using u32mat4x3_t pf_alignas(uint32_t) = matrix<uint32_t, 4, 3>;
	using u64mat4x3_t pf_alignas(uint64_t) = matrix<uint64_t, 4, 3>;

	/// MATH: imat4x3_t
	using i8mat4x3_t pf_alignas(int8_t)		= matrix<int8_t, 4, 3>;
	using i16mat4x3_t pf_alignas(int16_t) = matrix<int16_t, 4, 3>;
	using i32mat4x3_t pf_alignas(int32_t) = matrix<int32_t, 4, 3>;
	using i64mat4x3_t pf_alignas(int64_t) = matrix<int64_t, 4, 3>;

	/// MATH: fmat4x3_t
	using f32mat4x3_t pf_alignas(float32_t) = matrix<float32_t, 4, 3>;
	using f64mat4x3_t pf_alignas(float64_t) = matrix<float64_t, 4, 3>;
}

#endif // !PULSAR_MATH_MATRIX4X3_INL