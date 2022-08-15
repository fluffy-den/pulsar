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

	/// SIMD: Align Type
	enum simd_align_t : bool
	{
		SIMD_UNALIGNED,
		SIMD_ALIGNED
	};
	template <typename _Ty, size_t _Num, simd_align_t _SIMD = SIMD_ALIGNED>
	class simd_alignment_of;
	/// SIMD: Alignment
	/*! @brief Determines the required alignment of @a _Num of @a _Ty to use SIMD.
	 *
	 *  @tparam _Ty		Type to be SIMD aligned.
	 *  @tparam _Num	Number to be SIMD aligned.
	 *  @tparam _SIMD	(optional) Activate SIMD alignment. Default to SIMD_ALIGNED.
	 */
	template <typename _Ty, size_t _Num, simd_align_t _SIMD>
		requires(_Num > 1 && _SIMD == SIMD_ALIGNED)
	class simd_alignment_of<_Ty, _Num, _SIMD>
	{
	private:
		pf_hint_nodiscard pf_decl_static pf_decl_constexpr size_t __cal_align() pf_attr_noexcept
		{
			size_t s = _Num * alignof(_Ty);
			if (s < 8 || (s & (s - 1)))
				return alignof(_Ty);
			return std::min<size_t>(std::max<size_t>(s, 8), 64);
		}

	public:
		pf_decl_static pf_decl_constexpr size_t value = __cal_align();
	};
	template <typename _Ty, size_t _Num, simd_align_t _SIMD>
		requires(_Num == 1 || _SIMD == SIMD_UNALIGNED)
	class simd_alignment_of<_Ty, _Num, _SIMD>
	{
	public:
		pf_decl_static pf_decl_constexpr size_t value = alignof(_Ty);
	};
	template <typename _Ty, size_t _Num, simd_align_t _SIMD>
	pf_decl_static pf_decl_constexpr size_t simd_alignment_of_v = simd_alignment_of<_Ty, _Num, _SIMD>::value;

	/// SIMD: Alignable
	/*! @brief Checks if @a _Ty of number @a _Num is SIMD alignable.
	 *
	 *  @tparam _Ty  Type to be SIMD aligned.
	 *  @tparam _Num Number to be SIMD aligned.
	 */
	template <typename _Ty, size_t _Num>
	struct is_simd_alignable: public std::integral_constant<bool, (alignof(_Ty) * _Num & (alignof(_Ty) * _Num - 1)) == 0 && (alignof(_Ty) * _Num >= 8)>
	{};
	template <typename _Ty, size_t _Num>
	pf_decl_static pf_decl_constexpr bool is_simd_alignable_v = is_simd_alignable<_Ty, _Num>::value;

	/// SIMD: Select
	/*! @brief Select best alignment (SIMD or not) for @a _Ty and @a _Num.
	 *
	 *  @tparam _Ty  Type to be SIMD aligned.
	 *  @tparam _Num Number to be SIMD aligned.
	 */
	template <typename _Ty, size_t _Num>
	struct simd_select: public std::integral_constant<simd_align_t, SIMD_UNALIGNED>
	{};
	template <typename _Ty, size_t _Num>
		requires(simd_alignment_of_v<_Ty, _Num, SIMD_ALIGNED> != alignof(_Ty))
	struct simd_select<_Ty, _Num>: public std::integral_constant<simd_align_t, SIMD_ALIGNED>
	{};
	template <typename _Ty, size_t _Num>
	pf_decl_static pf_decl_constexpr simd_align_t simd_select_v = simd_select<_Ty, _Num>::value;

}

#endif // !PULSAR_INTRIN_HPP