/*! @file   row_vector4.inl
 *  @author Fluffy (noe.louis-quentin@hotmail.fr)
 *  @brief	Specialization for line vectors of size 4.
 *  @date   24-05-2022
 *
 *  @copyright Copyright (c) 2022 - Pulsar Software
 *
 *  @since 0.1.1
 */

#ifndef PULSAR_MATH_ROW_VECTOR4_INL
#define PULSAR_MATH_ROW_VECTOR4_INL 1

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
		/// MATH: Row Vector -> Definition
		template <typename _Ty, size_t _Num, simd_align_t _SIMD>
			requires(
					std::is_arithmetic_v<_Ty> &&_Num > 1
					&& (((is_simd_alignable_v<_Ty, _Num>)&&_SIMD == SIMD_ALIGNED) || (_SIMD == SIMD_UNALIGNED)))
		union row_vector;

		/// MATH: Row Vector 4
		template <typename _Ty, simd_align_t _SIMD>
			requires(
					std::is_arithmetic_v<_Ty> && ((is_simd_alignable_v<_Ty, 4> && _SIMD == SIMD_ALIGNED) || (_SIMD == SIMD_UNALIGNED)))
		union alignas(simd_alignment_of_v<_Ty, 4, _SIMD>) row_vector<_Ty, 4, _SIMD> pf_attr_final
		{
			template <typename _TyF, size_t _NumF, simd_align_t _SIMDF>
			pf_decl_friend union row_vector;

			using type = _Ty;

			/// Constructors
			pf_decl_constexpr row_vector(
					_Ty __val = static_cast<_Ty>(0)) pf_attr_noexcept
					: store_{ __val }
			{}
			pf_decl_constexpr row_vector(
					_Ty __val1,
					_Ty __val2,
					_Ty __val3,
					_Ty __val4) pf_attr_noexcept
					: store_{ __val1, __val2, __val3, __val4 }
			{}
			template <simd_align_t _SIMD1>
			pf_decl_constexpr row_vector(
					row_vector<_Ty, 3, _SIMD1> const &__v1,
					_Ty __val4) pf_attr_noexcept
					: store_{ __v1.x, __v1.y, __v1.z, __val4 }
			{}
			template <simd_align_t _SIMD1>
			pf_decl_constexpr row_vector(
					_Ty __val1,
					row_vector<_Ty, 3, _SIMD1> const &__v2) pf_attr_noexcept
					: store_{ __val1, __v2.x, __v2.y, __v2.z }
			{}
			template <simd_align_t _SIMD1>
			pf_decl_constexpr row_vector(
					_Ty __val1,
					row_vector<_Ty, 2, _SIMD1> const &__v2,
					_Ty __val4) pf_attr_noexcept
					: store_{ __val1, __v2.x, __v2.y, __val4 }
			{}
			template <simd_align_t _SIMD1, simd_align_t _SIMD2>
			pf_decl_constexpr row_vector(
					row_vector<_Ty, 2, _SIMD1> const &__v1,
					row_vector<_Ty, 2, _SIMD2> const &__v2) pf_attr_noexcept
					: store_{ __v1.x, __v1.y, __v2.x, __v2.y }
			{}
			template <simd_align_t _RSIMD>
			pf_decl_constexpr row_vector(
					row_vector<_Ty, 4, _RSIMD> const &__r) pf_attr_noexcept
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
			pf_hint_nodiscard pf_decl_constexpr row_vector<_Ty, 4, _SIMD> &operator+=(
					row_vector<_Ty, 4, _RSIMD> const &__r) pf_attr_noexcept
			{
				this->x += __r.x;
				this->y += __r.y;
				this->z += __r.z;
				this->w += __r.w;
				return *this;
			}

			/// Operator-=
			template <simd_align_t _RSIMD>
			pf_decl_constexpr row_vector<_Ty, 4, _SIMD> &operator-=(
					row_vector<_Ty, 4, _RSIMD> const &__r) pf_attr_noexcept
			{
				this->x -= __r.x;
				this->y -= __r.y;
				this->z -= __r.z;
				this->w -= __r.w;
				return *this;
			}

			/// Operator*=
			pf_decl_constexpr row_vector<_Ty, 4, _SIMD> &operator*=(
					_Ty __val) pf_attr_noexcept
			{
				this->x *= __val;
				this->y *= __val;
				this->z *= __val;
				this->w *= __val;
				return *this;
			}

			/// Operator/=
			pf_decl_constexpr row_vector<_Ty, 4, _SIMD> &operator/=(
					_Ty __val) pf_attr_noexcept
			{
				this->x /= __val;
				this->y /= __val;
				this->z /= __val;
				this->w /= __val;
				return *this;
			}

			/// Operator%=
			template <simd_align_t _RSIMD>
			pf_decl_constexpr row_vector<_Ty, 4, _SIMD> &operator%=(
					row_vector<_Ty, 4, _RSIMD> const &__r) pf_attr_noexcept requires(std::is_integral_v<_Ty>)
			{
				this->x %= __r.x;
				this->y %= __r.y;
				this->z %= __r.z;
				this->w %= __r.w;
				return *this;
			}

			/// Operator&=
			template <simd_align_t _RSIMD>
			pf_decl_constexpr row_vector<_Ty, 4, _SIMD> &operator&=(
					row_vector<_Ty, 4, _RSIMD> const &__r) pf_attr_noexcept requires(std::is_integral_v<_Ty>)
			{
				this->x &= __r.x;
				this->y &= __r.y;
				this->z &= __r.z;
				this->w &= __r.w;
				return *this;
			}

			/// Operator|=
			template <simd_align_t _RSIMD>
			pf_decl_constexpr row_vector<_Ty, 4, _SIMD> &operator|=(
					row_vector<_Ty, 4, _RSIMD> const &__r) pf_attr_noexcept requires(std::is_integral_v<_Ty>)
			{
				this->x |= __r.x;
				this->y |= __r.y;
				this->z |= __r.z;
				this->w |= __r.w;
				return *this;
			}

			/// Operator^=
			template <simd_align_t _RSIMD>
			pf_decl_constexpr row_vector<_Ty, 4, _SIMD> &operator^=(
					row_vector<_Ty, 4, _RSIMD> const &__r) pf_attr_noexcept requires(std::is_integral_v<_Ty>)
			{
				this->x ^= __r.x;
				this->y ^= __r.y;
				this->z ^= __r.z;
				this->w ^= __r.w;
				return *this;
			}

			/// Operator<<=
			template <simd_align_t _RSIMD>
			pf_decl_constexpr row_vector<_Ty, 4, _SIMD> &operator<<=(
					row_vector<_Ty, 4, _RSIMD> const &__r) pf_attr_noexcept requires(std::is_integral_v<_Ty>)
			{
				this->x <<= __r.x;
				this->y <<= __r.y;
				this->z <<= __r.z;
				this->w <<= __r.w;
				return *this;
			}

			/// Operator>>=
			template <simd_align_t _RSIMD>
			pf_decl_constexpr row_vector<_Ty, 4, _SIMD> &operator>>=(
					row_vector<_Ty, 4, _RSIMD> const &__r) pf_attr_noexcept requires(std::is_integral_v<_Ty>)
			{
				this->x >>= __r.x;
				this->y >>= __r.y;
				this->z >>= __r.z;
				this->w >>= __r.w;
				return *this;
			}

			/// Operator==
			template <simd_align_t _RSIMD>
			pf_hint_nodiscard pf_decl_constexpr bool operator==(
					row_vector<_Ty, 4, _RSIMD> const &__r) pf_attr_noexcept
			{
				return this->x == __r.x
						&& this->y == __r.y
						&& this->z == __r.z
						&& this->w == __r.w;
			}

			/// Compare
			template <simd_align_t _RSIMD>
			pf_hint_nodiscard pf_decl_constexpr row_vector<bool, 4> vals_equal(
					row_vector<_Ty, 4, _RSIMD> const &__r) const pf_attr_noexcept
			{
				return row_vector<bool, 4, simd_select_v<bool, 4>>(
						this->x == __r.x,
						this->y == __r.y,
						this->z == __r.z,
						this->w == __r.w);
			}
			template <simd_align_t _RSIMD>
			pf_hint_nodiscard pf_decl_constexpr row_vector<bool, 4, simd_select_v<bool, 4>> vals_greater(
					row_vector<_Ty, 4, _RSIMD> const &__r) const pf_attr_noexcept
			{
				return row_vector<bool, 4, simd_select_v<bool, 4>>(
						this->x > __r.x,
						this->y > __r.y,
						this->z > __r.z,
						this->w > __r.w);
			}
			template <simd_align_t _RSIMD>
			pf_hint_nodiscard pf_decl_constexpr row_vector<bool, 4, simd_select_v<bool, 4>> vals_greater_equal(
					row_vector<_Ty, 4, _RSIMD> const &__r) const pf_attr_noexcept
			{
				return row_vector<bool, 4, simd_select_v<bool, 4>>(
						this->x >= __r.x,
						this->y >= __r.y,
						this->z >= __r.z,
						this->w >= __r.w);
			}
			template <simd_align_t _RSIMD>
			pf_hint_nodiscard pf_decl_constexpr row_vector<bool, 4, simd_select_v<bool, 4>> vals_smaller(
					row_vector<_Ty, 4, _RSIMD> const &__r) const pf_attr_noexcept
			{
				return row_vector<bool, 4, simd_select_v<bool, 4>>(
						this->x < __r.x,
						this->y < __r.y,
						this->z < __r.z,
						this->w < __r.w);
			}
			template <simd_align_t _RSIMD>
			pf_hint_nodiscard pf_decl_constexpr row_vector<bool, 4, simd_select_v<bool, 4>> vals_smaller_equal(
					row_vector<_Ty, 4, _RSIMD> const &__r) const pf_attr_noexcept
			{
				return row_vector<bool, 4, simd_select_v<bool, 4>>(
						this->x <= __r.x,
						this->y <= __r.y,
						this->z <= __r.z,
						this->w <= __r.w);
			}
			template <simd_align_t _RSIMD>
			pf_hint_nodiscard pf_decl_constexpr row_vector<bool, 4, simd_select_v<bool, 4>> vals_not_equal(
					row_vector<_Ty, 4, _RSIMD> const &__r) const pf_attr_noexcept
			{
				return row_vector<bool, 4, simd_select_v<bool, 4>>(
						this->x != __r.x,
						this->y != __r.y,
						this->z != __r.z,
						this->w != __r.w);
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
				return 4;
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
				_Ty z;
				_Ty w;
			};
			struct
			{
				_Ty r;
				_Ty g;
				_Ty b;
				_Ty a;
			};

		private:
			std::array<_Ty, 4> store_;
		};

		/// MATH: brvec4u_t
		using brvec4u_t = row_vector<bool, 4, SIMD_UNALIGNED>;

		/// MATH: urvec4_t
		using u16rvec4_t = row_vector<uint16_t, 4, SIMD_ALIGNED>;
		using u44rvec4_t = row_vector<uint32_t, 4, SIMD_ALIGNED>;
		using u64rvec4_t = row_vector<uint64_t, 4, SIMD_ALIGNED>;

		/// MATH: urvec4u_t
		using u8rvec4u_t	= row_vector<uint8_t, 4, SIMD_UNALIGNED>;
		using u16rvec4u_t = row_vector<uint16_t, 4, SIMD_UNALIGNED>;
		using u44rvec4u_t = row_vector<uint32_t, 4, SIMD_UNALIGNED>;
		using u64rvec4u_t = row_vector<uint64_t, 4, SIMD_UNALIGNED>;

		/// MATH: irvec4_t
		using i16rvec4_t = row_vector<int16_t, 4, SIMD_ALIGNED>;
		using i32rvec4_t = row_vector<int32_t, 4, SIMD_ALIGNED>;
		using i64rvec4_t = row_vector<int64_t, 4, SIMD_ALIGNED>;

		/// MATH: irvec4u_t
		using i8rvec4u_t	= row_vector<int8_t, 4, SIMD_UNALIGNED>;
		using i16rvec4u_t = row_vector<int16_t, 4, SIMD_UNALIGNED>;
		using i32rvec4u_t = row_vector<int32_t, 4, SIMD_UNALIGNED>;
		using i64rvec4u_t = row_vector<int64_t, 4, SIMD_UNALIGNED>;

		/// MATH: frvec4_t
		using f32rvec4_t = row_vector<float32_t, 4, SIMD_ALIGNED>;
		using f64rvec4_t = row_vector<float64_t, 4, SIMD_ALIGNED>;

		/// MATH: frvec4u_t
		using f32rvec4u_t = row_vector<float32_t, 4, SIMD_UNALIGNED>;
		using f64rvec4u_t = row_vector<float64_t, 4, SIMD_UNALIGNED>;
	}
}

#endif // !PULSAR_MATH_ROW_VECTOR4_INL