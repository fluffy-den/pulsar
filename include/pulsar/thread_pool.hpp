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
#include "pulsar/malloc.hpp"
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
		bool
		__wait() pf_attr_noexcept
		{
      bool b = this->finished.load(atomic_order::relaxed);
      if (b) return false;
      if (this_thread::get_id() == 0)
      {
        while (!this->finished.load(atomic_order::relaxed))
        {
          process_tasks_0();
        }
      }
      else
      {
        while (!this->finished.load(atomic_order::relaxed))
        {
          this_thread::yield();
        }
      }
      return !b;
    }

		/// Val
		pf_hint_nodiscard _RetTy
		__value() pf_attr_noexcept
		{
      this->__wait();
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
		value()
		{
      return this->store_->__value();
		}

	private:
		__future_store<_RetTy> *store_;
	};

	/// CONCURRENCY: Type -> Task Function
  using __task_fun_t = fun_ptr<void(void *)>; 

  /// CONCURRENCY: Type -> Task
  struct __task_t pf_attr_final
	{
		/// Constructors
		__task_t(
      __task_fun_t __fun) pf_attr_noexcept
     : task_(__fun)
    {}
		__task_t(__task_t const &) = delete;
		__task_t(__task_t &&)			 = delete;

		/// Destructor
		~__task_t() pf_attr_noexcept = default;

		/// Operator =
		__task_t &operator=(__task_t const &) = delete;
		__task_t &operator=(__task_t &&)			= delete;

		/// Call
		pf_decl_inline void __call()
    {
      this->task_(&this->task_ + 1);
    }

    /// Store
    __task_fun_t task_;
  };
	template <
		typename _FunTy,
		typename ... _Args>
	struct __task_data pf_attr_final
	{
		/// Constructors
		__task_data(
			_FunTy && __fun,
			_Args && ... __args) pf_attr_noexcept
		: fun(std::move(__fun))
		, args(std::forward<_Args>(__args)...)
		{}
		__task_data(__task_data<_FunTy, _Args...> const &) = delete;
		__task_data(__task_data<_FunTy, _Args...> &&)			 = delete;

		/// Destructor
		~__task_data() pf_attr_noexcept = default;

		/// Operator =
		__task_data& operator=(__task_data<_FunTy, _Args...> const &) = delete;
		__task_data& operator=(__task_data<_FunTy, _Args...> &&)			= delete;

		/// Store
		_FunTy fun;
		tuple<_Args...> args;
	};
  template <
    typename _FunTy,
    typename ... _Args>
  pf_decl_inline void 
  __task_data_proc(
    void *__data)
  {
    auto data = union_cast<__task_data<_FunTy, _Args...>*>(__data);
    try
    {
      tuple_apply(std::move(data->fun), std::move(data->args));
    } catch(std::exception const&)
    {
      destroy(data);
      throw;
    }
    destroy(data);
  }
  template <
    typename _FunTy,
    typename ... _Args>
  struct __task_store
  {
    /// Constructors
    __task_store(
      _FunTy &&__fun,
      _Args && ...__args) pf_attr_noexcept
    : task(__task_data_proc<_FunTy, _Args...>)
    , data(std::move(__fun), std::forward<_Args>(__args)...)
    {}
    __task_store(__task_store<_FunTy, _Args...> const &) = delete;
    __task_store(__task_store<_FunTy, _Args...> &&) = delete;

    /// Destructor
    ~__task_store() pf_attr_noexcept = default;

    /// Operator =
    __task_store<_FunTy, _Args...> &operator=(__task_store<_FunTy, _Args...> const &) = delete;
    __task_store<_FunTy, _Args...> &operator=(__task_store<_FunTy, _Args...> &&) = delete;

    /// Store
    __task_t task;
    __task_data<_FunTy, _Args...> data;
  };

  template <
		typename _FunTy,
		typename ... _Args>
	struct __task_data_f pf_attr_final
	{
		/// Constructors
		__task_data_f(
			__future_store<std::invoke_result_t<_FunTy, _Args...>> *__store,
			_FunTy && __fun,
			_Args && ... __args)
      : store(__store)
      , fun(std::move(__fun))
      , args(std::forward<_Args>(__args)...)
		{}
		__task_data_f(__task_data_f<_FunTy, _Args...> const &) = delete;
		__task_data_f(__task_data_f<_FunTy, _Args...> &&)			 = delete;

		/// Destructor
		~__task_data_f() pf_attr_noexcept = default;

		/// Operator =
		__task_data_f<_FunTy, _Args...>& operator=(__task_data_f<_FunTy, _Args...> const &) = delete;
		__task_data_f<_FunTy, _Args...>& operator=(__task_data_f<_FunTy, _Args...> &&)			= delete;

		/// Store
		__future_store<std::invoke_result_t<_FunTy, _Args...>> *store;
		_FunTy fun;
		tuple<_Args...> args;
	};
  template <
    typename _FunTy,
    typename ... _Args>
  pf_decl_inline void 
  __task_data_f_proc(
    void *__data)
  {
    auto data = union_cast<__task_data_f<_FunTy, _Args...>*>(__data);
    pf_alignas(CCY_ALIGN) atomic<bool> ctrl = false;
    try
    {
      *union_cast<std::invoke_result_t<_FunTy, _Args...>*>(&data->store->retVal[0]) = tuple_apply(std::move(data->fun), std::move(data->args));
    } catch(std::exception const&)
    {
      data->store->finished.store(true, atomic_order::relaxed);
      destroy(data);
      throw;
    }
    data->store->finished.store(true, atomic_order::relaxed);
    destroy(data);
  }
  template <
    typename _FunTy,
    typename ... _Args>
  struct __task_store_f
  {
    /// Constructors
    __task_store_f(
      __future_store<std::invoke_result_t<_FunTy, _Args...>> *__s,
      _FunTy &&__fun,
      _Args &&... __args)
      : task(__task_data_f_proc<_FunTy, _Args...>)
      , data(__s, std::move(__fun), std::forward<_Args>(__args)...)
    {}
    __task_store_f(__task_store_f<_FunTy, _Args...> const &) = delete;
    __task_store_f(__task_store_f<_FunTy, _Args...> &&) = delete;

    /// Destructor
    ~__task_store_f() pf_attr_noexcept = default; 

    /// Operator =
    __task_store_f<_FunTy, _Args...> &operator=(__task_store_f<_FunTy, _Args...> const &) = delete;
    __task_store_f<_FunTy, _Args...> &operator=(__task_store_f<_FunTy, _Args...> &&) = delete;

    /// Store
    __task_t task;
    __task_data_f<_FunTy, _Args...> data;
  };

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
		_Args && ... __args)
	requires(std::is_invocable_v<_FunTy, _Args...>)
	{
		auto *t = cnew_construct<__task_store<_FunTy, _Args...>>(std::move(__fun), std::forward<_Args>(__args)...);
    __task_enqueue(&t->task);
	}
	template <
		typename _FunTy,
		typename ... _Args>
	pf_decl_static void
	submit_task_0(
		_FunTy&& __fun,
		_Args && ... __args)
	requires(std::is_invocable_v<_FunTy, _Args...>)
	{
		auto *t = cnew_construct<__task_store<_FunTy, _Args...>>(std::move(__fun), std::forward<_Args>(__args)...);
    __task_enqueue_0(&t->task);
	}
	template <
		typename _FunTy,
		typename ... _Args>
	pf_hint_nodiscard pf_decl_static future<std::invoke_result_t<_FunTy, _Args...>>
	submit_future_task(
		_FunTy&& __fun,
		_Args&& ... __args)
	requires(!std::is_void_v<std::invoke_result_t<_FunTy, _Args...>> 
           && std::is_invocable_v<_FunTy, _Args...>)
	{
		auto *s = new_construct<__future_store<std::invoke_result_t<_FunTy, _Args...>>>();
    try
    {
      auto *t = cnew_construct<__task_store_f<_FunTy, _Args...>>(s, std::move(__fun), std::forward<_Args>(__args)...);
      __task_enqueue(&t->task);
      return s;
    } catch(std::exception const&)
    {
      destroy_delete(s);
      throw;
    }
	}
	template <
		typename _FunTy,
		typename ... _Args>
	pf_hint_nodiscard pf_decl_static future<std::invoke_result_t<_FunTy, _Args...>>
	submit_future_task_0(
		_FunTy&& __fun,
		_Args&& ... __args)
	requires(!std::is_void_v<std::invoke_result_t<_FunTy, _Args...>> 
           && std::is_invocable_v<_FunTy, _Args...>)
	{
		auto *s = new_construct<__future_store<std::invoke_result_t<_FunTy, _Args...>>>();
    try
    {
      auto *t = cnew_construct<__task_store_f<_FunTy, _Args...>>(s, std::move(__fun), std::forward<_Args>(__args)...);
      __task_enqueue_0(&t->task);
      return s;
    } catch(std::exception const&)
    {
      destroy_delete(s);
      throw;
    }
	}

  /// CONCURRENCY: Task -> Pool
  struct __task_pool_store_t
  {
    /// Type -> Node
    struct __node_t
    {
      __node_t *next;
      __task_t task;
    };
    template <
      typename _FunTy,
      typename ... _Args>
    struct __node_data
    {
      /// Constructors
      __node_data(
        _FunTy &&__fun,
        _Args &&... __args)
        : next(nullptr)
        , data(std::move(__fun), std::forward<_Args>(__args)...)
      {}
      __node_data(
        __node_data<_FunTy, _Args...> const&) = delete;
      __node_data(
        __node_data<_FunTy, _Args...> &&) = delete;

      /// Destructor
      ~__node_data() pf_attr_noexcept = default;

      /// Operator =
      __node_data<_FunTy, _Args...> &operator=(__node_data<_FunTy, _Args...> const &) = delete;
      __node_data<_FunTy, _Args...> &operator=(__node_data<_FunTy, _Args...> &&) = delete;

      /// Store
      __node_t *next;
      __task_store<_FunTy, _Args...> data;
    };
    template <
      typename _FunTy,
      typename ... _Args>
    struct __node_data_f
    {
      /// Constructors
      __node_data_f(
        __future_store<std::invoke_result_t<_FunTy, _Args...>> *__s,
        _FunTy &&__fun,
        _Args &&... __args)
        : next(nullptr)
        , data(__s, std::move(__fun), std::forward<_Args>(__args)...)
      {}
      __node_data_f(
        __node_data_f<_FunTy, _Args...> const&) = delete;
      __node_data_f(
        __node_data_f<_FunTy, _Args...> &&) = delete;

      /// Destructor
      ~__node_data_f() pf_attr_noexcept = default;

      /// Operator =
      __node_data_f<_FunTy, _Args...> &operator=(__node_data_f<_FunTy, _Args...> const &) = delete;
      __node_data_f<_FunTy, _Args...> &operator=(__node_data_f<_FunTy, _Args...> &&) = delete;

      /// Store
      __node_t *next;
      __task_store_f<_FunTy, _Args...> data;
    };

    /// Constructors
    __task_pool_store_t() pf_attr_noexcept
      : numTasks(0)
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
    pf_hint_nodiscard pf_decl_static uint32_t 
    __process(
      __task_pool_store_t *__store)
    {
      uint32_t k = 0;
      auto *t = __store->pool.dequeue();
      while (t)
      {
        ++k;
        auto *n = t->next;
        try
        {
          t->task.__call();
        } catch(std::exception const&)
        {
          __store->numTasks.fetch_sub(k, atomic_order::relaxed);
          cdestroy_delete(t);
          throw;
        }
        cdestroy_delete(t);
        t = n;
      }
      return (__store->numTasks.fetch_sub(k, atomic_order::relaxed) - k);
    }
    pf_decl_static void 
    __process_auto_submit(
      __task_pool_store_t *__store) pf_attr_noexcept
    {
      if(__process(__store) > 0)
      {
        submit_task(__process_auto_submit, __store);
      }
    }
    pf_decl_static void 
    __process_auto_submit_0(
      __task_pool_store_t *__store)
    {
      if(__process(__store) > 0)
      {
        submit_task(__process_auto_submit_0, __store);
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
      auto* n = cnew_construct<__node_data<_FunTy, _Args...>>(std::move(__fun), std::forward<_Args>(__args)...);
      if (this->numTasks.fetch_add(1, atomic_order::relaxed) == 0)
      {
        submit_task(__process_auto_submit, this);
      }
      this->pool.enqueue(union_cast<__node_t *>(n));
    }
    template <
      typename _FunTy,
      typename ... _Args>
    void
    __submit_task_0(
      _FunTy&& __fun,
      _Args && ... __args) pf_attr_noexcept
    {
      auto *n = cnew_construct<__node_data<_FunTy, _Args...>>(std::move(__fun), std::forward<_Args>(__args)...);
      if (this->numTasks.fetch_add(1, atomic_order::relaxed) == 0)
      {
        submit_task_0(__process_auto_submit_0, this);
      }
      this->pool.enqueue(union_cast<__node_t *>(n));
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
      auto *s = new_construct<__future_store<std::invoke_result_t<_FunTy, _Args...>>>();
      auto *n = cnew_construct<__node_data_f<_FunTy, _Args...>>(s, std::move(__fun), std::forward<_Args>(__args)...);
      if (this->numTasks.fetch_add(1, atomic_order::relaxed) == 0)
      {
        submit_task(__process_auto_submit, this);
      }
      this->pool.enqueue(union_cast<__node_t *>(n));
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
      auto *s = new_construct<__future_store<std::invoke_result_t<_FunTy, _Args...>>>();
      auto *n = cnew_construct<__node_data_f<_FunTy, _Args...>>(s, std::move(__fun), std::forward<_Args>(__args)...);
      if (this->numTasks.fetch_add(1, atomic_order::relaxed) == 0)
      {
        submit_task_0(__process_auto_submit_0, this);
      }
      this->pool.enqueue(union_cast<__node_t *>(n));
      return s;
    }

    /// Finished
    pf_hint_nodiscard bool
    __is_finished() const pf_attr_noexcept
    {
      return this->numTasks.load(atomic_order::relaxed) == 0;
    }

    /// Wait
    bool
    __wait() const pf_attr_noexcept
    {
      // No need to wait
      if (numTasks.load(atomic_order::relaxed) == 0) return false;

      // Have to wait
      while (numTasks.load(atomic_order::relaxed) != 0);
      return true;
    }

    /// Store
    pf_alignas(CCY_ALIGN) atomic<uint32_t> numTasks;
    mpsc_singly_lifo<__node_t> pool;
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
      this->wait();
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
      this->wait();
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