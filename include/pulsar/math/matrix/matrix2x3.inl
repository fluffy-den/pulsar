/*! @file   matrix2x3.inl
 *  @author Fluffy (noe.louis-quentin@hotmail.fr)
 *  @brief	Specialization for matrices of 2 rows and 3 columns.
 *  @date   31-05-2022
 *
 *  @copyright Copyright (c) 2022 - Pulsar Software
 *
 *  @since 0.1.1
 */

#ifndef PULSAR_MATH_MATRIX2X3_INL
#define PULSAR_MATH_MATRIX2X3_INL 1

// Include: Pulsar
#include "pulsar/intrin.hpp"

// Pulsar
namespace pul
{
	// Math
	namespace math
	{
		/// MATH: Matrix -> Definition
		template <typename _Ty, size_t _RowNum, size_t _ColNum, simd_align_t _SIMD>
			requires(
					std::is_arithmetic_v<_Ty> &&_RowNum > 1 && _ColNum > 1
					&& ((is_simd_alignable_v<_Ty, _ColNum> && _SIMD == SIMD_ALIGNED) || (_SIMD == SIMD_UNALIGNED)))
		union alignas(simd_alignment_of_v<_Ty, _ColNum, _SIMD>) matrix;

		/// MATH: bmat2x3u_t
		using bmat2x3u_t = matrix<bool, 2, 3, SIMD_UNALIGNED>;

		/// MATH: umat2x3u_t
		using u8mat2x3u_t	 = matrix<uint8_t, 2, 3, SIMD_UNALIGNED>;
		using u16mat2x3u_t = matrix<uint16_t, 2, 3, SIMD_UNALIGNED>;
		using u32mat2x3u_t = matrix<uint32_t, 2, 3, SIMD_UNALIGNED>;
		using u64mat2x3u_t = matrix<uint64_t, 2, 3, SIMD_UNALIGNED>;

		/// MATH: imat2x3u_t
		using i8mat2x3u_t	 = matrix<int8_t, 2, 3, SIMD_UNALIGNED>;
		using i16mat2x3u_t = matrix<int16_t, 2, 3, SIMD_UNALIGNED>;
		using i32mat2x3u_t = matrix<int32_t, 2, 3, SIMD_UNALIGNED>;
		using i64mat2x3u_t = matrix<int64_t, 2, 3, SIMD_UNALIGNED>;

		/// MATH: fmat2x3u_t
		using f32mat2x3u_t = matrix<float32_t, 2, 3, SIMD_UNALIGNED>;
		using f64mat2x3u_t = matrix<float64_t, 2, 3, SIMD_UNALIGNED>;
	}
}

#endif // !PULSAR_MATH_MATRIX2X3_INL