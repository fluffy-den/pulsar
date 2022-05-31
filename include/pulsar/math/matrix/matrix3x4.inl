/*! @file   matrix3x4.inl
 *  @author Fluffy (noe.louis-quentin@hotmail.fr)
 *  @brief	Specialization for matrices of 3 rows and 4 columns.
 *  @date   31-05-2022
 *
 *  @copyright Copyright (c) 2022 - Pulsar Software
 *
 *  @since 0.1.1
 */

#ifndef PULSAR_MATH_MATRIX3X4_INL
#define PULSAR_MATH_MATRIX3X4_INL 1

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

		/// MATH: bmat3x4u_t
		using bmat3x4u_t = matrix<bool, 3, 4, SIMD_UNALIGNED>;

		/// MATH: umat3x4_t
		using u16mat3x4_t = matrix<uint16_t, 3, 4, SIMD_ALIGNED>;
		using u32mat3x4_t = matrix<uint32_t, 3, 4, SIMD_ALIGNED>;
		using u64mat3x4_t = matrix<uint64_t, 3, 4, SIMD_ALIGNED>;

		/// MATH: umat3x4u_t
		using u8mat3x4u_t	 = matrix<uint8_t, 3, 4, SIMD_UNALIGNED>;
		using u16mat3x4u_t = matrix<uint16_t, 3, 4, SIMD_UNALIGNED>;
		using u32mat3x4u_t = matrix<uint32_t, 3, 4, SIMD_UNALIGNED>;
		using u64mat3x4u_t = matrix<uint64_t, 3, 4, SIMD_UNALIGNED>;

		/// MATH: imat3x4_t
		using i16mat3x4_t = matrix<int16_t, 3, 4, SIMD_ALIGNED>;
		using i32mat3x4_t = matrix<int32_t, 3, 4, SIMD_ALIGNED>;
		using i64mat3x4_t = matrix<int64_t, 3, 4, SIMD_ALIGNED>;

		/// MATH: imat3x4u_t
		using i8mat3x4u_t	 = matrix<int8_t, 3, 4, SIMD_UNALIGNED>;
		using i16mat3x4u_t = matrix<int16_t, 3, 4, SIMD_UNALIGNED>;
		using i32mat3x4u_t = matrix<int32_t, 3, 4, SIMD_UNALIGNED>;
		using i64mat3x4u_t = matrix<int64_t, 3, 4, SIMD_UNALIGNED>;

		/// MATH: fmat3x4_t
		using f32mat3x4_t = matrix<float32_t, 3, 4, SIMD_ALIGNED>;
		using f64mat3x4_t = matrix<float64_t, 3, 4, SIMD_ALIGNED>;

		/// MATH: fmat3x4u_t
		using f32mat3x4u_t = matrix<float32_t, 3, 4, SIMD_UNALIGNED>;
		using f64mat3x4u_t = matrix<float64_t, 3, 4, SIMD_UNALIGNED>;
	}
}

#endif // !PULSAR_MATH_MATRIX3X4_INL