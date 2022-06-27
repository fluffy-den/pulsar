/*! @file   thread_identifier.hpp
 *  @author Fluffy (noe.louis-quentin@hotmail.fr)
 *  @brief
 *  @date   27-06-2022
 *
 *  @copyright Copyright (c) 2022 - Pulsar Software
 *
 *  @since 0.1.2
 */

#ifndef PULSAR_THREAD_IDENTIFIER_HPP
#define PULSAR_THREAD_IDENTIFIER_HPP 1

// Include: Pulsar
#include "pulsar/pulsar.hpp"

// Pul
namespace pul
{
	/// THREAD: ID
	using thread_id_t = size_t;

	/// This_Thread
	class this_thread
	{
		pf_decl_friend class job_system;

	public:
		/// Constructor
		this_thread() = delete;

		/// ID
		pf_decl_inline pf_decl_static thread_id_t ID() pf_attr_noexcept
		{
			return id_;
		}

	private:
		pf_decl_static pf_decl_inline pf_decl_thread_local thread_id_t id_ = 0;
	};
}

#endif // !PULSAR_THREAD_IDENTIFIER_HPP