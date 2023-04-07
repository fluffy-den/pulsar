/*! @file   algorithm.hpp
 *  @author Louis-Quentin Noé (noe.louis-quentin@hotmail.fr)
 *  @brief
 *  @date   05-03-2023
 *
 *  @copyright Copyright (c) 2023 - Pulsar Software
 *
 *  @since 0.1.4
 */

#ifndef PULSAR_ALGORITHM_HPP
#define PULSAR_ALGORITHM_HPP 1

// Include: Pulsar
#include "pulsar/iterator.hpp"

// Pulsar
namespace pul
{
	/// ALGORITHM:: Swap
	template <
		typename _Ty>
	pf_decl_constexpr void swap(
		_Ty &__l,
		_Ty &__r) pf_attr_noexcept
	requires(std::is_move_assignable_v<_Ty>)
	{
		_Ty tmp = std::move(__l);
		__l = std::move(__r);
		__r = std::move(tmp);
	}
	template <
		typename _Iterable>
	pf_decl_constexpr void swap(
		_Iterable &__l,
		_Iterable &__r) pf_attr_noexcept
	requires(is_swappable_v<_Iterable>)
	{
		__l.swap(__r);
	}

	/// ALGORITHM:: Copy
	template <
		typename _IteratorIn,
		typename _IteratorOut>
	pf_decl_constexpr void copy_forward(
		_IteratorIn __beg,
		_IteratorIn __end,
		_IteratorOut __dst) pf_attr_noexcept
	requires(is_iterator_v<_IteratorIn>
					 && is_iterator_v<_IteratorOut>)
	{
		for (; __beg != __end; ++__beg, ++__dst) *__dst = *__beg;
	}
	template <
		typename _IteratorIn,
		typename _IteratorOut>
	pf_decl_constexpr void copy_reverse(_IteratorIn __beg, _IteratorIn __end,
																			_IteratorOut __dst)
	requires(is_iterator_v<_IteratorIn>
					 && is_iterator_v<_IteratorOut>)
	{
		--__beg;
		--__end;
		__dst += countof(__beg, __end);
		for (; __end != __beg; --__end, --__dst) *__dst = *__end;
	}
	template <
		typename _IteratorIn,
		typename _IteratorOut>
	pf_decl_constexpr void copy(
		_IteratorIn __beg,
		_IteratorIn __end,
		_IteratorOut __dst) pf_attr_noexcept
	requires(is_sequence_iterator_v<_IteratorIn> &&
					 is_sequence_iterator_v<_IteratorOut>)
	{
		if ((__beg < __dst) && (__dst < __end))
		{
			copy_reverse(__beg, __end, __dst);
		}
		else
		{
			copy_forward(__beg, __end, __dst);
		}
	}
	template <
		typename _IteratorIn,
		typename _IteratorOut>
	pf_decl_constexpr void copy(
		_IteratorIn __beg,
		_IteratorIn __end,
		_IteratorOut __dst) pf_attr_noexcept
	requires(!is_sequence_iterator_v<_IteratorIn> ||
					 !is_sequence_iterator_v<_IteratorOut>)
	{
		copy_forward(__beg, __end, __dst);
	}

	/// ALGORITHM:: Move
	template <
		typename _IteratorIn,
		typename _IteratorOut>
	pf_decl_constexpr void move_forward(
		_IteratorIn __beg,
		_IteratorIn __end,
		_IteratorOut __dst) pf_attr_noexcept
	requires(is_iterator_v<_IteratorIn> && is_iterator_v<_IteratorOut>)
	{
		for (; __beg != __end; ++__beg, ++__dst) *__dst = std::move(*__beg);
	}
	template <
		typename _IteratorIn,
		typename _IteratorOut>
	pf_decl_constexpr void move_reverse(
		_IteratorIn __beg,
		_IteratorIn __end,
		_IteratorOut __dst) pf_attr_noexcept
	requires(is_iterator_v<_IteratorIn> && is_iterator_v<_IteratorOut>)
	{
		--__beg;
		--__end;
		__dst += distof(__beg, __end);
		for (; __end != __beg; --__end, --__dst) __dst = std::move(*__beg);
	}
	template <
		typename _IteratorIn,
		typename _IteratorOut>
	pf_decl_constexpr void move(
		_IteratorIn __beg,
		_IteratorIn __end,
		_IteratorOut __dst) pf_attr_noexcept
	requires(is_iterator_v<_IteratorIn> && is_iterator_v<_IteratorOut>)
	{
		if ((__beg < __dst) && (__dst < __end)) {
			move_reverse(__beg, __end, __dst);
		} else {
			move_forward(__beg, __end, __dst);
		}
	}

	/// ALGORITHM: Fill
	template <
		typename _IteratorOut>
	pf_decl_constexpr void fill(
		_IteratorOut __beg,
		_IteratorOut __end,
		typename _IteratorOut::value_t const &__val) pf_attr_noexcept
	requires(is_iterator_v<_IteratorOut>)
	{
		for (; __beg != __end; ++__beg) *__beg = __val;
	}

	/// ALGORITHM: Distance Of
	template <
		typename _IteratorIn>
	pf_hint_nodiscard pf_decl_constexpr size_t
	distof(
		_IteratorIn __beg,
		_IteratorIn __end) pf_attr_noexcept
	requires(is_sequence_iterator_v<_IteratorIn>)
	{
		return distof(__beg.get(), __end.get());
	}

	/// ALGORITHM: Count
	template <
		typename _IteratorIn>
	pf_hint_nodiscard pf_decl_constexpr size_t
	countof(
		_IteratorIn __beg,
		_IteratorIn __end) pf_attr_noexcept
	requires(!is_sequence_iterator_v<_IteratorIn>)
	{
		size_t c = 0;
		while (__beg != __end) {
			++__beg;
			++c;
		}
		return c;
	}
	template <
		typename _IteratorIn>
	pf_hint_nodiscard pf_decl_constexpr size_t
	countof(
		_IteratorIn __beg,
		_IteratorIn __end) pf_attr_noexcept
	requires(is_sequence_iterator_v<_IteratorIn>)
	{
		return countof(__beg.get(), __end.get());
	}

	/// ALGORITHM: Search
	template <
		typename _IteratorInA,
		typename _IteratorInB>
	pf_hint_nodiscard pf_decl_constexpr _IteratorInA search(
		_IteratorInA __ib,
		_IteratorInA __eb,
		_IteratorInB __ob,
		_IteratorInB __oe) pf_attr_noexcept
	requires(is_iterator_v<_IteratorInA>
					 && is_iterator_v<_IteratorInB>)
	{
		while (1) {
			_IteratorInA it = __ib;
			for (_IteratorInB k = __ob; *it != *k; ++it, ++k)
			{
				if (k == __oe)
					return __ib;
				if (it == __eb)
					return __eb;
			}
			++__ib;
		}
	}
	template <
		typename _IteratorInA,
		typename _IteratorInB,
		typename _BinaryPredicate>
	pf_hint_nodiscard pf_decl_constexpr _IteratorInA
	search(
		_IteratorInA __ib,
		_IteratorInA __eb,
		_IteratorInB __ob,
		_IteratorInB __oe,
		_BinaryPredicate &&__p) pf_attr_noexcept
	requires(is_iterator_v<_IteratorInA>
					 && is_iterator_v<_IteratorInB>)
	{
		while (1) {
			_IteratorInA it = __ib;
			for (_IteratorInB k = __ob; *it != __p(*it, *k); ++it, ++k)
			{
				if (k == __oe)
					return __ib;
				if (it == __eb)
					return __eb;
			}
			++__ib;
		}
	}

	/// ALGORITHM: Accumulate
	template <
		typename _IteratorIn,
		typename _Ty>
	pf_hint_nodiscard pf_decl_constexpr _Ty accumulate(
		_IteratorIn __beg,
		_IteratorIn __end,
		_Ty __val) pf_attr_noexcept
	requires(is_iterator_v<_IteratorIn>)
	{
		for (; __beg != __end; ++__beg) {
			__val = std::move(__val) + *__beg;
		}
		return __val;
	}
	template <
		typename _IteratorIn,
		typename _Ty,
		typename _BinaryOperation>
	pf_hint_nodiscard pf_decl_constexpr _Ty accumulate(
		_IteratorIn __beg,
		_IteratorIn __end, _Ty __val,
		_BinaryOperation &&__p) pf_attr_noexcept
	requires(is_iterator_v<_IteratorIn>)
	{
		for (; __beg != __end; ++__beg) {
			__val = __p(std::move(__val), *__beg);
		}
		return __val;
	}

	/// ALGORITHM: Transform
	template <
		typename _IteratorIn,
		typename _IteratorOut,
		typename _UnaryOperation>
	pf_hint_nodiscard pf_decl_constexpr _IteratorOut
	transform(
		_IteratorIn __ib,
		_IteratorIn __ie,
		_IteratorOut __ob,
		_UnaryOperation &&__op) pf_attr_noexcept
	requires(is_iterator_v<_IteratorIn>
					 && is_iterator_v<_IteratorOut>)
	{
		while (__ib != __ie) {
			*__ob++ = __op(*__ib++);
		}
		return __ob;
	}
	template <
		typename _IteratorIn,
		typename _IteratorOut,
		typename _BinaryOperation>
	pf_hint_nodiscard pf_decl_constexpr _IteratorOut
	transform(
		_IteratorIn __i1b,
		_IteratorIn __ie,
		_IteratorIn __i2b,
		_IteratorOut __eb,
		_BinaryOperation &&__op)  pf_attr_noexcept
	requires(is_iterator_v<_IteratorIn>
					 && is_iterator_v<_IteratorOut>)
	{
		while (__i1b != __ie) {
			*__eb++ = __op(*__i1b++, *__i2b++);
		}
		return __eb;
	}

	/// ALGORITHM: Compare
	template <
		typename _IteratorInA,
		typename _IteratorInB>
	pf_hint_nodiscard pf_decl_constexpr bool
	equal(
		_IteratorInA __ba,
		_IteratorInA __ea,
		_IteratorInB __bb) pf_attr_noexcept
	requires(is_iterator_v<_IteratorInA>
					 && is_iterator_v<_IteratorInB>)
	{
		for (; __ba != __ea; ++__ba, ++__bb) if (*__ba != *__bb) return false;
		return true;
	}
	template <
		typename _IteratorInA,
		typename _Uy>
	pf_hint_nodiscard pf_decl_constexpr bool
	equal(
		_IteratorInA __ba,
		_IteratorInA __ea,
		_Uy const &__val)
	requires(is_iterator_v<_IteratorInA>
					 && !is_iterator_v<_Uy>)// TODO: is_equality_comparable_v<_IteratorInA, _Uy>
	{
		for (; __ba != __ea; ++__ba) if (*__ba != __val) return false;
		return true;
	}
	template <
		typename _IteratorInA,
		typename _IteratorInB>
	pf_hint_nodiscard pf_decl_constexpr bool
	not_equal(
		_IteratorInA __ba,
		_IteratorInA __ea,
		_IteratorInB __bb) pf_attr_noexcept
	requires(is_iterator_v<_IteratorInA>
					 && is_iterator_v<_IteratorInB>)
	{
		return !equal(__ba, __ea, __bb);
	}
	template <
		typename _IteratorInA,
		typename _Uy>
	pf_hint_nodiscard pf_decl_constexpr bool
	not_equal(
		_IteratorInA __ba,
		_IteratorInA __ea,
		_Uy const &__val)
	requires(is_iterator_v<_IteratorInA>
					 && !is_iterator_v<_Uy>)// TODO: is_equality_comparable_v<_IteratorInA, _Uy>
	{
		return !equal(__ba, __ea, __val);
	}
	template <
		typename _IteratorInA,
		typename _IteratorInB>
	pf_hint_nodiscard pf_decl_constexpr bool
	greater(
		_IteratorInA __ba,
		_IteratorInA __ea,
		_IteratorInB __bb) pf_attr_noexcept
	requires(is_iterator_v<_IteratorInA>
					 && is_iterator_v<_IteratorInB>)
	{
		for (; __ba != __ea; ++__ba, ++__bb) if (*__ba <= *__bb) return false;
		return true;
	}
	template <
		typename _IteratorInA,
		typename _Uy>
	pf_hint_nodiscard pf_decl_constexpr bool
	greater(
		_IteratorInA __ba,
		_IteratorInA __ea,
		_Uy const &__val) pf_attr_noexcept
	requires(is_iterator_v<_IteratorInA>
					 && !is_iterator_v<_Uy>)// TODO: is_equality_comparable_v<_IteratorInA, _Uy>
	{
		for (; __ba != __ea; ++__ba) if (*__ba <= __val) return false;
		return true;
	}
	template <
		typename _IteratorInA,
		typename _IteratorInB>
	pf_hint_nodiscard pf_decl_constexpr bool
	greater_equal(
		_IteratorInA __ba,
		_IteratorInA __ea,
		_IteratorInB __bb) pf_attr_noexcept
	requires(is_iterator_v<_IteratorInA>
					 && is_iterator_v<_IteratorInB>)
	{
		for (; __ba != __ea; ++__ba, ++__bb) if (*__ba < *__bb) return false;
		return true;
	}
	template <
		typename _IteratorInA,
		typename _Uy>
	pf_hint_nodiscard pf_decl_constexpr bool
	greater_equal(
		_IteratorInA __ba,
		_IteratorInA __ea,
		_Uy const &__val) pf_attr_noexcept
	requires(is_iterator_v<_IteratorInA>
					 && !is_iterator_v<_Uy>)// TODO: is_equality_comparable_v<_IteratorInA, _Uy>
	{
		for (; __ba != __ea; ++__ba) if (*__ba < __val) return false;
		return true;
	}
	template <
		typename _IteratorInA,
		typename _IteratorInB>
	pf_hint_nodiscard pf_decl_constexpr bool
	smaller(
		_IteratorInA __ba,
		_IteratorInA __ea,
		_IteratorInB __bb) pf_attr_noexcept
	requires(is_iterator_v<_IteratorInA>
					 && is_iterator_v<_IteratorInB>)
	{
		return !greater_equal(__ba, __ea, __bb);
	}
	template <
		typename _IteratorInA,
		typename _Uy>
	pf_hint_nodiscard pf_decl_constexpr bool
	smaller(
		_IteratorInA __ba,
		_IteratorInA __ea,
		_Uy const& __val) pf_attr_noexcept
	requires(is_iterator_v<_IteratorInA>
					 && !is_iterator_v<_Uy>)// TODO: is_equality_comparable_v<_IteratorInA, _Uy>
	{
		return !greater_equal(__ba, __ea, __val);
	}
	template <
		typename _IteratorInA,
		typename _IteratorInB>
	pf_hint_nodiscard pf_decl_constexpr bool
	smaller_equal(
		_IteratorInA __ba,
		_IteratorInA __ea,
		_IteratorInB __bb) pf_attr_noexcept
	requires(is_iterator_v<_IteratorInA>
					 && is_iterator_v<_IteratorInB>)
	{
		return !greater(__ba, __ea, __bb);
	}
	template <
		typename _IteratorInA,
		typename _Uy>
	pf_hint_nodiscard pf_decl_constexpr bool
	smaller_equal(
		_IteratorInA __ba,
		_IteratorInA __ea,
		_Uy const& __val) pf_attr_noexcept
	requires(is_iterator_v<_IteratorInA>
					 && !is_iterator_v<_Uy>)// TODO: is_equality_comparable_v<_IteratorInA, _Uy>
	{
		return !greater(__ba, __ea, __val);
	}

	/// ALGORITHM: Find
	template <
		typename _IteratorIn,
		typename _Predicate>
	pf_hint_nodiscard pf_decl_constexpr _IteratorIn find(
		_IteratorIn __beg,
		_IteratorIn __end,
		_Predicate  &&__p) pf_attr_noexcept
	requires(is_iterator_v<_IteratorIn>)
	{
		for (; __beg != __end; ++__beg) {
			if (__p(*__beg)) {
				return __beg;
			}
		}
		return __end;
	}

	/// ALGORITHM: Search
	// TODO: Search Algorithms

	/// ALGORITHM: Sort
	// TODO: Sort Algorithms
}

#endif // !PULSAR_ALGORITHM_HPP