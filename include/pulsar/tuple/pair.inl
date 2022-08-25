/*! @file   pair.inl
 *  @author Fluffy (noe.louis-quentin@hotmail.fr)
 *  @brief  Implementation of pair.
 *  @date   24-08-2022
 *
 *  @copyright Copyright (c) 2022 - Pulsar Software
 *
 *  @since 0.1.2
 */

#ifndef PULSAR_PAIR_INL
#define PULSAR_PAIR_INL 1

// Include: Pulsar -> Tuple
#include "pulsar/tuple.hpp"

// Pulsar
namespace pul
{
	// Pair -> Constructors
	template <typename _TyA, typename _TyB>
	pf_decl_constexpr pair<_TyA, _TyB>::pair(
			_TyA &&__a,
			_TyB &&__b) pf_attr_noexcept
			: a(std::forward<_TyA>(__a))
			, b(std::forward<_TyB>(__b))
	{}
	template <typename _TyA, typename _TyB>
	template <typename _InTyA, typename _InTyB>
	pf_decl_constexpr pair<_TyA, _TyB>::pair(
			_InTyA &&__a,
			_InTyB &&__b) pf_attr_noexcept
			requires(std::is_convertible_v<_InTyA, _TyA> &&std::is_convertible_v<_InTyB, _TyB>)
			: a(std::forward<_InTyA>(__a))
			, b(std::forward<_InTyB>(__b))
	{}

	// Pair -> Maker
	template <typename _TyA, typename _TyB>
	pf_decl_constexpr pair<_TyA, _TyB> make_pair(
			_TyA &&__a,
			_TyB &&__b) pf_attr_noexcept
	{
		return pair(std::forward<_TyA>(__a), std::forward<_TyB>(__b));
	}
}

#endif // !PULSAR_PAIR_INL