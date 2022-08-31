/*! @file   matrix4x3.inl
 *  @author Fluffy (noe.louis-quentin@hotmail.fr)
 *  @brief	Specialization for matrices of 4 rows and 3 columns.
 *  @date   31-05-2022
 *
 *  @copyright Copyright (c) 2022 - Pulsar Software
 *
 *  @since 0.1.1
 */

#ifndef PULSAR_MATH_MATRIX4X3_INL
#define PULSAR_MATH_MATRIX4X3_INL 1

// Include: Pulsar
#include "pulsar/intrin.hpp"

// Pulsar
namespace pul
{
	/// MATH: Matrix -> Definition
	template <typename _Ty, size_t _RowNum, size_t _ColNum, simd_align_t _SIMD>
		requires(
				std::is_arithmetic_v<_Ty> &&_RowNum > 1 && _ColNum > 1
				&& ((is_simd_alignable_v<_Ty, _ColNum> && _SIMD == SIMD_ALIGNED) || (_SIMD == SIMD_UNALIGNED)))
	union alignas(simd_alignment_of_v<_Ty, _ColNum, _SIMD>) matrix;

	/// MATH: bmat4x3u_t
	using bmat4x3u_t = matrix<bool, 4, 3, SIMD_UNALIGNED>;

	/// MATH: umat4x3u_t
	using u8mat4x3u_t	 = matrix<uint8_t, 4, 3, SIMD_UNALIGNED>;
	using u16mat4x3u_t = matrix<uint16_t, 4, 3, SIMD_UNALIGNED>;
	using u32mat4x3u_t = matrix<uint32_t, 4, 3, SIMD_UNALIGNED>;
	using u64mat4x3u_t = matrix<uint64_t, 4, 3, SIMD_UNALIGNED>;

	/// MATH: imat4x3u_t
	using i8mat4x3u_t	 = matrix<int8_t, 4, 3, SIMD_UNALIGNED>;
	using i16mat4x3u_t = matrix<int16_t, 4, 3, SIMD_UNALIGNED>;
	using i32mat4x3u_t = matrix<int32_t, 4, 3, SIMD_UNALIGNED>;
	using i64mat4x3u_t = matrix<int64_t, 4, 3, SIMD_UNALIGNED>;

	/// MATH: fmat4x3u_t
	using f32mat4x3u_t = matrix<float32_t, 4, 3, SIMD_UNALIGNED>;
	using f64mat4x3u_t = matrix<float64_t, 4, 3, SIMD_UNALIGNED>;
}

#endif // !PULSAR_MATH_MATRIX4X3_INL