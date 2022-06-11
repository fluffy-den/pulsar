/*! @file   buffer_virtual.hpp
 *  @author Fluffy (noe.louis-quentin@hotmail.fr)
 *  @brief
 *  @date   11-06-2022
 *
 *  @copyright Copyright (c) 2022 - Pulsar Software
 *
 *  @since 0.1.1
 */

#ifndef PULSAR_MEMORY_BUFFER_VIRTUAL_HPP
#define PULSAR_MEMORY_BUFFER_VIRTUAL_HPP 1

// Include: Pulsar
#include "pulsar/memory.hpp"

// Pulsar
namespace pul
{
	// Memory
	namespace memory
	{
		/*! @brief
		 *
		 */
		class buffer_virtual
		{
		public:
			// TODO: Buffer Virtual

		private:
			byte_t *virtBeg_;
			byte_t *virtEnd_;
			byte_t *physBeg_;
			byte_t *physEnd_;
		};
	}
}

#endif // !PULSAR_MEMORY_BUFFER_VIRTUAL_HPP