/*! @file   mpsc_singly_list.hpp
 *  @author Louis-Quentin Noé (noe.louis-quentin@hotmail.fr)
 *  @brief
 *  @date   09-03-2023
 *
 *  @copyright Copyright (c) 2023 - Pulsar Software
 *
 *  @since 0.1.4
 */

#ifndef PULSAR_MPSC_SINGLY_LIST_HPP
#define PULSAR_MPSC_SINGLY_LIST_HPP 1

static uint32_t i = 0;

void blah()
{
	i += 1;
}


// mpsc_singly_list<job> j

// j.submit(blah);
// j.submit(blah);


// Include: Pulsar
#include "pulsar/pulsar.hpp"
#include "pulsar/debug.hpp"
#include "pulsar/magnifier.hpp"
#include "pulsar/allocator.hpp"

#endif // !PULSAR_MPSC_SINGLY_LIST_HPP


enum behaviours_t
{
	SAFE,
	COMBAT,
	STEALTH
};

static behaviours_t bevaviours = behaviours_t::STEALH;

if(bevaviours & behaviours_t::STEALTH)