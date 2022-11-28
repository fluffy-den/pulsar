/*! @file   unordered_dense.hpp
 *  @author Fluffy (noe.louis-quentin@hotmail.fr)
 *  @brief
 *  @date   03-09-2022
 *
 *  @copyright Copyright (c) 2022 - Pulsar Software
 *
 *  @since 0.1.2
 */

#ifndef PULSAR_UNORDERED_DENSE_HPP
#define PULSAR_UNORDERED_DENSE_HPP 1

// Include: Pulsar
#include "pulsar/hash.hpp"
#include "pulsar/memory.hpp"
#include "pulsar/tuple.hpp"

// Include: Pulsar -> Container
#include "pulsar/container/sequence.hpp"

// Include: C++
#include <memory>
#include <utility>

// Pulsar
namespace pul
{
	// https://github.com/martinus/unordered_dense/blob/main/include/ankerl/unordered_dense.h

	/// UNORDERED_BUCKET:
	class unordered_bucket_t
	{};

	/// UNORDERED-DENSE:
	template <
		typename _KeyTy,
		typename _Ty,
		typename _Hasher,
		typename _KeyComp,
		typename _Magnifier,
		typename _Allocator,
		typename _Bucket>
	class unordered_dense
	{
	public:
		using key_t						 = _KeyTy;
		using value_t					 = _Ty;
		using hasher_t				 = _Hasher;
		using key_comparator_t = _KeyComp;
		using magnifier_t			 = _Magnifier;
		using allocator_t			 = _Allocator;
		using container_t			 = forward_sequence<_Ty, _Magnifier, _Allocator>;
		using bucket_t				 = _Bucket;

	private:
		container_t values_;
		// buckets_
		size_t numBuckets_;
		size_t maxBucketCapacity_;
		float32_t maxLoadFactor = default_max_load_factor;
		_Hasher hash_;
		_KeyComp comp_;
		uint8_t initialShifts_;
		// hash_map
	};

	/// UNORDERED-MAP:
	template <
		typename _KeyTy,
		typename _Ty,
		typename _Hasher    = xxhash64<_KeyTy>,
		typename _KeyComp   = std::equal_to<_KeyTy>,
		typename _Allocator = std::allocator<pair<_KeyTy, _Ty>>,
		typename _Bucket    = unordered_bucket_t>
	using unordered_map = unordered_dense<
		_KeyTy,
		_Ty,
		_Hasher,
		_KeyComp,
		_Allocator,
		_Bucket>;

	/// UNORDERED-SET:
	template <
		typename _KeyTy,
		typename _Hasher    = xxhash64<_KeyTy>,
		typename _KeyComp   = std::equal_to<_KeyTy>,
		typename _Allocator = std::allocator<_KeyTy>,
		typename _Bucket    = unordered_bucket_t>
	using unordered_set = unordered_dense<
		_KeyTy,
		void,
		_Hasher,
		_KeyComp,
		_Allocator,
		_Bucket>;
}

#endif // !PULSAR_UNORDERED_DENSE_HPP