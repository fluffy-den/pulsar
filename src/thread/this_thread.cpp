/*! @file   this_thread.cpp
 *  @author Fluffy (noe.louis-quentin@hotmail.fr)
 *  @brief
 *  @date   17-07-2022
 *
 *  @copyright Copyright (c) 2022 - Pulsar Software
 *
 *  @since 0.1.2
 */

// Include: Pulsar
#include "pulsar/thread/thread_identifier.hpp"

// Include: Src
#include "thread/job_system.hpp"

// Pulsar
namespace pul
{
	/// This_Thread
	thread_id_t this_thread::ID() pf_attr_noexcept
	{
		return __job_sys_internal::localstore_->id_;
	}
}
