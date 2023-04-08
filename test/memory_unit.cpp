/*! @file   memory_unit.cpp
 *  @author Louis-Quentin Noé (noe.louis-quentin@hotmail.com)
 *  @brief
 *  @date   06-04-2023
 *
 *  @copyright Copyright (c) 2023 - Pulsar Software
 *
 *  @since 0.1.6
 */

// Include: Pulsar -> Tester
#include "pulsar_tester/pulsar_tester.hpp"

// Pulsar
namespace pul
{
	pt_pack(construction_or_assignment)
	{
		struct __constructing_or_assigning_t
		{
			/// Constructors
			__constructing_or_assigning_t(int32_t __i)
			{
				ignore = __i;
				pf_print("Constructor Called!\n");
			}

			/// Operator =
			__constructing_or_assigning_t &
			operator=(__constructing_or_assigning_t const &)
			{
				pf_print("Copy Operator= Called!\n");
				return *this;
			}
			__constructing_or_assigning_t &
			operator=(__constructing_or_assigning_t &&) = delete;
			__constructing_or_assigning_t &
			operator=(int32_t __i)
			{
				ignore = __i;
				pf_print("Assignment Operator= Called!\n");
				return *this;
			}
		};

		pt_unit(construct_unit)
		{
			__constructing_or_assigning_t *c = union_cast<__constructing_or_assigning_t *>(halloc(sizeof(__constructing_or_assigning_t)));
			construct(c, 103);
			assign(c, 485);
		}
	}

	// TODO: unique_ptr unit tests
	// TODO: shared_ptr unit tests
}	 // namespace pul
