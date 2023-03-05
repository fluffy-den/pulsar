/*! @file   utility.hpp
 *  @author Louis-Quentin Noé (noe.louis-quentin@hotmail.fr)
 *  @brief
 *  @date   11-09-2022
 *
 *  @copyright Copyright (c) 2023 - Pulsar Software
 *
 *  @since 0.1.2
 */

#ifndef PULSAR_UTILITY_HPP
#define PULSAR_UTILITY_HPP 1

// Include: Pulsar
#include "pulsar/pulsar.hpp"

// Include: C++
#include <concepts>
#include <utility>
#include <type_traits>
#include <compare>
#include <tuple>// std::ignore

// Pulsar
namespace pul
{
	/// UTILITY: SFINAE -> Value Type
	template <typename _Ty>
	struct value_type {
		using type = typename _Ty::value_t;
	};
	template <typename _Ty> using value_type_t = value_type<_Ty>::type;

	/// UTILITY: (STD)
	template <typename _Ty>
	using initializer_list = std::initializer_list<_Ty>;
	pf_decl_constexpr auto ignore = std::ignore;


	/// UTILITY: Union Cast
	/*! @brief
	 *
	 *  @tparam _ToTy
	 *  @tparam _InTy
	 *  @param[in] __in
	 *  @return pf_decl_inline
	 */
	// Union-Cast
	template <typename _ToTy, typename _InTy>
	union __union_cast
	{
		/// Constructors
		pf_decl_inline __union_cast(_InTy __in)
			: in(__in) {
		}
		__union_cast(__union_cast const &) = delete;
		__union_cast(__union_cast &&)			 = delete;

		/// Destructor
		pf_decl_inline ~__union_cast() pf_attr_noexcept = default;

		/// Operator=
		__union_cast<_ToTy, _InTy> &operator=(__union_cast<_ToTy, _InTy> const &) = delete;
		__union_cast<_ToTy, _InTy> &operator=(__union_cast<_ToTy, _InTy> &&)			= delete;

		_InTy in;
		_ToTy to;
	};
	template <typename _ToTy, typename _InTy>
	pf_hint_nodiscard pf_decl_inline pf_decl_constexpr _ToTy
	union_cast(
		_InTy __in) pf_attr_noexcept
	requires(
		// Convertible
		(std::is_convertible_v<_InTy, _ToTy>)
		// Pointer
		|| (std::is_pointer_v<_ToTy> && std::is_pointer_v<_InTy>
				&& (std::is_const_v<_InTy> && std::is_const_v<_ToTy>)
				|| (!std::is_const_v<_InTy>))
		// Integer
		|| (std::is_integral_v<_ToTy> && std::is_integral_v<_InTy>)
		// Floating-Point
		|| (std::is_floating_point_v<_ToTy> && std::is_floating_point_v<_InTy>))
	{
		__union_cast<_ToTy, _InTy> uc(__in);
		return uc.to;
	}

	/// UTILITY: Address <=> Pointer
	pf_hint_nodiscard pf_decl_constexpr size_t
	addressof(
		const void *__ptr) pf_attr_noexcept
	{
		return union_cast<size_t>(__ptr);
	}
	template<typename _Ty>
	pf_hint_nodiscard pf_decl_constexpr _Ty*
	addrtoptr(
		size_t __addr) pf_attr_noexcept
	{
		return union_cast<_Ty*>(__addr);
	}
	template<typename _Ty>
	pf_hint_nodiscard pf_decl_constexpr const _Ty*
	addrtocptr(
		size_t __addr) pf_attr_noexcept
	{
		return union_cast<const _Ty*>(__addr);
	}
}

#endif // !PULSAR_UTILITY_HPP