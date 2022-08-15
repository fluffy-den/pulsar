/*! @file   fun_buf.inl
 *  @author Fluffy (noe.louis-quentin@hotmail.fr)
 *  @brief	Implementation of function buffer.
 *  @date   11-08-2022
 *
 *  @copyright Copyright (c) 2022 - Pulsar Software
 *
 *  @since 0.1.2
 */

#ifndef PULSAR_FUN_BUF_INL
#define PULSAR_FUN_BUF_INL 1

// Include: Pulsar -> Function
#include "pulsar/function.hpp"

// Pulsar
namespace pul
{
	/// FUNCTION: Buffer -> Base Impl
	/// Constructor
	template <typename _FunTy, typename _RetTy, typename... _Args>
	pf_decl_inline pf_decl_constexpr __fun_buf_base_impl<_FunTy, _RetTy, _Args...>::__fun_buf_base_impl(
			_FunTy &&__f) pf_attr_noexcept
			: fun_(__f)
	{}

	/// Operator()
	template <typename _FunTy, typename _RetTy, typename... _Args>
	pf_decl_inline pf_decl_constexpr _RetTy __fun_buf_base_impl<_FunTy, _RetTy, _Args...>::operator()(
			_Args &&...__args) const
	{
		return this->fun_(std::forward<_Args>(__args)...);
	}

	/// FUNCTION: Buffer
	/// Constructors
	template <typename _RetTy, typename... _Args>
	pf_decl_inline pf_decl_constexpr fun_buf<_RetTy(_Args...)>::fun_buf() pf_attr_noexcept
			: base_{ '\0' }
	{}
	template <typename _RetTy, typename... _Args>
	pf_decl_inline pf_decl_constexpr fun_buf<_RetTy(_Args...)>::fun_buf(
			std::nullptr_t) pf_attr_noexcept
			: fun_buf()
	{}
	template <typename _RetTy, typename... _Args>
	pf_decl_inline pf_decl_constexpr fun_buf<_RetTy(_Args...)>::fun_buf(
			fun_buf<_RetTy(_Args...)> const &__r) pf_attr_noexcept
			: base_(__r.base_)
	{}
	template <typename _RetTy, typename... _Args>
	pf_decl_inline pf_decl_constexpr fun_buf<_RetTy(_Args...)>::fun_buf(
			fun_buf<_RetTy(_Args...)> &&__r) pf_attr_noexcept
			: base_(__r.base_)
	{
		__r.base_ = { '\0' };
	}
	template <typename _RetTy, typename... _Args>
	template <typename _FunTy>
	pf_decl_inline pf_decl_constexpr fun_buf<_RetTy(_Args...)>::fun_buf(
			_FunTy &&__ptr) pf_attr_noexcept
			requires(std::is_invocable_r_v<_RetTy, _FunTy, _Args...>)
			: fun_buf()
	{
		union
		{
			byte_t *as_byte;
			__fun_buf_base_impl<std::decay_t<_FunTy>, _RetTy, _Args...> *as_fun_base;
		};
		as_byte = this->base_.data();
		std::construct_at(as_fun_base, std::move(__ptr));
	}

	/// Destructor
	template <typename _RetTy, typename... _Args>
	pf_decl_inline pf_decl_constexpr fun_buf<_RetTy(_Args...)>::~fun_buf() pf_attr_noexcept
	{
		union
		{
			byte_t *as_byte;
			__fun_buf_base<_RetTy, _Args...> *as_base;
		};
		as_byte = this->base_.data();
		std::destroy_at(as_base);
	}

	/// Operator()
	template <typename _RetTy, typename... _Args>
	template <typename... _InArgs>
	pf_decl_inline pf_decl_constexpr _RetTy fun_buf<_RetTy(_Args...)>::operator()(
			_InArgs &&...__args) const
			requires(std::is_invocable_v<_RetTy(_Args...), _InArgs...>)
	{
		union
		{
			const byte_t *as_byte;
			const __fun_buf_base<_RetTy, _Args...> *as_base;
		};
		as_byte = this->base_.data();
		return as_base->operator()(std::forward<_InArgs>(__args)...);
	}

	/// Operator==
	template <typename _RetTy, typename... _Args>
	pf_decl_inline pf_decl_constexpr bool fun_buf<_RetTy(_Args...)>::operator==(
			fun_buf<_RetTy(_Args...)> const &__r) const pf_attr_noexcept
	{
		return this->base_ == __r.base_;
	}
}

#endif // !PULSAR_FUN_BUF_INL