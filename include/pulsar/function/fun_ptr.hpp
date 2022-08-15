/*! @file   fun_ptr.hpp
 *  @author Fluffy (noe.louis-quentin@hotmail.fr)
 *  @brief	Definition of function pointer.
 *  @date   23-05-2022
 *
 *  @copyright Copyright (c) 2022 - Pulsar Software
 *
 *  @since 0.1.0
 */

#ifndef PULSAR_FUN_PTR_HPP
#define PULSAR_FUN_PTR_HPP 1

// Include: Pulsar
#include "pulsar/pulsar.hpp"

// Include: C++
#include <utility>

// Pulsar
namespace pul
{

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
	template <typename _RetTy, typename... _Args>
	class fun_ptr<_RetTy(_Args...)>
	{
	public:
		using return_t = _RetTy;

		/// Constructors
		/*! @brief Default constructor.
		 */
		pf_decl_inline pf_decl_constexpr fun_ptr() pf_attr_noexcept;
		/*! @brief Nullptr constructor.
		 */
		pf_decl_inline pf_decl_constexpr fun_ptr(
				std::nullptr_t) pf_attr_noexcept;
		/*! @brief Constructor.
		 *
		 *  @param[in] __ptr Pointer of function.
		 */
		pf_decl_inline pf_decl_constexpr fun_ptr(
				_RetTy (*__ptr)(_Args...)) pf_attr_noexcept;
		/*! @brief Copy constructor for Functors / Empty Lambdas types.
		 *
		 *  @tparam _FunTy Functor / Lambda type.
		 *  @param[in] __ptr Function to copy.
		 */
		template <typename _FunTy>
		pf_decl_inline pf_decl_constexpr fun_ptr(
				_FunTy &&__ptr) pf_attr_noexcept
				requires(std::is_convertible_v<_FunTy, _RetTy (*)(_Args...)>);
		/*! @brief Copy constructor.
		 *
		 *  @param[in] __r Other function pointer.
		 */
		pf_decl_inline pf_decl_constexpr fun_ptr(
				fun_ptr<_RetTy(_Args...)> const &__r) pf_attr_noexcept;
		/*! @brief Move constructor.
		 *
		 *  @param[in] __r Other function pointer.
		 */
		pf_decl_inline pf_decl_constexpr fun_ptr(
				fun_ptr<_RetTy(_Args...)> &&__r) pf_attr_noexcept;

		/// Operator=
		/*! @brief Copy assignment operator.
		 *
		 *  @param[in] __r Other function pointer.
		 *  @return Reference on this function pointer.
		 */
		pf_decl_inline pf_decl_constexpr fun_ptr<_RetTy(_Args...)> &operator=(
				fun_ptr<_RetTy(_Args...)> const &__r) pf_attr_noexcept;
		/*! @brief Move assignment operator.
		 *
		 *  @param[in] __r Other function pointer.
		 *  @return Reference on this function pointer.
		 */
		pf_decl_inline pf_decl_constexpr fun_ptr<_RetTy(_Args...)> &operator=(
				fun_ptr<_RetTy(_Args...)> &&__r) pf_attr_noexcept;

		/// Operator()
		/*! @brief Call operator.
		 *
		 *  @param[in] __args Parameters to be sent to the encapsulated function pointer.
		 *  @return Value returned by the encapsulated function pointer.
		 */
		template <typename... _InArgs>
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr _RetTy operator()(
				_InArgs &&...__args) const;

		/// Operator==
		/*! @brief Equality operator.
		 *
		 *  @param[in] __r Other function pointer.
		 *  @return True If both pointer are equals.
		 *  @return False Otherwise.
		 */
		pf_decl_inline pf_decl_constexpr bool operator==(
				fun_ptr<_RetTy(_Args...)> const &__r) const pf_attr_noexcept;

	private:
		_RetTy (*ptr_)(_Args...);
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
			typename... _Args>
	struct __fun_helper<
			_Res (_Tp::*)(_Args...) noexcept(_Nx)>
	{
		using type = _Res(_Args...);
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
			typename... _Args>
	struct __fun_helper<
			_Res (_Tp::*)(_Args...) &noexcept(_Nx)>
	{
		using type = _Res(_Args...);
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
			typename... _Args>
	struct __fun_helper<
			_Res (_Tp::*)(_Args...) const noexcept(_Nx)>
	{
		using type = _Res(_Args...);
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
			typename... _Args>
	struct __fun_helper<
			_Res (_Tp::*)(_Args...) const &noexcept(_Nx)>
	{
		using type = _Res(_Args...);
	};

	/*! @brief Deduction guide for function pointers.
	 *
	 *  @tparam _Res 		  Return type.
	 *  @tparam _ArgTypes Type of function's arguments.
	 */
	template <
			typename _Res,
			typename... _ArgTypes>
	fun_ptr(_Res (*)(_ArgTypes...)) -> fun_ptr<_Res(_ArgTypes...)>;

	/*! @brief Deduction guide for fonctors / methods.
	 *
	 *  @tparam _Functor 	 Functor type.
	 *  @tparam _Signature Encapsulated function type.
	 */
	template <
			typename _Functor,
			typename _Signature = typename __fun_helper<decltype(&_Functor::operator())>::type>
	fun_ptr(_Functor) -> fun_ptr<_Signature>;
}

// Include: Pulsar -> Function -> Impl
#include "pulsar/function/fun_ptr.inl"

#endif // !PULSAR_FUN_PTR_HPP