/*! @file   tuple.inl
 *  @author Fluffy (noe.louis-quentin@hotmail.fr)
 *  @brief  Implementation of tuple.
 *  @date   24-08-2022
 *
 *  @copyright Copyright (c) 2022 - Pulsar Software
 *
 *  @since 0.1.2
 */

#ifndef PULSAR_TUPLE_INL
#define PULSAR_TUPLE_INL 1

// Include: Pulsar -> Tuple
#include "pulsar/tuple.hpp"

// Include: Pulsar
#include "pulsar/memory.hpp"

// Pulsar
namespace pul
{
	/// Tuple -> Get Index Value
	template <size_t _Index, typename _InTy, typename... _InTs>
	pf_hint_nodiscard pf_decl_constexpr auto &&__tuple_extract_at_index(
			_InTy &&__arg,
			_InTs &&...) pf_attr_noexcept
			requires(_Index == 0)
	{
		return std::move(__arg);
	}
	template <size_t _Index, typename _InTy, typename... _InTs>
	pf_hint_nodiscard pf_decl_constexpr auto &&__tuple_extract_at_index(
			_InTy &&,
			_InTs &&...__args) pf_attr_noexcept
			requires(_Index != 0 && (_Index < 1 + sizeof...(_InTs)))
	{
		return std::move(__tuple_extract_at_index<_Index - 1>(std::forward<_InTs>(__args)...));
	}

	/// Tuple -> Base -> Constructors
	template <typename _Ty, typename... _Ts>
	pf_decl_constexpr __tuple_base<_Ty, _Ts...>::__tuple_base(
			_Ty &&__arg,
			_Ts &&...__args)
			: elem_{ std::forward<_Ty>(__arg) }
			, rest_(std::forward<_Ts>(__args)...)
	{}
	template <typename _Ty, typename... _Ts>
	template <typename _InTy, typename... _InTs>
	pf_decl_constexpr __tuple_base<_Ty, _Ts...>::__tuple_base(
			_InTy &&__arg,
			_InTs &&...__args)
			: elem_{ std::forward<_InTy>(__arg) }
			, rest_(std::forward<_InTs>(__args)...)
	{}
	template <typename _Ty, typename... _Ts>
	pf_decl_constexpr __tuple_base<_Ty, _Ts...>::__tuple_base(
			__tuple_base<_Ty, _Ts...> const &__r)
			: elem_{ __r.elem_.data_ }
			, rest_(__r.rest_)
	{}
	template <typename _Ty, typename... _Ts>
	pf_decl_constexpr __tuple_base<_Ty, _Ts...>::__tuple_base(
			__tuple_base<_Ty, _Ts...> &&__r)
			: elem_{ std::move(__r.elem_.data_) }
			, rest_(std::move(__r.rest_))
	{}

	/// Tuple -> Base 1 -> Constructors
	template <typename _Ty>
	pf_decl_constexpr __tuple_base<_Ty>::__tuple_base(_Ty &&__arg)
			: elem_{ std::forward<_Ty>(__arg) }
	{}
	template <typename _Ty>
	template <typename _InTy>
	pf_decl_constexpr __tuple_base<_Ty>::__tuple_base(
			_InTy &&__arg)
			: elem_{ std::forward<_InTy>(__arg) }
	{}

	/// Tuple -> Make Base
	template <
			typename _Tuple,
			size_t... _Is,
			typename... _InTs>
	pf_hint_nodiscard pf_decl_constexpr _Tuple __tuple_make_base(
			index_sequence<_Is...>,
			_InTs &&...__args) pf_attr_noexcept
	{
		return _Tuple(
				__tuple_extract_at_index<_Is>(std::forward<_InTs>(__args)...)...);
	}

	/// Tuple -> Constructors
	template <typename... _Ts>
	pf_decl_constexpr tuple<_Ts...>::tuple(
			_Ts &&...__args)
			: __tuple_select_base<_Ts...>(
					__tuple_make_base<__tuple_select_base<_Ts...>>(
							typename __tuple_extract_sequence<__tuple_select_base<_Ts...>>::type{},
							std::forward<_Ts>(__args)...))
	{}
	template <typename... _Ts>
	template <typename... _InTs>
	pf_decl_constexpr tuple<_Ts...>::tuple(
			_InTs &&...__args)
			: __tuple_select_base<_Ts...>(
					__tuple_make_base<__tuple_select_base<_Ts...>>(
							typename __tuple_extract_sequence<__tuple_select_base<_Ts...>>::type{},
							std::forward<_InTs>(__args)...))
	{}

	/// Tuple -> Get
	template <size_t _Index, typename _Tuple>
	pf_hint_nodiscard pf_decl_constexpr auto &i_get(
			_Tuple &__tuple) pf_attr_noexcept
			requires(is_tuple_v<_Tuple> &&_Index == tuple_at<_Tuple, 0>::type::index)
	{
		return __tuple.elem_.data_;
	}
	template <size_t _Index, typename _Tuple>
	pf_hint_nodiscard pf_decl_constexpr auto &i_get(
			_Tuple &__tuple) pf_attr_noexcept
			requires(is_tuple_v<_Tuple> &&_Index != tuple_at<_Tuple, 0>::type::index && tuple_size_v<_Tuple> > 1)
	{
		return i_get<_Index>(__tuple.rest_);
	}
	template <size_t _Index, typename _Tuple>
	pf_hint_nodiscard pf_decl_constexpr const auto &i_get(
			_Tuple const &__tuple) pf_attr_noexcept
			requires(is_tuple_v<_Tuple> &&_Index == tuple_at<_Tuple, 0>::type::index)
	{
		return __tuple.elem_.data_;
	}
	template <size_t _Index, typename _Tuple>
	pf_hint_nodiscard pf_decl_constexpr const auto &i_get(
			_Tuple const &__tuple) pf_attr_noexcept
			requires(is_tuple_v<_Tuple> &&_Index != tuple_at<_Tuple, 0>::type::index && tuple_size_v<_Tuple> > 1)
	{
		return i_get<_Index>(__tuple.rest_);
	}
	template <size_t _Index, typename _Tuple>
	pf_hint_nodiscard pf_decl_constexpr auto &&i_get(
			_Tuple &&__tuple) pf_attr_noexcept
			requires(is_tuple_v<_Tuple> &&_Index == tuple_at<_Tuple, 0>::type::index)
	{
		return std::move(__tuple.elem_.data_);
	}
	template <size_t _Index, typename _Tuple>
	pf_hint_nodiscard pf_decl_constexpr auto &&i_get(
			_Tuple &&__tuple) pf_attr_noexcept
			requires(is_tuple_v<_Tuple> &&_Index != tuple_at<_Tuple, 0>::type::index && tuple_size_v<_Tuple> > 1)
	{
		return std::move(i_get<_Index>(__tuple.rest_));
	}
	template <size_t _Index, typename _Tuple>
	pf_hint_nodiscard pf_decl_constexpr const auto &&i_get(
			_Tuple const &&__tuple) pf_attr_noexcept
			requires(is_tuple_v<_Tuple> &&_Index == tuple_at<_Tuple, 0>::type::index)
	{
		return std::move(__tuple.elem_.data_);
	}
	template <size_t _Index, typename _Tuple>
	pf_hint_nodiscard pf_decl_constexpr const auto &&i_get(
			_Tuple const &&__tuple) pf_attr_noexcept
			requires(is_tuple_v<_Tuple> &&_Index != tuple_at<_Tuple, 0>::type::index && tuple_size_v<_Tuple> > 1)
	{
		return std::move(i_get<_Index>(__tuple.rest_));
	}

	/// Tuple -> Offsetof
	template <size_t _Index, typename _Tuple>
	pf_hint_nodiscard pf_decl_inline pf_decl_constexpr size_t i_offsetof(
			_Tuple const &__tuple) pf_attr_noexcept
			requires(is_tuple_v<_Tuple>)
	{
		return memory::addressof(&i_get<_Index>(__tuple)) - memory::addressof(&__tuple);
	}

	/// Tuple -> Sizeof
	template <size_t _Index, typename _Tuple>
	pf_hint_nodiscard pf_decl_inline pf_decl_constexpr size_t i_sizeof(
			_Tuple const &__tuple) pf_attr_noexcept
			requires(is_tuple_v<_Tuple>)
	{
		return sizeof(std::remove_reference_t<decltype(i_get<_Index>(__tuple))>);
	}

	/// Tuple -> Apply
	template <typename _Fun, typename _Tuple, size_t... _Is>
	pf_hint_nodiscard pf_decl_constexpr auto __tuple_apply(
			_Fun &&__fun,
			_Tuple &&__tuple,
			index_sequence<_Is...>)
	{
		return std::invoke(
				std::forward<_Fun>(__fun),
				i_get<_Is>(std::forward<_Tuple>(__tuple))...);
	}
	template <typename _Fun, typename _Tuple>
	pf_hint_nodiscard pf_decl_constexpr auto tuple_apply(
			_Fun &&__fun,
			_Tuple &&__tuple)
	{
		return __tuple_apply(
				std::forward<_Fun>(__fun),
				std::forward<_Tuple>(__tuple),
				make_index_sequence<tuple_size_v<_Tuple>>{});
	}
}

#endif // !PULSAR_TUPLE_INL