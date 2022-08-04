/*! @file   job_system.cpp
 *  @author Fluffy (noe.louis-quentin@hotmail.fr)
 *  @brief  Implements job system.
 *  @date   15-07-2022
 *
 *  @copyright Copyright (c) 2022 - Pulsar Software
 *
 *  @since 0.1.2
 */

// Include: Src
#include "thread/job_system.hpp"

// Pulsar
namespace pul
{
	// Job System -> Thread Storage -> Constructors
	__job_sys_thread_storage::__job_sys_thread_storage() pf_attr_noexcept
	{}
	__job_sys_thread_storage::__job_sys_thread_storage(
			size_t __wcs)
			: allocator_(__wcs)
	{}

	// Job System -> Internal -> Worker -> Process Handle Exception
	void __job_sys_worker::__proc_for_exception_task_handle(
			__job_sys_internal *__i,
			std::exception_ptr const &__e)
	{
		// Exception Ptr
		union pack
		{
			void *as_void;
			std::exception_ptr *as_exception_ptr;
		} as_pack;
		as_pack.as_void						= job_system::__local_allocate(sizeof(std::exception_ptr));
		*as_pack.as_exception_ptr = __e;
		// Exception Task
		fun_ptr task							= [](void *__arg)
		{
			pack as_pack{ __arg };
			std::rethrow_exception(*as_pack.as_exception_ptr);
		};
		// Send
		__i->__submit_0({
				{task, as_pack.as_void}
		});
	}

	// Job System -> Internal -> Worker -> Process -> Work on jobs
	thread_id_t __job_sys_worker::__proc(
			__job_sys_worker *__w,
			__job_sys_internal *__i,
			thread_id_t __id)
	{
		__job_sys_internal::localstore_			 = &(*__w->store_);
		__job_sys_internal::localstore_->id_ = __id;
		while (__i->running_.test(std::memory_order::relaxed))
		{
			if (__i->numJobs_.load(std::memory_order::relaxed) == 0)
				__i->numJobs_.wait(0, std::memory_order::acquire);
			try
			{
				if (!__i->__process_list(__w->store_->list_))
				{
					while (!__i->__process_list_steal(__w->pos_->list_)
								 && __w->pos_ != &__w->store_)
					{
						++__w->pos_;
					}
					__w->pos_ = singly_iterator(&__w->store_) + 1;
				}
			}
			catch (exception const &__e)
			{
				__proc_for_exception_task_handle(
						__i,
						std::make_exception_ptr(__e));
			}
			catch (std::exception const &__e)
			{
				__proc_for_exception_task_handle(
						__i,
						std::make_exception_ptr(__e));
			}
		}
		return this_thread::ID();
	}

	// Job System -> Worker -> Constructors
	__job_sys_worker::__job_sys_worker()
			: pos_(nullptr)
	{}
	__job_sys_worker::__job_sys_worker(
			size_t __wcs,
			__job_sys_internal *__i,
			thread_id_t __id)
			: store_(__wcs)
			, pos_(&this->store_)
			, thread_(&__job_sys_worker::__proc, this, __i, __id)
	{}

	// Job System -> Internal -> Constructor
	__job_sys_internal::__job_sys_internal()
			: store_(4194304)
			, cacheline1_{ '\0' }
			, running_(true)
			, cacheline2_{ '\0' }
			, numJobs_(0)
			, cacheline3_{ '\0' }
	{
		// Thread -> Main
		this->localstore_			 = &(*this->store_);
		this->localstore_->id_ = 0;
		const size_t n				 = std::thread::hardware_concurrency() - 1;
		this->traveler_.insert_tail(&this->store_);
		// Thread -> Workers
		this->workers_ = std::make_unique<singly_node<__job_sys_worker>[]>(n);
		for (size_t i = 0; i < n; ++i)
		{
			union
			{
				void *as_void;
				singly_node<__job_sys_worker> *as_worker;
			};
			as_worker = &this->workers_[i];
			::new (as_void) singly_node<__job_sys_worker>(
					1048576,
					this,
					i + 1);
			singly_iterator it = as_worker;
			this->traveler_.insert_tail(&it->store_);
		}
		// Threads -> Position
		this->pos_ = singly_iterator(&this->store_) + 1;
		for (size_t i = 0; i < n; ++i)
		{
			this->workers_[i]->pos_ = singly_iterator(&this->workers_[i]->store_) + 1;
		}
	}

	// Job System -> Internal -> Destructor
	__job_sys_internal::~__job_sys_internal()
	{
		this->running_.clear(std::memory_order::relaxed);
		this->numJobs_.fetch_add(1, std::memory_order::release);
		this->numJobs_.notify_all();
		for (size_t i = 0, n = std::thread::hardware_concurrency() - 1; i < n; ++i)
		{
			if (this->workers_[i]->thread_.joinable())
				this->workers_[i]->thread_.join();
		}
		this->traveler_.clear();
		this->workers_.reset();
	}

	// Job System -> Internal -> Process
	bool __job_sys_internal::__process_list(
			singly_cds_linked_lifo<job_system::__job> &__l)
	{
		singly_iterator j = __l.remove_head();
		if (!j)
			return false;
		while (j)
		{
			j->__process();
			this->numJobs_.fetch_sub(1, std::memory_order::release);
			j = __l.remove_head();
		};
		return true;
	}
	bool __job_sys_internal::__process_list_steal(
			singly_cds_linked_lifo<job_system::__job> &__l)
	{
		singly_iterator j = __l.remove_head();
		if (!j)
			return false;
		else
		{
			j->__process();
			this->numJobs_.fetch_sub(1, std::memory_order::release);
			j = __l.remove_head();
		}
		return true;
	}
	void __job_sys_internal::__process_0()
	{
		singly_iterator j = this->mainlist_.clear();
		while (j)
		{
			j->__process();
			++j;
		}
	}
	void __job_sys_internal::__process()
	{
		if (!this->__process_list(this->store_->list_))
		{
			while (this->pos_ != &this->store_)
			{
				if (this->__process_list_steal(this->pos_->list_))
					break;
				++this->pos_;
			}
			this->pos_ = singly_iterator(&this->store_) + 1;
		}
	}

	// Job System -> Internal -> Link
	void __job_sys_internal::__link_and_notify(
			singly_cds_linked_lifo<job_system::__job> &__l,
			singly_node<job_system::__job> *__j) pf_attr_noexcept
	{
		__l.insert_tail(__j);
		this->numJobs_.fetch_add(1, std::memory_order::release);
		this->numJobs_.notify_one();
	}
	void __job_sys_internal::__link_0(
			singly_node<job_system::__job> *__j) pf_attr_noexcept
	{
		this->mainlist_.insert_tail(__j);
	}

	// Job System -> Internal -> Unique Alloc
	singly_node<job_system::__job> *__job_sys_internal::__create_unique_job(
			__job_sys_thread_storage *__store,
			std::initializer_list<job_task_initializer> const &__il)
	{
		union
		{
			void *as_void;
			singly_node<job_system::__job_unique> *as_unique_job;
			singly_node<job_system::__job> *as_job;
		};
		as_void = __store->allocator_.allocate(sizeof(singly_node<job_system::__job_unique>));
		auto b	= __il.begin();
		singly_link(
				job_system::__task_initializer_iterator(b),
				job_system::__task_initializer_iterator(__il.end()));
		::new (as_void) singly_node<job_system::__job_unique>(b->task_);
		return as_job;
	}

	// Job System -> Internal -> Unique With Future Alloc
	singly_node<job_system::__job> *__job_sys_internal::__create_unique_with_future_job(
			__job_sys_thread_storage *__store,
			job_system::__future &__f,
			std::initializer_list<job_task_initializer> const &__il)
	{
		union
		{
			void *as_void;
			singly_node<job_system::__job_unique_future> *as_unique_future_job;
			singly_node<job_system::__job> *as_job;
		};
		as_void = __store->allocator_.allocate(sizeof(singly_node<job_system::__job_unique_future>));
		auto b	= __il.begin();
		singly_link(
				job_system::__task_initializer_iterator(b),
				job_system::__task_initializer_iterator(__il.end()));
		::new (as_void) singly_node<job_system::__job_unique_future>(b->task_, &__f);
		return as_job;
	}

	// Job System -> Internal -> Submit -> Unique
	void __job_sys_internal::__submit(
			std::initializer_list<job_task_initializer> const &__il)
	{
		auto store = this->localstore_;
		this->__link_and_notify(store->list_, this->__create_unique_job(store, __il));
	}
	void __job_sys_internal::__submit_0(
			std::initializer_list<job_task_initializer> const &__il)
	{
		this->__link_0(__create_unique_job(this->localstore_, __il));
	}

	// Job System -> Internal -> Submit -> Unique with Future
	void __job_sys_internal::__submit(
			job_system::__future &__f,
			std::initializer_list<job_task_initializer> const &__il)
	{
		auto store = this->localstore_;
		this->__link_and_notify(store->list_, this->__create_unique_with_future_job(store, __f, __il));
	}
	void __job_sys_internal::__submit_0(
			job_system::__future &__f,
			std::initializer_list<job_task_initializer> const &__il)
	{
		this->__link_0(this->__create_unique_with_future_job(this->localstore_, __f, __il));
	}

	// Job System -> Internal -> Submit -> Shared
	void __job_sys_internal::__submit(
			singly_node<job_system::__job_shared> &__j,
			std::initializer_list<job_task_initializer> const &__il)
	{
		auto store = this->localstore_;
		union
		{
			void *as_void;
			singly_node<job_system::__job_shared> *as_shared_job;
			singly_node<job_system::__job> *as_job;
		};
		as_shared_job			 = &__j;
		singly_iterator it = &__j;
		if (it->__insert(__il.begin(), __il.end()))
		{
			this->__link_and_notify(store->list_, as_job);
		}
		else
		{
			this->numJobs_.notify_one();
		}
	}
	void __job_sys_internal::__submit_0(
			singly_node<job_system::__job_shared> &__j,
			std::initializer_list<job_task_initializer> const &__il)
	{
		union
		{
			void *as_void;
			singly_node<job_system::__job_shared> *as_shared_job;
			singly_node<job_system::__job> *as_job;
		};
		as_shared_job			 = &__j;
		singly_iterator it = &__j;
		if (it->__insert(__il.begin(), __il.end()))
		{
			this->__link_0(as_job);
		}
	}

	// Job System -> Constructor
	job_system::job_system()
	{
		if (__job_sys_internal::instance_)
			return;
		__job_sys_internal::instance_ = std::make_unique<__job_sys_internal>();
	}

	// Job System -> Destructor
	job_system::~job_system()
	{
		__job_sys_internal::instance_.reset();
	}

	// Job System -> Internal -> Allocate
	void *job_system::__local_allocate(
			size_t __size) pf_attr_noexcept
	{
		return __job_sys_internal::localstore_->allocator_.allocate(__size);
	}

	// Job System -> Submit -> Unique
	void job_system::submit(std::initializer_list<task_initializer> const &__il)
	{
		__job_sys_internal::instance_->__submit(__il);
	}
	void job_system::submit_0(
			std::initializer_list<task_initializer> const &__il)
	{
		__job_sys_internal::instance_->__submit_0(__il);
	}

	// Job System -> Submit -> Unique with Future
	pf_decl_inline void job_system::submit(
			future &__f,
			std::initializer_list<task_initializer> const &__il)
	{
		__job_sys_internal::instance_->__submit(__f, __il);
	}
	pf_decl_inline void job_system::submit_0(
			future &__f,
			std::initializer_list<task_initializer> const &__il)
	{
		__job_sys_internal::instance_->__submit_0(__f, __il);
	}

	// Job System -> Submit -> Shared
	pf_decl_inline void job_system::submit(
			job &__j,
			std::initializer_list<task_initializer> const &__il)
	{
		__job_sys_internal::instance_->__submit(__j, __il);
	}
	pf_decl_inline void job_system::submit_0(
			job &__j,
			std::initializer_list<task_initializer> const &__il)
	{
		__job_sys_internal::instance_->__submit_0(__j, __il);
	}

	// Job System -> Process Local
	pf_decl_inline void job_system::process()
	{
		__job_sys_internal::instance_->__process();
	}
	pf_decl_inline void job_system::process_0()
	{
		__job_sys_internal::instance_->__process_0();
	}

	// Job System -> Initializer
	const job_system __job_system;
}