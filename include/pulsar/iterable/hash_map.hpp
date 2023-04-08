/*! @file   hash.hpp
 *  @author Louis-Quentin Noé (noe.louis-quentin@hotmail.fr)
 *  @brief
 *  @date   05-03-2023
 *
 *  @copyright Copyright (c) 2023 - Pulsar Software
 *
 *  @since 0.1.4
 */

#ifndef PULSAR_HASH_HPP
#define PULSAR_HASH_HPP 1

// Include: Pulsar
#include "pulsar/pulsar.hpp"

// Pulsar
namespace pul
{
	/// HASH: Concept
	template<typename _HashTy>
	concept __hash32_c = requires(
	 _HashTy &__hash) {
		{
			__hash.operator()(std::declval<uint32_t>())
		} -> std::convertible_to<uint32_t>;
	};
	template<typename _HashTy>
	concept __hash64_c = requires(
	 _HashTy &__hash) {
		{
			__hash.operator()(std::declval<uint64_t>())
		} -> std::convertible_to<uint64_t>;
	};
	template<typename _HashTy>
	concept __hashz_c = __hash32_c<_HashTy> || __hash64_c<_HashTy>;
	template<typename _HashTy>
	struct is_hash : std::false_type
	{};
	template<__hashz_c _HashTy>
	struct is_hash<_HashTy> : std::true_type
	{};
	template<typename _HashTy>
	pf_decl_static pf_decl_constexpr bool is_hash_v = is_hash<_HashTy>::value;
}	 // namespace pul

// Include: Pulsar -> Hash
#include "pulsar/hash/xxhash.hpp"
#include "pulsar/hash/murmurhash3.hpp"
#include "pulsar/hash/sha.hpp"

#endif	// !PULSAR_HASH_HPP
