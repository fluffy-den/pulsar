/*! @file   fun_signal.hpp
 *  @author Fluffy (noe.louis-quentin@hotmail.fr)
 *  @brief	Definition of function signals.
 *  @date   05-06-2022
 *
 *  @copyright Copyright (c) 2022 - Pulsar Software
 *
 *  @since 0.1.1
 */

#ifndef PULSAR_FUN_SIGNAL_HPP
#define PULSAR_FUN_SIGNAL_HPP 1

// Include: Pulsar
#include "pulsar/function/fun_ptr.hpp"
#include "pulsar/linked_list.hpp"

// Pulsar
namespace pul
{

	/// FUNCTION: Shared Output Signal
	template <
			typename _FunTy,
			template <typename> typename _ListTy = singly_linked_list>
	class shared_osignal
	{
		/// Signal -> Receiver
		class __receiver_t
		{
			pf_decl_friend shared_osignal<_FunTy, _ListTy>;

		public:
			/// Constructors
			pf_decl_inline pf_decl_constexpr __receiver_t(fun_ptr<_FunTy> const &__ptr) pf_attr_noexcept;
			__receiver_t(__receiver_t const &) pf_attr_noexcept = delete;
			pf_decl_inline pf_decl_constexpr __receiver_t(__receiver_t &&) pf_attr_noexcept;

			/// Destructor
			pf_decl_inline pf_decl_constexpr ~__receiver_t() pf_attr_noexcept;

			/// Transmitter -> Attached
			pf_hint_nodiscard pf_decl_inline pf_decl_constexpr shared_osignal<_FunTy> *attached_transmitter() const pf_attr_noexcept;

		private:
			shared_osignal<_FunTy, _ListTy> *transmitter_;
			fun_ptr<_FunTy> fun_;
		};

	public:
		using receiver = typename _ListTy<__receiver_t>::node;

		/// Constructors
		pf_decl_inline pf_decl_constexpr shared_osignal() pf_attr_noexcept			 = default;
		shared_osignal(shared_osignal<_FunTy, _ListTy> const &) pf_attr_noexcept = delete;
		pf_decl_inline pf_decl_constexpr shared_osignal(shared_osignal<_FunTy, _ListTy> &&__r) pf_attr_noexcept;

		/// Destructor
		pf_decl_inline pf_decl_constexpr ~shared_osignal() pf_attr_noexcept;

		/// Operator()
		template <typename... _InArgs>
		pf_decl_inline pf_decl_constexpr void operator()(
				_InArgs &...__args) const pf_attr_noexcept
				requires(std::is_invocable_v<_FunTy, _InArgs...>);

		/// Management
		pf_decl_inline pf_decl_constexpr void signal_add(
				receiver &__r) pf_attr_noexcept;
		pf_decl_inline pf_decl_constexpr void signal_rem(
				receiver &__r) pf_attr_noexcept;
		pf_decl_inline pf_decl_constexpr void signals_clear() pf_attr_noexcept;

	private:
		_ListTy<__receiver_t> list_;
	};

	/// FUNCTION: Shared Input Signal
	template <
			typename _FunTy,
			template <typename> typename _ListTy = singly_linked_list>
	using shared_isignal = typename shared_osignal<_FunTy, _ListTy>::receiver;

	/// FUNCTION: Unique Output Signal
	template <typename _FunTy>
	class unique_osignal
	{
		/// Signal -> Receiver
		class __receiver_t
		{
			pf_decl_friend unique_osignal<_FunTy>;

		public:
			/// Constructors
			pf_decl_inline pf_decl_constexpr __receiver_t(fun_ptr<_FunTy> const &__ptr) pf_attr_noexcept;
			__receiver_t(__receiver_t const &__r) pf_attr_noexcept = delete;
			pf_decl_inline pf_decl_constexpr __receiver_t(__receiver_t &&__r) pf_attr_noexcept;

			/// Destructor
			pf_decl_inline pf_decl_constexpr ~__receiver_t() pf_attr_noexcept;

			/// Transmitter -> Attached
			pf_hint_nodiscard pf_decl_inline pf_decl_constexpr unique_osignal<_FunTy> *attached_transmitter() const pf_attr_noexcept;

		private:
			unique_osignal<_FunTy> *transmitter_;
			fun_ptr<_FunTy> fun_;
		};

	public:
		using receiver = __receiver_t;

		/// Constructors
		pf_decl_inline pf_decl_constexpr unique_osignal() pf_attr_noexcept;
		unique_osignal(unique_osignal<_FunTy> const &) pf_attr_noexcept = delete;
		pf_decl_inline pf_decl_constexpr unique_osignal(unique_osignal<_FunTy> &&__r) pf_attr_noexcept;

		/// Destructor
		pf_decl_inline pf_decl_constexpr ~unique_osignal() pf_attr_noexcept;

		/// Operator()
		template <typename... _InArgs>
		pf_decl_inline pf_decl_constexpr void operator()(_InArgs &&...__args) const pf_attr_noexcept
				requires(std::is_invocable_v<_FunTy, _InArgs...>);

		/// Signals
		pf_decl_inline pf_decl_constexpr void signal_set(receiver &__r) pf_attr_noexcept;
		pf_decl_inline pf_decl_constexpr void signal_clear() pf_attr_noexcept;

	private:
		receiver *receiver_;
	};

	/// FUNCTION: Unique Input Signal
	template <typename _FunTy>
	using unique_isignal = typename unique_osignal<_FunTy>::receiver;
}

// Include: Pulsar -> Function
#include "pulsar/function/fun_signal.inl"

#endif // !PULSAR_FUN_SIGNAL_HPP