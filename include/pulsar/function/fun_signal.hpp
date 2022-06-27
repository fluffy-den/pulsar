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

	template <typename _FunTy>
	class fun_transmitter
	{
		/*! @brief
		 *
		 */
		struct __receiver_t
		{
			pf_decl_friend fun_transmitter<_FunTy>;

			/// Constructor
			pf_decl_constexpr __receiver_t(fun_ptr<_FunTy> const &__fun) pf_attr_noexcept
					: fun_(__fun) {}

			/// Destructor
			pf_decl_constexpr __receiver_t() pf_attr_noexcept
			{
				this->transmitter_->rem(*this);
			}

			/// Transmitter -> Attached
			pf_hint_nodiscard pf_decl_constexpr fun_transmitter<_FunTy> *attached_transmitter() const pf_attr_noexcept
			{
				return this->transmitter_;
			}

		private:
			fun_transmitter<_FunTy> *transmitter_;
			fun_ptr<_FunTy> fun_;
		};

	public:
		using receiver = singly_node<__receiver_t>;

		/// Constructors
		pf_decl_constexpr fun_transmitter() pf_attr_noexcept
		{}

		/// Operator()
		template <typename... _InArgs>
		pf_decl_constexpr void operator()(
				_InArgs const &...__args) const pf_attr_noexcept
		{
			for (auto b = this->list_.begin(), e = this->list_.end(); b != e; ++b)
			{
				b->fun_(__args...);
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

		/// Clear
		pf_decl_constexpr void clear() pf_attr_noexcept
		{
			for (auto b = this->list_.beg(), e = this->list_.end(); b != e; ++b)
			{
				this->list_.remove(*b);
			}
		}

	private:
		singly_linked_list<__receiver_t> list_;
	};

	template <typename _FunTy>
	using fun_receiver = fun_transmitter<_FunTy>::receiver;
}

#endif // !PULSAR_FUN_SIGNAL_HPP