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
#include "pulsar/concurrency/thread_pool.hpp"

// Pulsar
namespace pul
{
	/// CONCURRENCY: Thread Pool
	/// Instance
	__task_pool_t __task_pool;

	/// Buffer
	__task_pool_t::__buffer_t::__buffer_t() pf_attr_noexcept
		: numTasks(0)
		, run(true)
		, numRunning(CCY_NUM_WORKERS)
		, queue(CCY_TASKS_MAX_NUM)
		, queue0(CCY_TASKS_MAX_NUM_0)
	{}

	/// Allocator
	allocator_mamd_ring_buffer*
	__task_pool_t::__buffer_t::__get_allocator(
		uint32_t __index) pf_attr_noexcept
	{
		union
		{
			byte_t *as_byte;
			allocator_mamd_ring_buffer *as_allocator;
		};
		as_byte = &this->store[0];
		return as_allocator + __index;
	}

	/// Thread
	__thread_t*
	__task_pool_t::__buffer_t::__get_thread(
		uint32_t __index) pf_attr_noexcept
	{
		union
		{
			byte_t *as_byte;
			__thread_t *as_thread;
		};
		as_byte = &this->store[0] + CCY_NUM_THREADS * sizeof(allocator_mamd_ring_buffer);
		return as_thread + __index;
	}

	/// Buffer -> Make
	__task_pool_t::__buffer_t*
	__task_pool_t::__make_buffer()
	{
		return new_construct_ex<__buffer_t>(
			CCY_NUM_THREADS * sizeof(allocator_mamd_ring_buffer) +
			CCY_NUM_WORKERS * sizeof(__thread_t));
	}
	void
	__task_pool_t::__delete_buffer(
		__buffer_t *__buf) pf_attr_noexcept
	{
		destroy_delete(__buf);
	}

	/// Constructors
	__task_pool_t::__task_pool_t()
	{
		/// Make Buffer
		this->buf_ = this->__make_buffer();

		/// Allocators
		construct(this->buf_->__get_allocator(0), CCY_TASKS_ALLOCATOR_SIZE_0);
		for (size_t i = 1; i < CCY_NUM_THREADS; ++i)
		{
			construct(this->buf_->__get_allocator(i), CCY_TASKS_ALLOCATOR_SIZE);
		}

		/// Threads
		for (size_t i = 0; i < CCY_NUM_WORKERS; ++i)
		{
			construct(this->buf_->__get_thread(i), this->__thread_process);
		}
	}

	/// Destructor
	__task_pool_t::~__task_pool_t() pf_attr_noexcept
	{
		/// Stop the run
		this->buf_->run.store(false, atomic_order::release);
		this->buf_->cv.notify_all();

		/// Threads
		for (size_t i = 0; i < CCY_NUM_WORKERS; ++i)
		{
			auto *t = this->buf_->__get_thread(i);
			if (t->joinable()) t->join();
		}

		/// Process 0
		while(this->__process());
		while(this->__process_0() > 0);

		/// Allocators
		for (size_t i = 0; i < CCY_NUM_THREADS; ++i)
		{
			destroy(this->buf_->__get_allocator(i));
		}

		/// Buffer
		this->__delete_buffer(this->buf_);
	}

	/// Allocate
	void*
	__task_pool_t::__allocate(
		size_t __size) pf_attr_noexcept
	{
		auto *all = this->buf_->__get_allocator(this_thread::get_id());
		void *ptr = all->allocate(__size);
		pf_print("{}\n", ptr);
		pf_assert(ptr, "ptr is nullptr!");
		return ptr;
	}
	void
	__task_pool_t::__deallocate(
		void *__ptr) pf_attr_noexcept
	{
		auto *all = this->buf_->__get_allocator(this_thread::get_id());
		return all->deallocate(__ptr);
	}

	/// Submit
	void
	__task_pool_t::__submit(
		__task_t *__task)
	{
		// Add
		if(!this->buf_->queue.try_enqueue(__task))
		{
			// TODO: Exception
		}
		this->buf_->numTasks.fetch_add(1, atomic_order::relaxed);
		// Wake-up?
		if (this->buf_->numRunning.load(atomic_order::relaxed) <= CCY_NUM_WORKERS)
		{
			this->buf_->cv.notify_one();
		}
	}
	void
	__task_pool_t::__submit_0(
		__task_t *__task)
	{
		if(!this->buf_->queue0.try_enqueue(__task))
		{
			// TODO: Exception
		}
	}

	/// Process
	bool
	__task_pool_t::__process()
	{
		__task_t *t = this->buf_->queue.try_dequeue();
		if (t)
		{
			t->__call();
			this->buf_->numTasks.fetch_sub(1, atomic_order::relaxed);
			return true;
		}
		return false;
	}
	uint32_t
	__task_pool_t::__process_0()
	{
		__task_t *t[CCY_CACHE_NUM0] = { nullptr };
		const uint32_t i						= this->buf_->queue0.try_dequeue_bulk(begin(t), end(t));
		uint32_t j									= 0;
		while(j != i)
		{
			t[j]->__call();
			++j;
		}
		return i;
	}

	/// CONCURRENCY: Task -> Allocation
	pulsar_api
	void*
	__task_allocate(
		size_t __size) pf_attr_noexcept
	{
		return __task_pool.__allocate(__size);
	}
	pulsar_api
	void
	__task_deallocate(
		void *__ptr) pf_attr_noexcept
	{
		__task_pool.__deallocate(__ptr);
	}

	/// CONCURRENCY: Task -> Enqueue
	pulsar_api void
	__task_enqueue_0(
		__task_t *__task) pf_attr_noexcept
	{
		__task_pool.__submit_0(__task);
	}
	pulsar_api void
	__task_enqueue(
		__task_t *__task) pf_attr_noexcept
	{
		__task_pool.__submit(__task);
	}

	/// CONCURRENCY: Task -> Process
	pulsar_api bool
	process()
	{
		return __task_pool.__process();
	}
	pulsar_api uint32_t
	process_0()
	{
		return __task_pool.__process_0();
	}
}