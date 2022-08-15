/*! @file   vector.hpp
 *  @author Fluffy (noe.louis-quentin@hotmail.fr)
 *  @brief	Definitions of vectors.
 *  @date   24-05-2022
 *
 *  @copyright Copyright (c) 2022 - Pulsar Software
 *
 *  @since 0.1.1
 */

#ifndef PULSAR_MATH_VECTOR_HPP
#define PULSAR_MATH_VECTOR_HPP 1

// Include: Pulsar
#include "pulsar/math/vector/col_vector.hpp"
#include "pulsar/math/vector/row_vector.hpp"

// Pulsar
namespace pul
{
	// Math
	namespace math
	{
		/// MATH: Is vector?
		template <typename _Ty>
		struct is_vector: public std::integral_constant<bool, is_row_vector_v<_Ty> || is_col_vector_v<_Ty>>
		{};
		template <typename _Ty>
		pf_decl_static pf_decl_constexpr bool is_vector_v = is_vector<_Ty>::value;

		/// MATH: Vector -> Concept
		template <typename _Ty>
		concept vector_c = is_vector_v<_Ty>;

		/// MATH: Vector -> Operator*
		template <typename _Ty, size_t _Num, simd_align_t _LSIMD, simd_align_t _RSIMD>
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr _Ty operator*(
				row_vector<_Ty, _Num, _LSIMD> const &__l,
				col_vector<_Ty, _Num, _RSIMD> const &__r) pf_attr_noexcept
		{
			_Ty tmp = 0;
			for (size_t k = 0; k < _Num; ++k)
			{
				tmp += __l[k] * __r[k];
			}
			return tmp;
		}

		/// MATH: Vector -> Operator/
		template <typename _Ty, size_t _Num, simd_align_t _LSIMD, simd_align_t _RSIMD>
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr _Ty operator/(
				row_vector<_Ty, _Num, _LSIMD> const &__l,
				col_vector<_Ty, _Num, _RSIMD> const &__r) pf_attr_noexcept
		{
			_Ty tmp = 0;
			for (size_t k = 0; k < _Num; ++k)
			{
				tmp += __l[k] / __r[k];
			}
			return tmp;
		}
	}
}

#endif // !PULSAR_MATH_VECTOR_HPP