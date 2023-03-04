/*! @file   debug_ring_buffer.hpp
 *  @author Louis-Quentin Noé (noe.louis-quentin@hotmail.fr)
 *  @brief
 *  @date   04-03-2023
 *
 *  @copyright Copyright (c) 2023 - Pulsar Software
 *
 *  @since 0.1.3
 */

#ifndef PULSAR_DEBUG_RING_BUFFER_HPP
#define PULSAR_DEBUG_RING_BUFFER_HPP 1

// Include: Pulsar
#include "pulsar/pulsar.hpp"

// Pulsar
namespace pul
{
	/// DEBUG: Ring Buffer Local
	class __dbg_ring_buffer_local
	{
	};

	/// DEBUG: Ring Buffer
	class __dbg_ring_buffer
	{
	public:

	private:
		pf_decl_static pf_decl_inline pf_decl_thread_local __dbg_ring_buffer_local all_;
	};
}

#endif // !PULSAR_DEBUG_RING_BUFFER_HPP