/*! @file   nuple.hpp
 *  @author Fluffy (noe.louis-quentin@hotmail.fr)
 *  @brief	Definition of nuple. (tuple with field names).
 *  @date   24-08-2022
 *
 *  @copyright Copyright (c) 2022 - Pulsar Software
 *
 *  @since 0.1.2
 */

#ifndef PULSAR_NUPLE_HPP
#define PULSAR_NUPLE_HPP 1

// Include: Pulsar -> Tuple
#include "pulsar/tuple/tuple.hpp"

// Pulsar
namespace pul
{

/// NUPLE: Macro -> $
#define $(name) fnv1a::hash(name)

	/// NUPLE: SFINAE -> Index
	template <typename _Ty, size_t _Hash, size_t _Index>
	struct __nuple_index: public __tuple_index<_Ty, _Index>
	{
		pf_decl_static pf_decl_constexpr size_t hash = _Hash;
		using type																	 = _Ty;
	};

	/// NUPLE: SFINAE -> Is Nuple Index
	template <typename _Ty>
	struct __is_nuple_index: std::false_type
	{};
	template <typename _Ty, size_t _Hash, size_t _Index>
	struct __is_nuple_index<__nuple_index<_Ty, _Hash, _Index>>: std::true_type
	{};
	template <typename _Ty>
	pf_decl_static pf_decl_constexpr bool __is_nuple_index_v = __is_nuple_index<_Ty>::value;

	/// NUPLE: Nutag
	template <typename _Ty, size_t _Hash>
	struct nutag
	{
		using type																	 = _Ty;
		pf_decl_static pf_decl_constexpr size_t hash = _Hash;

		/// Constructors
		pf_decl_inline pf_decl_constexpr nutag() pf_attr_noexcept = default;
		pf_decl_inline pf_decl_constexpr nutag(_Ty &&__val)
				: val_(std::forward<_Ty>(__val))
		{}
		pf_decl_inline pf_decl_constexpr nutag(nutag<_Ty, _Hash> const &) pf_attr_noexcept = delete;
		pf_decl_inline pf_decl_constexpr nutag(nutag<_Ty, _Hash> &&) pf_attr_noexcept			 = delete;

		/// Store
		_Ty val_;
	};

	/// NUPLE: Nutag -> Nuval
	template <size_t _Hash, typename _Ty>
	pf_hint_nodiscard pf_decl_inline pf_decl_constexpr nutag<_Ty, _Hash> nuval(
			_Ty &&__val)
	{
		return nutag<_Ty, _Hash>(std::forward<_Ty>(__val));
	}

	/// NUPLE: SFINAE -> Is Nutag
	template <typename _Ty>
	struct is_nuple_nutag: std::false_type
	{};
	template <typename _Ty, size_t _Hash>
	struct is_nuple_nutag<nutag<_Ty, _Hash>>: std::true_type
	{};
	template <typename _Ty>
	pf_decl_static pf_decl_constexpr bool is_nuple_nutag_v = is_nuple_nutag<_Ty>::value;

	/// NUPLE: SFINAE -> Nutag Type
	template <typename _Ty>
	struct nuple_nutag_type;
	template <typename _Ty, size_t _Hash>
	struct nuple_nutag_type<nutag<_Ty, _Hash>>
	{
		using type = _Ty;
	};
	template <typename _Ty>
	using nuple_nutag_type_t = typename nuple_nutag_type<_Ty>::type;

	/// NUPLE: SFINAE -> Nutag Hash
	template <typename _Ty>
	struct nuple_nutag_hash;
	template <typename _Ty, size_t _Hash>
	struct nuple_nutag_hash<nutag<_Ty, _Hash>>
	{
		pf_decl_static pf_decl_constexpr size_t hash = _Hash;
	};
	template <typename _Ty>
	pf_decl_static pf_decl_constexpr size_t nuple_nutag_hash_v = nuple_nutag_hash<_Ty>::hash;

	/// NUPLE: SFINAE -> Make Indexed Sequence
	template <typename... _Ts>
	struct __nuple_make_indexed_sequence
	{
	private:
		template <typename _SequenceTy>
		struct __make_indexed_nuple;
		template <size_t... _Is>
		struct __make_indexed_nuple<index_sequence<_Is...>>
		{
			using tuple_type = __tuple_base<__nuple_index<
					nuple_nutag_type_t<_Ts>,
					nuple_nutag_hash_v<_Ts>,
					_Is>...>;
		};

	public:
		using tuple_type = __make_indexed_nuple<make_index_sequence<sizeof...(_Ts)>>::tuple_type;
	};

	/// NUPLE: Impl -> Base
	template <typename... _Ts>
	using __nuple_select_base = typename __tuple_selection_sort<
			typename __nuple_make_indexed_sequence<std::remove_reference_t<_Ts>...>::tuple_type,
			__tuple_descending_order_alignment>::tuple_type;

	/// NUPLE: Impl
	template <typename... _Ts>
		requires(is_nuple_nutag_v<_Ts> &&...)
	struct nuple: public __nuple_select_base<_Ts...>
	{
		using memory_type = __tuple_select_base<_Ts...>;

		/// Constructors
		pf_decl_inline pf_decl_constexpr nuple() pf_attr_noexcept = default;
		pf_decl_inline pf_decl_constexpr nuple(_Ts &&...__args)
				: __nuple_select_base<_Ts...>(
						__tuple_make_base<__nuple_select_base<_Ts...>>(
								typename __tuple_extract_sequence<__tuple_select_base<_Ts...>>::type{},
								__args.val_...))
		{}
		template <typename... _InTs>
		pf_decl_inline pf_decl_constexpr nuple(_InTs &&...__args)
				: __nuple_select_base<_Ts...>(
						__tuple_make_base<__nuple_select_base<_Ts...>>(
								typename __tuple_extract_sequence<__tuple_select_base<_Ts...>>::type{},
								std::forward<_InTs>(__args)...))
		{}
		pf_decl_inline pf_decl_constexpr nuple(nuple<_Ts...> const &) = default;
		pf_decl_inline pf_decl_constexpr nuple(nuple<_Ts...> &&)			= default;

		/// Operator=
		pf_decl_inline pf_decl_constexpr nuple<_Ts...> &operator=(nuple<_Ts...> const &__r) = default;
		pf_decl_inline pf_decl_constexpr nuple<_Ts...> &operator=(nuple<_Ts...> &&__r)			= default;
	};

	/// NUPLE: SFINAE -> Tuple Size
	template <typename... _Ts>
	struct tuple_size<nuple<_Ts...>>: std::integral_constant<size_t, sizeof...(_Ts)>
	{};

	/// NUPLE: SFINAE -> Tuple At
	template <size_t _Index, typename... _Ts>
	struct tuple_at<nuple<_Ts...>, _Index>
	{
		using type = tuple_at<__nuple_select_base<_Ts...>, _Index>::type;
	};

	/// NUPLE: SFINAE -> Is Tuple
	template <typename... _Ts>
	struct is_tuple<nuple<_Ts...>>: std::true_type
	{};

	/// NUPLE: SFINAE -> Is Nuple
	template <typename _Ty>
	struct is_nuple: std::false_type
	{};
	template <typename... _Ts>
		requires(__is_nuple_index_v<_Ts> &&...)
	struct is_nuple<__tuple_base<_Ts...>>: std::true_type
	{};
	template <typename... _Ts>
	struct is_nuple<nuple<_Ts...>>: std::true_type
	{};
	template <typename _Ty>
	pf_decl_static pf_decl_constexpr bool is_nuple_v = is_nuple<_Ty>::value;

	/// NUPLE: SFINAE -> Memory Type
	template <typename... _Ts>
	struct tuple_memory_type<nuple<_Ts...>>
	{
		using type = __nuple_select_base<_Ts...>;
	};

	/// NUPLE: (Named) Get
	template <size_t _Hash, typename _Nuple>
	pf_hint_nodiscard pf_decl_inline pf_decl_constexpr auto &n_get(
			_Nuple &__nuple) pf_attr_noexcept
			requires(is_nuple_v<_Nuple> &&_Hash == tuple_at<_Nuple, 0>::type::hash)
	{
		return __nuple.elem_.data_;
	}
	template <size_t _Hash, typename _Nuple>
	pf_hint_nodiscard pf_decl_inline pf_decl_constexpr auto &n_get(
			_Nuple &__nuple) pf_attr_noexcept
			requires(is_nuple_v<_Nuple> &&_Hash != tuple_at<_Nuple, 0>::type::hash && tuple_size_v<_Nuple> > 1)
	{
		return n_get<_Hash>(__nuple.rest_);
	}
	template <size_t _Hash, typename _Nuple>
	pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const auto &n_get(
			_Nuple const &__nuple) pf_attr_noexcept
			requires(is_nuple_v<_Nuple> &&_Hash == tuple_at<_Nuple, 0>::type::hash)
	{
		return __nuple.elem_.data_;
	}
	template <size_t _Hash, typename _Nuple>
	pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const auto &n_get(
			_Nuple const &__nuple) pf_attr_noexcept
			requires(is_nuple_v<_Nuple> &&_Hash != tuple_at<_Nuple, 0>::type::hash && tuple_size_v<_Nuple> > 1)
	{
		return n_get<_Hash>(__nuple.rest_);
	}
	template <size_t _Hash, typename _Nuple>
	pf_hint_nodiscard pf_decl_inline pf_decl_constexpr auto &&n_get(
			_Nuple &&__nuple) pf_attr_noexcept
			requires(is_nuple_v<_Nuple> &&_Hash == tuple_at<_Nuple, 0>::type::hash)
	{
		return std::move(__nuple.elem_.data_);
	}
	template <size_t _Hash, typename _Nuple>
	pf_hint_nodiscard pf_decl_inline pf_decl_constexpr auto &&n_get(
			_Nuple &&__nuple) pf_attr_noexcept
			requires(is_nuple_v<_Nuple> &&_Hash != tuple_at<_Nuple, 0>::type::hash && tuple_size_v<_Nuple> > 1)
	{
		return std::move(n_get<_Hash>(__nuple.rest_));
	}
	template <size_t _Hash, typename _Nuple>
	pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const auto &&n_get(
			_Nuple const &&__nuple) pf_attr_noexcept
			requires(is_nuple_v<_Nuple> &&_Hash == tuple_at<_Nuple, 0>::type::hash)
	{
		return std::move(__nuple.elem_.data_);
	}
	template <size_t _Hash, typename _Nuple>
	pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const auto &&n_get(
			_Nuple const &&__nuple) pf_attr_noexcept
			requires(is_nuple_v<_Nuple> &&_Hash != tuple_at<_Nuple, 0>::type::hash && tuple_size_v<_Nuple> > 1)
	{
		return std::move(n_get<_Hash>(__nuple.rest_));
	}

	/// NUPLE: (Named) Offsetof
	template <size_t _Hash, typename _Nuple>
	pf_hint_nodiscard pf_decl_inline pf_decl_constexpr size_t n_offsetof(_Nuple const &__nuple) pf_attr_noexcept
			requires(is_nuple_v<_Nuple>)
	{
		return pul::addressof(&n_get<_Hash>(__nuple)) - pul::addressof(&__nuple);
	}

	/// NUPLE: (Named) Sizeof
	template <size_t _Hash, typename _Nuple>
	pf_hint_nodiscard pf_decl_inline pf_decl_constexpr size_t n_sizeof(_Nuple const &__nuple) pf_attr_noexcept
			requires(is_nuple_v<_Nuple>)
	{
		return sizeof(std::remove_reference_t<decltype(n_get<_Hash>(__nuple))>);
	}
}

#endif // !PULSAR_NUPLE_HPP