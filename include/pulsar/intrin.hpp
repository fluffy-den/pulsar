/*! @file   intrin.hpp
 *  @author Fluffy (noe.louis-quentin@hotmail.fr)
 *  @brief
 *  @date   17-05-2022
 *
 *  @copyright Copyright (c) 2022 - Pulsar Software
 *
 *  @since 0.1.1
 */

// Include: Pulsar
#include "pulsar.hpp"

// Include: C++
#include <algorithm>
#include <type_traits>

// Include: C
#include <intrin.h>

// Pulsar
namespace pul
{

	/// SIMD: Types
	using m64_t		= __m64;
	using m128_t	= __m128;
	using m128i_t = __m128i;
	using m128d_t = __m128d;
	using m256_t	= __m256;
	using m256i_t = __m256i;
	using m256d_t = __m256d;

	/// SIMD: Alignment
	/*! @brief
	 *
	 *  @param[in] __elemsize
	 *  @param[in] __num
	 *  @return pf_decl_constexpr
	 */
	pf_hint_nodiscard pf_decl_constexpr align_val_t simd_alignof(
			size_t __elemsize,
			size_t __num) pf_attr_noexcept
	{
		if (__num > 4)
			__num = 4;
		__elemsize *= __num;
		while (__elemsize & (__elemsize - 1))
		{
			__elemsize = __elemsize & (__elemsize - 1);
		}
		return align_val_t(std::max<size_t>(__elemsize, 16));
	}

	/// REGISTER: Selector
	template <typename _Ty, size_t _Num>
	class register_pack
	{};

	// -> _Num = 1
	template <typename _Ty>
	class register_pack<_Ty, 1>
	{
		using type = _Ty;
	};

	// -> _Num = 2
	template <typename _Ty>
	class register_pack<_Ty, 2>
	{
		using type = m64_t;
	};
	template <>
	class register_pack<int64_t, 2>
	{
		using type = m128i_t;
	};
	template <>
	class register_pack<uint64_t, 2>
	{
		using type = m128i_t;
	};
	template <>
	class register_pack<float64_t, 2>
	{
		using type = m128d_t;
	};

	// -> _Num = 3
	template <typename _Ty>
	class register_pack<_Ty, 3>
	{
		using type = m64_t;
	};
	template <>
	class register_pack<int32_t, 3>
	{
		using type = m128_t;
	};
	template <>
	class register_pack<int64_t, 3>
	{
		using type = m256i_t;
	};
	template <>
	class register_pack<uint32_t, 3>
	{
		using type = m128_t;
	};
	template <>
	class register_pack<uint64_t, 3>
	{
		using type = m256i_t;
	};
	template <>
	class register_pack<float32_t, 3>
	{
		using type = m128_t;
	};
	template <>
	class register_pack<float64_t, 3>
	{
		using type = m256d_t;
	};

	// -> _Num = 4
	template <typename _Ty>
	class register_pack<_Ty, 4>
	{
		using type = m64_t;
	};
	template <>
	class register_pack<int32_t, 4>
	{
		using type = m128i_t;
	};
	template <>
	class register_pack<int64_t, 4>
	{
		using type = m256i_t;
	};
	template <>
	class register_pack<uint32_t, 4>
	{
		using type = m128i_t;
	};
	template <>
	class register_pack<uint64_t, 4>
	{
		using type = m256i_t;
	};
	template <>
	class register_pack<float32_t, 4>
	{
		using type = m128_t;
	};
	template <>
	class register_pack<float64_t, 4>
	{
		using type = m256d_t;
	};

	// -> Type
	template <typename _Ty, size_t _Num>
	using register_pack_t = typename register_pack<_Ty, _Num>::type;

	/// REGISTER: Cast
	template <typename _RegTy, typename _Ty>
	class register_cast;
	// -> m64_t
	template <typename _Ty>
	class register_cast<m64_t, _Ty>
	{
		using type = m64_t;
	};
	// -> m128_t
	template <typename _Ty>
		requires(!std::is_integral_v<_Ty>)
	class register_cast<m128_t, _Ty>
	{
		using type = m128_t;
	};
	template <typename _Ty>
		requires(!std::is_integral_v<_Ty>)
	class register_cast<m128i_t, _Ty>
	{
		using type = m128_t;
	};
	template <typename _Ty>
		requires(!std::is_integral_v<_Ty>)
	class register_cast<m128d_t, _Ty>
	{
		using type = m128_t;
	};
	template <typename _Ty>
		requires(std::is_integral_v<_Ty>)
	class register_cast<m128_t, _Ty>
	{
		using type = m128i_t;
	};
	template <typename _Ty>
		requires(std::is_integral_v<_Ty>)
	class register_cast<m128i_t, _Ty>
	{
		using type = m128i_t;
	};
	template <typename _Ty>
		requires(std::is_integral_v<_Ty>)
	class register_cast<m128d_t, _Ty>
	{
		using type = m128i_t;
	};
	template <>
	class register_cast<m128_t, float64_t>
	{
		using type = m128d_t;
	};
	template <>
	class register_cast<m128i_t, float64_t>
	{
		using type = m128d_t;
	};
	template <>
	class register_cast<m128d_t, float64_t>
	{
		using type = m128d_t;
	};
	// -> m256_t
	template <typename _Ty>
		requires(!std::is_integral_v<_Ty>)
	class register_cast<m256_t, _Ty>
	{
		using type = m256_t;
	};
	template <typename _Ty>
		requires(!std::is_integral_v<_Ty>)
	class register_cast<m256i_t, _Ty>
	{
		using type = m256_t;
	};
	template <typename _Ty>
		requires(!std::is_integral_v<_Ty>)
	class register_cast<m256d_t, _Ty>
	{
		using type = m256_t;
	};
	template <typename _Ty>
		requires(std::is_integral_v<_Ty>)
	class register_cast<m256_t, _Ty>
	{
		using type = m256i_t;
	};
	template <typename _Ty>
		requires(std::is_integral_v<_Ty>)
	class register_cast<m256i_t, _Ty>
	{
		using type = m256i_t;
	};
	template <typename _Ty>
		requires(std::is_integral_v<_Ty>)
	class register_cast<m256d_t, _Ty>
	{
		using type = m256i_t;
	};
	template <>
	class register_cast<m256_t, float64_t>
	{
		using type = m256d_t;
	};
	template <>
	class register_cast<m256i_t, float64_t>
	{
		using type = m256d_t;
	};
	template <>
	class register_cast<m256d_t, float64_t>
	{
		using type = m256d_t;
	};
	// -> Type
	template <typename _RegTy, typename _Ty>
	using register_cast_t = typename register_cast<_RegTy, _Ty>::type;
}