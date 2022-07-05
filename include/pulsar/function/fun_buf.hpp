/*! @file   fun_buf.hpp
 *  @author Fluffy (noe.louis-quentin@hotmail.fr)
 *  @brief
 *  @date   03-07-2022
 *
 *  @copyright Copyright (c) 2022 - Pulsar Software
 *
 *  @since 0.1.2
 */

#ifndef PULSAR_FUN_BUF_HPP
#define PULSAR_FUN_BUF_HPP 1

// Include: Pulsar -> Function
#include "pulsar/function/fun_ptr.hpp"

// Include: C++
#include <array>
#include <functional>


// Pulsar
namespace pul
{
	/// FUNCTION: Buffer -> Base
	template <typename _Ret, typename... _Args>
	class __fun_buf_base
	{
	public:
		/// Destructor
		pf_decl_constexpr pf_decl_virtual ~__fun_buf_base() = default;

		/// Operator()
		pf_decl_constexpr pf_decl_virtual _Ret operator()(
				_Args &&...) const = 0;
	};

	/// FUNCTION: Buffer -> Base Impl
	template <typename _Fun, typename _Ret, typename... _Args>
	class __fun_buf_base_impl: public __fun_buf_base<_Ret, _Args...>
	{
	public:
		/// Constructor
		pf_decl_constexpr __fun_buf_base_impl(
				_Fun &&__f)
				: fun_(__f)
		{}

		/// Operator()
		pf_decl_constexpr _Ret operator()(
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
	template <typename _Ret, typename... _Args>
	class fun_buf<_Ret(_Args...)> pf_attr_final
	{
	public:
		/// Constructors
		pf_decl_constexpr fun_buf() pf_attr_noexcept
				: base_{ '\0' }
		{}
		pf_decl_constexpr fun_buf(
				std::nullptr_t) pf_attr_noexcept
				: fun_buf()
		{}
		template <typename _Fun>
		pf_decl_constexpr fun_buf(
				_Fun &&__f)
				: fun_buf()
		{
			union
			{
				byte_t *as_byte;
				void *as_void;
			};
			as_byte = this->base_.data();
			::new (as_void) __fun_buf_base_impl<std::decay_t<_Fun>, _Ret, _Args...>(std::move(__f));
		}
		pf_decl_constexpr fun_buf(
				fun_buf<_Ret(_Args...)> const &__r) pf_attr_noexcept
				: base_(__r.base_)
		{}
		pf_decl_constexpr fun_buf(
				fun_buf<_Ret(_Args...)> &&__r) pf_attr_noexcept
				: base_(__r.base_)
		{
			__r.base_ = { '\0' };
		}

		/// Destructor
		pf_decl_constexpr ~fun_buf() pf_attr_noexcept
		{
			if (std::any_of(
							this->base_.begin(),
							this->base_.end(),
							[](byte_t c)
							{ return c != '\0'; }))
			{
				union
				{
					byte_t *as_byte;
					__fun_buf_base<_Ret, _Args...> *as_base;
				};
				as_byte = this->base_.data();
				std::destroy_at(as_base);
			}
		}

		/// Operator()
		pf_decl_constexpr _Ret operator()(
				_Args &&...__args) const
		{
			union
			{
				const byte_t *as_byte;
				const __fun_buf_base<_Ret, _Args...> *as_base;
			};
			as_byte = this->base_.data();
			return as_base->operator()(
					std::forward<_Args>(__args)...);
		}

		/// Operator==
		pf_decl_constexpr bool operator==(
				fun_buf<_Ret(_Args...)> const &__r) const pf_attr_noexcept
		{
			return this->base_ == __r.base_;
		}

	private:
		std::array<byte_t, sizeof(__fun_buf_base<_Ret, _Args...>)> base_;
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