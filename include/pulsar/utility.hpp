/*! @file   utility.hpp
 *  @author Fluffy (noe.louis-quentin@hotmail.fr)
 *  @brief
 *  @date   11-09-2022
 *
 *  @copyright Copyright (c) 2022 - Pulsar Software
 *
 *  @since 0.1.2
 */

#ifndef PULSAR_UTILITY_HPP
#define PULSAR_UTILITY_HPP 1

// Include: Pulsar
#include "pulsar/pulsar.hpp"
#include "pulsar/iterator.hpp"
#include "pulsar/assert.hpp"

// Include: C++
#include <utility>
#include <concepts>
#include <compare>
#include <algorithm>
#include <type_traits>// SFINAE
#include <tuple>// std::ignore

// Pulsar
namespace pul
{
	/// UTILITY: SFINAE -> Value Type
	template <typename _Ty>
	struct value_type
	{
		using type = typename _Ty::value_t;
	};
	template <typename _Ty>
	struct value_type<_Ty *>
	{
		using type = _Ty;
	};
	template <typename _Ty>
	struct value_type<const _Ty *>
	{
		using type = const _Ty;
	};
	template <typename _Ty>
	using value_type_t = value_type<_Ty>::type;

	/// UTILITY: (STD)
	template <typename _Ty>
	using initializer_list = std::initializer_list<_Ty>;
	using nullptr_t				 = std::nullptr_t;
	pf_decl_inline pf_decl_constexpr auto ignore = std::ignore;

	/// UTILITY: Flags
	pf_decl_constexpr uint32_t NONE_BIT	 = 0x00000000u;
	pf_decl_constexpr uint32_t READ_BIT	 = 0x00000001u;
	pf_decl_constexpr uint32_t WRITE_BIT = 0x00000002u;

	/// UTILITY: Union Cast
	/*! @brief
	 *
	 *  @tparam _ToTy
	 *  @tparam _InTy
	 *  @param[in] __in
	 *  @return pf_decl_inline
	 */
	template <typename _ToTy, typename _InTy>
	pf_decl_inline pf_decl_constexpr _ToTy
	union_cast(
		_InTy __in) pf_attr_noexcept
	{
		pf_static_assert(
			(std::is_pointer_v<_ToTy>
			 && std::is_pointer_v<_InTy>)
			||
			!(std::is_pointer_v<_ToTy>
				&& std::is_pointer_v<_InTy>));
		pf_static_assert(
			(std::is_const_v<_ToTy>
			 && std::is_const_v<_InTy>)
			||
			!(std::is_const_v<_ToTy>
				&& std::is_const_v<_InTy>));
		union
		{
			_InTy in;
			_ToTy to;
		};
		in = __in;
		return to;
	}

	/// UTILITY: Swap
	template <typename _Ty>
	pf_decl_constexpr pf_decl_inline void
	swap(
		_Ty &__l,
		_Ty &__r) pf_attr_noexcept
	requires(std::is_assignable_v<_Ty>)
	{
		_Ty tmp = std::move(__l);
		__l = std::move(__r);
		__r = std::move(tmp);
	}

	/// UTILITY: Sizeof -> Sequence
	/*! @brief
	 *
	 *  @tparam _Ty
	 *  @param[in] __beg
	 *  @param[in] __end
	 *  @return pf_decl_inline
	 */
	pf_decl_inline pf_decl_constexpr size_t
	sizeof_s(
		const void *__beg,
		const void *__end) pf_attr_noexcept
	{
		const size_t b = union_cast<size_t>(__beg);
		const size_t e = union_cast<size_t>(__end);
		if(b > e)
		{
			return b - e;
		}
		return e - b;
	}

	/// UTILITY: Count
	template <typename _InIterator>
	pf_decl_constexpr pf_decl_inline size_t
	countof(
		_InIterator __beg,
		_InIterator __end) pf_attr_noexcept
	requires(
		!std::is_pointer_v<_InIterator> &&
		std::is_same_v<iterator_category_t<_InIterator>, iterator_dangling_tag_t>)
	{
		size_t c = 0;
		while(__beg != __end)
		{
			++__beg;
			++c;
		}
		return c;
	}
	template <typename _InIterator>
	pf_decl_constexpr pf_decl_inline size_t
	countof(
		_InIterator __beg,
		_InIterator __end) pf_attr_noexcept
	requires(
		!std::is_pointer_v<_InIterator> &&
		std::is_same_v<iterator_category_t<_InIterator>, iterator_sequence_tag_t>)
	{
		return sizeof_s(__beg.get(), __end.get()) / sizeof(value_type_t<_InIterator>);
	}
	template <typename _Ty>
	pf_decl_constexpr pf_decl_inline size_t
	countof(
		_Ty *__beg,
		_Ty *__end) pf_attr_noexcept
	{
		return sizeof_s(__beg, __end) / sizeof(_Ty);
	}

	/// UTILITY: Copy
	template <typename _InIterator, typename _OutIterator>
	pf_decl_constexpr pf_decl_inline size_t
	copy(
		_InIterator __beg,
		_InIterator __end,
		_OutIterator __dst)
	{
		while(__beg != __end)
		{
			*__dst = *__beg;
			++__beg;
			++__dst;
		}
		return countof(__beg, __end);
	}

	/// UTILITY: Move
	template <typename _InIterator, typename _OutIterator>
	pf_decl_constexpr pf_decl_inline size_t
	move(
		_InIterator __beg,
		_InIterator __end,
		_OutIterator __dst)
	{
		while(__beg != __end)
		{
			*__dst = std::move(*__beg);
			++__beg;
			++__dst;
		}
		return countof(__beg, __end);
	}

	/// UTILITY: Search
	template <typename _InIterator1, typename _InIterator2>
	pf_decl_constexpr pf_decl_inline _InIterator1
	search(
		_InIterator1 __ib,
		_InIterator1 __eb,
		_InIterator2 __ob,
		_InIterator2 __oe)
	{
		while(1)
		{
			_InIterator1 it = __ib;
			for(_InIterator2 k = __ob;; ++it, ++k)
			{
				if(k == __oe)
					return __ib;
				if(it == __eb)
					return __eb;
				if(!(*it == *k))
					break;
			}
			++__ib;
		}
	}
	template <typename _InIterator1, typename _InIterator2, typename _BinaryPredicate>
	pf_decl_constexpr pf_decl_inline _InIterator1
	search(
		_InIterator1 __ib,
		_InIterator1 __eb,
		_InIterator2 __ob,
		_InIterator2 __oe,
		_BinaryPredicate &&__p)
	{
		while(1)
		{
			_InIterator1 it = __ib;
			for(_InIterator2 k = __ob;; ++it, ++k)
			{
				if(k == __oe)
					return __ib;
				if(it == __eb)
					return __eb;
				if(!__p(*it, *k))
					break;
			}
			++__ib;
		}
	}

	/// UTILITY: Find
	template <typename _InIterator, typename _Predicate>
	pf_decl_constexpr pf_decl_inline _InIterator
	find(
		_InIterator __beg,
		_InIterator __end,
		_Predicate &&__p)
	{

		for(; __beg != __end; ++__beg)
		{
			if(__p(*__beg))
				return __beg;
		}
		return __end;
	}

	/// UTILITY: Accumulate
	template <typename _InIterator, typename _Ty>
	pf_decl_constexpr pf_decl_inline _Ty
	accumulate(
		_InIterator __beg,
		_InIterator __end,
		_Ty __val)
	{
		for(; __beg != __end; ++__beg)
		{
			__val = std::move(__val) + *__beg;
		}
		return __val;
	}
	template <typename _InIterator, typename _Ty, typename _BinaryOperation>
	pf_decl_constexpr pf_decl_inline _Ty
	accumulate(
		_InIterator __beg,
		_InIterator __end,
		_Ty __val,
		_BinaryOperation &&__p)
	{
		for(; __beg != __end; ++__beg)
		{
			__val = __p(std::move(__val), *__beg);
		}
		return __val;
	}

	/// UTILITY: Transform
	template <typename _InIterator, typename _OutIterator, typename _UnaryOperation>
	pf_decl_constexpr pf_decl_inline _OutIterator
	transform(
		_InIterator __ib,
		_InIterator __ie,
		_OutIterator __ob,
		_UnaryOperation &&__op)
	{
		while(__ib != __ie)
		{
			*__ob++ = __op(*__ib++);
		}
		return __ob;
	}
	template <typename _InIterator, typename _OutIterator, typename _BinaryOperation>
	pf_decl_constexpr pf_decl_inline _OutIterator
	transform(
		_InIterator __i1b, _InIterator __ie,
		_InIterator __i2b, _OutIterator __eb,
		_BinaryOperation &&__op)
	{
		while(__i1b != __ie)
		{
			*__eb++ = __op(*__i1b++, *__i2b++);
		}
		return __eb;
	}
}

#endif // !PULSAR_UTILITY_HPP