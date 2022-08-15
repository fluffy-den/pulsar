/*! @file   fnv1a.hpp
 *  @author Fluffy (noe.louis-quentin@hotmail.fr)
 *  @brief	Definition of Fnv1-a hash function.
 *  @date   26-11-2021
 *
 *  @copyright Copyright (c) 2021 - Pulsar Software
 *
 *  @since 0.1.1
 */

#ifndef PULSAR_HASH_FNV1A_HPP
#define PULSAR_HASH_FNV1A_HPP 1

// Include: Pulsar
#include "pulsar/pulsar.hpp"

// Include: C++
#include <functional>

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
			/*! @brief Calculates the 32 bits version of the fnv1-a hash function.
			 *
			 *  @param __p		Pointer to the beginning of a continuous data
			 * structure.
			 *  @param __size Size of this data structure in bytes.
			 *  @return The hashed data pointed by @a __ptr of size @a __size in
			 * bytes.
			 */
			pf_hint_nodiscard pf_decl_constexpr uint32_t hash32(
					const void *__ptr,
					size_t __size) pf_attr_noexcept;
			/*! @brief Does the same thing as hash32 function.
			 *
			 *  @tparam _Ty  Type of @a __arr array.
			 *  @tparam _Len Len of @a __arr array.
			 *  @param __arr Reference to an array structure.
			 */
			template <typename _Ty, size_t _Len>
			pf_hint_nodiscard pf_decl_inline pf_decl_constexpr uint32_t hash32(
					const _Ty (&__arr)[_Len]) pf_attr_noexcept;

			/// 64 bits
			/*! @brief Calculates the 64 bits version of the fnv1-a hash function.
			 * 				 This function does same as the hash32 function.
			 *
			 *  @see hash32.
			 */
			pf_hint_nodiscard pf_decl_constexpr uint64_t hash64(
					const void *__ptr,
					size_t __size) pf_attr_noexcept;
			/*! @see hash64.
			 */
			template <typename _Ty, size_t _Len>
			pf_hint_nodiscard pf_decl_inline pf_decl_constexpr uint64_t hash64(
					const _Ty (&__arr)[_Len]) pf_attr_noexcept;

			/// Auto
			pf_hint_nodiscard pf_decl_inline pf_decl_constexpr size_t hash(
					const void *__ptr,
					size_t __size) pf_attr_noexcept;
			template <typename _Ty, size_t _Len>
			pf_hint_nodiscard pf_decl_inline pf_decl_constexpr size_t hash(
					const _Ty (&__arr)[_Len]) pf_attr_noexcept;

		} // Fnv1-a

	} // Hash

	/// STD: Fnv1a impl -> 32 bits
	/*! @brief Fnv1a 32 bits functor.
	 *
	 *  @tparam _Key Type of the value to be hashed.
	 */
	template <typename _Key>
	class hash32_fnv1a: public std::hash<_Key>
	{
	public:
		/// Operator()
		/*! @brief Applies Fnv1a on @a __key.
		 *
		 *  @param[in] __key Value to be hashed.
		 *  @return Hashed value.
		 */
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr uint32_t operator()(
				_Key __key) const pf_attr_noexcept;
	};

	/// STD: Fnv1a impl -> 64 bits
	/*! @brief Fnv1a 64 bits functor.
	 *
	 *  @tparam _Key Type of the value to be hashed.
	 */
	template <typename _Key>
	class hash64_fnv1a: public std::hash<_Key>
	{
	public:
		/// Operator()
		/*! @brief Applies Fnv1a on @a __key.
		 *
		 *  @param[in] __key Value to be hashed.
		 *  @return Hashed value.
		 */
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr uint64_t operator()(
				_Key __key) const pf_attr_noexcept;
	};

	/// STD: Fnv1a impl -> Auto
	template <typename _Key>
#ifdef PF_64BIT
	using hash_fnv1a = hash64_fnv1a<_Key>;
#else	 // ^^^ PF_64BIT ^^^ / vvv PF_32BIT vvv
	using hash_fnv1a = hash32_fnv1a<_Key>;
#endif // PF_64BIT

} // Pulsar

// Include: Pulsar -> Hash -> Fnv1a Impl
#include "pulsar/hash/fnv1a/fnv1a.inl"

#endif // !PULSAR_HASH_FNV1A_HPP