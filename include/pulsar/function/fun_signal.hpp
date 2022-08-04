/*! @file   fun_signal.hpp
 *  @author Fluffy (noe.louis-quentin@hotmail.fr)
 *  @brief
 *  @date   05-06-2022
 *
 *  @copyright Copyright (c) 2022 - Pulsar Software
 *
 *  @since 0.1.1
 */

#ifndef PULSAR_FUN_SIGNAL_HPP
#define PULSAR_FUN_SIGNAL_HPP 1

// Include: Pulsar
#include "pulsar/linked_list.hpp"

// Include: Pulsar -> Function
#include "pulsar/function/fun_ptr.hpp"

// Pulsar
namespace pul
{

	/// SIGNAL: Shared Output
	template <typename _FunTy>
	class shared_osignal
	{
		/// Signal -> Receiver
		class __receiver_t
		{
			pf_decl_friend shared_osignal<_FunTy>;

		public:
			/// Constructors
			pf_decl_constexpr __receiver_t(
					fun_ptr<_FunTy> const &__fun) pf_attr_noexcept
					: transmitter_(nullptr)
					, fun_(__fun)
			{}
			pf_decl_constexpr __receiver_t(
					__receiver_t const &__r) pf_attr_noexcept = delete;
			pf_decl_constexpr __receiver_t(
					__receiver_t &&__r) pf_attr_noexcept = delete;

			/// Destructor
			pf_decl_constexpr __receiver_t() pf_attr_noexcept
			{
				this->transmitter_->rem(*this);
			}

			/// Transmitter -> Attached
			pf_hint_nodiscard pf_decl_constexpr shared_osignal<_FunTy> *attached_transmitter() const pf_attr_noexcept
			{
				return this->transmitter_;
			}

		private:
			shared_osignal<_FunTy> *transmitter_;
			fun_ptr<_FunTy> fun_;
		};

	public:
		using receiver = singly_node<__receiver_t>;

		/// Constructors
		pf_decl_constexpr shared_osignal() pf_attr_noexcept
		{}
		pf_decl_constexpr shared_osignal(
				shared_osignal<_FunTy> const &__r) pf_attr_noexcept = delete;
		pf_decl_constexpr shared_osignal(
				shared_osignal<_FunTy> &&__r)
		{
			this->clear();
			this->list_ = std::move(__r.list_);
			for (auto b = this->list_.begin(), e = this->list_.end(); b != e; ++b)
			{
				b->transmitter_ = this;
			}
		}

		/// Destructor
		pf_decl_constexpr ~shared_osignal() pf_attr_noexcept
		{
			this->clear();
		}

		/// Operator()
		template <typename... _InArgs>
		pf_decl_constexpr void operator()(
				_InArgs &...__args) const pf_attr_noexcept
		{
			for (auto b = this->list_.begin(), e = this->list_.end(); b != e; ++b)
			{
				b->fun_(std::forward<_InArgs>(__args)...);
			}
		}

		/// Signal -> Add
		pf_decl_constexpr void add_signal(
				receiver &__r) pf_attr_noexcept
		{
			this->list_.insert_head(&__r);
			__r->transmitter_ = this;
		}

		/// Signal -> Remove
		pf_decl_constexpr void rem_signal(
				receiver &__r) pf_attr_noexcept
		{
			this->list_.remove(&__r);
			__r->transmitter_ = nullptr;
		}

		/// Signal -> Clear
		pf_decl_constexpr void clear() pf_attr_noexcept
		{
			for (auto b = this->list_.begin(), e = this->list_.end(); b != e; ++b)
			{
				this->list_.remove(b);
			}
		}

	private:
		singly_linked_list<__receiver_t> list_;
	};

	/// SIGNAL: Shared Input
	template <typename _FunTy>
	using shared_isignal = typename shared_osignal<_FunTy>::receiver;

	/// SIGNAL: Unique Output
	template <typename _FunTy>
	class unique_osignal
	{
		/// Signal -> Receiver
		class __receiver_t
		{
			pf_decl_friend unique_osignal<_FunTy>;

		public:
			/// Constructors
			pf_decl_constexpr __receiver_t(
					fun_ptr<_FunTy> const &__fun) pf_attr_noexcept
					: transmitter_(nullptr)
					, fun_(__fun)
			{}
			pf_decl_constexpr __receiver_t(
					__receiver_t const &__r) pf_attr_noexcept = delete;
			pf_decl_constexpr __receiver_t(
					__receiver_t &&__r) pf_attr_noexcept = delete;

			/// Destructor
			pf_decl_constexpr __receiver_t() pf_attr_noexcept
			{
				this->transmitter_->rem(*this);
			}

			/// Transmitter -> Attached
			pf_hint_nodiscard pf_decl_constexpr unique_osignal<_FunTy> *attached_transmitter() const pf_attr_noexcept
			{
				return this->transmitter_;
			}

		private:
			unique_osignal<_FunTy> *transmitter_;
			fun_ptr<_FunTy> fun_;
		};

	public:
		using receiver = __receiver_t;

		/// Constructors
		pf_decl_constexpr unique_osignal() pf_attr_noexcept
		{}
		pf_decl_constexpr unique_osignal(
				unique_osignal<_FunTy> const &__r) pf_attr_noexcept = delete;
		pf_decl_constexpr unique_osignal(
				unique_osignal<_FunTy> &&__r)
		{
			this->add_signal(*__r.receiver_);
			__r.receiver_ = nullptr;
		}

		/// Destructor
		pf_decl_constexpr ~unique_osignal() pf_attr_noexcept
		{
			this->rem_signal();
		}

		/// Operator()
		template <typename... _InArgs>
		pf_decl_constexpr void operator()(
				_InArgs &&...__args) const pf_attr_noexcept
		{
			if (this->receiver_)
				this->receiver_->fun_(std::forward<_InArgs>(__args)...);
		}

		/// Signal -> Add
		pf_decl_constexpr void add_signal(
				receiver &__r) pf_attr_noexcept
		{
			this->rem_signal();
			this->receiver_	 = &__r;
			__r.transmitter_ = this;
		}

		/// Signal -> Remove
		pf_decl_constexpr void rem_signal() pf_attr_noexcept
		{
			if (this->receiver_)
			{
				this->receiver_->transmitter_ = nullptr;
				this->receiver_								= nullptr;
			}
		}

	private:
		receiver *receiver_;
	};

	/// SIGNAL: Unique Input
	template <typename _FunTy>
	using unique_isignal = typename unique_osignal<_FunTy>::receiver;
}

#endif // !PULSAR_FUN_SIGNAL_HPP