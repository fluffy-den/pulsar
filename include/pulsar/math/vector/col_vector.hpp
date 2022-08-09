/*! @file   col_vector.hpp
 *  @author Fluffy (noe.louis-quentin@hotmail.fr)
 *  @brief	Definition of line vectors.
 *  @date   31-05-2022
 *
 *  @copyright Copyright (c) 2022 - Pulsar Software
 *
 *  @since 0.1.1
 */

#ifndef PULSAR_MATH_COL_VECTOR_HPP
#define PULSAR_MATH_COL_VECTOR_HPP 1

// Include: Pulsar
#include "pulsar/intrin.hpp"

// Include: C++
#include <array>
#include <type_traits>

// Pulsar
namespace pul
{
	// Math
	namespace math
	{
		/// MATH: Col Vector -> Definition
		template <typename _Ty, size_t _Num, simd_align_t _SIMD>
			requires(
					std::is_arithmetic_v<_Ty> &&_Num > 1
					&& (((is_simd_alignable_v<_Ty, _Num>)&&_SIMD == SIMD_ALIGNED) || (_SIMD == SIMD_UNALIGNED)))
		union col_vector;

		/// MATH: SFINAE
		/// Is col vector?
		template <typename _Ty>
		struct is_col_vector: public std::false_type
		{};
		template <typename _Ty>
			requires(std::is_arithmetic_v<_Ty>)
		struct is_col_vector<_Ty>: public std::true_type
		{};
		template <typename _Ty, size_t _Num, simd_align_t _SIMD>
		struct is_col_vector<col_vector<_Ty, _Num, _SIMD>>: public std::true_type
		{};
		template <typename _Ty>
		pf_decl_static pf_decl_constexpr bool is_col_vector_v = is_col_vector<_Ty>::value;

		/// Is col vector same type?
		template <typename _TyA, typename _TyB>
		struct have_same_type: public std::false_type
		{};
		template <typename _TyA, typename _TyB>
			requires(
					std::is_same_v<
							std::remove_const_t<std::remove_all_extents_t<_TyA>>,
							std::remove_const_t<std::remove_all_extents_t<_TyB>>>)
		struct have_same_type<_TyA, _TyB>: public std::true_type
		{};
		template <typename _TyA, typename _TyB, size_t _NumB, simd_align_t _SIMDB>
		struct have_same_type<_TyA, col_vector<_TyB, _NumB, _SIMDB>>: public std::true_type
		{};
		template <typename _TyA, typename _TyB>
		pf_decl_static pf_decl_constexpr bool have_same_type_v = have_same_type<_TyA, _TyB>::value;

		/// Num of col elem
		template <typename _Ty>
		struct num_of_col_elem: public std::integral_constant<size_t, 1>
		{};
		template <typename _Ty, size_t _Num, simd_align_t _SIMD>
		struct num_of_col_elem<col_vector<_Ty, _Num, _SIMD>>: public std::integral_constant<size_t, _Num>
		{};
		template <typename _Ty>
		pf_decl_static pf_decl_constexpr size_t num_of_col_elem_v = num_of_col_elem<_Ty>::value;

		/// Type Helper
		template <typename _Ty>
		struct arithmetic_type_of
		{
			using type = std::remove_const_t<std::remove_all_extents_t<_Ty>>;
		};
		template <typename _Ty, size_t _Num, simd_align_t _SIMD>
		struct arithmetic_type_of<col_vector<_Ty, _Num, _SIMD>>
		{
			using type = _Ty;
		};
		template <typename _Ty>
		using arithmetic_type_of_t = typename arithmetic_type_of<_Ty>::type;

		/// SIMD Helper
		template <typename _Ty>
		struct used_simd: public std::integral_constant<simd_align_t, SIMD_UNALIGNED>
		{};
		template <typename _Ty, size_t _Num, simd_align_t _SIMD>
		struct used_simd<col_vector<_Ty, _Num, _SIMD>>: public std::integral_constant<simd_align_t, _SIMD>
		{};
		template <typename _Ty>
		pf_decl_static pf_decl_constexpr simd_align_t used_simd_v = used_simd<_Ty>::value;

		/// MATH: Col Vector -> Concept
		template <typename _Ty>
		concept col_vector_c = is_col_vector_v<_Ty>;

		/// MATH: Col Vector
		template <typename _Ty, size_t _Num, simd_align_t _SIMD = simd_select_v<_Ty, _Num>>
			requires(
					std::is_arithmetic_v<_Ty> &&_Num > 1
					&& (((is_simd_alignable_v<_Ty, _Num>)&&_SIMD == SIMD_ALIGNED) || (_SIMD == SIMD_UNALIGNED)))
		union alignas(simd_alignment_of_v<_Ty, _Num, _SIMD>) col_vector pf_attr_final
		{
			template <typename _TyF, size_t _NumF, simd_align_t _SIMDF>
			pf_decl_friend union col_vector;

			using type																			 = _Ty;
			pf_decl_static pf_decl_constexpr size_t num			 = _Num;
			pf_decl_static pf_decl_constexpr bool simd_value = _SIMD;

		private:
			template <typename _InTy>
			pf_decl_constexpr void __init_rec(
					size_t &__index,
					_InTy __val) pf_attr_noexcept
			{
				this->store_[__index] = __val;
				++__index;
			}
			template <typename _InTy, size_t _InNum, simd_align_t _InSIMD>
			pf_decl_constexpr void __init_rec(
					size_t &__index,
					col_vector<_InTy, _InNum, _InSIMD> const &__r) pf_attr_noexcept
			{
				for (size_t i = 0; i < _InNum; ++i)
				{
					this->store_[__index++] = __r[i];
				}
			}
			template <typename _InTy, size_t _InNum>
			pf_decl_constexpr void __init_rec(
					size_t &__index,
					const _InTy (&__tab)[_InNum]) pf_attr_noexcept
			{
				for (size_t i = 0; i < _InNum; ++i)
				{
					this->store_[__index++] = __tab[i];
				}
			}
			template <typename _InTy, typename... _InArgs>
			pf_decl_constexpr void __init(
					size_t &__index,
					_InTy __val,
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
			pf_decl_constexpr col_vector(
					_Ty __val = static_cast<_Ty>(0)) pf_attr_noexcept
					: store_{ __val }
			{}
			template <simd_align_t _RSIMD>
			pf_decl_constexpr col_vector(
					col_vector<_Ty, _Num, _RSIMD> const &__r) pf_attr_noexcept
					: store_(__r.store_)
			{}
			template <typename... _InArgs>
				requires(
						(is_col_vector_v<_InArgs> && ...)
						&& (have_same_type_v<_Ty, _InArgs> && ...)
						&& (num_of_col_elem_v<_InArgs> + ...) == _Num)
			pf_decl_constexpr col_vector(
					_InArgs const &...__args)
			pf_attr_noexcept
			{
				size_t index = 0;
				this->__init(index, __args...);
			}

			/// Operator[]
			pf_hint_nodiscard pf_decl_constexpr _Ty &operator[](
					size_t __index) pf_attr_noexcept
			{
				return this->store_[__index];
			}
			pf_hint_nodiscard pf_decl_constexpr const _Ty &operator[](
					size_t __index) const pf_attr_noexcept
			{
				return this->store_[__index];
			}

			/// Operator+=
			template <simd_align_t _RSIMD>
			pf_hint_nodiscard pf_decl_constexpr col_vector<_Ty, _Num, _SIMD> &operator+=(
					col_vector<_Ty, _Num, _RSIMD> const &__r) pf_attr_noexcept
			{
				for (size_t i = 0; i < _Num; ++i)
				{
					this->store_[i] += __r[i];
				}
				return *this;
			}

			/// Operator-=
			template <simd_align_t _RSIMD>
			pf_decl_constexpr col_vector<_Ty, _Num, _SIMD> &operator-=(
					col_vector<_Ty, _Num, _RSIMD> const &__r) pf_attr_noexcept
			{
				for (size_t i = 0; i < _Num; ++i)
				{
					this->store_[i] -= __r[i];
				}
				return *this;
			}

			/// Operator*=
			pf_decl_constexpr col_vector<_Ty, _Num, _SIMD> &operator*=(
					_Ty __val) pf_attr_noexcept
			{
				for (size_t i = 0; i < _Num; ++i)
				{
					this->store_[i] *= __val;
				}
				return *this;
			}

			/// Operator/=
			pf_decl_constexpr col_vector<_Ty, _Num, _SIMD> &operator/=(
					_Ty __val) pf_attr_noexcept
			{
				for (size_t i = 0; i < _Num; ++i)
				{
					this->store_[i] /= __val;
				}
				return *this;
			}

			/// Operator%=
			template <simd_align_t _RSIMD>
			pf_decl_constexpr col_vector<_Ty, _Num, _SIMD> &operator%=(
					col_vector<_Ty, _Num, _RSIMD> const &__r) pf_attr_noexcept requires(std::is_integral_v<_Ty>)
			{
				for (size_t i = 0; i < _Num; ++i)
				{
					this->store_[i] %= __r[i];
				}
				return *this;
			}

			/// Operator&=
			template <simd_align_t _RSIMD>
			pf_decl_constexpr col_vector<_Ty, _Num, _SIMD> &operator&=(
					col_vector<_Ty, _Num, _RSIMD> const &__r) pf_attr_noexcept requires(std::is_integral_v<_Ty>)
			{
				for (size_t i = 0; i < _Num; ++i)
				{
					this->store_[i] &= __r[i];
				}
				return *this;
			}

			/// Operator|=
			template <simd_align_t _RSIMD>
			pf_decl_constexpr col_vector<_Ty, _Num, _SIMD> &operator|=(
					col_vector<_Ty, _Num, _RSIMD> const &__r) pf_attr_noexcept requires(std::is_integral_v<_Ty>)
			{
				for (size_t i = 0; i < _Num; ++i)
				{
					this->store_[i] |= __r[i];
				}
				return *this;
			}

			/// Operator^=
			template <simd_align_t _RSIMD>
			pf_decl_constexpr col_vector<_Ty, _Num, _SIMD> &operator^=(
					col_vector<_Ty, _Num, _RSIMD> const &__r) pf_attr_noexcept requires(std::is_integral_v<_Ty>)
			{
				for (size_t i = 0; i < _Num; ++i)
				{
					this->store_[i] ^= __r[i];
				}
				return *this;
			}

			/// Operator<<=
			template <simd_align_t _RSIMD>
			pf_decl_constexpr col_vector<_Ty, _Num, _SIMD> &operator<<=(
					col_vector<_Ty, _Num, _RSIMD> const &__r) pf_attr_noexcept requires(std::is_integral_v<_Ty>)
			{
				for (size_t i = 0; i < _Num; ++i)
				{
					this->store_[i] <<= __r[i];
				}
				return *this;
			}

			/// Operator>>=
			template <simd_align_t _RSIMD>
			pf_decl_constexpr col_vector<_Ty, _Num, _SIMD> &operator>>=(
					col_vector<_Ty, _Num, _RSIMD> const &__r) pf_attr_noexcept requires(std::is_integral_v<_Ty>)
			{
				for (size_t i = 0; i < _Num; ++i)
				{
					this->store_[i] >>= __r[i];
				}
				return *this;
			}

			/// Operator==
			template <simd_align_t _RSIMD>
			pf_hint_nodiscard pf_decl_constexpr bool operator==(
					col_vector<_Ty, _Num, _RSIMD> const &__r) const pf_attr_noexcept
			{
				for (size_t i = 0; i < _Num; ++i)
				{
					if (this->store_[i] != __r[i])
						return false;
				}
				return true;
			}

			/// Compare
			template <simd_align_t _RSIMD>
			pf_hint_nodiscard pf_decl_constexpr col_vector<bool, _Num, simd_select_v<bool, _Num>> vals_equal(
					col_vector<_Ty, _Num, _RSIMD> const &__r) const pf_attr_noexcept
			{
				col_vector<bool, _Num, simd_select_v<bool, _Num>> tmp;
				for (size_t i = 0; i < _Num; ++i)
				{
					tmp[i] = this->store_[i] == __r[i];
				}
				return tmp;
			}
			template <simd_align_t _RSIMD>
			pf_hint_nodiscard pf_decl_constexpr col_vector<bool, _Num, simd_select_v<bool, _Num>> vals_greater(
					col_vector<_Ty, _Num, _RSIMD> const &__r) const pf_attr_noexcept
			{
				col_vector<bool, _Num, simd_select_v<bool, _Num>> tmp;
				for (size_t i = 0; i < _Num; ++i)
				{
					tmp[i] = this->store_[i] > __r[i];
				}
				return tmp;
			}
			template <simd_align_t _RSIMD>
			pf_hint_nodiscard pf_decl_constexpr col_vector<bool, _Num, simd_select_v<bool, _Num>> vals_greater_equal(
					col_vector<_Ty, _Num, _RSIMD> const &__r) const pf_attr_noexcept
			{
				col_vector<bool, _Num, simd_select_v<bool, _Num>> tmp;
				for (size_t i = 0; i < _Num; ++i)
				{
					tmp[i] = this->store_[i] >= __r[i];
				}
				return tmp;
			}
			template <simd_align_t _RSIMD>
			pf_hint_nodiscard pf_decl_constexpr col_vector<bool, _Num, simd_select_v<bool, _Num>> vals_smaller(
					col_vector<_Ty, _Num, _RSIMD> const &__r) const pf_attr_noexcept
			{
				col_vector<bool, _Num, simd_select_v<bool, _Num>> tmp;
				for (size_t i = 0; i < _Num; ++i)
				{
					tmp[i] = this->store_[i] < __r[i];
				}
				return tmp;
			}
			template <simd_align_t _RSIMD>
			pf_hint_nodiscard pf_decl_constexpr col_vector<bool, _Num, simd_select_v<bool, _Num>> vals_smaller_equal(
					col_vector<_Ty, _Num, _RSIMD> const &__r) const pf_attr_noexcept
			{
				col_vector<bool, _Num, simd_select_v<bool, _Num>> tmp;
				for (size_t i = 0; i < _Num; ++i)
				{
					tmp[i] = this->store_[i] <= __r[i];
				}
				return tmp;
			}
			template <simd_align_t _RSIMD>
			pf_hint_nodiscard pf_decl_constexpr col_vector<bool, _Num, simd_select_v<bool, _Num>> vals_not_equal(
					col_vector<_Ty, _Num, _RSIMD> const &__r) const pf_attr_noexcept
			{
				col_vector<bool, _Num, simd_select_v<bool, _Num>> tmp;
				for (size_t i = 0; i < _Num; ++i)
				{
					tmp[i] = this->store_[i] != __r[i];
				}
				return tmp;
			}

			/// Data
			pf_hint_nodiscard pf_decl_constexpr _Ty *data() pf_attr_noexcept
			{
				return this->store_.data();
			}
			pf_hint_nodiscard pf_decl_constexpr const _Ty *data() const pf_attr_noexcept
			{
				return this->store_.data();
			}

			/// Num of elem
			pf_hint_nodiscard pf_decl_static pf_decl_constexpr size_t num_elem() pf_attr_noexcept
			{
				return _Num;
			}

			/// Is SIMD aligned?
			pf_hint_nodiscard pf_decl_static pf_decl_constexpr bool is_simd_aligned() pf_attr_noexcept
			{
				return _SIMD == SIMD_ALIGNED;
			}

		private:
			/// Array
			std::array<_Ty, _Num> store_;
		};

		/// MATH: Col Vector -> Operator+
		template <typename _Ty, size_t _Num, simd_align_t _LSIMD, simd_align_t _RSIMD>
		pf_hint_nodiscard pf_decl_constexpr col_vector<_Ty, _Num, simd_select_v<_Ty, _Num>> operator+(
				col_vector<_Ty, _Num, _LSIMD> const &__l,
				col_vector<_Ty, _Num, _RSIMD> const &__r) pf_attr_noexcept
		{
			col_vector<_Ty, _Num, simd_select_v<_Ty, _Num>> tmp(__l);
			return tmp += __r;
		}

		/// MATH: Col Vector -> Operator-
		template <typename _Ty, size_t _Num, simd_align_t _LSIMD, simd_align_t _RSIMD>
		pf_hint_nodiscard pf_decl_constexpr col_vector<_Ty, _Num, simd_select_v<_Ty, _Num>> operator-(
				col_vector<_Ty, _Num, _LSIMD> const &__l,
				col_vector<_Ty, _Num, _RSIMD> const &__r) pf_attr_noexcept
		{
			col_vector<_Ty, _Num, simd_select_v<_Ty, _Num>> tmp(__l);
			return tmp -= __r;
		}

		/// MATH: Col Vector -> Operator*
		template <typename _Ty, size_t _Num, simd_align_t _SIMD>
		pf_hint_nodiscard pf_decl_constexpr col_vector<_Ty, _Num, _SIMD> operator*(
				col_vector<_Ty, _Num, _SIMD> const &__l,
				_Ty __val) pf_attr_noexcept
		{
			col_vector<_Ty, _Num, _SIMD> tmp(__l);
			return tmp *= __val;
		}
		template <typename _Ty, size_t _Num, simd_align_t _SIMD>
		pf_hint_nodiscard pf_decl_constexpr col_vector<_Ty, _Num, _SIMD> operator*(
				_Ty __val,
				col_vector<_Ty, _Num, _SIMD> const &__l) pf_attr_noexcept
		{
			col_vector<_Ty, _Num, _SIMD> tmp(__l);
			return tmp *= __val;
		}

		/// MATH: Col Vector -> Operator/
		template <typename _Ty, size_t _Num, simd_align_t _SIMD>
		pf_hint_nodiscard pf_decl_constexpr col_vector<_Ty, _Num, _SIMD> operator/(
				col_vector<_Ty, _Num, _SIMD> const &__l,
				_Ty __val) pf_attr_noexcept
		{
			col_vector<_Ty, _Num, _SIMD> tmp(__l);
			return tmp /= __val;
		}
		template <typename _Ty, size_t _Num, simd_align_t _SIMD>
		pf_hint_nodiscard pf_decl_constexpr col_vector<_Ty, _Num, _SIMD> operator/(
				_Ty __val,
				col_vector<_Ty, _Num, _SIMD> const &__l) pf_attr_noexcept
		{
			col_vector<_Ty, _Num, _SIMD> tmp(__l);
			return tmp /= __val;
		}

		/// MATH: Col Vector -> Operator%
		template <typename _Ty, size_t _Num, simd_align_t _LSIMD, simd_align_t _RSIMD>
		pf_hint_nodiscard pf_decl_constexpr col_vector<_Ty, _Num, simd_select_v<_Ty, _Num>> operator%(
				col_vector<_Ty, _Num, _LSIMD> const &__l,
				col_vector<_Ty, _Num, _RSIMD> const &__r) pf_attr_noexcept requires(std::is_integral_v<_Ty>)
		{
			col_vector<_Ty, _Num, simd_select_v<_Ty, _Num>> tmp(__l);
			return tmp %= __r;
		}

		/// MATH: Col Vector -> Operator|
		template <typename _Ty, size_t _Num, simd_align_t _LSIMD, simd_align_t _RSIMD>
		pf_hint_nodiscard pf_decl_constexpr col_vector<_Ty, _Num, simd_select_v<_Ty, _Num>> operator|(
				col_vector<_Ty, _Num, _LSIMD> const &__l,
				col_vector<_Ty, _Num, _RSIMD> const &__r) pf_attr_noexcept requires(std::is_integral_v<_Ty>)
		{
			col_vector<_Ty, _Num, simd_select_v<_Ty, _Num>> tmp(__l);
			return tmp |= __r;
		}

		/// MATH: Col Vector -> Operator&
		template <typename _Ty, size_t _Num, simd_align_t _LSIMD, simd_align_t _RSIMD>
		pf_hint_nodiscard pf_decl_constexpr col_vector<_Ty, _Num, simd_select_v<_Ty, _Num>> operator&(
				col_vector<_Ty, _Num, _LSIMD> const &__l,
				col_vector<_Ty, _Num, _RSIMD> const &__r) pf_attr_noexcept requires(std::is_integral_v<_Ty>)
		{
			col_vector<_Ty, _Num, simd_select_v<_Ty, _Num>> tmp(__l);
			return tmp &= __r;
		}

		/// MATH: Col Vector -> Operator^
		template <typename _Ty, size_t _Num, simd_align_t _LSIMD, simd_align_t _RSIMD>
		pf_hint_nodiscard pf_decl_constexpr col_vector<_Ty, _Num, simd_select_v<_Ty, _Num>> operator^(
				col_vector<_Ty, _Num, _LSIMD> const &__l,
				col_vector<_Ty, _Num, _RSIMD> const &__r) pf_attr_noexcept requires(std::is_integral_v<_Ty>)
		{
			col_vector<_Ty, _Num, simd_select_v<_Ty, _Num>> tmp(__l);
			return tmp ^= __r;
		}

		/// MATH: Col Vector -> Operator>>
		template <typename _Ty, size_t _Num, simd_align_t _LSIMD, simd_align_t _RSIMD>
		pf_hint_nodiscard pf_decl_constexpr col_vector<_Ty, _Num, simd_select_v<_Ty, _Num>> operator>>(
				col_vector<_Ty, _Num, _LSIMD> const &__l,
				col_vector<_Ty, _Num, _RSIMD> const &__r) pf_attr_noexcept requires(std::is_integral_v<_Ty>)
		{
			col_vector<_Ty, _Num, simd_select_v<_Ty, _Num>> tmp(__l);
			return tmp >>= __r;
		}

		/// MATH: Col Vector -> Operator<<
		template <typename _Ty, size_t _Num, simd_align_t _LSIMD, simd_align_t _RSIMD>
		pf_hint_nodiscard pf_decl_constexpr col_vector<_Ty, _Num, simd_select_v<_Ty, _Num>> operator<<(
				col_vector<_Ty, _Num, _LSIMD> const &__l,
				col_vector<_Ty, _Num, _RSIMD> const &__r) pf_attr_noexcept requires(std::is_integral_v<_Ty>)
		{
			col_vector<_Ty, _Num, simd_select_v<_Ty, _Num>> tmp(__l);
			return tmp <<= __r;
		}

		/// MATH: Col Vector -> Operator~
		template <typename _Ty, size_t _Num, simd_align_t _SIMD>
		pf_hint_nodiscard pf_decl_constexpr col_vector<_Ty, _Num, _SIMD> operator~(
				col_vector<_Ty, _Num, _SIMD> const &__r) pf_attr_noexcept
		{
			col_vector<_Ty, _Num, _SIMD> tmp;
			for (size_t i = 0; i < _Num; ++i)
			{
				tmp[i] = ~__r[i];
			}
			return tmp;
		}

		/// MATH: Col Vector -> Compare
		template <typename _Ty, size_t _Num, simd_align_t _LSIMD, simd_align_t _RSIMD>
		pf_hint_nodiscard pf_decl_constexpr col_vector<bool, _Num, simd_select_v<bool, _Num>> vals_equal(
				col_vector<_Ty, _Num, _LSIMD> const &__l,
				col_vector<_Ty, _Num, _RSIMD> const &__r) pf_attr_noexcept
		{
			return __l.vals_equal(__r);
		}
		template <typename _Ty, size_t _Num, simd_align_t _LSIMD, simd_align_t _RSIMD>
		pf_hint_nodiscard pf_decl_constexpr col_vector<bool, _Num, simd_select_v<bool, _Num>> vals_greater(
				col_vector<_Ty, _Num, _LSIMD> const &__l,
				col_vector<_Ty, _Num, _RSIMD> const &__r) pf_attr_noexcept
		{
			return __l.vals_greater(__r);
		}
		template <typename _Ty, size_t _Num, simd_align_t _LSIMD, simd_align_t _RSIMD>
		pf_hint_nodiscard pf_decl_constexpr col_vector<bool, _Num, simd_select_v<bool, _Num>> vals_greater_equal(
				col_vector<_Ty, _Num, _LSIMD> const &__l,
				col_vector<_Ty, _Num, _RSIMD> const &__r) pf_attr_noexcept
		{
			return __l.vals_greater_equal(__r);
		}
		template <typename _Ty, size_t _Num, simd_align_t _LSIMD, simd_align_t _RSIMD>
		pf_hint_nodiscard pf_decl_constexpr col_vector<bool, _Num, simd_select_v<bool, _Num>> vals_smaller(
				col_vector<_Ty, _Num, _LSIMD> const &__l,
				col_vector<_Ty, _Num, _RSIMD> const &__r) pf_attr_noexcept
		{
			return __l.vals_smaller(__r);
		}
		template <typename _Ty, size_t _Num, simd_align_t _LSIMD, simd_align_t _RSIMD>
		pf_hint_nodiscard pf_decl_constexpr col_vector<bool, _Num, simd_select_v<bool, _Num>> vals_smaller_equal(
				col_vector<_Ty, _Num, _LSIMD> const &__l,
				col_vector<_Ty, _Num, _RSIMD> const &__r) pf_attr_noexcept
		{
			return __l.vals_smaller_equal(__r);
		}
		template <typename _Ty, size_t _Num, simd_align_t _LSIMD, simd_align_t _RSIMD>
		pf_hint_nodiscard pf_decl_constexpr col_vector<bool, _Num, simd_select_v<bool, _Num>> vals_not_equal(
				col_vector<_Ty, _Num, _LSIMD> const &__l,
				col_vector<_Ty, _Num, _RSIMD> const &__r) pf_attr_noexcept
		{
			return __l.vals_not_equal(__r);
		}

		/// MATH: Col Vector -> Alias
		template <typename _Ty, size_t _Num, simd_align_t _SIMD = simd_select_v<_Ty, _Num>>
		using cvec = col_vector<_Ty, _Num, _SIMD>;

		/// MATH: Col Vector -> CTAD
		template <typename _Ty, typename... _InArgs>
			requires(
					(is_col_vector_v<_InArgs> && ...)
					&& (have_same_type_v<_Ty, _InArgs> && ...))
		col_vector(
				_Ty const &,
				_InArgs const &...)
		->col_vector<
				arithmetic_type_of_t<_Ty>,
				1 + (num_of_col_elem_v<_InArgs> + ...),
				simd_select_v<_Ty, 1 + (num_of_col_elem_v<_InArgs> + ...)>>;
		template <simd_align_t _SIMD, typename _Ty, typename... _InArgs>
			requires(
					(is_col_vector_v<_InArgs> && ...)
					&& (have_same_type_v<_Ty, _InArgs> && ...))
		col_vector(
				_Ty const &,
				_InArgs const &...)
		->col_vector<
				arithmetic_type_of_t<_Ty>,
				1 + (num_of_col_elem_v<_InArgs> + ...),
				_SIMD>;
	}
}

// Pulsar: Math -> Col Vector Specializations
#include "pulsar/math/vector/col_vector/col_vector2.inl"
#include "pulsar/math/vector/col_vector/col_vector3.inl"
#include "pulsar/math/vector/col_vector/col_vector4.inl"

#endif // !PULSAR_MATH_COL_VECTOR_HPP