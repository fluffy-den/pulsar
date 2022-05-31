/*! @file   col_vector2.inl
 *  @author Fluffy (noe.louis-quentin@hotmail.fr)
 *  @brief	Specialization for column vectors of size 2.
 *  @date   24-05-2022
 *
 *  @copyright Copyright (c) 2022 - Pulsar Software
 *
 *  @since 0.1.1
 */

#ifndef PULSAR_MATH_COL_VECTOR2_INL
#define PULSAR_MATH_COL_VECTOR2_INL 1

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
					&& ((is_simd_alignable_v<_Ty, _Num> && _SIMD == SIMD_ALIGNED) || (_SIMD == SIMD_UNALIGNED)))
		union col_vector;

		/// MATH: Col Vector 2
		template <typename _Ty, simd_align_t _SIMD>
			requires(
					std::is_arithmetic_v<_Ty> && ((is_simd_alignable_v<_Ty, 2> && _SIMD == SIMD_ALIGNED) || (_SIMD == SIMD_UNALIGNED)))
		union alignas(simd_alignment_of_v<_Ty, 2, _SIMD>) col_vector<_Ty, 2, _SIMD> pf_attr_final
		{
			template <typename _TyF, size_t _NumF, simd_align_t _SIMDF>
			pf_decl_friend union col_vector;

			using type = _Ty;

			/// Constructors
			pf_decl_constexpr col_vector(
					_Ty __val = static_cast<_Ty>(0)) pf_attr_noexcept
					: store_{ __val }
			{}
			pf_decl_constexpr col_vector(
					_Ty __val1,
					_Ty __val2) pf_attr_noexcept
					: store_{ __val1, __val2 }
			{}
			template <simd_align_t _RSIMD>
			pf_decl_constexpr col_vector(
					col_vector<_Ty, 2, _RSIMD> const &__r) pf_attr_noexcept
					: store_(__r.store_)
			{}

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
			pf_decl_constexpr col_vector<_Ty, 2, _SIMD> &operator+=(
					col_vector<_Ty, 2, _RSIMD> const &__r) pf_attr_noexcept
			{
				this->x += __r.x;
				this->y += __r.y;
				return *this;
			}

			/// Operator-=
			template <simd_align_t _RSIMD>
			pf_decl_constexpr col_vector<_Ty, 2, _SIMD> &operator-=(
					col_vector<_Ty, 2, _RSIMD> const &__r) pf_attr_noexcept
			{
				this->x -= __r.x;
				this->y -= __r.y;
				return *this;
			}

			/// Operator*=
			pf_decl_constexpr col_vector<_Ty, 2, _SIMD> &operator*=(
					_Ty __val) pf_attr_noexcept
			{
				this->x *= __val;
				this->y *= __val;
				return *this;
			}

			/// Operator/=
			pf_decl_constexpr col_vector<_Ty, 2, _SIMD> &operator/=(
					_Ty __val) pf_attr_noexcept
			{
				this->x /= __val;
				this->y /= __val;
				return *this;
			}

			/// Operator%=
			template <simd_align_t _RSIMD>
			pf_decl_constexpr col_vector<_Ty, 2, _SIMD> &operator%=(
					col_vector<_Ty, 2, _RSIMD> const &__r) pf_attr_noexcept requires(std::is_integral_v<_Ty>)
			{
				this->x %= __r.x;
				this->y %= __r.y;
				return *this;
			}

			/// Operator&=
			template <simd_align_t _RSIMD>
			pf_decl_constexpr col_vector<_Ty, 2, _SIMD> &operator&=(
					col_vector<_Ty, 2, _RSIMD> const &__r) pf_attr_noexcept requires(std::is_integral_v<_Ty>)
			{
				this->x &= __r.x;
				this->y &= __r.y;
				return *this;
			}

			/// Operator|=
			template <simd_align_t _RSIMD>
			pf_decl_constexpr col_vector<_Ty, 2, _SIMD> &operator|=(
					col_vector<_Ty, 2, _RSIMD> const &__r) pf_attr_noexcept requires(std::is_integral_v<_Ty>)
			{
				this->x |= __r.x;
				this->y |= __r.y;
				return *this;
			}

			/// Operator^=
			template <simd_align_t _RSIMD>
			pf_decl_constexpr col_vector<_Ty, 2, _SIMD> &operator^=(
					col_vector<_Ty, 2, _RSIMD> const &__r) pf_attr_noexcept requires(std::is_integral_v<_Ty>)
			{
				this->x ^= __r.x;
				this->y ^= __r.y;
				return *this;
			}

			/// Operator<<=
			template <simd_align_t _RSIMD>
			pf_decl_constexpr col_vector<_Ty, 2, _SIMD> &operator<<=(
					col_vector<_Ty, 2, _RSIMD> const &__r) pf_attr_noexcept requires(std::is_integral_v<_Ty>)
			{
				this->x <<= __r.x;
				this->y <<= __r.y;
				return *this;
			}

			/// Operator>>=
			template <simd_align_t _RSIMD>
			pf_decl_constexpr col_vector<_Ty, 2, _SIMD> &operator>>=(
					col_vector<_Ty, 2, _RSIMD> const &__r) pf_attr_noexcept requires(std::is_integral_v<_Ty>)
			{
				this->x >>= __r.x;
				this->y >>= __r.y;
				return *this;
			}

			/// Operator==
			template <simd_align_t _RSIMD>
			pf_hint_nodiscard pf_decl_constexpr bool operator==(
					col_vector<_Ty, 2, _RSIMD> const &__r) pf_attr_noexcept
			{
				return this->x == __r.x
						&& this->y == __r.y;
			}

			/// Compare
			template <simd_align_t _RSIMD>
			pf_hint_nodiscard pf_decl_constexpr col_vector<bool, 2> vals_equal(
					col_vector<_Ty, 2, _RSIMD> const &__r) const pf_attr_noexcept
			{
				return col_vector<bool, 2, simd_select_v<bool, 2>>(
						this->x == __r.x,
						this->y == __r.y);
			}
			template <simd_align_t _RSIMD>
			pf_hint_nodiscard pf_decl_constexpr col_vector<bool, 2, simd_select_v<bool, 2>> vals_greater(
					col_vector<_Ty, 2, _RSIMD> const &__r) const pf_attr_noexcept
			{
				return col_vector<bool, 2, simd_select_v<bool, 2>>(
						this->x > __r.x,
						this->y > __r.y);
			}
			template <simd_align_t _RSIMD>
			pf_hint_nodiscard pf_decl_constexpr col_vector<bool, 2, simd_select_v<bool, 2>> vals_greater_equal(
					col_vector<_Ty, 2, _RSIMD> const &__r) const pf_attr_noexcept
			{
				return col_vector<bool, 2, simd_select_v<bool, 2>>(
						this->x >= __r.x,
						this->y >= __r.y);
			}
			template <simd_align_t _RSIMD>
			pf_hint_nodiscard pf_decl_constexpr col_vector<bool, 2, simd_select_v<bool, 2>> vals_smaller(
					col_vector<_Ty, 2, _RSIMD> const &__r) const pf_attr_noexcept
			{
				return col_vector<bool, 2, simd_select_v<bool, 2>>(
						this->x < __r.x,
						this->y < __r.y);
			}
			template <simd_align_t _RSIMD>
			pf_hint_nodiscard pf_decl_constexpr col_vector<bool, 2, simd_select_v<bool, 2>> vals_smaller_equal(
					col_vector<_Ty, 2, _RSIMD> const &__r) const pf_attr_noexcept
			{
				return col_vector<bool, 2, simd_select_v<bool, 2>>(
						this->x <= __r.x,
						this->y <= __r.y);
			}
			template <simd_align_t _RSIMD>
			pf_hint_nodiscard pf_decl_constexpr col_vector<bool, 2, simd_select_v<bool, 2>> vals_not_equal(
					col_vector<_Ty, 2, _RSIMD> const &__r) const pf_attr_noexcept
			{
				return col_vector<bool, 2, simd_select_v<bool, 2>>(
						this->x != __r.x,
						this->y != __r.y);
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
				return 2;
			}

			/// Is SIMD aligned?
			pf_hint_nodiscard pf_decl_static pf_decl_constexpr bool is_simd_aligned() pf_attr_noexcept
			{
				return _SIMD == SIMD_ALIGNED;
			}

			struct
			{
				_Ty x;
				_Ty y;
			};
			struct
			{
				_Ty r;
				_Ty g;
			};

		private:
			std::array<_Ty, 2> store_;
		};

		/// MATH: bcvecu_t
		using bcvec2u_t = col_vector<bool, 2, SIMD_UNALIGNED>;

		/// MATH: ucvec_t
		using u32cvec2_t = col_vector<uint32_t, 2, SIMD_ALIGNED>;
		using u64cvec2_t = col_vector<uint64_t, 2, SIMD_ALIGNED>;

		/// MATH: ucvec2u_t
		using u8cvec2u_t	= col_vector<uint8_t, 2, SIMD_UNALIGNED>;
		using u16cvec2u_t = col_vector<uint16_t, 2, SIMD_UNALIGNED>;
		using u32cvec2u_t = col_vector<uint32_t, 2, SIMD_UNALIGNED>;
		using u64cvec2u_t = col_vector<uint64_t, 2, SIMD_UNALIGNED>;

		/// MATH: icvec_t
		using i32cvec2_t = col_vector<int32_t, 2, SIMD_ALIGNED>;
		using i64cvec2_t = col_vector<int64_t, 2, SIMD_ALIGNED>;

		/// MATH: icvecu_t
		using i8cvec2u_t	= col_vector<int8_t, 2, SIMD_UNALIGNED>;
		using i16cvec2u_t = col_vector<int16_t, 2, SIMD_UNALIGNED>;
		using i32cvec2u_t = col_vector<int32_t, 2, SIMD_UNALIGNED>;
		using i64cvec2u_t = col_vector<int64_t, 2, SIMD_UNALIGNED>;

		/// MATH: fcvec_t
		using f32cvec2_t = col_vector<float32_t, 2, SIMD_ALIGNED>;
		using f64cvec2_t = col_vector<float64_t, 2, SIMD_ALIGNED>;

		/// MATH: fcvecu_t
		using f32cvec2u_t = col_vector<float32_t, 2, SIMD_UNALIGNED>;
		using f64cvec2u_t = col_vector<float64_t, 2, SIMD_UNALIGNED>;
	}
}

#endif // !PULSAR_MATH_COL_VECTOR2_INL