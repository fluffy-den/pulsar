/*! @file   list.hpp
 *  @author Louis-Quentin Noé (noe.louis-quentin@hotmail.fr)
 *  @brief
 *  @date   02-01-2023
 *
 *  @copyright Copyright (c) 2023 - Pulsar Software
 *
 *  @since 0.1.3
 */

#ifndef PULSAR_LIST_HPP
#define PULSAR_LIST_HPP 1

// Include: Pulsar
#include "pulsar/container.hpp"

// Pulsar
namespace pul
{
	/// LIST: Forward -> Node
	template <typename _Ty>
	struct __forward_node
	{
		__forward_node<_Ty> *next;
		_Ty store;
	};

	/// LIST: Forward -> Block
	template <typename _Ty>
	struct __forward_block// Act as ordered pool allocator
	{
		__forward_node<_Ty> *head;
		__forward_node<_Ty> *tail;
	};

	/// LIST: Forward -> Iterator
	template <typename _Ty>
	class __forward_iterator
	{
	public:
		using value_t	 = _Ty;
		using category = iterator_forward_iterator_tag_t;

		/// Constructors
		pf_decl_constexpr forward_iterator();

	// TODO: forward iterator

	private:
		__forward_node<_Ty> *ptr_;
	};
	template <typename _Ty>
	class __forward_iterator<const _Ty>
	{
	public:
		using value_t	 = _Ty;
		using category = iterator_forward_iterator_tag_t;

	// TODO: forward const iterator

	private:
		__forward_node<_Ty> *ptr_;
	};

	/// SPAN: Forward
	template <typename _Ty>
	class forward_view
	{
	pf_assert_static(!std::is_const_v<_Ty>, "_Ty mustn't be a const type!");

	public:
		using value_t					 = std::remove_const_t<_Ty>;
		using node_t					 = __forward_node<_Ty>;
		using iterator_t			 = __forward_iterator<_Ty>;
		using const_iterator_t = __forward_iterator<const _Ty>;

		/// Constructors
		pf_decl_constexpr forward_view(
			__forward_node<_Ty> *__head,
			__forward_node<_Ty> *__tail) pf_attr_noexcept
			: head_(__head)
			, tail_(__tail)
		{}

	private:
		__forward_node<_Ty> *head_;
		__forward_node<_Ty> *tail_;
	};

	/// LIST: Forward
	template<typename _Ty,
					 typename _Magnifier = magnifier_linear,
					 typename _Allocator = allocator_default>
	class forward_list
	{
	pf_assert_static(!std::is_const_v<_Ty>, "_Ty mustn't be a const type!");

	public:
		using value_t					 = _Ty;
		using node_t					 = __forward_node<_Ty>;
		using block_t					 = __forward_block<_Ty>;
		using view_t					 = forward_view<_Ty>;
		using iterator_t			 = __forward_iterator<_Ty>;
		using const_iterator_t = __forward_iterator<const _Ty>;
		using magnifier_t			 = _Magnifier;
		using allocator_t			 = _Allocator;

		/// Constructors
		pf_decl_constexpr forward_list(
			align_val_t __ema = align_val_t(alignof(_Ty)),
			_Magnifier &&__m	= magnifier_linear(32),
			_Allocator &&__a	= allocator_default()) pf_attr_noexcept
			: head_(nullptr)
			, tail_(nullptr)
			, next_(nullptr)
			, count_(0)
			, elemMaxAlign_(__ema)
			, magnifier_(std::move(__m))
			, allocator_(std::move(__a))
		{}
		pf_decl_constexpr forward_list(
			size_t __c,
			_Ty const &__val,
			align_val_t __ema = align_val_t(alignof(_Ty)),
			_Magnifier &&__m	= magnifier_linear(32),
			_Allocator &&__a	= allocator_default()) pf_attr_noexcept
			: forward_list(__ema, std::move(__m), std::move(__a))
		{
			// TODO: this->insert_back(__c, __val)
		}
		template <typename _IteratorIn>
		pf_decl_constexpr forward_list(
			_IteratorIn __beg,
			_IteratorIn __end,
			align_val_t __ema = align_val_t(alignof(_Ty)),
			_Magnifier &&__m	= magnifier_linear(32),
			_Allocator &&__a	= allocator_default()) pf_attr_noexcept
			: forward_list(__ema, std::move(__m), std::move(__a))
		{
			// TODO: this->insert_back(__beg, __end)
		}
		template <size_t _Num>
		pf_decl_constexpr forward_list(
			_Ty const (&__arr)[_Num],
			align_val_t __ema = align_val_t(alignof(_Ty)),
			_Magnifier &&__m	= magnifier_linear(32),
			_Allocator &&__a	= allocator_default()) pf_attr_noexcept
			: forward_list(pul::begin(__arr), pul::end(__arr), __ema, std::move(__m), std::move(__a))
		{}
		template <typename _Iterable>
		pf_decl_constexpr forward_list(
			_Iterable __ia,
			align_val_t __ema = align_val_t(alignof(_Ty)),
			_Magnifier &&__m	= magnifier_linear(),
			_Allocator &&__a	= allocator_default())
		requires(is_iterable_v<_Iterable> && std::is_same_v<value_t, value_type_t<_Iterable>>)
			: forward_list(__ia.begin(), __ia.end(), __ema, std::move(__m), std::move(__a))
		{}
		template <typename _MagnifierR, typename _AllocatorR>
		pf_decl_constexpr forward_list(
			forward_list<_Ty, _MagnifierR, _AllocatorR> const &__r,
			align_val_t __ema = align_val_t(alignof(_Ty)),
			_Magnifier &&__m	= magnifier_linear(32),
			_Allocator &&__a	= allocator_default())
			: forward_list(__r.begin(), __r.end(), __ema, std::move(__m), std::move(__a))
		{}
		template <typename _MagnifierR>
		pf_decl_constexpr forward_list(
			forward_list<_Ty, _MagnifierR, _Allocator> &&__r,
			_Magnifier &&__m = magnifier_linear(32)) pf_attr_noexcept
			: head_(__r.head_)
			, tail_(__r.tail_)
			, next_(__r.next_)
			, count_(__r.count_)
			, elemMaxAlign_(__r.elemMaxAlign_)
			, magnifier_(std::move(__m))
			, allocator_(std::move(__r.allocator_))
		{
			__r.head_	 = nullptr;
			__r.tail_	 = nullptr;
			__r.next_	 = nullptr;
			__r.count_ = 0;
		}
		pf_decl_constexpr forward_list(
			forward_list<_Ty, _Magnifier, _Allocator> &&__r) pf_attr_noexcept
			: head_(__r.head_)
			, tail_(__r.tail_)
			, next_(__r.next_)
			, count_(__r.count_)
			, elemMaxAlign_(__r.elemMaxAlign_)
			, magnifier_(std::move(__r.magnifier_))
			, allocator_(std::move(__r.allocator_))
		{
			__r.head_	 = nullptr;
			__r.tail_	 = nullptr;
			__r.next_	 = nullptr;
			__r.count_ = 0;
		}

		/// Destructor
		pf_decl_constexpr ~forward_list() pf_attr_noexcept
		{
			this->clear();
		}

		/// Operator =
		template <typename _MagnifierR, typename _AllocatorR>
		pf_decl_constexpr forward_list<_Ty, _Magnifier, _Allocator> &
		operator=(
			forward_list<_Ty, _MagnifierR, _AllocatorR> const &__r)
		{
			if (pf_likely(this != &__r))
			{
				// TODO: this->__assign
			}
			return *this;
		}
		pf_decl_constexpr forward_list<_Ty, _Magnifier, _Allocator> &
		operator=(
			forward_list<_Ty, _Magnifier, _Allocator> &&__r) pf_attr_noexcept
		{
			if (pf_likely(this != &__r))
			{
				this->head_					= __r.head_; __r.head_ = nullptr;
				this->tail_					= __r.tail_; __r.tail_ = nullptr;
				this->count_				= __r.count_; __r.count_ = 0;
				this->elemMaxAlign_ = __r.elemMaxAlign_;
				this->magnifier_		= std::move(__r.magnifier_);
				this->allocator_		= std::move(__r.allocator_);
			}
			return *this;
		}
		template <typename _Iterable>
		pf_decl_constexpr forward_list<_Ty, _Magnifier, _Allocator> &
		operator=(
			_Iterable __ia) requires(is_const_iterable_v<_Iterable>)
		{
			// TODO: this->__assign
			return *this;
		}
		// TODO: Test operator= array[_Num]

		/// Operator ==
		pf_decl_constexpr bool
		operator==(
			forward_view<_Ty> __v) pf_attr_noexcept
		{
			return this->view() == __v;
		}
		pf_decl_constexpr bool
		operator!=(
			forward_view<_Ty> __v) pf_attr_noexcept
		{
			return this->view() != __v;
		}

		/// Operator []

		/// Operator (Span)

		/// Operator (_Ty *)

		/// Insert

		/// Replace

		/// Erase

		/// Reserve

		/// Resize

		/// Realign

		/// Begin

		/// End

		/// Size

		/// Count

		/// Capacity

		/// Alignment

		/// Magnifier

		/// Allocator

		/// View

		/// Shrink

		/// Clear
		pf_decl_constexpr void clear() pf_attr_noexcept
		{
			// TODO: Clear
		}

		/// Is Empty

	private:
		__forward_block<_Ty> *head_;		// Head -> Singly-Linked list of blocks
		__forward_block<_Ty> *tail_;		// Tail -> for memory blocks management
		__forward_node<_Ty> *next_;		// Next first block to insert element
		size_t count_;								// Number of elements
		align_val_t elemMaxAlign_;	// Max alignment of each elements inside blocks
		pf_hint_nounique_address magnifier_t magnifier_;
		pf_hint_nounique_address allocator_t allocator_;
	};
}

#endif // !PULSAR_LIST_HPP