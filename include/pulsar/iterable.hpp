/*! @file   iterable.hpp
 *  @author Louis-Quentin Noé (noe.louis-quentin@hotmail.fr)
 *  @brief  Implementation of iterables.
 *  @date   31-12-2022
 *
 *  @copyright Copyright (c) 2023 - Pulsar Software
 *
 *  @since 0.1.4
 */

#ifndef PULSAR_ITERABLE_HPP
#define PULSAR_ITERABLE_HPP 1

// Include: Pulsar
#include "pulsar/pulsar.hpp"
#include "pulsar/memory.hpp"
#include "pulsar/iterator.hpp"
#include "pulsar/allocator.hpp"
#include "pulsar/algorithm.hpp"

// Pulsar
namespace pul
{
	/// ITERABLE: Concept -> View
	template<typename _Iterable>
	concept __iterable_view_c = requires(
	 _Iterable &__it) {
		__it.begin();	 // TODO: View Concept
	};

	template<typename _Iterable>
	struct is_view : std::false_type
	{};
	template<__iterable_view_c _Iterable>
	struct is_view<_Iterable> : std::true_type
	{};
	template<typename _Iterable>
	pf_decl_static pf_decl_constexpr bool is_view_v = is_view<_Iterable>::value;


	/// ITERABLE: Concept -> Container
	template<typename _Iterable>
	concept __iterable_container_c = requires(
	 _Iterable &__it) {
		__it.begin();	 // TODO: Container Concept
	};

	template<typename _Iterable>
	struct is_container : std::false_type
	{};
	template<__iterable_container_c _Iterable>
	struct is_container<_Iterable> : std::true_type
	{};
	template<typename _Iterable>
	pf_decl_static pf_decl_constexpr bool is_container_v = is_container<_Iterable>::value;


	/// ITERABLE: Begin
	template<typename _Iterable>
	pf_hint_nodiscard pf_decl_constexpr typename _Iterable::iterator_t
	begin(
	 _Iterable &__r) pf_attr_noexcept
		requires(is_iterable_v<_Iterable>)
	{
		return __r.begin();
	}
	template<typename _Iterable>
	pf_hint_nodiscard pf_decl_constexpr typename _Iterable::const_iterator_t
	begin(
	 const _Iterable &__r) pf_attr_noexcept
		requires(is_const_iterable_v<_Iterable>)
	{
		return __r.begin();
	}
	template<typename _Iterable>
	pf_hint_nodiscard pf_decl_constexpr typename _Iterable::const_iterator_t
	cbegin(
	 const _Iterable &__r) pf_attr_noexcept
		requires(is_const_iterable_v<_Iterable>)
	{
		return __r.cbegin();
	}
	template<typename _Ty, size_t _Num>
	pf_hint_nodiscard pf_decl_constexpr iterator<_Ty>
	begin(
	 _Ty (&__arr)[_Num]) pf_attr_noexcept
	{
		return &__arr[0];
	}
	template<typename _Ty, size_t _Num>
	pf_hint_nodiscard pf_decl_constexpr const_iterator<_Ty>
	begin(
	 const _Ty (&__arr)[_Num]) pf_attr_noexcept
	{
		return &__arr[0];
	}
	template<typename _Ty, size_t _Num>
	pf_hint_nodiscard pf_decl_constexpr const_iterator<_Ty>
	cbegin(
	 const _Ty (&__arr)[_Num]) pf_attr_noexcept
	{
		return &__arr[0];
	}

	/// ITERABLE: End
	template<typename _Iterable>
	pf_hint_nodiscard pf_decl_constexpr typename _Iterable::iterator_t
	end(
	 _Iterable &__r) pf_attr_noexcept
		requires(is_iterable_v<_Iterable>)
	{
		return __r.end();
	}
	template<typename _Iterable>
	pf_hint_nodiscard pf_decl_constexpr typename _Iterable::const_iterator_t
	end(
	 const _Iterable &__r) pf_attr_noexcept
		requires(is_const_iterable_v<_Iterable>)
	{
		return __r.end();
	}
	template<typename _Iterable>
	pf_hint_nodiscard pf_decl_constexpr typename _Iterable::const_iterator_t
	cend(
	 const _Iterable &__r) pf_attr_noexcept
		requires(is_const_iterable_v<_Iterable>)
	{
		return __r.cend();
	}
	template<typename _Ty, size_t _Num>
	pf_hint_nodiscard pf_decl_constexpr iterator<_Ty>
	end(
	 _Ty (&__arr)[_Num]) pf_attr_noexcept
	{
		return (&__arr[0] + _Num);
	}
	template<typename _Ty, size_t _Num>
	pf_hint_nodiscard pf_decl_constexpr const_iterator<_Ty>
	end(
	 const _Ty (&__arr)[_Num]) pf_attr_noexcept
	{
		return (&__arr[0] + _Num);
	}
	template<typename _Ty, size_t _Num>
	pf_hint_nodiscard pf_decl_constexpr const_iterator<_Ty>
	cend(
	 const _Ty (&__arr)[_Num]) pf_attr_noexcept
	{
		return (&__arr[0] + _Num);
	}

	/// ITERABLE: Reverse Begin
	template<typename _Iterable>
	pf_hint_nodiscard pf_decl_constexpr typename _Iterable::reverse_iterator_t
	rbegin(
	 _Iterable &__r) pf_attr_noexcept
		requires(is_reverse_iterable_v<_Iterable>)
	{
		return __r.rbegin();
	}
	template<typename _Iterable>
	pf_hint_nodiscard pf_decl_constexpr typename _Iterable::const_reverse_iterator_t
	rbegin(
	 const _Iterable &__r) pf_attr_noexcept
		requires(is_const_reverse_iterable_v<_Iterable>)
	{
		return __r.rbegin();
	}
	template<typename _Iterable>
	pf_hint_nodiscard pf_decl_constexpr typename _Iterable::const_reverse_iterator_t
	crbegin(
	 const _Iterable &__r) pf_attr_noexcept
		requires(is_const_reverse_iterable_v<_Iterable>)
	{
		return __r.crbegin();
	}
	template<typename _Ty, size_t _Num>
	pf_hint_nodiscard pf_decl_constexpr reverse_iterator<iterator<_Ty>>
	rbegin(
	 _Ty (&__arr)[_Num]) pf_attr_noexcept
	{
		return (&__arr[0] + (_Num - 1));
	}
	template<typename _Ty, size_t _Num>
	pf_hint_nodiscard pf_decl_constexpr reverse_iterator<const_iterator<_Ty>>
	rbegin(
	 const _Ty (&__arr)[_Num]) pf_attr_noexcept
	{
		return (&__arr[0] + (_Num - 1));
	}
	template<typename _Ty, size_t _Num>
	pf_hint_nodiscard pf_decl_constexpr reverse_iterator<const_iterator<_Ty>>
	crbegin(
	 const _Ty (&__arr)[_Num]) pf_attr_noexcept
	{
		return (&__arr[0] + (_Num - 1));
	}

	/// ITERABLE: Reverse End
	template<typename _Iterable>
	pf_hint_nodiscard pf_decl_constexpr typename _Iterable::reverse_iterator_t
	rend(
	 _Iterable &__r) pf_attr_noexcept
		requires(is_reverse_iterable_v<_Iterable>)
	{
		return __r.rend();
	}

	template<typename _Iterable>
	pf_hint_nodiscard pf_decl_constexpr typename _Iterable::const_reverse_iterator_t
	rend(
	 const _Iterable &__r) pf_attr_noexcept
		requires(is_const_reverse_iterable_v<_Iterable>)
	{
		return __r.rend();
	}
	template<typename _Iterable>
	pf_hint_nodiscard pf_decl_constexpr typename _Iterable::const_reverse_iterator_t
	crend(
	 const _Iterable &__r) pf_attr_noexcept
		requires(is_const_reverse_iterable_v<_Iterable>)
	{
		return __r.crend();
	}
	template<typename _Ty, size_t _Num>
	pf_hint_nodiscard pf_decl_constexpr reverse_iterator<iterator<_Ty>>
	rend(
	 _Ty (&__arr)[_Num]) pf_attr_noexcept
	{
		return (&__arr[0] - 1);
	}
	template<typename _Ty, size_t _Num>
	pf_hint_nodiscard pf_decl_constexpr reverse_iterator<const_iterator<_Ty>>
	rend(
	 const _Ty (&__arr)[_Num]) pf_attr_noexcept
	{
		return (&__arr[0] - 1);
	}
	template<typename _Ty, size_t _Num>
	pf_hint_nodiscard pf_decl_constexpr reverse_iterator<const_iterator<_Ty>>
	crend(
	 const _Ty (&__arr)[_Num]) pf_attr_noexcept
	{
		return (&__arr[0] - 1);
	}

	/// ITERABLE: Data
	template<typename _Iterable>
	pf_hint_nodiscard pf_decl_constexpr typename _Iterable::value_t *
	data(
	 _Iterable &__r) pf_attr_noexcept
		requires(is_mappable_v<_Iterable>)
	{
		return __r.data();
	}
	template<typename _Iterable>
	pf_hint_nodiscard pf_decl_constexpr const typename _Iterable::value_t *
	data(
	 const _Iterable &__r) pf_attr_noexcept
		requires(is_mappable_v<_Iterable>)
	{
		return __r.data();
	}
	template<typename _Ty, size_t _Num>
	pf_hint_nodiscard pf_decl_constexpr _Ty *
	data(
	 _Ty (&__arr)[_Num]) pf_attr_noexcept
	{
		return &__arr[0];
	}
	template<typename _Ty, size_t _Num>
	pf_hint_nodiscard const pf_decl_constexpr _Ty *
	data(
	 const _Ty (&__arr)[_Num]) pf_attr_noexcept
	{
		return &__arr[0];
	}

}	 // namespace pul

// Include: Pulsar -> Iterable
#include "pulsar/iterable/array.hpp"
#include "pulsar/iterable/sequence.hpp"
#include "pulsar/iterable/list.hpp"
#include "pulsar/iterable/lifo.hpp"
#include "pulsar/iterable/hash_map.hpp"

#endif	// !PULSAR_CONTAINER_HPP
