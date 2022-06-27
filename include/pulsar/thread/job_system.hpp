/*! @file   job_system.hpp
 *  @author Fluffy (noe.louis-quentin@hotmail.fr)
 *  @brief
 *  @date   27-06-2022
 *
 *  @copyright Copyright (c) 2022 - Pulsar Software
 *
 *  @since 0.1.2
 */

#ifndef PULSAR_THREAD_JOB_SYSTEM
#define PULSAR_THREAD_JOB_SYSTEM 1

// Include: Pulsar
#include "pulsar/debug.hpp"
#include "pulsar/function.hpp"
#include "pulsar/linked_list.hpp"
#include "pulsar/memory.hpp"
#include "pulsar/static_initializer.hpp"
#include "pulsar/tuple.hpp"

// Include: Pulsar -> Thread
#include "pulsar/thread/thread_identifier.hpp"

// Include: C++
#include <atomic>
#include <initializer_list>
#include <list>
#include <memory>
#include <numeric>
#include <thread>
#include <vector>

// Pulsar
namespace pul
{
/// JOB: Constant -> Main
#ifndef PULSAR_JOB_SYSTEM_MAIN_ALLOCATOR_SIZE
#	define PULSAR_JOB_SYSTEM_MAIN_ALLOCATOR_SIZE 4194304
#endif // !PULSAR_JOB_SYSTEM_MAIN_ALLOCATOR_SIZE

/// JOB: Constant -> Worker
#ifndef PULSAR_JOB_SYSTEM_WORKER_ALLOCATOR_SIZE
#	define PULSAR_JOB_SYSTEM_WORKER_ALLOCATOR_SIZE 1048576
#endif // !PULSAR_JOB_SYSTEM_WORKER_ALLOCATOR_SIZE

	/// JOB: System
	class job_system pf_attr_final
	{
		/// Types
		struct __thread_storage;
		struct __internal;
		struct __worker;
		struct __task;
		struct __job;
		struct __job_unique;
		struct __job_unique_future;
		struct __task_initializer_iterator;

		/// Types -> External
		template <typename _RetTy>
			requires(!std::is_void_v<_RetTy>)
		class __future_with_value;
		class __future;
		class __job_shared;
		class __task_initializer;

		/// Type -> Initializer Iterator
		struct __task_initializer_iterator
		{
		public:
			/// Constructor
			__task_initializer_iterator(
					const __task_initializer *__p) pf_attr_noexcept
					: ptr_(__p)
			{}
			__task_initializer_iterator(
					__task_initializer_iterator const &__r)
					: ptr_(__r.ptr_)
			{}

			/// Operator=
			__task_initializer_iterator &operator=(
					__task_initializer_iterator const &__r)
			{
				this->ptr_ = __r.ptr_;
				return *this;
			}

			/// Operator*
			singly_node<__task> &operator*() pf_attr_noexcept
			{
				return *ptr_->task_;
			}

			/// Operator++
			__task_initializer_iterator &operator++() pf_attr_noexcept
			{
				++this->ptr_;
				return *this;
			}

			/// Operator==
			bool operator==(
					__task_initializer_iterator const &__r) pf_attr_noexcept
			{
				return this->ptr_ == __r.ptr_;
			}

		private:
			const __task_initializer *ptr_;
		};

		/// Type -> Thread Store
		struct __thread_storage
		{
			singly_cds_linked_lifo<__job> list_;
			memory::allocator_ring allocator_;

			/// Constructors
			__thread_storage() pf_attr_noexcept
			{}
			__thread_storage(
					size_t __wcs)
					: allocator_(__wcs)
			{}
		};

		/// Type -> Internal
		struct __internal
		{
			/// Constructors
			__internal()
					: store_(PULSAR_JOB_SYSTEM_MAIN_ALLOCATOR_SIZE)
					, cacheline1_{ '\0' }
					, running_(true)
					, cacheline2_{ '\0' }
					, numJobs_(0)
					, cacheline3_{ '\0' }
			{
				// Thread -> Main
				localstore_			 = &(*this->store_);
				this_thread::id_ = 0;
				const size_t n	 = std::thread::hardware_concurrency() - 1;
				this->traveler_.insert_tail(&this->store_);
				// Thread -> Workers
				this->workers_ = std::make_unique<singly_node<__worker>[]>(n);
				for (size_t i = 0; i < n; ++i)
				{
					union
					{
						void *as_void;
						singly_node<__worker> *as_worker;
					};
					as_worker = &this->workers_[i];
					::new (as_void) singly_node<__worker>(
							PULSAR_JOB_SYSTEM_WORKER_ALLOCATOR_SIZE,
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

			/// Destructor
			~__internal()
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

			/// Process
			bool __process_list(
					singly_cds_linked_lifo<__job> &__l)
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
			bool __process_list_steal(
					singly_cds_linked_lifo<__job> &__l)
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
			void __process_0()
			{
				singly_iterator j = this->mainlist_.clear();
				while (j)
				{
					j->__process();
					++j;
				}
			}
			void __process_with_workers()
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

			/// Link
			void __link_and_notify(
					singly_cds_linked_lifo<__job> &__l,
					singly_node<__job> *__j) pf_attr_noexcept
			{
				__l.insert_tail(__j);
				this->numJobs_.fetch_add(1, std::memory_order::release);
				this->numJobs_.notify_one();
			}
			void __link_0(
					singly_node<__job> *__j) pf_attr_noexcept
			{
				this->mainlist_.insert_tail(__j);
			}

			/// Job -> Unique Alloc
			singly_node<__job> *__create_unique_job(
					__thread_storage *__store,
					std::initializer_list<__task_initializer> const &__il)
			{
				union
				{
					void *as_void;
					singly_node<__job_unique> *as_unique_job;
					singly_node<__job> *as_job;
				};
				as_void = __store->allocator_.allocate(sizeof(singly_node<__job_unique>));
				auto b	= __il.begin();
				singly_link(
						__task_initializer_iterator(b),
						__task_initializer_iterator(__il.end()));
				::new (as_void) singly_node<__job_unique>(b->task_);
				return as_job;
			}

			/// Job -> Unique With Future Alloc
			singly_node<__job> *__create_unique_with_future_job(
					__thread_storage *__store,
					__future &__f,
					std::initializer_list<__task_initializer> const &__il)
			{
				union
				{
					void *as_void;
					singly_node<__job_unique_future> *as_unique_future_job;
					singly_node<__job> *as_job;
				};
				as_void = __store->allocator_.allocate(sizeof(singly_node<__job_unique_future>));
				auto b	= __il.begin();
				singly_link(
						__task_initializer_iterator(b),
						__task_initializer_iterator(__il.end()));
				::new (as_void) singly_node<__job_unique_future>(b->task_, &__f);
				return as_job;
			}


			/// Submit -> Unique
			void __submit(
					std::initializer_list<__task_initializer> const &__il)
			{
				auto store = localstore_;
				this->__link_and_notify(store->list_, this->__create_unique_job(store, __il));
			}
			void __submit_0(
					std::initializer_list<__task_initializer> const &__il)
			{
				this->__link_0(__create_unique_job(localstore_, __il));
			}

			/// Submit -> Unique with Future
			void __submit(
					__future &__f,
					std::initializer_list<__task_initializer> const &__il)
			{
				auto store = localstore_;
				this->__link_and_notify(store->list_, this->__create_unique_with_future_job(store, __f, __il));
			}
			void __submit_0(
					__future &__f,
					std::initializer_list<__task_initializer> const &__il)
			{
				this->__link_0(this->__create_unique_with_future_job(localstore_, __f, __il));
			}

			/// Submit -> Shared
			void __submit(
					singly_node<__job_shared> &__j,
					std::initializer_list<__task_initializer> const &__il)
			{
				auto store = localstore_;
				union
				{
					void *as_void;
					singly_node<__job_shared> *as_shared_job;
					singly_node<__job> *as_job;
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
			void __submit_0(
					singly_node<__job_shared> &__j,
					std::initializer_list<__task_initializer> const &__il)
			{
				union
				{
					void *as_void;
					singly_node<__job_shared> *as_shared_job;
					singly_node<__job> *as_job;
				};
				as_shared_job			 = &__j;
				singly_iterator it = &__j;
				if (it->__insert(__il.begin(), __il.end()))
				{
					this->__link_0(as_job);
				}
			}

			// Workers
			std::unique_ptr<singly_node<__worker>[]> workers_;
			singly_rotative_linked_list<__thread_storage> traveler_;

			// This Thread (Main)
			singly_cds_linked_lifo<__job> mainlist_;
			singly_node<__thread_storage> store_;
			singly_rotative_linked_list<__thread_storage>::iterator pos_;

			// Control
			byte_t cacheline1_[64];
			std::atomic_flag running_;
			byte_t cacheline2_[64];
			std::atomic<uint32_t> numJobs_;
			byte_t cacheline3_[64];
		};

		/// Type -> Worker
		struct __worker
		{
			/// Process -> Handle Exception
			pf_decl_static void __proc_for_exception_task_handle(
					__internal *__i,
					std::exception_ptr const &__e)
			{
				// Exception Ptr
				union pack
				{
					void *as_void;
					std::exception_ptr *as_exception_ptr;
				} as_pack;
				as_pack.as_void						= localstore_->allocator_.allocate(sizeof(std::exception_ptr));
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
			/// Process -> Work on jobs
			pf_decl_static thread_id_t __proc(
					__worker *__w,
					__internal *__i,
					thread_id_t __id)
			{
				localstore_			 = &(*__w->store_);
				this_thread::id_ = __id;
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

			/// Constructors
			__worker()
					: pos_(nullptr)
			{}
			__worker(
					size_t __wcs,
					__internal *__i,
					thread_id_t __id)
					: store_(__wcs)
					, pos_(&this->store_)
					, thread_(&__worker::__proc, this, __i, __id)
			{}

			// This Thread (Workers)
			singly_node<__thread_storage> store_;
			singly_rotative_linked_list<__thread_storage>::iterator pos_;

			// Thread
			std::thread thread_;
		};

		/// Type -> Future
		class __future
		{
			template <typename _RetTy>
				requires(!std::is_void_v<_RetTy>)
			pf_decl_friend class __future_with_value;
			pf_decl_friend class __job_unique_future;
			pf_decl_friend class __job_shared;
			pf_decl_friend class __task;

			/// Internal -> Set
			void __set_unfinished()
			{
				this->ID_ = std::numeric_limits<thread_id_t>::max();
				this->finished_.clear(std::memory_order::relaxed);
			}
			void __set_finished(
					thread_id_t __ID)
			{
				this->ID_ = __ID;
				this->finished_.test_and_set(std::memory_order::relaxed);
			}

			/// Internal -> Throw
			void __throw_if_not_finished() const
			{
				if (!this->is_finished())
					throw(exception(
							std::generic_category(),
							debugger::generic_code(std::errc::operation_not_permitted),
							"Isn't processed!"));
			}

		public:
			/// Constructors
			__future()
					: ID_(std::numeric_limits<thread_id_t>::max())
					, cacheline1_{ '\0' }
					, finished_(false)
					, cacheline2_{ '\0' }

			{}
			__future(__future const &) = delete;

			/// Is Finished?
			pf_hint_nodiscard bool is_finished() const pf_attr_noexcept
			{
				return this->finished_.test(std::memory_order::relaxed);
			}

			/// ID
			pf_hint_nodiscard thread_id_t processed_by() const
			{
				this->__throw_if_not_finished();
				return this->ID_;
			}

		private:
			thread_id_t ID_;
			byte_t cacheline1_[64];
			std::atomic_flag finished_;
			byte_t cacheline2_[64];
		};

		/// Type -> Future With Ret
		template <typename _RetTy>
			requires(!std::is_void_v<_RetTy>)
		class __future_with_value pf_attr_final: public __future
		{
			pf_decl_friend class __task;

			/// Internal -> Set Ret Val
			void __set_ret_val(_RetTy &&__val)
			{
				this->retval_ = std::forward<_RetTy>(__val);
			}

		public:
			/// Constructors
			__future_with_value()
			{}
			__future_with_value(
					__future_with_value const &) = delete;

			/// RetVal
			pf_hint_nodiscard const _RetTy &value() const
			{
				this->__throw_if_not_finished();
				return this->retval_;
			}

		private:
			_RetTy retval_;
		};

		/// Type -> Task
		struct __task pf_attr_final
		{
			/// Constructors
			__task(
					fun_ptr<void(void *)> __fun,
					void *__args)
					: fun_(__fun)
			{
				union
				{
					__task *as_task;
					void *as_void;
				};
				as_task = this + 1;
				as_void = __args;
			}
			template <typename _RetTy, typename... _Args, typename... _InArgs>
			__task(
					fun_ptr<_RetTy(_Args...)> __fun,
					_InArgs &&...__args)
					: fun_(nullptr)
			{
				// Pack
				struct pack
				{
					fun_ptr<_RetTy(_Args...)> fun;
					tuple<_InArgs...> args;
				};

				// Fun
				this->fun_ = [](void *__pack_args) -> void
				{
					union
					{
						void *as_void;
						pack *as_pack;
					};
					as_void = __pack_args;
					tuple_apply(as_pack->fun, as_pack->args);
					std::destroy_at(as_pack);
				};
				// Args
				union
				{
					__task *as_task;
					pack *as_pack;
				};
				as_task = this + 1;
				::new (&as_pack->fun) fun_ptr<_RetTy(_Args...)>(__fun);
				::new (&as_pack->args) tuple<_InArgs...>(std::forward<_InArgs>(__args)...);
			}
			template <typename... _Args, typename... _InArgs>
			__task(
					__future &__fut,
					fun_ptr<void(_Args...)> __fun,
					_InArgs &&...__args)
					: fun_(nullptr)
			{
				// Future
				__fut.__set_unfinished();
				// Pack
				struct pack
				{
					__future *future;
					fun_ptr<void(_Args...)> fun;
					tuple<_InArgs...> args;
				};

				// Fun
				this->fun_ = [](void *__pack_args) -> void
				{
					union
					{
						void *as_void;
						pack *as_pack;
					};
					as_void = __pack_args;
					tuple_apply(as_pack->fun, as_pack->args);
					pf_assert(!__fut.is_finished(), "Future shouldn't be finished!");
					as_pack->future->__set_finished(this_thread::ID());
					std::destroy_at(as_pack);
				};
				// Args
				union
				{
					__task *as_task;
					pack *as_pack;
				};
				as_task					= this + 1;
				as_pack->future = &__fut;
				::new (&as_pack->fun) fun_ptr<void(_Args...)>(__fun);
				::new (&as_pack->args) tuple<_InArgs...>(std::forward<_InArgs>(__args)...);
			}
			template <typename _RetTy, typename... _Args, typename... _InArgs>
			__task(
					__future_with_value<_RetTy> &__fut,
					fun_ptr<_RetTy(_Args...)> __fun,
					_InArgs &&...__args)
					: fun_(nullptr)
			{
				// Future
				__fut.__set_unfinished();
				// Pack
				struct pack
				{
					__future_with_value<_RetTy> *future;
					fun_ptr<_RetTy(_Args...)> fun;
					tuple<_InArgs...> args;
				};
				// Fun
				this->fun_ = [](void *__pack_args) -> void
				{
					union
					{
						void *as_void;
						pack *as_pack;
					};
					as_void = __pack_args;
					pf_assert(!as_pack->future.is_finished(), "Future shouldn't be finished!");
					as_pack->future->__set_ret_val(tuple_apply(as_pack->fun, as_pack->args));
					as_pack->future->__set_finished(this_thread::ID());
					std::destroy_at(as_pack);
				};
				// Args
				union
				{
					__task *as_task;
					pack *as_pack;
				};
				as_task					= this + 1;
				as_pack->future = &__fut;
				::new (&as_pack->fun) fun_ptr<_RetTy(_Args...)>(__fun);
				::new (&as_pack->args) tuple<_InArgs...>(std::forward<_InArgs>(__args)...);
			}
			__task(
					__task const &) = delete;

			/// Run
			void __run()
			{
				union
				{
					__task *as_task;
					void *as_void;
				};
				as_task = this + 1;
				this->fun_(as_void);
			}

			/// Data
			fun_ptr<void(void *)> fun_;
		};

		/// Type -> Job
		struct __job
		{
			enum type : size_t
			{
				UNIQUE,
				UNIQUE_FUTURE,
				SHARED
			};

			/// Constructors
			__job(
					type __type)
					: type_(__type)
			{}

			/// Job -> Process
			void __process()
			{
				union
				{
					__job *as_job;
					__job_unique *as_unique;
					__job_unique_future *as_unique_future;
					__job_shared *as_shared;
				};
				as_job = this;
				// Determinate
				switch (as_job->type_)
				{
					case __job::UNIQUE:
						return as_unique->__process();
					case __job::UNIQUE_FUTURE:
						return as_unique_future->__process();
					case __job::SHARED:
						return as_shared->__process();
				}
			}

			/// Data
			type type_;
		};
		struct __job_unique pf_attr_final: public __job
		{
			/// Constructors
			__job_unique(
					singly_iterator<__task> __beg)
					: __job(__job::type::UNIQUE)
					, beg_(__beg)
			{}

			/// Process
			void __process()
			{
				while (this->beg_)
				{
					this->beg_->__run();
					++this->beg_;
				}
			}

			/// Job -> Task
			singly_iterator<__task> beg_;
		};
		struct __job_unique_future pf_attr_final: public __job
		{
			/// Constructors
			__job_unique_future(
					singly_iterator<__task> __beg,
					__future *__future)
					: __job(__job::type::UNIQUE_FUTURE)
					, beg_(__beg)
					, future_(__future)
			{}

			/// Process
			void __process()
			{
				while (this->beg_)
				{
					this->beg_->__run();
					++this->beg_;
				}
				this->future_->__set_finished(this_thread::ID());
			}

			/// Job -> Task
			singly_iterator<__task> beg_;
			__future *future_;
		};

		/// Type -> Job External
		class __job_shared pf_attr_final: public __job
		{
			pf_decl_friend struct __job;
			pf_decl_friend class job_system;

		public:
			/// Constructors
			__job_shared()
					: __job(__job::type::SHARED)
			{}

			/// Future
			const __future &future() const pf_attr_noexcept
			{
				return this->future_;
			}

		private:
			/// Process
			void __process()
			{
				singly_iterator c = this->list_.clear();
				while (c)
				{
					c->__run();
					++c;
				}
				this->future_.__set_finished(this_thread::ID());
			}

			/// Insert
			bool __insert(
					__task_initializer_iterator __b,
					__task_initializer_iterator __e) pf_attr_noexcept
			{
				bool neednotify = this->list_.empty();
				this->list_.insert_tail_bulk(__b, __e);
				return neednotify;
			}

			singly_cds_linked_lifo<__task> list_;
			__future future_;
		};

		/// Type -> Task Initializer
		class __task_initializer
		{
			pf_decl_friend class job_system;
			pf_decl_friend singly_node<__task>;

		public:
			/// Constructors
			__task_initializer(
					fun_ptr<void(void *)> __fun,
					void *__arg = nullptr)
					: task_(nullptr)
			{
				union
				{
					void *as_void;
					singly_node<__task> *as_task;
				};
				as_void = localstore_->allocator_.allocate(sizeof(singly_node<__task>) + sizeof(__arg));
				new (as_void) singly_node<__task>(__fun, __arg);
				this->task_ = as_task;
			}
			template <typename _RetTy, typename... _Args, typename... _InArgs>
				requires(std::is_invocable_v<_RetTy(_Args...), _InArgs...>)
			__task_initializer(
					fun_ptr<_RetTy(_Args...)> __fun,
					_InArgs &&...__args)
					: task_(nullptr)
			{
				union
				{
					void *as_void;
					singly_node<__task> *as_task;
				};
				as_void = localstore_->allocator_.allocate(sizeof(singly_node<__task>) + sizeof(__fun) + sizeof(tuple<_InArgs...>));
				new (as_void) singly_node<__task>(__fun, std::forward<_InArgs>(__args)...);
				this->task_ = as_task;
			}
			template <typename... _Args, typename... _InArgs>
				requires(std::is_invocable_v<void(_Args...), _InArgs...>)
			__task_initializer(
					__future &__fut,
					fun_ptr<void(_Args...)> __fun,
					_InArgs &&...__args)
					: task_(nullptr)
			{
				union
				{
					void *as_void;
					singly_node<__task> *as_task;
				};
				as_void = localstore_->allocator_.allocate(sizeof(singly_node<__task>) + sizeof(__future *) + sizeof(__fun) + sizeof(tuple<_InArgs...>));
				new (as_void) singly_node<__task>(
						__fut,
						__fun,
						std::forward<_InArgs>(__args)...);
				this->task_ = as_task;
			}
			template <typename _RetTy, typename... _Args, typename... _InArgs>
				requires(std::is_invocable_v<_RetTy(_Args...), _InArgs...>)
			__task_initializer(
					__future_with_value<_RetTy> &__fut,
					fun_ptr<_RetTy(_Args...)> __fun,
					_InArgs &&...__args)
					: task_(nullptr)
			{
				union
				{
					void *as_void;
					singly_node<__task> *as_task;
				};
				as_void = localstore_->allocator_.allocate(sizeof(singly_node<__task>) + sizeof(__future_with_value<_RetTy> *) + sizeof(__fun) + sizeof(tuple<_InArgs...>));
				new (as_void) singly_node<__task>(
						__fut,
						__fun,
						std::forward<_InArgs>(__args)...);
				this->task_ = as_task;
			}

		private:
			singly_node<__task> *task_;
		};

		/// Type -> Job Initializer
		class __job_initializer
		{
			pf_decl_friend class job_system;

		private:
			__job *job_;
		};

	public:
		/// External -> Types
		template <typename _RetTy>
		using future_with_value = __future_with_value<_RetTy>;
		using future						= __future;
		using task_initializer	= __task_initializer;
		using job								= singly_node<__job_shared>;

		/// Constructors
		job_system() = delete;

		/// Submit -> Unique
		pf_decl_static void submit(
				std::initializer_list<task_initializer> const &__il)
		{
			instance_->__submit(__il);
		}
		pf_decl_static void submit_0(
				std::initializer_list<task_initializer> const &__il)
		{
			instance_->__submit_0(__il);
		}

		/// Submit -> Unique with Future
		pf_decl_static void submit(
				future &__f,
				std::initializer_list<task_initializer> const &__il)
		{
			instance_->__submit(__f, __il);
		}
		pf_decl_static void submit_0(
				future &__f,
				std::initializer_list<task_initializer> const &__il)
		{
			instance_->__submit_0(__f, __il);
		}

		/// Submit -> Shared
		pf_decl_static void submit(
				job &__j,
				std::initializer_list<task_initializer> const &__il)
		{
			instance_->__submit(__j, __il);
		}
		pf_decl_static void submit_0(
				job &__j,
				std::initializer_list<task_initializer> const &__il)
		{
			instance_->__submit_0(__j, __il);
		}

		/// 0 -> Process Local
		pf_decl_static void process_0()
		{
			instance_->__process_0();
		}
		pf_decl_static void process_with_workers()
		{
			instance_->__process_with_workers();
		}

		/// Initialized
		pf_hint_nodiscard pf_decl_static bool is_initialized()
		{
			return instance_ != nullptr;
		}

		/// Num Workers
		pf_hint_nodiscard pf_decl_static size_t num_workers()
		{
			return std::thread::hardware_concurrency() - 1;
		}

		/// Initializers
		pf_decl_static void init()
		{
			if (instance_)
				return;
			instance_ = std::make_unique<__internal>();
		}
		pf_decl_static void terminate()
		{
			if (!instance_)
				return;
			instance_.reset();
		}

	private:
		pf_decl_static pf_decl_inline std::unique_ptr<__internal> instance_;
		pf_decl_static pf_decl_inline pf_decl_thread_local __thread_storage *localstore_ = nullptr;
		pf_static_initializer(job_system)
	};

	/// JOB: External -> Selector
	template <typename _RetTy>
	struct select_future_by_ret_type
	{
		using type = job_system::future_with_value<_RetTy>;
	};
	template <>
	struct select_future_by_ret_type<void>
	{
		using type = job_system::future;
	};
	template <typename _RetTy>
	using select_future_by_ret_type_t = select_future_by_ret_type<_RetTy>::type;

	/// JOB: External -> Types
	template <typename _RetTy>
	using job_future					 = select_future_by_ret_type_t<_RetTy>;
	using job_task_initializer = job_system::task_initializer;
	using job									 = job_system::job;
}

#endif // !PULSAR_THREAD_JOB_SYSTEM