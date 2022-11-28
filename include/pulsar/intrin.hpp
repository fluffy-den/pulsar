/*! @file   intrin.hpp
 *  @author Fluffy (noe.louis-quentin@hotmail.fr)
 *  @brief  Implementation of intrin utilitaries.
 *  @date   14-08-2022
 *
 *  @copyright Copyright (c) 2022 - Pulsar Software
 *
 *  @since 0.1.2
 */

#ifndef PULSAR_INTRIN_HPP
#define PULSAR_INTRIN_HPP 1

// Include: Pulsar
#include "pulsar/pulsar.hpp"

// Include: C++
#include <algorithm>
#include <type_traits>

// Include: C
#include <intrin.h>

// Pulsar
namespace pul
{

	/*! @brief Computes best alignment (SIMD or not) for type.
	 *
	 *  @tparam _Ty  Type
	 *  @tparam _Num Number of type.
	 */
	template <typename _Ty, size_t _Num>
	class simd_alignment_of
	{
		pf_decl_static pf_decl_constexpr size_t value = alignof(_Ty);
	};
	/// SIMD: Alignment
	/*! @brief Determines the required alignment of @a _Num of @a _Ty to use SIMD.
	 *
	 *  @tparam _Ty		Type to be SIMD aligned.
	 *  @tparam _Num	Number to be SIMD aligned.
	 *  @tparam _SIMD	(optional) Activate SIMD alignment. Default to SIMD_ALIGNED.
	 */
	template <typename _Ty, size_t _Num>
	requires(_Num > 1)
	class simd_alignment_of<_Ty, _Num>// TODO: convert to s_alignof()
	{
	private:
		pf_hint_nodiscard pf_decl_static pf_decl_constexpr size_t __calc_align() pf_attr_noexcept
		{
			size_t s = _Num * alignof(_Ty);
			if(s < 8 || (s & (s - 1)))
				return alignof(_Ty);
			return std::min<size_t>(std::max<size_t>(s, 8), 64);
		}

	public:
		pf_decl_static pf_decl_constexpr size_t value = __calc_align();
	};
	template <typename _Ty, size_t _Num>
	pf_decl_static pf_decl_constexpr size_t simd_alignment_of_v = simd_alignment_of<_Ty, _Num>::value;

	/// SIMD: Alignable
	/*! @brief Checks if @a _Ty of number @a _Num is SIMD alignable.
	 *
	 *  @tparam _Ty  Type to be SIMD aligned.
	 *  @tparam _Num Number to be SIMD aligned.
	 */
	template <typename _Ty, size_t _Num>
	struct is_simd_alignable : public std::integral_constant<
			bool,
			(alignof(_Ty) * _Num & (alignof(_Ty) * _Num - 1)) == 0 && (alignof(_Ty) * _Num >= 8)>
	{};
	template <typename _Ty, size_t _Num>
	pf_decl_static pf_decl_constexpr bool is_simd_alignable_v = is_simd_alignable<_Ty, _Num>::value;
}

/// ALIGNAS: N
#define pf_alignas_n(type, num) alignas(pul::simd_alignment_of_v<type, num>)

#endif // !PULSAR_INTRIN_HPP