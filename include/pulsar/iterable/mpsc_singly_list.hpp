/*! @file   mpsc_linked_list.hpp
 *  @author Louis-Quentin Noé (noe.louis-quentin@hotmail.fr)
 *  @brief
 *  @date   17-03-2023
 *
 *  @copyright Copyright (c) 2023 - Pulsar Software
 *
 *  @since 0.1.4
 */

#ifndef PULSAR_MPMC_LINKED_LIST_HPP
#define PULSAR_MPMC_LINKED_LIST_HPP 1

// Include: Pulsar
#include "pulsar/debug.hpp"
#include "pulsar/memory.hpp"
#include "pulsar/utility.hpp"
#include "pulsar/concurrency.hpp"

// Pulsar
namespace pul
{
	/// SINGLY: MPSC List
	template <typename _Ty>
	class mpsc_singly_list
	{
	private:
		/// Type -> Node
		struct __node_t
		{
			__node_t *next;
			_Ty *store[];
		};

		/// Type -> Buffer
		struct __buffer_t
		{



		};

	public:

	private:

	};
}


#endif // !PULSAR_MPMC_LINKED_LIST_HPP