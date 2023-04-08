/*! @file   ratio.hpp
 *  @author Louis-Quentin Noé (noe.louis-quentin@hotmail.fr)
 *  @brief
 *  @date   05-03-2023
 *
 *  @copyright Copyright (c) 2023 - Pulsar Software
 *
 *  @since 0.1.4
 */

#ifndef PULSAR_RATIO_HPP
#define PULSAR_RATIO_HPP 1

// Include: Pulsar
#include "pulsar/pulsar.hpp"

// Include: C++
#include <ratio>

// Pulsar
namespace pul
{
	/// UTILITY: Ratio
	template<size_t _Num, size_t _Denom = 1>
	using ratio = std::ratio<_Num, _Denom>;
	using atto	= std::atto;
	using femto = std::femto;
	using pico	= std::pico;
	using nano	= std::nano;
	using micro = std::micro;
	using milli = std::milli;
	using centi = std::centi;
	using deci	= std::deci;
	using deca	= std::deca;
	using hecto = std::hecto;
	using kilo	= std::kilo;
	using mega	= std::mega;
	using giga	= std::giga;
	using tera	= std::tera;
	using peta	= std::peta;
	using exa		= std::exa;

	/// UTILITY: Ratio -> Distance / Size
	// TODO: Inches, Feet, miles ratios
}	 // namespace pul

#endif	// !PULSAR_RATIO_HPP
