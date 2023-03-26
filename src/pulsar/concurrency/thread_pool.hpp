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
	/// task: Constants
	pf_decl_constexpr size_t CCY_TASKS_MAX_NUM					= 65536;
	pf_decl_constexpr size_t CCY_TASKS_MAX_NUM_0				= 131072;
	pf_decl_constexpr size_t CCY_CACHE_NUM0							= 32;
	pf_decl_constexpr size_t CCY_TASKS_ALLOCATOR_SIZE		= 131072;
	pf_decl_constexpr size_t CCY_TASKS_ALLOCATOR_SIZE_0 = 524288;

	/// Type -> Thread
	using __thread_t = std::thread;

	/// task: Pool
	class __thread_pool_t
	{
	private:
		/// Type -> Buffer
		struct __buffer_t
		{
			/// Constructors
			__buffer_t() pf_attr_noexcept;
			__buffer_t(__buffer_t const &) = delete;
			__buffer_t(__buffer_t &&)			 = delete;

			/// Destructor
			~__buffer_t() pf_attr_noexcept = default;

			/// Allocator
			pf_hint_nodiscard allocator_mamd_ring_buffer*
			__get_allocator(
				uint32_t __index) pf_attr_noexcept;

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
			pf_alignas(CCY_ALIGN) atomic<uint32_t> numRunning;
			mutex_t mutex;
			condition_variable_t cv;
			mpmc_lifo2<__task_t> queue;
			mpmc_lifo2<__task_t> queue0;// Will use bulks
			byte_t store[];	// [a1][a2]...[an] [t1][t2][tn-1]
											//   allocators        workers
		};

		/// Buffer -> Make
		pf_hint_nodiscard __buffer_t*
		__make_buffer();
		void
		__delete_buffer(
			__buffer_t *__buf) pf_attr_noexcept;

	public:
		/// Thread -> Process
		pf_decl_static int32_t
		__thread_process() pf_attr_noexcept;

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

		/// Allocate
		pf_hint_nodiscard void*
		__allocate(
			size_t __size) pf_attr_noexcept;
		void
		__deallocate(
			void *__ptr) pf_attr_noexcept;

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
		pf_decl_static pf_decl_inline __buffer_t *buf_ = nullptr;
	};

	/// CONCURRENCY: Thread Pool -> Instance
	pf_decl_extern __thread_pool_t __task_pool;
}

#endif // !PULSAR_SRC_THREAD_POOL_HPP