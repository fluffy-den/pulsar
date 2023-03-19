/*! @file   atomic_container.hpp
 *  @author Louis-Quentin Noé (noe.louis-quentin@hotmail.fr)
 *  @brief  Implementation of atomic (thread-safe) containers.
 *  @date   31-12-2022
 *
 *  @copyright Copyright (c) 2023 - Pulsar Software
 *
 *  @since 0.1.3
 */

#ifndef PULSAR_ATOMIC_CONTAINERS_HPP
#define PULSAR_ATOMIC_CONTAINERS_HPP 1

// Include: Pulsar
#include "pulsar/pulsar.hpp"
#include "pulsar/tuple.hpp"

// Include: C++
#include <thread>
#include <atomic>
#include <mutex>
#include <shared_mutex>
#include <condition_variable>

// Pulsar
namespace pul
{
	/// CONCURRENCY: Constants
	pf_decl_static pf_decl_inline size_t CCY_NUM_THREADS	 = std::thread::hardware_concurrency();
	pf_decl_static pf_decl_inline size_t CCY_NUM_WORKERS	 = CCY_NUM_THREADS - 1;
	pf_decl_static pf_decl_constexpr align_val_t CCY_ALIGN = align_val_t(64);

	/// CONCURRENCY: Atomic
	template <typename _Ty>
	using atomic			 = std::atomic<_Ty>;
	using atomic_order = std::memory_order;

	/// CONCURRENCY: Lock
	using mutex_t = std::mutex;
	template <typename _Lock>
	using lock_unique = std::unique_lock<_Lock>;

	/// CONCURRENCY: Condition Variable
	using condition_variable_t = std::condition_variable;

	/// CONCURRENCY: Thread
	using thread_t		= std::thread;
	using thread_id_t = uint16_t;

	/// CONCURRENCY: This Thread
namespace this_thread
{
	/// Sleep
	pf_decl_inline void sleep_for(
		nanoseconds_t __s)
	{
		std::this_thread::sleep_for(__s);
	}
	pf_decl_inline void sleep_until(
		high_resolution_point_t __p)
	{
		std::this_thread::sleep_until(__p);
	}

	/// Yield
	pf_decl_inline void yield()
	{
		std::this_thread::yield();
	}

	/// ID
	pf_hint_nodiscard pf_decl_inline thread_id_t get_id()
	{
		return union_cast<thread_id_t>(std::this_thread::get_id());
	}

	// Local ID
	pf_decl_static pf_decl_thread_local const thread_id_t id = this_thread::get_id();
}

	/// CONCURRENCY: Job -> future
	template <typename _RetTy>
	class future_store
	{
	};
	template <typename _RetTy>
	class future
	{
	// TODO
	};

	/// CONCURRENCY: Job
	pulsar_api void __submit_job_0_base(
		fun_ptr<void(void*)> __fun,
		void *__data) pf_attr_noexcept;
	pulsar_api void __submit_job_base(
		fun_ptr<void(void*)> __fun,
		void *__data) pf_attr_noexcept;
	template <typename _Fun, typename ... _Args>
	void submit_job_0(
		_Fun && __fun,
		_Args && ... __args) pf_attr_noexcept
	{

	}
	template <typename _Fun, typename ... _Args>
	void submit_job(
		_Fun && __fun,
		_Args && ... __args) pf_attr_noexcept
	{

	}
	template <typename _Fun, typename ... _Args>
	__submit_future_task(
		future_store<std::invoke_result_t<_Fun, _Args...>> *__store,
		_Fun && __fun,
		_Args && ... __args)
	{

	}
	template <typename _Fun, typename ... _Args>
	future<_RetTy> submit_future_0(
		_Fun && __fun,
		_Args && ... __args) pf_attr_noexcept
	{

	}
	template <typename _Fun, typename ... _Args>
	future<_RetTy> submit_future(
		_Fun && __fun,
		_Args && ... __args) pf_attr_noexcept
	{

	}

	/// CONCURRENCY: Job -> Lock
	// TODO

	/// CONCURRENCY: Job -> Functions
	pulsar_api bool
	job_process() pf_attr_noexcept;
	pulsar_api bool
	job_process0();
}

#endif // !PULSAR_ATOMIC_CONTAINERS_HPP