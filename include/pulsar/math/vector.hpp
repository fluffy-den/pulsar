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
#include "pulsar/intrin.hpp"

// Include: C++
#include <array>

// Pulsar
namespace pul
{
	/// MATH: Vector
	template <typename _Ty, size_t _Num, simd_align_t _Simd = simd_select_v<_Ty, _Num>>
		requires(
				std::is_arithmetic_v<_Ty> &&_Num > 1
				&& (((is_simd_alignable_v<_Ty, _Num>)&&_Simd == SIMD_ALIGNED) || (_Simd == SIMD_UNALIGNED)))
	union vector;

	/// MATH: Vector -> SFINAE -> Is vector?
	template <typename _Ty>
	struct is_vector: std::false_type
	{};
	template <typename _Ty>
		requires(std::is_arithmetic_v<_Ty>)
	struct is_vector<_Ty>: std::true_type // Scalar is a vector of 1 component.
	{};
	template <typename _Ty, size_t _Num, simd_align_t _Simd>
	struct is_vector<vector<_Ty, _Num, _Simd>>: std::true_type
	{};
	template <typename _Ty>
	pf_decl_static pf_decl_constexpr bool is_vector_v = is_vector<_Ty>::value;

	/// MATH: Vector -> Concept
	template <typename _Ty>
	concept vector_c = is_vector_v<_Ty>;

	/// MATH: Vector -> SFINAE -> Elem Type
	template <typename _Ty>
	struct elem_typeof
	{
		using type = _Ty;
	};
	template <typename _Ty, size_t _Num, simd_align_t _Simd>
	struct elem_typeof<vector<_Ty, _Num, _Simd>>
	{
		using type = _Ty;
	};
	template <typename _Ty>
	using elem_typeof_t = typename elem_typeof<_Ty>::type;

	/// MATH: Vector -> SFINAE -> Elem Num
	template <typename _Ty>
	struct elem_numof: std::integral_constant<size_t, 1>
	{};
	template <typename _Ty, size_t _Num, simd_align_t _Simd>
	struct elem_numof<vector<_Ty, _Num, _Simd>>: std::integral_constant<size_t, _Num>
	{};
	template <typename _Ty>
	pf_decl_static pf_decl_constexpr size_t elem_numof_v = elem_numof<_Ty>::value;

	/// MATH: Vector -> SFINAE -> Is Simd Aligned
	template <typename _Ty>
	struct is_simd_aligned: std::false_type
	{};
	template <typename _Ty, size_t _Num, simd_align_t _Simd>
	struct is_simd_aligned<vector<_Ty, _Num, _Simd>>: std::integral_constant<bool, _Simd>
	{};
	template <typename _Ty>
	pf_decl_static pf_decl_constexpr bool is_simd_aligned_v = is_simd_aligned<_Ty>::value;

	/// MATH: Vector
	template <typename _Ty, size_t _Num, simd_align_t _Simd>
		requires(
				std::is_arithmetic_v<_Ty> &&_Num > 1
				&& (((is_simd_alignable_v<_Ty, _Num>)&&_Simd == SIMD_ALIGNED) || (_Simd == SIMD_UNALIGNED)))
	union alignas(simd_alignment_of_v<_Ty, _Num, _Simd>) vector
	{
		template <typename _TyF, size_t _NumF, simd_align_t _SimdF>
			requires(
					std::is_arithmetic_v<_Ty> &&_Num > 1
					&& (((is_simd_alignable_v<_Ty, _Num>)&&_Simd == SIMD_ALIGNED) || (_Simd == SIMD_UNALIGNED)))
		pf_decl_friend union vector;

	private:
		/// Initialize
		template <typename _InTy>
		pf_decl_inline pf_decl_constexpr void __init_rec(
				size_t &__index,
				_InTy const &__val) pf_attr_noexcept
		{
			this->store_[__index] = __val;
			++__index;
		}
		template <typename _InTy, size_t _InNum, simd_align_t _InSimd>
		pf_decl_inline pf_decl_constexpr void __init_rec(
				size_t &__index,
				vector<_InTy, _InNum, _InSimd> const &__r) pf_attr_noexcept
		{
			for (size_t i = 0; i < _InNum; ++i)
			{
				this->store_[__index++] = __r[i];
			}
		}
		template <typename _InTy, size_t _InNum>
		pf_decl_inline pf_decl_constexpr void __init_rec(
				size_t &__index,
				_InTy const (&__tab)[_InNum]) pf_attr_noexcept
		{
			for (size_t i = 0; i < _InNum; ++i)
			{
				this->store_[__index++] = __tab[i];
			}
		}
		template <typename _InTy, typename... _InArgs>
		pf_decl_inline pf_decl_constexpr void __init(
				size_t &__index,
				_InTy const &__val,
				_InArgs const &...__args) pf_attr_noexcept
		{
			this->__init_rec(__index, __val);
			if pf_decl_constexpr (sizeof...(_InArgs) > 0)
			{
				this->__init(__index, __args...);
			}
		}

	public:
		/// Constructors
		pf_decl_inline pf_decl_constexpr vector(
				_Ty __val = static_cast<_Ty>(0)) pf_attr_noexcept
				: store_{ __val }
		{}
		template <simd_align_t _SimdR>
		pf_decl_inline pf_decl_constexpr vector(
				vector<_Ty, _Num, _SimdR> const &__r) pf_attr_noexcept
				: store_(__r.store_)
		{}
		template <typename... _InArgs>
		pf_decl_inline pf_decl_constexpr vector(
				_InArgs const &...__args) pf_attr_noexcept
				requires(
						(is_vector_v<_InArgs> && ...)
						&& (std::is_same_v<elem_typeof_t<_Ty>, elem_typeof_t<_InArgs>> && ...)
						&& (elem_numof_v<_InArgs> + ...) == _Num)
		{
			size_t index = 0;
			this->__init(index, __args...);
		}

		/// Destructor
		pf_decl_inline pf_decl_constexpr ~vector() pf_attr_noexcept = default;

		/// Operator=
		pf_decl_inline pf_decl_constexpr vector<_Ty, _Num, _Simd> &operator=(
				vector<_Ty, _Num, _Simd> const &__r) pf_attr_noexcept = default;

		/// Operator[]
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr _Ty &operator[](
				size_t __index) pf_attr_noexcept
		{
			return this->store_[__index];
		}
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const _Ty &operator[](
				size_t __index) const pf_attr_noexcept
		{
			return this->store_[__index];
		}

		/// Operator+=
		pf_decl_inline pf_decl_constexpr vector<_Ty, _Num, _Simd> &operator+=(
				_Ty __val) pf_attr_noexcept
		{
			for (size_t i = 0; i < _Num; ++i)
			{
				this->store_[i] += __val;
			}
			return *this;
		}
		template <simd_align_t _SimdR>
		pf_decl_inline pf_decl_constexpr vector<_Ty, _Num, _Simd> &operator+=(
				vector<_Ty, _Num, _SimdR> const &__r) pf_attr_noexcept
		{
			for (size_t i = 0; i < _Num; ++i)
			{
				this->store_[i] += __r[i];
			}
			return *this;
		}

		/// Operator-=
		pf_decl_inline pf_decl_constexpr vector<_Ty, _Num, _Simd> &operator-=(
				_Ty __val) pf_attr_noexcept
		{
			for (size_t i = 0; i < _Num; ++i)
			{
				this->store_[i] -= __val;
			}
			return *this;
		}
		template <simd_align_t _SimdR>
		pf_decl_inline pf_decl_constexpr vector<_Ty, _Num, _Simd> &operator-=(
				vector<_Ty, _Num, _SimdR> const &__r) pf_attr_noexcept
		{
			for (size_t i = 0; i < _Num; ++i)
			{
				this->store_[i] -= __r[i];
			}
			return *this;
		}

		/// Operator*=
		pf_decl_inline pf_decl_constexpr vector<_Ty, _Num, _Simd> &operator*=(
				_Ty __val) pf_attr_noexcept
		{
			for (size_t i = 0; i < _Num; ++i)
			{
				this->store_[i] *= __val;
			}
			return *this;
		}
		template <simd_align_t _SimdR>
		pf_decl_inline pf_decl_constexpr vector<_Ty, _Num, _Simd> &operator*=(
				vector<_Ty, _Num, _SimdR> const &__r) pf_attr_noexcept
		{
			for (size_t i = 0; i < _Num; ++i)
			{
				this->store_[i] *= __r[i];
			}
			return *this;
		}

		/// Operator/=
		pf_decl_inline pf_decl_constexpr vector<_Ty, _Num, _Simd> &operator/=(
				_Ty __val) pf_attr_noexcept
		{
			for (size_t i = 0; i < _Num; ++i)
			{
				this->store_[i] /= __val;
			}
			return *this;
		}
		template <simd_align_t _SimdR>
		pf_decl_inline pf_decl_constexpr vector<_Ty, _Num, _Simd> &operator/=(
				vector<_Ty, _Num, _SimdR> const &__r) pf_attr_noexcept
		{
			for (size_t i = 0; i < _Num; ++i)
			{
				this->store_[i] /= __r[i];
			}
			return *this;
		}

		/// Operator%=
		pf_decl_inline pf_decl_constexpr vector<_Ty, _Num, _Simd> &operator%=(
				_Ty __val) pf_attr_noexcept
				requires(std::is_integral_v<_Ty>)
		{
			for (size_t i = 0; i < _Num; ++i)
			{
				this->store_[i] %= __val;
			}
			return *this;
		}
		template <simd_align_t _SimdR>
		pf_decl_inline pf_decl_constexpr vector<_Ty, _Num, _Simd> &operator%=(
				vector<_Ty, _Num, _SimdR> const &__r) pf_attr_noexcept
				requires(std::is_integral_v<_Ty>)
		{
			for (size_t i = 0; i < _Num; ++i)
			{
				this->store_[i] %= __r[i];
			}
			return *this;
		}

		/// Operator&=
		pf_decl_inline pf_decl_constexpr vector<_Ty, _Num, _Simd> &operator&=(
				_Ty __val) pf_attr_noexcept
				requires(std::is_integral_v<_Ty>)
		{
			for (size_t i = 0; i < _Num; ++i)
			{
				this->store_[i] &= __val;
			}
			return *this;
		}
		template <simd_align_t _SimdR>
		pf_decl_inline pf_decl_constexpr vector<_Ty, _Num, _Simd> &operator&=(
				vector<_Ty, _Num, _SimdR> const &__r) pf_attr_noexcept
				requires(std::is_integral_v<_Ty>)
		{
			for (size_t i = 0; i < _Num; ++i)
			{
				this->store_[i] &= __r[i];
			}
			return *this;
		}

		/// Operator|=
		pf_decl_inline pf_decl_constexpr vector<_Ty, _Num, _Simd> &operator|=(
				_Ty __val) pf_attr_noexcept
				requires(std::is_integral_v<_Ty>)
		{
			for (size_t i = 0; i < _Num; ++i)
			{
				this->store_[i] |= __val;
			}
			return *this;
		}
		template <simd_align_t _SimdR>
		pf_decl_inline pf_decl_constexpr vector<_Ty, _Num, _Simd> &operator|=(
				vector<_Ty, _Num, _SimdR> const &__r) pf_attr_noexcept
				requires(std::is_integral_v<_Ty>)
		{
			for (size_t i = 0; i < _Num; ++i)
			{
				this->store_[i] |= __r[i];
			}
			return *this;
		}

		/// Operator^=
		pf_decl_inline pf_decl_constexpr vector<_Ty, _Num, _Simd> &operator^=(
				_Ty __val) pf_attr_noexcept
				requires(std::is_integral_v<_Ty>)
		{
			for (size_t i = 0; i < _Num; ++i)
			{
				this->store_[i] ^= __val;
			}
			return *this;
		}
		template <simd_align_t _SimdR>
		pf_decl_inline pf_decl_constexpr vector<_Ty, _Num, _Simd> &operator^=(
				vector<_Ty, _Num, _SimdR> const &__r) pf_attr_noexcept
				requires(std::is_integral_v<_Ty>)
		{
			for (size_t i = 0; i < _Num; ++i)
			{
				this->store_[i] ^= __r[i];
			}
			return *this;
		}

		/// Operator<<=
		pf_decl_inline pf_decl_constexpr vector<_Ty, _Num, _Simd> &operator<<=(
				_Ty __val) pf_attr_noexcept
				requires(std::is_integral_v<_Ty>)
		{
			for (size_t i = 0; i < _Num; ++i)
			{
				this->store_[i] <<= __val;
			}
			return *this;
		}
		template <simd_align_t _SimdR>
		pf_decl_inline pf_decl_constexpr vector<_Ty, _Num, _Simd> &operator<<=(
				vector<_Ty, _Num, _SimdR> const &__r) pf_attr_noexcept
				requires(std::is_integral_v<_Ty>)
		{
			for (size_t i = 0; i < _Num; ++i)
			{
				this->store_[i] <<= __r[i];
			}
			return *this;
		}

		/// Operator>>=
		pf_decl_inline pf_decl_constexpr vector<_Ty, _Num, _Simd> &operator>>=(
				_Ty __val) pf_attr_noexcept
				requires(std::is_integral_v<_Ty>)
		{
			for (size_t i = 0; i < _Num; ++i)
			{
				this->store_[i] >>= __val;
			}
			return *this;
		}
		template <simd_align_t _SimdR>
		pf_decl_inline pf_decl_constexpr vector<_Ty, _Num, _Simd> &operator>>=(
				vector<_Ty, _Num, _SimdR> const &__r) pf_attr_noexcept
				requires(std::is_integral_v<_Ty>)
		{
			for (size_t i = 0; i < _Num; ++i)
			{
				this->store_[i] >>= __r[i];
			}
			return *this;
		}

		/// Operator==
		template <simd_align_t _SimdR>
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr bool operator==(
				vector<_Ty, _Num, _SimdR> const &__r) const pf_attr_noexcept
		{
			for (size_t i = 0; i < _Num; ++i)
			{
				if (this->store_[i] != __r[i])
					return false;
			}
			return true;
		}

		/// Compare
		template <simd_align_t _SimdR>
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr vector<bool, _Num> equal(
				vector<_Ty, _Num, _SimdR> const &__r) const pf_attr_noexcept
		{
			vector<bool, _Num> tmp;
			for (size_t i = 0; i < _Num; ++i)
			{
				tmp[i] = this->store_[i] == __r[i];
			}
			return tmp;
		}
		template <simd_align_t _SimdR>
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr vector<bool, _Num> not_equal(
				vector<_Ty, _Num, _SimdR> const &__r) const pf_attr_noexcept
		{
			vector<bool, _Num> tmp;
			for (size_t i = 0; i < _Num; ++i)
			{
				tmp[i] = this->store_[i] != __r[i];
			}
			return tmp;
		}
		template <simd_align_t _SimdR>
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr vector<bool, _Num> greater(
				vector<_Ty, _Num, _SimdR> const &__v) const pf_attr_noexcept
		{
			vector<bool, _Num> tmp;
			for (size_t i = 0; i < _Num; ++i)
			{
				tmp[i] = this->store_[i] > __v[i];
			}
			return tmp;
		}
		template <simd_align_t _SimdR>
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr vector<bool, _Num> greater_equal(
				vector<_Ty, _Num, _SimdR> const &__v) const pf_attr_noexcept
		{
			vector<bool, _Num> tmp;
			for (size_t i = 0; i < _Num; ++i)
			{
				tmp[i] = this->store_[i] >= __v[i];
			}
			return tmp;
		}
		template <simd_align_t _SimdR>
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr vector<bool, _Num> smaller(
				vector<_Ty, _Num, _SimdR> const &__v) const pf_attr_noexcept
		{
			vector<bool, _Num> tmp;
			for (size_t i = 0; i < _Num; ++i)
			{
				tmp[i] = this->store_[i] < __v[i];
			}
			return tmp;
		}
		template <simd_align_t _SimdR>
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr vector<bool, _Num> smaller_equal(
				vector<_Ty, _Num, _SimdR> const &__v) const pf_attr_noexcept
		{
			vector<bool, _Num> tmp;
			for (size_t i = 0; i < _Num; ++i)
			{
				tmp[i] = this->store_[i] <= __v[i];
			}
			return tmp;
		}

		/// Data
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr _Ty *data() pf_attr_noexcept
		{
			return this->store_.data();
		}
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const _Ty *data() const pf_attr_noexcept
		{
			return this->store_.data();
		}

	private:
		/// Store
		std::array<_Ty, _Num> store_;
	};

	/// MATH: Vector -> Operator+
	template <typename _Ty, size_t _Num, simd_align_t _Simd>
	pf_hint_nodiscard pf_decl_inline pf_decl_constexpr vector<_Ty, _Num, _Simd> operator+(
			vector<_Ty, _Num, _Simd> const &__l,
			_Ty __val) pf_attr_noexcept
	{
		vector<_Ty, _Num, _Simd> tmp = __l;
		return tmp += __val;
	}
	template <typename _Ty, size_t _Num, simd_align_t _Simd>
	pf_hint_nodiscard pf_decl_inline pf_decl_constexpr vector<_Ty, _Num, _Simd> operator+(
			_Ty __val,
			vector<_Ty, _Num, _Simd> const &__r) pf_attr_noexcept
	{
		vector<_Ty, _Num, _Simd> tmp = __r;
		return tmp += __val;
	}
	template <typename _Ty, size_t _Num, simd_align_t _SimdL, simd_align_t _SimdR>
	pf_hint_nodiscard pf_decl_inline pf_decl_constexpr vector<_Ty, _Num> operator+(
			vector<_Ty, _Num, _SimdL> const &__l,
			vector<_Ty, _Num, _SimdR> const &__r) pf_attr_noexcept
	{
		vector<_Ty, _Num> tmp = __l;
		return tmp += __r;
	}

	/// MATH: Vector -> Operator-
	template <typename _Ty, size_t _Num, simd_align_t _Simd>
	pf_hint_nodiscard pf_decl_inline pf_decl_constexpr vector<_Ty, _Num, _Simd> operator-(
			vector<_Ty, _Num, _Simd> const &__l,
			_Ty __val) pf_attr_noexcept
	{
		vector<_Ty, _Num, _Simd> tmp = __l;
		return tmp -= __val;
	}
	template <typename _Ty, size_t _Num, simd_align_t _Simd>
	pf_hint_nodiscard pf_decl_inline pf_decl_constexpr vector<_Ty, _Num, _Simd> operator-(
			_Ty __val,
			vector<_Ty, _Num, _Simd> const &__r) pf_attr_noexcept
	{
		vector<_Ty, _Num, _Simd> tmp = __r;
		return tmp -= __val;
	}
	template <typename _Ty, size_t _Num, simd_align_t _SimdL, simd_align_t _SimdR>
	pf_hint_nodiscard pf_decl_inline pf_decl_constexpr vector<_Ty, _Num> operator-(
			vector<_Ty, _Num, _SimdL> const &__l,
			vector<_Ty, _Num, _SimdR> const &__r) pf_attr_noexcept
	{
		vector<_Ty, _Num> tmp = __l;
		return tmp -= __r;
	}

	/// MATH: Vector -> Operator*
	template <typename _Ty, size_t _Num, simd_align_t _Simd>
	pf_hint_nodiscard pf_decl_inline pf_decl_constexpr vector<_Ty, _Num, _Simd> operator*(
			vector<_Ty, _Num, _Simd> const &__l,
			_Ty __val) pf_attr_noexcept
	{
		vector<_Ty, _Num, _Simd> tmp = __l;
		return tmp *= __val;
	}
	template <typename _Ty, size_t _Num, simd_align_t _Simd>
	pf_hint_nodiscard pf_decl_inline pf_decl_constexpr vector<_Ty, _Num, _Simd> operator*(
			_Ty __val,
			vector<_Ty, _Num, _Simd> const &__r) pf_attr_noexcept
	{
		vector<_Ty, _Num, _Simd> tmp = __r;
		return tmp *= __val;
	}
	template <typename _Ty, size_t _Num, simd_align_t _SimdL, simd_align_t _SimdR>
	pf_hint_nodiscard pf_decl_inline pf_decl_constexpr vector<_Ty, _Num> operator*(
			vector<_Ty, _Num, _SimdL> const &__l,
			vector<_Ty, _Num, _SimdR> const &__r) pf_attr_noexcept
	{
		vector<_Ty, _Num> tmp = __l;
		return tmp *= __r;
	}

	/// MATH: Vector -> Operator/
	template <typename _Ty, size_t _Num, simd_align_t _Simd>
	pf_hint_nodiscard pf_decl_inline pf_decl_constexpr vector<_Ty, _Num, _Simd> operator/(
			vector<_Ty, _Num, _Simd> const &__l,
			_Ty __val) pf_attr_noexcept
	{
		vector<_Ty, _Num, _Simd> tmp = __l;
		return tmp /= __val;
	}
	template <typename _Ty, size_t _Num, simd_align_t _Simd>
	pf_hint_nodiscard pf_decl_inline pf_decl_constexpr vector<_Ty, _Num, _Simd> operator/(
			_Ty __val,
			vector<_Ty, _Num, _Simd> const &__r) pf_attr_noexcept
	{
		vector<_Ty, _Num, _Simd> tmp = __r;
		return tmp /= __val;
	}
	template <typename _Ty, size_t _Num, simd_align_t _SimdL, simd_align_t _SimdR>
	pf_hint_nodiscard pf_decl_inline pf_decl_constexpr vector<_Ty, _Num> operator/(
			vector<_Ty, _Num, _SimdL> const &__l,
			vector<_Ty, _Num, _SimdR> const &__r) pf_attr_noexcept
	{
		vector<_Ty, _Num> tmp = __l;
		return tmp /= __r;
	}

	/// MATH: Vector -> Operator%
	template <typename _Ty, size_t _Num, simd_align_t _Simd>
	pf_hint_nodiscard pf_decl_inline pf_decl_constexpr vector<_Ty, _Num, _Simd> operator%(
			vector<_Ty, _Num, _Simd> const &__l,
			_Ty __val) pf_attr_noexcept
			requires(std::is_integral_v<_Ty>)
	{
		vector<_Ty, _Num, _Simd> tmp = __l;
		return tmp %= __val;
	}
	template <typename _Ty, size_t _Num, simd_align_t _SimdL, simd_align_t _SimdR>
	pf_hint_nodiscard pf_decl_inline pf_decl_constexpr vector<_Ty, _Num> operator%(
			vector<_Ty, _Num, _SimdL> const &__l,
			vector<_Ty, _Num, _SimdR> const &__r) pf_attr_noexcept
			requires(std::is_integral_v<_Ty>)
	{
		vector<_Ty, _Num> tmp = __l;
		return tmp %= __r;
	}

	/// MATH: Vector -> Operator&
	template <typename _Ty, size_t _Num, simd_align_t _Simd>
	pf_hint_nodiscard pf_decl_inline pf_decl_constexpr vector<_Ty, _Num, _Simd> operator&(
			vector<_Ty, _Num, _Simd> const &__l,
			_Ty __val) pf_attr_noexcept
			requires(std::is_integral_v<_Ty>)
	{
		vector<_Ty, _Num, _Simd> tmp = __l;
		return tmp &= __val;
	}
	template <typename _Ty, size_t _Num, simd_align_t _SimdL, simd_align_t _SimdR>
	pf_hint_nodiscard pf_decl_inline pf_decl_constexpr vector<_Ty, _Num> operator&(
			vector<_Ty, _Num, _SimdL> const &__l,
			vector<_Ty, _Num, _SimdR> const &__r) pf_attr_noexcept
			requires(std::is_integral_v<_Ty>)
	{
		vector<_Ty, _Num> tmp = __l;
		return tmp &= __r;
	}

	/// MATH: Vector -> Operator|
	template <typename _Ty, size_t _Num, simd_align_t _Simd>
	pf_hint_nodiscard pf_decl_inline pf_decl_constexpr vector<_Ty, _Num, _Simd> operator|(
			vector<_Ty, _Num, _Simd> const &__l,
			_Ty __val) pf_attr_noexcept
			requires(std::is_integral_v<_Ty>)
	{
		vector<_Ty, _Num, _Simd> tmp = __l;
		return tmp |= __val;
	}
	template <typename _Ty, size_t _Num, simd_align_t _SimdL, simd_align_t _SimdR>
	pf_hint_nodiscard pf_decl_inline pf_decl_constexpr vector<_Ty, _Num> operator|(
			vector<_Ty, _Num, _SimdL> const &__l,
			vector<_Ty, _Num, _SimdR> const &__r) pf_attr_noexcept
			requires(std::is_integral_v<_Ty>)
	{
		vector<_Ty, _Num> tmp = __l;
		return tmp |= __r;
	}

	/// MATH: Vector -> Operator^
	template <typename _Ty, size_t _Num, simd_align_t _Simd>
	pf_hint_nodiscard pf_decl_inline pf_decl_constexpr vector<_Ty, _Num, _Simd> operator^(
			vector<_Ty, _Num, _Simd> const &__l,
			_Ty __val) pf_attr_noexcept
			requires(std::is_integral_v<_Ty>)
	{
		vector<_Ty, _Num, _Simd> tmp = __l;
		return tmp ^= __val;
	}
	template <typename _Ty, size_t _Num, simd_align_t _SimdL, simd_align_t _SimdR>
	pf_hint_nodiscard pf_decl_inline pf_decl_constexpr vector<_Ty, _Num> operator^(
			vector<_Ty, _Num, _SimdL> const &__l,
			vector<_Ty, _Num, _SimdR> const &__r) pf_attr_noexcept
			requires(std::is_integral_v<_Ty>)
	{
		vector<_Ty, _Num> tmp = __l;
		return tmp ^= __r;
	}

	/// MATH: Vector -> Operator<<
	template <typename _Ty, size_t _Num, simd_align_t _Simd>
	pf_hint_nodiscard pf_decl_inline pf_decl_constexpr vector<_Ty, _Num, _Simd> operator<<(
			vector<_Ty, _Num, _Simd> const &__l,
			_Ty __val) pf_attr_noexcept
			requires(std::is_integral_v<_Ty>)
	{
		vector<_Ty, _Num, _Simd> tmp = __l;
		return tmp <<= __val;
	}
	template <typename _Ty, size_t _Num, simd_align_t _SimdL, simd_align_t _SimdR>
	pf_hint_nodiscard pf_decl_inline pf_decl_constexpr vector<_Ty, _Num> operator<<(
			vector<_Ty, _Num, _SimdL> const &__l,
			vector<_Ty, _Num, _SimdR> const &__r) pf_attr_noexcept
			requires(std::is_integral_v<_Ty>)
	{
		vector<_Ty, _Num> tmp = __l;
		return tmp <<= __r;
	}

	/// MATH: Vector -> Operator>>
	template <typename _Ty, size_t _Num, simd_align_t _Simd>
	pf_hint_nodiscard pf_decl_inline pf_decl_constexpr vector<_Ty, _Num, _Simd> operator>>(
			vector<_Ty, _Num, _Simd> const &__l,
			_Ty __val) pf_attr_noexcept
			requires(std::is_integral_v<_Ty>)
	{
		vector<_Ty, _Num, _Simd> tmp = __l;
		return tmp >>= __val;
	}
	template <typename _Ty, size_t _Num, simd_align_t _SimdL, simd_align_t _SimdR>
	pf_hint_nodiscard pf_decl_inline pf_decl_constexpr vector<_Ty, _Num> operator>>(
			vector<_Ty, _Num, _SimdL> const &__l,
			vector<_Ty, _Num, _SimdR> const &__r) pf_attr_noexcept
			requires(std::is_integral_v<_Ty>)
	{
		vector<_Ty, _Num> tmp = __l;
		return tmp >>= __r;
	}

	/// MATH: Vector -> Operator~
	template <typename _Ty, size_t _Num, simd_align_t _Simd>
	pf_hint_nodiscard pf_decl_inline pf_decl_constexpr vector<_Ty, _Num, _Simd> operator~(
			vector<_Ty, _Num, _Simd> const &__r) pf_attr_noexcept
	{
		vector<_Ty, _Num, _Simd> tmp;
		for (size_t i = 0; i < _Num; ++i)
		{
			tmp[i] = ~__r[i];
		}
		return tmp;
	}

	/// MATH: Vector -> Compare
	template <typename _Ty, size_t _Num, simd_align_t _SimdL, simd_align_t _SimdR>
	pf_hint_nodiscard pf_decl_inline pf_decl_constexpr vector<bool, _Num> equal(
			vector<_Ty, _Num, _SimdL> const &__l,
			vector<_Ty, _Num, _SimdR> const &__r) pf_attr_noexcept
	{
		return __l.equal(__r);
	}
	template <typename _Ty, size_t _Num, simd_align_t _SimdL, simd_align_t _SimdR>
	pf_hint_nodiscard pf_decl_inline pf_decl_constexpr vector<bool, _Num> not_equal(
			vector<_Ty, _Num, _SimdL> const &__l,
			vector<_Ty, _Num, _SimdR> const &__r) pf_attr_noexcept
	{
		return __l.not_equal(__r);
	}
	template <typename _Ty, size_t _Num, simd_align_t _SimdL, simd_align_t _SimdR>
	pf_hint_nodiscard pf_decl_inline pf_decl_constexpr vector<bool, _Num> greater(
			vector<_Ty, _Num, _SimdL> const &__l,
			vector<_Ty, _Num, _SimdR> const &__r) pf_attr_noexcept
	{
		return __l.greater(__r);
	}
	template <typename _Ty, size_t _Num, simd_align_t _SimdL, simd_align_t _SimdR>
	pf_hint_nodiscard pf_decl_inline pf_decl_constexpr vector<bool, _Num> greater_equal(
			vector<_Ty, _Num, _SimdL> const &__l,
			vector<_Ty, _Num, _SimdR> const &__r) pf_attr_noexcept
	{
		return __l.greater_equal(__r);
	}
	template <typename _Ty, size_t _Num, simd_align_t _SimdL, simd_align_t _SimdR>
	pf_hint_nodiscard pf_decl_inline pf_decl_constexpr vector<bool, _Num> smaller(
			vector<_Ty, _Num, _SimdL> const &__l,
			vector<_Ty, _Num, _SimdR> const &__r) pf_attr_noexcept
	{
		return __l.smaller(__r);
	}
	template <typename _Ty, size_t _Num, simd_align_t _SimdL, simd_align_t _SimdR>
	pf_hint_nodiscard pf_decl_inline pf_decl_constexpr vector<bool, _Num> smaller_equal(
			vector<_Ty, _Num, _SimdL> const &__l,
			vector<_Ty, _Num, _SimdR> const &__r) pf_attr_noexcept
	{
		return __l.smaller_equal(__r);
	}

	/// MATH: Vector -> CTAD
	template <typename _InTy, typename... _InArgs>
		requires(
				(is_vector_v<_InArgs> && ...)
				&& (std::is_same_v<elem_typeof_t<_InTy>, elem_typeof_t<_InArgs>> && ...))
	vector(_InTy &&, _InArgs &&...)
	->vector<
			elem_typeof_t<_InTy>,
			1 + (elem_numof_v<_InArgs> + ...),
			simd_select_v<_InTy, 1 + (elem_numof_v<_InArgs> + ...)>>;
	template <simd_align_t _Simd, typename _InTy, typename... _InArgs>
		requires(
				(is_vector_v<_InArgs> && ...)
				&& (std::is_same_v<elem_typeof_t<_InTy>, elem_typeof_t<_InArgs>> && ...))
	vector(_InTy &&, _InArgs &&...)
	->vector<
			elem_typeof_t<_InTy>,
			1 + (elem_numof_v<_InArgs> + ...),
			_Simd>;
}

// Include: Pulsar -> Math -> Vectors Specializations
#include "pulsar/math/vector/vector2.inl"
#include "pulsar/math/vector/vector3.inl"
#include "pulsar/math/vector/vector4.inl"

#endif // !PULSAR_MATH_VECTOR_HPP