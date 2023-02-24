/*! @file   vector4.inl
 *  @author Louis-Quentin Noé (noe.louis-quentin@hotmail.fr)
 *  @brief  Definition of vector with 4 components.
 *  @date   30-08-2022
 *
 *  @copyright Copyright (c) 2023 - Pulsar Software
 *
 *  @since 0.1.1
 */

#ifndef PULSAR_MATH_VECTOR4_INL
#define PULSAR_MATH_VECTOR4_INL 1

// Include: Pulsar
#include "pulsar/pulsar.hpp"

// Anonymous structs -> Disable warning
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"

// Pulsar
namespace pul
{
	/// MATH: Vector
	template <typename _Ty, size_t _Num>
	requires(std::is_arithmetic_v<_Ty>
					 && _Num > 1)
	union vector;

	/// MATH: Vector -> 4
	template <typename _Ty>
	requires(std::is_arithmetic_v<_Ty>)
	union vector<_Ty, 4>
	{
		template <typename _TyF, size_t _NumF>
		requires(std::is_arithmetic_v<_TyF>&& _NumF > 1)
		pf_decl_friend union vector;

	public:
		/// Constructors
		pf_decl_constexpr vector(
			_Ty __ral = 0) pf_attr_noexcept
		: store_ { __ral }
		{}
		pf_decl_constexpr vector(
			_Ty __ral1,
			_Ty __ral2,
			_Ty __ral3,
			_Ty __ral4) pf_attr_noexcept
		: store_ { __ral1, __ral2, __ral3, __ral4 }
		{}
		pf_decl_constexpr vector(
			vector<_Ty, 3> const &__r1,
			_Ty __ral4) pf_attr_noexcept
		: store_ { __r1.x, __r1.y, __r1.z, __ral4 }
		{}
		pf_decl_constexpr vector(
			_Ty __ral1,
			vector<_Ty, 3> const &__r2) pf_attr_noexcept
		: store_ { __ral1, __r2.x, __r2.y, __r2.z }
		{}
		pf_decl_constexpr vector(
			vector<_Ty, 2> const &__r1,
			vector<_Ty, 2> const &__r2) pf_attr_noexcept
		: store_ { __r1.x, __r1.y, __r2.x, __r2.y }
		{}
		pf_decl_constexpr vector(
			vector<_Ty, 2> const &__r,
			_Ty __ral1,
			_Ty __ral2) pf_attr_noexcept
		: store_ { __r.x, __r.y, __ral1, __ral2 }
		{}
		pf_decl_constexpr vector(
			_Ty __ral1,
			vector<_Ty, 2> const &__r,
			_Ty __ral2) pf_attr_noexcept
		: store_ { __ral1, __r.x, __r.y, __ral2 }
		{}
		pf_decl_constexpr vector(
			_Ty __ral1,
			_Ty __ral2,
			vector<_Ty, 2> const &__r) pf_attr_noexcept
		: store_ { __ral1, __ral2, __r.x, __r.y }
		{}
		pf_decl_constexpr vector(
			vector<_Ty, 4> const &__r) pf_attr_noexcept
		: store_(__r.store_)
		{}

		/// Operator=
		pf_decl_constexpr vector<_Ty, 4> &
		operator =(
			vector<_Ty, 4> const &__r) pf_attr_noexcept = default;

		/// Destructor
		pf_decl_constexpr ~vector() pf_attr_noexcept = default;

		/// Operator[]
		pf_hint_nodiscard pf_decl_constexpr _Ty &operator [](
			size_t __index) pf_attr_noexcept
		{
			return this->store_[__index];
		}
		pf_hint_nodiscard pf_decl_constexpr const _Ty &operator [](
			size_t __index) const pf_attr_noexcept
		{
			return this->store_[__index];
		}

		/// Operator+=
		pf_decl_constexpr vector<_Ty, 4> &operator +=(
			_Ty __ral) pf_attr_noexcept
		{
			this->x += __ral;
			this->y += __ral;
			this->z += __ral;
			this->w += __ral;
			return *this;
		}
		pf_decl_constexpr vector<_Ty, 4> &operator +=(
			vector<_Ty, 4> const &__r) pf_attr_noexcept
		{
			this->x += __r.x;
			this->y += __r.y;
			this->z += __r.z;
			this->w += __r.w;
			return *this;
		}

		/// Operator-=
		pf_decl_constexpr vector<_Ty, 4> &operator -=(
			_Ty __ral) pf_attr_noexcept
		{
			this->x -= __ral;
			this->y -= __ral;
			this->z -= __ral;
			this->w -= __ral;
			return *this;
		}
		pf_decl_constexpr vector<_Ty, 4> &operator -=(
			vector<_Ty, 4> const &__r) pf_attr_noexcept
		{
			this->x -= __r.x;
			this->y -= __r.y;
			this->z -= __r.z;
			this->w -= __r.w;
			return *this;
		}

		/// Operator*=
		pf_decl_constexpr vector<_Ty, 4> &operator *=(
			_Ty __ral) pf_attr_noexcept
		{
			this->x *= __ral;
			this->y *= __ral;
			this->z *= __ral;
			this->w *= __ral;
			return *this;
		}
		pf_decl_constexpr vector<_Ty, 4> &operator *=(
			vector<_Ty, 4> const &__r) pf_attr_noexcept
		{
			this->x *= __r.x;
			this->y *= __r.y;
			this->z *= __r.z;
			this->w *= __r.w;
			return *this;
		}

		/// Operator/=
		pf_decl_constexpr vector<_Ty, 4> &operator /=(
			_Ty __ral) pf_attr_noexcept
		{
			this->x /= __ral;
			this->y /= __ral;
			this->z /= __ral;
			this->w /= __ral;
			return *this;
		}
		pf_decl_constexpr vector<_Ty, 4> &operator /=(
			vector<_Ty, 4> const &__r) pf_attr_noexcept
		{
			this->x /= __r.x;
			this->y /= __r.y;
			this->z /= __r.z;
			this->w /= __r.w;
			return *this;
		}

		/// Operator%=
		pf_decl_constexpr vector<_Ty, 4> &operator %=(
			_Ty __ral) pf_attr_noexcept
		requires(std::is_integral_v<_Ty>)
		{
			this->x %= __ral;
			this->y %= __ral;
			this->z %= __ral;
			this->w %= __ral;
			return *this;
		}
		pf_decl_constexpr vector<_Ty, 4> &operator %=(
			vector<_Ty, 4> const &__r) pf_attr_noexcept
		requires(std::is_integral_v<_Ty>)
		{
			this->x %= __r.x;
			this->y %= __r.y;
			this->z %= __r.z;
			this->w %= __r.w;
			return *this;
		}

		/// Operator&=
		pf_decl_constexpr vector<_Ty, 4> &operator &=(
			_Ty __ral) pf_attr_noexcept
		requires(std::is_integral_v<_Ty>)
		{
			this->x &= __ral;
			this->y &= __ral;
			this->z &= __ral;
			this->w &= __ral;
			return *this;
		}
		pf_decl_constexpr vector<_Ty, 4> &operator &=(
			vector<_Ty, 4> const &__r) pf_attr_noexcept
		requires(std::is_integral_v<_Ty>)
		{
			this->x &= __r.x;
			this->y &= __r.y;
			this->z &= __r.z;
			this->w &= __r.w;
			return *this;
		}

		/// Operator|=
		pf_decl_constexpr vector<_Ty, 4> &operator |=(
			_Ty __ral) pf_attr_noexcept
		requires(std::is_integral_v<_Ty>)
		{
			this->x |= __ral;
			this->y |= __ral;
			this->z |= __ral;
			this->w |= __ral;
			return *this;
		}
		pf_decl_constexpr vector<_Ty, 4> &operator |=(
			vector<_Ty, 4> const &__r) pf_attr_noexcept
		requires(std::is_integral_v<_Ty>)
		{
			this->x |= __r.x;
			this->y |= __r.y;
			this->z |= __r.z;
			this->w |= __r.w;
			return *this;
		}

		/// Operator^=
		pf_decl_constexpr vector<_Ty, 4> &operator ^=(
			_Ty __ral) pf_attr_noexcept
		requires(std::is_integral_v<_Ty>)
		{
			this->x ^= __ral;
			this->y ^= __ral;
			this->z ^= __ral;
			this->w ^= __ral;
			return *this;
		}
		pf_decl_constexpr vector<_Ty, 4> &operator ^=(
			vector<_Ty, 4> const &__r) pf_attr_noexcept
		requires(std::is_integral_v<_Ty>)
		{
			this->x ^= __r.x;
			this->y ^= __r.y;
			this->z ^= __r.z;
			this->w ^= __r.w;
			return *this;
		}

		/// Operator<<=
		pf_decl_constexpr vector<_Ty, 4> &operator <<=(
			_Ty __ral) pf_attr_noexcept
		requires(std::is_integral_v<_Ty>)
		{
			this->x <<= __ral;
			this->y <<= __ral;
			this->z <<= __ral;
			this->w <<= __ral;
			return *this;
		}
		pf_decl_constexpr vector<_Ty, 4> &operator <<=(
			vector<_Ty, 4> const &__r) pf_attr_noexcept
		requires(std::is_integral_v<_Ty>)
		{
			this->x <<= __r.x;
			this->y <<= __r.y;
			this->z <<= __r.z;
			this->w <<= __r.w;
			return *this;
		}

		/// Operator>>=
		pf_decl_constexpr vector<_Ty, 4> &operator >>=(
			_Ty __ral) pf_attr_noexcept
		requires(std::is_integral_v<_Ty>)
		{
			this->x >>= __ral;
			this->y >>= __ral;
			this->z >>= __ral;
			this->w >>= __ral;
			return *this;
		}
		pf_decl_constexpr vector<_Ty, 4> &operator >>=(
			vector<_Ty, 4> const &__r) pf_attr_noexcept
		requires(std::is_integral_v<_Ty>)
		{
			this->x >>= __r.x;
			this->y >>= __r.y;
			this->z >>= __r.z;
			this->w >>= __r.w;
			return *this;
		}

		/// Operator==
		pf_hint_nodiscard pf_decl_constexpr bool operator ==(
			vector<_Ty, 4> const &__r) const pf_attr_noexcept
		{
			for(size_t i = 0; i < 4; ++i)
			{
				if(this->store_[i] != __r[i])
					return false;
			}
			return true;
		}

		/// Compare
		pf_hint_nodiscard pf_decl_constexpr vector<bool, 4> equal(
			vector<_Ty, 4> const &__r) const pf_attr_noexcept
		{
			vector<bool, 4> tmp;
			for(size_t i = 0; i < 4; ++i)
			{
				tmp[i] = this->store_[i] == __r[i];
			}
			return tmp;
		}
		pf_hint_nodiscard pf_decl_constexpr vector<bool, 4> not_equal(
			vector<_Ty, 4> const &__r) const pf_attr_noexcept
		{
			vector<bool, 4> tmp;
			for(size_t i = 0; i < 4; ++i)
			{
				tmp[i] = this->store_[i] != __r[i];
			}
			return tmp;
		}
		pf_hint_nodiscard pf_decl_constexpr vector<bool, 4> greater(
			vector<_Ty, 4> const &__r) const pf_attr_noexcept
		{
			vector<bool, 4> tmp;
			for(size_t i = 0; i < 4; ++i)
			{
				tmp[i] = this->store_[i] > __r[i];
			}
			return tmp;
		}
		pf_hint_nodiscard pf_decl_constexpr vector<bool, 4> greater_equal(
			vector<_Ty, 4> const &__r) const pf_attr_noexcept
		{
			vector<bool, 4> tmp;
			for(size_t i = 0; i < 4; ++i)
			{
				tmp[i] = this->store_[i] >= __r[i];
			}
			return tmp;
		}
		pf_hint_nodiscard pf_decl_constexpr vector<bool, 4> smaller(
			vector<_Ty, 4> const &__r) const pf_attr_noexcept
		{
			vector<bool, 4> tmp;
			for(size_t i = 0; i < 4; ++i)
			{
				tmp[i] = this->store_[i] < __r[i];
			}
			return tmp;
		}
		pf_hint_nodiscard pf_decl_constexpr vector<bool, 4> smaller_equal(
			vector<_Ty, 4> const &__r) const pf_attr_noexcept
		{
			vector<bool, 4> tmp;
			for(size_t i = 0; i < 4; ++i)
			{
				tmp[i] = this->store_[i] <= __r[i];
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
		array<_Ty, 4> store_;
	};

	/// MATH: bvec4_t
	using bvec4_t
	pf_alignas(bool) = vector<bool, 4>;

	/// MATH: uvec4_t
	using u8vec4_t pf_alignas(uint8_t)	 = vector<uint8_t, 4>;
	using u16vec4_t pf_alignas(uint16_t) = vector<uint16_t, 4>;
	using u32vec4_t pf_alignas(uint32_t) = vector<uint32_t, 4>;
	using u64vec4_t pf_alignas(uint64_t) = vector<uint64_t, 4>;

	/// MATH: s_uvec4_t
	using s_u16vec4_t pf_alignas_n(uint16_t, 4) = vector<uint16_t, 4>;
	using s_u32vec4_t pf_alignas_n(uint32_t, 4) = vector<uint32_t, 4>;
	using s_u64vec4_t pf_alignas_n(uint64_t, 4) = vector<uint64_t, 4>;

	/// MATH: ivec4_t
	using i8vec4_t pf_alignas(int8_t)		= vector<int8_t, 4>;
	using i16vec4_t pf_alignas(int16_t) = vector<int16_t, 4>;
	using i32vec4_t pf_alignas(int32_t) = vector<int32_t, 4>;
	using i64vec4_t pf_alignas(int64_t) = vector<int64_t, 4>;

	/// MATH: s_ivec4_t
	using s_i16vec4_t pf_alignas_n(int16_t, 4) = vector<int16_t, 4>;
	using s_i32vec4_t pf_alignas_n(int32_t, 4) = vector<int32_t, 4>;
	using s_i64vec4_t pf_alignas_n(int64_t, 4) = vector<int64_t, 4>;

	/// MATH: fvec4_t
	using f32vec4_t pf_alignas(float32_t) = vector<float32_t, 4>;
	using f64vec4_t pf_alignas(float64_t) = vector<float64_t, 4>;

	/// MATH: s_fvec4_t
	using s_f32vec4_t pf_alignas_n(float32_t, 4) = vector<float32_t, 4>;
	using s_f64vec4_t pf_alignas_n(float64_t, 4) = vector<float64_t, 4>;
}

// Anonymous structs
#pragma GCC diagnostic pop

#endif // !PULSAR_MATH_VECTOR4_INL