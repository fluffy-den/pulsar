/*! @file   matrix2x3.inl
 *  @author Louis-Quentin Noé (noe.louis-quentin@hotmail.fr)
 *  @brief	Specialization for matrices of 2 rows and 3 columns.
 *  @date   31-05-2022
 *
 *  @copyright Copyright (c) 2023 - Pulsar Software
 *
 *  @since 0.1.1
 */

#ifndef PULSAR_MATH_MATRIX2X3_INL
#define PULSAR_MATH_MATRIX2X3_INL 1

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


	/// MATH: bmat2x3_t
	using bmat2x3_t
	pf_alignas(bool) = matrix<bool, 2, 3>;

	/// MATH: umat2x3_t
	using u8mat2x3_t pf_alignas(uint8_t)	 = matrix<uint8_t, 2, 3>;
	using u16mat2x3_t pf_alignas(uint16_t) = matrix<uint16_t, 2, 3>;
	using u32mat2x3_t pf_alignas(uint32_t) = matrix<uint32_t, 2, 3>;
	using u64mat2x3_t pf_alignas(uint64_t) = matrix<uint64_t, 2, 3>;

	/// MATH: imat2x3_t
	using i8mat2x3_t pf_alignas(int8_t)		= matrix<int8_t, 2, 3>;
	using i16mat2x3_t pf_alignas(int16_t) = matrix<int16_t, 2, 3>;
	using i32mat2x3_t pf_alignas(int32_t) = matrix<int32_t, 2, 3>;
	using i64mat2x3_t pf_alignas(int64_t) = matrix<int64_t, 2, 3>;

	/// MATH: fmat2x3_t
	using f32mat2x3_t pf_alignas(float32_t) = matrix<float32_t, 2, 3>;
	using f64mat2x3_t pf_alignas(float64_t) = matrix<float64_t, 2, 3>;
}

#endif // !PULSAR_MATH_MATRIX2X3_INL