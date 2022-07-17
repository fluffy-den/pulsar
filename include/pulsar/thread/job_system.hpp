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
	/// JOB: System
	class job_system pf_attr_final
	{
		/// Types
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

		/// Types -> Internal
		pf_decl_friend struct __job_sys_worker;
		pf_decl_friend struct __job_sys_thread_storage;
		pf_decl_friend struct __job_sys_internal;

		/// Type -> Initializer Iterator
		struct __task_initializer_iterator
		{
		public:
			/// Constructor
			pf_decl_inline __task_initializer_iterator(const __task_initializer *__p)
					pf_attr_noexcept: ptr_(__p) {}
			pf_decl_inline __task_initializer_iterator(__task_initializer_iterator const &__r)
					: ptr_(__r.ptr_)
			{}

			/// Operator=
			pf_decl_inline __task_initializer_iterator &operator=(__task_initializer_iterator const &__r)
			{
				this->ptr_ = __r.ptr_;
				return *this;
			}

			/// Operator*
			pf_decl_inline singly_node<__task> &operator*() pf_attr_noexcept
			{
				return *ptr_->task_;
			}

			/// Operator++
			pf_decl_inline __task_initializer_iterator &operator++() pf_attr_noexcept
			{
				++this->ptr_;
				return *this;
			}

			/// Operator==
			pf_decl_inline bool operator==(__task_initializer_iterator const &__r)
					pf_attr_noexcept
			{
				return this->ptr_ == __r.ptr_;
			}

		private:
			const __task_initializer *ptr_;
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
			pf_decl_inline void __set_unfinished()
			{
				this->ID_ = std::numeric_limits<thread_id_t>::max();
				this->finished_.clear(std::memory_order::relaxed);
			}
			pf_decl_inline void __set_finished(
					thread_id_t __ID)
			{
				this->ID_ = __ID;
				this->finished_.test_and_set(std::memory_order::relaxed);
			}

			/// Internal -> Throw
			pf_decl_inline void __throw_if_not_finished() const
			{
				if (!this->is_finished())
					throw(exception(
							std::generic_category(),
							debugger::generic_code(std::errc::operation_not_permitted),
							"Isn't processed!"));
			}

		public:
			/// Constructors
			pf_decl_inline __future()
					: ID_(std::numeric_limits<thread_id_t>::max())
					, cacheline1_{ '\0' }
					, finished_(false)
					, cacheline2_{ '\0' }

			{}
			pf_decl_inline __future(__future const &) = delete;

			/// Is Finished?
			pf_hint_nodiscard pf_decl_inline bool is_finished() const pf_attr_noexcept
			{
				return this->finished_.test(std::memory_order::relaxed);
			}

			/// ID
			pf_hint_nodiscard pf_decl_inline thread_id_t processed_by() const
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
			pf_decl_inline void __set_ret_val(_RetTy &&__val)
			{
				this->retval_ = std::forward<_RetTy>(__val);
			}

		public:
			/// Constructors
			pf_decl_inline __future_with_value()
			{}
			pf_decl_inline __future_with_value(
					__future_with_value const &) = delete;

			/// RetVal
			pf_hint_nodiscard pf_decl_inline const _RetTy &value() const
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
			pf_decl_inline __task(
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
			pf_decl_inline __task(
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
			pf_decl_inline __task(
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
			pf_decl_inline __task(
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
			pf_decl_inline __task(
					__task const &) = delete;

			/// Run
			pf_decl_inline void __run()
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
			pf_decl_inline __job(
					type __type)
					: type_(__type)
			{}

			/// Job -> Process
			pf_decl_inline void __process()
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

		/// Type -> Job -> Unique
		struct __job_unique pf_attr_final: public __job
		{
			/// Constructors
			pf_decl_inline __job_unique(
					singly_iterator<__task> __beg)
					: __job(__job::type::UNIQUE)
					, beg_(__beg)
			{}

			/// Process
			pf_decl_inline void __process()
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

		/// Type -> Job -> Unique with Future
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
			pf_decl_friend struct __job_sys_internal;
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
			pf_decl_friend struct __job_sys_internal;
			pf_decl_friend class job_system;
			pf_decl_friend singly_node<__task>;

		public:
			/// Constructors
			pf_decl_inline __task_initializer(
					fun_ptr<void(void *)> __fun,
					void *__arg = nullptr)
					: task_(nullptr)
			{
				union
				{
					void *as_void;
					singly_node<__task> *as_task;
				};
				as_void = __local_allocate(sizeof(singly_node<__task>) + sizeof(__arg));
				new (as_void) singly_node<__task>(__fun, __arg);
				this->task_ = as_task;
			}
			template <typename _RetTy, typename... _Args, typename... _InArgs>
				requires(std::is_invocable_v<_RetTy(_Args...), _InArgs...>)
			pf_decl_inline __task_initializer(
					fun_ptr<_RetTy(_Args...)> __fun,
					_InArgs &&...__args)
					: task_(nullptr)
			{
				union
				{
					void *as_void;
					singly_node<__task> *as_task;
				};
				as_void = __local_allocate(sizeof(singly_node<__task>) + sizeof(__fun) + sizeof(tuple<_InArgs...>));
				new (as_void) singly_node<__task>(__fun, std::forward<_InArgs>(__args)...);
				this->task_ = as_task;
			}
			template <typename... _Args, typename... _InArgs>
				requires(std::is_invocable_v<void(_Args...), _InArgs...>)
			pf_decl_inline __task_initializer(
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
				as_void = __local_allocate(sizeof(singly_node<__task>) + sizeof(__future *) + sizeof(__fun) + sizeof(tuple<_InArgs...>));
				new (as_void) singly_node<__task>(
						__fut,
						__fun,
						std::forward<_InArgs>(__args)...);
				this->task_ = as_task;
			}
			template <typename _RetTy, typename... _Args, typename... _InArgs>
				requires(std::is_invocable_v<_RetTy(_Args...), _InArgs...>)
			pf_decl_inline __task_initializer(
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
				as_void = __local_allocate(sizeof(singly_node<__task>) + sizeof(__future_with_value<_RetTy> *) + sizeof(__fun) + sizeof(tuple<_InArgs...>));
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

		/// Internal -> Allocate
		pulsar_api pf_decl_static void *__local_allocate(
				size_t __size) pf_attr_noexcept;

	public:
		/// External -> Types
		template <typename _RetTy>
		using future_with_value = __future_with_value<_RetTy>;
		using future						= __future;
		using task_initializer	= __task_initializer;
		using job								= singly_node<__job_shared>;

		/// Constructors
		/*! @brief
		 *
		 *  @return pulsar_api
		 */
		pulsar_api job_system();
		/*! @brief
		 *
		 *  @param[in] __r
		 */
		job_system(job_system const &__r) = delete;
		/*! @brief
		 *
		 *  @param[in] __r
		 */
		job_system(job_system &&__r)			= delete;

		/// Destructor
		/*! @brief
		 *
		 *  @return pulsar_api
		 */
		pulsar_api ~job_system();

		/// Submit -> Unique
		/*! @brief
		 *
		 *  @param[in] __il
		 *  @return pulsar_api
		 */
		pulsar_api pf_decl_static void submit(
				std::initializer_list<task_initializer> const &__il);
		/*! @brief
		 *
		 *  @param[in] __il
		 *  @return pulsar_api
		 */
		pulsar_api pf_decl_static void submit_0(
				std::initializer_list<task_initializer> const &__il);

		/// Submit -> Unique with Future
		/*! @brief
		 *
		 *  @param[in] __f
		 *  @param[in] __il
		 *  @return pulsar_api
		 */
		pulsar_api pf_decl_static void submit(
				future &__f,
				std::initializer_list<task_initializer> const &__il);
		/*! @brief
		 *
		 *  @param[in] __f
		 *  @param[in] __il
		 *  @return pulsar_api
		 */
		pulsar_api pf_decl_static void submit_0(
				future &__f,
				std::initializer_list<task_initializer> const &__il);

		/// Submit -> Shared
		/*! @brief
		 *
		 *  @param[in] __j
		 *  @param[in] __il
		 *  @return pulsar_api
		 */
		pulsar_api pf_decl_static void submit(
				job &__j,
				std::initializer_list<task_initializer> const &__il);
		/*! @brief
		 *
		 *  @param[in] __j
		 *  @param[in] __il
		 *  @return pulsar_api
		 */
		pulsar_api pf_decl_static void submit_0(
				job &__j,
				std::initializer_list<task_initializer> const &__il);

		/// 0 -> Process Local
		/*! @brief
		 *
		 *  @return pulsar_api
		 */
		pulsar_api pf_decl_static void process();
		/*! @brief
		 *
		 *  @return pulsar_api
		 */
		pulsar_api pf_decl_static void process_0();

		/// Num Workers
		/*! @brief
		 *
		 *  @return pf_hint_nodiscard
		 */
		pf_hint_nodiscard pf_decl_static size_t num_workers()
		{
			return std::thread::hardware_concurrency() - 1;
		}
	};

	/// JOB: System -> Initializer
	pulsar_api pf_decl_extern const job_system __job_system;

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