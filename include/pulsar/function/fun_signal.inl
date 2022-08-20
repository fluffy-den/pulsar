/*! @file   fun_signal.inl
 *  @author Fluffy (noe.louis-quentin@hotmail.fr)
 *  @brief  Implementation of function signals.
 *  @date   11-08-2022
 *
 *  @copyright Copyright (c) 2022 - Pulsar Software
 *
 *  @since 0.1.1
 */

#ifndef PULSAR_FUN_SIGNAL_INL
#define PULSAR_FUN_SIGNAL_INL 1

// Include: Pulsar -> Function
#include "pulsar/function.hpp"

// Pulsar
namespace pul
{
	/// FUNCTION: Shared Signal -> Input
	/// Constructors
	template <typename _FunTy, template <typename> typename _ListTy>
	pf_decl_inline pf_decl_constexpr shared_osignal<_FunTy, _ListTy>::__receiver_t::__receiver_t(
			fun_ptr<_FunTy> const &__ptr) pf_attr_noexcept
			: transmitter_(nullptr)
			, fun_(__ptr)
	{}
	template <typename _FunTy, template <typename> typename _ListTy>
	pf_decl_inline pf_decl_constexpr shared_osignal<_FunTy, _ListTy>::__receiver_t::__receiver_t(
			__receiver_t &&__r) pf_attr_noexcept
			: transmitter_(__r.transmitter_)
			, fun_(__r.ptr_)
	{
		__r.transmitter_->signal_rem(&__r);
		__r.transmitter_->signal_add(*this);
	}

	/// Destructor
	template <typename _FunTy, template <typename> typename _ListTy>
	pf_decl_inline pf_decl_constexpr shared_osignal<_FunTy, _ListTy>::__receiver_t::~__receiver_t() pf_attr_noexcept
	{
		if (this->transmitter_)
		{
			union
			{
				__receiver_t *as_rec;
				receiver *as_node;
				byte_t *as_byte;
			};
			as_rec = this;
			as_byte -= (sizeof(receiver) - sizeof(__receiver_t));
			this->transmitter_->signal_rem(*as_node);
		}
	}

	/// Transmitter -> Attached
	template <typename _FunTy, template <typename> typename _ListTy>
	pf_hint_nodiscard pf_decl_inline pf_decl_constexpr shared_osignal<_FunTy> *shared_osignal<_FunTy, _ListTy>::__receiver_t::attached_transmitter() const pf_attr_noexcept
	{
		return this->transmitter_;
	}

	/// FUNCTION: Shared Signal -> Output
	/// Constructors
	template <typename _FunTy, template <typename> typename _ListTy>
	pf_decl_inline pf_decl_constexpr shared_osignal<_FunTy, _ListTy>::shared_osignal(
			shared_osignal<_FunTy, _ListTy> &&__r) pf_attr_noexcept
	{
		this->clear();
		this->list_ = std::move(__r.list_);
		for (auto b = this->list_.begin(), e = this->list_.end(); b != e; ++b)
		{
			b->transmitter_ = this;
		}
	}

	/// Destructor
	template <typename _FunTy, template <typename> typename _ListTy>
	pf_decl_inline pf_decl_constexpr shared_osignal<_FunTy, _ListTy>::~shared_osignal() pf_attr_noexcept
	{
		this->signals_clear();
	}

	/// Operator()
	template <typename _FunTy, template <typename> typename _ListTy>
	template <typename... _InArgs>
	pf_decl_inline pf_decl_constexpr void shared_osignal<_FunTy, _ListTy>::operator()(
			_InArgs &...__args) const pf_attr_noexcept
			requires(std::is_invocable_v<_FunTy, _InArgs...>)
	{
		for (auto b = this->list_.begin(), e = this->list_.end(); b != e; ++b)
		{
			b->fun_(std::forward<_InArgs>(__args)...);
		}
	}

	/// Management
	template <typename _FunTy, template <typename> typename _ListTy>
	pf_decl_inline pf_decl_constexpr void shared_osignal<_FunTy, _ListTy>::signal_add(
			receiver &__r) pf_attr_noexcept
	{
		this->list_.insert_head(&__r);
		__r->transmitter_ = this;
	}
	template <typename _FunTy, template <typename> typename _ListTy>
	pf_decl_inline pf_decl_constexpr void shared_osignal<_FunTy, _ListTy>::signal_rem(
			receiver &__r) pf_attr_noexcept
	{
		this->list_.remove(&__r);
		__r->transmitter_ = nullptr;
	}
	template <typename _FunTy, template <typename> typename _ListTy>
	pf_decl_inline pf_decl_constexpr void shared_osignal<_FunTy, _ListTy>::signals_clear() pf_attr_noexcept
	{
		auto b = this->list_.begin();
		while (b)
		{
			b->transmitter_ = nullptr;
			auto n					= std::next(b);
			this->list_.remove_head();
			b = n;
		}
		pf_assert(this->list_.empty(), "Internal linked list must be empty!");
	}


	/// FUNCTION: Unique Signal -> Output
	/// Constructors
	template <typename _FunTy>
	pf_decl_inline pf_decl_constexpr unique_osignal<_FunTy>::__receiver_t::__receiver_t(
			fun_ptr<_FunTy> const &__ptr) pf_attr_noexcept
			: transmitter_(nullptr)
			, fun_(__ptr)
	{}
	template <typename _FunTy>
	pf_decl_inline pf_decl_constexpr unique_osignal<_FunTy>::__receiver_t::__receiver_t(
			__receiver_t &&__r) pf_attr_noexcept
			: transmitter_(nullptr)
			, fun_(__r.fun_)
	{
		__r.transmitter_->signal_set(*this);
	}

	/// Destructor
	template <typename _FunTy>
	pf_decl_inline pf_decl_constexpr unique_osignal<_FunTy>::__receiver_t::~__receiver_t() pf_attr_noexcept
	{
		this->transmitter_->signal_clear();
	}

	/// Transmitter -> Attached
	template <typename _FunTy>
	pf_hint_nodiscard pf_decl_inline pf_decl_constexpr unique_osignal<_FunTy> *unique_osignal<_FunTy>::__receiver_t::attached_transmitter() const pf_attr_noexcept
	{
		return this->transmitter_;
	}

	/// FUNCTION: Unique Signal -> Input
	/// Constructors
	template <typename _FunTy>
	pf_decl_inline pf_decl_constexpr unique_osignal<_FunTy>::unique_osignal() pf_attr_noexcept
			: receiver_(nullptr)
	{}
	template <typename _FunTy>
	pf_decl_inline pf_decl_constexpr unique_osignal<_FunTy>::unique_osignal(
			unique_osignal<_FunTy> &&__r) pf_attr_noexcept
	{
		this->signal_set(*__r.receiver_);
	}

	/// Destructor
	template <typename _FunTy>
	pf_decl_inline pf_decl_constexpr unique_osignal<_FunTy>::~unique_osignal() pf_attr_noexcept
	{
		this->signal_clear();
	}

	/// Operator()
	template <typename _FunTy>
	template <typename... _InArgs>
	pf_decl_inline pf_decl_constexpr void unique_osignal<_FunTy>::operator()(
			_InArgs &&...__args) const pf_attr_noexcept
			requires(std::is_invocable_v<_FunTy, _InArgs...>)
	{
		if (this->receiver_)
			return this->receiver_->fun_(std::forward<_InArgs>(__args)...);
	}

	/// Signals
	template <typename _FunTy>
	pf_decl_inline pf_decl_constexpr void unique_osignal<_FunTy>::signal_set(
			receiver &__r) pf_attr_noexcept
	{
		this->signal_clear();
		this->receiver_	 = &__r;
		__r.transmitter_ = this;
	}
	template <typename _FunTy>
	pf_decl_inline pf_decl_constexpr void unique_osignal<_FunTy>::signal_clear() pf_attr_noexcept
	{
		if (this->receiver_)
		{
			this->receiver_->transmitter_ = nullptr;
			this->receiver_								= nullptr;
		}
	}
}

#endif // !PULSAR_FUN_SIGNAL_INL