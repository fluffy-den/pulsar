/*! @file   matrix4.inl
 *  @author Louis-Quentin Noé (noe.louis-quentin@hotmail.fr)
 *  @brief	Specialization for square matrices of size 4.
 *  @date   31-05-2022
 *
 *  @copyright Copyright (c) 2023 - Pulsar Software
 *
 *  @since 0.1.1
 */

#ifndef PULSAR_MATH_MATRIX4_INL
#define PULSAR_MATH_MATRIX4_INL 1

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

	/// MATH: bmat4x4_t
	using bmat4_t
	pf_alignas(bool) = matrix<bool, 4, 4>;

	/// MATH: umat4x4_t
	using u8mat4_t pf_alignas(uint8_t)	 = matrix<uint8_t, 4, 4>;
	using u16mat4_t pf_alignas(uint16_t) = matrix<uint16_t, 4, 4>;
	using u32mat4_t pf_alignas(uint32_t) = matrix<uint32_t, 4, 4>;
	using u64mat4_t pf_alignas(uint64_t) = matrix<uint64_t, 4, 4>;

	/// MATH: s_umat4x4_t
	using s_u8mat4_t pf_alignas_n(uint8_t, 4)		= matrix<uint8_t, 4, 4>;
	using s_u16mat4_t pf_alignas_n(uint16_t, 4) = matrix<uint16_t, 4, 4>;
	using s_u32mat4_t pf_alignas_n(uint32_t, 4) = matrix<uint32_t, 4, 4>;
	using s_u64mat4_t pf_alignas_n(uint64_t, 4) = matrix<uint64_t, 4, 4>;

	/// MATH: imat4x4_t
	using i8mat4_t pf_alignas(int8_t)		= matrix<int8_t, 4, 4>;
	using i16mat4_t pf_alignas(int16_t) = matrix<int16_t, 4, 4>;
	using i32mat4_t pf_alignas(int32_t) = matrix<int32_t, 4, 4>;
	using i64mat4_t pf_alignas(int64_t) = matrix<int64_t, 4, 4>;

	/// MATH: s_imat4x4_t
	using s_i8mat4_t pf_alignas_n(int8_t, 4)	 = matrix<uint8_t, 4, 4>;
	using s_i16mat4_t pf_alignas_n(int16_t, 4) = matrix<uint16_t, 4, 4>;
	using s_i32mat4_t pf_alignas_n(int32_t, 4) = matrix<uint32_t, 4, 4>;
	using s_i64mat4_t pf_alignas_n(int64_t, 4) = matrix<uint64_t, 4, 4>;

	/// MATH: fmat4x4_t
	using f32mat4_t pf_alignas(float32_t) = matrix<float32_t, 4, 4>;
	using f64mat4_t pf_alignas(float64_t) = matrix<float64_t, 4, 4>;

	/// MATH: s_fmat4x4_t
	using s_f32mat4_t pf_alignas_n(float32_t, 4) = matrix<float32_t, 4, 4>;
	using s_f64mat4_t pf_alignas_n(float64_t, 4) = matrix<float64_t, 4, 4>;
}

#endif // !PULSAR_MATH_MATRIX4_INL