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
#include "pulsar/iterable.hpp"

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
	struct __future_store pf_attr_final
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
	class future pf_attr_final
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
      this->wait();
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
		, args(std::forward<_Args>(__args)...)
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
		, args(std::forward<_Args>(__args)...)
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
			this->store->finished.store(true, atomic_order::relaxed);
			__task_destroy_delete(this);
		}

		/// Store
		__future_store<std::invoke_result_t<_FunTy, _Args...>> *store;
		_FunTy fun;
		tuple<_Args...> args;
	};

	/// CONCURRENCY: Task -> Allocation
	template <
		typename _Ty,
		typename ... _Args>
	pf_hint_nodiscard pf_decl_inline
	_Ty*
	__task_new_construct(
		_Args&& ... __args) pf_attr_noexcept
	{
		_Ty *t = union_cast<_Ty*>(__dbg_allocate(sizeof(_Ty)));
    pf_assert(t != nullptr, "t is nullptr!");
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
		__dbg_deallocate(__ptr);
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
	pf_hint_nodiscard pf_decl_static future<std::invoke_result_t<_FunTy, _Args...>>
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
	pf_hint_nodiscard pf_decl_static future<std::invoke_result_t<_FunTy, _Args...>>
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

  /// CONCURRENCY: Task -> Pool
  struct __task_pool_store_t
  {
    /// Constructors
    __task_pool_store_t() pf_attr_noexcept
      : state(0)
    {}
    __task_pool_store_t(__task_pool_store_t const &) = delete;
    __task_pool_store_t(__task_pool_store_t &&)      = delete;

    /// Destructor
    ~__task_pool_store_t() pf_attr_noexcept
    {
      this->__wait();
    }

    /// Operator =
    __task_pool_store_t &operator=(__task_pool_store_t const &) = delete;
    __task_pool_store_t &operator=(__task_pool_store_t &&)      = delete;

    /// Process
    pf_decl_static void __process(
      __task_pool_store_t *__store)
    {
      while(true)
      {
        auto b = __store->pool.remove_head();
        while(b)
        {
          auto c = b;
          auto t = c->store;
          b = c->next;
          __task_destroy_delete(c);
          t->__call();
        }

        uint32_t v = __store->state.load(atomic_order::relaxed);
        uint32_t n = ((v & 2) == 0) ? 0 : 1;
        if (__store->state.compare_exchange_strong(v, n, atomic_order::relaxed, atomic_order::relaxed) && n == 0)
        {
          return;
        }
      }
    }

    /// Submit
    template <
      typename _FunTy,
      typename ... _Args>
    void
    __submit_task(
      _FunTy&& __fun,
      _Args && ... __args) pf_attr_noexcept
    {
      /// Initializing
      using node_t = mpsc_singly_lifo<__task_t*>::node_t;
      auto *n  = __task_new_construct<node_t>();
      n->store = __task_new_construct<__task<_FunTy, _Args...>>(std::move(__fun), std::forward<_Args>(__args)...);
      n->next  = nullptr;
      this->pool.insert_tail(n);

      /// Launch
      if (this->__is_finished())
      {
        this->state.fetch_or(1, atomic_order::relaxed);
        submit_task(__process, this);
      }
      else
      {
        this->state.fetch_or(2, atomic_order::relaxed);
      }
    }
    template <
      typename _FunTy,
      typename ... _Args>
    void
    __submit_task_0(
      _FunTy&& __fun,
      _Args && ... __args) pf_attr_noexcept
    {
      /// Initializing
      using node_t = mpsc_singly_lifo<__task_t*>::node_t;
      auto *n  = __task_new_construct<node_t>();
      n->store = __task_new_construct<__task<_FunTy, _Args...>>(std::move(__fun), std::forward<_Args>(__args)...);
      n->next  = nullptr;
      this->pool.insert_tail(n);

      /// Launch
      if (this->__is_finished())
      {
        this->state.fetch_or(1, atomic_order::relaxed);
        submit_task_0(__process, this);
      }
      else
      {
        this->state.fetch_or(2, atomic_order::relaxed);
      }
    }

    /// Submit Future
    template <
      typename _FunTy,
      typename ... _Args>
    pf_hint_nodiscard future<std::invoke_result_t<_FunTy, _Args...>>
    __submit_future_task(
      _FunTy&& __fun,
      _Args&& ... __args) pf_attr_noexcept
    {
      /// Initializing
      using node_t = mpsc_singly_lifo<__task_t*>::node_t;
      auto *n  = __task_new_construct<node_t>();
      auto *s  = new_construct<__future_store<std::invoke_result_t<_FunTy, _Args...>>>();
      n->store = __task_new_construct<__task_f<_FunTy, _Args...>>(s, std::move(__fun), std::forward<_Args>(__args)...);
      n->next  = nullptr;
      this->pool.insert_tail(n);

      /// Launch
      if (this->__is_finished())
      {
        this->state.fetch_or(1, atomic_order::relaxed);
        submit_task(__process, this);
      }
      else
      {
        this->state.fetch_or(2, atomic_order::relaxed);
      }

      /// Return
      return s;
    }
    template <
      typename _FunTy,
      typename ... _Args>
    pf_hint_nodiscard future<std::invoke_result_t<_FunTy, _Args...>>
    __submit_future_task_0(
      _FunTy&& __fun,
      _Args&& ... __args) pf_attr_noexcept
    {
      /// Initializing
      using node_t = mpsc_singly_lifo<__task_t*>::node_t;
      auto *n  = __task_new_construct<node_t>();
      auto *s  = new_construct<__future_store<std::invoke_result_t<_FunTy, _Args...>>>();
      n->store = __task_new_construct<__task_f<_FunTy, _Args...>>(s, std::move(__fun), std::forward<_Args>(__args)...);
      n->next  = nullptr;
      this->pool.insert_tail(n);

      /// Launch
      if (this->__is_finished())
      {
        this->state.fetch_or(1, atomic_order::relaxed);
        submit_task_0(__process, this);
      }
      else
      {
        this->state.fetch_or(2, atomic_order::relaxed);
      }

      /// Return
      return s;
    }

    /// Finished
    pf_hint_nodiscard bool
    __is_finished() const pf_attr_noexcept
    {
      return this->state.load(atomic_order::relaxed) == 0;
    }

    /// Wait
    bool
    __wait() const pf_attr_noexcept
    {
      bool b = this->state.load(atomic_order::relaxed) == 0;
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
        b = this->state.load(atomic_order::relaxed) == 0;
      }
      return !b;
    }

    /// Store
    pf_alignas(CCY_ALIGN) atomic<uint32_t> state; // 0 = waiting, 1 = running, 2 = enqueued
    mpsc_singly_lifo<__task_t*> pool;
  };
  class task_pool_t pf_attr_final
  {
  public:
    /// Constructors
    task_pool_t()
      : buf_(new_construct<__task_pool_store_t>())
    {}
    task_pool_t(task_pool_t const &) = delete;
    task_pool_t(task_pool_t &&__r)
      : buf_(__r.buf_)
    {
      __r.buf_ = nullptr;
    }

    /// Destructor
    ~task_pool_t() pf_attr_noexcept
    {
      if (this->buf_) destroy_delete(this->buf_);
    }

    /// Operator =
    task_pool_t &operator=(
        task_pool_t const &) = delete;
    task_pool_t &operator=(
        task_pool_t &&__r) pf_attr_noexcept
    {
      if (pf_likely(this != &__r))
      {
        if (this->buf_) destroy_delete(this->buf_);
        this->buf_ = __r.buf_;
        __r.buf_ = nullptr;
      }
      return *this;
    }

    /// Submit
    template <
      typename _FunTy,
      typename ... _Args>
    void 
    submit_task(
      _FunTy&& __fun,
      _Args&& ... __args) pf_attr_noexcept
    {
      this->buf_->__submit_task(std::move(__fun), std::forward<_Args>(__args)...);
    }

    /// Submit Future
    template <
      typename _FunTy,
      typename ... _Args>
    pf_hint_nodiscard future<std::invoke_result_t<_FunTy, _Args...>>
    submit_future_task(
      _FunTy&& __fun,
      _Args&& ... __args) pf_attr_noexcept
    requires(!std::is_void_v<std::invoke_result_t<_FunTy, _Args...>> 
            && std::is_invocable_v<_FunTy, _Args...>)
    {
      return this->buf_->__submit_future_task(std::move(__fun), std::forward<_Args>(__args)...);
    }

    /// Finished
    pf_hint_nodiscard bool
    is_finished() const pf_attr_noexcept
    {
      return this->buf_->__is_finished();
    }

    /// Wait
    bool
    wait() const pf_attr_noexcept
    {
      return this->buf_->__wait();
    }

  private:
    /// Store
    __task_pool_store_t *buf_;
  };

  /// CONCURRENCY: Task -> Pool
  class task_pool_0_t pf_attr_final
  {
  public:
    /// Constructors
    task_pool_0_t()
      : buf_(new_construct<__task_pool_store_t>())
    {}
    task_pool_0_t(task_pool_0_t const &) = delete;
    task_pool_0_t(task_pool_0_t &&__r)
      : buf_(__r.buf_)
    {
      __r.buf_ = nullptr;
    }
    
    /// Destructor
    ~task_pool_0_t() pf_attr_noexcept
    {
      if (this->buf_) destroy_delete(this->buf_);
    }

    /// Operator =
    task_pool_0_t &operator=(
        task_pool_0_t const &) = delete;
    task_pool_0_t &operator=(
        task_pool_0_t &&__r) pf_attr_noexcept
    {
      if (pf_likely(this != &__r))
      {
        if (this->buf_) destroy_delete(this->buf_);
        this->buf_ = __r.buf_;
        __r.buf_ = nullptr;
      }
      return *this;
    }

    /// Submit
    template <
      typename _FunTy,
      typename ... _Args>
    void 
    submit_task_0(
      _FunTy&& __fun,
      _Args&& ... __args) pf_attr_noexcept
    {
      this->buf_->__submit_task_0(std::move(__fun), std::forward<_Args>(__args)...);
    }

    /// Submit Future
    template <
      typename _FunTy,
      typename ... _Args>
    pf_hint_nodiscard future<std::invoke_result_t<_FunTy, _Args...>>
    submit_future_task_0(
      _FunTy&& __fun,
      _Args&& ... __args) pf_attr_noexcept
    requires(!std::is_void_v<std::invoke_result_t<_FunTy, _Args...>> 
            && std::is_invocable_v<_FunTy, _Args...>)
    {
      return this->buf_->__submit_future_task_0(std::move(__fun), std::forward<_Args>(__args)...);
    }

    /// Finished
    pf_hint_nodiscard bool
    is_finished() const pf_attr_noexcept
    {
      return this->buf_->__is_finished();
    }

    /// Wait
    bool
    wait() const pf_attr_noexcept
    {
      return this->buf_->__wait();
    }

  private:
    /// Store
    __task_pool_store_t *buf_;
  };
}

#endif // !PULSAR_THREAD_POOL_HPP