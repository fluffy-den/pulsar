/*! @file   chrono.hpp
 *  @author Louis-Quentin Noé (noe.louis-quentin@hotmail.fr)
 *  @brief
 *  @date   18-02-2023
 *
 *  @copyright Copyright (c) 2023 - Pulsar Software
 *
 *  @since 0.1.4
 */

#ifndef PULSAR_CHRONO_HPP
#define PULSAR_CHRONO_HPP 1

// Include: Pulsar
#include "pulsar.hpp"

// Include: C++
#include <chrono>

// Include: C
#include <ctime>

// Pulsar
namespace pul
{
	/// CHRONO: Types
	using time_t = std::time_t;
	using tm_t	 = std::tm;
	template <typename _Rep, typename _Period>
	using duration								= std::chrono::duration<_Rep, _Period>;
	using high_resolution_clock_t = std::chrono::high_resolution_clock;
	using high_resolution_point_t = std::chrono::high_resolution_clock::time_point;
	using nanoseconds_t						= std::chrono::nanoseconds;
	using microseconds_t					= std::chrono::microseconds;
	using milliseconds_t					= std::chrono::milliseconds;
	using seconds_t								= std::chrono::seconds;
	using minutes_t								= std::chrono::minutes;
	using hours_t									= std::chrono::hours;
	using days_t									= std::chrono::days;
	using weeks_t									= std::chrono::weeks;
	using months_t								= std::chrono::month;
	using years_t									= std::chrono::year;

	/// CHRONO: Cast
	template <typename _To, typename _From>
	pf_decl_inline pf_decl_constexpr duration_cast(
		_From const &__from)
	{
		return std::chrono::duration_cast<_To>(__from);
	}

	/// CHRONO: Literals
namespace chrono_literals
{
	using namespace std::chrono_literals;
}
}


#endif // !PULSAR_CHRONO_HPP