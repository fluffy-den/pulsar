/*! @file   concurrency.hpp
 *  @author Louis-Quentin Noé (noe.louis-quentin@hotmail.fr)
 *  @brief  Implementation of atomic (thread-safe) containers.
 *  @date   31-12-2022
 *
 *  @copyright Copyright (c) 2023 - Pulsar Software
 *
 *  @since 0.1.4
 */

#ifndef PULSAR_CONCURRENCY_HPP
#define PULSAR_CONCURRENCY_HPP 1

// Include: Pulsar
#include "pulsar/pulsar.hpp"
#include "pulsar/utility.hpp"
#include "pulsar/chrono.hpp"

// Include: C++
#include <thread>
#include <atomic>
#include <mutex>
#include <shared_mutex>
#include <condition_variable>

// Pulsar
namespace pul
{
	/// CONCURRENCY: Constants
	pf_decl_inline uint32_t CCY_NUM_THREADS								 = std::thread::hardware_concurrency();
	pf_decl_inline uint32_t CCY_NUM_WORKERS								 = CCY_NUM_THREADS - 1;
	pf_decl_inline pf_decl_constexpr align_val_t CCY_ALIGN = align_val_t(64);

	/// CONCURRENCY: Atomic
	template<typename _Ty>
	using atomic			 = std::atomic<_Ty>;
	using atomic_order = std::memory_order;

	/// CONCURRENCY: Lock
	using mutex_t = std::mutex;
	template<typename _Lock>
	using lock_unique = std::unique_lock<_Lock>;

	/// CONCURRENCY: Condition Variable
	using condition_variable_t = std::condition_variable;

	/// CONCURRENCY: Thread
	using thread_id_t = uint32_t;

	/// CONCURRENCY: This Thread
	namespace this_thread
	{
		pf_decl_inline void
		sleep_for(
		 nanoseconds_t __s)
		{
			std::this_thread::sleep_for(__s);
		}
		pf_decl_inline void
		sleep_until(
		 high_resolution_point_t __p)
		{
			std::this_thread::sleep_until(__p);
		}
		pf_decl_inline void
		yield()
		{
			std::this_thread::yield();
		}
		pf_hint_nodiscard pf_decl_inline thread_id_t
		get_id() pf_attr_noexcept
		{
			return union_cast<thread_id_t>(std::this_thread::get_id()) - 1;
		}
	}	 // namespace this_thread
}	 // namespace pul

#endif	// !PULSAR_CONCURRENCY_HPP
