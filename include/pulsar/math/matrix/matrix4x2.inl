/*! @file   matrix4x2.inl
 *  @author Fluffy (noe.louis-quentin@hotmail.fr)
 *  @brief	Specialization for matrices of 4 rows and 2 columns.
 *  @date   31-05-2022
 *
 *  @copyright Copyright (c) 2022 - Pulsar Software
 *
 *  @since 0.1.1
 */

#ifndef PULSAR_MATH_MATRIX4X2_INL
#define PULSAR_MATH_MATRIX4X2_INL 1

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

	/// MATH: bmat4x2_t
	using bmat4x2_t
	pf_alignas(bool) = matrix<bool, 4, 2>;

	/// MATH: umat4x2_t
	using u8mat4x2_t pf_alignas(uint8_t)	 = matrix<uint8_t, 4, 2>;
	using u16mat4x2_t pf_alignas(uint16_t) = matrix<uint16_t, 4, 2>;
	using u32mat4x2_t pf_alignas(uint32_t) = matrix<uint32_t, 4, 2>;
	using u64mat4x2_t pf_alignas(uint64_t) = matrix<uint64_t, 4, 2>;

	/// MATH: s_umat4x2_t
	using s_u32mat4x2_t pf_alignas_n(uint32_t, 2) = matrix<uint32_t, 4, 2>;
	using s_u64mat4x2_t pf_alignas_n(uint64_t, 2) = matrix<uint64_t, 4, 2>;

	/// MATH: imat4x2_t
	using i8mat4x2_t pf_alignas(int8_t)		= matrix<int8_t, 4, 2>;
	using i16mat4x2_t pf_alignas(int16_t) = matrix<int16_t, 4, 2>;
	using i32mat4x2_t pf_alignas(int32_t) = matrix<int32_t, 4, 2>;
	using i64mat4x2_t pf_alignas(int64_t) = matrix<int64_t, 4, 2>;

	/// MATH: s_imat4x2_t
	using s_i32mat4x2_t pf_alignas_n(int32_t, 2) = matrix<uint32_t, 4, 2>;
	using s_i64mat4x2_t pf_alignas_n(int64_t, 2) = matrix<uint64_t, 4, 2>;

	/// MATH: fmat4x2_t
	using f32mat4x2_t pf_alignas(float32_t) = matrix<float32_t, 4, 2>;
	using f64mat4x2_t pf_alignas(float64_t) = matrix<float64_t, 4, 2>;

	/// MATH: s_fmat4x2_t
	using s_f32mat4x2_t pf_alignas_n(float32_t, 2) = matrix<float32_t, 4, 2>;
	using s_f64mat4x2_t pf_alignas_n(float64_t, 2) = matrix<float64_t, 4, 2>;
}

#endif // !PULSAR_MATH_MATRIX4X2_INL