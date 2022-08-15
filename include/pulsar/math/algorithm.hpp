/*! @file   algorithm.hpp
 *  @author Fluffy (noe.louis-quentin@hotmail.fr)
 *  @brief
 *  @date   31-05-2022
 *
 *  @copyright Copyright (c) 2022 - Pulsar Software
 *
 *  @since 0.1.1
 */

#ifndef PULSAR_MATH_ALGORITHM_HPP
#define PULSAR_MATH_ALGORITHM_HPP 1

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
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr row_vector<_Ty, _Num, _SIMD> transpose(
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
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr col_vector<_Ty, _Num, _SIMD> transpose(
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
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr matrix<_Ty, _ColNum, _RowNum, _SIMD> transpose(
				matrix<_Ty, _RowNum, _ColNum, _SIMD> const &__m) pf_attr_noexcept
		{
			matrix<_Ty, _ColNum, _RowNum, _SIMD> tmp;
			for (size_t i = 0; i < _ColNum; ++i)
			{
				for (size_t j = 0; j < _RowNum; ++j)
				{
					tmp[i][j] = __m[j][i];
				}
			}
			return tmp;
		}

		/// MATH: Determinant
		// TODO: Determinant

		/// MATH: Cast -> Vector
		template <typename _ToTy, typename _InTy>
			requires(is_vector_v<_ToTy> &&is_vector_v<_InTy> &&_ToTy::num == _InTy::num)
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr _ToTy cast(
				_InTy const &__r)
		pf_attr_noexcept
		{
			_ToTy tmp;
			for (size_t i = 0,
									m = std::max<size_t>(num_of_row_elem_v<_ToTy>, num_of_col_elem_v<_ToTy>);
					 i < m;
					 ++i)
			{
				tmp[i] = static_cast<_ToTy::type>(__r[i]);
			}
			return tmp;
		}

		/// MATH: Cast -> Matrix
		template <typename _ToTy, typename _InTy>
			requires(is_matrix_v<_ToTy> &&is_matrix_v<_InTy> &&_ToTy::row_num == _InTy::row_num && _ToTy::col_num == _ToTy::col_num)
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr _ToTy cast(
				_InTy const &__r)
		pf_attr_noexcept
		{
			_ToTy tmp;
			for (size_t i = 0; i < _ToTy::row_num; ++i)
			{
				for (size_t j = 0; j < _ToTy::col_num; ++j)
				{
					tmp[i][j] = static_cast<_ToTy::type>(__r[i][j]);
				}
			}
			return tmp;
		}
	}
}

#endif // !PULSAR_MATH_ALGORITHM_HPP