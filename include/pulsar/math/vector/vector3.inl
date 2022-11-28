/*! @file   vector3.inl
 *  @author Fluffy (noe.louis-quentin@hotmail.fr)
 *  @brief  Definition of vector with 3 components.
 *  @date   30-08-2022
 *
 *  @copyright Copyright (c) 2022 - Pulsar Software
 *
 *  @since 0.1.1
 */

#ifndef PULSAR_MATH_VECTOR3_INL
#define PULSAR_MATH_VECTOR3_INL 1

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

	/// MATH: Vector -> 3
	template <typename _Ty>
	requires(std::is_arithmetic_v<_Ty>)
	union vector<_Ty, 3>
	{
		template <typename _TyF, size_t _NumF>
		requires(std::is_arithmetic_v<_TyF>&& _NumF > 1)
		pf_decl_friend union vector;

	public:
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
			_Ty __val2,
			_Ty __val3) pf_attr_noexcept
		: store_
		{
			__val1, __val2, __val3
		}
		{}
		pf_decl_inline pf_decl_constexpr vector(
			vector<_Ty, 3> const &__v1,
			_Ty __val3) pf_attr_noexcept
		: store_
		{
			__v1.x, __v1.y, __val3
		}
		{}
		pf_decl_inline pf_decl_constexpr vector(
			_Ty __val1,
			vector<_Ty, 3> const &__v2) pf_attr_noexcept
		: store_
		{
			__val1, __v2.x, __v2.y
		}
		{}
		pf_decl_inline pf_decl_constexpr vector(
			vector<_Ty, 3> const &__r) pf_attr_noexcept
		: store_(__r.store_)
		{}

		/// Operator=
		pf_decl_inline pf_decl_constexpr vector<_Ty, 3> &
		operator =(
			vector<_Ty, 3> const &__r) pf_attr_noexcept = default;

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
		pf_decl_inline pf_decl_constexpr vector<_Ty, 3> &operator +=(
			_Ty __val) pf_attr_noexcept
		{
			this->x += __val;
			this->y += __val;
			this->z += __val;
			return *this;
		}
		pf_decl_inline pf_decl_constexpr vector<_Ty, 3> &operator +=(
			vector<_Ty, 3> const &__r) pf_attr_noexcept
		{
			this->x += __r.x;
			this->y += __r.y;
			this->z += __r.z;
			return *this;
		}

		/// Operator-=
		pf_decl_inline pf_decl_constexpr vector<_Ty, 3> &operator -=(
			_Ty __val) pf_attr_noexcept
		{
			this->x -= __val;
			this->y -= __val;
			this->z -= __val;
			return *this;
		}
		pf_decl_inline pf_decl_constexpr vector<_Ty, 3> &operator -=(
			vector<_Ty, 3> const &__r) pf_attr_noexcept
		{
			this->x -= __r.x;
			this->y -= __r.y;
			this->z -= __r.z;
			return *this;
		}

		/// Operator*=
		pf_decl_inline pf_decl_constexpr vector<_Ty, 3> &operator *=(
			_Ty __val) pf_attr_noexcept
		{
			this->x *= __val;
			this->y *= __val;
			this->z *= __val;
			return *this;
		}
		pf_decl_inline pf_decl_constexpr vector<_Ty, 3> &operator *=(
			vector<_Ty, 3> const &__r) pf_attr_noexcept
		{
			this->x *= __r.x;
			this->y *= __r.y;
			this->z *= __r.z;
			return *this;
		}

		/// Operator/=
		pf_decl_inline pf_decl_constexpr vector<_Ty, 3> &operator /=(
			_Ty __val) pf_attr_noexcept
		{
			this->x /= __val;
			this->y /= __val;
			this->z /= __val;
			return *this;
		}
		pf_decl_inline pf_decl_constexpr vector<_Ty, 3> &operator /=(
			vector<_Ty, 3> const &__r) pf_attr_noexcept
		{
			this->x /= __r.x;
			this->y /= __r.y;
			this->z /= __r.z;
			return *this;
		}

		/// Operator%=
		pf_decl_inline pf_decl_constexpr vector<_Ty, 3> &operator %=(
			_Ty __val) pf_attr_noexcept
		requires(std::is_integral_v<_Ty>)
		{
			this->x %= __val;
			this->y %= __val;
			this->z %= __val;
			return *this;
		}
		pf_decl_inline pf_decl_constexpr vector<_Ty, 3> &operator %=(
			vector<_Ty, 3> const &__r) pf_attr_noexcept
		requires(std::is_integral_v<_Ty>)
		{
			this->x %= __r.x;
			this->y %= __r.y;
			this->z %= __r.z;
			return *this;
		}

		/// Operator&=
		pf_decl_inline pf_decl_constexpr vector<_Ty, 3> &operator &=(
			_Ty __val) pf_attr_noexcept
		requires(std::is_integral_v<_Ty>)
		{
			this->x &= __val;
			this->y &= __val;
			this->z &= __val;
			return *this;
		}
		pf_decl_inline pf_decl_constexpr vector<_Ty, 3> &operator &=(
			vector<_Ty, 3> const &__r) pf_attr_noexcept
		requires(std::is_integral_v<_Ty>)
		{
			this->x &= __r.x;
			this->y &= __r.y;
			this->z &= __r.z;
			return *this;
		}

		/// Operator|=
		pf_decl_inline pf_decl_constexpr vector<_Ty, 3> &operator |=(
			_Ty __val) pf_attr_noexcept
		requires(std::is_integral_v<_Ty>)
		{
			this->x |= __val;
			this->y |= __val;
			this->z |= __val;
			return *this;
		}
		pf_decl_inline pf_decl_constexpr vector<_Ty, 3> &operator |=(
			vector<_Ty, 3> const &__r) pf_attr_noexcept
		requires(std::is_integral_v<_Ty>)
		{
			this->x |= __r.x;
			this->y |= __r.y;
			this->z |= __r.z;
			return *this;
		}

		/// Operator^=
		pf_decl_inline pf_decl_constexpr vector<_Ty, 3> &operator ^=(
			_Ty __val) pf_attr_noexcept
		requires(std::is_integral_v<_Ty>)
		{
			this->x ^= __val;
			this->y ^= __val;
			this->z ^= __val;
			return *this;
		}
		pf_decl_inline pf_decl_constexpr vector<_Ty, 3> &operator ^=(
			vector<_Ty, 3> const &__r) pf_attr_noexcept
		requires(std::is_integral_v<_Ty>)
		{
			this->x ^= __r.x;
			this->y ^= __r.y;
			this->z ^= __r.z;
			return *this;
		}

		/// Operator<<=
		pf_decl_inline pf_decl_constexpr vector<_Ty, 3> &operator <<=(
			_Ty __val) pf_attr_noexcept
		requires(std::is_integral_v<_Ty>)
		{
			this->x <<= __val;
			this->y <<= __val;
			this->z <<= __val;
			return *this;
		}
		pf_decl_inline pf_decl_constexpr vector<_Ty, 3> &operator <<=(
			vector<_Ty, 3> const &__r) pf_attr_noexcept
		requires(std::is_integral_v<_Ty>)
		{
			this->x <<= __r.x;
			this->y <<= __r.y;
			this->z <<= __r.z;
			return *this;
		}

		/// Operator>>=
		pf_decl_inline pf_decl_constexpr vector<_Ty, 3> &operator >>=(
			_Ty __val) pf_attr_noexcept
		requires(std::is_integral_v<_Ty>)
		{
			this->x >>= __val;
			this->y >>= __val;
			this->z >>= __val;
			return *this;
		}
		pf_decl_inline pf_decl_constexpr vector<_Ty, 3> &operator >>=(
			vector<_Ty, 3> const &__r) pf_attr_noexcept
		requires(std::is_integral_v<_Ty>)
		{
			this->x >>= __r.x;
			this->y >>= __r.y;
			this->z >>= __r.z;
			return *this;
		}

		/// Operator==
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr bool operator ==(
			vector<_Ty, 3> const &__r) const pf_attr_noexcept
		{
			for(size_t i = 0; i < 3; ++i)
			{
				if(this->store_[i] != __r[i])
					return false;
			}
			return true;
		}

		/// Compare
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr vector<bool, 3> equal(
			vector<_Ty, 3> const &__r) const pf_attr_noexcept
		{
			vector<bool, 3> tmp;
			for(size_t i = 0; i < 3; ++i)
			{
				tmp[i] = this->store_[i] == __r[i];
			}
			return tmp;
		}
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr vector<bool, 3> not_equal(
			vector<_Ty, 3> const &__r) const pf_attr_noexcept
		{
			vector<bool, 3> tmp;
			for(size_t i = 0; i < 3; ++i)
			{
				tmp[i] = this->store_[i] != __r[i];
			}
			return tmp;
		}
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr vector<bool, 3> greater(
			vector<_Ty, 3> const &__r) const pf_attr_noexcept
		{
			vector<bool, 3> tmp;
			for(size_t i = 0; i < 3; ++i)
			{
				tmp[i] = this->store_[i] > __r[i];
			}
			return tmp;
		}
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr vector<bool, 3> greater_equal(
			vector<_Ty, 3> const &__r) const pf_attr_noexcept
		{
			vector<bool, 3> tmp;
			for(size_t i = 0; i < 3; ++i)
			{
				tmp[i] = this->store_[i] >= __r[i];
			}
			return tmp;
		}
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr vector<bool, 3> smaller(
			vector<_Ty, 3> const &__r) const pf_attr_noexcept
		{
			vector<bool, 3> tmp;
			for(size_t i = 0; i < 3; ++i)
			{
				tmp[i] = this->store_[i] < __r[i];
			}
			return tmp;
		}
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr vector<bool, 3> smaller_equal(
			vector<_Ty, 3> const &__r) const pf_attr_noexcept
		{
			vector<bool, 3> tmp;
			for(size_t i = 0; i < 3; ++i)
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
		};
		struct
		{
			_Ty r;
			_Ty g;
			_Ty b;
		};

	private:
		/// Store
		std::array<_Ty, 3> store_;
	};

	/// MATH: bvec3_t
	using bvec3_t
	pf_alignas(bool) = vector<bool, 3>;

	/// MATH: uvec3_t
	using u8vec3_t pf_alignas(uint8_t)	 = vector<uint8_t, 3>;
	using u16vec3_t pf_alignas(uint16_t) = vector<uint16_t, 3>;
	using u32vec3_t pf_alignas(uint32_t) = vector<uint32_t, 3>;
	using u64vec3_t pf_alignas(uint64_t) = vector<uint64_t, 3>;

	/// MATH: ivec3_t
	using i8vec3_t pf_alignas(int8_t)		= vector<int8_t, 3>;
	using i16vec3_t pf_alignas(int16_t) = vector<int16_t, 3>;
	using i32vec3_t pf_alignas(int32_t) = vector<int32_t, 3>;
	using i64vec3_t pf_alignas(int64_t) = vector<int64_t, 3>;

	/// MATH: fvec3_t
	using f32vec3_t pf_alignas(float32_t) = vector<float32_t, 3>;
	using f64vec3_t pf_alignas(float64_t) = vector<float64_t, 3>;
}

// Anonymous structs
#pragma GCC diagnostic pop

#endif // !PULSAR_MATH_VECTOR3_INL