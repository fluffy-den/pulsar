/*! @file   fun_ptr.hpp
 *  @author Fluffy (noe.louis-quentin@hotmail.fr)
 *  @brief
 *  @date   23-05-2022
 *
 *  @copyright Copyright (c) 2022 - Pulsar Software
 *
 *  @since 0.1.0
 */

#ifndef PULSAR_UTILITY_FUN_PTR_HPP
#define PULSAR_UTILITY_FUN_PTR_HPP 1

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
	 *  @tparam _Fun Encapsulated function type.
	 */
	template <typename _Fun>
	class fun_ptr;

	/*! @brief Function pointer specialization.
	 *
	 *  @tparam _Ret  Type of return
	 *  @tparam _Args Type of arguments.
	 */
	template <typename _Ret, typename... _Args>
	class fun_ptr<_Ret(_Args...)>
	{
	public:
		using return_t = _Ret;

		/// Constructors
		/*! @brief Default constructor.
		 */
		pf_decl_constexpr fun_ptr() pf_attr_noexcept
				: ptr_(nullptr)
		{}

		/*! @brief Constructor.
		 *
		 *  @param[in] __ptr Pointer of function.
		 */
		pf_decl_constexpr fun_ptr(
				_Ret (*__ptr)(_Args...)) pf_attr_noexcept
				: ptr_(__ptr)
		{}
		/*! @brief Constructor for functors.
		 *
		 *  @tparam _Fun Functor type.
		 *  @param[in] __ptr Function to copy.
		 */
		template <typename _Fun>
		pf_decl_constexpr fun_ptr(
				_Fun &&__ptr) pf_attr_noexcept
				: ptr_(std::move(__ptr))
		{}
		/*! @brief Copy constructor.
		 *
		 *  @param[in] __r Other function pointer.
		 */
		pf_decl_constexpr fun_ptr(
				fun_ptr<_Ret(_Args...)> const &__r) pf_attr_noexcept
				: ptr_(__r.ptr_)
		{}

		/// Operator=
		/*! @brief Assignment operator for functors.
		 *
		 *  @tparam _Fun Functor type.
		 *  @param[in] __ptr Function to copy.
		 *  @return This function pointer.
		 */
		template <typename _Fun>
		pf_decl_constexpr fun_ptr<_Ret(_Args...)> &operator=(
				_Fun &&__ptr) pf_attr_noexcept
		{
			this->ptr_ = __ptr;
		}
		/*! @brief Copy assignment operator.
		 *
		 *  @param[in] __r Other function pointer.
		 *  @return This function pointer.
		 */
		pf_decl_constexpr fun_ptr<_Ret(_Args...)> &operator=(
				fun_ptr<_Ret(_Args...)> const &__r) pf_attr_noexcept
		{
			this->ptr_ = __r.ptr_;
			return *this;
		}

		/// Operator()
		/*! @brief Call operator.
		 *
		 *  @param[in] __args Parameters to be sent to the encapsulated function pointer.
		 *  @return Value returned by the encapsulated function pointer.
		 */
		pf_decl_constexpr _Ret operator()(
				_Args &&...__args) const pf_attr_noexcept
		{
			return this->ptr_(std::forward<_Args>(__args)...);
		}

		/// Operator==
		/*! @brief Equality operator.
		 *
		 *  @param[in] __r Other function pointer.
		 *  @return True If both pointer are equals.
		 *  @return False Otherwise.
		 */
		pf_decl_constexpr bool operator==(
				fun_ptr<_Ret(_Args...)> const &__r) const pf_attr_noexcept
		{
			return this->ptr_ = __r.ptr_;
		}

	private:
		_Ret (*ptr_)(_Args...);
	};

	/// FUNCTION: Pointer Deduction Guide (Based on std::function)
	/*! @brief Structure to help deducing a function signature type.
	 */
	template <
			typename>
	struct __fun_ptr_helper
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
	struct __fun_ptr_helper<
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
	struct __fun_ptr_helper<
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
	struct __fun_ptr_helper<
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
	struct __fun_ptr_helper<
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
			typename _Signature = typename __fun_ptr_helper<decltype(&_Functor::operator())>::type>
	fun_ptr(_Functor) -> fun_ptr<_Signature>;
}

#endif // !PULSAR_UTILITY_FUN_PTR_HPP