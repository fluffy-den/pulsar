/*! @file   array_unit.cpp
 *  @author Louis-Quentin Noé (noe.louis-quentin@hotmail.com)
 *  @brief
 *  @date   14-04-2023
 *
 *  @copyright Copyright (c) 2023 - Pulsar Software
 *
 *  @since 0.1.6
 */

// Include: Pulsar
#include "pulsar/iterable.hpp"

// Include: Pulsar -> Tester
#include "pulsar_tester/pulsar_tester.hpp"

// Pulsar
namespace pul
{
	pt_pack(array_pack)
	{
		pt_unit(array_unit)
		{
			// array()
			array<int32_t, 4> arr1;

			// array(val)
			array<int32_t, 4> arr2(2);
			pt_check(arr2[0] == 2);
			pt_check(arr2[1] == 2);
			pt_check(arr2[2] == 2);
			pt_check(arr2[3] == 2);

			// array((&__arr)[_Num]) + CTAD
			int32_t carr1[] = { 1, 2, 3, 4 };
			array arr3			= carr1;
			pt_check(equal(arr3.begin(), arr3.end(), begin(carr1)));

			// array((&&__arr)[_Num]) + CTAD
			int32_t carr2[] = { 4, 3, 2, 1 };
			array arr4			= std::move(carr2);
			pt_check(arr4[0] == 4);
			pt_check(arr4[1] == 3);
			pt_check(arr4[2] == 2);
			pt_check(arr4[3] == 1);

			// array((_Args&& ... __args)) + CTAD
			array arr5 = { 1, 2, 3, 4 };
			pt_check(arr5[0] == 1);
			pt_check(arr5[1] == 2);
			pt_check(arr5[2] == 3);
			pt_check(arr5[3] == 4);

			// array.operator=((&__arr)[_Num])
			arr2 = carr1;
			pt_check(equal(arr2.begin(), arr2.end(), begin(carr1)));

			// array.operator=((&&__arr)[_Num])
			arr5 = std::move(carr2);
			pt_check(arr5[0] == 4);
			pt_check(arr5[1] == 3);
			pt_check(arr5[2] == 2);
			pt_check(arr5[3] == 1);
			pt_check(arr5.count() == 4);
			pt_check(arr5.size() == 4 * sizeof(int32_t));

			// array.operator=(array const&)
			arr4 = arr5;
			pt_check(equal(arr4.begin(), arr4.end(), arr5.begin()));

			// array.operator=(array &&)
			arr1 = std::move(arr4);
			pt_check(equal(arr1.begin(), arr1.end(), arr5.begin()));
		}
	}
}	 // namespace pul
