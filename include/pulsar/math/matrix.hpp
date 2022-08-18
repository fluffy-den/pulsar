/*! @file   matrix.hpp
 *  @author Fluffy (noe.louis-quentin@hotmail.fr)
 *  @brief	Definition of matrices.
 *  @date   24-05-2022
 *
 *  @copyright Copyright (c) 2022 - Pulsar Software
 *
 *  @since 0.1.1
 */

#ifndef PULSAR_MATH_MATRIX_HPP
#define PULSAR_MATH_MATRIX_HPP 1

// Include: Pulsar
#include "pulsar/math/vector.hpp"

// Include: C++
#include <array>
#include <type_traits>

// Pulsar
namespace pul
{
	// Mathematics
	namespace math
	{
		/// MATH: Matrix
		template <typename _Ty, size_t _RowNum, size_t _ColNum, simd_align_t _SIMD = simd_select_v<_Ty, _ColNum>>
			requires(
					std::is_arithmetic_v<_Ty> &&_RowNum > 1 && _ColNum > 1
					&& ((is_simd_alignable_v<_Ty, _ColNum> && _SIMD == SIMD_ALIGNED) || (_SIMD == SIMD_UNALIGNED)))
		union alignas(simd_alignment_of_v<_Ty, _ColNum, _SIMD>) matrix
		{
			template <typename _TyF, size_t _RowNumF, size_t _ColNumF, simd_align_t _SIMDF>
			pf_decl_friend union matrix;

		public:
			using type																			 = _Ty;
			pf_decl_static pf_decl_constexpr size_t row_num	 = _RowNum;
			pf_decl_static pf_decl_constexpr size_t col_num	 = _ColNum;
			pf_decl_static pf_decl_constexpr bool simd_value = _SIMD;

			/// Constructors
			pf_decl_inline pf_decl_constexpr matrix(
					_Ty __val = static_cast<_Ty>(0)) pf_attr_noexcept
					: store_{ __val }
			{}
			template <simd_align_t _RSIMD>
			pf_decl_inline pf_decl_constexpr matrix(
					matrix<_Ty, _RowNum, _ColNum, _RSIMD> const &__r) pf_attr_noexcept
					: store_(__r.store_)
			{}
			template <size_t _RNum>
				requires(_RNum == _RowNum)
			pf_decl_inline pf_decl_constexpr matrix(
					row_vector<_Ty, _ColNum, simd_select_v<_Ty, _ColNum>> const (&__l)[_RNum])
			pf_attr_noexcept
			{
				for (size_t i = 0; i < _RowNum; ++i)
				{
					this->store_[i] = __l[i];
				}
			}

			/// Operator[]
			pf_decl_inline pf_decl_constexpr row_vector<_Ty, _ColNum, SIMD_UNALIGNED> &operator[](
					size_t __index) pf_attr_noexcept
			{
				return this->store_[__index];
			}
			pf_decl_inline pf_decl_constexpr const row_vector<_Ty, _ColNum, SIMD_UNALIGNED> &operator[](
					size_t __index) const pf_attr_noexcept
			{
				return this->store_[__index];
			}

			/// Operator+=
			template <simd_align_t _RSIMD>
			pf_decl_inline pf_decl_constexpr matrix<_Ty, _RowNum, _ColNum, _SIMD> &operator+=(
					matrix<_Ty, _RowNum, _ColNum, _RSIMD> const &__r) pf_attr_noexcept
			{
				for (size_t i = 0; i < _RowNum; ++i)
				{
					this->store_[i] += __r[i];
				}
				return *this;
			}

			/// Operator-=
			template <simd_align_t _RSIMD>
			pf_decl_inline pf_decl_constexpr matrix<_Ty, _RowNum, _ColNum, _SIMD> &operator-=(
					matrix<_Ty, _RowNum, _ColNum, _RSIMD> const &__r) pf_attr_noexcept
			{
				for (size_t i = 0; i < _RowNum; ++i)
				{
					this->store_[i] -= __r[i];
				}
				return *this;
			}

			/// Operator*=
			pf_decl_inline pf_decl_constexpr matrix<_Ty, _RowNum, _ColNum, _SIMD> &operator*=(
					_Ty __val) pf_attr_noexcept
			{
				for (size_t i = 0; i < _RowNum; ++i)
				{
					this->store_[i] *= __val;
				}
				return *this;
			}

			/// Operator/=
			pf_decl_inline pf_decl_constexpr matrix<_Ty, _RowNum, _ColNum, _SIMD> &operator/=(
					_Ty __val) pf_attr_noexcept
			{
				for (size_t i = 0; i < _RowNum; ++i)
				{
					this->store_[i] /= __val;
				}
				return *this;
			}

			/// Operator%=
			template <simd_align_t _RSIMD>
			pf_decl_inline pf_decl_constexpr matrix<_Ty, _RowNum, _ColNum, _SIMD> &operator%=(
					matrix<_Ty, _RowNum, _ColNum, _RSIMD> const &__r) pf_attr_noexcept requires(std::is_integral_v<_Ty>)
			{
				for (size_t i = 0; i < _RowNum; ++i)
				{
					for (size_t j = 0; j < _ColNum; ++j)
					{
						this->store_[i][j] %= __r[i][j];
					}
				}
				return *this;
			}

			/// Operator&=
			template <simd_align_t _RSIMD>
			pf_decl_inline pf_decl_constexpr matrix<_Ty, _RowNum, _ColNum, _SIMD> &operator&=(
					matrix<_Ty, _RowNum, _ColNum, _RSIMD> const &__r) pf_attr_noexcept requires(std::is_integral_v<_Ty>)
			{
				for (size_t i = 0; i < _RowNum; ++i)
				{
					for (size_t j = 0; j < _ColNum; ++j)
					{
						this->store_[i][j] &= __r[i][j];
					}
				}
				return *this;
			}

			/// Operator|=
			template <simd_align_t _RSIMD>
			pf_decl_inline pf_decl_constexpr matrix<_Ty, _RowNum, _ColNum, _SIMD> &operator|=(
					matrix<_Ty, _RowNum, _ColNum, _RSIMD> const &__r) pf_attr_noexcept requires(std::is_integral_v<_Ty>)
			{
				for (size_t i = 0; i < _RowNum; ++i)
				{
					for (size_t j = 0; j < _ColNum; ++j)
					{
						this->store_[i][j] |= __r[i][j];
					}
				}
				return *this;
			}

			/// Operator^=
			template <simd_align_t _RSIMD>
			pf_decl_inline pf_decl_constexpr matrix<_Ty, _RowNum, _ColNum, _SIMD> &operator^=(
					matrix<_Ty, _RowNum, _ColNum, _RSIMD> const &__r) pf_attr_noexcept requires(std::is_integral_v<_Ty>)
			{
				for (size_t i = 0; i < _RowNum; ++i)
				{
					this->store_[i] ^= __r;
				}
				return *this;
			}

			/// Operator>>=
			template <simd_align_t _RSIMD>
			pf_decl_inline pf_decl_constexpr matrix<_Ty, _RowNum, _ColNum, _SIMD> &operator>>=(
					matrix<_Ty, _RowNum, _ColNum, _RSIMD> const &__r) pf_attr_noexcept requires(std::is_integral_v<_Ty>)
			{
				for (size_t i = 0; i < _RowNum; ++i)
				{
					for (size_t j = 0; j < _ColNum; ++j)
					{
						this->store_[i][j] >>= __r[i][j];
					}
				}
				return *this;
			}

			/// Operator<<=
			template <simd_align_t _RSIMD>
			pf_decl_inline pf_decl_constexpr matrix<_Ty, _RowNum, _ColNum, _SIMD> &operator<<=(
					matrix<_Ty, _RowNum, _ColNum, _RSIMD> const &__r) pf_attr_noexcept requires(std::is_integral_v<_Ty>)
			{
				for (size_t i = 0; i < _RowNum; ++i)
				{
					for (size_t j = 0; j < _ColNum; ++j)
					{
						this->store_[i][j] <<= __r[i][j];
					}
				}
				return *this;
			}

			/// Operator==
			template <simd_align_t _RSIMD>
			pf_hint_nodiscard pf_decl_inline pf_decl_constexpr bool operator==(
					matrix<_Ty, _RowNum, _ColNum, _RSIMD> const &__r) pf_attr_noexcept
			{
				for (size_t i = 0; i < _RowNum; ++i)
				{
					for (size_t j = 0; j < _ColNum; ++j)
					{
						if (this->store_[i][j] != __r[i][j])
							return false;
					}
				}
				return true;
			}

			/// Compare
			template <simd_align_t _RSIMD>
			pf_hint_nodiscard pf_decl_inline pf_decl_constexpr matrix<bool, _RowNum, _ColNum, simd_select_v<bool, _ColNum>> vals_equal(
					matrix<_Ty, _RowNum, _ColNum, _RSIMD> const &__r) const pf_attr_noexcept
			{
				matrix<bool, _RowNum, _ColNum, simd_select_v<bool, _ColNum>> tmp;
				for (size_t i = 0; i < _RowNum; ++i)
				{
					for (size_t j = 0; j < _ColNum; ++j)
					{
						tmp[i][j] = this->store_[i][j] == __r[i][j];
					}
				}
				return tmp;
			}
			template <simd_align_t _RSIMD>
			pf_hint_nodiscard pf_decl_inline pf_decl_constexpr matrix<bool, _RowNum, _ColNum, simd_select_v<bool, _ColNum>> vals_greater(
					matrix<_Ty, _RowNum, _ColNum, _RSIMD> const &__r) const pf_attr_noexcept
			{
				matrix<bool, _RowNum, _ColNum, simd_select_v<bool, _ColNum>> tmp;
				for (size_t i = 0; i < _RowNum; ++i)
				{
					for (size_t j = 0; j < _ColNum; ++j)
					{
						tmp[i][j] = this->store_[i][j] > __r[i][j];
					}
				}
				return tmp;
			}
			template <simd_align_t _RSIMD>
			pf_hint_nodiscard pf_decl_inline pf_decl_constexpr matrix<bool, _RowNum, _ColNum, simd_select_v<bool, _ColNum>> vals_greater_equal(
					matrix<_Ty, _RowNum, _ColNum, _RSIMD> const &__r) const pf_attr_noexcept
			{
				matrix<bool, _RowNum, _ColNum, simd_select_v<bool, _ColNum>> tmp;
				for (size_t i = 0; i < _RowNum; ++i)
				{
					for (size_t j = 0; j < _ColNum; ++j)
					{
						tmp[i][j] = this->store_[i][j] >= __r[i][j];
					}
				}
				return tmp;
			}
			template <simd_align_t _RSIMD>
			pf_hint_nodiscard pf_decl_inline pf_decl_constexpr matrix<bool, _RowNum, _ColNum, simd_select_v<bool, _ColNum>> vals_smaller(
					matrix<_Ty, _RowNum, _ColNum, _RSIMD> const &__r) const pf_attr_noexcept
			{
				matrix<bool, _RowNum, _ColNum, simd_select_v<bool, _ColNum>> tmp;
				for (size_t i = 0; i < _RowNum; ++i)
				{
					for (size_t j = 0; j < _ColNum; ++j)
					{
						tmp[i][j] = this->store_[i][j] < __r[i][j];
					}
				}
				return tmp;
			}
			template <simd_align_t _RSIMD>
			pf_hint_nodiscard pf_decl_inline pf_decl_constexpr matrix<bool, _RowNum, _ColNum, simd_select_v<bool, _ColNum>> vals_smaller_equal(
					matrix<_Ty, _RowNum, _ColNum, _RSIMD> const &__r) const pf_attr_noexcept
			{
				matrix<bool, _RowNum, _ColNum, simd_select_v<bool, _ColNum>> tmp;
				for (size_t i = 0; i < _RowNum; ++i)
				{
					for (size_t j = 0; j < _ColNum; ++j)
					{
						tmp[i][j] = this->store_[i][j] <= __r[i][j];
					}
				}
				return tmp;
			}
			template <simd_align_t _RSIMD>
			pf_hint_nodiscard pf_decl_inline pf_decl_constexpr matrix<bool, _RowNum, _ColNum, simd_select_v<bool, _ColNum>> vals_not_equal(
					matrix<_Ty, _RowNum, _ColNum, _RSIMD> const &__r) const pf_attr_noexcept
			{
				matrix<bool, _RowNum, _ColNum, simd_select_v<bool, _ColNum>> tmp;
				for (size_t i = 0; i < _RowNum; ++i)
				{
					for (size_t j = 0; j < _ColNum; ++j)
					{
						tmp[i][j] = this->store_[i][j] != __r[i][j];
					}
				}
				return tmp;
			}

			/// Data
			pf_hint_nodiscard pf_decl_inline pf_decl_constexpr _Ty *data() pf_attr_noexcept
			{
				return this->store_.data()->data();
			}
			pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const _Ty *data() const pf_attr_noexcept
			{
				return this->store_.data()->data();
			}

			/// Num of Row
			pf_hint_nodiscard pf_decl_static pf_decl_inline pf_decl_constexpr size_t num_row_elem() pf_attr_noexcept
			{
				return _RowNum;
			}

			/// Num of Col
			pf_hint_nodiscard pf_decl_static pf_decl_inline pf_decl_constexpr size_t num_col_elem() pf_attr_noexcept
			{
				return _ColNum;
			}

			/// Is SIMD aligned?
			pf_hint_nodiscard pf_decl_static pf_decl_inline pf_decl_constexpr bool is_simd_aligned() pf_attr_noexcept
			{
				return _SIMD == SIMD_ALIGNED;
			}

		private:
			std::array<row_vector<_Ty, _ColNum, SIMD_UNALIGNED>, _RowNum> store_;
		};

		/// MATH: Matrix -> SFINAE
		// Is matrix?
		template <typename _MatTy>
		struct is_matrix: public std::false_type
		{};
		template <typename _Ty, size_t _RowNum, size_t _ColNum, simd_align_t _SIMD>
		struct is_matrix<matrix<_Ty, _RowNum, _ColNum, _SIMD>>: public std::true_type
		{};
		template <typename _MatTy>
		pf_decl_static pf_decl_constexpr bool is_matrix_v = is_matrix<_MatTy>::value;

		// Is square matrix?
		template <typename _MatTy>
		struct is_square_matrix: public std::false_type
		{};
		template <typename _Ty, size_t _Num, simd_align_t _SIMD>
		struct is_square_matrix<matrix<_Ty, _Num, _Num, _SIMD>>: public std::true_type
		{};
		template <typename _MatTy>
		pf_decl_static pf_decl_constexpr bool is_square_matrix_v = is_square_matrix<_MatTy>::value;

		/// MATH: Matrix -> Square Concept
		template <typename _MatTy>
		concept square_matrix_c = is_square_matrix_v<_MatTy>;

		/// MATH: Matrix -> Concept
		template <typename _MatTy>
		concept matric_c = is_matrix_v<_MatTy>;

		/// MATH: Matrix -> Square
		template <typename _Ty, size_t _Num, simd_align_t _SIMD = simd_select_v<_Ty, _Num>>
		using square_matrix = matrix<_Ty, _Num, _Num, _SIMD>;

		/// MATH: Matrix -> Alias
		template <typename _Ty, size_t _RowNum, size_t _ColNum = _RowNum, simd_align_t _SIMD = simd_select_v<_Ty, _ColNum>>
		using mat = matrix<_Ty, _RowNum, _ColNum, _SIMD>;

		/// MATH: Matrix -> Identity
		template <size_t _Num, typename _Ty, simd_align_t _SIMD = simd_select_v<_Ty, _Num>>
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr matrix<_Ty, _Num, _Num, _SIMD> identity(
				_Ty __val = static_cast<_Ty>(1)) pf_attr_noexcept
		{
			matrix<_Ty, _Num, _Num, _SIMD> mat;
			for (size_t i = 0; i < _Num; ++i)
			{
				mat[i][i] = __val;
			}
			return mat;
		}
		template <square_matrix_c _SquareMat>
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr _SquareMat identity(
				typename _SquareMat::type __val = static_cast<typename _SquareMat::type>(1)) pf_attr_noexcept
		{
			_SquareMat mat;
			for (size_t i = 0; i < mat.num_row_elem(); ++i)
			{
				mat[i][i] = __val;
			}
			return mat;
		}

		/// MATH: Matrix -> Operator+
		template <typename _Ty, size_t _RowNum, size_t _ColNum, simd_align_t _LSIMD, simd_align_t _RSIMD>
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr matrix<_Ty, _RowNum, _ColNum, simd_select_v<_Ty, _ColNum>> operator+(
				matrix<_Ty, _RowNum, _ColNum, _LSIMD> const &__l,
				matrix<_Ty, _RowNum, _ColNum, _RSIMD> const &__r) pf_attr_noexcept
		{
			matrix<_Ty, _RowNum, _ColNum, simd_select_v<_Ty, _ColNum>> tmp(__l);
			return tmp += __r;
		}

		/// MATH: Matrix -> Operator-
		template <typename _Ty, size_t _RowNum, size_t _ColNum, simd_align_t _LSIMD, simd_align_t _RSIMD>
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr matrix<_Ty, _RowNum, _ColNum, simd_select_v<_Ty, _ColNum>> operator-(
				matrix<_Ty, _RowNum, _ColNum, _LSIMD> const &__l,
				matrix<_Ty, _RowNum, _ColNum, _RSIMD> const &__r) pf_attr_noexcept
		{
			matrix<_Ty, _RowNum, _ColNum, simd_select_v<_Ty, _ColNum>> tmp(__l);
			return tmp -= __r;
		}

		/// MATH: Matrix -> Operator*
		template <typename _Ty, size_t _RowNum, size_t _ColNum, simd_align_t _SIMD>
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr matrix<_Ty, _RowNum, _ColNum, _SIMD> operator*(
				matrix<_Ty, _RowNum, _ColNum, _SIMD> const &__l,
				_Ty __val) pf_attr_noexcept
		{
			matrix<_Ty, _RowNum, _ColNum, _SIMD> tmp(__l);
			return tmp *= __val;
		}
		template <typename _Ty, size_t _RowNum, size_t _ColNum, simd_align_t _SIMD>
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr matrix<_Ty, _RowNum, _ColNum, _SIMD> operator*(
				_Ty __val,
				matrix<_Ty, _RowNum, _ColNum, _SIMD> const &__l) pf_attr_noexcept
		{
			matrix<_Ty, _RowNum, _ColNum, _SIMD> tmp(__l);
			return tmp *= __val;
		}
		template <typename _Ty, size_t _LRowNum, size_t _LColNum, simd_align_t _LSIMD, size_t _RColNum, simd_align_t _RSIMD>
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr matrix<_Ty, _LRowNum, _RColNum, simd_select_v<_Ty, _LColNum>> operator*(
				matrix<_Ty, _LRowNum, _LColNum, _LSIMD> const &__l,
				matrix<_Ty, _LColNum, _RColNum, _RSIMD> const &__r) pf_attr_noexcept
		{
			matrix<_Ty, _LRowNum, _LColNum, simd_select_v<_Ty, _LColNum>> tmp;
			for (size_t i = 0; i < _LRowNum; ++i)
			{
				for (size_t j = 0; j < _RColNum; ++j)
				{
					for (size_t k = 0; k < _LColNum; ++k)
					{
						tmp[i][j] += __l[i][k] * __r[k][j];
					}
				}
			}
			return tmp;
		}
		template <typename _Ty, size_t _LNum, simd_align_t _LSIMD, size_t _RNum, simd_align_t _RSIMD>
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr matrix<_Ty, _LNum, _RNum, simd_select_v<_Ty, _RNum>> operator*(
				col_vector<_Ty, _LNum, _LSIMD> const &__l,
				row_vector<_Ty, _RNum, _RSIMD> const &__r) pf_attr_noexcept
		{
			matrix<_Ty, _LNum, _RNum, simd_select_v<_Ty, _RNum>> tmp;
			for (size_t i = 0; i < _LNum; ++i)
			{
				for (size_t j = 0; j < _RNum; ++j)
				{
					tmp[i][j] = __l[i] * __r[j];
				}
			}
			return tmp;
		}
		template <typename _Ty, size_t _LNum, simd_align_t _LSIMD, size_t _RNum, simd_align_t _RSIMD>
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr row_vector<_Ty, _RNum, simd_select_v<_Ty, _RNum>> operator*(
				row_vector<_Ty, _LNum, _LSIMD> const &__l,
				matrix<_Ty, _LNum, _RNum, _RSIMD> const &__r) pf_attr_noexcept
		{
			row_vector<_Ty, _RNum, simd_select_v<_Ty, _RNum>> tmp;
			for (size_t j = 0; j < _RNum; ++j)
			{
				for (size_t k = 0; k < _LNum; ++k)
				{
					tmp[j] += __l[k] * __r[j][k];
				}
			}
			return tmp;
		}
		template <typename _Ty, size_t _LNum, simd_align_t _LSIMD, size_t _RNum, simd_align_t _RSIMD>
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr col_vector<_Ty, _LNum, simd_select_v<_Ty, _LNum>> operator*(
				matrix<_Ty, _LNum, _RNum, _LSIMD> const &__l,
				col_vector<_Ty, _RNum, _RSIMD> const &__r) pf_attr_noexcept
		{
			col_vector<_Ty, _LNum, simd_select_v<_Ty, _LNum>> tmp;
			for (size_t i = 0; i < _RNum; ++i)
			{
				tmp[i] = __l[i] * __r;
			}
			return tmp;
		}

		/// MATH: Matrix -> Operator/
		template <typename _Ty, size_t _RowNum, size_t _ColNum, simd_align_t _SIMD>
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr matrix<_Ty, _RowNum, _ColNum, _SIMD> operator/(
				matrix<_Ty, _RowNum, _ColNum, _SIMD> const &__l,
				_Ty __val) pf_attr_noexcept
		{
			matrix<_Ty, _RowNum, _ColNum, _SIMD> tmp(__l);
			return tmp /= __val;
		}
		template <typename _Ty, size_t _RowNum, size_t _ColNum, simd_align_t _SIMD>
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr matrix<_Ty, _RowNum, _ColNum, _SIMD> operator/(
				_Ty __val,
				matrix<_Ty, _RowNum, _ColNum, _SIMD> const &__l) pf_attr_noexcept
		{
			matrix<_Ty, _RowNum, _ColNum, _SIMD> tmp(__l);
			return tmp /= __val;
		}
		template <typename _Ty, size_t _LRowNum, size_t _LColNum, simd_align_t _LSIMD, size_t _RColNum, simd_align_t _RSIMD>
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr matrix<_Ty, _LRowNum, _RColNum, simd_select_v<_Ty, _LColNum>> operator/(
				matrix<_Ty, _LRowNum, _LColNum, _LSIMD> const &__l,
				matrix<_Ty, _LColNum, _RColNum, _RSIMD> const &__r) pf_attr_noexcept
		{
			matrix<_Ty, _LRowNum, _LColNum, simd_select_v<_Ty, _LColNum>> tmp;
			for (size_t i = 0; i < _LRowNum; ++i)
			{
				for (size_t j = 0; j < _RColNum; ++j)
				{
					for (size_t k = 0; k < _LColNum; ++k)
					{
						tmp[i][j] += __l[i][k] / __r[k][j];
					}
				}
			}
			return tmp;
		}
		template <typename _Ty, size_t _LNum, simd_align_t _LSIMD, size_t _RNum, simd_align_t _RSIMD>
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr matrix<_Ty, _LNum, _RNum, simd_select_v<_Ty, _RNum>> operator/(
				col_vector<_Ty, _LNum, _LSIMD> const &__l,
				row_vector<_Ty, _RNum, _RSIMD> const &__r) pf_attr_noexcept
		{
			matrix<_Ty, _LNum, _RNum, simd_select_v<_Ty, _RNum>> tmp;
			for (size_t i = 0; i < _LNum; ++i)
			{
				for (size_t j = 0; j < _RNum; ++j)
				{
					tmp[i][j] = __l[i] / __r[j];
				}
			}
			return tmp;
		}
		template <typename _Ty, size_t _LNum, simd_align_t _LSIMD, size_t _RNum, simd_align_t _RSIMD>
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr row_vector<_Ty, _RNum, simd_select_v<_Ty, _RNum>> operator/(
				row_vector<_Ty, _LNum, _LSIMD> const &__l,
				matrix<_Ty, _LNum, _RNum, _RSIMD> const &__r) pf_attr_noexcept
		{
			row_vector<_Ty, _RNum, simd_select_v<_Ty, _RNum>> tmp;
			for (size_t j = 0; j < _RNum; ++j)
			{
				for (size_t k = 0; k < _LNum; ++k)
				{
					tmp[j] += __l[k] / __r[j][k];
				}
			}
			return tmp;
		}
		template <typename _Ty, size_t _LNum, simd_align_t _LSIMD, size_t _RNum, simd_align_t _RSIMD>
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr col_vector<_Ty, _LNum, simd_select_v<_Ty, _LNum>> operator/(
				matrix<_Ty, _LNum, _RNum, _LSIMD> const &__l,
				col_vector<_Ty, _RNum, _RSIMD> const &__r) pf_attr_noexcept
		{
			col_vector<_Ty, _LNum, simd_select_v<_Ty, _LNum>> tmp;
			for (size_t i = 0; i < _RNum; ++i)
			{
				tmp[i] = __l[i] / __r;
			}
			return tmp;
		}

		/// MATH: Matrix -> Operator%
		template <typename _Ty, size_t _RowNum, size_t _ColNum, simd_align_t _LSIMD, simd_align_t _RSIMD>
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr matrix<_Ty, _RowNum, _ColNum, simd_select_v<_Ty, _ColNum>> operator%(
				matrix<_Ty, _RowNum, _ColNum, _LSIMD> const &__l,
				matrix<_Ty, _RowNum, _ColNum, _RSIMD> const &__r) pf_attr_noexcept requires(std::is_integral_v<_Ty>)
		{
			matrix<_Ty, _RowNum, _ColNum, simd_select_v<_Ty, _ColNum>> tmp(__l);
			return tmp %= __r;
		}

		/// MATH: Matrix -> Operator&
		template <typename _Ty, size_t _RowNum, size_t _ColNum, simd_align_t _LSIMD, simd_align_t _RSIMD>
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr matrix<_Ty, _RowNum, _ColNum, simd_select_v<_Ty, _ColNum>> operator&(
				matrix<_Ty, _RowNum, _ColNum, _LSIMD> const &__l,
				matrix<_Ty, _RowNum, _ColNum, _RSIMD> const &__r) pf_attr_noexcept requires(std::is_integral_v<_Ty>)
		{
			matrix<_Ty, _RowNum, _ColNum, simd_select_v<_Ty, _ColNum>> tmp(__l);
			return tmp &= __r;
		}

		/// MATH: Matrix -> Operator|
		template <typename _Ty, size_t _RowNum, size_t _ColNum, simd_align_t _LSIMD, simd_align_t _RSIMD>
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr matrix<_Ty, _RowNum, _ColNum, simd_select_v<_Ty, _ColNum>> operator|(
				matrix<_Ty, _RowNum, _ColNum, _LSIMD> const &__l,
				matrix<_Ty, _RowNum, _ColNum, _RSIMD> const &__r) pf_attr_noexcept requires(std::is_integral_v<_Ty>)
		{
			matrix<_Ty, _RowNum, _ColNum, simd_select_v<_Ty, _ColNum>> tmp(__l);
			return tmp |= __r;
		}

		/// MATH: Matrix -> Operator^
		template <typename _Ty, size_t _RowNum, size_t _ColNum, simd_align_t _LSIMD, simd_align_t _RSIMD>
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr matrix<_Ty, _RowNum, _ColNum, simd_select_v<_Ty, _ColNum>> operator^(
				matrix<_Ty, _RowNum, _ColNum, _LSIMD> const &__l,
				matrix<_Ty, _RowNum, _ColNum, _RSIMD> const &__r) pf_attr_noexcept requires(std::is_integral_v<_Ty>)
		{
			matrix<_Ty, _RowNum, _ColNum, simd_select_v<_Ty, _ColNum>> tmp(__l);
			return tmp ^= __r;
		}

		/// MATH: Matrix -> Operator>>
		template <typename _Ty, size_t _RowNum, size_t _ColNum, simd_align_t _LSIMD, simd_align_t _RSIMD>
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr matrix<_Ty, _RowNum, _ColNum, simd_select_v<_Ty, _ColNum>> operator>>(
				matrix<_Ty, _RowNum, _ColNum, _LSIMD> const &__l,
				matrix<_Ty, _RowNum, _ColNum, _RSIMD> const &__r) pf_attr_noexcept requires(std::is_integral_v<_Ty>)
		{
			matrix<_Ty, _RowNum, _ColNum, simd_select_v<_Ty, _ColNum>> tmp(__l);
			return tmp >>= __r;
		}

		/// MATH: Matrix -> Operator<<
		template <typename _Ty, size_t _RowNum, size_t _ColNum, simd_align_t _LSIMD, simd_align_t _RSIMD>
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr matrix<_Ty, _RowNum, _ColNum, simd_select_v<_Ty, _ColNum>> operator<<(
				matrix<_Ty, _RowNum, _ColNum, _LSIMD> const &__l,
				matrix<_Ty, _RowNum, _ColNum, _RSIMD> const &__r) pf_attr_noexcept requires(std::is_integral_v<_Ty>)
		{
			matrix<_Ty, _RowNum, _ColNum, simd_select_v<_Ty, _ColNum>> tmp(__l);
			return tmp <<= __r;
		}

		/// MATH: Matrix -> Operator~
		template <typename _Ty, size_t _RowNum, size_t _ColNum, simd_align_t _SIMD>
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr matrix<_Ty, _RowNum, _ColNum, _SIMD> operator~(
				matrix<_Ty, _RowNum, _ColNum, _SIMD> const &__r) pf_attr_noexcept requires(_RowNum > 1 && _ColNum > 1)
		{
			matrix<_Ty, _RowNum, _ColNum, _SIMD> tmp;
			for (size_t i = 0; i < _RowNum; ++i)
			{
				for (size_t j = 0; j < _ColNum; ++j)
				{
					tmp[i][j] = ~__r[i][j];
				}
			}
			return tmp;
		}

		/// MATH: Matrix -> Compare
		template <typename _Ty, size_t _RowNum, size_t _ColNum, simd_align_t _LSIMD, simd_align_t _RSIMD>
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr matrix<bool, _RowNum, _ColNum, simd_select_v<bool, _ColNum>> vals_equal(
				matrix<_Ty, _RowNum, _ColNum, _LSIMD> const &__l,
				matrix<_Ty, _RowNum, _ColNum, _RSIMD> const &__r) pf_attr_noexcept
		{
			return __l.vals_equal(__r);
		}
		template <typename _Ty, size_t _RowNum, size_t _ColNum, simd_align_t _LSIMD, simd_align_t _RSIMD>
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr matrix<bool, _RowNum, _ColNum, simd_select_v<bool, _ColNum>> vals_greater(
				matrix<_Ty, _RowNum, _ColNum, _LSIMD> const &__l,
				matrix<_Ty, _RowNum, _ColNum, _RSIMD> const &__r) pf_attr_noexcept
		{
			return __l.vals_greater(__r);
		}
		template <typename _Ty, size_t _RowNum, size_t _ColNum, simd_align_t _LSIMD, simd_align_t _RSIMD>
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr matrix<bool, _RowNum, _ColNum, simd_select_v<bool, _ColNum>> vals_greater_equal(
				matrix<_Ty, _RowNum, _ColNum, _LSIMD> const &__l,
				matrix<_Ty, _RowNum, _ColNum, _RSIMD> const &__r) pf_attr_noexcept
		{
			return __l.vals_greater_equal(__r);
		}
		template <typename _Ty, size_t _RowNum, size_t _ColNum, simd_align_t _LSIMD, simd_align_t _RSIMD>
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr matrix<bool, _RowNum, _ColNum, simd_select_v<bool, _ColNum>> vals_smaller(
				matrix<_Ty, _RowNum, _ColNum, _LSIMD> const &__l,
				matrix<_Ty, _RowNum, _ColNum, _RSIMD> const &__r) pf_attr_noexcept
		{
			return __l.vals_smaller(__r);
		}
		template <typename _Ty, size_t _RowNum, size_t _ColNum, simd_align_t _LSIMD, simd_align_t _RSIMD>
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr matrix<bool, _RowNum, _ColNum, simd_select_v<bool, _ColNum>> vals_smaller_equal(
				matrix<_Ty, _RowNum, _ColNum, _LSIMD> const &__l,
				matrix<_Ty, _RowNum, _ColNum, _RSIMD> const &__r) pf_attr_noexcept
		{
			return __l.vals_smaller_equal(__r);
		}
		template <typename _Ty, size_t _RowNum, size_t _ColNum, simd_align_t _LSIMD, simd_align_t _RSIMD>
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr matrix<bool, _RowNum, _ColNum, simd_select_v<bool, _ColNum>> vals_not_equal(
				matrix<_Ty, _RowNum, _ColNum, _LSIMD> const &__l,
				matrix<_Ty, _RowNum, _ColNum, _RSIMD> const &__r) pf_attr_noexcept
		{
			return __l.vals_not_equal(__r);
		}
	}
}

// Pulsar: Math -> Matrix Specializations
#include "pulsar/math/matrix/matrix2.inl"
#include "pulsar/math/matrix/matrix2x3.inl"
#include "pulsar/math/matrix/matrix2x4.inl"
#include "pulsar/math/matrix/matrix3.inl"
#include "pulsar/math/matrix/matrix3x2.inl"
#include "pulsar/math/matrix/matrix3x4.inl"
#include "pulsar/math/matrix/matrix4.inl"
#include "pulsar/math/matrix/matrix4x2.inl"
#include "pulsar/math/matrix/matrix4x3.inl"

#endif // !PULSAR_MATH_MATRIX_HPP