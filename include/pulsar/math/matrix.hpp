/*! @file   matrix.hpp
 *  @author Louis-Quentin Noé (noe.louis-quentin@hotmail.fr)
 *  @brief	Definition of matrices.
 *  @date   24-05-2022
 *
 *  @copyright Copyright (c) 2023 - Pulsar Software
 *
 *  @since 0.1.1
 */

#ifndef PULSAR_MATH_MATRIX_HPP
#define PULSAR_MATH_MATRIX_HPP 1

// Include: Pulsar
#include "pulsar/pulsar.hpp"

// Include: Pulsar -> Math
#include "pulsar/math/vector.hpp"

// Pulsar
namespace pul
{
	/// MATH: Matrix
	template<typename _Ty, size_t _RowNum, size_t _ColNum>
		requires(std::is_arithmetic_v<_Ty> && _RowNum > 1 && _ColNum > 1)
	union matrix;

	/// MATH: Matrix -> SFINAE -> Is Matrix
	template<typename _MatTy>
	struct is_matrix : public std::false_type
	{};
	template<typename _Ty, size_t _RowNum, size_t _ColNum>
	struct is_matrix<matrix<_Ty, _RowNum, _ColNum>> : public std::true_type
	{};
	template<typename _MatTy>
	pf_decl_static pf_decl_constexpr bool is_matrix_v = is_matrix<_MatTy>::value;

	/// MATH: Matrix -> Concept
	template<typename _MatTy>
	concept matric_c = is_matrix_v<_MatTy>;

	/// MATH: Matrix -> SFINAE -> Is Square Matrix
	template<typename _MatTy>
	struct is_square_matrix : public std::false_type
	{};
	template<typename _Ty, size_t _Num>
	struct is_square_matrix<matrix<_Ty, _Num, _Num>> : public std::true_type
	{};
	template<typename _MatTy>
	pf_decl_static pf_decl_constexpr bool is_square_matrix_v = is_square_matrix<_MatTy>::value;

	/// MATH: Matrix -> Square Concept
	template<typename _MatTy>
	concept square_matrix_c = is_square_matrix_v<_MatTy>;

	/// MATH: Matrix -> Square
	template<typename _Ty, size_t _Num>
	using square_matrix = matrix<_Ty, _Num, _Num>;

	/// MATH: Matrix -> Elem Type
	template<typename _Ty, size_t _RowNum, size_t _ColNum>
	struct elem_typeof<matrix<_Ty, _RowNum, _ColNum>>
	{
		using type = _Ty;
	};

	/// MATH: Matrix -> Elem Row Num
	template<typename _Ty>
	struct elem_row_numof;
	template<typename _Ty, size_t _RowNum, size_t _ColNum>
	struct elem_row_numof<matrix<_Ty, _RowNum, _ColNum>> : std::integral_constant<size_t, _RowNum>
	{};
	template<typename _Ty>
	pf_decl_static pf_decl_constexpr size_t elem_row_numof_v = elem_row_numof<_Ty>::value;

	/// MATH: Matrix -> Elem Col Num
	template<typename _Ty>
	struct elem_col_numof;
	template<typename _Ty, size_t _RowNum, size_t _ColNum>
	struct elem_col_numof<matrix<_Ty, _RowNum, _ColNum>> : std::integral_constant<size_t, _ColNum>
	{};
	template<typename _Ty>
	pf_decl_static pf_decl_constexpr size_t elem_col_numof_v = elem_col_numof<_Ty>::value;

	/// MATH: Matrix -> Elem Num
	template<typename _Ty, size_t _RowNum, size_t _ColNum>
	struct elem_numof<matrix<_Ty, _RowNum, _ColNum>> : std::integral_constant<size_t, _RowNum * _ColNum>
	{};

	/// MATH: Matrix
	template<typename _Ty, size_t _RowNum, size_t _ColNum>
		requires(std::is_arithmetic_v<_Ty> && _RowNum > 1 && _ColNum > 1)
	union matrix
	{
		template<typename _TyF, size_t _RowNumF, size_t _ColNumF>
		pf_decl_friend union matrix;

	public:
		/// Constructors
		pf_hint_nodiscard pf_decl_constexpr
		matrix(
		 _Ty __val = 0) pf_attr_noexcept
			: store_ { __val }
		{
			for(size_t i = 0, l = std::min(_RowNum, _ColNum); i < l; ++i) this->store_[i][l] = __val;
		}
		pf_hint_nodiscard pf_decl_constexpr
		matrix(
		 matrix<_Ty, _RowNum, _ColNum> const &__r) pf_attr_noexcept
			: store_(__r.store_)
		{}
		template<size_t _Num>
			requires(_Num == _RowNum)
		pf_hint_nodiscard pf_decl_constexpr
		matrix(
		 vector<_Ty, _ColNum> const (&__l)[_Num])
		 pf_attr_noexcept
		{
			for(size_t i = 0; i < _RowNum; ++i)
			{
				this->store_[i] = __l[i];
			}
		}

		/// Operator=
		pf_decl_constexpr matrix<_Ty, _RowNum, _ColNum> &
		operator=(
		 matrix<_Ty, _RowNum, _ColNum> const &__r) pf_attr_noexcept = default;

		/// Operator[]
		pf_hint_nodiscard pf_decl_constexpr vector<_Ty, _ColNum> &
		operator[](
		 size_t __index) pf_attr_noexcept
		{
			return this->store_[__index];
		}
		pf_hint_nodiscard pf_decl_constexpr const vector<_Ty, _ColNum> &
		operator[](
		 size_t __index) const pf_attr_noexcept
		{
			return this->store_[__index];
		}

		/// Operator+=
		pf_decl_constexpr matrix<_Ty, _RowNum, _ColNum> &
		operator+=(
		 _Ty __val) pf_attr_noexcept
		{
			for(size_t i = 0; i < _RowNum; ++i)
			{
				this->store_[i] += __val;
			}
			return *this;
		}
		pf_decl_constexpr matrix<_Ty, _RowNum, _ColNum> &
		operator+=(
		 matrix<_Ty, _RowNum, _ColNum> const &__r) pf_attr_noexcept
		{
			for(size_t i = 0; i < _RowNum; ++i)
			{
				this->store_[i] += __r[i];
			}
			return *this;
		}

		/// Operator-=
		pf_decl_constexpr matrix<_Ty, _RowNum, _ColNum> &
		operator-=(
		 _Ty __val) pf_attr_noexcept
		{
			for(size_t i = 0; i < _RowNum; ++i)
			{
				this->store_[i] -= __val;
			}
			return *this;
		}
		pf_decl_constexpr matrix<_Ty, _RowNum, _ColNum> &
		operator-=(
		 matrix<_Ty, _RowNum, _ColNum> const &__r) pf_attr_noexcept
		{
			for(size_t i = 0; i < _RowNum; ++i)
			{
				this->store_[i] -= __r[i];
			}
			return *this;
		}

		/// Operator*=
		pf_decl_constexpr matrix<_Ty, _RowNum, _ColNum> &
		operator*=(
		 _Ty __val) pf_attr_noexcept
		{
			for(size_t i = 0; i < _RowNum; ++i)
			{
				this->store_[i] *= __val;
			}
			return *this;
		}
		pf_decl_constexpr matrix<_Ty, _RowNum, _ColNum>
		pf_alignas_n(_Ty, _ColNum) operator*=(
		 matrix<_Ty, _RowNum, _ColNum> const &__r) pf_attr_noexcept
			requires(_RowNum == _ColNum)
		{
			matrix<_Ty, _RowNum, _ColNum> pf_alignas_n(_Ty, _ColNum) tmp;
			for(size_t i = 0; i < _RowNum; ++i)
			{
				for(size_t j = 0; j < _ColNum; ++j)
				{
					for(size_t k = 0; k < _RowNum; ++k)
					{
						tmp[i][j] += this->store_[i][j + k] * __r[i + k][j];
					}
				}
			}
		}

		/// Operator/=
		pf_decl_constexpr matrix<_Ty, _RowNum, _ColNum> &
		operator/=(
		 _Ty __val) pf_attr_noexcept
		{
			for(size_t i = 0; i < _RowNum; ++i)
			{
				this->store_[i] /= __val;
			}
			return *this;
		}
		pf_decl_constexpr matrix<_Ty, _RowNum, _ColNum>
		pf_alignas_n(_Ty, _ColNum) operator/=(
		 matrix<_Ty, _RowNum, _ColNum> const &__r) pf_attr_noexcept
			requires(_RowNum == _ColNum)
		{
			matrix<_Ty, _RowNum, _ColNum> pf_alignas_n(_Ty, _ColNum) tmp;
			for(size_t i = 0; i < _RowNum; ++i)
			{
				for(size_t j = 0; j < _ColNum; ++j)
				{
					for(size_t k = 0; k < _RowNum; ++k)
					{
						tmp[i][j] += this->store_[i][j + k] / __r[i + k][j];
					}
				}
			}
		}

		/// Operator%=
		pf_decl_constexpr matrix<_Ty, _RowNum, _ColNum> &
		operator%=(
		 _Ty __val) pf_attr_noexcept
			requires(std::is_integral_v<_Ty>)
		{
			for(size_t i = 0; i < _RowNum; ++i)
			{
				this->store_[i] %= __val;
			}
			return *this;
		}
		pf_decl_constexpr matrix<_Ty, _RowNum, _ColNum> &
		operator%=(
		 matrix<_Ty, _RowNum, _ColNum> const &__r) pf_attr_noexcept
			requires(std::is_integral_v<_Ty>)
		{
			for(size_t i = 0; i < _RowNum; ++i)
			{
				this->store_[i] %= __r[i];
			}
			return *this;
		}

		/// Operator&=
		pf_decl_constexpr matrix<_Ty, _RowNum, _ColNum> &
		operator&=(
		 _Ty __val) pf_attr_noexcept
		{
			for(size_t i = 0; i < _RowNum; ++i)
			{
				this->store_[i] &= __val;
			}
			return *this;
		}
		pf_decl_constexpr matrix<_Ty, _RowNum, _ColNum> &
		operator&=(
		 matrix<_Ty, _RowNum, _ColNum> const &__r) pf_attr_noexcept
			requires(std::is_integral_v<_Ty>)
		{
			for(size_t i = 0; i < _RowNum; ++i)
			{
				this->store_[i] &= __r[i];
			}
			return *this;
		}

		/// Operator|=
		pf_decl_constexpr matrix<_Ty, _RowNum, _ColNum> &
		operator|=(
		 _Ty __val) pf_attr_noexcept
			requires(std::is_integral_v<_Ty>)
		{
			for(size_t i = 0; i < _RowNum; ++i)
			{
				this->store_[i] |= __val;
			}
			return *this;
		}
		pf_decl_constexpr matrix<_Ty, _RowNum, _ColNum> &
		operator|=(
		 matrix<_Ty, _RowNum, _ColNum> const &__r) pf_attr_noexcept
			requires(std::is_integral_v<_Ty>)
		{
			for(size_t i = 0; i < _RowNum; ++i)
			{
				this->store_[i] |= __r[i];
			}
			return *this;
		}

		/// Operator^=
		pf_decl_constexpr matrix<_Ty, _RowNum, _ColNum> &
		operator^=(
		 _Ty __val) pf_attr_noexcept
			requires(std::is_integral_v<_Ty>)
		{
			for(size_t i = 0; i < _RowNum; ++i)
			{
				this->store_[i] ^= __val;
			}
			return *this;
		}
		pf_decl_constexpr matrix<_Ty, _RowNum, _ColNum> &
		operator^=(
		 matrix<_Ty, _RowNum, _ColNum> const &__r) pf_attr_noexcept
			requires(std::is_integral_v<_Ty>)
		{
			for(size_t i = 0; i < _RowNum; ++i)
			{
				this->store_[i] ^= __r[i];
			}
			return *this;
		}

		/// Operator>>=
		pf_decl_constexpr matrix<_Ty, _RowNum, _ColNum> &
		operator>>=(
		 _Ty __val) pf_attr_noexcept
			requires(std::is_integral_v<_Ty>)
		{
			for(size_t i = 0; i < _RowNum; ++i)
			{
				this->store_[i] >>= __val;
			}
			return *this;
		}
		pf_decl_constexpr matrix<_Ty, _RowNum, _ColNum> &
		operator>>=(
		 matrix<_Ty, _RowNum, _ColNum> const &__r) pf_attr_noexcept
			requires(std::is_integral_v<_Ty>)
		{
			for(size_t i = 0; i < _RowNum; ++i)
			{
				this->store_[i] >>= __r[i];
			}
			return *this;
		}

		/// Operator<<=
		pf_decl_constexpr matrix<_Ty, _RowNum, _ColNum> &
		operator<<=(
		 _Ty __val) pf_attr_noexcept
			requires(std::is_integral_v<_Ty>)
		{
			for(size_t i = 0; i < _RowNum; ++i)
			{
				this->store_[i] <<= __val;
			}
			return *this;
		}
		pf_decl_constexpr matrix<_Ty, _RowNum, _ColNum> &
		operator<<=(
		 matrix<_Ty, _RowNum, _ColNum> const &__r) pf_attr_noexcept
			requires(std::is_integral_v<_Ty>)
		{
			for(size_t i = 0; i < _RowNum; ++i)
			{
				this->store_[i] <<= __r[i];
			}
			return *this;
		}

		/// Operator==
		pf_hint_nodiscard pf_decl_constexpr bool
		operator==(
		 matrix<_Ty, _RowNum, _ColNum> const &__r) pf_attr_noexcept
		{
			for(size_t i = 0; i < _RowNum; ++i)
			{
				if(this->store_[i] != __r[i])
					return false;
			}
			return true;
		}

		/// Compare
		pf_hint_nodiscard pf_decl_constexpr matrix<bool, _RowNum, _ColNum>
		equal(
		 matrix<_Ty, _RowNum, _ColNum> const &__r) const pf_attr_noexcept
		{
			matrix<bool, _RowNum, _ColNum> tmp;
			for(size_t i = 0; i < _RowNum; ++i)
			{
				for(size_t j = 0; j < _ColNum; ++j)
				{
					tmp[i][j] = this->store_[i][j] == __r[i][j];
				}
			}
			return tmp;
		}
		pf_hint_nodiscard pf_decl_constexpr matrix<bool, _RowNum, _ColNum>
		not_equal(
		 matrix<_Ty, _RowNum, _ColNum> const &__r) const pf_attr_noexcept
		{
			matrix<bool, _RowNum, _ColNum> tmp;
			for(size_t i = 0; i < _RowNum; ++i)
			{
				for(size_t j = 0; j < _ColNum; ++j)
				{
					tmp[i][j] = this->store_[i][j] != __r[i][j];
				}
			}
			return tmp;
		}
		pf_hint_nodiscard pf_decl_constexpr matrix<bool, _RowNum, _ColNum>
		greater(
		 matrix<_Ty, _RowNum, _ColNum> const &__r) const pf_attr_noexcept
		{
			matrix<bool, _RowNum, _ColNum> tmp;
			for(size_t i = 0; i < _RowNum; ++i)
			{
				for(size_t j = 0; j < _ColNum; ++j)
				{
					tmp[i][j] = this->store_[i][j] > __r[i][j];
				}
			}
			return tmp;
		}
		pf_hint_nodiscard pf_decl_constexpr matrix<bool, _RowNum, _ColNum>
		greater_equal(
		 matrix<_Ty, _RowNum, _ColNum> const &__r) const pf_attr_noexcept
		{
			matrix<bool, _RowNum, _ColNum> tmp;
			for(size_t i = 0; i < _RowNum; ++i)
			{
				for(size_t j = 0; j < _ColNum; ++j)
				{
					tmp[i][j] = this->store_[i][j] >= __r[i][j];
				}
			}
			return tmp;
		}
		pf_hint_nodiscard pf_decl_constexpr matrix<bool, _RowNum, _ColNum>
		smaller(
		 matrix<_Ty, _RowNum, _ColNum> const &__r) const pf_attr_noexcept
		{
			matrix<bool, _RowNum, _ColNum> tmp;
			for(size_t i = 0; i < _RowNum; ++i)
			{
				for(size_t j = 0; j < _ColNum; ++j)
				{
					tmp[i][j] = this->store_[i][j] < __r[i][j];
				}
			}
			return tmp;
		}
		pf_hint_nodiscard pf_decl_constexpr matrix<bool, _RowNum, _ColNum>
		smaller_equal(
		 matrix<_Ty, _RowNum, _ColNum> const &__r) const pf_attr_noexcept
		{
			matrix<bool, _RowNum, _ColNum> tmp;
			for(size_t i = 0; i < _RowNum; ++i)
			{
				for(size_t j = 0; j < _ColNum; ++j)
				{
					tmp[i][j] = this->store_[i][j] <= __r[i][j];
				}
			}
			return tmp;
		}

		/// Data
		pf_hint_nodiscard pf_decl_constexpr _Ty *
		data() pf_attr_noexcept
		{
			return this->store_.data()->data();
		}
		pf_hint_nodiscard pf_decl_constexpr const _Ty *
		data() const pf_attr_noexcept
		{
			return this->store_.data()->data();
		}

	private:
		array<vector<_Ty, _ColNum>, _RowNum> store_;	// TODO: array<vector<_Ty, _RowNum>, _ColNum>> => (i, j) become (j, i)
	};

	/// MATH: Matrix -> Operator+
	template<typename _Ty, size_t _RowNum, size_t _ColNum>
	pf_hint_nodiscard pf_decl_constexpr matrix<_Ty, _RowNum, _ColNum>
	pf_alignas_n(_Ty, _ColNum) operator+(
	 matrix<_Ty, _RowNum, _ColNum> const &__l,
	 _Ty __val) pf_attr_noexcept
	{
		matrix<_Ty, _RowNum, _ColNum> pf_alignas_n(_Ty, _ColNum) tmp = __l;
		return tmp																									+= __val;
	}
	template<typename _Ty, size_t _RowNum, size_t _ColNum>
	pf_hint_nodiscard pf_decl_constexpr matrix<_Ty, _RowNum, _ColNum>
	pf_alignas_n(_Ty, _ColNum) operator+(
	 _Ty __val,
	 matrix<_Ty, _RowNum, _ColNum> const &__l) pf_attr_noexcept
	{
		matrix<_Ty, _RowNum, _ColNum> pf_alignas_n(_Ty, _ColNum) tmp = __l;
		return tmp																									+= __val;
	}
	template<typename _Ty, size_t _RowNum, size_t _ColNum>
	pf_hint_nodiscard pf_decl_constexpr matrix<_Ty, _RowNum, _ColNum>
	pf_alignas_n(_Ty, _ColNum) operator+(
	 matrix<_Ty, _RowNum, _ColNum> const &__l,
	 matrix<_Ty, _RowNum, _ColNum> const &__r) pf_attr_noexcept
	{
		matrix<_Ty, _RowNum, _ColNum> pf_alignas_n(_Ty, _ColNum) tmp = __l;
		return tmp																									+= __r;
	}

	/// MATH: Matrix -> Operator-
	template<typename _Ty, size_t _RowNum, size_t _ColNum>
	pf_hint_nodiscard pf_decl_constexpr matrix<_Ty, _RowNum, _ColNum>
	pf_alignas_n(_Ty, _ColNum) operator-(
	 matrix<_Ty, _RowNum, _ColNum> const &__l,
	 _Ty __val) pf_attr_noexcept
	{
		matrix<_Ty, _RowNum, _ColNum> pf_alignas_n(_Ty, _ColNum) tmp = __l;
		return tmp																									-= __val;
	}
	template<typename _Ty, size_t _RowNum, size_t _ColNum>
	pf_hint_nodiscard pf_decl_constexpr matrix<_Ty, _RowNum, _ColNum>
	pf_alignas_n(_Ty, _ColNum) operator-(
	 _Ty __val,
	 matrix<_Ty, _RowNum, _ColNum> const &__l) pf_attr_noexcept
	{
		matrix<_Ty, _RowNum, _ColNum> pf_alignas_n(_Ty, _ColNum) tmp = __l;
		return tmp																									-= __val;
	}
	template<typename _Ty, size_t _RowNum, size_t _ColNum>
	pf_hint_nodiscard pf_decl_constexpr matrix<_Ty, _RowNum, _ColNum>
	pf_alignas_n(_Ty, _ColNum) operator-(
	 matrix<_Ty, _RowNum, _ColNum> const &__l,
	 matrix<_Ty, _RowNum, _ColNum> const &__r) pf_attr_noexcept
	{
		matrix<_Ty, _RowNum, _ColNum> pf_alignas_n(_Ty, _ColNum) tmp = __l;
		return tmp																									-= __r;
	}

	/// MATH: Matrix -> Operator*
	template<typename _Ty, size_t _RowNum, size_t _ColNum>
	pf_hint_nodiscard pf_decl_constexpr matrix<_Ty, _RowNum, _ColNum>
	pf_alignas_n(_Ty, _ColNum) operator*(
	 matrix<_Ty, _RowNum, _ColNum> const &__l,
	 _Ty __val) pf_attr_noexcept
	{
		matrix<_Ty, _RowNum, _ColNum> pf_alignas_n(_Ty, _ColNum) tmp = __l;
		return tmp																									*= __val;
	}
	template<typename _Ty, size_t _RowNum, size_t _ColNum>
	pf_hint_nodiscard pf_decl_constexpr matrix<_Ty, _RowNum, _ColNum>
	pf_alignas_n(_Ty, _ColNum) operator*(
	 _Ty __val,
	 matrix<_Ty, _RowNum, _ColNum> const &__l) pf_attr_noexcept
	{
		matrix<_Ty, _RowNum, _ColNum> pf_alignas_n(_Ty, _ColNum) tmp = __l;
		return tmp																									*= __val;
	}
	template<typename _Ty, size_t _RowNum, size_t _ColNum>
	pf_hint_nodiscard pf_decl_constexpr vector<_Ty, _ColNum>
	pf_alignas_n(_Ty, _ColNum) operator*(
	 vector<_Ty, _RowNum> const &__v,
	 matrix<_Ty, _RowNum, _ColNum> const &__m) pf_attr_noexcept
	{
		vector<_Ty, _ColNum> pf_alignas_n(_Ty, _ColNum) tmp;
		for(size_t i = 0; i < _ColNum; ++i)
		{
			for(size_t k = 0; k < _RowNum; ++k)
			{
				tmp[i] += __v[k] * __m[k][i];
			}
		}
		return tmp;
	}
	template<typename _Ty, size_t _RowNum, size_t _Num, size_t _ColNum>
	pf_hint_nodiscard pf_decl_constexpr matrix<_Ty, _RowNum, _ColNum>
	pf_alignas_n(_Ty, _ColNum) operator*(
	 matrix<_Ty, _RowNum, _Num> const &__l,
	 matrix<_Ty, _Num, _ColNum> const &__r) pf_attr_noexcept
	{
		matrix<_Ty, _RowNum, _ColNum> pf_alignas_n(_Ty, _ColNum) tmp;
		for(size_t i = 0; i < _RowNum; ++i)
		{
			for(size_t j = 0; j < _ColNum; ++j)
			{
				for(size_t k = 0; k < _Num; ++k)
				{
					tmp[i][j] += __l[i][k] * __r[k][j];
				}
			}
		}
		return tmp;
	}
	template<typename _Ty, size_t _RowNum, size_t _ColNum>
	pf_hint_nodiscard pf_decl_constexpr vector<_Ty, _RowNum>
	pf_alignas_n(_Ty, _ColNum) operator*(
	 matrix<_Ty, _RowNum, _ColNum> const &__m,
	 vector<_Ty, _ColNum> const &__v) pf_attr_noexcept
	{
		vector<_Ty, _RowNum> pf_alignas_n(_Ty, _ColNum) tmp;
		for(size_t i = 0; i < _RowNum; ++i)
		{
			for(size_t k = 0; k < _ColNum; ++k)
			{
				tmp[i] += __m[i][k] * __v[k];
			}
		}
		return tmp;
	}

	/// MATH: Matrix -> Operator/
	template<typename _Ty, size_t _RowNum, size_t _ColNum>
	pf_hint_nodiscard pf_decl_constexpr matrix<_Ty, _RowNum, _ColNum>
	pf_alignas_n(_Ty, _ColNum) operator/(
	 matrix<_Ty, _RowNum, _ColNum> const &__l,
	 _Ty __val) pf_attr_noexcept
	{
		matrix<_Ty, _RowNum, _ColNum> pf_alignas_n(_Ty, _ColNum) tmp = __l;
		return tmp																									/= __val;
	}
	template<typename _Ty, size_t _RowNum, size_t _ColNum>
	pf_hint_nodiscard pf_decl_constexpr matrix<_Ty, _RowNum, _ColNum>
	pf_alignas_n(_Ty, _ColNum) operator/(
	 _Ty __val,
	 matrix<_Ty, _RowNum, _ColNum> const &__l) pf_attr_noexcept
	{
		matrix<_Ty, _RowNum, _ColNum> pf_alignas_n(_Ty, _ColNum) tmp = __l;
		return tmp																									/= __val;
	}
	template<typename _Ty, size_t _RowNum, size_t _ColNum>
	pf_hint_nodiscard pf_decl_constexpr vector<_Ty, _ColNum>
	pf_alignas_n(_Ty, _ColNum) operator/(
	 vector<_Ty, _RowNum> const &__v,
	 matrix<_Ty, _RowNum, _ColNum> const &__m) pf_attr_noexcept
	{
		vector<_Ty, _ColNum> pf_alignas_n(_Ty, _ColNum) tmp;
		for(size_t i = 0; i < _ColNum; ++i)
		{
			for(size_t k = 0; k < _RowNum; ++k)
			{
				tmp[i] += __v[k] / __m[k][i];
			}
		}
		return tmp;
	}
	template<typename _Ty, size_t _RowNum, size_t _Num, size_t _ColNum>
	pf_hint_nodiscard pf_decl_constexpr matrix<_Ty, _RowNum, _ColNum>
	pf_alignas_n(_Ty, _ColNum) operator/(
	 matrix<_Ty, _RowNum, _Num> const &__l,
	 matrix<_Ty, _Num, _ColNum> const &__r) pf_attr_noexcept
	{
		matrix<_Ty, _RowNum, _ColNum> pf_alignas_n(_Ty, _ColNum) tmp;
		for(size_t i = 0; i < _RowNum; ++i)
		{
			for(size_t j = 0; j < _ColNum; ++j)
			{
				for(size_t k = 0; k < _Num; ++k)
				{
					tmp[i][j] += __l[i][k] / __r[k][j];
				}
			}
		}
		return tmp;
	}
	template<typename _Ty, size_t _RowNum, size_t _ColNum>
	pf_hint_nodiscard pf_decl_constexpr vector<_Ty, _RowNum>
	pf_alignas_n(_Ty, _ColNum) operator/(
	 matrix<_Ty, _RowNum, _ColNum> const &__m,
	 vector<_Ty, _ColNum> const &__v) pf_attr_noexcept
	{
		vector<_Ty, _RowNum> pf_alignas_n(_Ty, _ColNum) tmp;
		for(size_t i = 0; i < _RowNum; ++i)
		{
			for(size_t k = 0; k < _ColNum; ++k)
			{
				tmp[i] += __m[i][k] / __v[k];
			}
		}
		return tmp;
	}

	/// MATH: Matrix -> Operator%
	template<typename _Ty, size_t _RowNum, size_t _ColNum>
	pf_hint_nodiscard pf_decl_constexpr matrix<_Ty, _RowNum, _ColNum>
	pf_alignas_n(_Ty, _ColNum) operator%(
	 matrix<_Ty, _RowNum, _ColNum> const &__m,
	 _Ty __val) pf_attr_noexcept
		requires(std::is_integral_v<_Ty>)
	{
		matrix<_Ty, _RowNum, _ColNum> pf_alignas_n(_Ty, _ColNum) tmp;
		return tmp %= __val;
	}
	template<typename _Ty, size_t _RowNum, size_t _ColNum>
	pf_hint_nodiscard pf_decl_constexpr matrix<_Ty, _RowNum, _ColNum>
	pf_alignas_n(_Ty, _ColNum) operator%(
	 matrix<_Ty, _RowNum, _ColNum> const &__m1,
	 matrix<_Ty, _RowNum, _ColNum> const &__m2) pf_attr_noexcept
		requires(std::is_integral_v<_Ty>)
	{
		matrix<_Ty, _RowNum, _ColNum> pf_alignas_n(_Ty, _ColNum) tmp = __m1;
		return tmp																									%= __m2;
	}

	/// MATH: Matrix -> Operator&
	template<typename _Ty, size_t _RowNum, size_t _ColNum>
	pf_hint_nodiscard pf_decl_constexpr matrix<_Ty, _RowNum, _ColNum>
	pf_alignas_n(_Ty, _ColNum) operator&(
	 matrix<_Ty, _RowNum, _ColNum> const &__m,
	 _Ty __val) pf_attr_noexcept
		requires(std::is_integral_v<_Ty>)
	{
		matrix<_Ty, _RowNum, _ColNum> pf_alignas_n(_Ty, _ColNum) tmp;
		return tmp &= __val;
	}
	template<typename _Ty, size_t _RowNum, size_t _ColNum>
	pf_hint_nodiscard pf_decl_constexpr matrix<_Ty, _RowNum, _ColNum>
	pf_alignas_n(_Ty, _ColNum) operator&(
	 matrix<_Ty, _RowNum, _ColNum> const &__m1,
	 matrix<_Ty, _RowNum, _ColNum> const &__m2) pf_attr_noexcept
		requires(std::is_integral_v<_Ty>)
	{
		matrix<_Ty, _RowNum, _ColNum> pf_alignas_n(_Ty, _ColNum) tmp = __m1;
		return tmp																									&= __m2;
	}

	/// MATH: Matrix -> Operator|
	template<typename _Ty, size_t _RowNum, size_t _ColNum>
	pf_hint_nodiscard pf_decl_constexpr matrix<_Ty, _RowNum, _ColNum>
	pf_alignas_n(_Ty, _ColNum) operator|(
	 matrix<_Ty, _RowNum, _ColNum> const &__m,
	 _Ty __val) pf_attr_noexcept
		requires(std::is_integral_v<_Ty>)
	{
		matrix<_Ty, _RowNum, _ColNum> pf_alignas_n(_Ty, _ColNum) tmp;
		return tmp |= __val;
	}
	template<typename _Ty, size_t _RowNum, size_t _ColNum>
	pf_hint_nodiscard pf_decl_constexpr matrix<_Ty, _RowNum, _ColNum>
	pf_alignas_n(_Ty, _ColNum) operator|(
	 matrix<_Ty, _RowNum, _ColNum> const &__m1,
	 matrix<_Ty, _RowNum, _ColNum> const &__m2) pf_attr_noexcept
		requires(std::is_integral_v<_Ty>)
	{
		matrix<_Ty, _RowNum, _ColNum> pf_alignas_n(_Ty, _ColNum) tmp = __m1;
		return tmp																									|= __m2;
	}

	/// MATH: Matrix -> Operator^
	template<typename _Ty, size_t _RowNum, size_t _ColNum>
	pf_hint_nodiscard pf_decl_constexpr matrix<_Ty, _RowNum, _ColNum>
	pf_alignas_n(_Ty, _ColNum) operator^(
	 matrix<_Ty, _RowNum, _ColNum> const &__m,
	 _Ty __val) pf_attr_noexcept
		requires(std::is_integral_v<_Ty>)
	{
		matrix<_Ty, _RowNum, _ColNum> pf_alignas_n(_Ty, _ColNum) tmp;
		return tmp ^= __val;
	}
	template<typename _Ty, size_t _RowNum, size_t _ColNum>
	pf_hint_nodiscard pf_decl_constexpr matrix<_Ty, _RowNum, _ColNum>
	pf_alignas_n(_Ty, _ColNum) operator^(
	 matrix<_Ty, _RowNum, _ColNum> const &__m1,
	 matrix<_Ty, _RowNum, _ColNum> const &__m2) pf_attr_noexcept
		requires(std::is_integral_v<_Ty>)
	{
		matrix<_Ty, _RowNum, _ColNum> pf_alignas_n(_Ty, _ColNum) tmp = __m1;
		return tmp																									^= __m2;
	}

	/// MATH: Matrix -> Operator<<
	template<typename _Ty, size_t _RowNum, size_t _ColNum>
	pf_hint_nodiscard pf_decl_constexpr matrix<_Ty, _RowNum, _ColNum>
	pf_alignas_n(_Ty, _ColNum) operator<<(
	 matrix<_Ty, _RowNum, _ColNum> const &__m,
	 _Ty __val) pf_attr_noexcept
		requires(std::is_integral_v<_Ty>)
	{
		matrix<_Ty, _RowNum, _ColNum> pf_alignas_n(_Ty, _ColNum) tmp = __m;
		return tmp																								 <<= __val;
	}
	template<typename _Ty, size_t _RowNum, size_t _ColNum>
	pf_hint_nodiscard pf_decl_constexpr matrix<_Ty, _RowNum, _ColNum>
	pf_alignas_n(_Ty, _ColNum) operator<<(
	 matrix<_Ty, _RowNum, _ColNum> const &__m1,
	 matrix<_Ty, _RowNum, _ColNum> const &__m2) pf_attr_noexcept
		requires(std::is_integral_v<_Ty>)
	{
		matrix<_Ty, _RowNum, _ColNum> pf_alignas_n(_Ty, _ColNum) tmp = __m1;
		return tmp																								 <<= __m2;
	}

	/// MATH: Matrix -> Operator>>
	template<typename _Ty, size_t _RowNum, size_t _ColNum>
	pf_hint_nodiscard pf_decl_constexpr matrix<_Ty, _RowNum, _ColNum>
	pf_alignas_n(_Ty, _ColNum) operator>>(
	 matrix<_Ty, _RowNum, _ColNum> const &__m,
	 _Ty __val) pf_attr_noexcept
		requires(std::is_integral_v<_Ty>)
	{
		matrix<_Ty, _RowNum, _ColNum> pf_alignas_n(_Ty, _ColNum) tmp;
		return tmp >>= __val;
	}
	template<typename _Ty, size_t _RowNum, size_t _ColNum>
	pf_hint_nodiscard pf_decl_constexpr matrix<_Ty, _RowNum, _ColNum>
	pf_alignas_n(_Ty, _ColNum) operator>>(
	 matrix<_Ty, _RowNum, _ColNum> const &__m1,
	 matrix<_Ty, _RowNum, _ColNum> const &__m2) pf_attr_noexcept
		requires(std::is_integral_v<_Ty>)
	{
		matrix<_Ty, _RowNum, _ColNum> pf_alignas_n(_Ty, _ColNum) tmp = __m1;
		return tmp																								 >>= __m2;
	}

	/// MATH: Matrix -> Operator~
	template<typename _Ty, size_t _RowNum, size_t _ColNum>
	pf_hint_nodiscard pf_decl_constexpr matrix<_Ty, _RowNum, _ColNum>
	pf_alignas_n(_Ty, _ColNum) operator~(
	 matrix<_Ty, _RowNum, _ColNum> const &__m) pf_attr_noexcept
	{
		matrix<_Ty, _RowNum, _ColNum> pf_alignas_n(_Ty, _ColNum) tmp;
		for(size_t i = 0; i < _RowNum; ++i)
		{
			tmp[i] = ~__m[i];
		}
		return tmp;
	}

	/// MATH: Matrix -> Compare
	template<typename _Ty, size_t _RowNum, size_t _ColNum>
	pf_hint_nodiscard pf_decl_constexpr matrix<bool, _RowNum, _ColNum>
	equal(
	 matrix<_Ty, _RowNum, _ColNum> const &__l,
	 matrix<_Ty, _RowNum, _ColNum> const &__r) pf_attr_noexcept
	{
		return __l.equal(__r);
	}
	template<typename _Ty, size_t _RowNum, size_t _ColNum>
	pf_hint_nodiscard pf_decl_constexpr matrix<bool, _RowNum, _ColNum>
	not_equal(
	 matrix<_Ty, _RowNum, _ColNum> const &__l,
	 matrix<_Ty, _RowNum, _ColNum> const &__r) pf_attr_noexcept
	{
		return __l.not_equal(__r);
	}
	template<typename _Ty, size_t _RowNum, size_t _ColNum>
	pf_hint_nodiscard pf_decl_constexpr matrix<bool, _RowNum, _ColNum>
	greater(
	 matrix<_Ty, _RowNum, _ColNum> const &__l,
	 matrix<_Ty, _RowNum, _ColNum> const &__r) pf_attr_noexcept
	{
		return __l.greater(__r);
	}
	template<typename _Ty, size_t _RowNum, size_t _ColNum>
	pf_hint_nodiscard pf_decl_constexpr matrix<bool, _RowNum, _ColNum>
	greater_equal(
	 matrix<_Ty, _RowNum, _ColNum> const &__l,
	 matrix<_Ty, _RowNum, _ColNum> const &__r) pf_attr_noexcept
	{
		return __l.greater_equal(__r);
	}
	template<typename _Ty, size_t _RowNum, size_t _ColNum>
	pf_hint_nodiscard pf_decl_constexpr matrix<bool, _RowNum, _ColNum>
	smaller(
	 matrix<_Ty, _RowNum, _ColNum> const &__l,
	 matrix<_Ty, _RowNum, _ColNum> const &__r) pf_attr_noexcept
	{
		return __l.smaller(__r);
	}
	template<typename _Ty, size_t _RowNum, size_t _ColNum>
	pf_hint_nodiscard pf_decl_constexpr matrix<bool, _RowNum, _ColNum>
	smaller_equal(
	 matrix<_Ty, _RowNum, _ColNum> const &__l,
	 matrix<_Ty, _RowNum, _ColNum> const &__r) pf_attr_noexcept
	{
		return __l.smaller_equal(__r);
	}
}	 // namespace pul

// Include: Pulsar -> Math -> Matrix Specializations
#include "pulsar/math/matrix/matrix2.inl"
#include "pulsar/math/matrix/matrix2x3.inl"
#include "pulsar/math/matrix/matrix2x4.inl"
#include "pulsar/math/matrix/matrix3.inl"
#include "pulsar/math/matrix/matrix3x2.inl"
#include "pulsar/math/matrix/matrix3x4.inl"
#include "pulsar/math/matrix/matrix4.inl"
#include "pulsar/math/matrix/matrix4x2.inl"
#include "pulsar/math/matrix/matrix4x3.inl"

#endif	// !PULSAR_MATH_MATRIX_HPP
