/*! @file   fun_ptr.inl
 *  @author Fluffy (noe.louis-quentin@hotmail.fr)
 *  @brief  Implementation of function pointer.
 *  @date   11-08-2022
 *
 *  @copyright Copyright (c) 2022 - Pulsar Software
 *
 *  @since 0.1.0
 */

#ifndef PULSAR_FUN_PTR_INL
#define PULSAR_FUN_PTR_INL 1

// Include: Pulsar -> Function
#include "pulsar/function.hpp"

// Pulsar
namespace pul
{
	/// FUNCTION: Pointer
	/// Constructors
	template <typename _RetTy, typename... _Args>
	pf_decl_inline pf_decl_constexpr fun_ptr<_RetTy(_Args...)>::fun_ptr() pf_attr_noexcept
			: ptr_(nullptr)
	{}
	template <typename _RetTy, typename... _Args>
	pf_decl_inline pf_decl_constexpr fun_ptr<_RetTy(_Args...)>::fun_ptr(
			std::nullptr_t) pf_attr_noexcept
			: fun_ptr()
	{}
	template <typename _RetTy, typename... _Args>
	pf_decl_inline pf_decl_constexpr fun_ptr<_RetTy(_Args...)>::fun_ptr(
			_RetTy (*__ptr)(_Args...)) pf_attr_noexcept
			: ptr_(__ptr)
	{}
	template <typename _RetTy, typename... _Args>
	template <typename _FunTy>
	pf_decl_inline pf_decl_constexpr fun_ptr<_RetTy(_Args...)>::fun_ptr(
			_FunTy &&__ptr) pf_attr_noexcept
			requires(std::is_convertible_v<_FunTy, _RetTy (*)(_Args...)>)
			: ptr_(__ptr)
	{}
	template <typename _RetTy, typename... _Args>
	pf_decl_inline pf_decl_constexpr fun_ptr<_RetTy(_Args...)>::fun_ptr(
			fun_ptr<_RetTy(_Args...)> const &__r) pf_attr_noexcept
			: ptr_(__r.ptr_)
	{}
	template <typename _RetTy, typename... _Args>
	pf_decl_inline pf_decl_constexpr fun_ptr<_RetTy(_Args...)>::fun_ptr(
			fun_ptr<_RetTy(_Args...)> &&__r) pf_attr_noexcept
			: ptr_(__r.ptr_)
	{
		__r.ptr_ = nullptr;
	}

	/// Operator=
	template <typename _RetTy, typename... _Args>
	pf_decl_inline pf_decl_constexpr fun_ptr<_RetTy(_Args...)> &fun_ptr<_RetTy(_Args...)>::operator=(
			fun_ptr<_RetTy(_Args...)> const &__r) pf_attr_noexcept
	{
		this->ptr_ = __r.ptr_;
		return *this;
	}
	template <typename _RetTy, typename... _Args>
	pf_decl_inline pf_decl_constexpr fun_ptr<_RetTy(_Args...)> &fun_ptr<_RetTy(_Args...)>::operator=(
			fun_ptr<_RetTy(_Args...)> &&__r) pf_attr_noexcept
	{
		this->ptr_ = __r.ptr_;
		__r.ptr_	 = nullptr;
		return *this;
	}

	/// Operator()
	template <typename _RetTy, typename... _Args>
	template <typename... _InArgs>
	pf_hint_nodiscard pf_decl_inline pf_decl_constexpr _RetTy fun_ptr<_RetTy(_Args...)>::operator()(
			_InArgs &&...__args) const
	{
		return this->ptr_(__args...);
	}

	/// Operator==
	template <typename _RetTy, typename... _Args>
	pf_decl_inline pf_decl_constexpr bool fun_ptr<_RetTy(_Args...)>::operator==(
			fun_ptr<_RetTy(_Args...)> const &__r) const pf_attr_noexcept
	{
		return this->ptr_ = __r.ptr_;
	}

}

#endif // !PULSAR_FUN_PTR_INL