/*! @file   thread_pool.hpp
 *  @author Louis-Quentin Noé (noe.louis-quentin@hotmail.fr)
 *  @brief
 *  @date   18-03-2023
 *
 *  @copyright Copyright (c) 2023 - Pulsar Software
 *
 *  @since 0.1.5
 */

#ifndef PULSAR_THREAD_POOL_HPP
#define PULSAR_THREAD_POOL_HPP 1

// Include: Pulsar
#include "pulsar/pulsar.hpp"
#include "pulsar/debug.hpp"
#include "pulsar/concurrency.hpp"
#include "pulsar/iterable.hpp"

// Pulsar
namespace pul
{
	/// task: Constants
	pf_decl_constexpr size_t taskS_MAX_NUM				= 65536 * 16;
	pf_decl_constexpr size_t taskS_MAX_NUM_0			= 4096 * 16;
	pf_decl_constexpr size_t taskS_ALLOCATOR_SIZE = 16777216 * 16;

	/// task: Pool
	class __task_pool_t
	{
	private:
		/// Type -> Task
		struct __task_t
		{
			/// Store
			fun_ptr<void(void*)> fun;
			byte_t store[];
		};

		/// Type -> Buffer
		struct __buffer_t
		{
			/// Constructors
			__buffer_t() pf_attr_noexcept;
			__buffer_t(__buffer_t const &) = delete;
			__buffer_t(__buffer_t &&)			 = delete;

			/// Destructor
			~__buffer_t() pf_attr_noexcept;

			/// Operator =
			__buffer_t &operator=(
				__buffer_t const &) = delete;
			__buffer_t &operator=(
				__buffer_t &&) = delete;

			/// Get*
			pf_hint_nodiscard allocator_samd_ring_buffer*
			__get_allocator(
				uint32_t __index) pf_attr_noexcept;
			pf_hint_nodiscard thread_t*
			__get_thread_handle(
				uint32_t __index) pf_attr_noexcept;

			/// Store
			pf_alignas(CCY_ALIGN) atomic<size_t> numTasks;
			pf_alignas(CCY_ALIGN) atomic<bool> running;
			mutex_t mutex;
			condition_variable_t cv;
			mpmc_queue2<__task_t> queue;
			mpmc_queue2<__task_t> queue0;
			byte_t store[];	// [a1][a2]...[an] [t1][t2][tn-1]
											//   allocators        workers
		};

	public:
		/// Buffer -> Make
		pf_hint_nodiscard __buffer_t*
		__make_buffer() pf_attr_noexcept
		{
			__buffer_t *b = new_construct_ex<__buffer_t>(sizeof(thread_t) * CCY_NUM_WORKERS);
			std::memset(++b, 0, sizeof(thread_t) * CCY_NUM_WORKERS);
			return b;
		}

		/// Buffer -> Delete
		pf_hint_nodiscard void
		__delete_buffer(
			__buffer_t *__buf) pf_attr_noexcept
		{
			destroy_delete(__buf);
		}

		/// Constructors
		__task_pool_t()
			: buf_(this->__make_buffer())
		{}
		__task_pool_t(__task_pool_t const &) = delete;
		__task_pool_t(__task_pool_t &&)			 = delete;

		/// Destructor
		~__task_pool_t() pf_attr_noexcept
		{
			this->__delete_buffer(this->buf_);
		}

		/// Operator =
		__task_pool_t &operator=(
			__task_pool_t const&) = delete;
		__task_pool_t &operator=(
			__task_pool_t &&) = delete;

		/// Submit
		// TODO

		/// Submit_0
	// TODO

	private:
		/// Store
		__buffer_t *buf_;
	};

	/// task: Pool -> Instance
	pf_decl_extern __task_pool_t __task_pool;
}

#endif // !PULSAR_THREAD_POOL_HPP