/**
 * @file    memory_locks.hpp
 * @author  Fluffy (noe.louis-quentin@hotmail.fr)
 * @brief		Defines atomic mutex objects.
 * @date    15-12-2021
 *
 * @copyright Copyright (c) 2021 - Pulsar Software
 *
 */

#ifndef PULSAR_ATOMIC_MUTEX_HPP
#define PULSAR_ATOMIC_MUTEX_HPP 1

// Include: Pulsar
#include "pulsar/utils/atomic.hpp"

// Include: C++
#include <atomic>

// Pulsar
namespace pulsar
{
	/// Atomic Mutex
	/**
	 * @brief	Atomic mutex object.
	 *				With an atomic variable, this class manages the blocking of a thread execution in a
	 *				multithreaded context in order to avoid the execution of a critical part of a code.
	 *				The interest of this object is to redefine the mutexes proposed by the standard library
	 *				by rewriting the concept of a semaphore in a cross-platform way.
	 *				This class must be used in combination with atomic_lock @see atomic_lock.
	 */
	class atomic_mutex
	{
		pf_decl_friend class atomic_lock;

	public:
		/// Constructors
		/**
		 * @brief Default constructor.
		 * 				Initializes the atomic variable to 0.
		 */
		pf_decl_explicit atomic_mutex() pf_attr_noexcept
				: count_(0)
				, locked_(false)
		{}
		/**
		 * @brief Copy constructor.
		 * 				The instances of this object are unique.
		 *
		 * @param __other Another instancied atomic_mutex type.
		 */
		pf_decl_explicit atomic_mutex(atomic_mutex const &__other) pf_attr_noexcept = delete;

		/// Destructor
		/**
		 * @brief Destructor.
		 * 				Automatically unlocks the mutex if is was locked.
		 */
		~atomic_mutex()
		{
			if (locked_) this->unlock();
		}

		/// Operator=
		/**
		 * @brief Copy assignment.
		 * 				The instances of this object are unique.
		 *
		 * @param __other Another instancied atomic_mutex type.
		 */
		atomic_mutex &operator=(atomic_mutex const &__other) pf_attr_noexcept = delete;

	private:
		/// Lock
		/**
		 * @brief Increment the atomic variable by 1.
		 * 				If the old value returned by the increment returns 0, the current thread gains
		 * 				ownership of the mutex, blocking the other threads from executing this function until
		 * 				the thread with the mutex calls the unlock function.
		 * 			  The old incremented value is used to identify the threads.
		 *
		 * @return The old value returned by the increment of the atomic variable.
		 * 				 -1 if the mutex was locked.
		 */
		int64_t lock() pf_attr_noexcept
		{
			if (locked_)
				return -1;
			int64_t i = this->count_.fetch_add(1, memory_order::store);
			if (i > 0) this->count_.wait(i + 1, memory_order::relaxed);
			locked_ = true;
			return i;
		}

		/// Unlock
		/**
		 * @brief Decreases the value of the atomic variable by 1.
		 * 				Notifies blocked threads that the atomic variable has changed its value.
		 *
		 * @return The old value returned by the decrement of the atomic variable.
		 * 				 -1 if the mutex was unlocked.
		 */
		int64_t unlock() pf_attr_noexcept
		{
			if (!locked_)
				return -1;
			int64_t i = this->count_.fetch_sub(1, memory_order::store);
			this->count_.notify_one();
			locked_ = false;
			return i;
		}

		/// Atomic lock
		std::atomic<int64_t> count_;
		pf_decl_thread_local pf_decl_static pf_decl_inline bool locked_;
	};

	/// Atomic Lock
	/**
	 * @brief Atomic mutex lock.
	 *			  Automatically lock the targeted mutex on construction, and unlock it on destruction.
	 */
	class atomic_lock
	{
	public:
		/// Constructor
		/**
		 * @brief Constructor.
		 * 				Lock the targeted mutex on construction.
		 *
		 * @param m
		 * @return pf_decl_explicit
		 */
		pf_decl_explicit atomic_lock(atomic_mutex &m)
				: mutex_(&m)
		{
			// TODO: Exception m == nullptr
			this->mutex_->lock();
		}
		/**
		 * @brief Copy constructor.
		 * 				A lock isn't copy constructible.
		 */
		pf_decl_explicit atomic_lock(atomic_lock const &) pf_attr_noexcept = delete;

		/// Operator=
		/**
		 * @brief Copy asignment.
		 * 				A lock isn't copy assignable.
		 */
		atomic_lock &operator=(atomic_lock const &) pf_attr_noexcept = delete;

		/// Destructor
		/**
		 * @brief Destroy the atomic lock object.
		 */
		~atomic_lock() pf_attr_noexcept
		{
			this->mutex_->unlock();
		}

	private:
		atomic_mutex *mutex_;
	};

} // Pulsar

#endif // !PULSAR_ATOMIC_MUTEX_HPP