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
			// Fnv1-a
			/**
			 * @category Fnv1-a 32 bits version
			 */
			/**
			 * @brief Hash calculation function of the Fnv1-a function. Returns a 32-bit unsigned integer
			 * 				representing the hash of the Fnv1-a function from the memory pointed to by "p", of
			 * 				size "sizeInBytes".
			 *
			 * @warning This hash function should not be used to secure data.
			 *
			 * @param (const void*) 	 p  				 Pointer to the beginning of a continuous data
			 * 																		 structure
			 * @param (pulsar::size_t) sizeInBytes Size of this data structure
			 * @return (pulsar::uint32_t) Numerical representation of the function Fnv1-a of the data
			 * 														structure pointed by "p", of length "sizeInBytes"
			 */
			pf_hint_nodiscard pf_decl_constexpr uint32_t hash32(const void *p,
																													size_t sizeInBytes) pf_attr_noexcept
			{
				uint32_t val = 0x811c9dc5u;
				union
				{
					const byte_t *as_byte = nullptr;
					const void *as_void;
				};

				as_void = p;

				for (size_t i = 0; i < sizeInBytes; ++i)
				{
					val = (val ^ *as_byte) * 0x01000193u;
					as_byte++;
				}

				return val;
			}
			/**
			 * @see hash32
			 */
			template <typename _Ty, size_t _Len>
			pf_hint_nodiscard pf_decl_constexpr uint32_t hash32(const _Ty (&arr)[_Len])
					pf_attr_noexcept
			{
				return hash32(&arr[0], _Len * sizeof(_Ty));
			}

			/**
			 * @category Fnv-1a 64 bits version
			 */
			/**
			 * @brief Hash calculation function of the Fnv1-a function. Returns a 64-bit unsigned integer
			 * 				representing the hash of the Fnv1-a function from the memory pointed to by "p", of
			 * 				size "sizeInBytes".
			 *
			 * @see hash32
			 */
			pf_hint_nodiscard pf_decl_constexpr uint64_t hash64(const void *p,
																													size_t sizeInBytes) pf_attr_noexcept
			{
				uint64_t val = 0xcbf29ce484222325ull;

				union
				{
					const byte_t *as_byte = nullptr;
					const void *as_void;
				};

				as_void = p;

				for (size_t i = 0; i < sizeInBytes; ++i)
				{
					val = (val ^ *as_byte) * 0xcbf29ce484222325ull;
					as_byte++;
				}

				return val;
			}
			/**
			 * @see hash64
			 */
			template <typename _Ty, size_t _Len>
			pf_hint_nodiscard pf_decl_constexpr uint64_t hash64(const _Ty (&arr)[_Len])
					pf_attr_noexcept
			{
				return hash64(&arr[0], _Len * sizeof(_Ty));
			}
		} // Fnv1-a

	} // Hash

} // Pulsar
#endif // !PULSAR_MATHEMATICS_HASH_FNV1A_HPP