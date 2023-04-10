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
	pt_pack(raii_pack)
	{
		// RAII
		pt_unit(unique_ptr_unit)
		{
			// make_unique
			unique_ptr p1 = make_unique<int32_t>(1);
			pt_check(p1.get());
			pt_check(*p1 == 1);

			// make_unique_aligned
			unique_ptr p2 = make_unique_aligned<int32_t>(align_val_t(16), 2);
			pt_check(p2.get());
			pt_check(is_aligned(p2.get(), align_val_t(16)));
			pt_check(*p2 == 2);

			// make_unique_aligned_at
			unique_ptr p3 = make_unique_aligned_at<size_t>(align_val_t(16), 8, 4);
			pt_check(p3.get());
			pt_check(is_aligned(p3.get(), align_val_t(16), 8));

			// array => make_unique (count)
			unique_ptr p4 = make_unique<int32_t[]>(4);
			pt_check(p4.count() == 4);

			// array => make_unique (beg, end)
			array<int32_t, 2> arr5 = { -2, -1 };
			unique_ptr p5					 = make_unique<int32_t[]>(arr5.begin(), arr5.end());
			pt_check(p5.count() == 2);
			pt_check(p5[0] == -2);
			pt_check(p5[1] == -1);

			// array => make_unique (initializer_list)
			unique_ptr p6 = make_unique<int32_t[]>({ 1, 2, 3, 4 });
			pt_check(p6[0] == 1);
			pt_check(p6[1] == 2);
			pt_check(p6[2] == 3);
			pt_check(p6[3] == 4);

			// array => make_unique_aligned (count)
			unique_ptr p7 = make_unique_aligned<int32_t[]>(5, align_val_t(16));
			pt_check(is_aligned(p7, align_val_t(16)));

			// array => make_unique_aligned (beg, end)
			array<int32_t, 2> arr8 = { 6, 7 };
			unique_ptr p8					 = make_unique_aligned<int32_t[]>(arr8.begin(), arr8.end(), align_val_t(16));
			pt_check(is_aligned(p8, align_val_t(16)));
			pt_check(p8.count() == 2);
			pt_check(p8[0] == 6);
			pt_check(p8[1] == 7);

			// array => make_unique_aligned (initializer_list)
			unique_ptr p9 = make_unique_aligned<int32_t[]>({ 1, 2, 3 }, align_val_t(16));
			pt_check(is_aligned(p9, align_val_t(16)));
			pt_check(p9.count() == 3);
		}
		pt_unit(shared_ptr_unit)
		{
			// make_shared
			shared_ptr p1 = make_shared<int32_t>(1);
			pt_check(p1.get());
			pt_check(*p1 == 1);

			// make_shared_aligned
			shared_ptr p2 = make_shared_aligned<int32_t>(align_val_t(16), 2);
			pt_check(p2.get());
			pt_check(is_aligned(p2.get(), align_val_t(16)));
			pt_check(*p2 == 2);

			// make_shared_aligned_at
			shared_ptr p3 = make_shared_aligned_at<size_t>(align_val_t(16), 8, 4);
			pt_check(p3.get());
			pt_check(is_aligned(p3.get(), align_val_t(16), 8));

			// array => make_shared (count)
			shared_ptr p4 = make_shared<int32_t[]>(4);
			pt_check(p4.count() == 4);

			// array => make_shared (beg, end)
			array<int32_t, 2> arr5 = { -2, -1 };
			shared_ptr p5					 = make_shared<int32_t[]>(arr5.begin(), arr5.end());
			pt_check(p5.count() == 2);
			pt_check(p5[0] == -2);
			pt_check(p5[1] == -1);

			// array => make_shared (initializer_list)
			shared_ptr p6 = make_shared<int32_t[]>({ 1, 2, 3, 4 });
			pt_check(p6[0] == 1);
			pt_check(p6[1] == 2);
			pt_check(p6[2] == 3);
			pt_check(p6[3] == 4);

			// array => make_shared_aligned (count)
			shared_ptr p7 = make_shared_aligned<int32_t[]>(5, align_val_t(16));
			pt_check(is_aligned(p7, align_val_t(16)));

			// array => make_shared_aligned (beg, end)
			array<int32_t, 2> arr8 = { 6, 7 };
			shared_ptr p8					 = make_shared_aligned<int32_t[]>(arr8.begin(), arr8.end(), align_val_t(16));
			pt_check(is_aligned(p8, align_val_t(16)));
			pt_check(p8.count() == 2);
			pt_check(p8[0] == 6);
			pt_check(p8[1] == 7);

			// array => make_shared_aligned (initializer_list)
			shared_ptr p9 = make_shared_aligned<int32_t[]>({ 1, 2, 3 }, align_val_t(16));
			pt_check(is_aligned(p9, align_val_t(16)));
			pt_check(p9.count() == 3);
		}
	}
}	 // namespace pul
