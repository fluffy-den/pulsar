/*! @file   thread_pool.hpp
 *  @author Louis-Quentin Noé (noe.louis-quentin@hotmail.fr)
 *  @brief
 *  @date   24-03-2023
 *
 *  @copyright Copyright (c) 2023 - Pulsar Software
 *
 *  @since 0.1.4
 */

#ifndef PULSAR_THREAD_POOL_HPP
#define PULSAR_THREAD_POOL_HPP 1

// Include: Pulsar
#include "pulsar/concurrency.hpp"
#include "pulsar/tuple.hpp"
#include "pulsar/function.hpp"
#include "pulsar/memory.hpp"
#include "pulsar/utility.hpp"
#include "pulsar/intrin.hpp"

// Pulsar
namespace pul
{
  
	/// CONCURRENCY: Task -> Process
	pulsar_api bool
	process_tasks();
	pulsar_api uint32_t
	process_tasks_0();

	/// CONCURRENCY: Task -> Future
	template <typename _RetTy>
	struct __future_store
	{
		/// Constructors
		__future_store() pf_attr_noexcept
		: finished(false)
		, retVal{ 0 }
		{}
		__future_store(__future_store<_RetTy> const&) = delete;
		__future_store(__future_store<_RetTy> &&)			= delete;

		/// Destructor
		~__future_store() pf_attr_noexcept = default;

		/// Operator =
		__future_store<_RetTy> &operator=(__future_store<_RetTy> const &) = delete;
		__future_store<_RetTy> &operator=(__future_store<_RetTy> &&)			= delete;

		/// Is Finished?
		pf_hint_nodiscard bool
		__is_finished() pf_attr_noexcept
		{
			return this->finished.load(atomic_order::relaxed);
		}

		/// Wait
		pf_hint_nodiscard bool
		__wait() pf_attr_noexcept
		{
      bool b = this->finished.load(atomic_order::relaxed);
      if (b) return false;
      while (!b)
			{
        if (this_thread::get_id() == 0)
        {
          if (process_tasks_0() == 0)
          {
            process_tasks();
          }
        }
        else
        {
          process_tasks();
        }
        b = this->finished.load(atomic_order::relaxed);
      }
      return !b;
    }

		/// Val
		pf_hint_nodiscard _RetTy
		__value() pf_attr_noexcept
		{
			pf_assert(this->__is_finished(), "Cannot get value of unfinished task!");
			return std::move(*union_cast<_RetTy*>(&this->retVal[0]));
		}

		/// Store
		pf_alignas(64) atomic<bool> finished;
		byte_t retVal[sizeof(_RetTy)];
	};
	template <typename _RetTy>
	class future
	{
	public:
		/// Constructors
		future(
			__future_store<_RetTy> *__store) pf_attr_noexcept
		: store_(__store)
		{}
		future(future<_RetTy> const &) = delete;
		future(future<_RetTy> && __r)
			: store_(__r.store_)
		{
			__r.store_ = nullptr;
		}

		/// Destructor
		~future() pf_attr_noexcept
		{
			if (this->store_)
			{
				this->wait();
				destroy_delete(this->store_);
			}
		}

		/// Operator =
		future<_RetTy> &operator=(future<_RetTy> const &) = delete;
		future<_RetTy> &operator=(future<_RetTy> &&)			= default;

		/// Is Finished?
		pf_hint_nodiscard pf_decl_inline bool
		is_finished() const pf_attr_noexcept
		{
			return this->store_->__is_finished();
		}

		/// Wait
		pf_decl_inline bool
		wait() const pf_attr_noexcept
		{
			return this->store_->__wait();
		}

		/// Val
		pf_hint_nodiscard pf_decl_inline _RetTy
		value() pf_attr_noexcept
		{
			return this->store_->__value();
		}

	private:
		__future_store<_RetTy> *store_;
	};

	/// CONCURRENCY: Type -> Task
	struct __task_t
	{
		/// Constructors
		__task_t() pf_attr_noexcept = default;
		__task_t(__task_t const &) = delete;
		__task_t(__task_t &&)			 = delete;

		/// Destructor
		pf_decl_virtual ~__task_t() pf_attr_noexcept = default;

		/// Operator =
		__task_t &operator=(__task_t const &) = delete;
		__task_t &operator=(__task_t &&)			= delete;

		/// Call
		pf_decl_virtual void __call() = 0;
	};
	template <
		typename _FunTy,
		typename ... _Args>
	struct __task pf_attr_final : public __task_t
	{
		/// Constructors
		__task(
			_FunTy && __fun,
			_Args && ... __args) pf_attr_noexcept
		: fun(std::move(__fun))
		, args{ std::forward<_Args>(__args)... }
		{}
		__task(__task<_FunTy, _Args...> const &) = delete;
		__task(__task<_FunTy, _Args...> &&)			 = delete;

		/// Destructor
		~__task() pf_attr_noexcept = default;

		/// Operator =
		__task& operator=(__task<_FunTy, _Args...> const &) = delete;
		__task& operator=(__task<_FunTy, _Args...> &&)			= delete;

		/// Call
		pf_decl_inline void
		__call() pf_attr_override
		{
			tuple_apply(this->fun, std::move(this->args));
			__task_destroy_delete(this);
		}

		/// Store
		_FunTy fun;
		tuple<_Args...> args;
	};
	template <
		typename _FunTy,
		typename ... _Args>
	struct __task_f pf_attr_final : public __task_t
	{
		/// Constructors
		__task_f(
			__future_store<std::invoke_result_t<_FunTy, _Args...>> *__store,
			_FunTy && __fun,
			_Args && ... __args) pf_attr_noexcept
		: store(__store)
		, fun(std::move(__fun))
		, args(std::move(__args)...)
		{}
		__task_f(__task_f const &) = delete;
		__task_f(__task_f &&)			 = delete;

		/// Destructor
		~__task_f() pf_attr_noexcept = default;

		/// Operator =
		__task_f& operator=(__task_f const &) = delete;
		__task_f& operator=(__task_f &&)			= delete;

		/// Call
		pf_decl_inline void
		__call() pf_attr_override
		{
			*union_cast<std::invoke_result_t<_FunTy, _Args...>*>(&this->store->retVal[0]) = tuple_apply(this->fun, std::move(this->args));
			this->store->finished.store(true, atomic_order::release);
			__task_destroy_delete(this);
		}

		/// Store
		__future_store<std::invoke_result_t<_FunTy, _Args...>> *store;
		_FunTy fun;
		tuple<_Args...> args;
	};

	/// CONCURRENCY: Task -> Allocation
	pf_hint_nodiscard pulsar_api
	void*
	__task_allocate(
		size_t __size) pf_attr_noexcept;
	pulsar_api
	void
	__task_deallocate(
		void *__ptr) pf_attr_noexcept;
	template <
		typename _Ty,
		typename ... _Args>
	pf_hint_nodiscard pf_decl_inline
	_Ty*
	__task_new_construct(
		_Args&& ... __args) pf_attr_noexcept
	{
		_Ty *t = union_cast<_Ty*>(__task_allocate(sizeof(_Ty)));
		construct(t, std::forward<_Args>(__args)...);
		return t;
	}
	template <
		typename _Ty>
	pf_decl_inline
	void
	__task_destroy_delete(
		_Ty *__ptr) pf_attr_noexcept
	{
		destroy(__ptr);
		__task_deallocate(__ptr);
	}

	/// CONCURRENCY: Task -> Enqueue
	pulsar_api void
	__task_enqueue_0(
		__task_t *__task) pf_attr_noexcept;
	pulsar_api void
	__task_enqueue(
		__task_t *__task) pf_attr_noexcept;

	/// CONCURRENCY: Task -> Submit
	template <
		typename _FunTy,
		typename ... _Args>
	pf_decl_static void
	submit_task(
		_FunTy&& __fun,
		_Args && ... __args) pf_attr_noexcept
	requires(std::is_invocable_v<_FunTy, _Args...>)
	{
		auto *t = __task_new_construct<__task<_FunTy, _Args...>>(std::move(__fun), std::forward<_Args>(__args)...);
		__task_enqueue(t);
	}
	template <
		typename _FunTy,
		typename ... _Args>
	pf_decl_static void
	submit_task_0(
		_FunTy&& __fun,
		_Args && ... __args) pf_attr_noexcept
	requires(std::is_invocable_v<_FunTy, _Args...>)
	{
		auto *t = __task_new_construct<__task<_FunTy, _Args...>>(std::move(__fun), std::forward<_Args>(__args)...);
		__task_enqueue_0(t);
	}
	template <
		typename _FunTy,
		typename ... _Args>
	pf_decl_static future<std::invoke_result_t<_FunTy, _Args...>>
	submit_future_task(
		_FunTy&& __fun,
		_Args&& ... __args) pf_attr_noexcept
	requires(!std::is_void_v<std::invoke_result_t<_FunTy, _Args...>> 
           && std::is_invocable_v<_FunTy, _Args...>)
	{
		auto *s = new_construct<__future_store<std::invoke_result_t<_FunTy, _Args...>>>();
		auto *t = __task_new_construct<__task_f<_FunTy, _Args...>>(s, std::move(__fun), std::forward<_Args>(__args)...);
		__task_enqueue(t);
		return s;
	}
	template <
		typename _FunTy,
		typename ... _Args>
	pf_decl_static future<std::invoke_result_t<_FunTy, _Args...>>
	submit_future_task_0(
		_FunTy&& __fun,
		_Args&& ... __args) pf_attr_noexcept
	requires(!std::is_void_v<std::invoke_result_t<_FunTy, _Args...>> 
           && std::is_invocable_v<_FunTy, _Args...>)
	{
		auto *s = new_construct<__future_store<std::invoke_result_t<_FunTy, _Args...>>>();
		auto *t = __task_new_construct<__task_f<_FunTy, _Args...>>(s, std::move(__fun), std::forward<_Args>(__args)...);
		__task_enqueue_0(t);
		return s;
	}
}

#endif // !PULSAR_THREAD_POOL_HPP