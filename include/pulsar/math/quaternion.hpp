/*! @file   quaternion.hpp
 *  @author Louis-Quentin Noé (noe.louis-quentin@hotmail.fr)
 *  @brief	Definition of quaternions.
 *  @date   24-05-2022
 *
 *  @copyright Copyright (c) 2023 - Pulsar Software
 *
 *  @since 0.1.1
 */

#ifndef PULSAR_MATH_QUATERNION_HPP
#define PULSAR_MATH_QUATERNION_HPP 1

// Include: Pulsar
#include "pulsar/pulsar.hpp"

// Pulsar
namespace pul
{

	/// QUATERNION:
	template <typename _Ty>
	class quaternion
	{
	public:
	// TODO: Quaternion

	private:
		_Ty scalar_;
		vector<_Ty, 3> vector_;
	};
}

#endif // !PULSAR_MATH_COL_VECTOR2_INL