/*! @file   functions.hpp
 *  @author Fluffy (noe.louis-quentin@hotmail.fr)
 *  @brief
 *  @date   31-05-2022
 *
 *  @copyright Copyright (c) 2022 - Pulsar Software
 *
 *  @since 0.1.1
 */

#ifndef PULSAR_MATH_FUNCTIONS_HPP
#define PULSAR_MATH_FUNCTIONS_HPP 1

// Include: Pulsar
#include "pulsar/math/matrix.hpp"
#include "pulsar/math/vector.hpp"

// Pulsar
namespace pul
{
	// Math
	namespace math
	{
		/// MATH: Transpose
		/*! @brief Apply the transpose of a column vector to a row vector.
		 *
		 *  @tparam _Ty		Arithmetic type of the vector.
		 *  @tparam _Num	Number of elements of the vector.
		 *  @tparam _SIMD SIMD alignment.
		 *  @param[in] __v Constant reference on a column vector.
		 *  @return Transposed row vector.
		 */
		template <typename _Ty, size_t _Num, simd_align_t _SIMD>
		pf_hint_nodiscard pf_decl_constexpr row_vector<_Ty, _Num, _SIMD> transpose(
				col_vector<_Ty, _Num, _SIMD> const &__v) pf_attr_noexcept
		{
			row_vector<_Ty, _Num, _SIMD> tmp;
			for (size_t i = 0; i < _Num; ++i)
			{
				tmp[i] = __v[i];
			}
			return tmp;
		}
		/*! @brief Apply the transpose of a row vector to a column vector.
		 *
		 *  @tparam _Ty		Arithmetic type of the vector.
		 *  @tparam _Num	Number of elements of the vector.
		 *  @tparam _SIMD SIMD alignment.
		 *  @param[in] __v Constant reference on a row vector.
		 *  @return Transposed column vector.
		 */
		template <typename _Ty, size_t _Num, simd_align_t _SIMD>
		pf_hint_nodiscard pf_decl_constexpr col_vector<_Ty, _Num, _SIMD> transpose(
				row_vector<_Ty, _Num, _SIMD> const &__v) pf_attr_noexcept
		{
			col_vector<_Ty, _Num, _SIMD> tmp;
			for (size_t i = 0; i < _Num; ++i)
			{
				tmp[i] = __v[i];
			}
			return tmp;
		}
		/*! @brief Apply the transpose of a matrix.
		 *
		 *  @tparam _Ty		  Arithmetic type of the matrix.
		 *  @tparam _RowNum Number of rows of the matrix.
		 *  @tparam _ColNum Number of columns of the matrix.
		 *  @tparam _SIMD		SIMD alignment.
		 *  @param[in] __m 	Constant reference on a matrix.
		 *  @return Transposed matrix.
		 */
		template <typename _Ty, size_t _RowNum, size_t _ColNum, simd_align_t _SIMD>
		pf_hint_nodiscard pf_decl_constexpr matrix<_Ty, _ColNum, _RowNum, _SIMD> transpose(
				matrix<_Ty, _RowNum, _ColNum, _SIMD> const &__m) pf_attr_noexcept
		{
			matrix<_Ty, _RowNum, _ColNum, _SIMD> tmp;
			for (size_t i = 0; i < _RowNum; ++i)
			{
				for (size_t j = 0; j < _RowNum; ++j)
				{
					tmp[i][j] = __m[i][j];
				}
			}
			return tmp;
		}

		/// MATH: Determinant
		// TODO: Determinant
	}
}

#endif // !PULSAR_MATH_FUNCTIONS_HPP