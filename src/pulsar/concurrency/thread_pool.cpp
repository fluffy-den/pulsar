/*! @file   thread_pool.cpp
 *  @author Louis-Quentin Noé (noe.louis-quentin@hotmail.fr)
 *  @brief
 *  @date   18-03-2023
 *
 *  @copyright Copyright (c) 2023 - Pulsar Software
 *
 *  @since 0.1.5
 */

// Include: Pulsar
#include "pulsar/internal.hpp"

// Pulsar
namespace pul
{
	/// CONCURRENCY: Thread Pool
	/// Buffer
	__thread_pool_t::__buffer_t::__buffer_t() pf_attr_noexcept
		: run(true)
		, numTasks(0)
		, numProcessing(0)
		, queue(CCY_TASKS_MAX_NUM)
		, queue0(CCY_TASKS_MAX_NUM_0)
	{}

	/// Thread
	__thread_t*
	__thread_pool_t::__buffer_t::__get_thread(
		uint32_t __index) pf_attr_noexcept
	{
		union
		{
			byte_t *as_byte;
			__thread_t *as_thread;
		};
		as_byte = &this->store[0];
		return as_thread + __index;
	}

	/// Buffer -> Make
	__thread_pool_t::__buffer_t*
	__thread_pool_t::__make_buffer()
	{
		return new_construct_ex<__buffer_t>(
			CCY_NUM_WORKERS * sizeof(__thread_t));
	}
	void
	__thread_pool_t::__delete_buffer(
		__buffer_t *__buf) pf_attr_noexcept
	{
		destroy_delete(__buf);
	}

	/// Thread -> Process
	int32_t
	__thread_process(
		__thread_pool_t::__buffer_t *__buf) pf_attr_noexcept
	{
		// Security
		__buf->numProcessing.fetch_add(1, atomic_order::relaxed);

		// Worker
		do
		{
			// Stop?
			if (__buf->numTasks.load(atomic_order::relaxed) < __buf->numProcessing.load(atomic_order::relaxed))
			{
				__buf->numProcessing.fetch_sub(1, atomic_order::relaxed);
				{
					lock_unique lck(__buf->mutex);
					__buf->cv.wait(lck, [&]() pf_attr_noexcept
												 { return __buf->run.load(atomic_order::relaxed) == false
													 || __buf->numTasks.load(atomic_order::relaxed) > __buf->numProcessing.load(atomic_order::relaxed); });
				}
				__buf->numProcessing.fetch_add(1, atomic_order::relaxed);
			}

			// Process
			while (__buf->numTasks.load(atomic_order::relaxed) >= __buf->numProcessing.load(atomic_order::relaxed))	// NOTE: Avoid low task overhead
			{
				uint32_t i	= 0;
				__task_t *t = __buf->queue.try_dequeue();
				while (t)
				{
					try
					{
						t->__call();
					}
					catch (dbg_exception const &__e)
					{
						// TODO: Move exception to main thread!
					}
					__dbg_deallocate(t);
					t = __buf->queue.try_dequeue();
					++i;
				};
				if (i > 0) __buf->numTasks.fetch_sub(i, atomic_order::relaxed);
			};
		} while (__buf->run.load(atomic_order::relaxed) == true);

		// Success
		return 0;
	}

	/// Constructors
	__thread_pool_t::__thread_pool_t()
	{
		/// Make Buffer
		this->buf_ = this->__make_buffer();

		/// Threads
		for (size_t i = 0; i != CCY_NUM_WORKERS; ++i)
		{
			construct(this->buf_->__get_thread(i), __thread_process, this->buf_);
		}
	}

	/// Destructor
	__thread_pool_t::~__thread_pool_t() pf_attr_noexcept
	{
		/// Stop the run
		this->buf_->run.store(false, atomic_order::release);
		this_thread::yield();
		while (this->buf_->numProcessing.load(atomic_order::relaxed) != CCY_NUM_WORKERS) this->buf_->cv.notify_all();

		/// Threads
		for (size_t i = 0; i < CCY_NUM_WORKERS; ++i)
		{
			auto t = this->buf_->__get_thread(i);
			if(t->joinable()) t->join();
		}

		/// Process 0
		while(this->__process());
		while(this->__process_0() > 0);

		/// Buffer
		this->__delete_buffer(this->buf_);
	}

	/// Submit
	void
	__thread_pool_t::__submit(
		__task_t *__task)
	{
		// Add
		if(!this->buf_->queue.try_enqueue(__task))
		{
			return;
			// TODO: Exception
		}

		// Notify
		if (this->buf_->numTasks.fetch_add(1, atomic_order::relaxed) + 1 <= CCY_NUM_WORKERS) this->buf_->cv.notify_one();

	}
	void
	__thread_pool_t::__submit_0(
		__task_t *__task)
	{
		if(!this->buf_->queue0.try_enqueue(__task))
		{
			return;
			// TODO: Exception
		}
	}

	/// Process
	bool
	__thread_pool_t::__process()
	{
		__task_t *t = this->buf_->queue.try_dequeue();
		if (t)
		{
			t->__call();
			__dbg_deallocate(t);
			this->buf_->numTasks.fetch_sub(1, atomic_order::relaxed);
			return true;
		}
		return false;
	}
	uint32_t
	__thread_pool_t::__process_0()
	{
		__task_t *t[CCY_CACHE_NUM0] = { nullptr };
		const uint32_t i						= this->buf_->queue0.try_dequeue_bulk(begin(t), end(t));
		uint32_t j									= 0;
		while(j != i)
		{
			t[j]->__call();
			__dbg_deallocate(t[j]);
			++j;
		}
		return i;
	}

	/// CONCURRENCY: Task -> Enqueue
	pulsar_api void
	__task_enqueue_0(
		__task_t *__task) pf_attr_noexcept
	{
		__internal.thread_pool.__submit_0(__task);
	}
	pulsar_api void
	__task_enqueue(
		__task_t *__task) pf_attr_noexcept
	{
		__internal.thread_pool.__submit(__task);
	}

	/// CONCURRENCY: Task -> Process
	pulsar_api bool
	process_tasks()
	{
		return __internal.thread_pool.__process();
	}
	pulsar_api uint32_t
	process_tasks_0()
	{
		return __internal.thread_pool.__process_0();
	}
}