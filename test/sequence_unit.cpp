/*! @file   sequence_unit.cpp
 *  @author Louis-Quentin Noé (noe.louis-quentin@hotmail.com)
 *  @brief
 *  @date   06-04-2023
 *
 *  @copyright Copyright (c) 2023 - Pulsar Software
 *
 *  @since 0.1.6
 */

// Include: Pulsar
#include "pulsar/algorithm.hpp"

// Include: Pulsar -> Tester
#include "pulsar_tester/pulsar_tester.hpp"

// Pulsar
namespace pul
{
	pt_pack(sequence_pack)
	{
		pt_unit(sequence_view_unit)
		{
			// sequence_view()
			sequence_view<int32_t> v1;

			// sequence_view().is_empty()
			pt_check(v1.is_empty());

			// sequence_view(__data, __count)
			pf_decl_constexpr size_t arr1s = 16;
			pf_decl_constexpr size_t arr2s = 22;
			int32_t arr1[arr1s];
			array<int32_t, arr2s> arr2;
			sequence_view v2 = sequence_view(data(arr1), arr1s);
			pt_check(v2.begin() == begin(arr1));
			pt_check(v2.rbegin() == rbegin(arr1));
			pt_check(v2.end() == end(arr1));
			pt_check(v2.rend() == rend(arr1));
			pt_check(v2.data() == data(arr1));
			pt_check(v2.count() == arr1s);
			pt_check(v2.size() == sizeof(arr1));

			// sequence_view(__arr1)
			sequence_view v2b = arr1;
			pt_check(v2b.begin() == v2.begin());
			pt_check(v2b.count() == v2.count());
			v2b								= sequence_view(arr2.data(), arr2s);
			sequence_view v2c = arr2;
			pt_check(v2c.begin() == v2b.begin());
			pt_check(v2c.count() == v2b.count());

			// sequence_view(__beg, __end)
			sequence_view v3 = sequence_view(begin(arr1), end(arr1));
			pt_check(v3.begin() == v2.begin());
			pt_check(v3.end() == v2.end());
			int32_t arr3[] = { 1, 2, 3, 4, 5, 6, 7, 8 };

			// sequence_view(__il)
			sequence_view v4 = arr3;
			pt_check(v4.begin() == begin(arr3));
			pt_check(v4.end() == end(arr3));

			// sequence_view(__r)
			sequence_view v5 = v3;
			pt_check(v5.begin() == v2.begin());
			pt_check(v5.end() == v2.end());

			// sequence_view.operator=(sequence_view const &__r)
			v4 = v5;
			pt_check(v4.begin() == v3.begin());
			pt_check(v4.end() == v3.end());
		}
		pt_unit(sequence_unit)
		{
			// sequence()
			sequence<int32_t> s1;
			pt_check(s1.alignment() == ALIGN_DEFAULT);

			// sequence(val, count)
			const int32_t s2v			= 16;
			const size_t s2c			= 32;
			const align_val_t s2a = align_val_t(32);
			sequence s2						= sequence(s2v, s2c, s2a);
			pt_check(s2.count() == s2c);
			pt_check(s2.size() == s2c * sizeof(int32_t));
			pt_check(s2.alignment() == s2a);
			pt_check(s2.capacity() >= s2.count());
			pt_check(s2.storage_size() >= s2.size());

			// sequence(begin, end, align)
			const size_t arr1c	= 8;
			int32_t arr1[arr1c] = { 1, 2, 3, 4, 5, 6, 7, 8 };
			sequence s3(begin(arr1), end(arr1));
			pt_check(s3.count() == arr1c);
			pt_check(s3.size() == sizeof(arr1));
			pt_check(s3.alignment() == ALIGN_DEFAULT);
			pt_check(equal(s3.begin(), s3.end(), begin(arr1)));

			// sequence(array)
			sequence s4 = arr1;
			pt_check(s4.count() == arr1c);
			pt_check(s4.size() == sizeof(arr1));
			pt_check(s4.alignment() == ALIGN_DEFAULT);
			pt_check(equal(s4.begin(), s4.end(), begin(arr1)));

			// sequence(view)
			sequence_view v1 = arr1;
			sequence s5			 = v1;
			pt_check(s5.count() == v1.count());
			pt_check(s5.size() == v1.size());
			pt_check(s5.alignment() == ALIGN_DEFAULT);
			pt_check(equal(s5.begin(), s5.end(), v1.begin()));

			// sequence(sequence const&)
			sequence s6 = s2;
			pt_check(s6.count() == s2.count());
			pt_check(s6.size() == s2.size());
			pt_check(equal(s6.begin(), s6.end(), s2.begin()));

			// sequence(sequence &&)
			sequence s7 = std::move(s6);
			pt_check(s6.count() == 0);
			pt_check(s6.size() == 0);
			pt_check(s6.capacity() == 0);
			pt_check(s6.storage_size() == 0);
			pt_check(s7.count() == s2.count());
			pt_check(equal(s7.begin(), s7.end(), s2.begin()));

			// sequence(sequence &&, magnifier&&)
			sequence s8(std::move(s7), magnifier_quadratic());
			pt_check(s7.count() == 0);
			pt_check(s7.size() == 0);
			pt_check(s7.capacity() == 0);
			pt_check(s7.storage_size() == 0);
			pt_check(s8.count() == s2.count());
			pt_check(equal(s8.begin(), s8.end(), s2.begin()));

			// sequence().operator=(view)
			s6 = v1;
			pt_check(s6.count() == v1.count());
			pt_check(s6.size() == v1.size());
			pt_check(equal(s6.begin(), s6.end(), v1.begin()));

			// sequence().operator=(sequence const&)
			s7 = s6;
			pt_check(s7.count() == v1.count());
			pt_check(s7.size() == v1.size());
			pt_check(equal(s7.begin(), s7.end(), v1.begin()));

			// sequence().operator=(sequence &&)
			s1 = std::move(s7);
			pt_check(s1.count() == v1.count());
			pt_check(s1.size() == v1.size());
			pt_check(s7.count() == 0);
			pt_check(s7.size() == 0);
			pt_check(equal(s1.begin(), s1.end(), v1.begin()));

			// operator().view()
			sequence_view v2 = s1.view();
			pt_check(v2.count() == s1.count());
			pt_check(v2.size() == s1.size());
			pt_check(equal(v2.begin(), v2.end(), s1.begin()));

			// assign(val, count)
			s1.assign(8, 8);
			v1 = s1;
			pt_check(v1.count() == s1.count());
			pt_check(v1.size() == s1.size());
			pt_check(equal(v1.begin(), v1.end(), s1.begin()));

			// assign(beg, end)
			s2.assign(v2.begin(), v2.end());
			pt_check(equal(s2.begin(), s2.end(), v2.begin()));

			// insert_back(...)
			sequence s9 = {
				{1, 2, 3, 4}
			};
			s9.insert_back(5);
			// { 1, 2, 3, 4, 5 }
			pt_check(s9.back() == 5);
			pt_check(s9.count() == 5);
			s9.insert_back(6);
			// { 1, 2, 3, 4, 5, 6 }
			pt_check(s9.back() == 6);
			pt_check(s9.count() == 6);


			// insert_back(val, count)
			s9.insert_back(7, 5);
			pt_check(s9.count() == 11);
			pt_check(equal(s9.begin() + 6, s9.end(), 7));
			// { 1, 2, 3, 4, 5, 6, 7, 7, 7, 7, 7 }

			// insert_back(view) => (beg, end)
			int32_t arr3[]	 = { 8, 9, 10 };
			sequence_view v3 = arr3;
			s9.insert_back(v3);
			// { 1, 2, 3, 4, 5, 6, 7, 7, 7, 7, 7, 8, 9, 10 }
			pt_check(equal(s9.begin() + 11, s9.end(), v3.begin()));
			pt_check(s9.count() == 14);
			pt_check(s9.count() == countof(s9.begin(), s9.end()));

			// insert(w, ...)
			pt_check((s9.begin() + 1) == &s9[1]);
			pt_check(s9.end() == (&s9[0] + s9.count()));
			s9.insert(s9.begin() + 1, 4);
			pt_check(s9[1] == 4);
			// { 1, 4, 2, 3, 4, 5, 6, 7, 7, 7, 7, 7, 8, 9, 10 }

			s9.insert(s9.begin() + 3, 6);
			pt_check(s9[3] == 6);
			// { 1, 4, 2, 6, 3, 4, 5, 6, 7, 7, 7, 7, 7, 8, 9, 10 }

			// insert(w, val, count)
			s9.insert(s9.begin() + 1, 2, 2);
			pt_check(equal(s9.begin() + 1, s9.begin() + 3, 2));
			// { 1, 2, 2, 4, 2, 6, 3, 4, 5, 6, 7, 7, 7, 7, 7, 8, 9, 10 }

			// insert(w, view) => insert(w, beg, end)
			s9.insert(s9.begin() + 0, v3);
			pt_check(equal(s9.begin(), s9.begin() + 3, v3.begin()));
			// { 8, 9, 10, 1, 2, 2, 4, 2, 6, 3, 4, 5, 6, 7, 7, 7, 7, 7, 8, 9, 10 }

			// remove_back
			s9.remove_back();
			pt_check(s9.back() == 9);
			// { 8, 9, 10, 1, 2, 2, 4, 2, 6, 3, 4, 5, 6, 7, 7, 7, 7, 7, 8, 9 }

			// remove(w)
			s9.remove(s9.begin() + 2);
			pt_check(s9[2] == 1);
			// { 8, 9, 1, 2, 2, 4, 2, 6, 3, 4, 5, 6, 7, 7, 7, 7, 7, 8, 9 }

			// remove(beg, end)
			s9.remove(s9.begin() + 12, s9.begin() + 17);
			pt_check(s9[12] == 8);
			// { 8, 9, 1, 2, 2, 4, 2, 6, 3, 4, 5, 6, 8, 9 }

			// replace(w, ...)
			s9.replace(s9.begin() + 4, 3);
			pt_check(s9[4] == 3);
			s9.replace(s9.begin() + 6, 5);
			pt_check(s9[6] == 5);
			// { 8, 9, 1, 2, 3, 4, 5, 6, 3, 4, 5, 6, 8, 9 }

			// replace(w, val, count)
			s9.replace(s9.begin() + 0, 0, 2);
			pt_check(equal(s9.begin() + 0, s9.begin() + 2, 0));
			// { 0, 0, 9, 1, 2, 3, 4, 5, 6, 3, 4, 5, 6, 8, 9 }

			// replace(w, v) -> replace(w, beg, end)
			initializer_list v4 = { -2, -1 };
			s9.replace(s9.begin() + 0, v4);
			pt_check(equal(s9.begin() + 0, s9.begin() + 2, iterator(v4.begin())));
			// { -2, -1, 0, 9, 1, 2, 3, 4, 5, 6, 3, 4, 5, 6, 8, 9 }

			// replace(wbeg, wend, val, count)
			s9.replace(s9.begin() + 3, s9.begin() + 16, 9, 7);
			pt_check(equal(s9.begin() + 3, s9.begin() + 8, 9));
			// { -2, -1, 0, 9, 9, 9, 9, 9, 9, 9 }
			// replace(wbeg, wend, ibeg, iend)
			initializer_list v5 = { -5, -4, -3, -2, -1 };
			s9.replace(s9.begin() + 0, s9.begin() + 2, v5);
			pt_check(equal(s9.begin() + 0, s9.begin() + 5, iterator(v5.begin())));
			// { -5, -4, -3, -2, -1, 0, 9, 9, 9, 9, 9, 9 }
			initializer_list v6 = { 1, 2, 3, 4, 5 };
			s9.replace(s9.begin() + 6, s9.begin() + 13, v6);
			pt_check(equal(s9.begin() + 6, s9.begin() + 11, iterator(v6.begin())));
			// { -5, -4, -3, -2, -1, 0, 1, 2, 3, 4, 5 }

			// shrink(val) + reserve(val)
			pt_check(s9.reserve(30) == 9);	// NOTE: Last capacity={21}
			pt_check(s9.shrink(15) == 15);

			// shrink_to_fit()
			pt_check(s9.shrink_to_fit() == 4);

			// shrink_to_magnifier() + reserve_with_magnifier(val)
			sequence<int32_t, magnifier_linear> s10({ 1, 2, 3, 4, 5 }, align_val_t(8), magnifier_linear(16));
			pt_check((s10.magnifier()(63) - s10.capacity()) == s10.reserve_with_magnifier(63));
			pt_check((s10.capacity() - s10.magnifier()(s10.count())) == s10.shrink_to_magnifier());

			// resize(val, count)
			s10.resize(0, 32);

			// realign(align)
			s10.realign(align_val_t(32));
			pt_check(s10.alignment() == align_val_t(32));
			pt_check(s10.count() == 32);

			// clear()
			s9.clear();
			pt_check(!s9.is_allocated());
			pt_check(s9.is_empty());
			pt_check(s9.capacity() == 0);
			pt_check(s9.storage_size() == 0);
			pt_check(s9.count() == 0);
			pt_check(s9.size() == 0);

			// rbegin()
			// rend()
			pt_check(s10.begin() == (s10.rend() - 1));
			pt_check(s10.end() == (s10.rbegin() - 1));

			// is_allocated()
			pt_check(s10.is_allocated());
			pt_check(!s10.is_empty());

			// is_empty()
			sequence<int32_t> s11;
			pt_check(!s11.is_allocated());
			pt_check(s11.is_empty());

			// sequence -> sequence_view by CTAD
			v2 = s1;
		}

		pt_benchmark(sequence_insert_back_m4096, __bvn, 65'536)
		{
			sequence<int32_t, magnifier_linear> s(align_val_t(8), magnifier_linear(4'096));
			__bvn.measure([&](size_t __index)
										{ return s.push_back(__index); });
		}
	}
}	 // namespace pul
