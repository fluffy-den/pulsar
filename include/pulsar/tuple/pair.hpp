/*! @file   pair.hpp
 *  @author Fluffy (noe.louis-quentin@hotmail.fr)
 *  @brief  Definition of pair.
 *  @date   24-08-2022
 *
 *  @copyright Copyright (c) 2022 - Pulsar Software
 *
 *  @since 0.1.2
 */

#ifndef PULSAR_PAIR_HPP
#define PULSAR_PAIR_HPP 1

// Include: Pulsar
#include "pulsar/pulsar.hpp"

// Anonymous structs -> Disable warning
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"

// Pulsar
namespace pul
{
	/// PAIR: Impl
	template <typename _TyA, typename _TyB>
	union pair
	{
		/// Constructors
		pf_decl_inline pf_decl_constexpr pair() pf_attr_noexcept = default;
		pf_decl_inline pf_decl_constexpr pair(_TyA &&__a, _TyB &&__b) pf_attr_noexcept;
		template <typename _InTyA, typename _InTyB>
		pf_decl_inline pf_decl_constexpr pair(_InTyA &&__a, _InTyB &&__b) pf_attr_noexcept
				requires(std::is_convertible_v<_InTyA, _TyA> &&std::is_convertible_v<_InTyB, _TyB>);
		pf_decl_inline pf_decl_constexpr pair(pair<_TyA, _TyB> const &) pf_attr_noexcept = default;
		pf_decl_inline pf_decl_constexpr pair(pair<_TyA, _TyB> &&) pf_attr_noexcept			 = default;

		/// Destructor
		pf_decl_inline pf_decl_constexpr ~pair() pf_attr_noexcept = default;

		/// Store
		struct
		{
			_TyA x;
			_TyB y;
		};
		struct
		{
			_TyA r;
			_TyB g;
		};
		struct
		{
			_TyA a;
			_TyB b;
		};
	};

	/// PAIR: Make
	template <typename _TyA, typename _TyB>
	pf_decl_inline pf_decl_constexpr pair<_TyA, _TyB> make_pair(
			_TyA &&__a,
			_TyB &&__b) pf_attr_noexcept;
}

// Anonymous structs
#pragma GCC diagnostic pop

// Include: Pair -> Impl
#include "pulsar/tuple/pair.inl"

#endif // !PULSAR_PAIR_HPP