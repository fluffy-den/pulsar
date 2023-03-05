/*! @file   allocator_lspsc_ring_buffer.hpp
 *  @author Louis-Quentin Noé (noe.louis-quentin@hotmail.fr)
 *  @brief  Thread local ring buffer
 *  @date   05-03-2023
 *
 *  @copyright Copyright (c) 2023 - Pulsar Software
 *
 *  @since 0.1.4
 */

#ifndef PULSAR_ALLOCATOR_TL_RING_BUFFER_HPP
#define PULSAR_ALLOCATOR_TL_RING_BUFFER_HPP 1

// https://github.com/utaal/spsc-bip-buffer

// Include: Pulsar
#include "pulsar/allocator.hpp"

// Pulsar
namespace pul
{
	/// ALLOCATOR: Thread Local Ring Buffer
	template <
		typename _Ty,
		typename _Magnifier>
	class allocator_tl_ring_buffer pf_attr_final
	{
	public:
		/// Constructors

		/// Destructor

		/// Operator =

	private:
	};
}

#endif // !PULSAR_ALLOCATOR_TL_RING_BUFFER_HPP