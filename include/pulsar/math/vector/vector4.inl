/*! @file   vector4.inl
 *  @author Fluffy (noe.louis-quentin@hotmail.fr)
 *  @brief  Definition of vector with 4 components.
 *  @date   40-08-2022
 *
 *  @copyright Copyright (c) 2022 - Pulsar Software
 *
 *  @since 0.1.1
 */

#ifndef PULSAR_MATH_VECTOR4_INL
#define PULSAR_MATH_VECTOR4_INL 1

// Include: Pulsar
#include "pulsar/intrin.hpp"

// Anonymous structs -> Disable warning
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"

// Pulsar
namespace pul
{
	/// MATH: Vector
	template <typename _Ty, size_t _Num, simd_align_t _Simd>
		requires(
				std::is_arithmetic_v<_Ty> &&_Num > 1
				&& (((is_simd_alignable_v<_Ty, _Num>)&&_Simd == SIMD_ALIGNED) || (_Simd == SIMD_UNALIGNED)))
	union vector;

	/// MATH: Vector -> 4
	template <typename _Ty, simd_align_t _Simd>
		requires(
				std::is_arithmetic_v<_Ty> && (((is_simd_alignable_v<_Ty, 4>)&&_Simd == SIMD_ALIGNED) || (_Simd == SIMD_UNALIGNED)))
	union alignas(simd_alignment_of_v<_Ty, 4, _Simd>) vector<_Ty, 4, _Simd>
	{
		template <typename _TyF, size_t _NumF, simd_align_t _SimdF>
			requires(
					std::is_arithmetic_v<_TyF> &&_NumF > 1
					&& (((is_simd_alignable_v<_TyF, _NumF>)&&_SimdF == SIMD_ALIGNED) || (_SimdF == SIMD_UNALIGNED)))
		pf_decl_friend union vector;

	public:
		/// Constructors
		pf_decl_inline pf_decl_constexpr vector(
				_Ty __ral = static_cast<_Ty>(0)) pf_attr_noexcept
				: store_{ __ral }
		{}
		pf_decl_inline pf_decl_constexpr vector(
				_Ty __ral1,
				_Ty __ral2,
				_Ty __ral3,
				_Ty __ral4) pf_attr_noexcept
				: store_{ __ral1, __ral2, __ral3, __ral4 }
		{}
		template <simd_align_t _SimdR>
		pf_decl_inline pf_decl_constexpr vector(
				vector<_Ty, 3, _SimdR> const &__r1,
				_Ty __ral4) pf_attr_noexcept
				: store_{ __r1.x, __r1.y, __r1.z, __ral4 }
		{}
		template <simd_align_t _SimdR>
		pf_decl_inline pf_decl_constexpr vector(
				_Ty __ral1,
				vector<_Ty, 3, _SimdR> const &__r2) pf_attr_noexcept
				: store_{ __ral1, __r2.x, __r2.y, __r2.z }
		{}
		template <simd_align_t _SimdR1, simd_align_t _SimdR2>
		pf_decl_inline pf_decl_constexpr vector(
				vector<_Ty, 2, _SimdR1> const &__r1,
				vector<_Ty, 2, _SimdR2> const &__r2) pf_attr_noexcept
				: store_{ __r1.x, __r1.y, __r2.x, __r2.y }
		{}
		template <simd_align_t _SimdR>
		pf_decl_inline pf_decl_constexpr vector(
				vector<_Ty, 2, _SimdR> const &__r,
				_Ty __ral1,
				_Ty __ral2) pf_attr_noexcept
				: store_{ __r.x, __r.y, __ral1, __ral2 }
		{}
		template <simd_align_t _SimdR>
		pf_decl_inline pf_decl_constexpr vector(
				_Ty __ral1,
				vector<_Ty, 2, _SimdR> const &__r,
				_Ty __ral2) pf_attr_noexcept
				: store_{ __ral1, __r.x, __r.y, __ral2 }
		{}
		template <simd_align_t _SimdR>
		pf_decl_inline pf_decl_constexpr vector(
				_Ty __ral1,
				_Ty __ral2,
				vector<_Ty, 2, _SimdR> const &__r) pf_attr_noexcept
				: store_{ __ral1, __ral2, __r.x, __r.y }
		{}
		template <simd_align_t _SimdR>
		pf_decl_inline pf_decl_constexpr vector(
				vector<_Ty, 4, _SimdR> const &__r) pf_attr_noexcept
				: store_(__r.store_)
		{}

		/// Operator=
		pf_decl_inline pf_decl_constexpr vector<_Ty, 4, _Simd> &operator=(
				vector<_Ty, 4, _Simd> const &__r) pf_attr_noexcept = default;

		/// Destructor
		pf_decl_inline pf_decl_constexpr ~vector() pf_attr_noexcept = default;

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
		pf_decl_inline pf_decl_constexpr vector<_Ty, 4, _Simd> &operator+=(
				_Ty __ral) pf_attr_noexcept
		{
			this->x += __ral;
			this->y += __ral;
			this->z += __ral;
			this->w += __ral;
			return *this;
		}
		template <simd_align_t _SimdR>
		pf_decl_inline pf_decl_constexpr vector<_Ty, 4, _Simd> &operator+=(
				vector<_Ty, 4, _SimdR> const &__r) pf_attr_noexcept
		{
			this->x += __r.x;
			this->y += __r.y;
			this->z += __r.z;
			this->w += __r.w;
			return *this;
		}

		/// Operator-=
		pf_decl_inline pf_decl_constexpr vector<_Ty, 4, _Simd> &operator-=(
				_Ty __ral) pf_attr_noexcept
		{
			this->x -= __ral;
			this->y -= __ral;
			this->z -= __ral;
			this->w -= __ral;
			return *this;
		}
		template <simd_align_t _SimdR>
		pf_decl_inline pf_decl_constexpr vector<_Ty, 4, _Simd> &operator-=(
				vector<_Ty, 4, _SimdR> const &__r) pf_attr_noexcept
		{
			this->x -= __r.x;
			this->y -= __r.y;
			this->z -= __r.z;
			this->w -= __r.w;
			return *this;
		}

		/// Operator*=
		pf_decl_inline pf_decl_constexpr vector<_Ty, 4, _Simd> &operator*=(
				_Ty __ral) pf_attr_noexcept
		{
			this->x *= __ral;
			this->y *= __ral;
			this->z *= __ral;
			this->w *= __ral;
			return *this;
		}
		template <simd_align_t _SimdR>
		pf_decl_inline pf_decl_constexpr vector<_Ty, 4, _Simd> &operator*=(
				vector<_Ty, 4, _SimdR> const &__r) pf_attr_noexcept
		{
			this->x *= __r.x;
			this->y *= __r.y;
			this->z *= __r.z;
			this->w *= __r.w;
			return *this;
		}

		/// Operator/=
		pf_decl_inline pf_decl_constexpr vector<_Ty, 4, _Simd> &operator/=(
				_Ty __ral) pf_attr_noexcept
		{
			this->x /= __ral;
			this->y /= __ral;
			this->z /= __ral;
			this->w /= __ral;
			return *this;
		}
		template <simd_align_t _SimdR>
		pf_decl_inline pf_decl_constexpr vector<_Ty, 4, _Simd> &operator/=(
				vector<_Ty, 4, _SimdR> const &__r) pf_attr_noexcept
		{
			this->x /= __r.x;
			this->y /= __r.y;
			this->z /= __r.z;
			this->w /= __r.w;
			return *this;
		}

		/// Operator%=
		pf_decl_inline pf_decl_constexpr vector<_Ty, 4, _Simd> &operator%=(
				_Ty __ral) pf_attr_noexcept
				requires(std::is_integral_v<_Ty>)
		{
			this->x %= __ral;
			this->y %= __ral;
			this->z %= __ral;
			this->w %= __ral;
			return *this;
		}
		template <simd_align_t _SimdR>
		pf_decl_inline pf_decl_constexpr vector<_Ty, 4, _Simd> &operator%=(
				vector<_Ty, 4, _SimdR> const &__r) pf_attr_noexcept
				requires(std::is_integral_v<_Ty>)
		{
			this->x %= __r.x;
			this->y %= __r.y;
			this->z %= __r.z;
			this->w %= __r.w;
			return *this;
		}

		/// Operator&=
		pf_decl_inline pf_decl_constexpr vector<_Ty, 4, _Simd> &operator&=(
				_Ty __ral) pf_attr_noexcept
				requires(std::is_integral_v<_Ty>)
		{
			this->x &= __ral;
			this->y &= __ral;
			this->z &= __ral;
			this->w &= __ral;
			return *this;
		}
		template <simd_align_t _SimdR>
		pf_decl_inline pf_decl_constexpr vector<_Ty, 4, _Simd> &operator&=(
				vector<_Ty, 4, _SimdR> const &__r) pf_attr_noexcept
				requires(std::is_integral_v<_Ty>)
		{
			this->x &= __r.x;
			this->y &= __r.y;
			this->z &= __r.z;
			this->w &= __r.w;
			return *this;
		}

		/// Operator|=
		pf_decl_inline pf_decl_constexpr vector<_Ty, 4, _Simd> &operator|=(
				_Ty __ral) pf_attr_noexcept
				requires(std::is_integral_v<_Ty>)
		{
			this->x |= __ral;
			this->y |= __ral;
			this->z |= __ral;
			this->w |= __ral;
			return *this;
		}
		template <simd_align_t _SimdR>
		pf_decl_inline pf_decl_constexpr vector<_Ty, 4, _Simd> &operator|=(
				vector<_Ty, 4, _SimdR> const &__r) pf_attr_noexcept
				requires(std::is_integral_v<_Ty>)
		{
			this->x |= __r.x;
			this->y |= __r.y;
			this->z |= __r.z;
			this->w |= __r.w;
			return *this;
		}

		/// Operator^=
		pf_decl_inline pf_decl_constexpr vector<_Ty, 4, _Simd> &operator^=(
				_Ty __ral) pf_attr_noexcept
				requires(std::is_integral_v<_Ty>)
		{
			this->x ^= __ral;
			this->y ^= __ral;
			this->z ^= __ral;
			this->w ^= __ral;
			return *this;
		}
		template <simd_align_t _SimdR>
		pf_decl_inline pf_decl_constexpr vector<_Ty, 4, _Simd> &operator^=(
				vector<_Ty, 4, _SimdR> const &__r) pf_attr_noexcept
				requires(std::is_integral_v<_Ty>)
		{
			this->x ^= __r.x;
			this->y ^= __r.y;
			this->z ^= __r.z;
			this->w ^= __r.w;
			return *this;
		}

		/// Operator<<=
		pf_decl_inline pf_decl_constexpr vector<_Ty, 4, _Simd> &operator<<=(
				_Ty __ral) pf_attr_noexcept
				requires(std::is_integral_v<_Ty>)
		{
			this->x <<= __ral;
			this->y <<= __ral;
			this->z <<= __ral;
			this->w <<= __ral;
			return *this;
		}
		template <simd_align_t _SimdR>
		pf_decl_inline pf_decl_constexpr vector<_Ty, 4, _Simd> &operator<<=(
				vector<_Ty, 4, _SimdR> const &__r) pf_attr_noexcept
				requires(std::is_integral_v<_Ty>)
		{
			this->x <<= __r.x;
			this->y <<= __r.y;
			this->z <<= __r.z;
			this->w <<= __r.w;
			return *this;
		}

		/// Operator>>=
		pf_decl_inline pf_decl_constexpr vector<_Ty, 4, _Simd> &operator>>=(
				_Ty __ral) pf_attr_noexcept
				requires(std::is_integral_v<_Ty>)
		{
			this->x >>= __ral;
			this->y >>= __ral;
			this->z >>= __ral;
			this->w >>= __ral;
			return *this;
		}
		template <simd_align_t _SimdR>
		pf_decl_inline pf_decl_constexpr vector<_Ty, 4, _Simd> &operator>>=(
				vector<_Ty, 4, _SimdR> const &__r) pf_attr_noexcept
				requires(std::is_integral_v<_Ty>)
		{
			this->x >>= __r.x;
			this->y >>= __r.y;
			this->z >>= __r.z;
			this->w >>= __r.w;
			return *this;
		}

		/// Operator==
		template <simd_align_t _SimdR>
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr bool operator==(
				vector<_Ty, 4, _SimdR> const &__r) const pf_attr_noexcept
		{
			for (size_t i = 0; i < 4; ++i)
			{
				if (this->store_[i] != __r[i])
					return false;
			}
			return true;
		}

		/// Compare
		template <simd_align_t _SimdR>
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr vector<bool, 4> equal(
				vector<_Ty, 4, _SimdR> const &__r) const pf_attr_noexcept
		{
			vector<bool, 4> tmp;
			for (size_t i = 0; i < 4; ++i)
			{
				tmp[i] = this->store_[i] == __r[i];
			}
			return tmp;
		}
		template <simd_align_t _SimdR>
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr vector<bool, 4> not_equal(
				vector<_Ty, 4, _SimdR> const &__r) const pf_attr_noexcept
		{
			vector<bool, 4> tmp;
			for (size_t i = 0; i < 4; ++i)
			{
				tmp[i] = this->store_[i] != __r[i];
			}
			return tmp;
		}
		template <simd_align_t _SimdR>
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr vector<bool, 4> greater(
				vector<_Ty, 4, _SimdR> const &__r) const pf_attr_noexcept
		{
			vector<bool, 4> tmp;
			for (size_t i = 0; i < 4; ++i)
			{
				tmp[i] = this->store_[i] > __r[i];
			}
			return tmp;
		}
		template <simd_align_t _SimdR>
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr vector<bool, 4> greater_equal(
				vector<_Ty, 4, _SimdR> const &__r) const pf_attr_noexcept
		{
			vector<bool, 4> tmp;
			for (size_t i = 0; i < 4; ++i)
			{
				tmp[i] = this->store_[i] >= __r[i];
			}
			return tmp;
		}
		template <simd_align_t _SimdR>
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr vector<bool, 4> smaller(
				vector<_Ty, 4, _SimdR> const &__r) const pf_attr_noexcept
		{
			vector<bool, 4> tmp;
			for (size_t i = 0; i < 4; ++i)
			{
				tmp[i] = this->store_[i] < __r[i];
			}
			return tmp;
		}
		template <simd_align_t _SimdR>
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr vector<bool, 4> smaller_equal(
				vector<_Ty, 4, _SimdR> const &__r) const pf_attr_noexcept
		{
			vector<bool, 4> tmp;
			for (size_t i = 0; i < 4; ++i)
			{
				tmp[i] = this->store_[i] <= __r[i];
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

		/// Store -> By Components
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
		/// Store
		std::array<_Ty, 4> store_;
	};

	/// MATH: bvec4u_t
	using bvec4u_t = vector<bool, 4, SIMD_UNALIGNED>;

	/// MATH: uvec4_t
	using u16vec4_t = vector<uint16_t, 4, SIMD_ALIGNED>;
	using u32vec4_t = vector<uint32_t, 4, SIMD_ALIGNED>;
	using u64vec4_t = vector<uint64_t, 4, SIMD_ALIGNED>;

	/// MATH: uvec4u_t
	using u8vec4u_t	 = vector<uint8_t, 4, SIMD_UNALIGNED>;
	using u16vec4u_t = vector<uint16_t, 4, SIMD_UNALIGNED>;
	using u32vec4u_t = vector<uint32_t, 4, SIMD_UNALIGNED>;
	using u64vec4u_t = vector<uint64_t, 4, SIMD_UNALIGNED>;

	/// MATH: ivec4_t
	using i16vec4_t = vector<int16_t, 4, SIMD_ALIGNED>;
	using i32vec4_t = vector<int32_t, 4, SIMD_ALIGNED>;
	using i64vec4_t = vector<int64_t, 4, SIMD_ALIGNED>;

	/// MATH: ivec4u_t
	using i8vec4u_t	 = vector<int8_t, 4, SIMD_UNALIGNED>;
	using i16vec4u_t = vector<int16_t, 4, SIMD_UNALIGNED>;
	using i32vec4u_t = vector<int32_t, 4, SIMD_UNALIGNED>;
	using i64vec4u_t = vector<int64_t, 4, SIMD_UNALIGNED>;

	/// MATH: fvec4_t
	using f32vec4_t = vector<float32_t, 4, SIMD_ALIGNED>;
	using f64vec4_t = vector<float64_t, 4, SIMD_ALIGNED>;

	/// MATH: fvec4u_t
	using f32vec4u_t = vector<float32_t, 4, SIMD_UNALIGNED>;
	using f64vec4u_t = vector<float64_t, 4, SIMD_UNALIGNED>;
}

// Anonymous structs
#pragma GCC diagnostic pop

#endif // !PULSAR_MATH_VECTOR4_INL