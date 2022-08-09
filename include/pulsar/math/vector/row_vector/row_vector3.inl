/*! @file   row_vector3.inl
 *  @author Fluffy (noe.louis-quentin@hotmail.fr)
 *  @brief	Specialization for line vectors of size 3.
 *  @date   24-05-2022
 *
 *  @copyright Copyright (c) 2022 - Pulsar Software
 *
 *  @since 0.1.1
 */

#ifndef PULSAR_MATH_ROW_VECTOR3_INL
#define PULSAR_MATH_ROW_VECTOR3_INL 1

// Include: Pulsar
#include "pulsar/intrin.hpp"

// Include: C++
#include <array>
#include <type_traits>

// Anonymous structs -> Disable warning
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"

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

		/// MATH: Row Vector 3
		template <typename _Ty, simd_align_t _SIMD>
			requires(
					std::is_arithmetic_v<_Ty> && ((is_simd_alignable_v<_Ty, 3> && _SIMD == SIMD_ALIGNED) || (_SIMD == SIMD_UNALIGNED)))
		union alignas(simd_alignment_of_v<_Ty, 3, _SIMD>) row_vector<_Ty, 3, _SIMD> pf_attr_final
		{
			template <typename _TyF, size_t _NumF, simd_align_t _SIMDF>
			pf_decl_friend union row_vector;

			using type																			 = _Ty;
			pf_decl_static pf_decl_constexpr size_t num			 = 3;
			pf_decl_static pf_decl_constexpr bool simd_value = _SIMD;

			/// Constructors
			pf_decl_constexpr row_vector(
					_Ty __val = static_cast<_Ty>(0)) pf_attr_noexcept
					: store_{ __val }
			{}
			pf_decl_constexpr row_vector(
					_Ty __val1,
					_Ty __val2,
					_Ty __val3) pf_attr_noexcept
					: store_{ __val1, __val2, __val3 }
			{}
			template <simd_align_t _SIMD1>
			pf_decl_constexpr row_vector(
					row_vector<_Ty, 2, _SIMD1> const &__v1,
					_Ty __val3) pf_attr_noexcept
					: store_{ __v1.x, __v1.y, __val3 }
			{}
			template <simd_align_t _SIMD1>
			pf_decl_constexpr row_vector(
					_Ty __val1,
					row_vector<_Ty, 2, _SIMD1> const &__v2) pf_attr_noexcept
					: store_{ __val1, __v2.x, __v2.y }
			{}
			template <simd_align_t _RSIMD>
			pf_decl_constexpr row_vector(
					row_vector<_Ty, 3, _RSIMD> const &__r) pf_attr_noexcept
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
			pf_decl_constexpr row_vector<_Ty, 3, _SIMD> &operator+=(
					row_vector<_Ty, 3, _RSIMD> const &__r) pf_attr_noexcept
			{
				this->x += __r.x;
				this->y += __r.y;
				this->z += __r.z;
				return *this;
			}

			/// Operator-=
			template <simd_align_t _RSIMD>
			pf_decl_constexpr row_vector<_Ty, 3, _SIMD> &operator-=(
					row_vector<_Ty, 3, _RSIMD> const &__r) pf_attr_noexcept
			{
				this->x -= __r.x;
				this->y -= __r.y;
				this->z -= __r.z;
				return *this;
			}

			/// Operator*=
			pf_decl_constexpr row_vector<_Ty, 3, _SIMD> &operator*=(
					_Ty __val) pf_attr_noexcept
			{
				this->x *= __val;
				this->y *= __val;
				this->z *= __val;
				return *this;
			}

			/// Operator/=
			pf_decl_constexpr row_vector<_Ty, 3, _SIMD> &operator/=(
					_Ty __val) pf_attr_noexcept
			{
				this->x /= __val;
				this->y /= __val;
				this->z /= __val;
				return *this;
			}

			/// Operator%=
			pf_decl_constexpr row_vector<_Ty, 3, _SIMD> &operator%=(
					_Ty __val) pf_attr_noexcept requires(std::is_integral_v<_Ty>)
			{
				this->x %= __val;
				this->y %= __val;
				this->z %= __val;
				return *this;
			}
			template <simd_align_t _RSIMD>
			pf_decl_constexpr row_vector<_Ty, 3, _SIMD> &operator%=(
					row_vector<_Ty, 3, _RSIMD> const &__r) pf_attr_noexcept requires(std::is_integral_v<_Ty>)
			{
				this->x %= __r.x;
				this->y %= __r.y;
				this->z %= __r.z;
				return *this;
			}

			/// Operator&=
			template <simd_align_t _RSIMD>
			pf_decl_constexpr row_vector<_Ty, 3, _SIMD> &operator&=(
					row_vector<_Ty, 3, _RSIMD> const &__r) pf_attr_noexcept requires(std::is_integral_v<_Ty>)
			{
				this->x &= __r.x;
				this->y &= __r.y;
				this->z &= __r.z;
				return *this;
			}

			/// Operator|=
			template <simd_align_t _RSIMD>
			pf_decl_constexpr row_vector<_Ty, 3, _SIMD> &operator|=(
					row_vector<_Ty, 3, _RSIMD> const &__r) pf_attr_noexcept requires(std::is_integral_v<_Ty>)
			{
				this->x |= __r.x;
				this->y |= __r.y;
				this->z |= __r.z;
				return *this;
			}

			/// Operator^=
			template <simd_align_t _RSIMD>
			pf_decl_constexpr row_vector<_Ty, 3, _SIMD> &operator^=(
					row_vector<_Ty, 3, _RSIMD> const &__r) pf_attr_noexcept requires(std::is_integral_v<_Ty>)
			{
				this->x ^= __r.x;
				this->y ^= __r.y;
				this->z ^= __r.z;
				return *this;
			}

			/// Operator<<=
			template <simd_align_t _RSIMD>
			pf_decl_constexpr row_vector<_Ty, 3, _SIMD> &operator<<=(
					row_vector<_Ty, 3, _RSIMD> const &__r) pf_attr_noexcept requires(std::is_integral_v<_Ty>)
			{
				this->x <<= __r.x;
				this->y <<= __r.y;
				this->z <<= __r.z;
				return *this;
			}

			/// Operator>>=
			template <simd_align_t _RSIMD>
			pf_decl_constexpr row_vector<_Ty, 3, _SIMD> &operator>>=(
					row_vector<_Ty, 3, _RSIMD> const &__r) pf_attr_noexcept requires(std::is_integral_v<_Ty>)
			{
				this->x >>= __r.x;
				this->y >>= __r.y;
				this->z >>= __r.z;
				return *this;
			}

			/// Operator==
			template <simd_align_t _RSIMD>
			pf_hint_nodiscard pf_decl_constexpr bool operator==(
					row_vector<_Ty, 3, _RSIMD> const &__r) pf_attr_noexcept
			{
				return this->x == __r.x
						&& this->y == __r.y
						&& this->z == __r.z;
			}

			/// Compare
			template <simd_align_t _RSIMD>
			pf_hint_nodiscard pf_decl_constexpr row_vector<bool, 3> vals_equal(
					row_vector<_Ty, 3, _RSIMD> const &__r) const pf_attr_noexcept
			{
				return row_vector<bool, 3, simd_select_v<bool, 3>>(
						this->x == __r.x,
						this->y == __r.y,
						this->z == __r.z);
			}
			template <simd_align_t _RSIMD>
			pf_hint_nodiscard pf_decl_constexpr row_vector<bool, 3, simd_select_v<bool, 3>> vals_greater(
					row_vector<_Ty, 3, _RSIMD> const &__r) const pf_attr_noexcept
			{
				return row_vector<bool, 3, simd_select_v<bool, 3>>(
						this->x > __r.x,
						this->y > __r.y,
						this->z > __r.z);
			}
			template <simd_align_t _RSIMD>
			pf_hint_nodiscard pf_decl_constexpr row_vector<bool, 3, simd_select_v<bool, 3>> vals_greater_equal(
					row_vector<_Ty, 3, _RSIMD> const &__r) const pf_attr_noexcept
			{
				return row_vector<bool, 3, simd_select_v<bool, 3>>(
						this->x >= __r.x,
						this->y >= __r.y,
						this->z >= __r.z);
			}
			template <simd_align_t _RSIMD>
			pf_hint_nodiscard pf_decl_constexpr row_vector<bool, 3, simd_select_v<bool, 3>> vals_smaller(
					row_vector<_Ty, 3, _RSIMD> const &__r) const pf_attr_noexcept
			{
				return row_vector<bool, 3, simd_select_v<bool, 3>>(
						this->x < __r.x,
						this->y < __r.y,
						this->z < __r.z);
			}
			template <simd_align_t _RSIMD>
			pf_hint_nodiscard pf_decl_constexpr row_vector<bool, 3, simd_select_v<bool, 3>> vals_smaller_equal(
					row_vector<_Ty, 3, _RSIMD> const &__r) const pf_attr_noexcept
			{
				return row_vector<bool, 3, simd_select_v<bool, 3>>(
						this->x <= __r.x,
						this->y <= __r.y,
						this->z <= __r.z);
			}
			template <simd_align_t _RSIMD>
			pf_hint_nodiscard pf_decl_constexpr row_vector<bool, 3, simd_select_v<bool, 3>> vals_not_equal(
					row_vector<_Ty, 3, _RSIMD> const &__r) const pf_attr_noexcept
			{
				return row_vector<bool, 3, simd_select_v<bool, 3>>(
						this->x != __r.x,
						this->y != __r.y,
						this->z != __r.z);
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
				return 3;
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
			};
			struct
			{
				_Ty r;
				_Ty g;
				_Ty b;
			};

		private:
			std::array<_Ty, 3> store_;
		};

		/// MATH: brvec3u_t
		using brvec3u_t = row_vector<bool, 3, SIMD_UNALIGNED>;

		/// MATH: urvec3u_t
		using u8rvec3u_t	= row_vector<uint8_t, 3, SIMD_UNALIGNED>;
		using u16rvec3u_t = row_vector<uint16_t, 3, SIMD_UNALIGNED>;
		using u32rvec3u_t = row_vector<uint32_t, 3, SIMD_UNALIGNED>;
		using u64rvec3u_t = row_vector<uint64_t, 3, SIMD_UNALIGNED>;

		/// MATH: irvec3u_t
		using i8rvec3u_t	= row_vector<int8_t, 3, SIMD_UNALIGNED>;
		using i16rvec3u_t = row_vector<int16_t, 3, SIMD_UNALIGNED>;
		using i32rvec3u_t = row_vector<int32_t, 3, SIMD_UNALIGNED>;
		using i64rvec3u_t = row_vector<int64_t, 3, SIMD_UNALIGNED>;

		/// MATH: frvec3u_t
		using f32rvec3u_t = row_vector<float32_t, 3, SIMD_UNALIGNED>;
		using f64rvec3u_t = row_vector<float64_t, 3, SIMD_UNALIGNED>;
	}
}

// Anonymous structs
#pragma GCC diagnostic pop

#endif // !PULSAR_MATH_ROW_VECTOR3_INL