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
	/// MATH: Transpose
	/*! @brief Apply the transpose of a matrix.
	 *
	 *  @tparam _Ty		  Arithmetic type of the matrix.
	 *  @tparam _RowNum Number of rows of the matrix.
	 *  @tparam _ColNum Number of columns of the matrix.
	 *  @param[in] __m 	Constant reference on a matrix.
	 *  @return Transposed matrix.
	 */
	template <typename _Ty, size_t _RowNum, size_t _ColNum>
	pf_hint_nodiscard pf_decl_constexpr matrix<_Ty, _ColNum, _RowNum> pf_alignas_n(_Ty, _RowNum)
	transpose(
		matrix<_Ty, _RowNum, _ColNum> const & __m) pf_attr_noexcept
	{
		matrix<_Ty, _ColNum, _RowNum> pf_alignas_n(_Ty, _RowNum) tmp;
		for(size_t i = 0; i < _ColNum; ++i)
		{
			for(size_t j = 0; j < _RowNum; ++j)
			{
				tmp[i][j] = __m[j][i];
			}
		}
		return tmp;
	}

	/// MATH: Determinant
	template <typename _Ty, size_t _Num>
	pf_hint_nodiscard pf_decl_constexpr float64_t pf_alignas_n(_Ty, _Num)
	determinant(
		matrix<_Ty, _Num, _Num> const & __m) pf_attr_noexcept
	{
		// TODO: https://www.geeksforgeeks.org/determinant-of-a-matrix/
	}

	/// MATH: Inverse
	template <typename _Ty, size_t _Num>
	pf_hint_nodiscard pf_decl_constexpr matrix<_Ty, _Num, _Num> pf_alignas_n(_Ty, _Num)
	inverse(
		matrix<_Ty, _Num, _Num> const & __m) pf_attr_noexcept
	{}

	/// MATH: Cast -> Vector
	template <typename _ToTy, typename _InTy>
	pf_hint_nodiscard pf_decl_constexpr _ToTy math_cast(
		_InTy const &__r)
	pf_attr_noexcept requires(
		is_vector_v<_ToTy> &&
		is_vector_v<_InTy> &&
		elem_numof_v<_ToTy> == elem_numof_v<_InTy>)
	{
		_ToTy tmp;
		for(size_t i = 0,
				m = elem_numof_v<_ToTy>;
				i < m;
				++i)
		{
			tmp[i] = static_cast<elem_typeof_t<_ToTy>>(__r[i]);
		}
		return tmp;
	}

	/// MATH: Cast -> Matrix
	template <typename _ToTy, typename _InTy>
	pf_hint_nodiscard pf_decl_constexpr _ToTy math_cast(
		_InTy const &__r)
	pf_attr_noexcept requires(
		is_matrix_v<_ToTy> &&
		is_matrix_v<_InTy> &&
		elem_row_numof_v<_ToTy> == elem_row_numof_v<_InTy> &&
		elem_col_numof_v<_ToTy> == elem_col_numof_v<_InTy>)
	{
		_ToTy tmp;
		for(size_t i = 0; i < elem_row_numof_v<_ToTy>; ++i)
		{
			for(size_t j = 0; j < elem_col_numof_v<_ToTy>; ++j)
			{
				tmp[i][j] = static_cast<elem_typeof_t<_ToTy>>(__r[i][j]);
			}
		}
		return tmp;
	}
}

#endif // !PULSAR_MATH_ALGORITHM_HPP