/*! @file   common.hpp
 *  @author Louis-Quentin Noé (noe.louis-quentin@hotmail.fr)
 *  @brief
 *  @date   02-01-2023
 *
 *  @copyright Copyright (c) 2023 - Pulsar Software
 *
 *  @since 0.1.3
 */

#ifndef PULSAR_COMMON_HPP
#define PULSAR_COMMON_HPP 1

// Include: Pulsar
#include "pulsar/utility.hpp"

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
	using nullptr_t				 = std::nullptr_t;
	pf_decl_constexpr auto ignore = std::ignore;

	/// UTILITY: Ratio
	template <size_t _Num, size_t _Denom = 1>
	using ratio = std::ratio<_Num, _Denom>;
	using atto	= std::atto;
	using femto = std::femto;
	using pico	= std::pico;
	using nano	= std::nano;
	using micro = std::micro;
	using milli = std::milli;
	using centi = std::centi;
	using deci	= std::deci;
	using deca	= std::deca;
	using hecto = std::hecto;
	using kilo	= std::kilo;
	using mega	= std::mega;
	using giga	= std::giga;
	using tera	= std::tera;
	using peta	= std::peta;
	using exa		= std::exa;

	/// UTILITY: Ratio -> Time

	template <typename _Rep, typename _Period>
	using duration = std::chrono::duration<_Rep, _Period>;
	using time_t	 = std::time_t;
	using tm_t		 = std::tm;

	/// UTILITY: Ratio -> Distance / Size
	// TODO: Inches, Feet, miles ratios

	/// UTILITY: Ratio -> Time Cast
	template <typename _ToDuration, typename _Rep, typename _Period>
	pf_decl_inline pf_decl_constexpr _ToDuration duration_cast(
		std::chrono::duration<_Rep, _Period> const &__dur) pf_attr_noexcept {
		return std::chrono::duration_cast<_ToDuration>(__dur);
	}

	/// UTILITY: Ratio -> Time Litterals
namespace time_litterals
{
	using namespace std::chrono_literals;
}

	/// UTILITY: Ratio -> Lightspeed ratios
	// TODO: Lightspeed Ratios

	/// UTILITY: Union Cast
	/*! @brief
	 *
	 *  @tparam _ToTy
	 *  @tparam _InTy
	 *  @param[in] __in
	 *  @return pf_decl_inline
	 */


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
		return &__arr[0] + _Num;
	}
	template <typename _Ty, size_t _Num>
	pf_hint_nodiscard pf_decl_constexpr const_iterator<_Ty>
	end(
		const _Ty (&__arr)[_Num]) pf_attr_noexcept
	{
		return &__arr[0] + _Num;
	}
	template <typename _Ty, size_t _Num>
	pf_hint_nodiscard pf_decl_constexpr const_iterator<_Ty>
	cend(
		_Ty (&__arr)[_Num]) pf_attr_noexcept
	{
		return &__arr[0] + _Num;
	}
	template <typename _Ty, size_t _Num>
	pf_hint_nodiscard pf_decl_constexpr const_iterator<_Ty>
	cend(
		const _Ty (&__arr)[_Num]) pf_attr_noexcept
	{
		return &__arr[0] + _Num;
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
		return &__arr[0] + (_Num - 1);
	}
	template <typename _Ty, size_t _Num>
	pf_hint_nodiscard pf_decl_constexpr reverse_iterator<const_iterator<_Ty>>
	rbegin(
		const _Ty (&__arr)[_Num]) pf_attr_noexcept
	{
		return &__arr[0] + (_Num - 1);
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
		return &__arr[0] + (_Num - 1);
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
		return &__arr[0] - 1;
	}
	template <typename _Ty, size_t _Num>
	pf_hint_nodiscard pf_decl_constexpr reverse_iterator<const_iterator<_Ty>>
	rend(
		const _Ty (&__arr)[_Num]) pf_attr_noexcept
	{
		return &__arr[0] - 1;
	}
	template <typename _Ty, size_t _Num>
	pf_hint_nodiscard pf_decl_constexpr reverse_iterator<const_iterator<_Ty>>
	crend(
		_Ty (&__arr)[_Num]) pf_attr_noexcept
	{
		return &__arr[0] - 1;
	}
	template <typename _Ty, size_t _Num>
	pf_hint_nodiscard pf_decl_constexpr reverse_iterator<const_iterator<_Ty>>
	crend(
		const _Ty (&__arr)[_Num]) pf_attr_noexcept
	{
		return &__arr[0] - 1;
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

	/// UTILITY: Swap
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

	/// UTILITY: Copy
	template <typename _IteratorIn, typename _IteratorOut>
	pf_decl_constexpr void copy_forward(
		_IteratorIn __beg,
		_IteratorIn __end,
		_IteratorOut __dst)
	requires(is_iterator_v<_IteratorIn>
					 && is_iterator_v<_IteratorOut>)
	{
		// TODO: Exception !__beg || !__end || !__dst
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
		_IteratorOut __dst)
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
		_IteratorOut __dst)
	requires(!is_sequence_iterator_v<_IteratorIn> ||
					 !is_sequence_iterator_v<_IteratorOut>)
	{
		copy_forward(__beg, __end, __dst);
	}

	/// UTILITY: Move
	template <typename _IteratorIn, typename _IteratorOut>
	pf_decl_constexpr void move_forward(
		_IteratorIn __beg,
		_IteratorIn __end,
		_IteratorOut __dst)
	requires(is_iterator_v<_IteratorIn> && is_iterator_v<_IteratorOut>)
	{
		// TODO: Exception !__beg || !__end || !__dst
		for (; __beg != __end; ++__beg, ++__dst) *__dst = std::move(*__beg);
	}
	template <typename _IteratorIn, typename _IteratorOut>
	pf_decl_constexpr void move_reverse(
		_IteratorIn __beg,
		_IteratorIn __end,
		_IteratorOut __dst)
	requires(is_iterator_v<_IteratorIn> && is_iterator_v<_IteratorOut>)
	{
		// TODO: Exception !__beg || !__end || !__dst
		--__beg;
		--__end;
		__dst += distof(__beg, __end);
		for (; __end != __beg; --__end, --__dst) __dst = std::move(*__beg);
	}
	template <typename _IteratorIn, typename _IteratorOut>
	pf_decl_constexpr void move(
		_IteratorIn __beg,
		_IteratorIn __end,
		_IteratorOut __dst)
	requires(is_iterator_v<_IteratorIn> && is_iterator_v<_IteratorOut>)
	{
		// TODO: Exception !__beg || !__end || !__dst
		if ((__beg < __dst) && (__dst < __end)) {
			move_reverse(__beg, __end, __dst);
		} else {
			move_forward(__beg, __end, __dst);
		}
	}

	/// MEMORY: Utility -> Fill
	template <typename _IteratorOut>
	pf_decl_constexpr void fill(
		_IteratorOut __beg,
		_IteratorOut __end,
		typename _IteratorOut::value_t const &__val)
	requires(is_iterator_v<_IteratorOut>)
	{
		// TODO: Exception !__beg || !__end || !__dst
		for (; __beg != __end; ++__beg) *__beg = __val;
	}

	/// UTILITY: Dist
	template <typename _Ty>
	pf_hint_nodiscard pf_decl_constexpr diff_t
	diffof(
		const _Ty *__beg,
		const _Ty *__end)
	{
		// TODO: Exception !__beg || !__end
		return union_cast<diff_t>(__end) - union_cast<diff_t>(__beg);
	}
	template <typename _Ty>
	pf_hint_nodiscard pf_decl_constexpr size_t
	distof(
		const _Ty *__beg,
		const _Ty *__end)
	{
		// TODO: Exception !__beg || !__end
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
		_IteratorIn __end)
	requires(is_sequence_iterator_v<_IteratorIn>)
	{
		return distof(__beg.get(), __end.get());
	}

	/// UTILITY: Count
	template <typename _IteratorIn>
	pf_hint_nodiscard pf_decl_constexpr size_t
	countof(
		_IteratorIn __beg,
		_IteratorIn __end)
	requires(!is_sequence_iterator_v<_IteratorIn>)
	{
		// TODO: Exception !__beg || !__end
		size_t c = 0;
		while (__beg != __end) {
			++__beg;
			++c;
		}
		return c;
	}
	template <typename _Ty>
	pf_hint_nodiscard pf_decl_constexpr size_t
	countof(
		const _Ty *__beg,
		const _Ty *__end)
	{
		// TODO: Exception !__beg || !__end
		if (pf_likely(__end >= __beg))
		{
			return (union_cast<size_t>(__end) - union_cast<size_t>(__beg)) / sizeof(_Ty);
		}
		return (union_cast<size_t>(__beg) - union_cast<size_t>(__end)) / sizeof(_Ty);
	}
	template <typename _IteratorIn>
	pf_hint_nodiscard pf_decl_constexpr size_t
	countof(
		_IteratorIn __beg,
		_IteratorIn __end)
	requires(is_sequence_iterator_v<_IteratorIn>)
	{
		return countof(__beg.get(), __end.get());
	}

	/// UTILITY: Search
	template <typename _IteratorIn1, typename _IteratorIn2>
	pf_hint_nodiscard pf_decl_constexpr _IteratorIn1 search(
		_IteratorIn1 __ib,
		_IteratorIn1 __eb,
		_IteratorIn2 __ob,
		_IteratorIn2 __oe) {
		// TODO: Exception !__beg || !__end
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
	search(_IteratorIn1 __ib, _IteratorIn1 __eb, _IteratorIn2 __ob,
				 _IteratorIn2 __oe, _BinaryPredicate &&__p) {
		// TODO: Exception !__beg || !__end
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

	/// UTILITY: Find
	template <
		typename _IteratorIn,
		typename _Predicate>
	pf_hint_nodiscard pf_decl_constexpr _IteratorIn find(
		_IteratorIn __beg,
		_IteratorIn __end,
		_Predicate  &&__p) {

		// TODO: Exception !__beg || !__end
		for (; __beg != __end; ++__beg) {
			if (__p(*__beg)) {
				return __beg;
			}
		}
		return __end;
	}

	/// UTILITY: Accumulate
	template <
		typename _IteratorIn,
		typename _Ty>
	pf_hint_nodiscard pf_decl_constexpr _Ty accumulate(
		_IteratorIn __beg,
		_IteratorIn __end,
		_Ty __val) {
		// TODO: Exception !__beg || !__end
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
		_BinaryOperation &&__p) {
		// TODO: Exception !__beg || !__end
		for (; __beg != __end; ++__beg) {
			__val = __p(std::move(__val), *__beg);
		}
		return __val;
	}

	/// UTILITY: Transform
	template <
		typename _IteratorIn,
		typename _OutIterator,
		typename _UnaryOperation>
	pf_hint_nodiscard pf_decl_constexpr _OutIterator
	transform(
		_IteratorIn __ib,
		_IteratorIn __ie,
		_OutIterator __ob,
		_UnaryOperation &&__op) {
		// TODO: Exception !__beg || !__end
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
		_BinaryOperation &&__op) {
		// TODO: Exception !__beg || !__end
		while (__i1b != __ie) {
			*__eb++ = __op(*__i1b++, *__i2b++);
		}
		return __eb;
	}

	/// UTILITY: Compare
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

	/*
	 * !           !
	 *   Magnifier
	 * !           !
	 */
	/// UTILITY: Magnifier -> Concept
	template <typename _Magnifier>
	concept __magnifier_c = requires(_Magnifier const & __m) {
		{ __m(std::declval<size_t>()) }->std::convertible_to<size_t>;
	};

	/// UTILITY: Magnifier -> SFINAE
	template <typename _Magnifier> struct is_magnifier : std::false_type {};
	template <__magnifier_c _Magnifier>
	struct is_magnifier<_Magnifier> : std::true_type {};
	template <typename _Magnifier>
	pf_decl_constexpr pf_decl_inline bool is_magnifier_v = is_magnifier<_Magnifier>::value;

	/// UTILITY: Magnifier -> Default
	class magnifier_default pf_attr_final {
	public:
		/// Operator()
		pf_hint_nodiscard pf_decl_constexpr size_t
		operator()(size_t __rs) const pf_attr_noexcept {
			return __rs;
		}
	};

	/// UTILITY: Magnifier -> Linear
	class magnifier_linear pf_attr_final
	{
	public:
		/// Constructors
		pf_decl_constexpr magnifier_linear(
			size_t __count = 2) : count_(__count)
		{
			// if(pf_hint_unlikely(__count == 0))
			// throw(exception(exception_std(),
			// 								err_std::invalid_argument,
			// 								"__count is null!"));
			// TODO: pf_throw
		}
		pf_decl_constexpr
		magnifier_linear(magnifier_linear const & __r) pf_attr_noexcept
		: count_(__r.count_) {}
		pf_decl_constexpr
		magnifier_linear(magnifier_linear &&) pf_attr_noexcept = default;

		/// Destructor
		pf_decl_constexpr ~magnifier_linear() pf_attr_noexcept = default;

		/// Operator=
		pf_decl_constexpr magnifier_linear &
		operator=(magnifier_linear const & __r) pf_attr_noexcept
		{
			if (pf_likely(this != &__r)) {
				this->count_ = __r.count_;
			}
			return *this;
		}
		pf_decl_constexpr magnifier_linear &
		operator=(magnifier_linear && __r) pf_attr_noexcept = default;

		/// Operator()
		pf_hint_nodiscard pf_decl_constexpr size_t
		operator()(size_t __rs) const pf_attr_noexcept
		{
			if (pf_unlikely(__rs == 0))
				return 0;
			return (__rs / this->count_ + 1) * this->count_;
		}

	private:
		size_t count_;
	};

	/// UTILITY: Magnifier -> Quadratic
	class magnifier_quadratic pf_attr_final
	{
	public:
		/// Operator()
		pf_hint_nodiscard pf_decl_constexpr size_t
		operator()(
			size_t __rs) const pf_attr_noexcept
		{
			--__rs;
			__rs |= __rs >> 1;
			__rs |= __rs >> 2;
			__rs |= __rs >> 4;
			__rs |= __rs >> 8;
			__rs |= __rs >> 16;
			#ifdef PF_64BIT
			__rs |= __rs >> 32;
			#endif // !PF_64BIT
			++__rs;
			return __rs;
		}
	};

	/// MEMORY: Utility
	pf_hint_nodiscard pf_decl_inline pf_decl_constexpr bool
	is_power_of_two(
		size_t __val) pf_attr_noexcept
	{
		return (__val & (__val - 1)) == 0;
	}
	pf_hint_nodiscard pf_decl_inline pf_decl_constexpr size_t
	paddingof(
		size_t __addr,
		align_val_t __align) pf_attr_noexcept
	{
		const size_t a = union_cast<size_t>(__align);
		return (a - (__addr & (a - 1))) & (a - 1);
	}
	pf_hint_nodiscard pf_decl_inline pf_decl_constexpr void*
	align_top(
		void *__ptr,
		align_val_t __align = ALIGN_MAX,
		size_t __offset			= 0) pf_attr_noexcept
	{
		byte_t *p = union_cast<byte_t*>(__ptr);
		p += paddingof(addressof(p + __offset), __align);
		return p;
	}
	pf_hint_nodiscard pf_decl_inline pf_decl_constexpr size_t
	__base2_mod(
		size_t __x,
		size_t __y) pf_attr_noexcept
	{
		return __y & (__y - 1) ? __x % __y : __x & (__y - 1);
	}
	pf_hint_nodiscard pf_decl_inline pf_decl_constexpr bool
	is_aligned(
		const void *__ptr,
		align_val_t __align,
		size_t __offset = 0)
	{
		return __base2_mod(addressof(__ptr) + __offset, union_cast<size_t>(__align)) == 0;
	}

	/// MEMORY: Utility -> Array Type
	template <typename _Ty>
	struct memory_array
	{
		size_t count;
		_Ty data[];	// TODO -WPedentic to ignore
	};
	using memory_array_t = memory_array<byte_t>;

	/// MEMORY: Utility -> Construct
	template<typename _Ty, typename ... _Args>
	pf_decl_inline pf_decl_constexpr void
	construct(
		_Ty *__ptr,
		_Args &&... __args)
	requires(std::is_constructible_v<_Ty, _Args...>)
	{
		// pf_assert(__ptr, "__ptr is nullptr!"); // TODO: Exception
		new(__ptr) _Ty(std::forward<_Args>(__args)...);
	}
	template <typename _IteratorIn>
	pf_decl_inline pf_decl_constexpr void
	construct(
		_IteratorIn __beg,
		_IteratorIn __end)
	requires(is_iterator_v<_IteratorIn>
					 && std::is_default_constructible_v<typename _IteratorIn::value_t>)
	{
		for (; __beg != __end; ++__beg) construct(__beg.get());
	}
	template <typename _IteratorIn>
	pf_decl_inline pf_decl_constexpr void
	construct(
		_IteratorIn __beg,
		_IteratorIn __end,
		typename _IteratorIn::value_t const &__val)
	requires(is_iterator_v<_IteratorIn>
					 && std::is_copy_constructible_v<typename _IteratorIn::value_t>)
	{
		for (; __beg != __end; ++__beg) construct(__beg.get(), __val);
	}
	template <typename _Iterator, typename _IteratorIn>
	pf_decl_inline pf_decl_constexpr void
	construct(
		_Iterator __ab,
		_Iterator __ae,
		_IteratorIn __bb)
	requires(is_iterator_v<_Iterator>
					 && is_iterator_v<_IteratorIn>
					 && std::is_convertible_v<typename _IteratorIn::value_t, typename _Iterator::value_t>)
	{
		for (; __ab != __ae; ++__bb) construct(__ab.get(), *__bb);
	}


	/// MEMORY: Utility -> Assign
	template <typename _Ty, typename ... _Args>
	pf_decl_inline pf_decl_constexpr void
	assign(
		_Ty *__ptr,
		_Args&&... __args) pf_attr_noexcept
	requires(std::is_assignable_v<_Ty, _Args...>)
	{
		*__ptr = { std::forward<_Args>(__args) ... };
	}
	template <typename _Ty, typename ... _Args>
	pf_decl_inline pf_decl_constexpr void
	assign(
		_Ty *__ptr,
		_Args&& ... __args) pf_attr_noexcept
	requires(!std::is_assignable_v<_Ty, _Args...>
					 && std::is_constructible_v<_Ty, _Args...>)
	{
		destroy(__ptr);
		*__ptr = { std::forward<_Args>(__args) ... };
	}


	/// MEMORY: Utility -> Destroy
	template<typename _Ty>
	pf_decl_inline pf_decl_constexpr void
	destroy(
		_Ty *__ptr) pf_attr_noexcept
	{
		__ptr->~_Ty();
	}
	template <typename _IteratorIn>
	pf_decl_inline pf_decl_constexpr void
	destroy(
		_IteratorIn __beg,
		_IteratorIn __end) pf_attr_noexcept
	requires(is_iterator_v<_IteratorIn>)
	{
		for (; __beg != __end; ++__beg) destroy(__beg.get());
	}
	template <typename _Ty>
	pf_decl_inline pf_decl_constexpr void
	destroy(
		memory_array<_Ty> *__ptr) pf_attr_noexcept
	{
		destroy(iterator(&__ptr->data[0]), iterator(&__ptr->data[0] + __ptr->count));
	}
	template <typename _Ty>
	pf_decl_inline pf_decl_constexpr void
	destroy_array(
		_Ty *__ptr) pf_attr_noexcept
	{
		union
		{
			memory_array_t *as_array;
			_Ty *as_type;
			size_t *as_size;
		};
		as_type = __ptr;
		--as_size;
		destroy(as_array);
	}


	/// MEMORY: New Construct
	template <typename _Ty, typename ... _Args>
	pf_decl_inline pf_decl_constexpr _Ty*
	new_construct_aligned_at_ex(
		size_t __exBytes,
		align_val_t __align,
		size_t __offset,
		_Args&& ... __args)
	requires(std::is_constructible_v<_Ty, _Args...>)
	{
		_Ty *p = union_cast<_Ty*>(
			allocate(sizeof(_Ty) + __exBytes, __align, __offset));
		construct(p, std::forward<_Args>(__args)...);
		return p;
	}
	template <typename _Ty, typename ... _Args>
	pf_decl_inline pf_decl_constexpr _Ty*
	new_construct_aligned_ex(
		size_t __exBytes,
		align_val_t __align,
		_Args&& ... __args)
	requires(std::is_constructible_v<_Ty, _Args...>)
	{
		return new_construct_aligned_at_ex<_Ty>(
			__exBytes,
			__align,
			0,
			std::forward<_Args>(__args)...);
	}
	template <typename _Ty, typename ... _Args>
	pf_decl_inline pf_decl_constexpr _Ty*
	new_construct_ex(
		size_t __exBytes,
		_Args&& ... __args)
	requires(std::is_constructible_v<_Ty, _Args...>)
	{
		return new_construct_aligned_at_ex<_Ty>(
			__exBytes,
			align_val_t(alignof(_Ty)),
			0,
			std::forward<_Args>(__args)...);
	}
	template <typename _Ty, typename _Allocator, typename ... _Args>
	pf_decl_inline pf_decl_constexpr _Ty*
	new_construct_aligned_at_ex(
		_Allocator  &__all,
		align_val_t __align,
		size_t __offset,
		size_t __exBytes,
		_Args&& ... __args)
	requires(is_allocator_v<_Allocator>
					 && std::is_constructible_v<_Ty, _Args...>)
	{
		_Ty *p = union_cast<_Ty*>(
			allocate<_Ty>(__all, sizeof(_Ty) + __exBytes, __align, __offset));
		construct(p, std::forward<_Args>(__args)...);
		return p;
	}
	template <typename _Ty, typename _Allocator, typename ... _Args>
	pf_decl_inline pf_decl_constexpr _Ty*
	new_construct_aligned_ex(
		_Allocator  &__all,
		align_val_t __align,
		size_t __exBytes,
		_Args&& ... __args)
	requires(is_allocator_v<_Allocator>
					 && std::is_constructible_v<_Ty, _Args...>)
	{
		return new_construct_aligned_at_ex<_Ty>(
			__all,
			0,
			__align,
			0,
			std::forward<_Args>(__args)...);
	}
	template <typename _Ty, typename _Allocator, typename ... _Args>
	pf_decl_inline pf_decl_constexpr _Ty*
	new_construct_ex(
		_Allocator  &__all,
		size_t __exBytes,
		_Args&& ... __args)
	requires(is_allocator_v<_Allocator>
					 && std::is_constructible_v<_Ty, _Args...>)
	{
		return new_construct_aligned_at_ex<_Ty>(
			__all,
			0,
			align_val_t(alignof(_Ty)),
			0,
			std::forward<_Args>(__args)...);
	}
	template <typename _Ty, typename ... _Args>
	pf_decl_inline pf_decl_constexpr _Ty*
	new_construct_aligned_at(
		align_val_t __align,
		size_t __offset,
		_Args&& ... __args)
	requires(std::is_constructible_v<_Ty, _Args...>)
	{
		return new_construct_aligned_at_ex<_Ty>(0, __align, __offset, std::forward<_Args>(__args)...);
	}
	template <typename _Ty, typename ... _Args>
	pf_decl_inline pf_decl_constexpr _Ty*
	new_construct_aligned(
		align_val_t __align,
		_Args&& ... __args)
	requires(std::is_constructible_v<_Ty, _Args...>)
	{
		return new_construct_aligned_ex<_Ty>(0, __align, std::forward<_Args>(__args)...);
	}
	template <typename _Ty, typename ... _Args>
	pf_decl_inline pf_decl_constexpr _Ty*
	new_construct(
		_Args&& ... __args)
	requires(std::is_constructible_v<_Ty, _Args...>)
	{
		return new_construct_ex<_Ty>(0, std::forward<_Args>(__args)...);
	}

	template <typename _Ty, typename _Allocator, typename ... _Args>
	pf_decl_inline pf_decl_constexpr _Ty*
	new_construct_aligned_at(
		_Allocator  &__all,
		align_val_t __align,
		size_t __offset,
		_Args&& ... __args)
	requires(is_allocator_v<_Allocator>
					 && std::is_constructible_v<_Ty, _Args...>)
	{
		return new_construct_aligned_at_ex<_Ty>(__all, 0, __align, __offset, std::forward<_Args>(__args)...);
	}
	template <typename _Ty, typename _Allocator, typename ... _Args>
	pf_decl_inline pf_decl_constexpr _Ty*
	new_construct_aligned(
		_Allocator  &__all,
		align_val_t __align,
		_Args&& ... __args)
	requires(is_allocator_v<_Allocator>
					 && std::is_constructible_v<_Ty, _Args...>)
	{
		return new_construct_aligned_ex<_Ty>(__all, 0, __align, std::forward<_Args>(__args)...);
	}
	template <typename _Ty, typename _Allocator, typename ... _Args>
	pf_decl_inline pf_decl_constexpr _Ty*
	new_construct(
		_Allocator  &__all,
		_Args&& ... __args)
	requires(is_allocator_v<_Allocator>
					 && std::is_constructible_v<_Ty, _Args...>)
	{
		return new_construct_ex<_Ty>(__all, 0, std::forward<_Args>(__args)...);
	}

	/// MEMORY: Utility -> Destroy Delete
	template<typename _Ty>
	pf_decl_inline pf_decl_constexpr void
	destroy_delete(
		_Ty *__ptr) pf_attr_noexcept
	{
		destroy(__ptr);
		deallocate(__ptr);
	}
	template<typename _Ty, typename _Allocator>
	pf_decl_inline pf_decl_constexpr void
	destroy_delete(
		_Allocator &__all,
		_Ty *__ptr) pf_attr_noexcept
	requires(is_allocator_v<_Allocator>)
	{
		destroy(__ptr);
		deallocate(__all, __ptr);
	}


	/// MEMORY: Utility -> New Construct Array
	template <typename _Ty>
	pf_decl_inline pf_decl_constexpr _Ty*
	new_construct_array_aligned_at(
		size_t __count,
		align_val_t __align,
		size_t __offset)
	requires(std::is_default_constructible_v<_Ty>)
	{
		union
		{
			memory_array_t *as_array;
			size_t *as_size;
		};
		as_array = union_cast<memory_array_t*>(
			allocate(sizeof(size_t) + sizeof(_Ty) * __count, __align, sizeof(size_t) + __offset));
		as_array->count = __count;
		construct(iterator(&as_array->data[0]), iterator(&as_array->data[0] + __count));
		return union_cast<_Ty*>(&as_array->data[0]);
	}
	template <typename _Ty>
	pf_decl_inline pf_decl_constexpr _Ty*
	new_construct_array_aligned(
		size_t __count,
		align_val_t __align)
	requires(std::is_default_constructible_v<_Ty>)
	{
		return new_construct_array_aligned_at<_Ty>(__count, __align, 0);
	}
	template <typename _Ty>
	pf_decl_inline pf_decl_constexpr _Ty*
	new_construct_array(
		size_t __count)
	requires(std::is_default_constructible_v<_Ty>)
	{
		return new_construct_array_aligned_at<_Ty>(__count, align_val_t(alignof(_Ty)), 0);
	}
	template <typename _Ty, typename _Allocator>
	pf_decl_inline pf_decl_constexpr _Ty*
	new_construct_array_aligned_at(
		_Allocator  &__all,
		size_t __count,
		align_val_t __align,
		size_t __offset)
	requires(is_allocator_v<_Allocator>
					 && std::is_default_constructible_v<_Ty>)
	{
		union
		{
			memory_array_t *as_array;
			size_t *as_size;
		};
		as_array = union_cast<memory_array_t*>(
			allocate(__all, sizeof(size_t) + sizeof(_Ty) * __count, __align, sizeof(size_t) + __offset));
		as_array->count = __count;
		construct(iterator(&as_array->data[0]), iterator(&as_array->data[0] + __count));
		return union_cast<_Ty*>(&as_array->data[0]);
	}
	template <typename _Ty, typename _Allocator>
	pf_decl_inline pf_decl_constexpr _Ty*
	new_construct_array_aligned(
		_Allocator  &__all,
		size_t __count,
		align_val_t __align)
	requires(is_allocator_v<_Allocator>
					 && std::is_default_constructible_v<_Ty>)
	{
		return new_construct_array_aligned_at<_Ty>(__all, __count, __align, 0);
	}
	template <typename _Ty, typename _Allocator>
	pf_decl_inline pf_decl_constexpr _Ty*
	new_construct_array(
		_Allocator &__all,
		size_t __count)
	requires(is_allocator_v<_Allocator>
					 && std::is_default_constructible_v<_Ty>)
	{
		return new_construct_array_aligned_at<_Ty>(__all, __count, align_val_t(alignof(_Ty)), 0);
	}

	template <typename _Ty>
	pf_decl_inline pf_decl_constexpr _Ty*
	new_construct_array_aligned_at(
		size_t __count,
		align_val_t __align,
		size_t __offset,
		_Ty const   &__val)
	requires(std::is_copy_constructible_v<_Ty>)
	{
		union
		{
			memory_array_t *as_array;
			size_t *as_size;
		};
		as_array = union_cast<memory_array_t*>(
			allocate(sizeof(size_t) + sizeof(_Ty) * __count, __align, sizeof(size_t) + __offset));
		as_array->count = __count;
		construct(iterator(&as_array->data[0]), iterator(&as_array->data[0] + __count), __val);
		return union_cast<_Ty*>(&as_array->data[0]);
	}
	template <typename _Ty>
	pf_decl_inline pf_decl_constexpr _Ty*
	new_construct_array_aligned(
		size_t __count,
		align_val_t __align,
		_Ty const   &__val)
	requires(std::is_copy_constructible_v<_Ty>)
	{
		return new_construct_array_aligned_at<_Ty>(__count, __align, 0, __val);
	}
	template <typename _Ty>
	pf_decl_inline pf_decl_constexpr _Ty*
	new_construct_array(
		size_t __count,
		_Ty const &__val)
	requires(std::is_copy_constructible_v<_Ty>)
	{
		return new_construct_array_aligned_at<_Ty>(__count, align_val_t(alignof(_Ty)), 0, __val);
	}

	template <typename _Ty, typename _Allocator>
	pf_decl_inline pf_decl_constexpr _Ty*
	new_construct_array_aligned_at(
		_Allocator  &__all,
		size_t __count,
		align_val_t __align,
		size_t __offset,
		_Ty const   &__val)
	requires(is_allocator_v<_Allocator>
					 && std::is_copy_constructible_v<_Ty>)
	{
		union
		{
			memory_array_t *as_array;
			size_t *as_size;
		};
		as_array = union_cast<memory_array_t*>(
			allocate(__all, sizeof(_Ty) * __count, __align, sizeof(size_t) + __offset));
		as_array->count = __count;
		construct(iterator(&as_array->data[0]), iterator(&as_array->data[0] + __count), __val);
		return union_cast<_Ty*>(&as_array->data[0]);
	}
	template <typename _Ty, typename _Allocator>
	pf_decl_inline pf_decl_constexpr _Ty*
	new_construct_array_aligned(
		_Allocator  &__all,
		size_t __count,
		align_val_t __align,
		_Ty const   &__val)
	requires(is_allocator_v<_Allocator>
					 && std::is_copy_constructible_v<_Ty>)
	{
		return new_construct_array_aligned_at<_Ty>(__all, __count, __align, 0, __val);
	}
	template <typename _Ty, typename _Allocator>
	pf_decl_inline pf_decl_constexpr _Ty*
	new_construct_array(
		_Allocator &__all,
		size_t __count,
		_Ty const  &__val)
	requires(is_allocator_v<_Allocator>
					 && std::is_copy_constructible_v<_Ty>)
	{
		return new_construct_array_aligned_at<_Ty>(__all, __count, align_val_t(alignof(_Ty)), 0, __val);
	}

	template <typename _IteratorIn>
	pf_decl_inline pf_decl_constexpr typename _IteratorIn::value_t*
	new_construct_array_aligned_at(
		_IteratorIn __beg,
		_IteratorIn __end,
		align_val_t __align,
		size_t __offset)
	requires(std::is_copy_constructible_v<typename _IteratorIn::value_t>)
	{
		const size_t c = countof(__beg, __end);
		union
		{
			memory_array_t *as_array;
			size_t *as_size;
		};
		as_array = union_cast<memory_array_t*>(
			allocate(sizeof(typename _IteratorIn::value_t) * c, __align, sizeof(size_t) + __offset));
		as_array->count = c;
		construct(iterator(&as_array->data[0]), iterator(&as_array->data[0] + c), __beg);
		return union_cast<typename _IteratorIn::value_t*>(&as_array->data[0]);
	}
	template <typename _IteratorIn>
	pf_decl_inline pf_decl_constexpr typename _IteratorIn::value_t*
	new_construct_array_aligned(
		_IteratorIn __beg,
		_IteratorIn __end,
		align_val_t __align)
	requires(std::is_copy_constructible_v<typename _IteratorIn::value_t>)
	{
		return new_construct_array_aligned_at(
			__beg,
			__end,
			__align,
			0);
	}
	template <typename _IteratorIn>
	pf_decl_inline pf_decl_constexpr typename _IteratorIn::value_t*
	new_construct_array(
		_IteratorIn __beg,
		_IteratorIn __end)
	requires(std::is_copy_constructible_v<typename _IteratorIn::value_t>)
	{
		return new_construct_array_aligned_at(
			__beg,
			__end,
			align_val_t(alignof(typename _IteratorIn::value_t)),
			0);
	}
	template <typename _IteratorIn, typename _Allocator>
	pf_decl_inline pf_decl_constexpr typename _IteratorIn::value_t*
	new_construct_array_aligned_at(
		_Allocator  &__all,
		_IteratorIn __beg,
		_IteratorIn __end,
		align_val_t __align,
		size_t __offset)
	requires(is_allocator_v<_Allocator>
					 && std::is_copy_constructible_v<typename _IteratorIn::value_t>)
	{
		const size_t c = countof(__beg, __end);
		union
		{
			memory_array_t *as_array;
			size_t *as_size;
		};
		as_array = union_cast<memory_array_t*>(
			allocate(__all, sizeof(typename _IteratorIn::value_t) * c, __align, sizeof(size_t) + __offset));
		as_array->count = c;
		construct(iterator(&as_array->data[0]), iterator(&as_array->data[0] + c), __beg);
		return union_cast<typename _IteratorIn::value_t*>(&as_array->data[0]);
	}
	template <typename _IteratorIn, typename _Allocator>
	pf_decl_inline pf_decl_constexpr typename _IteratorIn::value_t*
	new_construct_array_aligned(
		_Allocator  &__all,
		_IteratorIn __beg,
		_IteratorIn __end,
		align_val_t __align)
	requires(is_allocator_v<_Allocator>
					 && std::is_copy_constructible_v<typename _IteratorIn::value_t>)
	{
		return new_construct_array_aligned_at(
			__all,
			__beg,
			__end,
			__align,
			0);
	}
	template <typename _IteratorIn, typename _Allocator>
	pf_decl_inline pf_decl_constexpr typename _IteratorIn::value_t*
	new_construct_array(
		_Allocator  &__all,
		_IteratorIn __beg,
		_IteratorIn __end)
	requires(is_allocator_v<_Allocator>
					 && std::is_copy_constructible_v<typename _IteratorIn::value_t>)
	{
		return new_construct_array_aligned_at(
			__all,
			__beg,
			__end,
			std::align_val_t(alignof(typename _IteratorIn::value_t)),
			0);
	}


	/// MEMORY: Utility -> Destroy Delete
	template<typename _Ty>
	pf_decl_inline pf_decl_constexpr void
	destroy_delete_array(
		_Ty *__ptr) pf_attr_noexcept
	{
		destroy_array(__ptr--);
		deallocate(__ptr);
	}
	template<typename _Ty, typename _Allocator>
	pf_decl_inline pf_decl_constexpr void
	destroy_delete_array(
		_Allocator &__all,
		_Ty *__ptr) pf_attr_noexcept
	requires(is_allocator_v<_Allocator>)
	{
		destroy_array(__ptr--);
		deallocate(__all, __ptr);
	}


	/// MEMORY: Deleter -> Default
	template<typename _Ty>
	class deleter_default
	{
	public:
		/// Operator()
		pf_decl_inline pf_decl_constexpr void
		operator()(
			_Ty *__ptr) pf_attr_noexcept
		{
			destroy_delete(__ptr);
		}
	};
	template<typename _Ty>
	class deleter_default<_Ty[]>
	{
	public:
		/// Operator()
		pf_decl_inline pf_decl_constexpr void
		operator()(
			_Ty *__ptr) pf_attr_noexcept
		{
			destroy_delete_array(__ptr);
		}
	};

	/// MEMORY: Deleter -> Allocator
	template<typename _Ty, typename _Allocator>
	class deleter_allocator
	{
	public:
		/// Constructor
		pf_decl_inline pf_decl_constexpr
		deleter_allocator(
			_Allocator * __all) pf_attr_noexcept
		: allocator_(__all)
		{}

		/// Operator()
		pf_decl_inline pf_decl_constexpr void
		operator()(
			_Ty *__ptr) const pf_attr_noexcept
		{
			destroy(__ptr);
			this->allocator_->deallocate(__ptr);
		}

	private:
		_Allocator *allocator_;
	};
	template<typename _Ty, typename _Allocator>
	class deleter_allocator<_Ty[], _Allocator>
	{
	public:
		/// Constructor
		pf_decl_inline pf_decl_constexpr
		deleter_allocator(
			_Allocator * __all) pf_attr_noexcept
		: allocator_(__all)
		{}

		/// Operator()
		pf_decl_inline pf_decl_constexpr void
		operator()(
			_Ty *__ptr) const pf_attr_noexcept
		{
			(__ptr);
			this->allocator_->deallocate(__ptr, 1);
		}

	private:
		_Allocator *allocator_;
	};
}

#endif // !PULSAR_COMMON_HPP