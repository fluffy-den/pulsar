/*! @file   tuple.hpp
 *  @author Fluffy (noe.louis-quentin@hotmail.fr)
 *  @brief	Tuple definition header.
 *  @date   16-06-2022
 *
 *  @copyright Copyright (c) 2022 - Pulsar Software
 *
 *  @since 0.1.2
 */

#ifndef PULSAR_TUPLE_HPP
#define PULSAR_TUPLE_HPP 1

// Include: Pulsar
#include "pulsar/hash.hpp"

// Include: C++
#include <functional>

// Pulsar
namespace pul
{
	/// TUPLE: Definition
	template <typename... _Ts>
	struct tuple;

	/// TUPLE: SFINAE
	template <typename _TyA, typename _TyB>
	struct __tuple_descending_order_alignment;
	template <typename _Tuple, template <typename, typename> typename _Comparator>
	struct __tuple_selection_sort;

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
		/// Constructors
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

		/// Store
		_Ty elem_;
		__tuple_base<_Ts...> rest_;
	};

	template <typename _Ty>
	struct __tuple_base<_Ty>
	{
		/// Constructors
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

		/// Store
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
	pf_decl_static pf_decl_constexpr size_t tuple_size_v = tuple_size<std::remove_cvref_t<_Tuple>>::value;

	/// TUPLE: Make -> Indexed Sequence
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

	/// TUPLE: Parameter Pack -> At
	/// Move by position not element value!
	template <typename _Tuple, size_t _Index>
	struct tuple_at;
	template <typename _Ty, typename... _Ts>
	struct tuple_at<__tuple_base<_Ty, _Ts...>, 0>
	{
		using type = _Ty;
	};
	template <typename _Ty, typename... _Ts, size_t _Index>
	struct tuple_at<__tuple_base<_Ty, _Ts...>, _Index>
			: tuple_at<__tuple_base<_Ts...>, _Index - 1>
	{};

	/// TUPLE: Parameter Pack -> At Index
	template <typename _Tuple, size_t _Index>
	struct tuple_at_index;
	template <typename _Ty, typename... _Ts, size_t _Index>
		requires(_Ty::index == _Index)
	struct tuple_at_index<__tuple_base<_Ty, _Ts...>, _Index>
	{
		using type = _Ty;
	};
	template <typename _Ty, typename... _Ts, size_t _Index>
	struct tuple_at_index<__tuple_base<_Ty, _Ts...>, _Index>
			: tuple_at_index<__tuple_base<_Ts...>, _Index>
	{};

	/// TUPLE: Parameter Pack -> Element
	template <typename _Tuple, size_t _Index>
	struct tuple_element
	{
		using type = tuple_at<_Tuple, _Index>::type::type;
	};
	template <typename _Tuple, size_t _Index>
	using tuple_element_t = tuple_element<_Tuple, _Index>::type;

	/// TUPLE: Parameter Pack -> Concatenate Linear
	template <typename _TupleA, typename _TupleB>
	struct tuple_conc;
	template <typename... _TsA, typename... _TsB>
	struct tuple_conc<__tuple_base<_TsA...>, __tuple_base<_TsB...>>
	{
		using tuple_type = __tuple_selection_sort<
				__tuple_base<_TsA..., __tuple_index<typename _TsB::type, _TsB::index + sizeof...(_TsA)>...>,
				__tuple_descending_order_alignment>::tuple_type;
	};
	template <typename _TupleA, typename _TupleB>
	using tuple_conc_t = typename tuple_conc<_TupleA, _TupleB>::tuple_type;

	/// TUPLE: Parameter Pack -> Extract Linear
	template <typename _Tuple, size_t _Start, size_t _End>
	struct __tuple_extract_without_reordering
	{
	private:
		template <typename _SequenceTy>
		struct __extract_from_index_sequence;
		template <size_t... _Is>
		struct __extract_from_index_sequence<index_sequence<_Is...>>
		{
			using tuple_type = __tuple_base<typename tuple_at<_Tuple, _Is + _Start>::type...>;
		};

	public:
		using tuple_type = typename __extract_from_index_sequence<make_index_sequence<_End - _Start + 1>>::tuple_type;
	};

	/// TUPLE: Parameter Pack -> Decrement if equal to Index
	template <typename _Tuple, size_t _Index>
	struct __tuple_decrement_if_greater_equal_to_index;
	template <typename... _Ts, size_t _Index>
	struct __tuple_decrement_if_greater_equal_to_index<
			__tuple_base<_Ts...>,
			_Index>
	{
		using tuple_type = __tuple_base<__tuple_index<
				typename _Ts::type,
				(_Ts::index > _Index) ? _Ts::index - 1 : _Ts::index>...>;
	};

	/// TUPLE: Parameter Pack -> Remove
	template <typename _Tuple, size_t _Index>
	struct tuple_remove
	{
	public:
		using tuple_type = __tuple_decrement_if_greater_equal_to_index<
				tuple_conc_t<
						typename __tuple_extract_without_reordering<_Tuple, 0, _Index - 1>::tuple_type,
						typename __tuple_extract_without_reordering<_Tuple, _Index + 1, tuple_size<_Tuple>::value - 1>::tuple_type>,
				_Index>::tuple_type;
	};
	template <typename _Tuple, size_t _Index>
		requires(_Index == 0)
	struct tuple_remove<_Tuple, _Index>
	{
		using tuple_type = typename __tuple_decrement_if_greater_equal_to_index<
				typename __tuple_extract_without_reordering<_Tuple, 1, tuple_size<_Tuple>::value - 1>::tuple_type,
				_Index>::tuple_type;
	};
	template <typename _Tuple, size_t _Index>
		requires(_Index == tuple_size<_Tuple>::value - 1)
	struct tuple_remove<_Tuple, _Index>
	{
		using tuple_type = typename __tuple_extract_without_reordering<_Tuple, 0, _Index - 1>::tuple_type;
	};
	template <typename _Tuple, size_t _Index>
	using tuple_remove_t = typename tuple_remove<_Tuple, _Index>::tuple_type;

	/// TUPLE: Parameter Pack -> Order by Alignment type
	template <typename _TyA, typename _TyB>
	struct __tuple_descending_order_alignment: std::conditional_t<(alignof(_TyA) <= alignof(_TyB)), std::true_type, std::false_type>
	{};

	/// TUPLE: Parameter Pack -> Swap Elements
	template <typename _Tuple, size_t _I, size_t _J>
	struct __tuple_swap
	{
	private:
		template <typename _IndexSequence>
		struct element_swap;
		template <size_t... _Is>
		struct element_swap<index_sequence<_Is...>>
		{
			using tuple_type = __tuple_base<
					typename tuple_at<
							_Tuple,
							_Is != _I && _Is != _J ? _Is : _Is == _I ? _J
																											 : _I>::type...>;
		};

	public:
		using tuple_type = typename element_swap<
				make_index_sequence<tuple_size<_Tuple>::value>>::tuple_type;
	};
	template <typename _Tuple, size_t _I, size_t _J>
	using tuple_swap_t = typename __tuple_swap<_Tuple, _I, _J>::tuple_type;

	/// TUPLE: Parameter Pack -> Sort
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
					typename __tuple_swap<_TupleLoop, _I, _J>::tuple_type,
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

	/// TUPLE: Parameter Pack -> Sorted Base
	template <typename... _Ts>
	using __tuple_sorted_base = typename __tuple_selection_sort<
			typename __tuple_make_indexed_sequence<_Ts...>::tuple_type,
			__tuple_descending_order_alignment>::tuple_type;

	template <size_t _Index, typename... _Ts>
	struct tuple_at<tuple<_Ts...>, _Index>
	{
		using type = tuple_at<__tuple_sorted_base<_Ts...>, _Index>::type;
	};

	/// TUPLE: Implementation
	template <typename... _Ts>
	struct tuple: public __tuple_sorted_base<_Ts...>
	{
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
		pf_decl_inline pf_decl_constexpr tuple<_Ts...> &operator=(tuple<_Ts...> &&__r)			= default;
	};

	/// TUPLE: Is Tuple
	template <typename _TupleTy>
	struct is_tuple: std::false_type
	{};
	template <typename... _Ts>
	struct is_tuple<tuple<_Ts...>>: std::true_type
	{};
	template <typename... _Ts>
	struct is_tuple<__tuple_base<_Ts...>>: std::true_type
	{};
	template <typename _TupleTy>
	pf_decl_static pf_decl_constexpr bool is_tuple_v = is_tuple<_TupleTy>::value;

	/// TUPLE: Get
	template <size_t _Index, typename _Tuple>
		requires((_Index == tuple_at<_Tuple, 0>::type::index))
	pf_hint_nodiscard pf_decl_inline pf_decl_constexpr auto &get(
			_Tuple &__tuple)

	{
		return __tuple.elem_.data_;
	}
	template <size_t _Index, typename _Tuple>
		requires((_Index != tuple_at<_Tuple, 0>::type::index))
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