/*! @file   function.hpp
 *  @author Louis-Quentin Noé (noe.louis-quentin@hotmail.fr)
 *  @brief  Adds function utilities.
 *  @date   16-06-2022
 *
 *  @copyright Copyright (c) 2023 - Pulsar Software
 *
 *  @since 0.1.2
 */

#ifndef PULSAR_FUNCTION_HPP
#define PULSAR_FUNCTION_HPP 1

// Include: Pulsar
#include "pulsar/pulsar.hpp"

// Pulsar
namespace pul
{
	/*
	 * !          !
	 *   Function
	 * !          !
	 */
	/// FUNCTION: Pointer
	/*! @brief Function pointer.
	 *
	 *  @tparam _FunTy Encapsulated function type.
	 */
	template <typename _FunTy>
	class fun_ptr;

	/*! @brief Function pointer specialization.
	 *
	 *  @tparam _RetTy  Type of return
	 *  @tparam _Args Type of arguments.
	 */
	template <typename _RetTy, typename ..._Args>
	class fun_ptr<_RetTy(_Args ...)>
	{
	public:
		using return_type = _RetTy;

		/// Constructors
		/*! @brief Default constructor.
		 */
		pf_decl_inline pf_decl_constexpr fun_ptr() pf_attr_noexcept
			: ptr_(nullptr)
		{}
		/*! @brief Nullptr constructor.
		 */
		pf_decl_inline pf_decl_constexpr fun_ptr(
			nullptr_t) pf_attr_noexcept
			: fun_ptr()
		{}
		/*! @brief Constructor.
		 *
		 *  @param[in] __ptr Pointer of function.
		 */
		pf_decl_inline pf_decl_constexpr fun_ptr(
			_RetTy (*__ptr)(_Args ...)) pf_attr_noexcept
			: ptr_(__ptr)
		{}
		/*! @brief Copy constructor for Functors / Empty Lambdas types.
		 *
		 *  @tparam _FunTy Functor / Lambda type.
		 *  @param[in] __ptr Function to copy.
		 */
		template <typename _FunTy>
		pf_decl_inline pf_decl_constexpr fun_ptr(
			_FunTy &&__ptr) pf_attr_noexcept
		requires(std::is_convertible_v<_FunTy, _RetTy (*)(_Args ...)>)
			: ptr_(__ptr)
		{}
		/*! @brief Copy constructor.
		 *
		 *  @param[in] __r Other function pointer.
		 */
		pf_decl_inline pf_decl_constexpr fun_ptr(
			fun_ptr<_RetTy(_Args ...)> const &__r) pf_attr_noexcept
			: ptr_(__r.ptr_)
		{}
		/*! @brief Move constructor.
		 *
		 *  @param[in] __r Other function pointer.
		 */
		pf_decl_inline pf_decl_constexpr fun_ptr(
			fun_ptr<_RetTy(_Args ...)> &&__r) pf_attr_noexcept
			: ptr_(__r.ptr_)
		{
			__r.ptr_ = nullptr;
		}

		/// Operator=
		/*! @brief Copy assignment operator.
		 *
		 *  @param[in] __r Other function pointer.
		 *  @return Reference on this function pointer.
		 */
		pf_decl_inline pf_decl_constexpr fun_ptr<_RetTy(_Args ...)> &operator =(
			fun_ptr<_RetTy(_Args ...)> const &__r) pf_attr_noexcept
		{
			this->ptr_ = __r.ptr_;
			return *this;
		}
		/*! @brief Move assignment operator.
		 *
		 *  @param[in] __r Other function pointer.
		 *  @return Reference on this function pointer.
		 */
		pf_decl_inline pf_decl_constexpr fun_ptr<_RetTy(_Args ...)> &operator =(
			fun_ptr<_RetTy(_Args ...)> &&__r) pf_attr_noexcept
		{
			this->ptr_ = __r.ptr_;
			__r.ptr_	 = nullptr;
			return *this;
		}

		/// Operator()
		/*! @brief Call operator.
		 *
		 *  @param[in] __args Parameters to be sent to the encapsulated function pointer.
		 *  @return Value returned by the encapsulated function pointer.
		 */
		template <typename ..._InArgs>
		pf_decl_inline pf_decl_constexpr _RetTy operator ()(
			_InArgs &&... __args) const
		{
			return this->ptr_(__args ...);
		}

		/// Operator==
		/*! @brief Equality operator.
		 *
		 *  @param[in] __r Other function pointer.
		 *  @return True If both pointer are equals.
		 *  @return False Otherwise.
		 */
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr bool operator ==(
			fun_ptr<_RetTy(_Args ...)> const &__r) const pf_attr_noexcept
		{
			return this->ptr_ = __r.ptr_;
		}

		/// Operator (Bool)
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr
		operator bool() const pf_attr_noexcept
		{
			return this->ptr_ != nullptr;
		}

	private:
		_RetTy (* ptr_)(_Args ...);
	};

	/// FUNCTION: Pointer Deduction Guide (Based on std::function)
	/*! @brief Structure to help deducing a function signature type.
	 */
	template <
		typename>
	struct __fun_helper
	{};

	/*! @brief Deduction specialization for non-constant non-referred methods.
	 *
	 *  @tparam _Res  Return type.
	 *  @tparam _Tp   Type of class containing the method.
	 *  @tparam _Nx	  Is noexcept.
	 *  @tparam _Args Type of function's arguments.
	 */
	template <
		typename _Res,
		typename _Tp,
		bool _Nx,
		typename ..._Args>
	struct __fun_helper<
		_Res (_Tp::*)(_Args ...) noexcept(_Nx)>
	{
		using type = _Res(_Args ...);
	};

	/*! @brief Deduction specialization for non-constant referred methods.
	 *
	 *  @tparam _Res  Return type.
	 *  @tparam _Tp   Type of class containing the method.
	 *  @tparam _Nx	  Is noexcept.
	 *  @tparam _Args Type of function's arguments.
	 */
	template <
		typename _Res,
		typename _Tp,
		bool _Nx,
		typename ..._Args>
	struct __fun_helper<
		_Res (_Tp::*)(_Args ...) &noexcept(_Nx)>
	{
		using type = _Res(_Args ...);
	};

	/*! @brief Deduction specialization for constant non-referred methods.
	 *
	 *  @tparam _Res  Return type.
	 *  @tparam _Tp   Type of class containing the method.
	 *  @tparam _Nx	  Is noexcept.
	 *  @tparam _Args Type of function's arguments.
	 */
	template <
		typename _Res,
		typename _Tp,
		bool _Nx,
		typename ..._Args>
	struct __fun_helper<
		_Res (_Tp::*)(_Args ...) const noexcept(_Nx)>
	{
		using type = _Res(_Args ...);
	};

	/*! @brief Deduction specialization for constant referred methods.
	 *
	 *  @tparam _Res  Return type.
	 *  @tparam _Tp   Type of class containing the method.
	 *  @tparam _Nx	  Is noexcept.
	 *  @tparam _Args Type of function's arguments.
	 */
	template <
		typename _Res,
		typename _Tp,
		bool _Nx,
		typename ..._Args>
	struct __fun_helper<
		_Res (_Tp::*)(_Args ...) const &noexcept(_Nx)>
	{
		using type = _Res(_Args ...);
	};

	/*! @brief Deduction guide for function pointers.
	 *
	 *  @tparam _Res 		  Return type.
	 *  @tparam _ArgTypes Type of function's arguments.
	 */
	template <
		typename _Res,
		typename ..._ArgTypes>
	fun_ptr(_Res (*)(_ArgTypes...))->fun_ptr<_Res(_ArgTypes...)>;

	/*! @brief Deduction guide for fonctors / methods.
	 *
	 *  @tparam _Functor 	 Functor type.
	 *  @tparam _Signature Encapsulated function type.
	 */
	template <
		typename _Functor,
		typename _Signature = typename __fun_helper<decltype(&_Functor::operator ())>::type>
	fun_ptr(_Functor)->fun_ptr<_Signature>;

	/// FUNCTION: Buffer -> Base
	template<typename _RetTy, typename ... _Args>
	class __fun_buf_base
	{
	public:
		/// Destructor
		pf_decl_constexpr pf_decl_virtual ~__fun_buf_base() = default;

		/// Operator()
		pf_decl_constexpr pf_decl_virtual _RetTy
		operator()(
			_Args &&...) const = 0;
	};

	/// FUNCTION: Buffer -> Base Impl
	template<typename _FunTy, typename _RetTy, typename ... _Args>
	class __fun_buf_base_impl : public __fun_buf_base<_RetTy, _Args...>
	{
	public:
		/// Constructors
		pf_decl_constexpr
		__fun_buf_base_impl(
			_FunTy &&__f) pf_attr_noexcept
			: fun_(__f)
		{}
		__fun_buf_base_impl(__fun_buf_base_impl const &) = delete;
		__fun_buf_base_impl(__fun_buf_base_impl &&)			 = delete;

		/// Destructor
		pf_decl_constexpr ~__fun_buf_base_impl() pf_attr_noexcept = default;

		/// Operator()
		pf_decl_constexpr _RetTy
		operator()(
			_Args &&... __args) const pf_attr_override
		{
			return this->fun_(std::forward<_Args>(__args)...);
		}

	private:
		_FunTy fun_;
	};

	/// FUNCTION: Buffer
	template<typename _FunTy>
	class fun_buf;
	template<typename _RetTy, typename ... _Args>
	class fun_buf<_RetTy(_Args...)> pf_attr_final
	{
	public:
		/// Constructors
		pf_decl_constexpr
		fun_buf() pf_attr_noexcept
			: base_{ '\0' }
		{}
		pf_decl_constexpr
		fun_buf(
			nullptr_t) pf_attr_noexcept
			: fun_buf()
		{}
		pf_decl_constexpr
		fun_buf(
			fun_buf<_RetTy(_Args...)> const &__r) pf_attr_noexcept
			: base_(__r.base_)
		{}
		pf_decl_constexpr
		fun_buf(
			fun_buf<_RetTy(_Args...)> &&__r) pf_attr_noexcept
			: base_(__r.base_)
		{
			__r.base_ = { '\0' };
		}
		template<typename _FunTy>
		pf_decl_constexpr
		fun_buf(
			_FunTy &&__ptr) pf_attr_noexcept
		requires(std::is_invocable_r_v<_RetTy, _FunTy, _Args...>)
			: fun_buf()
		{
			union
			{
				byte_t *as_byte;
				__fun_buf_base_impl<std::decay_t<_FunTy>, _RetTy, _Args...> *as_fun_base;
			};
			as_byte = data(this->base_);
			construct(as_fun_base, std::move(__ptr));
		}

		/// Destructor
		pf_decl_constexpr ~fun_buf() pf_attr_noexcept
		{
			union
			{
				byte_t *as_byte;
				__fun_buf_base<_RetTy, _Args...> *as_base;
			};
			as_byte = data(this->base_);
			destroy(as_base);
		}

		/// Operator()
		template<typename ... _InArgs>
		pf_decl_constexpr _RetTy
		operator()(
			_InArgs &&... __args) const
		requires(std::is_invocable_v<_RetTy (_Args...), _InArgs...>)
		{
			union
			{
				const byte_t *as_byte;
				const __fun_buf_base<_RetTy, _Args...> *as_base;
			};
			as_byte = data(this->base_);
			return as_base->operator()(std::forward<_InArgs>(__args)...);
		}

		/// Operator==
		pf_decl_constexpr bool
		operator==(
			fun_buf<_RetTy(_Args...)> const &__r) const pf_attr_noexcept
		{
			return this->base_ == __r.base_;
		}

	private:
		byte_t base_[2 * sizeof(__fun_buf_base<_RetTy, _Args...>)];
	};

	/*! @brief Deduction guide for function pointers.
	 *
	 *  @tparam _Res 		  Return type.
	 *  @tparam _ArgTypes Type of function's arguments.
	 */
	template<
		typename _Res,
		typename ... _ArgTypes>
	fun_buf(_Res (*)(_ArgTypes...))->fun_buf<_Res(_ArgTypes...)>;

	/*! @brief Deduction guide for fonctors / methods.
	 *
	 *  @tparam _Functor 	 Functor type.
	 *  @tparam _Signature Encapsulated function type.
	 */
	template<
		typename _Functor,
		typename _Signature = typename __fun_helper<decltype(&_Functor::operator())>::type>
	fun_buf(_Functor)->fun_buf<_Signature>;
}


#endif // !PULSAR_FUNCTION_HPP