/**
 * @file    memory_locks.hpp
 * @author  Fluffy (noe.louis-quentin@hotmail.fr)
 * @brief
 * @date    15-12-2021
 *
 * @copyright Copyright (c) 2021 - Pulsar Software
 *
 */

#ifndef PULSAR_MEMORY_LOCK_HPP
#define PULSAR_MEMORY_LOCK_HPP 1

// Include: Pulsar
#include "pulsar/pulsar.hpp"

// Include: C++
#include <atomic>

// Pulsar
namespace pulsar
{
	// Atomic
	template <typename _Ty>
	using atomic			 = std::atomic<_Ty>;
	using atomic_flag	 = std::atomic_flag;
	using memory_order = std::memory_order;

	// Atomic Mutex
	/**
	 * @brief
	 *
	 */
	class atomic_mutex
	{
		pf_decl_friend class atomic_lock;

	public:
		// Constructor
		/**
		 * @brief
		 *
		 * @return pf_decl_explicit
		 */
		pf_decl_explicit atomic_mutex() pf_attr_noexcept: count_(0)
		{}
		/**
		 * @brief
		 *
		 * @param r
		 * @return pf_decl_explicit
		 */
		pf_decl_explicit atomic_mutex(atomic_mutex const &r) pf_attr_noexcept = delete;

		// Operator=
		/**
		 * @brief
		 *
		 * @param r
		 * @return atomic_mutex&
		 */
		atomic_mutex &operator=(atomic_mutex const &r) pf_attr_noexcept = delete;

	private:
		// Lock
		/**
		 * @brief
		 *
		 * @return int64_t
		 */
		int64_t lock() pf_attr_noexcept
		{
			int64_t i = this->count_.fetch_add(1, memory_order::release);
			if (i > 0) this->count_.wait(0, memory_order::relaxed);
			return i;
		}

		// Unlock
		/**
		 * @brief
		 *
		 * @return int64_t
		 */
		int64_t unlock() pf_attr_noexcept
		{
			int64_t i = this->count_.fetch_sub(1, memory_order::release);
			return i;
		}

		// Atomic lock
		atomic<int64_t> count_;
	};

	// Counter: Lock
	class atomic_lock
	{
	public:
		// Constructor
		/**
		 * @brief
		 *
		 * @param m
		 * @return pf_decl_explicit
		 */
		pf_decl_explicit atomic_lock(atomic_mutex &m) pf_attr_noexcept: mutex_(&m)
		{
			this->mutex_->lock();
		}
		/**
		 * @brief
		 *
		 * @return pf_decl_explicit
		 */
		pf_decl_explicit atomic_lock(atomic_lock const &) pf_attr_noexcept = delete;

		// Operator=
		/**
		 * @brief
		 *
		 * @return atomic_lock&
		 */
		atomic_lock &operator=(atomic_lock const &) pf_attr_noexcept = delete;

		// Destructor
		/**
		 * @brief Destroy the atomic lock object
		 *
		 */
		~atomic_lock() pf_attr_noexcept
		{
			this->mutex_->unlock();
		}

	private:
		atomic_mutex *mutex_;
	};

} // Pulsar

#endif // !PULSAR_MEMORY_LOCKS_HPP