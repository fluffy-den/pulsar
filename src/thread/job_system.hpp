/*! @file   job_system.hpp
 *  @author Fluffy (noe.louis-quentin@hotmail.fr)
 *  @brief
 *  @date   14-05-2022
 *
 *  @copyright Copyright (c) 2022 - Pulsar Software
 *
 *  @since 0.1.1
 */

#ifndef SRC_PULSAR_JOB_SYSTEM_HPP
#define SRC_PULSAR_JOB_SYSTEM_HPP 1

// Include: Pulsar
#include "pulsar/pulsar.hpp"
#include "pulsar/utility/utility.hpp"

// Include: C++
#include <atomic>
#include <thread>


// Pulsar
namespace pulsar
{
	/// Thread
	namespace thread
	{
		/// THREAD: Job System Future
		template <typename _Ret>
		class future
		{
		private:
			std::atomic_flag finished_;
			_Ret retval_;
		};

		/// THREAD: Job System Task Function
		using task_fun = fun_ptr<void(void *, future *)>;

		/// THREAD: Job System Task
		class task
		{
		public:
			/// Process

		private:
			task_fun fun_;
			future *future_;
			void *data_;
		};

		pulsar_api std::unique_ptr<task, task_deleter> create_task(
				fun_ptr<_Fun> __fun,
				_Args &&...__args) pf_attr_noexcept;
		pulsar_api std::unique_ptr<task, task_deleter> create_task(
				fun_ptr<_Fun> __fun,
				future &__future,
				_Args &&...__args) pf_attr_noexcept;

		class task_deleter
		{};

		/// THREAD: Job System Job
		class job
		{
		public:
		private:
		};

		class job_deleter
		{};

		pulsar_api std::unique_ptr<job, job_deleter> create_job() pf_attr_noexcept;
		template <typename _Fun, typename... _Args>

		pulsar_api void attach_task(
				job &__job,
				task &__task) pf_attr_noexcept;
		template <typename _Iterator>
		pulsar_api void attach_task_bulk(
				job &__job,
				_Iterator __inbeg,
				_Iterator __inend) pf_attr_noexcept;




	}
}

#endif // SRC_PULSAR_JOB_SYSTEM_HPP