/*! @file   fnv1a.inl
 *  @author Fluffy (noe.louis-quentin@hotmail.fr)
 *  @brief  Definition of Fnv1-a hash function.
 *  @date   12-08-2022
 *
 *  @copyright Copyright (c) 2022 - Pulsar Software
 *
 *  @since 0.1.2
 */

#ifndef PULSAR_HASH_FNV1A_INL
#define PULSAR_HASH_FNV1A_INL 1

// Include: Pulsar -> Hash
#include "pulsar/hash/fnv1a/fnv1a.hpp"

// Pulsar
namespace pul
{
	// Hash
	namespace hash
	{
		// Fnv1-a
		namespace fnv1a
		{
			/// 32 bits
			pf_hint_nodiscard pf_decl_constexpr uint32_t hash32(
					const void *__ptr,
					size_t __size) pf_attr_noexcept
			{
				uint32_t val = 0x811c9dc5u;
				union
				{
					const byte_t *as_byte;
					const void *as_void;
				};

				as_void = __ptr;

				for (size_t i = 0; i < __size; ++i)
				{
					val = (val ^ *as_byte) * 0x01000193u;
					as_byte++;
				}

				return val;
			}
			template <typename _Ty, size_t _Len>
			pf_hint_nodiscard pf_decl_inline pf_decl_constexpr uint32_t hash32(
					const _Ty (&__arr)[_Len]) pf_attr_noexcept
			{
				return hash32(&__arr[0], _Len * sizeof(_Ty));
			}

			/// 64 bits
			pf_hint_nodiscard pf_decl_constexpr uint64_t hash64(
					const void *__ptr,
					size_t __size) pf_attr_noexcept
			{
				uint64_t val = 0xcbf29ce484222325ull;

				union
				{
					const byte_t *as_byte;
					const void *as_void;
				};

				as_void = __ptr;

				for (size_t i = 0; i < __size; ++i)
				{
					val = (val ^ *as_byte) * 0xcbf29ce484222325ull;
					as_byte++;
				}

				return val;
			}
			template <typename _Ty, size_t _Len>
			pf_hint_nodiscard pf_decl_inline pf_decl_constexpr uint64_t hash64(
					const _Ty (&__arr)[_Len]) pf_attr_noexcept
			{
				return hash64(&__arr[0], _Len * sizeof(_Ty));
			}

			/// Auto
			pf_hint_nodiscard pf_decl_inline pf_decl_constexpr size_t hash(
					const void *__ptr,
					size_t __size) pf_attr_noexcept
			{
#ifdef PF_64BIT
				return hash64(__ptr, __size);
#else	 // ^^^ PF_64BIT ^^^ / vvv PF_32BIT vvv
				return hash32(__ptr, __size);
#endif // PF_64BIT
			}
			template <typename _Ty, size_t _Len>
			pf_hint_nodiscard pf_decl_inline pf_decl_constexpr size_t hash(
					const _Ty (&__arr)[_Len]) pf_attr_noexcept
			{
#ifdef PF_64BIT
				return hash64(__arr);
#else	 // ^^^ PF_64BIT ^^^ / vvv PF_32BIT vvv
				return hash32(__arr);
#endif // PF_64BIT
			}
		}
	}

	/// STD: Fnv1a impl -> 32 bits
	template <typename _Key>
	pf_hint_nodiscard pf_decl_inline pf_decl_constexpr uint32_t hash32_fnv1a<_Key>::operator()(
			_Key __key) const pf_attr_noexcept
	{
		return hash::fnv1a::hash32(&__key, sizeof(__key));
	}

	/// STD: Fnv1a impl -> 64 bits
	template <typename _Key>
	pf_hint_nodiscard pf_decl_inline pf_decl_constexpr uint64_t hash64_fnv1a<_Key>::operator()(
			_Key __key) const pf_attr_noexcept
	{
		return hash::fnv1a::hash64(&__key, sizeof(__key));
	}
}

#endif // !PULSAR_HASH_FNV1A_INL