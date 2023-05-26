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
	__thread_pool_storage_t::__thread_pool_storage_t() pf_attr_noexcept
		: run(true)
		, numTasks(0)
		, numProcessing(0)
		, queue(CCY_TASKS_MAX_NUM)
		, queue0(CCY_TASKS_MAX_NUM_0)
	{}

	/// Thread
	__thread_t *
	__thread_pool_storage_t::__get_thread(
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
	__thread_pool_storage_t *
	__thread_pool_t::__make_storage()
	{
		return new_construct_ex<__thread_pool_storage_t>(
		 CCY_NUM_WORKERS * sizeof(__thread_t));
	}
	void
	__thread_pool_t::__delete_storage(
	 __thread_pool_storage_t *__buf) pf_attr_noexcept
	{
		destroy_delete(__buf);
	}

	/// Thread -> Process
	int32_t
	__thread_process(
	 __thread_pool_storage_t *__buf) pf_attr_noexcept
	{
		// Security
		__buf->numProcessing.fetch_add(1, atomic_order::relaxed);

		// Worker
		do
		{
			// Stop?
			if(__buf->numTasks.load(atomic_order::relaxed) < __buf->numProcessing.load(atomic_order::relaxed))
			{
				__buf->numProcessing.fetch_sub(1, atomic_order::relaxed);
				{
					while(__buf->run.load(atomic_order::relaxed)
								&& __buf->numTasks.load(atomic_order::relaxed) <= __buf->numProcessing.load(atomic_order::relaxed))
					{
						this_thread::yield();
					}
				}
				__buf->numProcessing.fetch_add(1, atomic_order::relaxed);
			}

			// Process
			while(__buf->numTasks.load(atomic_order::relaxed) >= __buf->numProcessing.load(atomic_order::relaxed))	// NOTE: Avoid low task overhead
			{
				uint32_t i	= 0;
				__task_t *t = __buf->queue.try_dequeue();
				while(t)
				{
					try
					{
						t->__call();
					} catch(std::exception const &)
					{
						__dbg_move_exception_record_to_0();
					}
					cdestroy_delete(t);
					t = __buf->queue.try_dequeue();
					++i;
				};
				if(i > 0) __buf->numTasks.fetch_sub(i, atomic_order::relaxed);
			};
		} while(__buf->run.load(atomic_order::relaxed) == true);

		__buf->numProcessing.fetch_add(1, atomic_order::relaxed);
		// Success
		return 0;
	}

	/// Constructors
	__thread_pool_t::__thread_pool_t()
	{
		/// Make Buffer
		this->buf_ = this->__make_storage();

		/// Threads
		for(uint32_t i = 0; i != CCY_NUM_WORKERS; ++i)
		{
			construct(this->buf_->__get_thread(i), __thread_process, this->buf_);
		}
	}

	/// Destructor
	__thread_pool_t::~__thread_pool_t() pf_attr_noexcept
	{
		/// Stop the run
		this->buf_->run.store(false, atomic_order::release);
		while(this->buf_->numProcessing.load(atomic_order::relaxed) != 2 * CCY_NUM_WORKERS) process_tasks_0();	// Waits for all workers to terminate

		/// Threads
		for(uint32_t i = 0; i < CCY_NUM_WORKERS; ++i)
		{
			auto t = this->buf_->__get_thread(i);
			if(t->joinable()) t->join();
		}

		/// Process Remaining
		while(this->__process())
			;
		while(this->__process_0() > 0)
			;

		/// Buffer
		this->__delete_storage(this->buf_);
	}

	/// Submit
	void
	__thread_pool_t::__submit(
	 __task_t *__task)
	{
		// Add
		if(!this->buf_->queue.try_enqueue(__task))
		{
			pf_throw(
			 dbg_category_generic(),
			 dbg_code::runtime_error,
			 0,
			 "Problem when adding a task to the shared queue. task={}",
			 union_cast<void *>(__task));
			return;
		}

		// Notify
		this->buf_->numTasks.fetch_add(1, atomic_order::relaxed);
	}
	void
	__thread_pool_t::__submit_0(
	 __task_t *__task)
	{
		if(!this->buf_->queue0.try_enqueue(__task))
		{
			pf_throw(
			 dbg_category_generic(),
			 dbg_code::runtime_error,
			 0,
			 "Problem when adding a task to the main queue. task={}",
			 union_cast<void *>(__task));
			return;
		}
	}

	/// Process
	bool
	__thread_pool_t::__process()
	{
		__task_t *t = this->buf_->queue.try_dequeue();
		if(t)
		{
			t->__call();
			cdestroy_delete(t);
			this->buf_->numTasks.fetch_sub(1, atomic_order::relaxed);
			return true;
		}
		return false;
	}
	uint32_t
	__thread_pool_t::__process_0()
	{
		__task_t *t[32]	 = { nullptr };
		const uint32_t i = union_cast<uint32_t>(this->buf_->queue0.try_dequeue_bulk(begin(t), end(t)));
		uint32_t j			 = 0;
		while(j != i)
		{
			t[j]->__call();
			cdestroy_delete(t[j]);
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
}	 // namespace pul
