/*! @file   char_traits_span.hpp
 *  @author Louis-Quentin Noé (noe.louis-quentin@hotmail.fr)
 *  @brief
 *  @date   02-01-2023
 *
 *  @copyright Copyright (c) 2023 - Pulsar Software
 *
 *  @since 0.1.3
 */

#ifndef PULSAR_CHAR_TRAITS_SPAN_HPP
#define PULSAR_CHAR_TRAITS_SPAN_HPP 1

// Include: Pulsar
#include "pulsar/encoding.hpp"

// Pulsar
namespace pul
{
	/// TRAITS: String View
	template<typename _Traits>
	class char_traits_string_view
	{
	public:
		using traits									 = _Traits;
		using value_t									 = typename _Traits::value_t;
		using code_value_t						 = typename _Traits::code_value_t;
		using const_iterator_t				 = char_traits_const_iterator<_Traits>;
		using const_reverse_iterator_t = char_traits_const_reverse_iterator<_Traits>;

		/// Constructors
		pf_decl_constexpr
		char_traits_string_view() pf_attr_noexcept
		{}
		pf_decl_constexpr
		char_traits_string_view(
			const_iterator_t __beg,
			const_iterator_t __end)
			: view_(__beg.get(), __end.get())
		{
			// TODO: !is_valid __beg, __end
		}
		pf_decl_constexpr
		char_traits_string_view(
			sequence_view<value_t> __view)
			: char_traits_string_view(__view.begin(), __view.end())
		{}
		pf_decl_constexpr
		char_traits_string_view(
			const value_t *__str)
			: char_traits_string_view(
				const_iterator_t(__str),
				const_iterator_t(__str + traits::lenof(__str)))
		{}
		pf_decl_constexpr
		char_traits_string_view(
			char_traits_string_view<value_t> const &__r) pf_attr_noexcept
			: view_(__r.view_)
		{}

		/// Destructor
		pf_decl_constexpr ~char_traits_string_view() pf_attr_noexcept = default;

		/// Operator ==
		pf_hint_nodiscard pf_decl_constexpr bool
		operator==(
			char_traits_string_view<value_t> __r) pf_attr_noexcept
		{
			return this->spn_ == __r.spn_;
		}
		pf_hint_nodiscard pf_decl_constexpr bool
		operator!=(
			char_traits_string_view<value_t> __r) pf_attr_noexcept
		{
			return this->spn_ != __r.spn_;
		}

		/// Begin
		pf_hint_nodiscard pf_decl_constexpr const_iterator_t
		begin() const pf_attr_noexcept
		{
			return this->data();
		}
		pf_hint_nodiscard pf_decl_constexpr const_iterator_t
		cbegin() const pf_attr_noexcept
		{
			return this->data();
		}

		/// Reverse Begin
		pf_hint_nodiscard pf_decl_constexpr const_reverse_iterator_t
		rbegin() const pf_attr_noexcept
		{
			return traits::prevof(this->end());
		}
		pf_hint_nodiscard pf_decl_constexpr const_reverse_iterator_t
		crbegin() const pf_attr_noexcept
		{
			return traits::prevof(this->end());
		}

		/// End
		pf_hint_nodiscard pf_decl_constexpr const_iterator_t
		end() const pf_attr_noexcept
		{
			return this->data() + this->count();
		}
		pf_hint_nodiscard pf_decl_constexpr const_iterator_t
		cend() const pf_attr_noexcept
		{
			return this->data() + this->count();
		}

		/// Reverse End
		pf_hint_nodiscard pf_decl_constexpr const_reverse_iterator_t
		rend() const pf_attr_noexcept
		{
			return this->data();
		}
		pf_hint_nodiscard pf_decl_constexpr const_reverse_iterator_t
		crend() const pf_attr_noexcept
		{
			return this->data();
		}

		/// Data
		pf_hint_nodiscard pf_decl_constexpr const value_t*
		data() const pf_attr_noexcept
		{
			return this->view_.data();
		}

		/// Count
		pf_hint_nodiscard pf_decl_constexpr size_t
		count() const pf_attr_noexcept
		{
			return this->view_.count();
		}

		/// Code Count
		pf_hint_nodiscard pf_decl_constexpr size_t
		code_count() const pf_attr_noexcept
		{
			return this->countof(this->begin(), this->end());
		}

		/// Size
		pf_hint_nodiscard pf_decl_constexpr size_t
		size() const pf_attr_noexcept
		{
			return this->view_.size();
		}

		/// Is Empty
		pf_hint_nodiscard pf_decl_constexpr bool
		is_empty() const pf_attr_noexcept
		{
			return this->view_.is_empty();
		}

		/// Operator (span<value_t> &)
		pf_decl_constexpr operator sequence_view<value_t> &() const pf_attr_noexcept
		{
			return this->view_;
		}

	private:
		sequence_view<value_t> view_;
	};

	/// TRAITS: String Span -> Alias
	using a7string_view_t = char_traits_string_view<char_traits::ascii>;
	// TODO: using wsstring_view_t = char_traits_string_view<char_traits::wide>;
	// TODO: using u8string_view_t = char_traits_string_view<char_traits::utf8>;
}

#endif // !PULSAR_CHAR_TRAITS_SPAN_HPP