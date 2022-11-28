/*! @file   vector2.inl
 *  @author Fluffy (noe.louis-quentin@hotmail.fr)
 *  @brief  Definition of vector with 2 components.
 *  @date   30-08-2022
 *
 *  @copyright Copyright (c) 2022 - Pulsar Software
 *
 *  @since 0.1.1
 */

#ifndef PULSAR_MATH_VECTOR2_INL
#define PULSAR_MATH_VECTOR2_INL 1

// Include: Pulsar
#include "pulsar/intrin.hpp"

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

	/// MATH: Vector -> 2
	template <typename _Ty>
	requires(std::is_arithmetic_v<_Ty>)
	union vector<_Ty, 2>
	{
		template <typename _TyF, size_t _NumF>
		requires(std::is_arithmetic_v<_TyF>&& _NumF > 1)
		pf_decl_friend union vector;

		/// Constructors
		pf_decl_inline pf_decl_constexpr vector(
			_Ty __val = static_cast<_Ty>(0)) pf_attr_noexcept
		: store_
		{
			__val
		}
		{}
		pf_decl_inline pf_decl_constexpr vector(
			_Ty __val1,
			_Ty __val2) pf_attr_noexcept
		: store_
		{
			__val1, __val2
		}
		{}
		pf_decl_inline pf_decl_constexpr vector(
			vector<_Ty, 2> const &__r) pf_attr_noexcept
		: store_(__r.store_)
		{}

		/// Operator=
		pf_decl_inline pf_decl_constexpr vector<_Ty, 2> &
		operator =(
			vector<_Ty, 2> const &__r) pf_attr_noexcept = default;

		/// Destructor
		pf_decl_inline pf_decl_constexpr ~vector() pf_attr_noexcept = default;

		/// Operator[]
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr _Ty &operator [](
			size_t __index) pf_attr_noexcept
		{
			return this->store_[__index];
		}
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const _Ty &operator [](
			size_t __index) const pf_attr_noexcept
		{
			return this->store_[__index];
		}

		/// Operator+=
		pf_decl_inline pf_decl_constexpr vector<_Ty, 2> &operator +=(
			_Ty __val) pf_attr_noexcept
		{
			this->x += __val;
			this->y += __val;
			return *this;
		}
		pf_decl_inline pf_decl_constexpr vector<_Ty, 2> &operator +=(
			vector<_Ty, 2> const &__r) pf_attr_noexcept
		{
			this->x += __r.x;
			this->y += __r.y;
			return *this;
		}

		/// Operator-=
		pf_decl_inline pf_decl_constexpr vector<_Ty, 2> &operator -=(
			_Ty __val) pf_attr_noexcept
		{
			this->x -= __val;
			this->y -= __val;
			return *this;
		}
		pf_decl_inline pf_decl_constexpr vector<_Ty, 2> &operator -=(
			vector<_Ty, 2> const &__r) pf_attr_noexcept
		{
			this->x -= __r.x;
			this->y -= __r.y;
			return *this;
		}

		/// Operator*=
		pf_decl_inline pf_decl_constexpr vector<_Ty, 2> &operator *=(
			_Ty __val) pf_attr_noexcept
		{
			this->x *= __val;
			this->y *= __val;
			return *this;
		}
		pf_decl_inline pf_decl_constexpr vector<_Ty, 2> &operator *=(
			vector<_Ty, 2> const &__r) pf_attr_noexcept
		{
			this->x *= __r.x;
			this->y *= __r.y;
			return *this;
		}

		/// Operator/=
		pf_decl_inline pf_decl_constexpr vector<_Ty, 2> &operator /=(
			_Ty __val) pf_attr_noexcept
		{
			this->x /= __val;
			this->y /= __val;
			return *this;
		}
		pf_decl_inline pf_decl_constexpr vector<_Ty, 2> &operator /=(
			vector<_Ty, 2> const &__r) pf_attr_noexcept
		{
			this->x /= __r.x;
			this->y /= __r.y;
			return *this;
		}

		/// Operator%=
		pf_decl_inline pf_decl_constexpr vector<_Ty, 2> &operator %=(
			_Ty __val) pf_attr_noexcept
		requires(std::is_integral_v<_Ty>)
		{
			this->x %= __val;
			this->y %= __val;
			return *this;
		}
		pf_decl_inline pf_decl_constexpr vector<_Ty, 2> &operator %=(
			vector<_Ty, 2> const &__r) pf_attr_noexcept
		requires(std::is_integral_v<_Ty>)
		{
			this->x %= __r.x;
			this->y %= __r.y;
			return *this;
		}

		/// Operator&=
		pf_decl_inline pf_decl_constexpr vector<_Ty, 2> &operator &=(
			_Ty __val) pf_attr_noexcept
		requires(std::is_integral_v<_Ty>)
		{
			this->x &= __val;
			this->y &= __val;
			return *this;
		}
		pf_decl_inline pf_decl_constexpr vector<_Ty, 2> &operator &=(
			vector<_Ty, 2> const &__r) pf_attr_noexcept
		requires(std::is_integral_v<_Ty>)
		{
			this->x &= __r.x;
			this->y &= __r.y;
			return *this;
		}

		/// Operator|=
		pf_decl_inline pf_decl_constexpr vector<_Ty, 2> &operator |=(
			_Ty __val) pf_attr_noexcept
		requires(std::is_integral_v<_Ty>)
		{
			this->x |= __val;
			this->y |= __val;
			return *this;
		}
		pf_decl_inline pf_decl_constexpr vector<_Ty, 2> &operator |=(
			vector<_Ty, 2> const &__r) pf_attr_noexcept
		requires(std::is_integral_v<_Ty>)
		{
			this->x |= __r.x;
			this->y |= __r.y;
			return *this;
		}

		/// Operator^=
		pf_decl_inline pf_decl_constexpr vector<_Ty, 2> &operator ^=(
			_Ty __val) pf_attr_noexcept
		requires(std::is_integral_v<_Ty>)
		{
			this->x ^= __val;
			this->y ^= __val;
			return *this;
		}
		pf_decl_inline pf_decl_constexpr vector<_Ty, 2> &operator ^=(
			vector<_Ty, 2> const &__r) pf_attr_noexcept
		requires(std::is_integral_v<_Ty>)
		{
			this->x ^= __r.x;
			this->y ^= __r.y;
			return *this;
		}

		/// Operator<<=
		pf_decl_inline pf_decl_constexpr vector<_Ty, 2> &operator <<=(
			_Ty __val) pf_attr_noexcept
		requires(std::is_integral_v<_Ty>)
		{
			this->x <<= __val;
			this->y <<= __val;
			return *this;
		}
		pf_decl_inline pf_decl_constexpr vector<_Ty, 2> &operator <<=(
			vector<_Ty, 2> const &__r) pf_attr_noexcept
		requires(std::is_integral_v<_Ty>)
		{
			this->x <<= __r.x;
			this->y <<= __r.y;
			return *this;
		}

		/// Operator>>=
		pf_decl_inline pf_decl_constexpr vector<_Ty, 2> &operator >>=(
			_Ty __val) pf_attr_noexcept
		requires(std::is_integral_v<_Ty>)
		{
			this->x >>= __val;
			this->y >>= __val;
			return *this;
		}
		pf_decl_inline pf_decl_constexpr vector<_Ty, 2> &operator >>=(
			vector<_Ty, 2> const &__r) pf_attr_noexcept
		requires(std::is_integral_v<_Ty>)
		{
			this->x >>= __r.x;
			this->y >>= __r.y;
			return *this;
		}

		/// Operator==
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr bool operator ==(
			vector<_Ty, 2> const &__r) const pf_attr_noexcept
		{
			for(size_t i = 0; i < 2; ++i)
			{
				if(this->store_[i] != __r[i])
					return false;
			}
			return true;
		}

		/// Compare
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr vector<bool, 2> equal(
			vector<_Ty, 2> const &__r) const pf_attr_noexcept
		{
			vector<bool, 2> tmp;
			for(size_t i = 0; i < 2; ++i)
			{
				tmp[i] = this->store_[i] == __r[i];
			}
			return tmp;
		}
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr vector<bool, 2> not_equal(
			vector<_Ty, 2> const &__r) const pf_attr_noexcept
		{
			vector<bool, 2> tmp;
			for(size_t i = 0; i < 2; ++i)
			{
				tmp[i] = this->store_[i] != __r[i];
			}
			return tmp;
		}
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr vector<bool, 2> greater(
			vector<_Ty, 2> const &__r) const pf_attr_noexcept
		{
			vector<bool, 2> tmp;
			for(size_t i = 0; i < 2; ++i)
			{
				tmp[i] = this->store_[i] > __r[i];
			}
			return tmp;
		}
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr vector<bool, 2> greater_equal(
			vector<_Ty, 2> const &__r) const pf_attr_noexcept
		{
			vector<bool, 2> tmp;
			for(size_t i = 0; i < 2; ++i)
			{
				tmp[i] = this->store_[i] >= __r[i];
			}
			return tmp;
		}
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr vector<bool, 2> smaller(
			vector<_Ty, 2> const &__r) const pf_attr_noexcept
		{
			vector<bool, 2> tmp;
			for(size_t i = 0; i < 2; ++i)
			{
				tmp[i] = this->store_[i] < __r[i];
			}
			return tmp;
		}
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr vector<bool, 2> smaller_equal(
			vector<_Ty, 2> const &__r) const pf_attr_noexcept
		{
			vector<bool, 2> tmp;
			for(size_t i = 0; i < 2; ++i)
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
		};
		struct
		{
			_Ty r;
			_Ty g;
		};

	private:
		/// Store
		std::array<_Ty, 2> store_;
	};

	/// MATH: bvec2_t
	using bvec2_t pf_alignas(alignof(bool)) = vector<bool, 2>;

	/// MATH: uvec2_t
	using u8vec2_t pf_alignas(alignof(uint8_t))		= vector<uint8_t, 2>;
	using u16vec2_t pf_alignas(alignof(uint16_t)) = vector<uint16_t, 2>;
	using u32vec2_t pf_alignas(alignof(uint32_t)) = vector<uint32_t, 2>;
	using u64vec2_t pf_alignas(alignof(uint64_t)) = vector<uint64_t, 2>;

	/// MATH: s_uvec2_t
	using s_u32vec2_t pf_alignas_n(uint32_t, 2) = vector<uint32_t, 2>;
	using s_u64vec2_t pf_alignas_n(uint64_t, 2) = vector<uint64_t, 2>;

	/// MATH: ivec2_t
	using i8vec2_t pf_alignas(int8_t)		= vector<int8_t, 2>;
	using i16vec2_t pf_alignas(int16_t) = vector<int16_t, 2>;
	using i32vec2_t pf_alignas(int32_t) = vector<int32_t, 2>;
	using i64vec2_t pf_alignas(int64_t) = vector<int64_t, 2>;

	/// MATH: s_ivec2_t
	using s_i32vec2_t pf_alignas_n(int32_t, 2) = vector<int32_t, 2>;
	using s_i64vec2_t pf_alignas_n(int64_t, 2) = vector<int64_t, 2>;

	/// MATH: fvec2_t
	using f32vec2_t pf_alignas(float32_t) = vector<float32_t, 2>;
	using f64vec2_t pf_alignas(float64_t) = vector<float64_t, 2>;

	/// MATH: s_fvec2_t
	using s_f32vec2_t pf_alignas_n(float32_t, 2) = vector<float32_t, 2>;
	using s_f64vec2_t pf_alignas_n(float64_t, 2) = vector<float64_t, 2>;
}

// Anonymous structs
#pragma GCC diagnostic pop

#endif // !PULSAR_MATH_VECTOR2_INL