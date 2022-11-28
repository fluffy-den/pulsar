/*! @file   xxhash.hpp
 *  @author Fluffy (noe.louis-quentin@hotmail.fr)
 *  @brief
 *  @date   10-09-2022
 *
 *  @copyright Copyright (c) 2022 - Pulsar Software
 *
 *  @since 0.1.2
 */

// https://github.com/Cyan4973/xxHash

#ifndef PULSAR_XXHASH_HPP
#define PULSAR_XXHASH_HPP 1

// Include: Pulsar
#include "pulsar/pulsar.hpp"

// Pulsar
namespace pul
{
	// TODO: xxhash

	// xx
	namespace xx
	{
		// 32 bits
		using hash32_t = uint32_t;

		pf_decl_inline pf_decl_constexpr hash32_t hash32(
				const void *__in,
				size_t __len)
		{
			// TODO: xxhash32
			return 0;
		}
		/*! @brief
		 *
		 *  @tparam _Ty
		 *  @tparam _Num
		 *  @param[in] __str
		 *  @return pf_decl_constexpr
		 */
		template <typename _Ty, size_t _Num>
		pf_decl_inline pf_decl_constexpr hash32_t hash32(
				const _Ty (&__str)[_Num])
		{
			return hash32(&__str[0], _Num);
		}


		// 64 bits
		using hash64_t = uint64_t;

		/*! @brief
		 *
		 *  @param[in] __in
		 *  @param[in] __len
		 *  @return pf_decl_constexpr
		 */
		pf_decl_inline pf_decl_constexpr hash64_t hash64(
				const void *__in,
				size_t __len)
		{
			// TODO: xxhash64
			return 0;
		}
		/*! @brief
		 *
		 *  @tparam _Ty
		 *  @tparam _Num
		 *  @param[in] __str
		 *  @return pf_decl_constexpr
		 */
		template <typename _Ty, size_t _Num>
		pf_decl_inline pf_decl_constexpr hash64_t hash64(
				const _Ty (&__str)[_Num])
		{
			return hash64(&__str[0], _Num);
		}

		// 128 bits
		struct hash128_t
		{
			hash64_t hi;
			hash64_t lo;

			/// Operator==
			/// Operator<=>
		};

		/*! @brief
		 *
		 *  @param[in] __in
		 *  @param[in] __len
		 *  @return pf_decl_constexpr
		 */
		pf_decl_inline pf_decl_constexpr hash128_t hash128(
				const void *__in,
				size_t __len)
		{
			// TODO: xxh128
			return { 0, 0 };
		}
		/*! @brief
		 *
		 *  @tparam _Ty
		 *  @tparam _Num
		 *  @param[in] __str
		 *  @return pf_decl_constexpr
		 */
		template <typename _Ty, size_t _Num>
		pf_decl_inline pf_decl_constexpr hash128_t hash128(
				const _Ty (&__str)[_Num])
		{
			return hash128(&__str[0], _Num);
		}

		// auto bits
		/*! @brief
		 *
		 *  @param[in] __in
		 *  @param[in] __len
		 *  @return pf_decl_constexpr
		 */
		pf_decl_inline pf_decl_constexpr size_t hash(
				const void *__in,
				size_t __len)
		{
#ifdef PF_32BIT
			return hash32(__in, __len);
#else	 // ^^^ PF_32BIT ^^^ / vvv PF_64BIT vvv
			return hash64(__in, __len);
#endif // !PF_32BIT
		}
		/*! @brief
		 *
		 *  @tparam _Ty
		 *  @tparam _Num
		 *  @return pf_decl_constexpr
		 */
		template <typename _Ty, size_t _Num>
		pf_decl_inline pf_decl_constexpr size_t hash(
				const _Ty (&__str)[_Num])
		{
			return hash(&__str[0], _Num);
		}
	}

	/// XXHASH: 32
	template <typename _Ty>
	struct xxhash32
	{
		pf_decl_inline pf_decl_constexpr xx::hash32_t operator()(
				_Ty const &__val)
		{
			return xx::hash32(&__val, sizeof(_Ty));
		}
	};

	/// XXHASH: 64
	template <typename _Ty>
	struct xxhash64
	{
		pf_decl_inline pf_decl_constexpr xx::hash64_t operator()(
				_Ty const &__val)
		{
			return xx::hash64(&__val, sizeof(_Ty));
		}
	};

	/// XXHASH: 128
	template <typename _Ty>
	struct xxhash128
	{
		pf_decl_inline pf_decl_constexpr xx::hash128_t operator()(
				_Ty const &__val)
		{
			return xx::hash128(&__val, sizeof(_Ty));
		}
	};
}

#endif // !PULSAR_XXHASH_HPP