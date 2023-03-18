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
			const size_t idx;
			_Ty *ptr;
		};
		struct __cache_t
		{
			atomic<__node_t*> node;
			byte_t pad[union_cast<size_t>(CCY_ALIGN) - sizeof(atomic<__node_t*>)];
		};

		/// Type -> Buffer
		struct __buffer_t
		{
			// Flexible Arrays -> Disable warning
	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wpedantic"

			pf_alignas(CCY_ALIGN) atomic<size_t> counter;
			pf_alignas(CCY_ALIGN) atomic<__node_t*> head;
			pf_alignas(CCY_ALIGN) atomic<__node_t*> tail;
			pf_alignas(CCY_ALIGN) __cache_t cache[];

			// Flexible Arrays
	#pragma GCC diagnostic pop
		};

	public:

	private:
	};
}


#endif // !PULSAR_MPMC_LINKED_LIST_HPP