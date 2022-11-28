/*! @file   ratio.hpp
 *  @author Louis-Quentin NOE (noe.louis-quentin@hotmail.fr)
 *  @brief
 *  @date   02-11-2022
 *
 *  @copyright Copyright (c) 2022 - Pulsar Software
 *
 *  @since 0.1.2
 */

#ifndef PULSAR_RATIO_HPP
#define PULSAR_RATIO_HPP 1

// Include: Pulsar
#include "pulsar/pulsar.hpp"

// Include: C++
#include <ratio>
#include <chrono>

// Pulsar
namespace pul
{
	/// RATIO: Types
	template <size_t _Num, size_t _Denom = 1>
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

	/// RATIO: Distance / Size
	// TODO: Inches, Feet, miles...

	/// RATIO: Time
	using nanosecond_t	= std::chrono::nanoseconds;
	using microsecond_t = std::chrono::microseconds;
	using millisecond_t = std::chrono::milliseconds;
	using second_t			= std::chrono::seconds;
	using minute_t			= std::chrono::minutes;
	using hour_t				= std::chrono::hours;
	using day_t					= std::chrono::days;
	using week_t				= std::chrono::weeks;
	using month_t				= std::chrono::month;
	using year_t				= std::chrono::year;
	template <typename _Rep, typename _Period>
	using duration			= std::chrono::duration<_Rep, _Period>;
	using time_t				= std::time_t;
	using tm_t					= std::tm;
	// TODO: decades, centuries...

	/// RATIO: Time Cast
	template <typename _ToDuration, typename _Rep, typename _Period>
	pf_decl_constexpr pf_decl_inline _ToDuration
	duration_cast(
		std::chrono::duration<_Rep, _Period> const &__dur) pf_attr_noexcept
	{
		return std::chrono::duration_cast<_ToDuration>(__dur);
	}

	/// RATIO: Time Litterals
namespace time_litterals
{
	using namespace std::chrono_literals;
}

	/// RATIO: Speed
	// TODO: Lightspeed ratio
}

#endif // !PULSAR_RATIO_HPP