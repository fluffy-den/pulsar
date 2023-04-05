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

	/// UTILITY: Begin
	template <typename _Iterable>
	pf_hint_nodiscard pf_decl_constexpr typename _Iterable::iterator_t
	begin(
		_Iterable &__r) pf_attr_noexcept
	requires(is_iterable_v<_Iterable>)
	{
		return __r.begin();
	}
	template <typename _Iterable>
	pf_hint_nodiscard pf_decl_constexpr typename _Iterable::const_iterator_t
	begin(
		_Iterable const &__r) pf_attr_noexcept
	requires(is_const_iterable_v<_Iterable>)
	{
		return __r.begin();
	}
	template <typename _Iterable>
	pf_hint_nodiscard pf_decl_constexpr typename _Iterable::const_iterator_t
	cbegin(
		_Iterable const &__r) pf_attr_noexcept
	requires(is_const_iterable_v<_Iterable>)
	{
		return __r.cbegin();
	}
	template <typename _Ty, size_t _Num>
	pf_hint_nodiscard pf_decl_constexpr iterator<_Ty>
	begin(
		_Ty (&__arr)[_Num]) pf_attr_noexcept
	{
		return &__arr[0];
	}
	template <typename _Ty, size_t _Num>
	pf_hint_nodiscard pf_decl_constexpr const_iterator<_Ty>
	begin(
		const _Ty (&__arr)[_Num]) pf_attr_noexcept
	{
		return &__arr[0];
	}
	template <typename _Ty, size_t _Num>
	pf_hint_nodiscard pf_decl_constexpr const_iterator<_Ty>
	cbegin(
		_Ty (&__arr)[_Num]) pf_attr_noexcept
	{
		return &__arr[0];
	}
	template <typename _Ty, size_t _Num>
	pf_hint_nodiscard pf_decl_constexpr const_iterator<_Ty>
	cbegin(
		const _Ty (&__arr)[_Num]) pf_attr_noexcept
	{
		return &__arr[0];
	}

	/// UTILITY: End
	template <typename _Iterable>
	pf_hint_nodiscard pf_decl_constexpr typename _Iterable::iterator_t
	end(
		_Iterable &__r) pf_attr_noexcept
	requires(is_iterable_v<_Iterable>)
	{
		return __r.end();
	}
	template <typename _Iterable>
	pf_hint_nodiscard pf_decl_constexpr typename _Iterable::const_iterator_t
	end(
		_Iterable const &__r) pf_attr_noexcept
	requires(is_const_iterable_v<_Iterable>)
	{
		return __r.end();
	}
	template <typename _Iterable>
	pf_hint_nodiscard pf_decl_constexpr typename _Iterable::const_iterator_t
	cend(
		_Iterable const &__r) pf_attr_noexcept
	requires(is_const_iterable_v<_Iterable>)
	{
		return __r.cend();
	}
	template <typename _Ty, size_t _Num>
	pf_hint_nodiscard pf_decl_constexpr iterator<_Ty>
	end(
		_Ty (&__arr)[_Num]) pf_attr_noexcept
	{
		return (&__arr[0] + _Num);
	}
	template <typename _Ty, size_t _Num>
	pf_hint_nodiscard pf_decl_constexpr const_iterator<_Ty>
	end(
		const _Ty (&__arr)[_Num]) pf_attr_noexcept
	{
		return (&__arr[0] + _Num);
	}
	template <typename _Ty, size_t _Num>
	pf_hint_nodiscard pf_decl_constexpr const_iterator<_Ty>
	cend(
		_Ty (&__arr)[_Num]) pf_attr_noexcept
	{
		return (&__arr[0] + _Num);
	}
	template <typename _Ty, size_t _Num>
	pf_hint_nodiscard pf_decl_constexpr const_iterator<_Ty>
	cend(
		const _Ty (&__arr)[_Num]) pf_attr_noexcept
	{
		return (&__arr[0] + _Num);
	}

	/// UTILITY: Reverse Begin
	template <typename _Iterable>
	pf_hint_nodiscard pf_decl_constexpr typename _Iterable::reverse_iterator_t
	rbegin(
		_Iterable &__r) pf_attr_noexcept
	requires(is_reverse_iterable_v<_Iterable>)
	{
		return __r.rbegin();
	}
	template <typename _Iterable>
	pf_hint_nodiscard pf_decl_constexpr typename _Iterable::const_reverse_iterator_t
	rbegin(
		_Iterable const &__r) pf_attr_noexcept
	requires(is_const_reverse_iterable_v<_Iterable>)
	{
		return __r.rbegin();
	}
	template <typename _Iterable>
	pf_hint_nodiscard pf_decl_constexpr typename _Iterable::const_reverse_iterator_t
	crbegin(
		_Iterable const &__r) pf_attr_noexcept
	requires(is_const_reverse_iterable_v<_Iterable>)
	{
		return __r.crbegin();
	}
	template <typename _Ty, size_t _Num>
	pf_hint_nodiscard pf_decl_constexpr reverse_iterator<iterator<_Ty>>
	rbegin(
		_Ty (&__arr)[_Num]) pf_attr_noexcept
	{
		return (&__arr[0] + (_Num - 1));
	}
	template <typename _Ty, size_t _Num>
	pf_hint_nodiscard pf_decl_constexpr reverse_iterator<const_iterator<_Ty>>
	rbegin(
		const _Ty (&__arr)[_Num]) pf_attr_noexcept
	{
		return (&__arr[0] + (_Num - 1));
	}
	template <typename _Ty, size_t _Num>
	pf_hint_nodiscard pf_decl_constexpr reverse_iterator<const_iterator<_Ty>>
	crbegin(
		_Ty (&__arr)[_Num]) pf_attr_noexcept
	{
		return const_reverse_iterator(&__arr[0] + (_Num - 1));
	}
	template <typename _Ty, size_t _Num>
	pf_hint_nodiscard pf_decl_constexpr reverse_iterator<const_iterator<_Ty>>
	crbegin(
		const _Ty (&__arr)[_Num]) pf_attr_noexcept
	{
		return (&__arr[0] + (_Num - 1));
	}

	/// UTILITY: Reverse End
	template <typename _Iterable>
	pf_hint_nodiscard pf_decl_constexpr typename _Iterable::reverse_iterator_t
	rend(
		_Iterable &__r) pf_attr_noexcept
	requires(is_reverse_iterable_v<_Iterable>)
	{
		return __r.rend();
	}

	template <typename _Iterable>
	pf_hint_nodiscard pf_decl_constexpr typename _Iterable::const_reverse_iterator_t
	rend(
		_Iterable const &__r) pf_attr_noexcept
	requires(is_const_reverse_iterable_v<_Iterable>)
	{
		return __r.rend();
	}
	template <typename _Iterable>
	pf_hint_nodiscard pf_decl_constexpr typename _Iterable::const_reverse_iterator_t
	crend(
		_Iterable const &__r) pf_attr_noexcept
	requires(is_const_reverse_iterable_v<_Iterable>)
	{
		return __r.crend();
	}
	template <typename _Ty, size_t _Num>
	pf_hint_nodiscard pf_decl_constexpr reverse_iterator<iterator<_Ty>>
	rend(
		_Ty (&__arr)[_Num]) pf_attr_noexcept
	{
		return (&__arr[0] - 1);
	}
	template <typename _Ty, size_t _Num>
	pf_hint_nodiscard pf_decl_constexpr reverse_iterator<const_iterator<_Ty>>
	rend(
		const _Ty (&__arr)[_Num]) pf_attr_noexcept
	{
		return (&__arr[0] - 1);
	}
	template <typename _Ty, size_t _Num>
	pf_hint_nodiscard pf_decl_constexpr reverse_iterator<const_iterator<_Ty>>
	crend(
		_Ty (&__arr)[_Num]) pf_attr_noexcept
	{
		return (&__arr[0] - 1);
	}
	template <typename _Ty, size_t _Num>
	pf_hint_nodiscard pf_decl_constexpr reverse_iterator<const_iterator<_Ty>>
	crend(
		const _Ty (&__arr)[_Num]) pf_attr_noexcept
	{
		return (&__arr[0] - 1);
	}

	/// UTILITY: Data
	template <typename _Iterable>
	pf_hint_nodiscard pf_decl_constexpr typename _Iterable::value_t*
	data(
		_Iterable &__r) pf_attr_noexcept
	requires(is_mappable_v<_Iterable>)
	{
		return __r.data();
	}
	template <typename _Iterable>
	pf_hint_nodiscard pf_decl_constexpr const typename _Iterable::value_t*
	data(
		_Iterable const &__r) pf_attr_noexcept
	requires(is_mappable_v<_Iterable>)
	{
		return __r.data();
	}
	template <typename _Ty, size_t _Num>
	pf_hint_nodiscard pf_decl_constexpr _Ty*
	data(
		_Ty (&__arr)[_Num]) pf_attr_noexcept
	{
		return &__arr[0];
	}
	template <typename _Ty, size_t _Num>
	pf_hint_nodiscard pf_decl_constexpr const _Ty*
	data(
		const _Ty (&__arr)[_Num]) pf_attr_noexcept
	{
		return &__arr[0];
	}

	/// ALGORITHM:: Swap
	template <typename _Ty>
	pf_decl_constexpr void swap(
		_Ty &__l,
		_Ty &__r) pf_attr_noexcept
	requires(std::is_move_assignable_v<_Ty>)
	{
		_Ty tmp = std::move(__l);
		__l = std::move(__r);
		__r = std::move(tmp);
	}
	template <typename _Iterable>
	pf_decl_constexpr void swap(
		_Iterable &__l,
		_Iterable &__r) pf_attr_noexcept
	requires(is_swappable_v<_Iterable>)
	{
		__l.swap(__r);
	}

	/// ALGORITHM:: Copy
	template <typename _IteratorIn, typename _IteratorOut>
	pf_decl_constexpr void copy_forward(
		_IteratorIn __beg,
		_IteratorIn __end,
		_IteratorOut __dst) pf_attr_noexcept
	requires(is_iterator_v<_IteratorIn>
					 && is_iterator_v<_IteratorOut>)
	{
		for (; __beg != __end; ++__beg, ++__dst) *__dst = *__beg;
	}
	template <typename _IteratorIn, typename _IteratorOut>
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
	template <typename _IteratorIn, typename _IteratorOut>
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
	template <typename _IteratorIn, typename _IteratorOut>
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
	template <typename _IteratorIn, typename _IteratorOut>
	pf_decl_constexpr void move_forward(
		_IteratorIn __beg,
		_IteratorIn __end,
		_IteratorOut __dst) pf_attr_noexcept
	requires(is_iterator_v<_IteratorIn> && is_iterator_v<_IteratorOut>)
	{
		for (; __beg != __end; ++__beg, ++__dst) *__dst = std::move(*__beg);
	}
	template <typename _IteratorIn, typename _IteratorOut>
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
	template <typename _IteratorIn, typename _IteratorOut>
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
	template <typename _IteratorOut>
	pf_decl_constexpr void fill(
		_IteratorOut __beg,
		_IteratorOut __end,
		typename _IteratorOut::value_t const &__val) pf_attr_noexcept
	requires(is_iterator_v<_IteratorOut>)
	{
		for (; __beg != __end; ++__beg) *__beg = __val;
	}

	/// ALGORITHM: Dist
	template <typename _Ty>
	pf_hint_nodiscard pf_decl_constexpr diff_t
	diffof(
		const _Ty *__beg,
		const _Ty *__end) pf_attr_noexcept
	{
		return union_cast<diff_t>(__end) - union_cast<diff_t>(__beg);
	}
	template <typename _Ty>
	pf_hint_nodiscard pf_decl_constexpr size_t
	distof(
		const _Ty *__beg,
		const _Ty *__end) pf_attr_noexcept
	{
		if (pf_likely(__end >= __beg))
		{
			return union_cast<size_t>(__end) - union_cast<size_t>(__beg);
		}
		return union_cast<size_t>(__beg) - union_cast<size_t>(__end);
	}
	template <typename _IteratorIn>
	pf_hint_nodiscard pf_decl_constexpr pf_decl_inline size_t
	distof(
		_IteratorIn __beg,
		_IteratorIn __end) pf_attr_noexcept
	requires(is_sequence_iterator_v<_IteratorIn>)
	{
		return distof(__beg.get(), __end.get());
	}

	/// ALGORITHM: Count
	template <typename _IteratorIn>
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
	template <typename _IteratorIn>
	pf_hint_nodiscard pf_decl_constexpr size_t
	countof(
		_IteratorIn __beg,
		_IteratorIn __end) pf_attr_noexcept
	requires(is_sequence_iterator_v<_IteratorIn>)
	{
		return countof(__beg.get(), __end.get());
	}

	/// ALGORITHM: Search
	template <typename _IteratorIn1, typename _IteratorIn2>
	pf_hint_nodiscard pf_decl_constexpr _IteratorIn1 search(
		_IteratorIn1 __ib,
		_IteratorIn1 __eb,
		_IteratorIn2 __ob,
		_IteratorIn2 __oe) pf_attr_noexcept
	{
		while (1) {
			_IteratorIn1 it = __ib;
			for (_IteratorIn2 k = __ob; *it != *k; ++it, ++k) {
				if (k == __oe)
					return __ib;
				if (it == __eb)
					return __eb;
			}
			++__ib;
		}
		return __oe;
	}
	template <typename _IteratorIn1, typename _IteratorIn2,
						typename _BinaryPredicate>
	pf_hint_nodiscard pf_decl_constexpr _IteratorIn1
	search(
		_IteratorIn1 __ib,
		_IteratorIn1 __eb,
		_IteratorIn2 __ob,
		_IteratorIn2 __oe,
		_BinaryPredicate &&__p) pf_attr_noexcept
	{
		while (1) {
			_IteratorIn1 it = __ib;
			for (_IteratorIn2 k = __ob; *it != __p(*it, *k); ++it, ++k) {
				if (k == __oe)
					return __ib;
				if (it == __eb)
					return __eb;
			}
			++__ib;
		}
		return __oe;
	}

	/// ALGORITHM: Find
	template <
		typename _IteratorIn,
		typename _Predicate>
	pf_hint_nodiscard pf_decl_constexpr _IteratorIn find(
		_IteratorIn __beg,
		_IteratorIn __end,
		_Predicate  &&__p) pf_attr_noexcept
	{
		for (; __beg != __end; ++__beg) {
			if (__p(*__beg)) {
				return __beg;
			}
		}
		return __end;
	}

	/// ALGORITHM: Accumulate
	template <
		typename _IteratorIn,
		typename _Ty>
	pf_hint_nodiscard pf_decl_constexpr _Ty accumulate(
		_IteratorIn __beg,
		_IteratorIn __end,
		_Ty __val) pf_attr_noexcept
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
	{
		for (; __beg != __end; ++__beg) {
			__val = __p(std::move(__val), *__beg);
		}
		return __val;
	}

	/// ALGORITHM: Transform
	template <
		typename _IteratorIn,
		typename _OutIterator,
		typename _UnaryOperation>
	pf_hint_nodiscard pf_decl_constexpr _OutIterator
	transform(
		_IteratorIn __ib,
		_IteratorIn __ie,
		_OutIterator __ob,
		_UnaryOperation &&__op) pf_attr_noexcept
	{
		while (__ib != __ie) {
			*__ob++ = __op(*__ib++);
		}
		return __ob;
	}
	template <
		typename _IteratorIn,
		typename _OutIterator,
		typename _BinaryOperation>
	pf_hint_nodiscard pf_decl_constexpr _OutIterator
	transform(
		_IteratorIn __i1b,
		_IteratorIn __ie,
		_IteratorIn __i2b,
		_OutIterator __eb,
		_BinaryOperation &&__op)  pf_attr_noexcept
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
	{
		for (; __ba != __ea; ++__ba) if (*__ba != *__bb) return false;
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
	{
		return !equal(__ba, __ea, __bb);
	}
	template <
		typename _IteratorInA,
		typename _IteratorInB>
	pf_hint_nodiscard pf_decl_constexpr bool
	greater(
		_IteratorInA __ba,
		_IteratorInA __ea,
		_IteratorInB __bb) pf_attr_noexcept
	{
		for (; __ba != __ea; ++__ba) if (*__ba <= *__bb) return false;
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
	{
		for (; __ba != __ea; ++__ba) if (*__ba < *__bb) return false;
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
	{
		return !greater_equal(__ba, __ea, __bb);
	}
	template <
		typename _IteratorInA,
		typename _IteratorInB>
	pf_hint_nodiscard pf_decl_constexpr bool
	smaller_equal(
		_IteratorInA __ba,
		_IteratorInA __ea,
		_IteratorInB __bb) pf_attr_noexcept
	{
		return !greater(__ba, __ea, __bb);
	}

	/// ALGORITHM: Sort
	// TODO
}

#endif // !PULSAR_ALGORITHM_HPP