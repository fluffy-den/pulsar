/*! @file   tuple.hpp
 *  @author Fluffy (noe.louis-quentin@hotmail.fr)
 *  @brief
 *  @date   16-06-2022
 *
 *  @copyright Copyright (c) 2022 - Pulsar Software
 *
 *  @since 0.1.2
 */

#ifndef PULSAR_TUPLE_HPP
#define PULSAR_TUPLE_HPP 1

// Include: Pulsar
#include "pulsar/pulsar.hpp"

// Include: C++
#include <functional>

// Pulsar
namespace pul
{
	/// TUPLE: Definition
	template <typename...>
	struct tuple;

	/// TUPLE: Parameter Pack -> Index
	template <typename _Ty, size_t _Index>
	struct __tuple_index
	{
		pf_decl_static pf_decl_constexpr size_t index = _Index;
		using type																		= _Ty;
		_Ty data_;
	};

	/// SFINAE: Index Sequence
	template <size_t...>
	struct index_sequence
	{};
	template <size_t _ID, size_t... _IDs>
	struct __index_sequence_helper
			: public __index_sequence_helper<_ID - 1U, _ID - 1U, _IDs...>
	{};
	template <size_t... _IDs>
	struct __index_sequence_helper<0U, _IDs...>
	{
		using type = index_sequence<_IDs...>;
	};
	template <size_t _Num>
	using make_index_sequence = typename __index_sequence_helper<_Num>::type;

	/// TUPLE: Param Pack -> Get Index Value
	template <size_t _Index, typename _InTy, typename... _InTs>
		requires(_Index == 0)
	pf_hint_nodiscard pf_decl_inline pf_decl_constexpr auto &__param_pack_get_index_value(
			_InTy &&__arg,
			_InTs &&...)
			pf_attr_noexcept
	{
		return __arg;
	}
	template <size_t _Index, typename _InTy, typename... _InTs>
		requires(_Index != 0 && _Index < 1 + sizeof...(_InTs))
	pf_hint_nodiscard pf_decl_inline pf_decl_constexpr auto &__param_pack_get_index_value(
			_InTy &&,
			_InTs &&...__args) pf_attr_noexcept
	{
		return __param_pack_get_index_value<_Index - 1>(std::forward<_InTs>(__args)...);
	}

	/// TUPLE: Base
	template <typename... _Ts>
	struct __tuple_base;
	template <typename _Ty, typename... _Ts>
	struct alignas(alignof(_Ty)) __tuple_base<_Ty, _Ts...>
	{
	public:
		/// Constructor
		pf_decl_inline pf_decl_constexpr __tuple_base() pf_attr_noexcept
		{}
		template <typename... _InTs>
		pf_decl_inline pf_decl_constexpr __tuple_base(
				_InTs &&...__args) pf_attr_noexcept
				: elem_{ __param_pack_get_index_value<_Ty::index>(std::forward<_InTs>(__args)...) }
				, rest_(std::forward<_InTs>(__args)...)
		{}
		pf_decl_inline pf_decl_constexpr __tuple_base(
				__tuple_base<_Ty, _Ts...> const &__r) pf_attr_noexcept
				: elem_{ __r.elem_.data_ }
				, rest_(__r.rest_)
		{}
		pf_decl_inline pf_decl_constexpr __tuple_base(
				__tuple_base<_Ty, _Ts...> &&__r) pf_attr_noexcept
				: elem_{ std::move(__r.elem_.data_) }
				, rest_(std::move(__r.rest_))
		{}

		/// Operator=
		pf_decl_inline pf_decl_constexpr __tuple_base<_Ty, _Ts...> &operator=(
				__tuple_base<_Ty, _Ts...> const &__r) = default;
		pf_decl_inline pf_decl_constexpr __tuple_base<_Ty, _Ts...> &operator=(
				__tuple_base<_Ty, _Ts...> &&__r) = default;

		_Ty elem_;
		__tuple_base<_Ts...> rest_;
	};
	template <typename _Ty>
	struct __tuple_base<_Ty>
	{
		/// Constructor
		pf_decl_inline pf_decl_constexpr __tuple_base() pf_attr_noexcept
		{}
		template <typename... _InTs>
		pf_decl_inline pf_decl_constexpr __tuple_base(
				_InTs &&...__args) pf_attr_noexcept
				: elem_{ __param_pack_get_index_value<_Ty::index>(std::forward<_InTs>(__args)...) }
		{}
		pf_decl_inline pf_decl_constexpr __tuple_base(
				_Ty &&__arg)
				: elem_{ std::forward<_Ty>(__arg) }
		{}
		pf_decl_inline pf_decl_explicit pf_decl_constexpr __tuple_base(
				__tuple_base<_Ty> const &__r) = default;
		pf_decl_inline pf_decl_explicit pf_decl_constexpr __tuple_base(
				__tuple_base<_Ty> &&__r) = default;

		/// Operator=
		pf_decl_inline pf_decl_constexpr __tuple_base<_Ty> &operator=(
				__tuple_base<_Ty> const &__r) = default;
		pf_decl_inline pf_decl_constexpr __tuple_base<_Ty> &operator=(
				__tuple_base<_Ty> &&__r) = default;

		_Ty elem_;
	};
	template <>
	struct __tuple_base<>
	{};

	/// TUPLE: Parameter Pack -> Size
	template <typename _Tuple>
	struct tuple_size;
	template <typename... _Ts>
	struct tuple_size<__tuple_base<_Ts...>>: std::integral_constant<size_t, sizeof...(_Ts)>
	{};
	template <typename... _Ts>
	struct tuple_size<tuple<_Ts...>>: std::integral_constant<size_t, sizeof...(_Ts)>
	{};
	template <typename _Tuple>
	pf_decl_static pf_decl_constexpr size_t tuple_size_v =
			tuple_size<std::remove_cvref_t<_Tuple>>::value;

	/// TUPPLE: Make -> Indexed Sequence
	template <typename... _Ts>
	struct __tuple_make_indexed_sequence
	{
	private:
		template <typename _SequenceTy>
		struct __make_indexed_tuple;
		template <size_t... _Is>
		struct __make_indexed_tuple<index_sequence<_Is...>>
		{
			using tuple_type = __tuple_base<__tuple_index<_Ts, _Is>...>;
		};

	public:
		using tuple_type = __make_indexed_tuple<make_index_sequence<sizeof...(_Ts)>>::tuple_type;
	};

	/// TUPPLE: Parameter Pack -> Index At
	template <typename _Tuple, size_t _Index>
	struct __tuple_index_at;
	template <typename _Ty, typename... _Ts>
	struct __tuple_index_at<__tuple_base<_Ty, _Ts...>, 0>
	{
		using type = _Ty;
	};
	template <size_t _Index, typename _Ty, typename... _Ts>
	struct __tuple_index_at<__tuple_base<_Ty, _Ts...>, _Index>: __tuple_index_at<__tuple_base<_Ts...>, _Index - 1>
	{};

	/// TUPPLE: Parameter Pack -> Element
	template <typename _Tuple, size_t _Index>
	struct tuple_element
	{
		using type = __tuple_index_at<_Tuple, _Index>::type::type;
	};
	template <typename _Tuple, size_t _Index>
	using tuple_element_t = tuple_element<_Tuple, _Index>::type;

	/// TUPPLE: Parameter Pack -> Concatenate
	template <typename _TupleA, typename _TupleB>
	struct tuple_conc;
	template <typename... _TsA, typename... _TsB>
	struct tuple_conc<__tuple_base<_TsA...>, __tuple_base<_TsB...>>
	{
		using tuple_type = __tuple_base<_TsA..., _TsB...>;
	};
	template <typename _TupleA, typename _TupleB>
	using tuple_conc_t = typename tuple_conc<_TupleA, _TupleB>::tuple_type;

	/// TUPPLE: Parameter Pack -> Extract
	template <typename _Tuple, size_t _Start, size_t _End>
	struct tuple_extract
	{
	private:
		template <typename _SequenceTy>
		struct __extract_from_index_sequence;
		template <size_t... _Is>
		struct __extract_from_index_sequence<index_sequence<_Is...>>
		{
			using tuple_type = __tuple_base<typename __tuple_index_at<_Tuple, _Is + _Start>::type...>;
		};

	public:
		using tuple_type = typename __extract_from_index_sequence<make_index_sequence<_End - _Start + 1>>::tuple_type;
	};
	template <typename _Tuple, size_t _Start, size_t _End>
	using tuple_extract_t = typename tuple_extract<_Tuple, _Start, _End>::tuple_type;

	/// TUPPLE: Parameter Pack -> Remove
	template <typename _Tuple, size_t _Index>
	struct tuple_remove
	{
		using tuple_type = typename tuple_conc<
				typename tuple_extract<_Tuple, 0, _Index - 1>::tuple_type,
				typename tuple_extract<_Tuple, _Index + 1, tuple_size<_Tuple>::value - 1>::tuple_type>::tuple_type;
	};
	template <typename _Tuple, size_t _Index>
		requires(_Index == 0)
	struct tuple_remove<_Tuple, _Index>
	{
		using tuple_type =
				typename tuple_extract<_Tuple, 1, tuple_size<_Tuple>::value - 1>::tuple_type;
	};
	template <typename _Tuple, size_t _Index>
		requires(_Index == tuple_size<_Tuple>::value - 1)
	struct tuple_remove<_Tuple, _Index>
	{
		using tuple_type =
				typename tuple_extract<_Tuple, 0, _Index - 1>::tuple_type;
	};
	template <typename _Tuple, size_t _Index>
	using tuple_remove_t = typename tuple_remove<_Tuple, _Index>::tuple_type;

	/// TUPPLE: Parameter Pack -> Order by Alignment type
	template <typename _TyA, typename _TyB>
	struct __tuple_descending_order_alignment: std::conditional_t<(alignof(_TyA) <= alignof(_TyB)), std::true_type, std::false_type>
	{};

	/// TUPPLE: Parameter Pack -> Swap Elements
	template <typename _Tuple, size_t _I, size_t _J>
	struct tuple_swap
	{
	private:
		template <typename _IndexSequence>
		struct element_swap;
		template <size_t... _Is>
		struct element_swap<index_sequence<_Is...>>
		{
			using tuple_type = __tuple_base<
					typename __tuple_index_at<
							_Tuple,
							_Is != _I && _Is != _J ? _Is : _Is == _I ? _J
																											 : _I>::type...>;
		};

	public:
		using tuple_type = typename element_swap<
				make_index_sequence<tuple_size<_Tuple>::value>>::tuple_type;
	};
	template <typename _Tuple, size_t _I, size_t _J>
	using tuple_swap_t = typename tuple_swap<_Tuple, _I, _J>::tuple_type;

	/// TUPPLE: Parameter Pack -> Sort
	template <typename _Tuple, template <typename, typename> typename _Comparator>
	struct __tuple_selection_sort
	{
	private:
		template <typename _TupleLoop, size_t _TupleSize, size_t _I, size_t _J>
		struct __selection_sort
		{
		private:
			using next_tuple_type = std::conditional_t<
					_Comparator<
							typename tuple_element<_TupleLoop, _I>::type,
							typename tuple_element<_TupleLoop, _J>::type>::value,
					typename tuple_swap<_TupleLoop, _I, _J>::tuple_type,
					_TupleLoop>;

		public:
			using tuple_type = typename __selection_sort<
					next_tuple_type,
					_TupleSize,
					_I,
					_J + 1>::tuple_type;
		};
		template <typename _TupleLoop, size_t _TupleSize, size_t _I>
		struct __selection_sort<_TupleLoop, _TupleSize, _I, _TupleSize>
		{
			using tuple_type = typename __selection_sort<
					_TupleLoop,
					_TupleSize,
					_I + 1,
					_I + 2>::tuple_type;
		};
		template <typename _TupleLoop, size_t _TupleSize, size_t _J>
		struct __selection_sort<_TupleLoop, _TupleSize, _TupleSize, _J>
		{
			using tuple_type = _TupleLoop;
		};

	public:
		using tuple_type = typename __selection_sort<
				_Tuple,
				tuple_size<_Tuple>::value,
				0,
				1>::tuple_type;
	};

	/// TUPPLE: Sorted Base
	template <typename... _Ts>
	using __tuple_sorted_base = typename __tuple_selection_sort<
			typename __tuple_make_indexed_sequence<_Ts...>::tuple_type,
			__tuple_descending_order_alignment>::tuple_type;

	template <size_t _Index, typename... _Ts>
	struct __tuple_index_at<tuple<_Ts...>, _Index>
	{
		using type = __tuple_index_at<__tuple_sorted_base<_Ts...>, _Index>::type;
	};

	/// TUPPLE: Implementation
	template <typename... _Ts>
	struct tuple: public __tuple_sorted_base<_Ts...>
	{
		using base = __tuple_sorted_base<_Ts...>;

		/// Constructors
		pf_decl_inline pf_decl_constexpr tuple()
		{}
		pf_decl_inline pf_decl_constexpr tuple(
				_Ts &&...__args) pf_attr_noexcept
				: __tuple_sorted_base<_Ts...>(std::forward<_Ts>(__args)...)
		{}
		pf_decl_inline pf_decl_constexpr tuple(tuple<_Ts...> const &__r) pf_attr_noexcept = default;
		pf_decl_inline pf_decl_constexpr tuple(tuple<_Ts...> &&__r) pf_attr_noexcept			= default;

		/// Operator=
		pf_decl_inline pf_decl_constexpr tuple<_Ts...> &operator=(tuple<_Ts...> const &__r) = default;
		pf_decl_inline pf_decl_constexpr tuple<_Ts...> &operator=(
				tuple<_Ts...> &&__r) = default;
	};

	/// TUPLE: Get
	template <size_t _Index, typename _Tuple>
		requires((_Index == __tuple_index_at<_Tuple, 0>::type::index))
	pf_hint_nodiscard pf_decl_inline pf_decl_constexpr auto &get(
			_Tuple &__tuple)

	{
		return __tuple.elem_.data_;
	}
	template <size_t _Index, typename _Tuple>
		requires((_Index != __tuple_index_at<_Tuple, 0>::type::index))
	pf_hint_nodiscard pf_decl_inline pf_decl_constexpr auto &get(
			_Tuple &__tuple)

	{
		return get<_Index>(__tuple.rest_);
	}

	/// TUPLE: Apply
	template <typename _Fun, typename _Tuple, size_t... _Is>
	pf_hint_nodiscard pf_decl_inline pf_decl_constexpr auto __tuple_apply(
			_Fun &&__fun,
			_Tuple &&__args,
			index_sequence<_Is...>)
	{
		return std::invoke(
				std::forward<_Fun>(__fun),
				get<_Is>(std::forward<_Tuple>(__args))...);
	}
	template <typename _Fun, typename _Tuple>
	pf_hint_nodiscard pf_decl_inline pf_decl_constexpr auto tuple_apply(
			_Fun &&__fun,
			_Tuple &&__args)
	{
		return __tuple_apply(
				std::forward<_Fun>(__fun),
				std::forward<_Tuple>(__args),
				make_index_sequence<tuple_size_v<_Tuple>>{});
	}
}

#endif // !PULSAR_TUPLE_HPP