/*! @file   debug.hpp
 *  @author Louis-Quentin Noé (noe.louis-quentin@hotmail.fr)
 *  @brief
 *  @date   19-03-2023
 *
 *  @copyright Copyright (c) 2023 - Pulsar Software
 *
 *  @since 0.1.5
 */

// Include: Pulsar
#include "pulsar/debug.hpp"
#include "pulsar/allocator.hpp"
#include "pulsar/concurrency.hpp"

#ifndef PULSAR_SRC_DEBUG_HPP
#define PULSAR_SRC_DEBUG_HPP 1

// Pulsar
namespace pul
{
	/// DEBUG: Internal Allocator -> Constant
	pf_decl_constexpr size_t DBG_ALLOCATOR_SIZE = 65536;

	/// DEBUG: Internal Allocator
	class __dbg_internal_allocator_t
	{
	public:
		/// Constructors
		__dbg_internal_allocator_t();
		__dbg_internal_allocator_t(__dbg_internal_allocator_t const &) = delete;
		__dbg_internal_allocator_t(__dbg_internal_allocator_t &&)			 = delete;

		/// Destructor
		~__dbg_internal_allocator_t() pf_attr_noexcept;

		/// Operator =
		__dbg_internal_allocator_t &operator=(
			__dbg_internal_allocator_t const&) = delete;
		__dbg_internal_allocator_t &operator=(
			__dbg_internal_allocator_t &&) = delete;

		/// Allocate
		pf_hint_nodiscard pf_decl_inline void*
		allocate(
			size_t __size,
			align_val_t __align = ALIGN_DEFAULT,
			size_t __offset			= 0) pf_attr_noexcept
		{
			return this->store_[(this_thread::id - 1) % CCY_NUM_THREADS].allocate(__size, __align, __offset);
		}

		/// Deallocate
		pf_decl_inline void
		deallocate(
			void *__ptr) pf_attr_noexcept
		{
			return allocator_samd_ring_buffer::deallocate(__ptr);
		}

	private:
		allocator_samd_ring_buffer *store_;// [a1][a2][a3]...[an]
	};

	/// DEBUG: Internal Allocator -> Instance
	pf_decl_extern __dbg_internal_allocator_t __dbg_internal_allocator;
}

#endif // !PULSAR_SRC_DEBUG_HPP