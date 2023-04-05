/*! @file   thread_pool.hpp
 *  @author Louis-Quentin Noé (noe.louis-quentin@hotmail.fr)
 *  @brief
 *  @date   18-03-2023
 *
 *  @copyright Copyright (c) 2023 - Pulsar Software
 *
 *  @since 0.1.5
 */

#ifndef PULSAR_SRC_THREAD_POOL_HPP
#define PULSAR_SRC_THREAD_POOL_HPP 1

// Include: Pulsar
#include "pulsar/pulsar.hpp"
#include "pulsar/thread_pool.hpp"
#include "pulsar/allocator.hpp"
#include "pulsar/iterable.hpp"
#include "pulsar/tuple.hpp"
#include "pulsar/memory.hpp"

// Pulsar
namespace pul
{
	/// TASK Constants
	pf_decl_constexpr size_t CCY_TASKS_MAX_NUM	 = 8192;
	pf_decl_constexpr size_t CCY_TASKS_MAX_NUM_0 = 4096;

	/// Type -> Thread
	using __thread_t = std::thread;

	/// TASK: Pool
	class __thread_pool_t
	{
	public:
		/// Type -> Buffer
		struct __buffer_t
		{
			/// Constructors
			__buffer_t() pf_attr_noexcept;
			__buffer_t(__buffer_t const &) = delete;
			__buffer_t(__buffer_t &&)			 = delete;

			/// Destructor
			~__buffer_t() pf_attr_noexcept = default;

			/// Thread
			pf_hint_nodiscard __thread_t*
			__get_thread(
				uint32_t __index) pf_attr_noexcept;

			/// Operator =
			__buffer_t &operator=(
				__buffer_t const &) = delete;
			__buffer_t &operator=(
				__buffer_t &&) = delete;

			/// Store
			pf_alignas(CCY_ALIGN) atomic<bool> run;
			pf_alignas(CCY_ALIGN) atomic<uint32_t> numTasks;
			pf_alignas(CCY_ALIGN) atomic<uint32_t> numProcessing;
			mutex_t mutex;
			condition_variable_t cv;
			mpmc_lifo2<__task_t> queue;
			mpmc_lifo2<__task_t> queue0;// Will use bulks
			byte_t store[];	// [t1][t2][tn-1]
											//     workers
		};

		/// Buffer -> Make
		pf_hint_nodiscard __buffer_t*
		__make_buffer();
		void
		__delete_buffer(
			__buffer_t *__buf) pf_attr_noexcept;

		/// Constructors
		__thread_pool_t();
		__thread_pool_t(__thread_pool_t const &) = delete;
		__thread_pool_t(__thread_pool_t &&)			 = delete;

		/// Destructor
		~__thread_pool_t() pf_attr_noexcept;

		/// Operator =
		__thread_pool_t &operator=(
			__thread_pool_t const&) = delete;
		__thread_pool_t &operator=(
			__thread_pool_t &&) = delete;

		/// Submit
		void __submit(
			__task_t *__task);
		void __submit_0(
			__task_t *__task);

		/// Process
		bool __process();
		uint32_t __process_0();

	private:
		/// Store
		__buffer_t *buf_;
	};
}

#endif // !PULSAR_SRC_THREAD_POOL_HPP