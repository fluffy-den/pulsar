/*! @file   pulsar_tester.cpp
 *  @author Louis-Quentin Noé (noe.louis-quentin@hotmail.fr)
 *  @brief
 *  @date   29-01-2023
 *
 *  @copyright Copyright (c) 2023 - Pulsar Software
 *
 *  @since 0.1.2
 */

// Include: Pulsar
#include "pulsar_tester/pulsar_tester.hpp"

// Pulsar
pul::int32_t
main()
{
	pf_print(pul::dbg_type::info, pul::dbg_level::high, "This is a message.");
	pf_print(pul::dbg_type::warning, pul::dbg_level::high, "This is a warning.\nWith a new line!\nAnd another one!");
	pf_print(pul::dbg_type::extension, pul::dbg_level::high, "This is an extension.");
	// pul::dbg_print_exception(pul::dbg_category_generic(), pul::union_cast<uint32_t>(pul::errv::unknown), "Blah!");
	return 0;
}