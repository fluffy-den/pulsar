/*! @file   iterator.hpp
 *  @author Louis-Quentin Noé (noe.louis-quentin@hotmail.fr)
 *  @brief
 *  @date   02-01-2023
 *
 *  @copyright Copyright (c) 2023 - Pulsar Software
 *
 *  @since 0.1.3
 */

#ifndef PULSAR_ITERATOR_HPP
#define PULSAR_ITERATOR_HPP 1

// Include: Pulsar
#include "pulsar/pulsar.hpp"

// Include: C++
#include <type_traits>

// Pulsar
namespace pul
{
	/// ITERATOR: Category
	struct iterator_incrementable_tag_t
	{};
	struct iterator_decrementable_tag_t
	{};
	struct iterator_sequence_tag_t
		: public iterator_incrementable_tag_t
		, public iterator_decrementable_tag_t
	{};
	struct iterator_forward_iterator_tag_t
		: public iterator_incrementable_tag_t
	{};
	struct iterator_random_access_iterator_tag_t
		: public iterator_forward_iterator_tag_t
		, public iterator_decrementable_tag_t
	{};

	/// ITERATOR: Concept -> Arrowable
	template<typename _Iterator>
	concept __iterator_arrowable_c
		= std::is_trivial_v<typename _Iterator::value_t>
			|| requires(_Iterator __it, _Iterator const __ct)
		{
		{__it.operator->()}->std::convertible_to<typename _Iterator::value_t*>;
		{__ct.operator->()}->std::convertible_to<const typename _Iterator::value_t*>;
		};

	/// ITERATOR: Concept -> Base
	template<typename _Iterator>
	concept __iterator_base_c
		= std::is_copy_constructible_v<_Iterator>
			&& std::is_copy_assignable_v<_Iterator>
			&& !std::is_void_v<typename _Iterator::value_t>
			&& !std::is_const_v<typename _Iterator::value_t>
			&& __iterator_arrowable_c<_Iterator>
			&& requires(_Iterator __it, _Iterator const __ct)
		{
		{__it.get()}->std::convertible_to<typename _Iterator::value_t*>;
		{__ct.get()}->std::convertible_to<const typename _Iterator::value_t*>;
		{*__it}->std::convertible_to<typename _Iterator::value_t &>;
		{*__ct}->std::convertible_to<const typename _Iterator::value_t &>;
		{__ct == std::declval<const typename _Iterator::value_t*>()}->std::same_as<bool>;
		{__ct != std::declval<const typename _Iterator::value_t*>()}->std::same_as<bool>;
		};

	/// ITERATOR: Concept -> Const Arrowable
	template<typename _Iterator>
	concept __iterator_const_arrowable_c
		= std::is_trivial_v<typename _Iterator::value_t>
			|| requires(const _Iterator __it)
		{
		{__it.operator->()}->std::convertible_to<const typename _Iterator::value_t*>;
		};

	/// ITERATOR: Concept -> Const
	template<typename _Iterator>
	concept __iterator_base_const_c
		= std::is_copy_constructible_v<_Iterator>
			&& std::is_copy_assignable_v<_Iterator>
			&& !std::is_void_v<typename _Iterator::value_t>
			&& std::is_const_v<typename _Iterator::value_t>
			&& __iterator_const_arrowable_c<_Iterator>
			&& requires(const _Iterator __it)
		{
		{__it.get()}->std::convertible_to<const typename _Iterator::value_t*>;
		{*__it}->std::convertible_to<const typename _Iterator::value_t &>;
		{__it == std::declval<const typename _Iterator::value_t*>()}->std::same_as<bool>;
		{__it != std::declval<const typename _Iterator::value_t*>()}->std::same_as<bool>;
		};
	template <typename _Iterator>
	struct is_const_iterator : std::false_type
	{};
	template <__iterator_base_c _Iterator>
	struct is_const_iterator<_Iterator> : std::false_type
	{};
	template <__iterator_base_const_c _Iterator>
	struct is_const_iterator<_Iterator> : std::true_type
	{};
	template <typename _Iterator>
	pf_decl_static pf_decl_constexpr bool is_const_iterator_v
		= is_const_iterator<_Iterator>::value;

	/// ITERATOR: Concept -> Incrementable
	template<typename _Iterator>
	concept __iterator_incrementable_c
		= requires(_Iterator __it)
		{
		{++__it}->std::same_as<_Iterator &>;
		{__it++}->std::same_as<_Iterator>;
		{__it += std::declval<diff_t>()}->std::same_as<_Iterator &>;
		};
	template <typename _Iterator>
	struct is_incrementable : std::false_type
	{};
	template <__iterator_incrementable_c _Iterator>
	struct is_incrementable<_Iterator> : std::true_type
	{};
	template <typename _Iterator>
	pf_decl_static pf_decl_constexpr bool is_incrementable_v
		= is_incrementable<_Iterator>::value;

	/// ITERATOR: Concept -> Decrementable
	template<typename _Iterator>
	concept __iterator_decrementable_c
		= requires(_Iterator __it)
		{
		{--__it}->std::same_as<_Iterator &>;
		{__it--}->std::same_as<_Iterator>;
		{__it -= std::declval<diff_t>()}->std::same_as<_Iterator &>;
		};
	template <typename _Iterator>
	struct is_decrementable : std::false_type
	{};
	template <__iterator_decrementable_c _Iterator>
	struct is_decrementable<_Iterator> : std::true_type
	{};
	template <typename _Iterator>
	pf_decl_static pf_decl_constexpr bool is_decrementable_v
		= is_decrementable<_Iterator>::value;

	/// ITERATOR: Concept -> Sequence
	template<typename _Iterator>
	concept __iterator_sequence_c
		= (__iterator_base_c<_Iterator> || __iterator_base_const_c<_Iterator>)
			&& __iterator_incrementable_c<_Iterator>
			&& __iterator_decrementable_c<_Iterator>
			&& std::derived_from<typename _Iterator::category, iterator_sequence_tag_t>
			&& requires(const _Iterator __it)
		{
		{__it < std::declval<const typename _Iterator::value_t*>()}->std::same_as<bool>;
		{__it <= std::declval<const typename _Iterator::value_t*>()}->std::same_as<bool>;
		{__it > std::declval<const typename _Iterator::value_t*>()}->std::same_as<bool>;
		{__it >= std::declval<const typename _Iterator::value_t*>()}->std::same_as<bool>;
		};
	template<typename _Iterator>
	struct is_sequence_iterator : std::false_type
	{};
	template<__iterator_sequence_c _Iterator>
	struct is_sequence_iterator<_Iterator> : std::true_type
	{};
	template<typename _Iterator>
	pf_decl_static pf_decl_constexpr bool is_sequence_iterator_v
		= is_sequence_iterator<_Iterator>::value;

	/// ITERATOR: Concept -> Forward
	template<typename _Iterator>
	concept __iterator_forward_c
		= (__iterator_base_c<_Iterator> || __iterator_base_const_c<_Iterator>)
			&& __iterator_incrementable_c<_Iterator>
			&& std::equality_comparable<_Iterator>
			&& std::derived_from<typename _Iterator::category, iterator_forward_iterator_tag_t>;
	template<typename _Iterator>
	struct is_forward_iterator : std::false_type
	{};
	template<__iterator_forward_c _Iterator>
	struct is_forward_iterator<_Iterator> : std::true_type
	{};
	template<typename _Iterator>
	pf_decl_static pf_decl_constexpr bool is_forward_iterator_v
		= is_forward_iterator<_Iterator>::value;


	/// ITERATOR: Concept -> Random Access
	template<typename _Iterator>
	concept __iterator_random_access_c
		= __iterator_decrementable_c<_Iterator>
			&& __iterator_forward_c<_Iterator>
			&& std::derived_from<typename _Iterator::category, iterator_random_access_iterator_tag_t>;
	template<typename _Iterator>
	struct is_random_access_iterator : std::false_type
	{};
	template<__iterator_random_access_c _Iterator>
	struct is_random_access_iterator<_Iterator> : std::true_type
	{};
	template<typename _Iterator>
	pf_decl_static pf_decl_constexpr bool is_random_access_iterator_v
		= is_random_access_iterator<_Iterator>::value;

	/// ITERATOR: Concept
	template<typename _Iterator>
	concept __iterator_c
		= is_sequence_iterator_v<_Iterator>
			|| is_forward_iterator_v<_Iterator>
			|| is_random_access_iterator_v<_Iterator>;
	template<typename _Iterator>
	struct is_iterator : std::false_type
	{};
	template<__iterator_c _Iterator>
	struct is_iterator<_Iterator> : std::true_type
	{};
	template<typename _Iterator>
	pf_decl_static pf_decl_constexpr bool is_iterator_v
		= is_iterator<_Iterator>::value;

	/// ITERATOR: Operator +/-
	template <typename _Iterator>
	pf_hint_nodiscard pf_decl_constexpr _Iterator
	operator+(_Iterator __it, diff_t __i) pf_attr_noexcept
	requires(is_incrementable_v<_Iterator>)
	{
		return __it += __i;
	}
	template<typename _Iterator>
	pf_hint_nodiscard pf_decl_constexpr _Iterator
	operator-(_Iterator __it, diff_t __i) pf_attr_noexcept
	requires(is_decrementable_v<_Iterator>)
	{
		return __it -= __i;
	}

	/// ITERATOR: Next/Prev
	template <typename _Iterator>
	pf_hint_nodiscard pf_decl_constexpr _Iterator
	nextof(
		_Iterator __it) pf_attr_noexcept
	requires(is_incrementable_v<_Iterator>)
	{
		return ++__it;
	}
	template <typename _Iterator>
	pf_hint_nodiscard pf_decl_constexpr _Iterator
	prevof(
		_Iterator __it) pf_attr_noexcept
	requires(is_decrementable_v<_Iterator>)
	{
		return --__it;
	}


	/// ITERATOR: Reverse
	template <typename _Iterator>
	class reverse_iterator
	{};

	template <typename _Iterator>
	requires(is_decrementable_v<_Iterator>
					 && !is_const_iterator_v<_Iterator>)
	class reverse_iterator<_Iterator>
	{
	public:
		using value_t	 = typename _Iterator::value_t;
		using category = typename _Iterator::category;

		/// Constructors
		template <typename ... _Args>
		pf_decl_constexpr reverse_iterator(
			_Args ... __args) pf_attr_noexcept
		requires(std::is_constructible_v<_Iterator, _Args...>)
			: it_(__args ...)
		{}

		/// Destructor
		pf_decl_constexpr ~reverse_iterator() = default;

		/// Get
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr value_t*
		get() pf_attr_noexcept
		{
			return this->it_.get();
		}
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const value_t*
		get() const pf_attr_noexcept
		{
			return this->it_.get();
		}

		/// Operator =
		pf_decl_constexpr reverse_iterator<_Iterator>&
		operator=(
			reverse_iterator<_Iterator> const &__r) pf_attr_noexcept = default;

		/// Operator +=
		pf_decl_constexpr reverse_iterator<_Iterator>&
		operator +=(
			diff_t __i) pf_attr_noexcept
		{
			this->it_ -= __i;
			return *this;
		}

		/// Operator ++
		pf_decl_constexpr reverse_iterator<_Iterator>
		operator++(int32_t) pf_attr_noexcept
		{
			reverse_iterator<_Iterator> p = this->it_--;
			return p;
		}
		pf_decl_constexpr reverse_iterator<_Iterator>&
		operator++() pf_attr_noexcept
		{
			--this->it_;
			return *this;
		}

		/// Operator -=
		pf_decl_constexpr reverse_iterator<_Iterator>&
		operator -=(
			diff_t __i) pf_attr_noexcept
		{
			this->it_ += __i;
			return *this;
		}

		/// Operator --
		pf_decl_constexpr reverse_iterator<_Iterator>
		operator--(int32_t) pf_attr_noexcept
		{
			reverse_iterator<_Iterator> p = this->it_++;
			return p;
		}
		pf_decl_constexpr reverse_iterator<_Iterator>&
		operator--() pf_attr_noexcept
		{
			++this->it_;
			return *this;
		}

		/// Operator *
		pf_hint_nodiscard pf_decl_constexpr value_t&
		operator*() pf_attr_noexcept
		{
			return *this->it_;
		}
		pf_hint_nodiscard pf_decl_constexpr const value_t&
		operator*() const pf_attr_noexcept
		{
			return *this->it_;
		}

		/// Operator ->
		pf_hint_nodiscard pf_decl_constexpr value_t*
		operator->() pf_attr_noexcept
		{
			return this->it_.operator->();
		}
		pf_hint_nodiscard pf_decl_constexpr const value_t*
		operator->() const pf_attr_noexcept
		{
			return this->it_.operator->();
		}

		/// Operator (bool)
		pf_hint_nodiscard pf_decl_explicit pf_decl_constexpr
		operator bool() const pf_attr_noexcept
		{
			return this->it_ != nullptr;
		}

		/// Operator (value_t)
		pf_hint_nodiscard pf_decl_constexpr
		operator value_t* () pf_attr_noexcept
		{
			return this->get();
		}
		pf_hint_nodiscard pf_decl_constexpr
		operator const value_t* () const pf_attr_noexcept
		{
			return this->get();
		}

	private:
		_Iterator it_;
	};

	template <typename _Iterator>
	requires(is_decrementable_v<_Iterator>	// && is_incrementable_v<_Iterator>
					 && is_const_iterator_v<_Iterator>)
	class reverse_iterator<_Iterator>
	{
	public:
		using value_t	 = typename _Iterator::value_t;
		using category = typename _Iterator::category;

		/// Constructors
		template <typename ... _Args>
		pf_decl_constexpr reverse_iterator(
			_Args ... __args) pf_attr_noexcept
		requires(std::is_constructible_v<_Iterator, _Args...>)
			: it_(__args ...)
		{}

		/// Destructor
		pf_decl_constexpr ~reverse_iterator() = default;

		/// Get
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr value_t*
		get() const pf_attr_noexcept
		{
			return this->ptr_;
		}

		/// Operator =
		pf_decl_constexpr reverse_iterator<_Iterator> &
		operator=(
			reverse_iterator<_Iterator> const &__r) pf_attr_noexcept = default;

		/// Operator +=
		pf_decl_constexpr reverse_iterator<_Iterator> &
		operator +=(
			diff_t __i) pf_attr_noexcept
		{
			this->it_ -= __i;
			return *this;
		}

		/// Operator ++
		pf_decl_constexpr reverse_iterator<_Iterator>
		operator++(int32_t) pf_attr_noexcept
		{
			reverse_iterator<_Iterator> p = this->it_--;
			return p;
		}
		pf_decl_constexpr reverse_iterator<_Iterator>&
		operator++() pf_attr_noexcept
		{
			--this->it_;
			return *this;
		}

		/// Operator -=
		pf_decl_constexpr reverse_iterator<_Iterator> &
		operator -=(
			diff_t __i) pf_attr_noexcept
		{
			this->it_ += __i;
			return *this;
		}

		/// Operator --
		pf_decl_constexpr reverse_iterator<_Iterator>
		operator--(int32_t) pf_attr_noexcept
		{
			reverse_iterator<_Iterator> p = this->it_++;
			return p;
		}
		pf_decl_constexpr reverse_iterator<_Iterator>&
		operator--() pf_attr_noexcept
		{
			++this->it_;
			return *this;
		}

		/// Operator *
		pf_hint_nodiscard pf_decl_constexpr value_t&
		operator*() const pf_attr_noexcept
		{
			return *this->it_;
		}

		/// Operator ->
		pf_hint_nodiscard pf_decl_constexpr value_t*
		operator->() const pf_attr_noexcept
		{
			return this->it_.operator->();
		}

		/// Operator (bool)
		pf_hint_nodiscard pf_decl_explicit pf_decl_constexpr
		operator bool() const pf_attr_noexcept
		{
			return this->it_ != nullptr;
		}

		/// Operator (value_t)
		pf_hint_nodiscard pf_decl_constexpr
		operator const value_t* () const pf_attr_noexcept
		{
			return this->get();
		}

	private:
		_Iterator it_;
	};

	/// ITERATOR: Is Reverse Iterator
	template <typename _Iterator>
	struct is_reverse_iterator : std::false_type
	{};
	template <typename _Iterator>
	struct is_reverse_iterator<reverse_iterator<_Iterator>> : std::true_type
	{};
	template <typename _Iterator>
	pf_decl_static pf_decl_constexpr bool is_reverse_iterator_v
		= is_reverse_iterator<_Iterator>::value;


	/// ITERATOR: Mover
	template <typename _Iterator>
	class move_iterator
	{};

	template <typename _Iterator>
	requires(!is_const_iterator_v<_Iterator>)
	class move_iterator<_Iterator>
	{
	public:
		using value_t	 = typename _Iterator::value_t;
		using category = typename _Iterator::category;

		/// Constructors
		pf_decl_constexpr move_iterator(
			_Iterator __it) pf_attr_noexcept
			: it_(__it)
		{}
		pf_decl_constexpr move_iterator(
			move_iterator<_Iterator> const &__r) pf_attr_noexcept = default;

		/// Destructor
		~move_iterator() = default;

		/// Get
		pf_hint_nodiscard pf_decl_constexpr value_t* get() pf_attr_noexcept
		{
			return this->it_.get();
		}
		pf_hint_nodiscard pf_decl_constexpr const value_t* get() const pf_attr_noexcept
		{
			return this->it_.get();
		}

		/// Operator=
		pf_decl_constexpr move_iterator<_Iterator> &
		operator =(
			_Iterator const &__r) pf_attr_noexcept
		{
			this->it_ = __r;
			return *this;
		}

		/// Operator+=
		pf_decl_constexpr move_iterator<_Iterator> &
		operator +=(
			diff_t __i) pf_attr_noexcept
		{
			this->it_ += __i;
			return *this;
		}

		/// Operator++
		pf_decl_constexpr move_iterator<_Iterator>
		operator ++(
			int32_t) pf_attr_noexcept
		{
			_Iterator p = this->it_++;
			return p;
		}
		pf_decl_constexpr move_iterator<_Iterator> &
		operator ++() pf_attr_noexcept
		{
			++this->it_;
			return *this;
		}

		/// Operator-=
		pf_decl_constexpr move_iterator<_Iterator> &
		operator -=(
			diff_t __i) pf_attr_noexcept
		{
			this->it_ -= __i;
			return *this;
		}

		/// Operator--
		pf_decl_constexpr move_iterator<_Iterator>
		operator --(
			int32_t) pf_attr_noexcept
		{
			_Iterator p = this->it_--;
			return p;
		}
		pf_decl_constexpr move_iterator<_Iterator> &
		operator --() pf_attr_noexcept
		{
			--this->it_;
			return *this;
		}

		/// Operator*
		pf_hint_nodiscard pf_decl_constexpr value_t &
		operator*() pf_attr_noexcept
		{
			return std::move(*this->it_);
		}
		pf_hint_nodiscard pf_decl_constexpr const value_t &
		operator *() const pf_attr_noexcept
		{
			return std::move(*this->it_);
		}

		/// Operator->
		pf_hint_nodiscard pf_decl_constexpr value_t*
		operator ->() pf_attr_noexcept
		{
			return this->it_.get();
		}
		pf_hint_nodiscard pf_decl_constexpr const value_t*
		operator ->() const pf_attr_noexcept
		{
			return this->it_.get();
		}

		/// Operator <=>
		pf_hint_nodiscard pf_decl_constexpr bool
		operator ==(
			const value_t *__ptr) const pf_attr_noexcept
		{
			return this->it_ == __ptr;
		}
		pf_hint_nodiscard pf_decl_constexpr bool
		operator !=(
			const value_t *__ptr) const pf_attr_noexcept
		{
			return this->it_ != __ptr;
		}
		pf_hint_nodiscard pf_decl_constexpr std::strong_ordering
		operator <=>(
			const value_t *__ptr) const pf_attr_noexcept
		{
			return this->it_ <=> __ptr;
		}

		/// Operator (bool)
		pf_hint_nodiscard pf_decl_explicit pf_decl_constexpr
		operator bool() const pf_attr_noexcept
		{
			return this->it_ != nullptr;
		}

		/// Operator (value_t)
		pf_hint_nodiscard pf_decl_constexpr
		operator const value_t* () const pf_attr_noexcept
		{
			return this->get();
		}

	private:
		_Iterator it_;
	};

	template <typename _Iterator>
	requires(is_const_iterator_v<_Iterator>)
	class move_iterator<_Iterator>
	{
	public:
		using value_t	 = typename _Iterator::value_t;
		using category = typename _Iterator::category;

		/// Constructors
		pf_decl_constexpr move_iterator(
			_Iterator __it) pf_attr_noexcept
			: it_(__it)
		{}
		pf_decl_constexpr move_iterator(
			move_iterator<_Iterator> const &__r) pf_attr_noexcept
			: it_(__r.it_)
		{}

		/// Destructor
		~move_iterator() = default;

		/// Get
		pf_hint_nodiscard pf_decl_constexpr value_t* get() const pf_attr_noexcept
		{
			return this->it_.get();
		}

		/// Operator=
		pf_decl_constexpr move_iterator<_Iterator> &
		operator =(
			_Iterator const &__r) pf_attr_noexcept
		{
			this->it_ = __r;
			return *this;
		}

		/// Operator+=
		pf_decl_constexpr move_iterator<_Iterator> &
		operator +=(
			diff_t __i) pf_attr_noexcept
		{
			this->it_ += __i;
			return *this;
		}

		/// Operator++
		pf_decl_constexpr move_iterator<_Iterator>
		operator ++(
			int32_t) pf_attr_noexcept
		{
			_Iterator p = this->it_++;
			return p;
		}
		pf_decl_constexpr move_iterator<_Iterator> &
		operator ++() pf_attr_noexcept
		{
			++this->it_;
			return *this;
		}

		/// Operator-=
		pf_decl_constexpr move_iterator<_Iterator> &
		operator -=(
			diff_t __i) pf_attr_noexcept
		{
			this->it_ -= __i;
			return *this;
		}

		/// Operator--
		pf_decl_constexpr move_iterator<_Iterator>
		operator --(
			int32_t) pf_attr_noexcept
		{
			_Iterator p = this->it_--;
			return p;
		}
		pf_decl_constexpr move_iterator<_Iterator> &
		operator --() pf_attr_noexcept
		{
			--this->it_;
			return *this;
		}

		/// Operator*
		pf_hint_nodiscard pf_decl_constexpr const value_t &&
		operator*() const pf_attr_noexcept
		{
			return std::move(*this->it_);
		}

		/// Operator->
		pf_hint_nodiscard pf_decl_constexpr const value_t*
		operator ->() const pf_attr_noexcept
		{
			return this->it_.get();
		}

		/// Operator <=>
		pf_hint_nodiscard pf_decl_constexpr bool
		operator ==(
			const value_t *__ptr) const pf_attr_noexcept
		{
			return this->it_ == __ptr;
		}
		pf_hint_nodiscard pf_decl_constexpr bool
		operator !=(
			const value_t *__ptr) const pf_attr_noexcept
		{
			return this->it_ != __ptr;
		}
		pf_hint_nodiscard pf_decl_constexpr std::strong_ordering
		operator <=>(
			const value_t *__ptr) const pf_attr_noexcept
		{
			return this->it_ <=> __ptr;
		}

		/// Operator (bool)
		pf_hint_nodiscard pf_decl_explicit pf_decl_constexpr
		operator bool() const pf_attr_noexcept
		{
			return this->it_ != nullptr;
		}

		/// Operator (value_t)
		pf_hint_nodiscard pf_decl_constexpr
		operator const value_t* () const pf_attr_noexcept
		{
			return this->get();
		}

	private:
		_Iterator it_;
	};

	/// SEQUENCE: Iterator
	template<typename _Ty>
	class iterator
	{
	public:
		using value_t	 = _Ty;
		using category = iterator_sequence_tag_t;

		/// Constructors
		pf_decl_constexpr
		iterator(_Ty *__ptr = nullptr) pf_attr_noexcept
			: ptr_(__ptr)
		{}
		pf_decl_constexpr
		iterator(iterator<_Ty> const &__it) pf_attr_noexcept
			: iterator(__it.ptr_)
		{}

		/// Destructor
		~iterator() = default;

		/// Get
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr value_t*
		get() pf_attr_noexcept
		{
			return this->ptr_;
		}
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const value_t*
		get() const pf_attr_noexcept
		{
			return this->ptr_;
		}

		/// Operator =
		pf_decl_constexpr iterator<_Ty> &
		operator=(iterator<_Ty> const &__r) pf_attr_noexcept = default;

		/// Operator +=
		pf_decl_constexpr iterator<_Ty> &
		operator+=(diff_t __i) pf_attr_noexcept
		{
			this->ptr_ += __i;
			return *this;
		}

		/// Operator ++
		pf_decl_constexpr iterator<_Ty>
		operator++(int32_t) pf_attr_noexcept
		{
			iterator<_Ty> p = this->ptr_++;
			return p;
		}
		pf_decl_constexpr iterator<_Ty> &
		operator++() pf_attr_noexcept
		{
			++this->ptr_;
			return *this;
		}

		/// Operator -=
		pf_decl_constexpr iterator<_Ty> &
		operator-=(diff_t __i) pf_attr_noexcept
		{
			this->ptr_ -= __i;
			return *this;
		}

		/// Operator --
		pf_decl_constexpr iterator<_Ty>
		operator--(int32_t) pf_attr_noexcept
		{
			iterator<_Ty> p = this->ptr_--;
			return p;
		}
		pf_decl_constexpr iterator<_Ty> &
		operator--() pf_attr_noexcept
		{
			--this->ptr_;
			return *this;
		}

		/// Operator *
		pf_hint_nodiscard pf_decl_constexpr value_t &
		operator*() pf_attr_noexcept
		{
			return *this->ptr_;
		}
		pf_hint_nodiscard pf_decl_constexpr const value_t &
		operator*() const pf_attr_noexcept
		{
			return *this->ptr_;
		}

		/// Operator ->
		pf_hint_nodiscard pf_decl_constexpr value_t*
		operator->() pf_attr_noexcept
		{
			return this->ptr_;
		}
		pf_hint_nodiscard pf_decl_constexpr const value_t*
		operator->() const pf_attr_noexcept
		{
			return this->ptr_;
		}

		/// Operator (bool)
		pf_hint_nodiscard pf_decl_explicit pf_decl_constexpr
		operator bool() const pf_attr_noexcept
		{
			return this->ptr_ != nullptr;
		}

		/// Operator (value_t*)
		pf_hint_nodiscard pf_decl_constexpr
		operator value_t* () pf_attr_noexcept
		{
			return this->ptr_;
		}
		pf_hint_nodiscard pf_decl_constexpr
		operator const value_t* () const pf_attr_noexcept
		{
			return this->ptr_;
		}

	private:
		value_t *ptr_;
	};

	/// SEQUENCE: Iterator Iterator -> CTAD
	template<typename _Ty>
	iterator(_Ty*)->iterator<_Ty>;


	/// SEQUENCE: Iterator Const
	template<typename _Ty>
	class iterator<const _Ty>
	{
	pf_decl_friend iterator<_Ty>;

	public:
		using value_t	 = const _Ty;
		using category = iterator_sequence_tag_t;

		/// Constructors
		pf_decl_constexpr iterator(
			const _Ty *__ptr) pf_attr_noexcept
			: ptr_(__ptr)
		{}
		pf_decl_constexpr iterator(
			const iterator<const _Ty> &__r) pf_attr_noexcept
			: iterator(__r.ptr_)
		{}

		/// Destructor
		pf_decl_constexpr ~iterator() = default;

		/// Get
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const value_t*
		get() const pf_attr_noexcept
		{
			return this->ptr_;
		}

		/// Operator =
		pf_decl_constexpr iterator<const _Ty>&
		operator=(
			iterator<const _Ty> const &__r) pf_attr_noexcept = default;

		/// Operator +=
		pf_decl_constexpr iterator<const _Ty>&
		operator+=(
			diff_t __i) pf_attr_noexcept
		{
			this->ptr_ += __i;
			return *this;
		}

		/// Operator ++
		pf_decl_constexpr iterator<const _Ty>
		operator++(int32_t) pf_attr_noexcept
		{
			iterator<const _Ty> p = this->ptr_++;
			return p;
		}
		pf_decl_constexpr iterator<const _Ty>&
		operator++() pf_attr_noexcept
		{
			++this->ptr_;
			return *this;
		}

		/// Operator -=
		pf_decl_constexpr iterator<const _Ty>&
		operator-=(diff_t __i) pf_attr_noexcept
		{
			this->ptr_ -= __i;
			return *this;
		}

		/// Operator --
		pf_decl_constexpr iterator<const _Ty>
		operator--(int32_t) pf_attr_noexcept
		{
			iterator<_Ty> p = this->ptr_--;
			return p;
		}
		pf_decl_constexpr iterator<const _Ty>&
		operator--() pf_attr_noexcept
		{
			--this->ptr_;
			return *this;
		}

		/// Operator *
		pf_hint_nodiscard pf_decl_constexpr const value_t&
		operator*() const pf_attr_noexcept
		{
			return *this->ptr_;
		}

		/// Operator ->
		pf_hint_nodiscard pf_decl_constexpr const value_t*
		operator->() const pf_attr_noexcept
		{
			return this->ptr_;
		}

		/// Operator (bool)
		pf_hint_nodiscard pf_decl_explicit pf_decl_constexpr
		operator bool() const pf_attr_noexcept
		{
			return this->ptr_ != nullptr;
		}

		/// Operator (const value_t *)
		pf_hint_nodiscard pf_decl_constexpr
		operator const value_t* () const pf_attr_noexcept
		{
			return this->ptr_;
		}

	private:
		const value_t *ptr_;
	};

	/// SEQUENCE: Const Iterator -> Alias
	template<typename _Ty>
	using const_iterator = iterator<const _Ty>;

	/// ITERABLE: Concept -> Iterable
	template <typename _Iterable>
	concept __const_iterable_c
		= requires(_Iterable const & __cia)
		{
		typename _Iterable::const_iterator_t;
		{ __cia.begin() }->std::same_as<typename _Iterable::const_iterator_t>;
		{ __cia.end()}->std::same_as<typename _Iterable::const_iterator_t>;
		};
	template <typename _Iterable>
	struct is_const_iterable : std::false_type
	{};
	template <__const_iterable_c _Iterable>
	struct is_const_iterable<_Iterable> : std::true_type
	{};
	template <typename _Iterable>
	pf_decl_static pf_decl_constexpr bool is_const_iterable_v
		= is_const_iterable<_Iterable>::value;

	template <typename _Iterable>
	concept __iterable_c
		= requires(_Iterable & __ia)
		{
		typename _Iterable::iterator_t;
		{ __ia.begin() }->std::same_as<typename _Iterable::iterator_t>;
		{ __ia.end() }->std::same_as<typename _Iterable::iterator_t>;
		};
	template <typename _Iterable>
	struct is_iterable : std::false_type
	{};
	template <__iterable_c _Iterable>
	struct is_iterable<_Iterable> : std::true_type
	{};
	template <typename _Iterable>
	pf_decl_static pf_decl_constexpr bool is_iterable_v
		= is_iterable<_Iterable>::value;

	template <typename _Iterable>
	concept __reverse_const_iterable_c
		= requires(_Iterable const & __ia)
		{
		typename _Iterable::const_reverse_iterator_t;
		{ __ia.rbegin() }->std::same_as<typename _Iterable::const_reverse_iterator_t>;
		{ __ia.rend() }->std::same_as<typename _Iterable::const_reverse_iterator_t>;
		};
	template <typename _Iterable>
	struct is_const_reverse_iterable : std::false_type
	{};
	template <__reverse_const_iterable_c _Iterable>
	struct is_const_reverse_iterable<_Iterable> : std::true_type
	{};
	template <typename _Iterable>
	pf_decl_static pf_decl_constexpr bool is_const_reverse_iterable_v
		= is_const_reverse_iterable<_Iterable>::value;

	template <typename _Iterable>
	concept __reverse_iterable_c
		= requires(_Iterable & __ia)
		{
		typename _Iterable::reverse_iterator_t;
		{ __ia.rbegin() }->std::same_as<typename _Iterable::reverse_iterator_t>;
		{ __ia.rend() }->std::same_as<typename _Iterable::reverse_iterator_t>;
		};
	template <typename _Iterable>
	struct is_reverse_iterable : std::false_type
	{};
	template <__reverse_iterable_c _Iterable>
	struct is_reverse_iterable<_Iterable> : std::true_type
	{};
	template <typename _Iterable>
	pf_decl_static pf_decl_constexpr bool is_reverse_iterable_v
		= is_reverse_iterable<_Iterable>::value;

	/// ITERABLE: Concept -> Mappable
	template <typename _Iterable>
	concept __iterable_mappable_c
		= is_iterable_v<_Iterable>
			&& requires(_Iterable & __ia)
		{
		{ __ia.data() }->std::same_as<typename _Iterable::value_t*>;
		{ __ia.data() }->std::same_as<const typename _Iterable::value_t*>;
		};
	template <typename _Iterable>
	struct is_mappable : std::false_type
	{};
	template <__iterable_mappable_c _Iterable>
	struct is_mappable<_Iterable> : std::true_type
	{};
	template <typename _Iterable>
	pf_decl_static pf_decl_constexpr bool is_mappable_v
		= is_mappable<_Iterable>::value;

	/// ITERABLE: Concept -> Swappable
	template <typename _Iterable>
	concept __iterable_swappable_c
		= is_iterable_v<_Iterable>
			&& requires(_Iterable & __ia)
		{
		__ia.swap(std::declval<_Iterable>());
		};
	template <typename _Iterable>
	struct is_swappable : std::false_type
	{};
	template <__iterable_swappable_c _Iterable>
	struct is_swappable<_Iterable> : std::true_type
	{};
	template <typename _Iterable>
	pf_decl_static pf_decl_constexpr bool is_swappable_v
		= is_swappable<_Iterable>::value;

	/// ITERABLE: Concept -> Insertable
	template <typename _Iterable>
	concept __iterable_front_insertable_c
		= is_iterable_v<_Iterable>
			&& requires(_Iterable & __ia)
		{
		__ia.insert_front(
			std::declval<typename _Iterable::value_t>());
		};
	template <typename _Iterable>
	struct is_front_insertable : std::false_type
	{};
	template <__iterable_front_insertable_c _Iterable>
	struct is_front_insertable<_Iterable> : std::true_type
	{};
	template <typename _Iterable>
	pf_decl_static pf_decl_constexpr bool is_front_insertable_v
		= is_front_insertable<_Iterable>::value;
	template <typename _Iterable>
	concept __iterable_back_insertable_c
		= is_iterable_v<_Iterable>
			&& requires(_Iterable & __ia)
		{
		__ia.insert_back(
			std::declval<typename _Iterable::value_t>());
		};
	template <typename _Iterable>
	struct is_back_insertable : std::false_type
	{};
	template <__iterable_back_insertable_c _Iterable>
	struct is_back_insertable<_Iterable> : std::true_type
	{};
	template <typename _Iterable>
	pf_decl_static pf_decl_constexpr bool is_back_insertable_v
		= is_back_insertable<_Iterable>::value;
	template <typename _Iterable>
	concept __iterable_placement_insertable_c
		= is_iterable_v<_Iterable>
			&& requires(_Iterable & __ia)
		{
		__ia.insert(
			std::declval<typename _Iterable::iterator_t>(),
			std::declval<typename _Iterable::value_t>());
		};
	template <typename _Iterable>
	struct is_placement_insertable : std::false_type
	{};
	template <__iterable_placement_insertable_c _Iterable>
	struct is_placement_insertable<_Iterable> : std::true_type
	{};
	template <typename _Iterable>
	pf_decl_static pf_decl_constexpr bool is_placement_insertable_v
		= is_placement_insertable<_Iterable>::value;

	template <typename _Iterable>
	pf_decl_static pf_decl_constexpr bool is_insertable_v
		= is_back_insertable_v<_Iterable>
			|| is_front_insertable_v<_Iterable>
			|| is_placement_insertable_v<_Iterable>;

	/// ITERABLE: Inserter -> Front
	template <typename _Iterable>
	class __front_insert_iterator_assigner
	{
	public:
		using value_t = typename _Iterable::value_t;

		/// Constructors
		__front_insert_iterator_assigner() = default;
		__front_insert_iterator_assigner(
			__front_insert_iterator_assigner<_Iterable> const &) = delete;

		/// Operator =
		__front_insert_iterator_assigner<_Iterable> &
		operator=(
			__front_insert_iterator_assigner<_Iterable> const &) = delete;
		pf_decl_constexpr __front_insert_iterator_assigner<_Iterable> &
		operator=(
			typename _Iterable::value_t &&__val)
		{
			this->ia_->insert_front(__val);
		}
		template <typename _IterableR>
		pf_decl_constexpr __front_insert_iterator_assigner<_Iterable> &
		operator=(
			_IterableR &&__ia)
		requires(is_iterable_v<_IterableR>)
		{
			this->ia_->insert_front(__ia.begin(), __ia.end());
		}

	private:
		_Iterable *ia_;
	};
	template <typename _Iterable>
	requires(is_front_insertable_v<_Iterable>)
	class front_insert_iterator
	{
	public:
		using value_t	 = typename _Iterable::value_t;
		using category = typename _Iterable::iterator_t::category;

		/// Constructors
		pf_decl_constexpr front_insert_iterator(
			_Iterable *__ia)
			: ia_(__ia)
		{}

		/// Destructor
		pf_decl_constexpr ~front_insert_iterator() pf_attr_noexcept = default;

		/// Get
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const value_t*
		get() const pf_attr_noexcept
		{
			return this->ia_->begin();
		}

		/// Operator =
		pf_decl_constexpr front_insert_iterator<_Iterable>&
		operator=(
			front_insert_iterator<_Iterable> const &__r) pf_attr_noexcept = default;

		/// Operator +=
		pf_decl_constexpr front_insert_iterator<_Iterable>&
		operator+=(
			diff_t) pf_attr_noexcept
		{
			return *this;
		}

		/// Operator ++
		pf_decl_constexpr front_insert_iterator<_Iterable>
		operator++(int32_t) pf_attr_noexcept
		{
			return *this;
		}
		pf_decl_constexpr front_insert_iterator<_Iterable> &
		operator++() pf_attr_noexcept
		{
			return *this;
		}

		/// Operator -=
		pf_decl_constexpr front_insert_iterator<_Iterable>&
		operator-=(
			diff_t) pf_attr_noexcept
		{
			return *this;
		}

		/// Operator --
		pf_decl_constexpr front_insert_iterator<_Iterable>
		operator--(int32_t) pf_attr_noexcept
		{
			return *this;
		}
		pf_decl_constexpr front_insert_iterator<_Iterable> &
		operator--() pf_attr_noexcept
		{
			return *this;
		}

		/// Operator *
		pf_hint_nodiscard pf_decl_constexpr
		operator*() const pf_attr_noexcept
		{
			return __front_insert_iterator_assigner(this->ia_);
		}

		/// Operator ->
		pf_hint_nodiscard pf_decl_constexpr const value_t*
		operator->() const pf_attr_noexcept
		{
			return this->ia_.begin();
		}

		/// Operator (bool)
		pf_hint_nodiscard pf_decl_explicit pf_decl_constexpr
		operator bool() const pf_attr_noexcept
		{
			return this->ia_.begin() != nullptr;
		}

		/// Operator (const value_t *)
		pf_hint_nodiscard pf_decl_constexpr
		operator const value_t* () const pf_attr_noexcept
		{
			return this->ia_.begin().get();
		}

	private:
		_Iterable *ia_;
	};

	template <typename _Iterable>
	pf_hint_nodiscard pf_decl_constexpr front_insert_iterator<_Iterable>
	make_front_inserter(
		_Iterable &__ct) pf_attr_noexcept
	requires(is_front_insertable_v<_Iterable>)
	{
		return front_insert_iterator<_Iterable>(&__ct);
	}


	/// ITERABLE: Inserter -> Back
	template <typename _Iterable>
	class __back_insert_iterator_assigner
	{
	public:
		using value_t = typename _Iterable::value_t;

		/// Constructors
		__back_insert_iterator_assigner() = default;
		__back_insert_iterator_assigner(
			__back_insert_iterator_assigner<_Iterable> const &) = delete;

		/// Operator =
		__back_insert_iterator_assigner<_Iterable> &
		operator=(
			__back_insert_iterator_assigner<_Iterable> const &) = delete;
		pf_decl_constexpr __back_insert_iterator_assigner<_Iterable>&
		operator =(
			typename _Iterable::value_t &&__val)
		{
			this->ia_->insert_back(__val);
		}
		template <typename _IterableR>
		pf_decl_constexpr __back_insert_iterator_assigner<_Iterable>&
		operator =(
			_IterableR &&__ia)
		requires(is_iterable_v<_IterableR>)
		{
			this->ia_->insert_back(__ia.begin(), __ia.end());
		}

	private:
		_Iterable *ia_;
	};

	template <typename _Iterable>
	requires(is_back_insertable_v<_Iterable>)
	class back_insert_iterator
	{
	public:
		using value_t	 = typename _Iterable::value_t;
		using category = typename _Iterable::iterator_t::category;

		/// Constructors
		pf_decl_constexpr back_insert_iterator(
			_Iterable *__ia)
			: ia_(__ia)
		{}

		/// Destructor
		pf_decl_constexpr ~back_insert_iterator() pf_attr_noexcept = default;

		/// Get
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const value_t*
		get() const pf_attr_noexcept
		{
			return this->ia_->end();
		}

		/// Operator =
		pf_decl_constexpr back_insert_iterator<_Iterable>&
		operator=(
			back_insert_iterator<_Iterable> const &__r) pf_attr_noexcept = default;

		/// Operator +=
		pf_decl_constexpr back_insert_iterator<_Iterable>&
		operator+=(
			diff_t) pf_attr_noexcept
		{
			return *this;
		}

		/// Operator ++
		pf_decl_constexpr back_insert_iterator<_Iterable>
		operator++(int32_t) pf_attr_noexcept
		{
			return *this;
		}
		pf_decl_constexpr back_insert_iterator<_Iterable> &
		operator++() pf_attr_noexcept
		{
			return *this;
		}

		/// Operator -=
		pf_decl_constexpr back_insert_iterator<_Iterable>&
		operator-=(
			diff_t) pf_attr_noexcept
		{
			return *this;
		}

		/// Operator --
		pf_decl_constexpr back_insert_iterator<_Iterable>
		operator--(int32_t) pf_attr_noexcept
		{
			return *this;
		}
		pf_decl_constexpr back_insert_iterator<_Iterable> &
		operator--() pf_attr_noexcept
		{
			return *this;
		}

		/// Operator *
		pf_hint_nodiscard pf_decl_constexpr
		operator*() const pf_attr_noexcept
		{
			return __back_insert_iterator_assigner(this->ia_);
		}

		/// Operator ->
		pf_hint_nodiscard pf_decl_constexpr const value_t*
		operator->() const pf_attr_noexcept
		{
			return this->ia_.end();
		}

		/// Operator (bool)
		pf_hint_nodiscard pf_decl_explicit pf_decl_constexpr
		operator bool() const pf_attr_noexcept
		{
			return this->ia_.end() != nullptr;
		}

		/// Operator (const value_t *)
		pf_hint_nodiscard pf_decl_constexpr
		operator const value_t* () const pf_attr_noexcept
		{
			return this->ia_.end().get();
		}

	private:
		_Iterable *ia_;
	};

	template <typename _Iterable>
	pf_hint_nodiscard pf_decl_constexpr back_insert_iterator<_Iterable>
	make_back_inserter(
		_Iterable &__ct) pf_attr_noexcept
	requires(is_back_insertable_v<_Iterable>)
	{
		return back_insert_iterator<_Iterable>(&__ct);
	}

	/// ITERABLE: Inserter
	template <typename _Iterable>
	class __insert_iterator_assigner
	{
	public:
		using value_t		 = typename _Iterable::value_t;
		using iterator_t = typename _Iterable::iterator_t;

		/// Constructors
		__insert_iterator_assigner() = default;
		__insert_iterator_assigner(
			__insert_iterator_assigner<_Iterable> const &) = delete;

		/// Operator =
		__insert_iterator_assigner<_Iterable> &
		operator=(
			__insert_iterator_assigner<_Iterable> const &) = delete;
		pf_decl_constexpr __insert_iterator_assigner<_Iterable> &
		operator =(
			typename _Iterable::value_t &&__val)
		{
			*this->it_ = this->ia_->insert(*this->it_, __val);
		}
		template <typename _IterableR>
		pf_decl_constexpr __insert_iterator_assigner<_Iterable> &
		operator =(
			_IterableR &&__ia)
		requires(is_iterable_v<_IterableR>)
		{
			*this->it_ = this->ia_->insert(*this->it_, __ia.begin(), __ia.end());
		}

		/// Operator (value_t)
		pf_hint_nodiscard pf_decl_constexpr
		operator value_t&() pf_attr_noexcept
		{
			return **this->it_;
		}

	private:
		_Iterable *ia_;
		iterator_t *it_;
	};
	template <typename _Iterable>
	requires(is_placement_insertable_v<_Iterable>)
	class insert_iterator
	{
	public:
		using value_t		 = typename _Iterable::value_t;
		using iterator_t = typename _Iterable::iterator_t;
		using category	 = typename _Iterable::iterator_t::category;

		/// Constructors
		pf_decl_constexpr insert_iterator(
			_Iterable *__ia,
			iterator_t __it) pf_attr_noexcept
			: ia_(__ia)
			, it_(__ia)
		{}

		/// Destructor
		pf_decl_constexpr ~insert_iterator() pf_attr_noexcept = default;

		/// Get
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const value_t*
		get() const pf_attr_noexcept
		{
			return this->it_.get();
		}

		/// Operator =
		pf_decl_constexpr insert_iterator<_Iterable>&
		operator=(
			insert_iterator<_Iterable> const &__r) pf_attr_noexcept = default;

		/// Operator +=
		pf_decl_constexpr back_insert_iterator<_Iterable>&
		operator+=(
			diff_t __i) pf_attr_noexcept
		{
			this->it_ += __i;
			return *this;
		}

		/// Operator ++
		pf_decl_constexpr back_insert_iterator<_Iterable>
		operator++(int32_t) pf_attr_noexcept
		{
			iterator_t p = this->it_++;
			return *this;
		}
		pf_decl_constexpr back_insert_iterator<_Iterable> &
		operator++() pf_attr_noexcept
		{
			++this->it_;
			return *this;
		}

		/// Operator -=
		pf_decl_constexpr back_insert_iterator<_Iterable>&
		operator-=(
			diff_t __i) pf_attr_noexcept
		{
			this->it_ -= __i;
			return *this;
		}

		/// Operator --
		pf_decl_constexpr back_insert_iterator<_Iterable>
		operator--(int32_t) pf_attr_noexcept
		{
			iterator_t p = this->it_--;
			return *this;
		}
		pf_decl_constexpr back_insert_iterator<_Iterable> &
		operator--() pf_attr_noexcept
		{
			--this->it_;
			return *this;
		}

		/// Operator *
		pf_hint_nodiscard pf_decl_constexpr
		operator*() const pf_attr_noexcept
		{
			return __insert_iterator_assigner(this->ia_, &this->it_);
		}

		/// Operator ->
		pf_hint_nodiscard pf_decl_constexpr const value_t*
		operator->() const pf_attr_noexcept
		{
			return this->it_.get();
		}

		/// Operator (bool)
		pf_hint_nodiscard pf_decl_explicit pf_decl_constexpr
		operator bool() const pf_attr_noexcept
		{
			return this->it_ != nullptr;
		}

		/// Operator (const value_t *)
		pf_hint_nodiscard pf_decl_constexpr
		operator const value_t* () const pf_attr_noexcept
		{
			return this->it_.get();
		}

	private:
		_Iterable *ia_;
		iterator_t it_;
	};

	template <typename _Iterable>
	pf_hint_nodiscard pf_decl_constexpr insert_iterator<_Iterable>
	make_inserter(
		_Iterable &__ct,
		typename _Iterable::iterator_t __w) pf_attr_noexcept
	requires(is_placement_insertable_v<_Iterable>)
	{
		return insert_iterator<_Iterable>(&__ct, __w);
	}



	/// ITERABLE: Concept -> Replaceable
	template <typename _Iterable>
	concept __iterable_replaceable_front_c
		= is_iterable_v<_Iterable>
			&& requires(_Iterable & __ia)
		{
		__ia.replace_front(
			std::declval<typename _Iterable::value_t>());
		};
	template <typename _Iterable>
	struct is_front_replaceable : std::false_type
	{};
	template <__iterable_replaceable_front_c _Iterable>
	struct is_front_replaceable<_Iterable> : std::true_type
	{};
	template <typename _Iterable>
	pf_decl_static pf_decl_constexpr bool is_front_replaceable_v
		= is_front_replaceable<_Iterable>::value;

	template <typename _Iterable>
	concept __iterable_replaceable_back_c
		= is_iterable_v<_Iterable>
			&& requires(_Iterable & __ia)
		{
		__ia.replace_back(
			std::declval<typename _Iterable::value_t>());
		};
	template <typename _Iterable>
	struct is_back_replaceable : std::false_type
	{};
	template <__iterable_replaceable_front_c _Iterable>
	struct is_back_replaceable<_Iterable> : std::true_type
	{};
	template <typename _Iterable>
	pf_decl_static pf_decl_constexpr bool is_back_replaceable_v
		= is_back_replaceable<_Iterable>::value;

	template <typename _Iterable>
	concept __iterable_placement_replaceable_c
		= is_iterable_v<_Iterable>
			&& requires(_Iterable & __ia)
		{
		__ia.replace(
			std::declval<typename _Iterable::iterator_t>(),
			std::declval<typename _Iterable::value_t>());
		};
	template <typename _Iterable>
	struct is_placement_replaceable : std::false_type
	{};
	template <__iterable_replaceable_front_c _Iterable>
	struct is_placement_replaceable<_Iterable> : std::true_type
	{};
	template <typename _Iterable>
	pf_decl_static pf_decl_constexpr bool is_placement_replaceable_v
		= is_placement_replaceable<_Iterable>::value;


	/// ITERABLE: Replacer -> Front
	template <typename _Iterable>
	class __front_replace_iterator_assigner
	{
	public:
		using value_t	 = typename _Iterable::value_t;
		using category = typename _Iterable::iterator_t::category;

		/// Constructors
		__front_replace_iterator_assigner() = default;
		__front_replace_iterator_assigner(
			__front_replace_iterator_assigner<_Iterable> const &) = delete;

		/// Operator =
		__front_replace_iterator_assigner<_Iterable> &
		operator=(
			__front_replace_iterator_assigner<_Iterable> const &) = delete;
		pf_decl_constexpr __front_replace_iterator_assigner<_Iterable> &
		operator=(
			typename _Iterable::value_t &&__val)
		{
			this->ia_->replace_front(__val);
		}
		template <typename _IterableR>
		pf_decl_constexpr __front_replace_iterator_assigner<_Iterable> &
		operator=(
			_IterableR &&__ia)
		requires(is_iterable_v<_IterableR>)
		{
			this->ia_->replace_front(__ia.begin(), __ia.end());
		}

	private:
		_Iterable *ia_;
	};

	template <typename _Iterable>
	requires(is_front_replaceable_v<_Iterable>)
	class front_replace_iterator
	{
	public:
		using value_t	 = typename _Iterable::value_t;
		using category = typename _Iterable::iterator_t::category;

		/// Constructors
		pf_decl_constexpr front_replace_iterator(
			_Iterable *__ia)
			: ia_(__ia)
		{}

		/// Destructor
		pf_decl_constexpr ~front_replace_iterator() pf_attr_noexcept = default;

		/// Get
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const value_t*
		get() const pf_attr_noexcept
		{
			return this->ia_->begin();
		}

		/// Operator =
		pf_decl_constexpr front_replace_iterator<_Iterable>&
		operator=(
			front_replace_iterator<_Iterable> const &__r) pf_attr_noexcept = default;

		/// Operator +=
		pf_decl_constexpr front_replace_iterator<_Iterable>&
		operator+=(
			diff_t) pf_attr_noexcept
		{
			return *this;
		}

		/// Operator ++
		pf_decl_constexpr front_replace_iterator<_Iterable>
		operator++(int32_t) pf_attr_noexcept
		{
			return *this;
		}
		pf_decl_constexpr front_replace_iterator<_Iterable> &
		operator++() pf_attr_noexcept
		{
			return *this;
		}

		/// Operator -=
		pf_decl_constexpr front_replace_iterator<_Iterable>&
		operator-=(
			diff_t) pf_attr_noexcept
		{
			return *this;
		}

		/// Operator --
		pf_decl_constexpr front_replace_iterator<_Iterable>
		operator--(int32_t) pf_attr_noexcept
		{
			return *this;
		}
		pf_decl_constexpr front_replace_iterator<_Iterable> &
		operator--() pf_attr_noexcept
		{
			return *this;
		}

		/// Operator *
		pf_hint_nodiscard pf_decl_constexpr
		operator*() const pf_attr_noexcept
		{
			return __front_replace_iterator_assigner(this->ia_);
		}

		/// Operator ->
		pf_hint_nodiscard pf_decl_constexpr const value_t*
		operator->() const pf_attr_noexcept
		{
			return this->ia_.begin();
		}

		/// Operator (bool)
		pf_hint_nodiscard pf_decl_explicit pf_decl_constexpr
		operator bool() const pf_attr_noexcept
		{
			return this->ia_.begin() != nullptr;
		}

		/// Operator (const value_t *)
		pf_hint_nodiscard pf_decl_constexpr
		operator const value_t* () const pf_attr_noexcept
		{
			return this->ia_.begin();
		}

	private:
		_Iterable *ia_;
	};

	template <typename _Iterable>
	pf_hint_nodiscard pf_decl_constexpr front_replace_iterator<_Iterable>
	make_replacer_front(
		_Iterable &__ct) pf_attr_noexcept
	requires(is_front_replaceable_v<_Iterable>)
	{
		return front_replace_iterator<_Iterable>(&__ct);
	}

	/// ITERABLE: Replacer -> Back
	template <typename _Iterable>
	class __back_replace_iterator_assigner
	{
	public:
		using value_t = typename _Iterable::value_t;

		/// Constructors
		__back_replace_iterator_assigner() = default;
		__back_replace_iterator_assigner(
			__back_replace_iterator_assigner<_Iterable> const &) = delete;

		/// Operator =
		__back_replace_iterator_assigner<_Iterable> &
		operator=(
			__back_replace_iterator_assigner<_Iterable> const &) = delete;
		pf_decl_constexpr __back_replace_iterator_assigner<_Iterable>&
		operator =(
			typename _Iterable::value_t &&__val)
		{
			this->ia_->replace_back(__val);
		}
		template <typename _IterableR>
		pf_decl_constexpr __back_replace_iterator_assigner<_Iterable>&
		operator =(
			_IterableR &&__ia)
		requires(is_iterable_v<_IterableR>)
		{
			this->ia_->replace_back(__ia.begin(), __ia.end());
		}

	private:
		_Iterable *ia_;
	};

	template <typename _Iterable>
	requires(is_back_replaceable_v<_Iterable>)
	class back_replace_iterator
	{
	public:
		using value_t	 = typename _Iterable::value_t;
		using category = typename _Iterable::iteator_t::category;

		/// Constructors
		pf_decl_constexpr back_replace_iterator(
			_Iterable *__ia)
			: ia_(__ia)
		{}

		/// Destructor
		pf_decl_constexpr ~back_replace_iterator() pf_attr_noexcept = default;

		/// Get
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const value_t*
		get() const pf_attr_noexcept
		{
			return this->ia_->end();
		}

		/// Operator =
		pf_decl_constexpr back_replace_iterator<_Iterable>&
		operator=(
			back_replace_iterator<_Iterable> const &__r) pf_attr_noexcept = default;

		/// Operator +=
		pf_decl_constexpr back_replace_iterator<_Iterable>&
		operator+=(
			diff_t) pf_attr_noexcept
		{
			return *this;
		}

		/// Operator ++
		pf_decl_constexpr back_replace_iterator<_Iterable>
		operator++(int32_t) pf_attr_noexcept
		{
			return *this;
		}
		pf_decl_constexpr back_replace_iterator<_Iterable> &
		operator++() pf_attr_noexcept
		{
			return *this;
		}

		/// Operator -=
		pf_decl_constexpr back_replace_iterator<_Iterable>&
		operator-=(
			diff_t) pf_attr_noexcept
		{
			return *this;
		}

		/// Operator --
		pf_decl_constexpr back_replace_iterator<_Iterable>
		operator--(int32_t) pf_attr_noexcept
		{
			return *this;
		}
		pf_decl_constexpr back_replace_iterator<_Iterable> &
		operator--() pf_attr_noexcept
		{
			return *this;
		}

		/// Operator *
		pf_hint_nodiscard pf_decl_constexpr
		operator*() const pf_attr_noexcept
		{
			return __back_replace_iterator_assigner(this->ia_);
		}

		/// Operator ->
		pf_hint_nodiscard pf_decl_constexpr const value_t*
		operator->() const pf_attr_noexcept
		{
			return this->ia_.end();
		}

		/// Operator (bool)
		pf_hint_nodiscard pf_decl_explicit pf_decl_constexpr
		operator bool() const pf_attr_noexcept
		{
			return this->ia_.end() != nullptr;
		}

		/// Operator (const value_t *)
		pf_hint_nodiscard pf_decl_constexpr
		operator const value_t* () const pf_attr_noexcept
		{
			return this->ia_.end().get();
		}

	private:
		_Iterable *ia_;
	};

	template <typename _Iterable>
	pf_hint_nodiscard pf_decl_constexpr back_replace_iterator<_Iterable>
	make_replacer_back(
		_Iterable &__ct) pf_attr_noexcept
	requires(is_back_replaceable_v<_Iterable>)
	{
		return back_replace_iterator<_Iterable>(&__ct);
	}

	/// ITERABLE: Replacer
	template <typename _Iterable>
	class __replace_iterator_assigner
	{
	public:
		using value_t		 = typename _Iterable::value_t;
		using iterator_t = typename _Iterable::iterator_t;

		/// Constructors
		__replace_iterator_assigner() = default;
		__replace_iterator_assigner(
			__replace_iterator_assigner<_Iterable> const &) = delete;

		/// Operator =
		__replace_iterator_assigner<_Iterable> &
		operator=(
			__replace_iterator_assigner<_Iterable> const &) = delete;
		pf_decl_constexpr __replace_iterator_assigner<_Iterable> &
		operator =(
			typename _Iterable::value_t &&__val)
		{
			*this->it_ = this->ia_->replace(*this->it_, __val);
		}
		template <typename _IterableR>
		pf_decl_constexpr __replace_iterator_assigner<_Iterable> &
		operator =(
			_IterableR &&__ia)
		requires(is_iterable_v<_IterableR>)
		{
			*this->it_ = this->ia_->replace(*this->it_, __ia.begin(), __ia.end());
		}

		/// Operator (value_t)
		pf_hint_nodiscard pf_decl_constexpr
		operator value_t&() pf_attr_noexcept
		{
			return **this->it_;
		}

	private:
		_Iterable *ia_;
		iterator_t *it_;
	};

	template <typename _Iterable>
	requires(is_placement_replaceable_v<_Iterable>)
	class replace_iterator
	{
	public:
		using value_t		 = typename _Iterable::value_t;
		using iterator_t = typename _Iterable::iterator_t;
		using category	 = typename _Iterable::iterator_t::category;

		/// Constructors
		pf_decl_constexpr replace_iterator(
			_Iterable *__ia,
			iterator_t __it) pf_attr_noexcept
			: ia_(__ia)
			, it_(__ia)
		{}

		/// Destructor
		pf_decl_constexpr ~replace_iterator() pf_attr_noexcept = default;

		/// Get
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const value_t*
		get() const pf_attr_noexcept
		{
			return this->it_.get();
		}

		/// Operator =
		pf_decl_constexpr replace_iterator<_Iterable>&
		operator=(
			replace_iterator<_Iterable> const &__r) pf_attr_noexcept = default;

		/// Operator +=
		pf_decl_constexpr replace_iterator<_Iterable>&
		operator+=(
			diff_t __i) pf_attr_noexcept
		{
			this->it_ += __i;
			return *this;
		}

		/// Operator ++
		pf_decl_constexpr replace_iterator<_Iterable>
		operator++(int32_t) pf_attr_noexcept
		{
			iterator_t p = this->it_++;
			return *this;
		}
		pf_decl_constexpr replace_iterator<_Iterable> &
		operator++() pf_attr_noexcept
		{
			++this->it_;
			return *this;
		}

		/// Operator -=
		pf_decl_constexpr replace_iterator<_Iterable>&
		operator-=(
			diff_t __i) pf_attr_noexcept
		{
			this->it_ -= __i;
			return *this;
		}

		/// Operator --
		pf_decl_constexpr replace_iterator<_Iterable>
		operator--(int32_t) pf_attr_noexcept
		{
			iterator_t p = this->it_--;
			return *this;
		}
		pf_decl_constexpr replace_iterator<_Iterable> &
		operator--() pf_attr_noexcept
		{
			--this->it_;
			return *this;
		}

		/// Operator *
		pf_hint_nodiscard pf_decl_constexpr
		operator*() const pf_attr_noexcept
		{
			return __replace_iterator_assigner(this->ia_, &this->it_);
		}

		/// Operator ->
		pf_hint_nodiscard pf_decl_constexpr const value_t*
		operator->() const pf_attr_noexcept
		{
			return this->it_.get();
		}

		/// Operator (bool)
		pf_hint_nodiscard pf_decl_explicit pf_decl_constexpr
		operator bool() const pf_attr_noexcept
		{
			return this->it_ != nullptr;
		}

		/// Operator (const value_t *)
		pf_hint_nodiscard pf_decl_constexpr
		operator const value_t* () const pf_attr_noexcept
		{
			return this->it_.get();
		}

	private:
		_Iterable *ia_;
		iterator_t it_;
	};

	template <typename _Iterable>
	pf_hint_nodiscard pf_decl_constexpr replace_iterator<_Iterable>
	make_replacer(
		_Iterable &__ct,
		typename _Iterable::iterator __it) pf_attr_noexcept
	requires(is_placement_replaceable_v<_Iterable>)
	{
		return replace_iterator<_Iterable>(&__ct, __it);
	}

	// Tests
	pf_assert_static(is_sequence_iterator_v<iterator<int32_t>>);
	pf_assert_static(is_iterator_v<iterator<int32_t>>);
	pf_assert_static(is_sequence_iterator_v<iterator<const int32_t>>);
	pf_assert_static(is_sequence_iterator_v<reverse_iterator<iterator<int32_t>>>);
	pf_assert_static(is_sequence_iterator_v<reverse_iterator<iterator<const int32_t>>>);
	pf_assert_static(is_sequence_iterator_v<move_iterator<iterator<int32_t>>>);
	pf_assert_static(is_sequence_iterator_v<move_iterator<iterator<const int32_t>>>);
}

#endif // !PULSAR_ITERATOR_HPP