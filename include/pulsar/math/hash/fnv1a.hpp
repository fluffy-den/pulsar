/**
 * @file    fnv1a.hpp
 * @author  Fluffy (noe.louis-quentin@hotmail.fr)
 * @brief
 * @date    26-11-2021
 *
 * @copyright Copyright (c) 2021 - Pulsar Software
 *
 */

#ifndef PULSAR_MATHEMATICS_HASH_FNV1A_HPP
#define PULSAR_MATHEMATICS_HASH_FNV1A_HPP 1

// Include: Pulsar
#include "pulsar/pulsar.hpp"

// Pulsar
namespace pulsar
{
	// Hash
	namespace hash
	{
		// Fnv1-a
		namespace fnv1a
		{
			/// 32 bits
			/*! @brief Calculates the 32 bits version of the fnv1-a hash function.
			 *
			 *  @param __p		Pointer to the beginning of a continuous data structure.
			 *  @param __size Size of this data structure in bytes.
			 *  @return The hashed data pointed by @a __ptr of size @a __size in bytes.
			 */
			pf_hint_nodiscard pf_decl_constexpr uint32_t hash32(const void *__ptr,
																													size_t __size) pf_attr_noexcept
			{
				uint32_t val = 0x811c9dc5u;
				union
				{
					const byte_t *as_byte = nullptr;
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
			/*! @brief Does the same thing as hash32 function.
			 *
			 *  @tparam _Ty  Type of @a __arr array.
			 *  @tparam _Len Len of @a __arr array.
			 *  @param __arr Reference to an array structure.
			 */
			template <typename _Ty, size_t _Len>
			pf_hint_nodiscard pf_decl_constexpr uint32_t hash32(const _Ty (&__arr)[_Len])
					pf_attr_noexcept
			{
				return hash32(&__arr[0], _Len * sizeof(_Ty));
			}

			/// 64 bits
			/*! @brief Calculates the 64 bits version of the fnv1-a hash function.
			 * 				 This function does same as the hash32 function.
			 *
			 *  @see hash32.
			 */
			pf_hint_nodiscard pf_decl_constexpr uint64_t hash64(const void *__ptr,
																													size_t __size) pf_attr_noexcept
			{
				uint64_t val = 0xcbf29ce484222325ull;

				union
				{
					const byte_t *as_byte = nullptr;
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
			/*! @see hash64.
			 */
			template <typename _Ty, size_t _Len>
			pf_hint_nodiscard pf_decl_constexpr uint64_t hash64(const _Ty (&__arr)[_Len])
					pf_attr_noexcept
			{
				return hash64(&__arr[0], _Len * sizeof(_Ty));
			}

		} // Fnv1-a

	} // Hash

} // Pulsar
#endif // !PULSAR_MATHEMATICS_HASH_FNV1A_HPP