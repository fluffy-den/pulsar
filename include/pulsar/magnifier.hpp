/*! @file   magnifier.hpp
 *  @author Louis-Quentin Noé (noe.louis-quentin@hotmail.fr)
 *  @brief
 *  @date   05-03-2023
 *
 *  @copyright Copyright (c) 2023 - Pulsar Software
 *
 *  @since 0.1.4
 */

#ifndef PULSAR_MAGNIFIER_HPP
#define PULSAR_MAGNIFIER_HPP 1

// Include: Pulsar
#include "pulsar/pulsar.hpp"
#include "pulsar/debug.hpp"

// Include: C++
#include <concepts>

// Pulsar
namespace pul
{
	/*
	 * !           !
	 *   Magnifier
	 * !           !
	 */
	/// MAGNIFIER: Concept
	template<typename _Magnifier>
	concept __magnifier_c =
	 requires(	// clang-format off
		_Magnifier const &__m) 
	{
		{ __m(std::declval<size_t>()) } -> std::convertible_to<size_t>;
	};	 // clang-format on
	template<typename _Magnifier>
	struct is_magnifier : std::false_type
	{};
	template<__magnifier_c _Magnifier>
	struct is_magnifier<_Magnifier> : std::true_type
	{};
	template<typename _Magnifier>
	pf_decl_constexpr pf_decl_inline bool is_magnifier_v = is_magnifier<_Magnifier>::value;

	/// MAGNIFIER: Default
	class magnifier_default pf_attr_final
	{
	public:
		/// Operator()
		pf_hint_nodiscard pf_decl_constexpr size_t
		operator()(size_t __rs) const pf_attr_noexcept
		{
			return __rs;
		}
	};

	/// MAGNIFIER: Linear
	class magnifier_linear pf_attr_final
	{
	public:
		/// Constructors
		pf_decl_constexpr
		magnifier_linear(
		 size_t __count = 2) pf_attr_noexcept
			: count_(__count)
		{
			if(__count == 0)
				pf_throw(
				 dbg_category_generic(),
				 dbg_code::invalid_argument,
				 dbg_flags::none,
				 "__count can't be equal to 0!");
		}
		pf_decl_constexpr
		magnifier_linear(
		 magnifier_linear const &__r) pf_attr_noexcept
			: count_(__r.count_)
		{}
		pf_decl_constexpr
		magnifier_linear(
		 magnifier_linear &&) pf_attr_noexcept = default;

		/// Destructor
		pf_decl_constexpr ~magnifier_linear() pf_attr_noexcept = default;

		/// Operator=
		pf_decl_constexpr magnifier_linear &
		operator=(
		 magnifier_linear const &__r) pf_attr_noexcept
		{
			if(pf_likely(this != &__r))
			{
				this->count_ = __r.count_;
			}
			return *this;
		}
		pf_decl_constexpr magnifier_linear &
		operator=(
		 magnifier_linear &&__r) pf_attr_noexcept = default;

		/// Operator()
		pf_hint_nodiscard pf_decl_constexpr size_t
		operator()(
			size_t __rs) const pf_attr_noexcept
		{
			return (__rs + this->count_ - 1) / this->count_;
		}

	private:
		size_t count_;
	};

	/// MAGNIFIER: Quadratic
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
#endif	// !PF_64BIT
			++__rs;
			return __rs;
		}
	};
}	 // namespace pul

#endif	// !PULSAR_MAGNIFIER_HPP
