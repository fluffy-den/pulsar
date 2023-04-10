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
	pf_decl_constexpr size_t CCY_TASKS_MAX_NUM	 = 8'192;
	pf_decl_constexpr size_t CCY_TASKS_MAX_NUM_0 = 4'096;

	/// Type -> Thread
	using __thread_t = std::thread;

	/// TASK: Pool -> Buffer
	struct __thread_pool_storage_t
	{
		/// Constructors
		__thread_pool_storage_t() pf_attr_noexcept;
		__thread_pool_storage_t(__thread_pool_storage_t const &) = delete;
		__thread_pool_storage_t(__thread_pool_storage_t &&)			 = delete;

		/// Destructor
		~__thread_pool_storage_t() pf_attr_noexcept = default;

		/// Thread
		pf_hint_nodiscard __thread_t *
		__get_thread(
		 uint32_t __index) pf_attr_noexcept;

		/// Operator =
		__thread_pool_storage_t &
		operator=(
		 __thread_pool_storage_t const &) = delete;
		__thread_pool_storage_t &
		operator=(
		 __thread_pool_storage_t &&) = delete;

		/// Store
		pf_alignas(CCY_ALIGN) atomic<bool> run;
		pf_alignas(CCY_ALIGN) atomic<uint32_t> numTasks;
		pf_alignas(CCY_ALIGN) atomic<uint32_t> numProcessing;
		// condition_variable_t cv;
		mpmc_lifo2<__task_t> queue;
		mpmc_lifo2<__task_t> queue0;	// Will use bulks
		byte_t store[];								// [t1][t2][tn-1]
																	//     workers
	};

	/// TASK: Pool
	class __thread_pool_t
	{
	public:
		/// Store -> Make
		pf_hint_nodiscard __thread_pool_storage_t *
		__make_storage();
		void
		__delete_storage(
		 __thread_pool_storage_t *__s) pf_attr_noexcept;

		/// Constructors
		__thread_pool_t();
		__thread_pool_t(__thread_pool_t const &) = delete;
		__thread_pool_t(__thread_pool_t &&)			 = delete;

		/// Destructor
		~__thread_pool_t() pf_attr_noexcept;

		/// Operator =
		__thread_pool_t &
		operator=(
		 __thread_pool_t const &) = delete;
		__thread_pool_t &
		operator=(
		 __thread_pool_t &&) = delete;

		/// Submit
		void
		__submit(
		 __task_t *__task);
		void
		__submit_0(
		 __task_t *__task);

		/// Process
		bool
		__process();
		uint32_t
		__process_0();

	private:
		/// Store
		__thread_pool_storage_t *buf_;
	};
}	 // namespace pul

#endif	// !PULSAR_SRC_THREAD_POOL_HPP
