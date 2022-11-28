/*! @file   iterator.hpp
 *  @author Fluffy (noe.louis-quentin@hotmail.fr)
 *  @brief
 *  @date   11-09-2022
 *
 *  @copyright Copyright (c) 2022 - Pulsar Software
 *
 *  @since 0.1.2
 */

#ifndef PULSAR_ITERATOR_HPP
#define PULSAR_ITERATOR_HPP 1

// Include: Pulsar
#include "pulsar/pulsar.hpp"

// Include: C++
#include <iterator>

// Pulsar
namespace pul
{
	/// ITERATOR: Category
	struct iterator_sequence_tag_t {};
	struct iterator_dangling_tag_t {};

	/// ITERATOR: Impl
	template <typename _Ty>
	class iterator
	{
		pf_decl_friend iterator<const _Ty>;

	public:
		using value_t		 = _Ty;
		using category_t = iterator_sequence_tag_t;

		/// Constructors
		pf_decl_inline pf_decl_constexpr
		iterator(
			_Ty *__ptr = nullptr) pf_attr_noexcept
			: ptr_(__ptr)
		{}
		pf_decl_inline pf_decl_constexpr
		iterator(
			iterator<_Ty> const &__it) pf_attr_noexcept
			: iterator(__it.ptr_)
		{}

		/// Destructor
		pf_decl_inline pf_decl_constexpr
		~iterator() pf_attr_noexcept = default;

		/// Get
		pf_decl_inline pf_decl_constexpr _Ty *get() pf_attr_noexcept
		{
			return this->ptr_;
		}
		pf_decl_inline pf_decl_constexpr const _Ty *get() const pf_attr_noexcept
		{
			return this->ptr_;
		}

		/// Operator =
		pf_decl_inline pf_decl_constexpr iterator<_Ty>
		&operator =(
			iterator<_Ty> const &__r) pf_attr_noexcept
		{
			this->ptr_ = __r.ptr_;
			return *this;
		}

		/// Operator +=
		pf_decl_inline pf_decl_constexpr iterator<_Ty>
		&operator +=(
			diff_t __i) pf_attr_noexcept
		{
			this->ptr_ += __i;
			return *this;
		}

		/// Operator ++
		pf_decl_inline pf_decl_constexpr iterator<_Ty>
		operator ++(
			int32_t) pf_attr_noexcept
		{
			iterator<_Ty> p = this->ptr_++;
			return p;
		}
		pf_decl_inline pf_decl_constexpr iterator<_Ty>
		&operator ++() pf_attr_noexcept
		{
			++this->ptr_;
			return *this;
		}

		/// Operator -=
		pf_decl_inline pf_decl_constexpr iterator<_Ty>
		&operator -=(
			diff_t __i) pf_attr_noexcept
		{
			this->ptr_ -= __i;
			return *this;
		}

		/// Operator --
		pf_decl_inline pf_decl_constexpr iterator<_Ty>
		operator --(
			int32_t) pf_attr_noexcept
		{
			iterator<_Ty> p = this->ptr_--;
			return p;
		}
		pf_decl_inline pf_decl_constexpr iterator<_Ty>
		&operator --() pf_attr_noexcept
		{
			--this->ptr_;
			return *this;
		}

		/// Operator *
		pf_decl_inline pf_decl_constexpr _Ty &
		operator *() pf_attr_noexcept
		{
			return *this->ptr_;
		}
		pf_decl_inline pf_decl_constexpr const _Ty &
		operator *() const pf_attr_noexcept
		{
			return *this->ptr_;
		}

		/// Operator ->
		pf_decl_inline pf_decl_constexpr _Ty *
		operator ->() pf_attr_noexcept
		{
			return this->ptr_;
		}
		pf_decl_inline pf_decl_constexpr const _Ty *
		operator ->() const pf_attr_noexcept
		{
			return this->ptr_;
		}

		/// Operator ==
		// TODO: Iterator Operator <=>
		pf_decl_inline pf_decl_constexpr bool
		operator ==(
			iterator<const _Ty> __it) const pf_attr_noexcept
		{
			return this->ptr_ == __it.get();
		}
		pf_decl_inline pf_decl_constexpr bool
		operator !=(
			iterator<const _Ty> __it) const pf_attr_noexcept
		{
			return this->ptr_ != __it.get();
		}

		/// Operator (Bool)
		pf_decl_explicit pf_decl_inline pf_decl_constexpr
		operator bool() const pf_attr_noexcept
		{
			return this->ptr_ != nullptr;
		}

		/// Operator (_Ty*)
		pf_decl_inline pf_decl_constexpr
		operator _Ty *() pf_attr_noexcept
		{
			return this->ptr_;
		}
		pf_decl_inline pf_decl_constexpr
		operator const _Ty *() const pf_attr_noexcept
		{
			return this->ptr_;
		}

	private:
		_Ty *ptr_;
	};

	/// Operator +
	template <typename _Ty>
	pf_hint_nodiscard pf_decl_inline pf_decl_constexpr iterator<_Ty>
	operator +(
		iterator<_Ty> const &__l,
		diff_t __i) pf_attr_noexcept
	{
		iterator<_Ty> it = __l;
		return it += __i;
	}

	/// Operator -
	template <typename _Ty>
	pf_hint_nodiscard pf_decl_inline pf_decl_constexpr iterator<_Ty>
	operator -(
		iterator<_Ty> const &__l,
		diff_t __i) pf_attr_noexcept
	{
		iterator<_Ty> it = __l;
		return it -= __i;
	}

	/// CTAD
	template <typename _Ty>
	iterator(_Ty *)->iterator<_Ty>;


	/// ITERATOR: Const Impl
	template <typename _Ty>
	class iterator<const _Ty>
	{
	public:
		using value_t		 = const _Ty;
		using category_t = iterator_sequence_tag_t;

		/// Constructors
		pf_decl_inline pf_decl_constexpr
		iterator(
			const _Ty *__ptr) pf_attr_noexcept
			: ptr_(__ptr)
		{}
		pf_decl_inline pf_decl_constexpr
		iterator(
			iterator<const _Ty> const &__r) pf_attr_noexcept
			: iterator(__r.ptr_)
		{}
		pf_decl_inline pf_decl_constexpr
		iterator(
			iterator<_Ty> const &__r) pf_attr_noexcept
			: iterator(__r.ptr_)
		{}

		/// Destructor
		pf_decl_inline pf_decl_constexpr
		~iterator() pf_attr_noexcept = default;

		/// Get
		pf_decl_inline pf_decl_constexpr const _Ty *get() const pf_attr_noexcept
		{
			return this->ptr_;
		}

		/// Operator =
		pf_decl_inline pf_decl_constexpr iterator<const _Ty>
		&operator =(
			iterator<const _Ty> const &__r) pf_attr_noexcept
		{
			this->ptr_ = __r.ptr_;
			return *this;
		}

		/// Operator +=
		pf_decl_inline pf_decl_constexpr iterator<const _Ty>
		&operator +=(
			diff_t __i) pf_attr_noexcept
		{
			this->ptr_ += __i;
			return *this;
		}

		/// Operator ++
		pf_decl_inline pf_decl_constexpr iterator<const _Ty>
		operator ++(
			int32_t) pf_attr_noexcept
		{
			iterator<const _Ty> p = this->ptr_++;
			return p;
		}
		pf_decl_inline pf_decl_constexpr iterator<const _Ty>
		&operator ++() pf_attr_noexcept
		{
			++this->ptr_;
			return *this;
		}

		/// Operator -=
		pf_decl_inline pf_decl_constexpr iterator<const _Ty>
		&operator -=(
			diff_t __i) pf_attr_noexcept
		{
			this->ptr_ -= __i;
			return *this;
		}

		/// Operator --
		pf_decl_inline pf_decl_constexpr iterator<const _Ty>
		operator --(
			int32_t) pf_attr_noexcept
		{
			iterator<_Ty> p = this->ptr_--;
			return p;
		}
		pf_decl_inline pf_decl_constexpr iterator<const _Ty>
		&operator --() pf_attr_noexcept
		{
			--this->ptr_;
			return *this;
		}

		/// Operator *
		pf_decl_inline pf_decl_constexpr const _Ty &
		operator *() const pf_attr_noexcept
		{
			return *this->ptr_;
		}

		/// Operator ->
		pf_decl_inline pf_decl_constexpr const _Ty *
		operator ->() const pf_attr_noexcept
		{
			return this->ptr_;
		}

		/// Operator ==
		// TODO: Const Iterator Operator<=>
		pf_decl_inline pf_decl_constexpr bool
		operator ==(
			iterator<const _Ty> __it) const pf_attr_noexcept
		{
			return this->ptr_ == __it.get();
		}
		pf_decl_inline pf_decl_constexpr bool
		operator !=(
			iterator<const _Ty> __it) const pf_attr_noexcept
		{
			return this->ptr_ != __it.get();
		}

		/// Operator (Bool)
		pf_decl_explicit pf_decl_inline pf_decl_constexpr
		operator bool() const pf_attr_noexcept
		{
			return this->ptr_ != nullptr;
		}

		/// Operator (_Ty*)
		pf_decl_inline pf_decl_constexpr
		operator const _Ty *() const pf_attr_noexcept
		{
			return this->ptr_;
		}

	private:
		const _Ty *ptr_;
	};

	/// Operator +
	template <typename _Ty>
	pf_hint_nodiscard pf_decl_inline pf_decl_constexpr iterator<const _Ty>
	operator +(
		iterator<const _Ty> const &__l,
		diff_t __i) pf_attr_noexcept
	{
		iterator<const _Ty> it = __l;
		return it += __i;
	}

	/// Operator -
	template <typename _Ty>
	pf_hint_nodiscard pf_decl_inline pf_decl_constexpr iterator<const _Ty>
	operator -(
		iterator<const _Ty> const &__l,
		diff_t __i) pf_attr_noexcept
	{
		iterator<const _Ty> it = __l;
		return it -= __i;
	}

	/// CTAD
	template <typename _Ty>
	iterator(const _Ty *)->iterator<const _Ty>;

	/// Alias
	template <typename _Ty>
	using const_iterator = iterator<const _Ty>;




	/// ITERATOR: Reverse Impl
	template <typename _Ty>
	class reverse_iterator
	{
		pf_decl_friend reverse_iterator<const _Ty>;

	public:
		using value_t		 = _Ty;
		using category_t = iterator_sequence_tag_t;

		/// Constructors
		pf_decl_inline pf_decl_constexpr
		reverse_iterator(
			_Ty *__ptr = nullptr) pf_attr_noexcept
			: ptr_(__ptr)
		{}
		pf_decl_inline pf_decl_constexpr
		reverse_iterator(
			reverse_iterator<_Ty> const &__it) pf_attr_noexcept
			: reverse_iterator(__it.ptr_)
		{}

		/// Destructor
		pf_decl_inline pf_decl_constexpr
		~reverse_iterator() pf_attr_noexcept = default;

		/// Get
		pf_decl_inline pf_decl_constexpr _Ty *get() pf_attr_noexcept
		{
			return this->ptr_;
		}
		pf_decl_inline pf_decl_constexpr const _Ty *get() const pf_attr_noexcept
		{
			return this->ptr_;
		}

		/// Operator =
		pf_decl_inline pf_decl_constexpr reverse_iterator<_Ty>
		&operator =(
			reverse_iterator<_Ty> const &__r) pf_attr_noexcept
		{
			this->ptr_ = __r.ptr_;
			return *this;
		}

		/// Operator +=
		pf_decl_inline pf_decl_constexpr reverse_iterator<_Ty>
		&operator +=(
			diff_t __i) pf_attr_noexcept
		{
			this->ptr_ -= __i;
			return *this;
		}

		/// Operator ++
		pf_decl_inline pf_decl_constexpr reverse_iterator<_Ty>
		operator ++(
			int32_t) pf_attr_noexcept
		{
			reverse_iterator<_Ty> p = this->ptr_--;
			return p;
		}
		pf_decl_inline pf_decl_constexpr reverse_iterator<_Ty>
		&operator ++() pf_attr_noexcept
		{
			--this->ptr_;
			return *this;
		}

		/// Operator -=
		pf_decl_inline pf_decl_constexpr reverse_iterator<_Ty>
		&operator -=(
			diff_t __i) pf_attr_noexcept
		{
			this->ptr_ += __i;
			return *this;
		}

		/// Operator --
		pf_decl_inline pf_decl_constexpr reverse_iterator<_Ty>
		operator --(
			int32_t) pf_attr_noexcept
		{
			reverse_iterator<_Ty> p = this->ptr_++;
			return p;
		}
		pf_decl_inline pf_decl_constexpr reverse_iterator<_Ty>
		&operator --() pf_attr_noexcept
		{
			++this->ptr_;
			return *this;
		}

		/// Operator *
		pf_decl_inline pf_decl_constexpr _Ty &
		operator *() pf_attr_noexcept
		{
			return *this->ptr_;
		}
		pf_decl_inline pf_decl_constexpr const _Ty &
		operator *() const pf_attr_noexcept
		{
			return *this->ptr_;
		}

		/// Operator ->
		pf_decl_inline pf_decl_constexpr _Ty *
		operator ->() pf_attr_noexcept
		{
			return this->ptr_;
		}
		pf_decl_inline pf_decl_constexpr const _Ty *
		operator ->() const pf_attr_noexcept
		{
			return this->ptr_;
		}

		/// Operator <=>
		// pf_decl_inline pf_decl_constexpr bool
		// operator ==(
		// 	iterator<const _Ty> __it) const pf_attr_noexcept
		// {
		// 	return this->ptr_ == __it.get();
		// }
		// pf_decl_inline pf_decl_constexpr bool
		// operator !=(
		// 	iterator<const _Ty> __it) const pf_attr_noexcept
		// {
		// 	return this->ptr_ != __it.get();
		// }
		// TODO: Reverse Iterator Operator <=>
		pf_decl_inline pf_decl_constexpr std::strong_ordering
		operator <=>(
			iterator<const _Ty> __it) const pf_attr_noexcept
		{
			return this->ptr_ <=> __it.get();
		}


		/// Operator (Bool)
		pf_decl_explicit pf_decl_inline pf_decl_constexpr
		operator bool() const pf_attr_noexcept
		{
			return this->ptr_ != nullptr;
		}

		/// Operator (_Ty*)
		pf_decl_inline pf_decl_constexpr
		operator _Ty *() pf_attr_noexcept
		{
			return this->ptr_;
		}
		pf_decl_inline pf_decl_constexpr
		operator const _Ty *() const pf_attr_noexcept
		{
			return this->ptr_;
		}

	private:
		_Ty *ptr_;
	};

	/// Operator +
	template <typename _Ty>
	pf_hint_nodiscard pf_decl_inline pf_decl_constexpr reverse_iterator<_Ty>
	operator +(
		reverse_iterator<_Ty> const &__l,
		diff_t __i) pf_attr_noexcept
	{
		reverse_iterator<_Ty> it = __l;
		return it += __i;
	}

	/// Operator -
	template <typename _Ty>
	pf_hint_nodiscard pf_decl_inline pf_decl_constexpr reverse_iterator<_Ty>
	operator -(
		reverse_iterator<_Ty> const &__l,
		diff_t __i) pf_attr_noexcept
	{
		reverse_iterator<_Ty> it = __l;
		return it -= __i;
	}

	/// CTAD
	template <typename _Ty>
	reverse_iterator(_Ty *)->reverse_iterator<_Ty>;




	/// ITERATOR: Const Reverse Impl
	template <typename _Ty>
	class reverse_iterator<const _Ty>
	{
	public:
		using value_t		 = const _Ty;
		using category_t = iterator_sequence_tag_t;

		/// Constructors
		pf_decl_inline pf_decl_constexpr
		reverse_iterator(
			const _Ty *__ptr) pf_attr_noexcept
			: ptr_(__ptr)
		{}
		pf_decl_inline pf_decl_constexpr
		reverse_iterator(
			reverse_iterator<const _Ty> const &__r) pf_attr_noexcept
			: reverse_iterator(__r.ptr_)
		{}
		pf_decl_inline pf_decl_constexpr
		reverse_iterator(
			reverse_iterator<_Ty> const &__r) pf_attr_noexcept
			: reverse_iterator(__r.ptr_)
		{}

		/// Destructor
		pf_decl_inline pf_decl_constexpr
		~reverse_iterator() pf_attr_noexcept = default;

		/// Get
		pf_decl_inline pf_decl_constexpr const _Ty *get() const pf_attr_noexcept
		{
			return this->ptr_;
		}

		/// Operator =
		pf_decl_inline pf_decl_constexpr reverse_iterator<const _Ty>
		&operator =(
			reverse_iterator<const _Ty> const &__r) pf_attr_noexcept
		{
			this->ptr_ = __r.ptr_;
			return *this;
		}

		/// Operator +=
		pf_decl_inline pf_decl_constexpr reverse_iterator<const _Ty>
		&operator +=(
			diff_t __i) pf_attr_noexcept
		{
			this->ptr_ -= __i;
			return *this;
		}

		/// Operator ++
		pf_decl_inline pf_decl_constexpr reverse_iterator<const _Ty>
		operator ++(
			int32_t) pf_attr_noexcept
		{
			reverse_iterator<const _Ty> p = this->ptr_--;
			return p;
		}
		pf_decl_inline pf_decl_constexpr reverse_iterator<const _Ty>
		&operator ++() pf_attr_noexcept
		{
			--this->ptr_;
			return *this;
		}

		/// Operator -=
		pf_decl_inline pf_decl_constexpr reverse_iterator<const _Ty>
		&operator -=(
			diff_t __i) pf_attr_noexcept
		{
			this->ptr_ += __i;
			return *this;
		}

		/// Operator --
		pf_decl_inline pf_decl_constexpr reverse_iterator<const _Ty>
		operator --(
			int32_t) pf_attr_noexcept
		{
			reverse_iterator<_Ty> p = this->ptr_++;
			return p;
		}
		pf_decl_inline pf_decl_constexpr reverse_iterator<const _Ty>
		&operator --() pf_attr_noexcept
		{
			++this->ptr_;
			return *this;
		}

		/// Operator *
		pf_decl_inline pf_decl_constexpr const _Ty &
		operator *() const pf_attr_noexcept
		{
			return *this->ptr_;
		}

		/// Operator ->
		pf_decl_inline pf_decl_constexpr const _Ty *
		operator ->() const pf_attr_noexcept
		{
			return this->ptr_;
		}

		/// Operator ==
		// TODO: Const Reverse Iterator operator<=>
		pf_decl_inline pf_decl_constexpr bool
		operator ==(
			iterator<const _Ty> __it) const pf_attr_noexcept
		{
			return this->ptr_ == __it.get();
		}
		pf_decl_inline pf_decl_constexpr bool
		operator !=(
			iterator<const _Ty> __it) const pf_attr_noexcept
		{
			return this->ptr_ != __it.get();
		}

		/// Operator (Bool)
		pf_decl_explicit pf_decl_inline pf_decl_constexpr
		operator bool() const pf_attr_noexcept
		{
			return this->ptr_ != nullptr;
		}

		/// Operator (_Ty*)
		pf_decl_inline pf_decl_constexpr
		operator const _Ty *() const pf_attr_noexcept
		{
			return this->ptr_;
		}

	private:
		const _Ty *ptr_;
	};

	/// Operator +
	template <typename _Ty>
	pf_hint_nodiscard pf_decl_inline pf_decl_constexpr reverse_iterator<const _Ty>
	operator +(
		reverse_iterator<const _Ty> const &__l,
		diff_t __i) pf_attr_noexcept
	{
		reverse_iterator<const _Ty> it = __l;
		return it += __i;
	}

	/// Operator -
	template <typename _Ty>
	pf_hint_nodiscard pf_decl_inline pf_decl_constexpr reverse_iterator<const _Ty>
	operator -(
		reverse_iterator<const _Ty> const &__l,
		diff_t __i) pf_attr_noexcept
	{
		reverse_iterator<const _Ty> it = __l;
		return it -= __i;
	}

	/// CTAD
	template <typename _Ty>
	reverse_iterator(const _Ty *)->reverse_iterator<const _Ty>;

	/// Alias
	template <typename _Ty>
	using const_reverse_iterator = reverse_iterator<const _Ty>;


	/// ITERATOR: SFINAE -> Iterator Category
	template <typename _Ty>
	struct iterator_category
	{
		using type = typename _Ty::category_t;
	};
	template <typename _Ty>
	using iterator_category_t = typename iterator_category<_Ty>::type;

	/// UTILITY: SFINAE -> Is Iterator
	template <typename _Ty>
	struct is_iterator : std::false_type
	{};
}

#endif // !PULSAR_ITERATOR_HPP