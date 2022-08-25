/*! @file   nuple.inl
 *  @author Fluffy (noe.louis-quentin@hotmail.fr)
 *  @brief  Implementation of nuple.
 *  @date   24-08-2022
 *
 *  @copyright Copyright (c) 2022 - Pulsar Software
 *
 *  @since 0.1.2
 */

#ifndef PULSAR_NUPLE_INL
#define PULSAR_NUPLE_INL 1

// Include: Pulsar -> Tuple
#include "pulsar/tuple.hpp"

// Pulsar
namespace pul
{
	/// Nutag -> Constructor
	template <typename _Ty, size_t _Hash>
	pf_decl_constexpr nutag<_Ty, _Hash>::nutag(_Ty &&__val)
			: val_(std::forward<_Ty>(__val))
	{}

	/// Nutag -> Nuval
	template <size_t _Hash, typename _Ty>
	pf_hint_nodiscard pf_decl_constexpr nutag<_Ty, _Hash> nuval(
			_Ty &&__val)
	{
		return nutag<_Ty, _Hash>(std::forward<_Ty>(__val));
	}

	/// Nuple -> Constructors
	template <typename... _Ts>
		requires(__is_nuple_nutag_v<_Ts> &&...)
	pf_decl_constexpr nuple<_Ts...>::nuple(_Ts &&...__args)
			: __nuple_select_base<_Ts...>(
					__tuple_make_base<__nuple_select_base<_Ts...>>(
							typename __tuple_extract_sequence<__tuple_select_base<_Ts...>>::type{},
							__args.val_...))
	{}
	template <typename... _Ts>
		requires(__is_nuple_nutag_v<_Ts> &&...)
	template <typename... _InTs>
	pf_decl_constexpr nuple<_Ts...>::nuple(_InTs &&...__args)
			: __nuple_select_base<_Ts...>(
					__tuple_make_base<__nuple_select_base<_Ts...>>(
							typename __tuple_extract_sequence<__tuple_select_base<_Ts...>>::type{},
							std::forward<_InTs>(__args)...))
	{}

	/// Nuple -> Get
	template <size_t _Hash, typename _Nuple>
	pf_hint_nodiscard pf_decl_constexpr auto &n_get(
			_Nuple &__nuple) pf_attr_noexcept
			requires(is_nuple_v<_Nuple> &&_Hash == tuple_at<_Nuple, 0>::type::hash)
	{
		return __nuple.elem_.data_;
	}
	template <size_t _Hash, typename _Nuple>
	pf_hint_nodiscard pf_decl_constexpr auto &n_get(
			_Nuple &__nuple) pf_attr_noexcept
			requires(is_nuple_v<_Nuple> &&_Hash != tuple_at<_Nuple, 0>::type::hash && tuple_size_v<_Nuple> > 1)
	{
		return n_get<_Hash>(__nuple.rest_);
	}
	template <size_t _Hash, typename _Nuple>
	pf_hint_nodiscard pf_decl_constexpr const auto &n_get(
			_Nuple const &__nuple) pf_attr_noexcept
			requires(is_nuple_v<_Nuple> &&_Hash == tuple_at<_Nuple, 0>::type::hash)
	{
		return __nuple.elem_.data_;
	}
	template <size_t _Hash, typename _Nuple>
	pf_hint_nodiscard pf_decl_constexpr const auto &n_get(
			_Nuple const &__nuple) pf_attr_noexcept
			requires(is_nuple_v<_Nuple> &&_Hash != tuple_at<_Nuple, 0>::type::hash && tuple_size_v<_Nuple> > 1)
	{
		return n_get<_Hash>(__nuple.rest_);
	}
	template <size_t _Hash, typename _Nuple>
	pf_hint_nodiscard pf_decl_constexpr auto &&n_get(
			_Nuple &&__nuple) pf_attr_noexcept
			requires(is_nuple_v<_Nuple> &&_Hash == tuple_at<_Nuple, 0>::type::hash)
	{
		return std::move(__nuple.elem_.data_);
	}
	template <size_t _Hash, typename _Nuple>
	pf_hint_nodiscard pf_decl_constexpr auto &&n_get(
			_Nuple &&__nuple) pf_attr_noexcept
			requires(is_nuple_v<_Nuple> &&_Hash != tuple_at<_Nuple, 0>::type::hash && tuple_size_v<_Nuple> > 1)
	{
		return std::move(n_get<_Hash>(__nuple.rest_));
	}
	template <size_t _Hash, typename _Nuple>
	pf_hint_nodiscard pf_decl_constexpr const auto &&n_get(
			_Nuple const &&__nuple) pf_attr_noexcept
			requires(is_nuple_v<_Nuple> &&_Hash == tuple_at<_Nuple, 0>::type::hash)
	{
		return std::move(__nuple.elem_.data_);
	}
	template <size_t _Hash, typename _Nuple>
	pf_hint_nodiscard pf_decl_constexpr const auto &&n_get(
			_Nuple const &&__nuple) pf_attr_noexcept
			requires(is_nuple_v<_Nuple> &&_Hash != tuple_at<_Nuple, 0>::type::hash && tuple_size_v<_Nuple> > 1)
	{
		return std::move(n_get<_Hash>(__nuple.rest_));
	}
}

#endif // !PULSAR_NUPLE_INL