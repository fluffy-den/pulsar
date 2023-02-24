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

// Include: C++
#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>

// Pulsar
namespace pul
{
	/// CONCURRENCY: Atomic


	/// CONCURRENCY: Lock
	using lock				= std::mutex;
	using lock_unique = std::unique_lock<lock>;

	/// CONCURRENCY: Condition Variable
	using condition_variable = std::condition_variable;

	/// CONCURRENCY: Thread
	using thread_t = std::thread;

	/// CONCURRENCY: Job -> Task
	struct __job_task
	{
		fun_ptr<void(void*)> fun;
		// task args
	};

	/// CONCURRENCY: Job
	struct __job
	{
		size_t count_;
	};

	// void fun(int32_t __i)
	//
	//
	//

	template <typename ... _Args>
	struct type
	{

		private:
			_Ty val1_;
			int64_t val_;
	};

	/// CONCURRENCY: Job -> Future
	template <typename _Ret>
	struct alignas(alignof(_Ret)) job_future
	{

		private:
			byte_t value_[sizeof(_Ret)];
			atomic_flag_t finished_;
			// TODO: Padding
	};

	/// CONCURRENCY: Job Ring Buffer
	class pf_alignas(8) __job_ring_buffer pf_attr_final	// Ring Buffer
	{
	/// Lock & Grow
	void
	__lock_and_grow(
		size_t __req)
	{
		size_t wnt = __req;
		{	// TODO Critical Section

		}
	}

	public:
		/// Constructors
		__job_ring_buffer(
			size_t __grow) pf_attr_noexcept
		{
			// TODO
		}
		__job_ring_buffer(
			__job_ring_buffer const &) = delete;
		__job_ring_buffer(
			__job_ring_buffer &&) = delete;

		/// Destructor
		~__job_ring_buffer() pf_attr_noexcept
		{
			// TODO
		}

		/// Alloc
		__job*
		insert_job()
		{
			// TODO
		}
		__job*
		remove_front()
		{
			// TODO
		}

		/// Shrink
		size_t
		shrink_to_magnifier() pf_attr_noexcept
		{
			// TODO
		}
		size_t
		shrink_to_fit() pf_attr_noexcept
		{
			// TODO
		}

	private:
		byte_t *store_;
		// mutex mutex_;
		magnifier_linear magnifier_;
		size_t end_;
		atomic<size_t> beg_;
		// TODO Padding
	};

	/// CONCURRENCY: Job Worker Storage
	class __job_worker_storage pf_attr_final
	{

	private:
		__job_worker_storage *next_;
		__job_ring_buffer rb_;
	};

	/// CONCURRENCY: Job Worker Storage -> Iterator
	class __job_worker_storage_iterator pf_attr_final
	{
	private:
		__job_worker_storage *cur_;
	};

	/// CONCURRENCY: Job Worker
	class __job_worker pf_attr_final
	{
	private:
		__job_worker_storage store_;
		std::thread worker_;
	};

	/// CONCURRENCY: Job Scheduler
	class alignas(8) __job_scheduler pf_attr_final
	{
	private:
		sequence_forward<__job_worker> ws_;
		__job_worker_storage ms_;
		condition_variable cond_;
		atomic<size_t> numjobs_;
		// TODO PADDING + ALIGNMENT
	};
}

#endif // !PULSAR_ATOMIC_CONTAINERS_HPP