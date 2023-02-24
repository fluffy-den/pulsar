/*! @file   char_traits_iterator.hpp
 *  @author Louis-Quentin Noé (noe.louis-quentin@hotmail.fr)
 *  @brief
 *  @date   02-01-2023
 *
 *  @copyright Copyright (c) 2023 - Pulsar Software
 *
 *  @since 0.1.3
 */

#ifndef PULSAR_CHAR_TRAITS_ITERATOR_HPP
#define PULSAR_CHAR_TRAITS_ITERATOR_HPP 1

// Include: Pulsar
#include "pulsar/encoding.hpp"

// Pulsar
namespace pul
{
	/// TRAITS: Iterator -> Concept
	// TODO

	/// TRAITS: Iterator -> Replacer
	template <typename _Traits>
	class char_traits_string_view;
	template<typename _TraitIterator, typename _String>
	class __char_traits_iterator_replacer
	{
	public:
		using traits			 = typename _TraitIterator::traits;
		using value_t			 = typename _TraitIterator::value_t;
		using code_value_t = typename _TraitIterator::code_value_t;

		/// Constructors
		pf_decl_constexpr
		__char_traits_iterator_replacer(
			_TraitIterator __it) pf_attr_noexcept
			: it_(__it)
		{}
		pf_decl_constexpr
		__char_traits_iterator_replacer(
			__char_traits_iterator_replacer<_TraitIterator, _String> const &) = delete;

		/// Destructor
		pf_decl_constexpr ~__char_traits_iterator_replacer() pf_attr_noexcept = default;

		/// Operator=
		__char_traits_iterator_replacer<_TraitIterator, _String> &
		operator=(__char_traits_iterator_replacer<_TraitIterator, _String> const &) = delete;

		pf_decl_constexpr __char_traits_iterator_replacer<_TraitIterator, _String> &
		operator=(code_value_t __cv) pf_attr_noexcept
		{
			this->it_.cnt_->replace(this->it_, __cv);
			return *this;
		}
		pf_decl_constexpr __char_traits_iterator_replacer<_TraitIterator, _String> &
		operator=(
			char_traits_string_view<traits> __ia)
		{
			this->it_.cnt_->replace(this->it_, __ia.begin(), __ia.end());
		}

		/// Operator (code_value_t)
		pf_hint_nodiscard pf_decl_constexpr
		operator code_value_t() const pf_attr_noexcept
		{
			return traits::to_code_point(this->it_.ptr_);
		}

	private:
		_TraitIterator it_;
	};


	/// TRAITS: Iterator
	template<typename _String>
	class char_traits_iterator
	{
	pf_decl_friend class __char_traits_iterator_replacer<char_traits_iterator<_String>, _String>;

	public:
		using traits			 = typename _String::traits;
		using value_t			 = typename _String::value_t;
		using code_value_t = typename _String::code_value_t;
		using category		 = iterator_sequence_tag_t;

		/// Constructors
		pf_decl_constexpr
		char_traits_iterator(
			value_t *__ptr,
			_String *__cnt) pf_attr_noexcept
			: ptr_(__ptr)
			, cnt_(__cnt)
		{}
		pf_decl_constexpr
		char_traits_iterator(
			char_traits_iterator<_String> const &__it) pf_attr_noexcept
			: ptr_(__it.ptr_)
			, cnt_(__it.cnt_)
		{}

		/// Destructor
		pf_decl_constexpr ~char_traits_iterator() pf_attr_noexcept = default;

		/// Get
		pf_hint_nodiscard pf_decl_constexpr value_t*
		get() pf_attr_noexcept
		{
			return this->ptr_;
		}
		pf_hint_nodiscard pf_decl_constexpr const value_t*
		get() const pf_attr_noexcept
		{
			return this->ptr_;
		}

		/// String
		pf_hint_nodiscard pf_decl_constexpr _String*
		linked_string() pf_attr_noexcept
		{
			return this->cnt_;
		}

		/// Operator =
		pf_decl_constexpr char_traits_iterator<_String> &
		operator=(char_traits_iterator<_String> const &__r) pf_attr_noexcept = default;

		/// Operator +=
		pf_decl_constexpr char_traits_iterator<_String> &
		operator+=(diff_t __i) pf_attr_noexcept
		{
			for(; __i >= 0; --__i) { this->ptr_ = traits::nextof(this->ptr_); }
			for(; __i <= 0; ++__i) { this->ptr_ = traits::prevof(this->ptr_); }
			return *this;
		}

		/// Operator ++
		pf_decl_constexpr char_traits_iterator<_String>
		operator++(int32_t) pf_attr_noexcept
		{
			char_traits_iterator<_String> p = this->ptr_;
			this->ptr_ = traits::nextof(this->ptr_);
			return p;
		}
		pf_decl_constexpr char_traits_iterator<_String> &
		operator++() pf_attr_noexcept
		{
			this->ptr_ = traits::nextof(this->ptr_);
			return *this;
		}

		/// Operator -=
		pf_decl_constexpr char_traits_iterator<_String> &
		operator-=(diff_t __i) pf_attr_noexcept
		{
			for(; __i >= 0; --__i) { this->ptr_ = traits::prevof(this->ptr_); }
			for(; __i <= 0; ++__i) { this->ptr_ = traits::nextof(this->ptr_); }
			return *this;
		}

		/// Operator --
		pf_decl_constexpr char_traits_iterator<_String>
		operator--(int32_t) pf_attr_noexcept
		{
			char_traits_iterator<_String> p = this->ptr_;
			this->ptr_ = traits::prevof(this->ptr_);
			return p;
		}
		pf_decl_constexpr char_traits_iterator<_String> &
		operator--() pf_attr_noexcept
		{
			this->ptr_ = traits::prevof(this->ptr_);
			return *this;
		}

		/// Operator *
		pf_hint_nodiscard pf_decl_constexpr __char_traits_iterator_replacer<char_traits_iterator<_String>, _String>
		operator*() pf_attr_noexcept
		{
			return __char_traits_iterator_replacer<char_traits_iterator<_String>, _String>(*this);
		}
		pf_hint_nodiscard pf_decl_constexpr code_value_t
		operator*() const pf_attr_noexcept
		{
			return traits::to_code_point(this->ptr_);
		}

		/// Operator (value_t *)
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

		/// Operator (bool)
		pf_hint_nodiscard pf_decl_explicit pf_decl_constexpr
		operator bool() pf_attr_noexcept
		{
			return this->ptr_ != nullptr;
		}

	private:
		value_t *ptr_;
		_String *cnt_;
	};

	template<typename _Traits>
	class char_traits_const_iterator
	{
	public:
		using traits			 = _Traits;
		using value_t			 = const typename _Traits::value_t;
		using code_value_t = typename _Traits::code_value_t;
		using category		 = iterator_sequence_tag_t;

		/// Constructors
		pf_decl_constexpr
		char_traits_const_iterator(
			value_t *__ptr) pf_attr_noexcept
			: ptr_(__ptr)
		{}
		pf_decl_constexpr
		char_traits_const_iterator(
			char_traits_const_iterator<_Traits> const &__it) pf_attr_noexcept
			: ptr_(__it.ptr_)
		{}

		/// Destructor
		pf_decl_constexpr ~char_traits_const_iterator() pf_attr_noexcept = default;

		/// Get
		pf_hint_nodiscard pf_decl_constexpr value_t*
		get() const pf_attr_noexcept
		{
			return this->ptr_;
		}

		/// Operator =
		pf_decl_constexpr char_traits_const_iterator<_Traits> &
		operator=(char_traits_const_iterator<_Traits> const &__r) pf_attr_noexcept = default;

		/// Operator +=
		pf_decl_constexpr char_traits_const_iterator<_Traits> &
		operator+=(diff_t __i) pf_attr_noexcept
		{
			for(; __i > 0; --__i) { this->ptr_ = traits::nextof(this->ptr_); }
			for(; __i < 0; ++__i) { this->ptr_ = traits::prevof(this->ptr_); }
			return *this;
		}

		/// Operator ++
		pf_decl_constexpr char_traits_const_iterator<_Traits>
		operator++(int32_t) pf_attr_noexcept
		{
			char_traits_const_iterator<_Traits> p = this->ptr_;
			this->ptr_ = traits::nextof(this->ptr_);
			return p;
		}
		pf_decl_constexpr char_traits_const_iterator<_Traits> &
		operator++() pf_attr_noexcept
		{
			this->ptr_ = traits::nextof(this->ptr_);
			return *this;
		}

		/// Operator -=
		pf_decl_constexpr char_traits_const_iterator<_Traits> &
		operator-=(diff_t __i) pf_attr_noexcept
		{
			for(; __i > 0; --__i) { this->ptr_ = traits::prevof(this->ptr_); }
			for(; __i < 0; ++__i) { this->ptr_ = traits::nextof(this->ptr_); }
			return *this;
		}

		/// Operator --
		pf_decl_constexpr char_traits_const_iterator<_Traits>
		operator--(int32_t) pf_attr_noexcept
		{
			char_traits_const_iterator<_Traits> p = this->ptr_;
			this->ptr_ = traits::prevof(this->ptr_);
			return p;
		}
		pf_decl_constexpr char_traits_const_iterator<_Traits> &
		operator--() pf_attr_noexcept
		{
			this->ptr_ = traits::prevof(this->ptr_);
			return *this;
		}

		/// Operator *
		pf_hint_nodiscard pf_decl_constexpr code_value_t
		operator*() const pf_attr_noexcept
		{
			return traits::to_code_point(this->ptr_);
		}

		/// Operator (const value_t *)
		pf_hint_nodiscard pf_decl_constexpr
		operator value_t* () const pf_attr_noexcept
		{
			return this->ptr_;
		}

		/// Operator (bool)
		pf_hint_nodiscard pf_decl_explicit pf_decl_constexpr
		operator bool() pf_attr_noexcept
		{
			return this->ptr_ != nullptr;
		}

	private:
		value_t *ptr_;
	};
	template <typename _String>
	using char_traits_reverse_iterator = reverse_iterator<char_traits_iterator<_String>>;
	template <typename _Traits>
	using char_traits_const_reverse_iterator = reverse_iterator<char_traits_const_iterator<_Traits>>;

	/// TRAITS: Iterator -> Is Sequence (Explicit)
	template <typename _String>
	struct is_sequence_iterator<char_traits_iterator<_String>>
		: std::true_type
	{};
	template <typename _Traits>
	struct is_sequence_iterator<char_traits_const_iterator<_Traits>>
		: std::true_type
	{};

	/// TRAITS: Iterator -> Is Traits Iterator

}

#endif // !PULSAR_CHAR_TRAITS_ITERATOR_HPP