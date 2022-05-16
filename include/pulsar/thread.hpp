/*! @file   thread.hpp
 *  @author Fluffy (noe.louis-quentin@hotmail.fr)
 *  @brief
 *  @date   23-01-2022
 *
 *  @copyright Copyright (c) 2022 - Pulsar Software
 *
 *  @since 0.1.1
 */

#ifndef PULSAR_THREAD_HPP
#define PULSAR_THREAD_HPP 1

// Include: Pulsar
#include "pulsar/pulsar.hpp"

// Include: C++
#include <sstream>
#include <thread>

// Pulsar
namespace pul
{
	// Thread Identifier
	using thread_id_t = int32_t;

	// This Thread
	namespace this_thread
	{
		/// ID
		/*! @brief
		 *
		 *  @return
		 */
		pf_decl_static thread_id_t get_id() pf_attr_noexcept
		{
			std::stringstream ss;
			ss << std::this_thread::get_id();
			return std::stoi(ss.str());
		}

		/// Thread Local ID
		pf_thread_local pf_decl_static pf_decl_inline thread_id_t ID = get_id();
	}

	/// Thread
	namespace thread
	{

		/// Job System
		template <typename _Ret>
		class future
		{
		private:
			std::atomic_flag finished_;
			_Ret retval_;
		};
		class task pf_attr_final
		{};
		class job pf_attr_final
		{};
		class job_deleter
		{};
		class task_deleter
		{};

		
	}
}

#endif // !PULSAR_THREAD_HPP