/*! @file   matrix2.inl
 *  @author Louis-Quentin Noé (noe.louis-quentin@hotmail.fr)
 *  @brief	Specialization for square matrices of size 2.
 *  @date   31-05-2022
 *
 *  @copyright Copyright (c) 2023 - Pulsar Software
 *
 *  @since 0.1.1
 */

#ifndef PULSAR_MATH_MATRIX2_INL
#define PULSAR_MATH_MATRIX2_INL 1

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

	/// MATH: bmat2x2_t
	using bmat2_t
	pf_alignas(bool) = matrix<bool, 2, 2>;

	/// MATH: umat2x2_t
	using u8mat2_t pf_alignas(uint8_t)	 = matrix<uint8_t, 2, 2>;
	using u16mat2_t pf_alignas(uint16_t) = matrix<uint16_t, 2, 2>;
	using u32mat2_t pf_alignas(uint32_t) = matrix<uint32_t, 2, 2>;
	using u64mat2_t pf_alignas(uint64_t) = matrix<uint64_t, 2, 2>;

	/// MATH: s_umat2x2_t
	using s_u32mat2_t pf_alignas_n(uint32_t, 2) = matrix<uint32_t, 2, 2>;
	using s_u64mat2_t pf_alignas_n(uint64_t, 2) = matrix<uint64_t, 2, 2>;

	/// MATH: imat2x2_t
	using i8mat2_t pf_alignas(int8_t)		= matrix<int8_t, 2, 2>;
	using i16mat2_t pf_alignas(int16_t) = matrix<int16_t, 2, 2>;
	using i32mat2_t pf_alignas(int32_t) = matrix<int32_t, 2, 2>;
	using i64mat2_t pf_alignas(int64_t) = matrix<int64_t, 2, 2>;

	/// MATH: s_imat2x2_t
	using s_i32mat2_t pf_alignas_n(int32_t, 2) = matrix<int32_t, 2, 2>;
	using s_i64mat2_t pf_alignas_n(int64_t, 2) = matrix<int64_t, 2, 2>;

	/// MATH: fmat2x2_t
	using f32mat2_t pf_alignas(float32_t) = matrix<float32_t, 2, 2>;
	using f64mat2_t pf_alignas(float64_t) = matrix<float64_t, 2, 2>;

	/// MATH: s_fmat2x2_t
	using s_f32mat2_t pf_alignas_n(float32_t, 2) = matrix<float32_t, 2, 2>;
	using s_f64mat2_t pf_alignas_n(float64_t, 2) = matrix<float64_t, 2, 2>;
}

#endif // !PULSAR_MATH_MATRIX2_INL