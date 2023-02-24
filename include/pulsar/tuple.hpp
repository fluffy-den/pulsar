/*! @file   tuple.hpp
 *  @author Louis-Quentin Noé (noe.louis-quentin@hotmail.fr)
 *  @brief
 *  @date   19-02-2023
 *
 *  @copyright Copyright (c) 2023 - Pulsar Software
 *
 *  @since 0.1.4
 */

#ifndef PULSAR_TUPLE_HPP
#define PULSAR_TUPLE_HPP 1

// Include: Pulsar
#include "pulsar/hash.hpp"

// Pulsar
namespace pul
{
	/// TUPLE: Definition
	template<typename ... _Ts>
	struct tuple;

	/// TUPLE: SFINAE -> Index
	template<typename _Ty, size_t _Index>
	struct __tuple_index
	{
		pf_decl_static pf_decl_constexpr size_t index = _Index;
		using type = _Ty;
		_Ty data_;
	};

	/// SFINAE: Index Sequence
	template<size_t...>
	struct index_sequence
	{};
	template<size_t _ID, size_t... _IDs>
	struct __index_sequence_helper
		: public __index_sequence_helper<_ID - 1U, _ID - 1U, _IDs...>
	{};
	template<size_t... _IDs>
	struct __index_sequence_helper<0U, _IDs...>
	{
		using type = index_sequence<_IDs...>;
	};
	template<size_t _Num>
	using make_index_sequence = typename __index_sequence_helper<_Num>::type;

	/// TUPLE: SFINAE -> Parameter Pack Extract At Index
	template<size_t _Index, typename _InTy, typename ... _InTs>
	pf_hint_nodiscard pf_decl_constexpr auto &&
	__param_pack_extract_at_index(
		_InTy &&__arg,
		_InTs &&...) pf_attr_noexcept
	requires(_Index == 0)
	{
		return std::move(__arg);
	}
	template<size_t _Index, typename _InTy, typename ... _InTs>
	pf_hint_nodiscard pf_decl_constexpr auto &&
	__param_pack_extract_at_index(
		_InTy &&,
		_InTs &&... __args) pf_attr_noexcept
	requires(_Index != 0 && (_Index < 1 + sizeof...(_InTs)))
	{
		return std::move(__param_pack_extract_at_index<_Index - 1>(
											 std::forward<_InTs>(__args)...));
	}

	/// TUPLE: Base
	template<typename ... _Ts>
	struct __tuple_base;
	template<typename _Ty, typename ... _Ts>
	struct alignas(alignof(_Ty)) __tuple_base<_Ty, _Ts...>
	{
		public:
			/// Constructors
			pf_decl_constexpr
			__tuple_base() pf_attr_noexcept = default;
			pf_decl_constexpr
			__tuple_base(_Ty &&__arg, _Ts &&... __args)
				: elem_{ std::forward<_Ty>(__arg) }
				, rest_(std::forward<_Ts>(__args)...)
			{}
			template<typename _InTy, typename ... _InTs>
			pf_decl_constexpr
			__tuple_base(_InTy &&__arg, _InTs &&... __args)
				: elem_{ std::forward<_InTy>(__arg) }
				, rest_(std::forward<_InTs>(__args)...)
			{}
			pf_decl_constexpr
			__tuple_base(__tuple_base<_Ty, _Ts...> const &__r)
				: elem_{ __r.elem_.data_ }
				, rest_(__r.rest_)
			{}
			pf_decl_constexpr
			__tuple_base(__tuple_base<_Ty, _Ts...> &&__r)
				: elem_{ std::move(__r.elem_.data_) }
				, rest_(std::move(__r.rest_))
			{}

			/// Operator=
			pf_decl_constexpr __tuple_base<_Ty, _Ts...> &
			operator=(__tuple_base<_Ty, _Ts...> const &) = default;
			pf_decl_constexpr __tuple_base<_Ty, _Ts...> &
			operator=(__tuple_base<_Ty, _Ts...> &&) = default;

			/// Store
			_Ty elem_;
			__tuple_base<_Ts...> rest_;
	};

	/// TUPLE: Base -> 1
	template<typename _Ty>
	struct __tuple_base<_Ty>
	{
		/// Constructors
		pf_decl_constexpr
		__tuple_base() pf_attr_noexcept = default;
		pf_decl_constexpr
		__tuple_base(_Ty &&__arg)
			: elem_{ std::forward<_Ty>(__arg) }
		{}
		template<typename _InTy>
		pf_decl_constexpr
		__tuple_base(_InTy &&__arg)
			: elem_{ std::forward<_InTy>(__arg) }
		{}
		pf_decl_explicit pf_decl_constexpr
		__tuple_base(__tuple_base<_Ty> const &) =
		default;
		pf_decl_explicit pf_decl_constexpr
		__tuple_base(__tuple_base<_Ty> &&) =
		default;

		/// Operator=
		pf_decl_constexpr __tuple_base<_Ty> &
		operator=(__tuple_base<_Ty> const &) = default;
		pf_decl_constexpr __tuple_base<_Ty> &
		operator=(__tuple_base<_Ty> &&) = default;

		/// Store
		_Ty elem_;
	};

	/// TUPLE: Base -> 0
	template<>
	struct __tuple_base<>
	{};

	/// TUPLE: SFINAE -> Size
	template<typename _Tuple>
	struct tuple_size;
	template<typename ... _Ts>
	struct tuple_size<__tuple_base<_Ts...>>
		: std::integral_constant<size_t, sizeof...(_Ts)>
	{};
	template<typename ... _Ts>
	struct tuple_size<tuple<_Ts...>>
		: std::integral_constant<size_t, sizeof...(_Ts)>
	{};
	template<typename _Tuple>
	pf_decl_static pf_decl_constexpr size_t tuple_size_v =
		tuple_size<std::remove_cvref_t<_Tuple>>::value;

	/// TUPLE: SFINAE -> Maked Indexed Sequence
	template<typename ... _Ts>
	struct __tuple_make_indexed_sequence
	{
		private:
			template<typename _SequenceTy>
			struct __make_indexed_tuple;
			template<size_t... _Is>
			struct __make_indexed_tuple<index_sequence<_Is...>>
			{
				using tuple_type = __tuple_base<__tuple_index<_Ts, _Is>...>;
			};

		public:
			using tuple_type =
				__make_indexed_tuple<make_index_sequence<sizeof...(_Ts)>>::tuple_type;
	};

	/// TUPLE: SFINAE -> At
	/// Move by position not element value!
	template<typename _Tuple, size_t _Index>
	struct tuple_at;
	template<typename _Ty, typename ... _Ts>
	struct tuple_at<__tuple_base<_Ty, _Ts...>, 0>
	{
		using type = _Ty;
	};
	template<typename _Ty, typename ... _Ts, size_t _Index>
	struct tuple_at<__tuple_base<_Ty, _Ts...>, _Index>
		: tuple_at<__tuple_base<_Ts...>, _Index - 1>
	{};

	/// TUPLE: SFINAE -> At Index
	template<typename _Tuple, size_t _Index>
	struct tuple_at_index;
	template<typename _Ty, typename ... _Ts, size_t _Index>
	requires(_Ty::index == _Index)
	struct tuple_at_index<__tuple_base<_Ty, _Ts...>, _Index>
	{
		using type = _Ty;
	};
	template<typename _Ty, typename ... _Ts, size_t _Index>
	struct tuple_at_index<__tuple_base<_Ty, _Ts...>, _Index>
		: tuple_at_index<__tuple_base<_Ts...>, _Index>
	{};

	/// TUPLE: SFINAE -> Element
	template<typename _Tuple, size_t _Index>
	struct tuple_element
	{
		using type = tuple_at<_Tuple, _Index>::type::type;
	};
	template<typename _Tuple, size_t _Index>
	using tuple_element_t = tuple_element<_Tuple, _Index>::type;

	/// TUPLE: SFINAE -> Order by Alignment type
	template<typename _TyA, typename _TyB>
	struct __tuple_descending_order_alignment
		: std::conditional_t<(alignof(_TyA) <= alignof(_TyB)), std::true_type, std::false_type>
	{};

	/// TUPLE: SFINAE -> Selection Sort Definition
	template<typename _Tuple, template<typename, typename> typename _Comparator>
	struct __tuple_selection_sort;

	/// TUPLE: SFINAE -> Concatenate Linear
	template<typename _TupleA, typename _TupleB>
	struct tuple_conc;
	template<typename ... _TsA, typename ... _TsB>
	struct tuple_conc<__tuple_base<_TsA...>, __tuple_base<_TsB...>>
	{
		using tuple_type = __tuple_selection_sort<
			__tuple_base<_TsA..., __tuple_index<typename _TsB::type, _TsB::index + sizeof...(_TsA)>...>,
			__tuple_descending_order_alignment>::tuple_type;
	};
	template<typename _TupleA, typename _TupleB>
	using tuple_conc_t = typename tuple_conc<_TupleA, _TupleB>::tuple_type;

	/// TUPLE: SFINAE -> Extract Linear
	template<typename _Tuple, size_t _Start, size_t _End>
	struct __tuple_extract_without_reordering
	{
		private:
			template<typename _SequenceTy>
			struct __extract_from_index_sequence;
			template<size_t... _Is>
			struct __extract_from_index_sequence<index_sequence<_Is...>>
			{
				using tuple_type =
					__tuple_base<typename tuple_at<_Tuple, _Is + _Start>::type...>;
			};

		public:
			using tuple_type = typename __extract_from_index_sequence<
				make_index_sequence<_End - _Start + 1>>::tuple_type;
	};

	/// TUPLE: SFINAE -> Decrement if equal to Index
	template<typename _Tuple, size_t _Index>
	struct __tuple_decrement_if_greater_equal_to_index;
	template<typename ... _Ts, size_t _Index>
	struct __tuple_decrement_if_greater_equal_to_index<__tuple_base<_Ts...>,
																										 _Index>
	{
		using tuple_type = __tuple_base <
											 __tuple_index < typename _Ts::type,
		((_Ts::index > _Index) ? _Ts::index - 1 : _Ts::index) > ... >;
	};

	/// TUPLE: SFINAE -> Remove
	template<typename _Tuple, size_t _Index>
	struct tuple_remove
	{
		public:
			using tuple_type = __tuple_decrement_if_greater_equal_to_index<
				tuple_conc_t<
					typename __tuple_extract_without_reordering<_Tuple, 0, _Index - 1>::tuple_type,
					typename __tuple_extract_without_reordering<
						_Tuple,
						_Index + 1,
						tuple_size<_Tuple>::value - 1>::tuple_type>,
				_Index>::tuple_type;
	};
	template<typename _Tuple, size_t _Index>
	requires(_Index == 0)
	struct tuple_remove<_Tuple, _Index>
	{
		using tuple_type = typename __tuple_decrement_if_greater_equal_to_index<
			typename __tuple_extract_without_reordering<
				_Tuple,
				1,
				tuple_size<_Tuple>::value - 1>::tuple_type,
			_Index>::tuple_type;
	};
	template<typename _Tuple, size_t _Index>
	requires(_Index == tuple_size<_Tuple>::value - 1)
	struct tuple_remove<_Tuple, _Index>
	{
		using tuple_type =
			typename __tuple_extract_without_reordering<_Tuple, 0, _Index - 1>::tuple_type;
	};
	template<typename _Tuple, size_t _Index>
	using tuple_remove_t = typename tuple_remove<_Tuple, _Index>::tuple_type;

	/// TUPLE: SFINAE -> Swap Elements
	template<typename _Tuple, size_t _I, size_t _J>
	struct __tuple_swap
	{
		private:
			template<typename _IndexSequence>
			struct element_swap;
			template<size_t... _Is>
			struct element_swap<index_sequence<_Is...>>
			{
				using tuple_type =
					__tuple_base < typename tuple_at < _Tuple, (_Is != _I) && (_Is != _J) ? _Is : (_Is == _I) ? _J
																																															: _I > ::type ... >;
			};

		public:
			using tuple_type = typename element_swap<
				make_index_sequence<tuple_size<_Tuple>::value>>::tuple_type;
	};
	template<typename _Tuple, size_t _I, size_t _J>
	using tuple_swap_t = typename __tuple_swap<_Tuple, _I, _J>::tuple_type;

	/// TUPLE: SFINAE -> Sort
	template<typename _Tuple, template<typename, typename> typename _Comparator>
	struct __tuple_selection_sort
	{
		private:
			template<typename _TupleLoop, size_t _TupleSize, size_t _I, size_t _J>
			struct __selection_sort
			{
				private:
					using next_tuple_type = std::conditional_t<
						_Comparator<typename tuple_element<_TupleLoop, _I>::type,
												typename tuple_element<_TupleLoop, _J>::type>::value,
						typename __tuple_swap<_TupleLoop, _I, _J>::tuple_type,
						_TupleLoop>;

				public:
					using tuple_type = typename __selection_sort<next_tuple_type, _TupleSize, _I, _J + 1>::tuple_type;
			};
			template<typename _TupleLoop, size_t _TupleSize, size_t _I>
			struct __selection_sort<_TupleLoop, _TupleSize, _I, _TupleSize>
			{
				using tuple_type = typename __selection_sort<_TupleLoop, _TupleSize, _I + 1, _I + 2>::tuple_type;
			};
			template<typename _TupleLoop, size_t _TupleSize, size_t _J>
			struct __selection_sort<_TupleLoop, _TupleSize, _TupleSize, _J>
			{
				using tuple_type = _TupleLoop;
			};

		public:
			using tuple_type =
				typename __selection_sort<_Tuple, tuple_size<_Tuple>::value, 0, 1>::tuple_type;
	};

	/// TUPLE: SFINAE -> Base
	template<typename ... _Ts>
	using __tuple_select_base = typename __tuple_selection_sort<
		typename __tuple_make_indexed_sequence<
			std::remove_reference_t<_Ts>...>::tuple_type,
		__tuple_descending_order_alignment>::tuple_type;

	/// TUPLE: SFINAE -> Tuple At
	template<size_t _Index, typename ... _Ts>
	struct tuple_at<tuple<_Ts...>, _Index>
	{
		using type = tuple_at<__tuple_select_base<_Ts...>, _Index>::type;
	};

	/// TUPLE: SFINAE -> Index Sequence from Base
	template<typename _Tuple>
	struct __tuple_extract_sequence;
	template<typename ... _Ts>
	struct __tuple_extract_sequence<__tuple_base<_Ts...>>
	{
		using type = index_sequence<_Ts::index...>;
	};

	/// TUPLE: SFINAE -> Make Base
	template<typename _Tuple, size_t... _Is, typename ... _InTs>
	pf_hint_nodiscard pf_decl_constexpr _Tuple
	__tuple_make_base(index_sequence<_Is...>, _InTs &&... __args) pf_attr_noexcept
	{
		return _Tuple(
			__param_pack_extract_at_index<_Is>(std::forward<_InTs>(__args)...)...);
	}

	/// TUPLE: Impl
	template<typename ... _Ts>
	struct tuple : public __tuple_select_base<_Ts...>
	{
		using memory_type = __tuple_select_base<_Ts...>;

		/// Constructors
		pf_decl_constexpr
		tuple() pf_attr_noexcept = default;
		pf_decl_constexpr
		tuple(_Ts &&... __args)
			: __tuple_select_base<_Ts...>(
				__tuple_make_base<__tuple_select_base<_Ts...>>(
					typename __tuple_extract_sequence<
						__tuple_select_base<_Ts...>>::type{},
					std::forward<_Ts>(__args)...))
		{}
		template<typename ... _InTs>
		pf_decl_constexpr
		tuple(_InTs &&... __args)
			: __tuple_select_base<_Ts...>(
				__tuple_make_base<__tuple_select_base<_Ts...>>(
					typename __tuple_extract_sequence<
						__tuple_select_base<_Ts...>>::type{},
					std::forward<_InTs>(__args)...))
		{}
		pf_decl_constexpr
		tuple(tuple<_Ts...> const &__r) = default;
		pf_decl_constexpr
		tuple(tuple<_Ts...> &&__r) = default;

		/// Operator=
		pf_decl_constexpr tuple<_Ts...> &
		operator=(tuple<_Ts...> const &__r) = default;
		pf_decl_constexpr tuple<_Ts...> &
		operator=(tuple<_Ts...> &&__r) = default;
	};

	/// TUPLE: SFINAE -> Is Tuple
	template<typename _Tuple>
	struct is_tuple : std::false_type
	{};
	template<typename ... _Ts>
	struct is_tuple<tuple<_Ts...>> : std::true_type
	{};
	template<typename ... _Ts>
	struct is_tuple<__tuple_base<_Ts...>> : std::true_type
	{};
	template<typename _Tuple>
	pf_decl_static pf_decl_constexpr bool is_tuple_v = is_tuple<_Tuple>::value;

	/// TUPLE: SFINAE -> Memory Type
	template<typename _Tuple>
	struct tuple_memory_type;
	template<typename _Tuple>
	using tuple_memory_type_t = typename tuple_memory_type<_Tuple>::type;
	template<typename ... _Ts>
	struct tuple_memory_type<tuple<_Ts...>>
	{
		using type = __tuple_select_base<_Ts...>;
	};

	/// TUPLE: (Index) Get
	template<size_t _Index, typename _Tuple>
	pf_hint_nodiscard pf_decl_constexpr auto &
	i_get(_Tuple &__tuple) pf_attr_noexcept
	requires(is_tuple_v<_Tuple> && _Index == tuple_at<_Tuple, 0>::type::index)
	{
		return __tuple.elem_.data_;
	}
	template<size_t _Index, typename _Tuple>
	pf_hint_nodiscard pf_decl_constexpr auto &
	i_get(_Tuple &__tuple) pf_attr_noexcept
	requires(is_tuple_v<_Tuple> && _Index != tuple_at<_Tuple, 0>::type::index && tuple_size_v<_Tuple> > 1)
	{
		return i_get<_Index>(__tuple.rest_);
	}
	template<size_t _Index, typename _Tuple>
	pf_hint_nodiscard pf_decl_constexpr const auto &
	i_get(_Tuple const &__tuple) pf_attr_noexcept
	requires(is_tuple_v<_Tuple> && _Index == tuple_at<_Tuple, 0>::type::index)
	{
		return __tuple.elem_.data_;
	}
	template<size_t _Index, typename _Tuple>
	pf_hint_nodiscard pf_decl_constexpr const auto &
	i_get(_Tuple const &__tuple) pf_attr_noexcept
	requires(is_tuple_v<_Tuple> && _Index != tuple_at<_Tuple, 0>::type::index && tuple_size_v<_Tuple> > 1)
	{
		return i_get<_Index>(__tuple.rest_);
	}
	template<size_t _Index, typename _Tuple>
	pf_hint_nodiscard pf_decl_constexpr auto &&
	i_get(_Tuple &&__tuple) pf_attr_noexcept
	requires(is_tuple_v<_Tuple> && _Index == tuple_at<_Tuple, 0>::type::index)
	{
		return std::move(__tuple.elem_.data_);
	}
	template<size_t _Index, typename _Tuple>
	pf_hint_nodiscard pf_decl_constexpr auto &&
	i_get(_Tuple &&__tuple) pf_attr_noexcept
	requires(is_tuple_v<_Tuple> && _Index != tuple_at<_Tuple, 0>::type::index && tuple_size_v<_Tuple> > 1)
	{
		return std::move(i_get<_Index>(__tuple.rest_));
	}
	template<size_t _Index, typename _Tuple>
	pf_hint_nodiscard pf_decl_constexpr const auto &&
	i_get(_Tuple const &&__tuple) pf_attr_noexcept
	requires(is_tuple_v<_Tuple> && _Index == tuple_at<_Tuple, 0>::type::index)
	{
		return std::move(__tuple.elem_.data_);
	}
	template<size_t _Index, typename _Tuple>
	pf_hint_nodiscard pf_decl_constexpr const auto &&
	i_get(_Tuple const &&__tuple) pf_attr_noexcept
	requires(is_tuple_v<_Tuple> && _Index != tuple_at<_Tuple, 0>::type::index && tuple_size_v<_Tuple> > 1)
	{
		return std::move(i_get<_Index>(__tuple.rest_));
	}

	/// TUPLE: (Index) Offsetof
	template<size_t _Index, typename _Tuple>
	pf_hint_nodiscard pf_decl_constexpr size_t
	i_offsetof(_Tuple const &__tuple)
	pf_attr_noexcept
	requires(is_tuple_v<_Tuple>)
	{
		return addressof(&i_get<_Index>(__tuple)) - addressof(&__tuple);
	}

	/// TUPLE: (Index) Sizeof
	template<size_t _Index, typename _Tuple>
	pf_hint_nodiscard pf_decl_constexpr size_t
	i_sizeof(_Tuple const &__tuple)
	pf_attr_noexcept
	requires(is_tuple_v<_Tuple>)
	{
		return sizeof(std::remove_reference_t<decltype(i_get<_Index>(__tuple))>);
	}

	/// TUPLE: (Memory) Get
	template<size_t _Index, typename _Tuple>
	pf_hint_nodiscard pf_decl_constexpr auto &
	m_get(_Tuple &__tuple) pf_attr_noexcept
	requires(is_tuple_v<_Tuple> && _Index == 0 && tuple_size_v<_Tuple> > 0)
	{
		return __tuple.elem_.data_;
	}
	template<size_t _Index, typename _Tuple>
	pf_hint_nodiscard pf_decl_constexpr auto &
	m_get(_Tuple &__tuple) pf_attr_noexcept
	requires(is_tuple_v<_Tuple> && _Index != 0 && tuple_size_v<_Tuple> > _Index)
	{
		return m_get<_Index - 1>(__tuple.rest_);
	}
	template<size_t _Index, typename _Tuple>
	pf_hint_nodiscard pf_decl_constexpr const auto &
	m_get(_Tuple const &__tuple) pf_attr_noexcept
	requires(is_tuple_v<_Tuple> && _Index == 0 && tuple_size_v<_Tuple> > 0)
	{
		return __tuple.elem_.data_;
	}
	template<size_t _Index, typename _Tuple>
	pf_hint_nodiscard pf_decl_constexpr const auto &
	m_get(_Tuple const &__tuple) pf_attr_noexcept
	requires(is_tuple_v<_Tuple> && _Index != 0 && tuple_size_v<_Tuple> > _Index)
	{
		return m_get<_Index - 1>(__tuple.rest_);
	}
	template<size_t _Index, typename _Tuple>
	pf_hint_nodiscard pf_decl_constexpr auto &&
	m_get(_Tuple &&__tuple) pf_attr_noexcept
	requires(is_tuple_v<_Tuple> && _Index == 0 && tuple_size_v<_Tuple> > 0)
	{
		return std::move(__tuple.elem_.data_);
	}
	template<size_t _Index, typename _Tuple>
	pf_hint_nodiscard pf_decl_constexpr auto &&
	m_get(_Tuple &&__tuple) pf_attr_noexcept
	requires(is_tuple_v<_Tuple> && _Index != 0 && tuple_size_v<_Tuple> > _Index)
	{
		return std::move(m_get<_Index - 1>(__tuple.rest_));
	}
	template<size_t _Index, typename _Tuple>
	pf_hint_nodiscard pf_decl_constexpr const auto &&
	m_get(_Tuple const &&__tuple) pf_attr_noexcept
	requires(is_tuple_v<_Tuple> && _Index == 0 && tuple_size_v<_Tuple> > 0)
	{
		return std::move(__tuple.elem_.data_);
	}
	template<size_t _Index, typename _Tuple>
	pf_hint_nodiscard pf_decl_constexpr const auto &&
	m_get(_Tuple const &&__tuple) pf_attr_noexcept
	requires(is_tuple_v<_Tuple> && _Index != 0 && tuple_size_v<_Tuple> > _Index)
	{
		return std::move(m_get<_Index - 1>(__tuple.rest_));
	}

	/// TUPLE: (Memory) Offsetof
	template<size_t _Index, typename _Tuple>
	pf_hint_nodiscard pf_decl_constexpr size_t
	m_offsetof(_Tuple const &__tuple)
	pf_attr_noexcept
	requires(is_tuple_v<_Tuple> && tuple_size_v<_Tuple> > _Index)
	{
		return addressof(&m_get<_Index>(__tuple)) - addressof(&__tuple);
	}

	/// TUPLE: (Memory) Sizeof
	template<size_t _Index, typename _Tuple>
	pf_hint_nodiscard pf_decl_constexpr size_t
	m_sizeof(_Tuple const &__tuple)
	pf_attr_noexcept
	requires(is_tuple_v<_Tuple> && tuple_size_v<_Tuple> > _Index)
	{
		return sizeof(std::remove_reference_t<decltype(m_get<_Index>(__tuple))>);
	}

	/// TUPLE: Apply
	template<typename _Fun, typename _Tuple, size_t... _Is>
	pf_hint_nodiscard pf_decl_constexpr auto
	__tuple_apply(_Fun &&__fun, _Tuple &&__tuple, index_sequence<_Is...>)
	{
		return std::invoke(std::forward<_Fun>(__fun),
											 i_get<_Is>(std::forward<_Tuple>(__tuple))...);
	}
	template<typename _Fun, typename _Tuple>
	pf_hint_nodiscard pf_decl_constexpr auto
	tuple_apply(_Fun &&__fun,
							_Tuple &&__tuple)
	{
		return __tuple_apply(std::forward<_Fun>(__fun), std::forward<_Tuple>(__tuple), make_index_sequence<tuple_size_v<_Tuple>>{});
	}



  /// PAIR:

	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wpedantic"

  /// PAIR: Impl
  template <typename _TyA, typename _TyB> 
  union pair 
  {
    /// Constructors
    pf_decl_constexpr pair() pf_attr_noexcept = default;
    pf_decl_constexpr pair(
      _TyA &&__a, 
      _TyB &&__b) pf_attr_noexcept
        : a(std::move<_TyA>(__a)),
          b(std::move<_TyB>(__b)) {}
    template <typename _InTyA, typename _InTyB>
    pf_decl_constexpr pair(
      _InTyA &&__a, 
    _InTyB &&__b) pf_attr_noexcept
      requires(std::is_convertible_v<_InTyA, _TyA> &&
              std::is_convertible_v<_InTyB, _TyB>)
        : a(std::move<_InTyA>(__a)), b(std::move<_InTyB>(__b)) {}
    pf_decl_constexpr pair(pair<_TyA, _TyB> const &) pf_attr_noexcept = default;
    pf_decl_constexpr pair(pair<_TyA, _TyB> &&) pf_attr_noexcept = default;

    /// Destructor
    pf_decl_constexpr ~pair() pf_attr_noexcept = default;

    /// Store
    union {
      struct {
        _TyA x;
        _TyB y;
      };
      struct {
        _TyA r;
        _TyB g;
      };
      struct {
        _TyA a;
        _TyB b;
      };
    };
  };

  /// PAIR: Make
  template <typename _TyA, typename _TyB>
  pf_decl_constexpr pair<_TyA, _TyB> make_pair(_TyA &&__a,
                                              _TyB &&__b) pf_attr_noexcept {
    return pair(std::forward<_TyA>(__a), std::forward<_TyB>(__b));
  }

	#pragma GCC diagnostic pop
	#pragma GCC diagnostic ignored "-Wpedantic"




  /// NUPLE: Macro -> $
#define $(name) xx::hash(name)

	/// NUPLE: SFINAE -> Index
	template <typename _Ty, size_t _Hash, size_t _Index>
	struct __nuple_index : public __tuple_index<_Ty, _Index>
	{
		pf_decl_static pf_decl_constexpr size_t hash = _Hash;
		using type = _Ty;
	};

	/// NUPLE: SFINAE -> Is Nuple Index
	template <typename _Ty>
	struct __is_nuple_index : std::false_type
	{};
	template <typename _Ty, size_t _Hash, size_t _Index>
	struct __is_nuple_index<__nuple_index<_Ty, _Hash, _Index>> : std::true_type
	{};
	template <typename _Ty>
	pf_decl_static pf_decl_constexpr bool __is_nuple_index_v = __is_nuple_index<_Ty>::value;

	/// NUPLE: Nutag
	template <typename _Ty, size_t _Hash>
	struct nutag
	{
		using type = _Ty;
		pf_decl_static pf_decl_constexpr size_t hash = _Hash;

		/// Constructors
		pf_decl_constexpr
		nutag() pf_attr_noexcept = default;
		pf_decl_constexpr nutag(_Ty &&__val)
			: val_(std::forward<_Ty>(__val))
		{}
		pf_decl_constexpr
		nutag(nutag<_Ty, _Hash> const &) pf_attr_noexcept = delete;
		pf_decl_constexpr
		nutag(nutag<_Ty, _Hash> &&) pf_attr_noexcept			= delete;

		/// Store
		_Ty val_;
	};

	/// NUPLE: Nutag -> Nuval
	template <size_t _Hash, typename _Ty>
	pf_hint_nodiscard pf_decl_constexpr nutag<_Ty, _Hash> nuval(
		_Ty &&__val)
	{
		return nutag<_Ty, _Hash>(std::forward<_Ty>(__val));
	}

	/// NUPLE: SFINAE -> Is Nutag
	template <typename _Ty>
	struct is_nuple_nutag : std::false_type
	{};
	template <typename _Ty, size_t _Hash>
	struct is_nuple_nutag<nutag<_Ty, _Hash>> : std::true_type
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
	template <typename ..._Ts>
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
	template <typename ..._Ts>
	using __nuple_select_base = typename __tuple_selection_sort<
		typename __nuple_make_indexed_sequence<std::remove_reference_t<_Ts>...>::tuple_type,
		__tuple_descending_order_alignment>::tuple_type;

	/// NUPLE: Impl
	template <typename ..._Ts>
	requires(is_nuple_nutag_v<_Ts>&& ...)
	struct nuple : public __nuple_select_base<_Ts ...>
	{
		using memory_type = __tuple_select_base<_Ts ...>;

		/// Constructors
		pf_decl_constexpr
		nuple() pf_attr_noexcept = default;
		pf_decl_constexpr nuple(_Ts && ... __args)
			: __nuple_select_base<_Ts ...>(
				__tuple_make_base<__nuple_select_base<_Ts ...>>(
					typename __tuple_extract_sequence<__tuple_select_base<_Ts ...>>::type{},
					__args.val_ ...))
		{}
		template <typename ..._InTs>
		pf_decl_constexpr nuple(_InTs && ... __args)
			: __nuple_select_base<_Ts ...>(
				__tuple_make_base<__nuple_select_base<_Ts ...>>(
					typename __tuple_extract_sequence<__tuple_select_base<_Ts ...>>::type{},
					std::forward<_InTs>(__args)...))
		{}
		pf_decl_constexpr
		nuple(nuple<_Ts ...> const &) = default;
		pf_decl_constexpr
		nuple(nuple<_Ts ...> &&)			= default;

		/// Operator=
		pf_decl_constexpr nuple<_Ts ...> &
		operator =(nuple<_Ts ...> const &__r) = default;
		pf_decl_constexpr nuple<_Ts ...> &
		operator =(nuple<_Ts ...> &&__r)			= default;
	};

	/// NUPLE: SFINAE -> Tuple Size
	template <typename ..._Ts>
	struct tuple_size<nuple<_Ts ...>> : std::integral_constant<size_t, sizeof...(_Ts)>
	{};

	/// NUPLE: SFINAE -> Tuple At
	template <size_t _Index, typename ..._Ts>
	struct tuple_at<nuple<_Ts ...>, _Index>
	{
		using type = tuple_at<__nuple_select_base<_Ts ...>, _Index>::type;
	};

	/// NUPLE: SFINAE -> Is Tuple
	template <typename ..._Ts>
	struct is_tuple<nuple<_Ts ...>> : std::true_type
	{};

	/// NUPLE: SFINAE -> Is Nuple
	template <typename _Ty>
	struct is_nuple : std::false_type
	{};
	template <typename ..._Ts>
	requires(__is_nuple_index_v<_Ts>&& ...)
	struct is_nuple<__tuple_base<_Ts ...>> : std::true_type
	{};
	template <typename ..._Ts>
	struct is_nuple<nuple<_Ts ...>> : std::true_type
	{};
	template <typename _Ty>
	pf_decl_static pf_decl_constexpr bool is_nuple_v = is_nuple<_Ty>::value;

	/// NUPLE: SFINAE -> Memory Type
	template <typename ..._Ts>
	struct tuple_memory_type<nuple<_Ts ...>>
	{
		using type = __nuple_select_base<_Ts ...>;
	};

	/// NUPLE: (Named) Get
	template <size_t _Hash, typename _Nuple>
	pf_hint_nodiscard pf_decl_constexpr auto &n_get(
		_Nuple &__nuple) pf_attr_noexcept
	requires(is_nuple_v<_Nuple> && _Hash == tuple_at<_Nuple, 0>::type::hash)
	{
		return __nuple.elem_.data_;
	}
	template <size_t _Hash, typename _Nuple>
	pf_hint_nodiscard pf_decl_constexpr auto &n_get(
		_Nuple &__nuple) pf_attr_noexcept
	requires(is_nuple_v<_Nuple> && _Hash != tuple_at<_Nuple, 0>::type::hash && tuple_size_v<_Nuple> > 1)
	{
		return n_get<_Hash>(__nuple.rest_);
	}
	template <size_t _Hash, typename _Nuple>
	pf_hint_nodiscard pf_decl_constexpr const auto &n_get(
		_Nuple const &__nuple) pf_attr_noexcept
	requires(is_nuple_v<_Nuple> && _Hash == tuple_at<_Nuple, 0>::type::hash)
	{
		return __nuple.elem_.data_;
	}
	template <size_t _Hash, typename _Nuple>
	pf_hint_nodiscard pf_decl_constexpr const auto &n_get(
		_Nuple const &__nuple) pf_attr_noexcept
	requires(is_nuple_v<_Nuple> && _Hash != tuple_at<_Nuple, 0>::type::hash && tuple_size_v<_Nuple> > 1)
	{
		return n_get<_Hash>(__nuple.rest_);
	}
	template <size_t _Hash, typename _Nuple>
	pf_hint_nodiscard pf_decl_constexpr auto &&n_get(
		_Nuple &&__nuple) pf_attr_noexcept
	requires(is_nuple_v<_Nuple> && _Hash == tuple_at<_Nuple, 0>::type::hash)
	{
		return std::move(__nuple.elem_.data_);
	}
	template <size_t _Hash, typename _Nuple>
	pf_hint_nodiscard pf_decl_constexpr auto &&n_get(
		_Nuple &&__nuple) pf_attr_noexcept
	requires(is_nuple_v<_Nuple> && _Hash != tuple_at<_Nuple, 0>::type::hash && tuple_size_v<_Nuple> > 1)
	{
		return std::move(n_get<_Hash>(__nuple.rest_));
	}
	template <size_t _Hash, typename _Nuple>
	pf_hint_nodiscard pf_decl_constexpr const auto &&n_get(
		_Nuple const &&__nuple) pf_attr_noexcept
	requires(is_nuple_v<_Nuple> && _Hash == tuple_at<_Nuple, 0>::type::hash)
	{
		return std::move(__nuple.elem_.data_);
	}
	template <size_t _Hash, typename _Nuple>
	pf_hint_nodiscard pf_decl_constexpr const auto &&n_get(
		_Nuple const &&__nuple) pf_attr_noexcept
	requires(is_nuple_v<_Nuple> && _Hash != tuple_at<_Nuple, 0>::type::hash && tuple_size_v<_Nuple> > 1)
	{
		return std::move(n_get<_Hash>(__nuple.rest_));
	}

	/// NUPLE: (Named) Offsetof
	template <size_t _Hash, typename _Nuple>
	pf_hint_nodiscard pf_decl_constexpr size_t n_offsetof(_Nuple const &__nuple) pf_attr_noexcept
	requires(is_nuple_v<_Nuple>)
	{
		return addressof(&n_get<_Hash>(__nuple)) - addressof(&__nuple);
	}

	/// NUPLE: (Named) Sizeof
	template <size_t _Hash, typename _Nuple>
	pf_hint_nodiscard pf_decl_constexpr size_t n_sizeof(_Nuple const &__nuple) pf_attr_noexcept
	requires(is_nuple_v<_Nuple>)
	{
		return sizeof(std::remove_reference_t<decltype(n_get<_Hash>(__nuple))>);
	}
}

#endif // !PULSAR_TUPLE_HPP