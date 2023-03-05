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
	/// DEBUG: Ring Buffer -> Constant
	pf_decl_constexpr uint32_t DBG_RING_BUFFER_GROW_SIZE = 65536;

	/// DEBUG: Ring Buffer -> Header
	struct __dbg_ring_buffer_header_t
	{
		size_t offset : sizeof(size_t) - 1;
		size_t bit : 1;	// Multithreaded should be deallocated
		byte_t store[];
	};

	/// DEBUG: Ring Buffer -> Block
	struct __dbg_ring_buffer_block_t
	{
		/// Allocate
		pf_hint_nodiscard void*
		allocate(
			size_t __size)
		{
			// TODO
		}

		/// Deallocate
		void
		deallocate(
			void *__ptr)
		{
			// TODO
		}

		__dbg_ring_buffer_block_t *next;
		__dbg_ring_buffer_header_t *head;
		__dbg_ring_buffer_header_t *tail;
		size_t size;
		byte_t *store;
	};

	/// DEBUG: Ring Buffer Local
	class __dbg_ring_buffer_local_t
	{
	public:
		/// Constructors
		__dbg_ring_buffer_local_t() pf_attr_noexcept
			: head_(nullptr)
			, tail_(nullptr)
			, nextGrow_(DBG_RING_BUFFER_GROW_SIZE)
		{}
		__dbg_ring_buffer_local_t(__dbg_ring_buffer_local_t const &) = delete;
		__dbg_ring_buffer_local_t(__dbg_ring_buffer_local_t &&)			 = delete;

		/// Destructor
		~__dbg_ring_buffer_local_t() pf_attr_noexcept
		{
			// TODO: Deallocate all blocks
		}

		/// Operator =
		__dbg_ring_buffer_local_t &operator=(__dbg_ring_buffer_local_t const &) = delete;
		__dbg_ring_buffer_local_t &operator=(__dbg_ring_buffer_local_t &&)			= delete;

		/// Allocate
		pf_hint_nodiscard void*
		allocate(
			size_t __size)
		{
			// TODO
		}

		/// Deallocate
		void deallocate(
			void *__ptr)
		{
			// TODO
		}

	private:
		__dbg_ring_buffer_block_t *head_;
		__dbg_ring_buffer_block_t *tail_;
		size_t nextGrow_;
	};
}

#endif // !PULSAR_DEBUG_RING_BUFFER_HPP