/*! @file   singly_list.hpp
 *  @author Fluffy (noe.louis-quentin@hotmail.fr)
 *  @brief	Defines singly linked lists.
 *  @date   24-05-2022
 *
 *  @copyright Copyright (c) 2022 - Pulsar Software
 *
 *  @since 0.1.1
 */

#ifndef PULSAR_SINGLY_LINKED_LIST_HPP
#define PULSAR_SINGLY_LINKED_LIST_HPP 1

// Include: Pulsar
#include "pulsar/pulsar.hpp"

// Include: C++
#include <atomic>
#include <type_traits>

// Pulsar
namespace pul
{
	/// SINGLY: Types
	template <typename _Ty>
		requires(!std::is_void_v<_Ty>)
	class singly_node;
	template <typename _Ty>
	class singly_iterator;
	template <typename _Ty>
	class singly_linked_list;
	template <typename _Ty>
	class singly_rotative_linked_list;

	/// SINGLY: CDS Types
	template <typename _Ty>
	class singly_cds_linked_list;
	template <typename _Ty>
	class singly_cds_linked_lifo;
	template <typename _Ty>
	class singly_cds_linked_fifo;

	/// SINGLY: Node
	/*! @brief Singly linked node.
	 *
	 *  @tparam _Ty Type of contained object.
	 */
	template <typename _Ty>
		requires(!std::is_void_v<_Ty>)
	class singly_node
	{
		/// Linked-Lists
		pf_decl_friend class singly_iterator<std::remove_const_t<_Ty>>;
		pf_decl_friend class singly_iterator<const std::remove_const_t<_Ty>>;
		pf_decl_friend class singly_linked_list<_Ty>;
		pf_decl_friend class singly_rotative_linked_list<_Ty>;
		/// CDS
		pf_decl_friend class singly_cds_linked_list<_Ty>;
		pf_decl_friend class singly_cds_linked_lifo<_Ty>;
		pf_decl_friend class singly_cds_linked_fifo<_Ty>;

		template <typename _InIterator>
		pf_decl_friend pf_decl_constexpr auto singly_link(
				_InIterator __beg,
				_InIterator __end);
		template <typename _InIterator>
		pf_decl_friend pf_decl_constexpr auto singly_last(
				_InIterator __beg);

		/// Unlink
		/*! @brief Removes the links to other nodes.
		 */
		pf_decl_constexpr void __unlink() pf_attr_noexcept
		{
			this->next_ = nullptr;
		}

	public:
		/// Constructors
		/*! @brief Default constructor.
		 */
		pf_decl_constexpr singly_node() requires(std::is_default_constructible_v<std::remove_all_extents_t<_Ty>>)
				: next_(nullptr)
		{}
		/*! @brief Constructor. Builds the @a _Ty object and initializes the node to zero.
		 *
		 *  @tparam _Args Types of arguments.
		 *  @param[in] __args Arguments to pass to the constructor of @a _Ty .
		 */
		template <typename... _Args>
		pf_decl_constexpr singly_node(
				_Args &&...__args)
				: next_(nullptr)
				, store_(std::forward<_Args>(__args)...)
		{}
		/*! @brief Constructor. Construct from an @a _Ty move.
		 *
		 *  @param[in] __val Instance to be moved.
		 */
		pf_decl_constexpr singly_node(
				_Ty &&__val) requires(std::is_move_constructible_v<_Ty>)
				: next_(nullptr)
				, store_(std::move(__val))
		{}
		/*! @brief Constructor. Construct from an @a _Ty copy.
		 *
		 *  @param[in] __val Instance to be copied.
		 */
		pf_decl_constexpr singly_node(
				_Ty const &__val) requires(std::is_copy_constructible_v<_Ty>)
				: next_(nullptr)
				, store_(__val)
		{}
		/*! @brief Copy constructor.
		 *
		 *  @param[in] __r Other instance to copy from.
		 */
		singly_node(singly_node<_Ty> const &__r) = delete;

		/// Destructor
		/*! @brief Destructor.
		 */
		pf_decl_constexpr ~singly_node() pf_attr_noexcept = default;

		/// Operator=
		/*! @brief Assignment operator. Move an @a __Ty into this node.
		 *
		 *  @param[in] __val Instance to be moved.
		 *  @return Reference on this instance.
		 */
		pf_decl_constexpr singly_node<_Ty> &operator=(
				_Ty &&__val) requires(std::is_move_assignable_v<_Ty>)
		{
			this->store_ = std::move(__val);
			return *this;
		}
		/*! @brief Assignment operator. Copy an @a __Ty into this node.
		 *
		 *  @param[in] __val Instance to be copied.
		 *  @return Reference on this instance.
		 */
		pf_decl_constexpr singly_node<_Ty> &operator=(
				_Ty const &__val) requires(std::is_copy_assignable_v<_Ty>)
		{
			this->store_ = __val;
			return *this;
		}
		/*! @brief Copy assignment operator.
		 *
		 *  @param[in] __r Other instance to copy from.
		 *  @return Reference on this instance.
		 */
		singly_node<_Ty> &operator=(singly_node<_Ty> const &__r) = delete;

		/// Operator*
		/*! @brief Deference operator.
		 *
		 *  @return Reference to the contained @a _Ty instance.
		 */
		pf_hint_nodiscard pf_decl_constexpr _Ty &operator*() pf_attr_noexcept
		{
			return this->store_;
		}
		/*! @brief Const deference operator.
		 *
		 *  @return Const reference to the contained @a _Ty instance.
		 */
		pf_hint_nodiscard pf_decl_constexpr const _Ty &operator*() const pf_attr_noexcept
		{
			return this->store;
		}

		/// Operator->
		/*! @brief Arrow operator.
		 *
		 *  @return Pointer to the contained @a _Ty instance.
		 */
		pf_hint_nodiscard pf_decl_constexpr _Ty *operator->() pf_attr_noexcept
		{
			return &this->store_;
		}
		/*! @brief Const arrow operator.
		 *
		 *  @return Const pointer to the contained @a _Ty instance.
		 */
		pf_hint_nodiscard pf_decl_constexpr const _Ty *operator->() const pf_attr_noexcept
		{
			return &this->store_;
		}

		/// Operator==
		/*! @brief Equality operator.
		 *
		 *  @param[in] __r node from which to compare the instances of @a _Ty contained.
		 *  @return True if equal.
		 *  @return False if not equal.
		 */
		pf_hint_nodiscard pf_decl_constexpr bool operator==(
				singly_node<_Ty> const &__r) const pf_attr_noexcept
		{
			return this->store_ == __r.store_;
		}

		/// Operator<=>
		/*! @brief Spaceship operator.
		 *
		 *  @param[in] __r Another node from which to compare the instances of @a _Ty contained.
		 *  @return Operator (<, <=, >, >=) equality.
		 */
		pf_hint_nodiscard pf_decl_constexpr bool operator<=>(
				singly_node<_Ty> const &__r) const pf_attr_noexcept
		{
			return this->store_ <=> __r->store_;
		}

		/// Operator (Cast)
		pf_hint_nodiscard pf_decl_constexpr operator _Ty &() pf_attr_noexcept
		{
			return this->store_;
		}
		pf_hint_nodiscard pf_decl_constexpr operator const _Ty &() const pf_attr_noexcept
		{
			return this->store_;
		}

		/// Next
		/*! @brief Get a pointer to the next node.
		 *
		 *  @return Pointer to the next node.
		 */
		pf_hint_nodiscard pf_decl_constexpr singly_node<_Ty> *next() const pf_attr_noexcept
		{
			return this->next_;
		}

		/// Is Linked
		/*! @brief Checks if the next contained node exists.
		 *
		 *  @return True if this->next_ isn't equal to nullptr.
		 *  @return False if this->next_ is equal to nullptr.
		 */
		pf_hint_nodiscard pf_decl_constexpr bool is_linked() const pf_attr_noexcept
		{
			return this->next_;
		}

	private:
		singly_node<_Ty> *next_;
		_Ty store_;
	};

	/// SINGLY: Iterator
	/*! @brief Singly iterator for singly linked nodes uses.
	 *
	 *  @tparam _Ty Type of working object.
	 */
	template <typename _Ty>
	class singly_iterator
	{
		pf_decl_friend singly_iterator<const _Ty>;

	public:
		using iterator_category = std::forward_iterator_tag;
		using value_type				= _Ty;
		using node							= singly_node<_Ty>;
		using difference_type		= diff_t;

		/// Constructors
		/*! @brief Constructor.
		 *
		 *  @param[in] __n Pointer to a singly linked node.
		 */
		pf_decl_constexpr singly_iterator(
				node *__n = nullptr) pf_attr_noexcept
				: ptr_(__n)
		{}
		/*! @brief Copy constructor.
		 *
		 *  @param[in] __i Another iterator from which to copy the pointer.
		 */
		pf_decl_constexpr singly_iterator(
				singly_iterator<_Ty> const &__i) pf_attr_noexcept
				: ptr_(__i.ptr_)
		{}

		/// Operator=
		/*! @brief Copy assignment operator.
		 *
		 *  @param[in] __i Another iterator from which to copy the pointer.
		 *  @return Reference on this instance.
		 */
		pf_decl_constexpr singly_iterator<_Ty> &operator=(
				singly_iterator<_Ty> const &__i) pf_attr_noexcept
		{
			this->ptr_ = __i.ptr_;
			return *this;
		}

		/// Operator++
		/*! @brief Pre-incremental operator.
		 *
		 *  @return Reference on this instance after incrementation.
		 */
		pf_decl_constexpr singly_iterator<_Ty> &operator++() pf_attr_noexcept
		{
			this->ptr_ = this->ptr_->next_;
			return *this;
		}
		/*! @brief Post-incremental operator.
		 *
		 *  @param[in] __i Identifier for pre-incremental operator.
		 *  @return Copy of this instance before incrementation.
		 */
		pf_decl_constexpr singly_iterator<_Ty> operator++(
				int32_t __i) pf_attr_noexcept
		{
			singly_iterator<_Ty> c = this->ptr_;
			this->ptr_						 = this->ptr_->next_;
			return c;
		}

		/// Operator+=
		/*! @brief Addition operator.
		 *
		 *  @param[in] __i Difference to be applied to the iterator as is this->ptr_ = this->ptr_->next_
		 *  					 by __i times.
		 *  @return Reference on this instance after difference.
		 */
		pf_decl_constexpr singly_iterator<_Ty> &operator+=(
				difference_type __i) pf_attr_noexcept
		{
			for (; __i > 0; --__i)
			{
				this->ptr_ = this->ptr_->next_;
			}
			return *this;
		}

		/// Base
		/*! @brief Get the pointer associated with this iterator.
		 *
		 *  @return Pointer on a singly_node of this iterator.
		 */
		pf_hint_nodiscard pf_decl_constexpr node *base() const pf_attr_noexcept
		{
			return this->ptr_;
		}

		/// Operator*
		/*! @brief Deference operator.
		 *
		 *  @return Reference to the contained pointed node @a _Ty instance.
		 */
		pf_hint_nodiscard pf_decl_constexpr _Ty &operator*() const pf_attr_noexcept
		{
			return this->base()->store_;
		}

		/// Operator->
		/*! @brief Arrow operator.
		 *
		 *  @return Pointer to the contained pointed node @a _Ty instance.
		 */
		pf_hint_nodiscard pf_decl_constexpr _Ty *operator->() const pf_attr_noexcept
		{
			return &this->base()->store_;
		}

		/// Operator cast: node*
		/*! @brief Cast operator to node *. Convert this instance type to a node object with the stored
		 *				 iterator.
		 *
		 *  @return Converted pointer of type node*.
		 */
		pf_hint_nodiscard pf_decl_constexpr operator node *() const pf_attr_noexcept
		{
			return this->base();
		}

		/// Operator==
		/*! @brief Equality operator. Checks if two iterators have the same pointer.
		 *
		 *  @param[in] __r Another iterator to compare with.
		 *  @return True if is equal.
		 *  @return False if isn't equal.
		 */
		pf_hint_nodiscard pf_decl_constexpr bool operator==(
				singly_iterator<_Ty> const &__r) const pf_attr_noexcept
		{
			return this->ptr_ == __r.ptr_;
		}
		/*! @brief Equality operator. Checks if contained iterator pointer is the same as @a __p .
		 *
		 *  @param[in] __p Pointer on a node to compare with.
		 *  @return True if is equal.
		 *  @return False if isn't equal.
		 */
		pf_hint_nodiscard pf_decl_constexpr bool operator==(
				node *__p) const pf_attr_noexcept
		{
			return this->ptr_ == __p;
		}

	private:
		node *ptr_;
	};

	/// SINGLY: Link list of Nodes
	template <typename _InIterator>
	pf_decl_constexpr auto singly_link(
			_InIterator __beg,
			_InIterator __end)
	{
		// Init List
		singly_iterator b = &*__beg;
		++__beg;
		for (;;)
		{
			if (__beg == __end)
			{
				return b.base();
			}
			b.base()->next_ = &*__beg;
			++__beg;
			++b;
		}
	}
	template <typename _InIterator>
	pf_decl_constexpr auto singly_last(
			_InIterator __beg)
	{
		singly_iterator b = &*__beg;
		while (b.base()->next_)
		{
			b = b.base()->next_;
		}
		return b.base();
	}


	/// SINGLY: Const Iterator
	/*! @brief Specialization of a singly iterator with the type @a _Ty = const _Ty (const iterator).
	 *
	 *  @tparam @a _Ty = const _Ty
	 */
	template <typename _Ty>
	class singly_iterator<const _Ty>
	{
	public:
		using iterator_category = std::forward_iterator_tag;
		using value_type				= const _Ty;
		using node							= const singly_node<_Ty>;
		using difference_type		= diff_t;

		/// Constructors
		/*! @brief Constructor.
		 *
		 *  @param[in] __n Const pointer to a singly linked node.
		 */
		pf_decl_constexpr singly_iterator(
				node *__n = nullptr) pf_attr_noexcept
				: ptr_(__n)
		{}
		/*! @brief Copy constructor.
		 *
		 *  @param[in] __i Another const iterator from which to copy the pointer.
		 */
		pf_decl_constexpr singly_iterator(
				singly_iterator<const _Ty> const &__i) pf_attr_noexcept
				: ptr_(__i.ptr_)
		{}

		/// Operator=
		/*! @brief Copy assignment operator.
		 *
		 *  @param[in] __i Another const iterator from which to copy the pointer.
		 *  @return Reference on this instance.
		 */
		pf_decl_constexpr singly_iterator<const _Ty> &operator=(
				singly_iterator<const _Ty> const &__i) pf_attr_noexcept
		{
			this->ptr_ = __i.ptr_;
			return *this;
		}

		/// Operator++
		/*! @brief Pre-incremental operator.
		 *
		 *  @return Reference on this instance after incrementation.
		 */
		pf_decl_constexpr singly_iterator<const _Ty> &operator++() pf_attr_noexcept
		{
			this->ptr_ = this->ptr_->next_;
			return *this;
		}
		/*! @brief Post-incremental operator.
		 *
		 *  @param[in] __i Identifier for pre incremental operator.
		 *  @return Copy of this instance before incrementation.
		 */
		pf_decl_constexpr singly_iterator<const _Ty> operator++(
				int32_t __i) pf_attr_noexcept
		{
			singly_iterator<const _Ty> c = this->ptr_;
			this->ptr_									 = this->ptr_->next_;
			return c;
		}

		/// Operator+=
		/*! @brief Addition operator.
		 *
		 *  @param[in] __i Difference to be applied to the iterator as is this->ptr_ = this->ptr_->next_
		 *  					 by __i times.
		 *  @return Reference on this instance after difference.
		 */
		pf_decl_constexpr singly_iterator<const _Ty> &operator+=(
				difference_type __i) pf_attr_noexcept
		{
			for (; __i > 0; --__i)
			{
				this->ptr_ = this->ptr_->next_;
			}
			return *this;
		}

		/// Base
		/*! @brief Get the pointer associated with this iterator.
		 *
		 *  @return Const pointer on a singly_node of this iterator.
		 */
		pf_hint_nodiscard pf_decl_constexpr node *base() const pf_attr_noexcept
		{
			return this->ptr_;
		}

		/// Operator*
		/*! @brief Deference operator.
		 *
		 *  @return Const reference to the contained pointed node @a _Ty instance.
		 */
		pf_hint_nodiscard pf_decl_constexpr const _Ty &operator*() const pf_attr_noexcept
		{
			return this->base()->store_;
		}

		/// Operator->
		/*! @brief Arrow operator.
		 *
		 *  @return Const pointer to the contained pointed node @a _Ty instance.
		 */
		pf_hint_nodiscard pf_decl_constexpr const _Ty *operator->() const pf_attr_noexcept
		{
			return &this->base()->store_;
		}

		/// Operator cast: node*
		/*! @brief Cast operator to node *. Convert this instance type to a node object with the stored
		 *				 iterator.
		 *
		 *  @return Converted pointer of type node*.
		 */
		pf_hint_nodiscard pf_decl_constexpr operator node *() const pf_attr_noexcept
		{
			return this->base();
		}

		/// Operator==
		/*! @brief Equality operator. Checks if two iterators have the same pointer.
		 *
		 *  @param[in] __r Another iterator to compare with.
		 *  @return True if is equal.
		 *  @return False if isn't equal.
		 */
		pf_hint_nodiscard pf_decl_constexpr bool operator==(
				singly_iterator<const _Ty> const &__r) const pf_attr_noexcept
		{
			return this->ptr_ == __r.ptr_;
		}
		/*! @brief Equality operator. Checks if contained iterator pointer is the same as @a __p .
		 *
		 *  @param[in] __p Pointer on a node to compare with.
		 *  @return True if is equal.
		 *  @return False if isn't equal.
		 */
		pf_hint_nodiscard pf_decl_constexpr bool operator==(
				node *__p) const pf_attr_noexcept
		{
			return this->ptr_ == __p;
		}

	private:
		node *ptr_;
	};

	/// SINGLY: Iterator Operator+
	/*! @brief Addition operator.
	 *
	 *  @tparam _Ty Type of the encapsulated object.
	 *  @param[in] __it Iterator from which to apply the difference.
	 *  @param[in] __i Difference.
	 *  @return Copy of the iterator checking __it += __i.
	 */
	template <typename _Ty>
	pf_hint_nodiscard pf_decl_constexpr singly_iterator<_Ty> operator+(
			singly_iterator<_Ty> __it,
			diff_t __i) pf_attr_noexcept
	{
		return __it += __i;
	}

	/*! @brief Alias of a constant singly iterator.
	 *
	 *  @tparam _Ty Type of the encapsulated object.
	 */
	template <typename _Ty>
	using singly_const_iterator = singly_iterator<const std::remove_const_t<_Ty>>;

	/// SINGLY: Const Iterator -> Deduction Guide
	template <typename _Ty>
	singly_iterator(const singly_node<_Ty> *)
			-> singly_iterator<const _Ty>;

	/// SINGLY: Linked-List
	/*! @brief Singly linked-list of nodes.
	 *
	 *  @tparam _Ty Type of the encapsulated object.
	 */
	template <typename _Ty>
	class singly_linked_list pf_attr_final
	{
	public:
		using value_type		 = _Ty;
		using node					 = singly_node<_Ty>;
		using iterator			 = singly_iterator<_Ty>;
		using const_iterator = singly_const_iterator<_Ty>;

		/// Constructors
		/*! @brief Default constructor. Initializes the control variables to nullptr.
		 */
		pf_decl_constexpr singly_linked_list() pf_attr_noexcept
				: head_(nullptr)
				, tail_(nullptr)
		{}
		/*! @brief Move constructor.
		 *
		 *  @param[in] __r Another instance to move from.
		 */
		pf_decl_constexpr singly_linked_list(
				singly_linked_list<_Ty> &&__r) pf_attr_noexcept
				: head_(__r.head_)
				, tail_(__r.tail_)
		{
			__r.head_ = nullptr;
			__r.tail_ = nullptr;
		}

		/// Destructor
		/*! @brief Destructor. Clear the linked-list.
		 */
		pf_decl_constexpr ~singly_linked_list() pf_attr_noexcept
		{
			this->clear();
		}

		/// Operator=
		/*! @brief Move assignment operator.
		 *
		 *  @param[in] __r Another instance to move from. Sets to nullptr the control variables of the
		 * 								 other @a __r list.
		 *  @return Reference on this list.
		 */
		pf_decl_constexpr singly_linked_list<_Ty> &operator=(
				singly_linked_list<_Ty> &&__r) pf_attr_noexcept
		{
			if (!this->empty()) this->clear();
			this->head_ = __r.head_;
			__r.head_		= nullptr;
			this->tail_ = __r.tail_;
			__r.tail_		= nullptr;
			return *this;
		}

		/// Head
		/*! @brief Get a pointer to the head of the list.
		 *
		 *  @return Pointer to the head of the list.
		 */
		pf_hint_nodiscard pf_decl_constexpr node *head() pf_attr_noexcept
		{
			return this->head_;
		}
		/*! @brief Get a constant pointer to the head of the list.
		 *
		 *  @return Constant pointer to the head of the list.
		 */
		pf_hint_nodiscard pf_decl_constexpr const node *head() const pf_attr_noexcept
		{
			return this->head_;
		}

		/// Begin
		/*! @brief
		 *
		 *  @return pf_hint_nodiscard
		 */
		pf_hint_nodiscard pf_decl_constexpr iterator begin() pf_attr_noexcept
		{
			return this->head_;
		}
		/*! @brief
		 *
		 *  @return pf_hint_nodiscard
		 */
		pf_hint_nodiscard pf_decl_constexpr const_iterator cbegin() const pf_attr_noexcept
		{
			return this->head_;
		}
		/*! @brief
		 *
		 *  @return pf_hint_nodiscard
		 */
		pf_hint_nodiscard pf_decl_constexpr const_iterator begin() const pf_attr_noexcept
		{
			return this->cbegin();
		}

		/// Tail
		/*! @brief Get a pointer to the tail of the list.
		 *
		 *  @return Pointer to the tail of the list.
		 */
		pf_hint_nodiscard pf_decl_constexpr node *tail() pf_attr_noexcept
		{
			return this->tail_;
		}
		/*! @brief Get a constant pointer to the tail of the list.
		 *
		 *  @return Constant pointer to the tail of the list.
		 */
		pf_hint_nodiscard pf_decl_constexpr const node *tail() const pf_attr_noexcept
		{
			return this->tail_;
		}

		/// End
		/*! @brief
		 *
		 *  @return pf_hint_nodiscard
		 */
		pf_hint_nodiscard pf_decl_constexpr iterator end() pf_attr_noexcept
		{
			return nullptr;
		}
		/*! @brief
		 *
		 *  @return pf_hint_nodiscard
		 */
		pf_hint_nodiscard pf_decl_constexpr const_iterator cend() const pf_attr_noexcept
		{
			return nullptr;
		}
		/*! @brief
		 *
		 *  @return pf_hint_nodiscard
		 */
		pf_hint_nodiscard pf_decl_constexpr const_iterator end() const pf_attr_noexcept
		{
			return this->cend();
		}

		/// Insert
		/*! @brief Link @a __n in the place of the head of the list.
		 *
		 *  @param[in] __n Pointer on a node to link.
		 *  @return Pointer on the linked node.
		 */
		pf_decl_constexpr node *insert_head(
				node *__n) pf_attr_noexcept
		{
			pf_assert(__n != nullptr, "__n is nullptr!");
			pf_assert(!this->contains(__n), "__n is already contained in this list!");
			pf_assert(!__n->is_linked(), "__n is already linked!");
			if (!this->head_)
			{
				this->head_ = __n;
				this->tail_ = __n;
			}
			else
			{
				__n->next_	= this->head_;
				this->head_ = __n;
			}
			return this->head_;
		}

		/*! @brief Link @a __n in the place of the tail of the list.
		 *
		 *  @param[in] __n Pointer on a node to link.
		 *  @return Pointer on the linked node.
		 */
		pf_decl_constexpr node *insert_tail(
				node *__n) pf_attr_noexcept
		{
			pf_assert(__n != nullptr, "__n is nullptr!");
			pf_assert(!this->contains(__n), "__n is already contained in this list!");
			pf_assert(!__n->is_linked(), "__n is already linked!");
			if (!this->tail_)
			{
				this->head_ = __n;
				this->tail_ = __n;
			}
			else
			{
				this->tail_->next_ = __n;
				this->tail_				 = __n;
			}
			return this->tail_;
		}

		/*! @brief Link @a __n after @a __w.
		 *
		 *  @param[in] __w Pointer on a node after which to link @a __n.
		 *  @param[in] __n Pointer on a node to link.
		 *  @return Pointer on the linked node.
		 */
		pf_decl_constexpr node *insert_next(
				node *__w,
				node *__n) pf_attr_noexcept
		{
			if (!__w) return this->insert_head(__n);
			if (__w == this->tail_) return this->insert_tail(__n);
			pf_assert(__n != nullptr, "__n is nullptr!");
			pf_assert(!this->contains(__n), "__n is already contained in this list!");
			pf_assert(!__n->is_linked(), "__n is already linked!");
			__n->next_ = __w->next_;
			__w->next_ = __n;
			return __w;
		}

		/*! @brief Link @a __n before @a __w.
		 *
		 *  @param[in] __w Pointer on a node before which to link @a __n.
		 *  @param[in] __n Pointer on a node to link.
		 *  @return Pointer on the linked node.
		 */
		pf_decl_constexpr node *insert_prev(
				node *__w,
				node *__n) pf_attr_noexcept
		{
			if (!__w) return this->insert_tail(__n);
			if (__w == this->head_) return this->insert_head(__n);
			pf_assert(__n != nullptr, "__n is nullptr!");
			pf_assert(!this->contains(__n), "__n is already containted in this list!");
			pf_assert(!__n->is_linked(), "__n is already linked!");
			node *p		 = this->parent_of(__w);
			p->next_	 = __n;
			__n->next_ = __w;
			return __n;
		}

		/// Remove
		/*! @brief Unlink the head of the list.
		 *
		 *  @return Pointer on the unlinked node.
		 */
		pf_decl_constexpr node *remove_head() pf_attr_noexcept
		{
			if (this->empty()) return nullptr;
			node *c			= this->head_;
			this->head_ = this->head_->next_;
			if (!this->head_) this->tail_ = nullptr;
			c->__unlink();
			return c;
		}

		/*! @brief Unlink the tail of the list.
		 *
		 *  @return Pointer on the unlinked node.
		 */
		pf_decl_constexpr node *remove_tail() pf_attr_noexcept
		{
			if (this->empty()) return nullptr;
			node *c = this->tail_, *p = this->parent_of(this->tail_);
			if (p)
			{
				p->next_ = nullptr;
			}
			this->tail_ = p;
			if (!this->tail_) this->head_ = nullptr;
			c->__unlink();
			return c;
		}

		/*! @brief Unlink a specific @a __n node.
		 *
		 *  @param[in] __n Pointer on a node to unlink.
		 *  @return Pointer on the unlinked node.
		 */
		pf_decl_constexpr node *remove(
				node *__n) pf_attr_noexcept
		{
			pf_assert(__n != nullptr, "__n is nullptr!");
			pf_assert(this->contains(__n), "__n isn't contained in the list!");
			if (__n == this->head_) return this->remove_head();
			if (__n == this->tail_) return this->remove_tail();
			node *p	 = this->parent_of(__n);
			p->next_ = __n->next_;
			__n->__unlink();
			return __n;
		}

		/*! @brief Unlink the node after @a __w.
		 *
		 *  @param[in] __w Pointer on a node after which to unlink a node.
		 *  @return Pointer on the unlinked node.
		 */
		pf_decl_constexpr node *remove_next(
				node *__w) pf_attr_noexcept
		{
			pf_assert(this->contains(__w), "__w isn't contained in the list!");
			if (!__w) return this->remove_head();
			node *c = __w->next_;
			if (c == this->tail_) return this->remove_tail();
			__w->next_ = c->next_;
			c->__unlink();
			return c;
		}

		/*! @brief Unlink the node before @a __w.
		 *
		 *  @param[in] __n Pointer of a node before wich to unlink a node.
		 *  @return Pointer on the unlinked node.
		 */
		pf_decl_constexpr node *remove_prev(
				node *__w) pf_attr_noexcept
		{
			pf_assert(this->contains(__w), "__n isn't contained in the list!");
			if (!__w) return this->remove_tail();
			if (__w == this->head_) return nullptr;
			// double parent needed
			node *c = this->head_, *p = nullptr;
			while (c && c->next_ != this->head_ && c->next_ != __w)
			{
				p = c;
				c = c->next_;
			}
			if (c == this->head_) return this->remove_head();
			p->next_ = __w;
			c->__unlink();
			return c;
		}

		/// Parent
		/*! @brief Get the parent of a node from this list.
		 *
		 *  @param[in] __n Constant pointer to a node from which to retrieve its parent.
		 *  @return Pointer on the parent node of @a __n.
		 */
		pf_hint_nodiscard pf_decl_constexpr node *parent_of(
				const node *__n) pf_attr_noexcept
		{
			pf_assert(__n != nullptr, "__n is nullptr!");
			pf_assert(this->contains(__n), "__n isn't contained in the list!");
			node *p = nullptr, *c = this->head_;
			while (c && c != __n)
			{
				p = c;
				c = c->next_;
			}
			return p;
		}
		/*! @brief Get the parent of a node from this list.
		 *
		 *  @param[in] __n Constant pointer to a node from which to retrieve its parent.
		 *  @return Constant pointer on the parent node of @a __n.
		 */
		pf_hint_nodiscard pf_decl_constexpr const node *parent_of(
				const node *__n) const pf_attr_noexcept
		{
			this->parent_of(__n);
		}

		/// Empty
		/*! @brief Checks if this list is empty.
		 *
		 *  @return True if this list is empty.
		 *  @return False if this list isn't empty.
		 */
		pf_hint_nodiscard pf_decl_constexpr bool empty() const pf_attr_noexcept
		{
			return !this->head_;
		}

		/// Contains
		/*! @brief Check if @a __n is contained in this list.
		 *
		 *  @param[in] __n Pointer on a node.
		 *  @return True if @a __n is contained in this list.
		 *  @return False otherwise.
		 */
		pf_hint_nodiscard pf_decl_constexpr bool contains(
				const node *__n) const pf_attr_noexcept
		{
			const node *c = this->head_;
			while (c)
			{
				if (c == __n) return true;
				c = c->next_;
			}
			return false;
		}

		/// Size
		/*! @brief Retrieves the size of the list.
		 *
		 *  @return The size of the list. 0 if empty.
		 */
		pf_hint_nodiscard pf_decl_constexpr size_t size() const pf_attr_noexcept
		{
			size_t i			= 0;
			const node *c = this->head_;
			while (c)
			{
				c = c->next_;
				++i;
			}
			return i;
		}

		/// Clear
		/*! @brief Unlink all the nodes associated with this list.
		 */
		pf_decl_constexpr void clear() pf_attr_noexcept
		{
			while (this->remove_head())
				;
		}

	private:
		node *head_;
		node *tail_;
	};

	/// SINGLY: Rotative Linked-List
	/*! @brief Singly linked-list of nodes. Rotative version wich means, the tail is always linked to
	 *				 the head except if nullptr.
	 *
	 *  @tparam _Ty Type of the encapsulated object.
	 */
	template <typename _Ty>
	class singly_rotative_linked_list pf_attr_final
	{
	public:
		using value_type		 = _Ty;
		using node					 = singly_node<_Ty>;
		using iterator			 = singly_iterator<_Ty>;
		using const_iterator = singly_const_iterator<_Ty>;

		/// Constructors
		/*! @brief Default constructor. Initializes the control variables to nullptr.
		 */
		pf_decl_constexpr singly_rotative_linked_list() pf_attr_noexcept
				: head_(nullptr)
				, tail_(nullptr)
		{}
		/*! @brief Move constructor.
		 *
		 *  @param[in] __r Another instance to move from.
		 */
		pf_decl_constexpr singly_rotative_linked_list(
				singly_rotative_linked_list<_Ty> &&__r) pf_attr_noexcept
				: head_(__r.head_)
				, tail_(__r.tail_)
		{
			__r.head_ = nullptr;
			__r.tail_ = nullptr;
		}

		/// Destructor
		/*! @brief Destructor. Clear the linked-list.
		 */
		pf_decl_constexpr ~singly_rotative_linked_list() pf_attr_noexcept
		{
			this->clear();
		}

		/// Operator=
		/*! @brief Move assignment operator.
		 *
		 *  @param[in] __r Another instance to move from. Sets to nullptr the control variables of the
		 * 								 other @a __r list.
		 *  @return Reference on this list.
		 */
		pf_decl_constexpr singly_rotative_linked_list<_Ty> &operator=(
				singly_rotative_linked_list<_Ty> &&__r) pf_attr_noexcept
		{
			if (!this->empty()) this->clear();
			this->head_ = __r.head_;
			__r.head_		= nullptr;
			this->tail_ = __r.tail_;
			__r.tail_		= nullptr;
			return *this;
		}

		/// Head
		/*! @brief Get a constant pointer on the head of the list.
		 *
		 *  @return Constant pointer on the head of the list.
		 */
		pf_hint_nodiscard pf_decl_constexpr node *head() pf_attr_noexcept
		{
			return this->head_;
		}
		/*! @brief Get a pointer on the head of the list.
		 *
		 *  @return Pointer on the head of the list.
		 */
		pf_hint_nodiscard pf_decl_constexpr const node *head() const pf_attr_noexcept
		{
			return this->head_;
		}

		/// Tail
		/*! @brief Get a pointer on the tail of the list.
		 *
		 *  @return Pointer on the tail of the list.
		 */
		pf_hint_nodiscard pf_decl_constexpr node *tail() pf_attr_noexcept
		{
			return this->tail_;
		}
		/*! @brief Get a constant pointer on the tail of the list.
		 *
		 *  @return Constant pointer on the tail of the list.
		 */
		pf_hint_nodiscard pf_decl_constexpr const node *tail() const pf_attr_noexcept
		{
			return this->tail_;
		}

		/// Begin
		/*! @brief
		 *
		 *  @return pf_hint_nodiscard
		 */
		pf_hint_nodiscard pf_decl_constexpr iterator begin() pf_attr_noexcept
		{
			return this->head_;
		}
		/*! @brief
		 *
		 *  @return pf_hint_nodiscard
		 */
		pf_hint_nodiscard pf_decl_constexpr const_iterator cbegin() const pf_attr_noexcept
		{
			return this->head_;
		}
		/*! @brief
		 *
		 *  @return pf_hint_nodiscard
		 */
		pf_hint_nodiscard pf_decl_constexpr const_iterator begin() const pf_attr_noexcept
		{
			return this->cbegin();
		}

		/// Insert
		/*! @brief Link @a __n in the place of the head of the list.
		 *
		 *  @param[in] __n Pointer on a node to link.
		 *  @return Pointer on the linked node.
		 */
		pf_decl_constexpr node *insert_head(
				node *__n) pf_attr_noexcept
		{
			pf_assert(__n != nullptr, "__n is nullptr!");
			pf_assert(!this->contains(__n), "__n is already contained in this list!");
			pf_assert(!__n->is_linked(), "__n is already linked!");
			if (!this->head_)
			{
				this->head_ = __n;
				this->tail_ = __n;
				__n->next_	= __n;
			}
			else
			{
				__n->next_				 = this->head_;
				this->head_				 = __n;
				this->tail_->next_ = __n;
			}
			return this->head_;
		}

		/*! @brief Link @a __n in the place of the tail of this list.
		 *
		 *  @param[in] __n Pointer on a node to link.
		 *  @return Pointer on the linked node.
		 */
		pf_decl_constexpr node *insert_tail(
				node *__n) pf_attr_noexcept
		{
			pf_assert(__n != nullptr, "__n is nullptr!");
			pf_assert(!this->contains(__n), "__n is already contained in this list!");
			pf_assert(!__n->is_linked(), "__n is already linked!");
			if (!this->tail_)
			{
				this->head_ = __n;
				this->tail_ = __n;
				__n->next_	= __n;
			}
			else
			{
				this->tail_->next_ = __n;
				this->tail_				 = __n;
				__n->next_				 = this->head_;
			}
			return this->tail_;
		}

		/*! @brief Link @a __n after @a __w.
		 *
		 *  @param[in] __w Pointer on a node after which to link @a __n.
		 *  @param[in] __n Pointer on a node to link.
		 *  @return Pointer on the linked node.
		 */
		pf_decl_constexpr node *insert_next(
				node *__w,
				node *__n) pf_attr_noexcept
		{
			if (!__w) return this->insert_head(__n);
			if (__w == this->tail_) return this->insert_tail(__n);
			pf_assert(__n != nullptr, "__n is nullptr!");
			pf_assert(!this->contains(__n), "__n is already contained in this list!");
			pf_assert(!__n->is_linked(), "__n is already linked!");
			__n->next_ = __w->next_;
			__w->next_ = __n;
			return __w;
		}

		/*! @brief Link @a __n before @a __w.
		 *
		 *  @param[in] __w Pointer on a node before which to link @a __n.
		 *  @param[in] __n Pointer on a node to link.
		 *  @return Pointer on the linked node.
		 */
		pf_decl_constexpr node *insert_prev(
				node *__w,
				node *__n) pf_attr_noexcept
		{
			if (!__w) return this->insert_tail(__n);
			if (__w == this->head_) return this->insert_head(__n);
			pf_assert(__n != nullptr, "__n is nullptr!");
			pf_assert(!this->contains(__n), "__n is already contained in this list!");
			pf_assert(!__n->is_linked(), "__n is already linked!");
			node *p		 = this->parent_of(__w);
			p->next_	 = __n;
			__n->next_ = __w;
			return __n;
		}

		/// Remove
		/*! @brief Unlink the head of this list.
		 *
		 *  @return Pointer on the unlinked node.
		 */
		pf_decl_constexpr node *remove_head() pf_attr_noexcept
		{
			if (this->empty()) return nullptr;
			node *c = this->head_;
			if (this->head_->next_ == this->head_)
			{
				this->head_ = nullptr;
				this->tail_ = nullptr;
			}
			else
			{
				this->head_				 = this->head_->next_;
				this->tail_->next_ = this->head_;
			}
			c->__unlink();
			return c;
		}

		/*! @brief Unlink the tail of this list.
		 *
		 *  @return Pointer on the unlinked node.
		 */
		pf_decl_constexpr node *remove_tail() pf_attr_noexcept
		{
			if (this->empty()) return nullptr;
			node *c = this->tail_;
			if (this->tail_->next_ == this->tail_)
			{
				this->head_ = nullptr;
				this->tail_ = nullptr;
			}
			else
			{
				this->tail_				 = this->parent_of(this->tail_);
				this->tail_->next_ = this->head_;
			}
			c->__unlink();
			return c;
		}

		/*! @brief Unlink a specific @a __n node.
		 *
		 *  @param[in] __n Pointer on a node to unlink.
		 *  @return Pointer on the unlinked node.
		 */
		pf_decl_constexpr node *remove(
				node *__n) pf_attr_noexcept
		{
			pf_assert(__n != nullptr, "__n is nullptr!");
			pf_assert(this->contains(__n), "__n isn't contained in this list!");
			if (__n == head_) return this->remove_head();
			if (__n == tail_) return this->remove_tail();
			node *p	 = this->parent_of(__n);
			p->next_ = __n->next_;
			__n->__unlink();
			return __n;
		}

		/*! @brief Unlink the node after @a __w.
		 *
		 *  @param[in] __w Pointer on a node after which to unlink a node.
		 *  @return Pointer on the unlinked node.
		 */
		pf_decl_constexpr node *remove_next(
				node *__w) pf_attr_noexcept
		{
			pf_assert(this->contains(__w), "__w isn't contained in this list!");
			if (!__w) return this->remove_head();
			node *c = __w->next_;
			if (c == this->tail_) return this->remove_tail();
			__w->next_ = c->next_;
			c->__unlink();
			return c;
		}

		/*! @brief Unlink the node before @a __w.
		 *
		 *  @param[in] __n Pointer of a node before wich to unlink a node.
		 *  @return Pointer on the unlinked node.
		 */
		pf_decl_constexpr node *remove_prev(
				node *__w) pf_attr_noexcept
		{
			pf_assert(this->contains(__w), "__n isn't contained to this list!");
			if (!__w) return this->remove_tail();
			if (__w == this->head_) return nullptr;
			// double parent needed
			node *c = this->head_, *p = nullptr;
			while (c && c->next_ != this->head_ && c->next_ != __w)
			{
				p = c;
				c = c->next_;
			}
			if (c == this->head_) return this->remove_head();
			p->next_ = __w;
			c->__unlink();
			return c;
		}

		/// Parent
		/*! @brief Get the parent of a node from this list.
		 *
		 *  @param[in] __n Constant pointer to a node from which to retrieve its parent.
		 *  @return Pointer on the parent node of @a __n.
		 */
		pf_hint_nodiscard pf_decl_constexpr node *parent_of(
				const node *__n) pf_attr_noexcept
		{
			pf_assert(__n != nullptr, "__n is nullptr");
			pf_assert(this->contains(__n), "__n isn't contained in this list");
			node *c = this->head_;
			do
			{
				c = c->next_;
			} while (c != this->head_ && c->next_ != __n);
			return c;
		}
		/*! @brief Get the parent of a node from this list.
		 *
		 *  @param[in] __n Constant pointer to a node from which to retrieve its parent.
		 *  @return Constant pointer on the parent node of @a __n.
		 */
		pf_hint_nodiscard pf_decl_constexpr const node *parent_of(
				const node *__n) const pf_attr_noexcept
		{
			this->parent_of(__n);
		}

		/// Empty
		/*! @brief Checks if this list is empty.
		 *
		 *  @return True if this list is empty.
		 *  @return False if this list isn't empty.
		 */
		pf_hint_nodiscard pf_decl_constexpr bool empty() const pf_attr_noexcept
		{
			return !this->head_;
		}

		/// Contains
		/*! @brief Check if @a __n is contained in this list.
		 *
		 *  @param[in] __n Pointer on a node.
		 *  @return True if @a __n is contained in this list.
		 *  @return False otherwise.
		 */
		pf_hint_nodiscard pf_decl_constexpr bool contains(
				const node *__n) pf_attr_noexcept
		{
			pf_assert(__n != nullptr, "__n is nullptr");
			node *c = this->head_;
			if (!c)
				return false;
			do
			{
				if (c == __n) return true;
				c = c->next_;
			} while (c != this->head_);
			return false;
		}

		/// Size
		/*! @brief Retrieves the size of this list.
		 *
		 *  @return The size of this list. 0 if empty.
		 */
		pf_hint_nodiscard pf_decl_constexpr size_t size() const pf_attr_noexcept
		{
			if (this->empty()) return 0;
			size_t i = 0;
			node *c	 = this->head_;
			do
			{
				c = c->next_;
				++i;
			} while (c != this->head_);
			return i;
		}

		/// Clear
		/*! @brief Unlink all the nodes associated with this list.
		 */
		pf_decl_constexpr void clear() pf_attr_noexcept
		{
			while (!this->empty()) this->remove_head();
		}

	private:
		node *head_;
		node *tail_;
	};
}

#endif // !PULSAR_SINGLY_LINKED_LIST_HPP