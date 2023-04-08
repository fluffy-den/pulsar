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

// Pulsar Tester -> Main
pul::int32_t
main()
{
	return pul::tester_engine.run();
}
