/*! @file   fun_buf.hpp
 *  @author Fluffy (noe.louis-quentin@hotmail.fr)
 *  @brief	Definition of function buffer.
 *  @date   03-07-2022
 *
 *  @copyright Copyright (c) 2022 - Pulsar Software
 *
 *  @since 0.1.2
 */

#ifndef PULSAR_FUN_BUF_HPP
#define PULSAR_FUN_BUF_HPP 1

// Include: Pulsar
#include "pulsar/function/fun_ptr.hpp"

// Include: C++
#include <array>
#include <functional>

// Pulsar
namespace pul
{
	/// FUNCTION: Buffer -> Base
	template <typename _RetTy, typename... _Args>
	class __fun_buf_base
	{
	public:
		/// Destructor
		pf_decl_inline pf_decl_constexpr pf_decl_virtual ~__fun_buf_base() = default;

		/// Operator()
		pf_decl_inline pf_decl_constexpr pf_decl_virtual _RetTy operator()(
				_Args &&...) const = 0;
	};

	/// FUNCTION: Buffer -> Base Impl
	template <typename _Fun, typename _RetTy, typename... _Args>
	class __fun_buf_base_impl: public __fun_buf_base<_RetTy, _Args...>
	{
	public:
		/// Constructors
		pf_decl_inline pf_decl_constexpr __fun_buf_base_impl(_Fun &&__f) pf_attr_noexcept
				: fun_(__f)
		{}
		__fun_buf_base_impl(__fun_buf_base_impl const &) = delete;
		__fun_buf_base_impl(__fun_buf_base_impl &&)			 = delete;

		/// Destructor
		pf_decl_inline pf_decl_constexpr ~__fun_buf_base_impl() pf_attr_noexcept = default;

		/// Operator()
		pf_decl_inline pf_decl_constexpr _RetTy operator()(
				_Args &&...__args) const pf_attr_override
		{
			return this->fun_(std::forward<_Args>(__args)...);
		}

	private:
		_Fun fun_;
	};

	/// FUNCTION: Buffer
	template <typename _Fun>
	class fun_buf;
	template <typename _RetTy, typename... _Args>
	class fun_buf<_RetTy(_Args...)> pf_attr_final
	{
	public:
		/// Constructors
		pf_decl_inline pf_decl_constexpr fun_buf() pf_attr_noexcept
				: base_{ '\0' }
		{}
		pf_decl_inline pf_decl_constexpr fun_buf(
				std::nullptr_t) pf_attr_noexcept
				: fun_buf()
		{}
		pf_decl_inline pf_decl_constexpr fun_buf(
				fun_buf<_RetTy(_Args...)> const &__r) pf_attr_noexcept
				: base_(__r.base_)
		{}
		pf_decl_inline pf_decl_constexpr fun_buf(
				fun_buf<_RetTy(_Args...)> &&__r) pf_attr_noexcept
				: base_(__r.base_)
		{
			__r.base_ = { '\0' };
		}
		template <typename _FunTy>
		pf_decl_inline pf_decl_constexpr fun_buf(
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
		pf_decl_inline pf_decl_constexpr ~fun_buf() pf_attr_noexcept
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
		template <typename... _InArgs>
		pf_decl_inline pf_decl_constexpr _RetTy operator()(
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
		pf_decl_inline pf_decl_constexpr bool operator==(
				fun_buf<_RetTy(_Args...)> const &__r) const pf_attr_noexcept
		{
			return this->base_ == __r.base_;
		}

	private:
		std::array<byte_t, 2 * sizeof(__fun_buf_base<_RetTy, _Args...>)> base_;
	};

	/*! @brief Deduction guide for function pointers.
	 *
	 *  @tparam _Res 		  Return type.
	 *  @tparam _ArgTypes Type of function's arguments.
	 */
	template <
			typename _Res,
			typename... _ArgTypes>
	fun_buf(_Res (*)(_ArgTypes...)) -> fun_buf<_Res(_ArgTypes...)>;

	/*! @brief Deduction guide for fonctors / methods.
	 *
	 *  @tparam _Functor 	 Functor type.
	 *  @tparam _Signature Encapsulated function type.
	 */
	template <
			typename _Functor,
			typename _Signature = typename __fun_helper<decltype(&_Functor::operator())>::type>
	fun_buf(_Functor) -> fun_buf<_Signature>;
}

#endif // !PULSAR_FUN_BUF_HPP