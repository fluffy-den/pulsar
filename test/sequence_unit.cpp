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
		pt_unit(sequence_iterator_unit)
		{
			// sequence_iterator
			int32_t arr[32];
			iterator it = begin(arr);
			pt_check((it + 1) == &arr[1]);
		}
		pt_unit(sequence_const_iterator_unit)
		{

		}
		pt_unit(sequence_reverse_iterator_unit)
		{

		}
		pt_unit(sequence_const_reverse_iterator_unit)
		{

		}
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
			v2b = sequence_view(arr2.data(), arr2s);
			sequence_view v2c = arr2;
			pt_check(v2c.begin() == v2b.begin());
			pt_check(v2c.count() == v2b.count());

			// sequence_view(__beg, __end)
			sequence_view v3 = sequence_view(begin(arr1), end(arr1));
			pt_check(v3.begin() == v2.begin());
			pt_check(v3.end() == v2.end());
			initializer_list arr3 = { 1, 2, 3, 4, 5, 6, 7, 8 };
			sequence_view v3b			= { 1, 2, 3, 4, 5, 6, 7, 8 };

			// sequence_view(__il)
			sequence_view v4 = arr3;
			pt_check(v4.begin() == arr3.begin());
			pt_check(v4.end() == arr3.end());

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
			pt_check(s2.reserved() >= s2.size());

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
			pt_check(s6.reserved() == 0);
			pt_check(s7.count() == s2.count());
			pt_check(equal(s7.begin(), s7.end(), s2.begin()));

			// sequence(sequence &&, magnifier&&)
			sequence s8(std::move(s7), magnifier_quadratic());
			pt_check(s7.count() == 0);
			pt_check(s7.size() == 0);
			pt_check(s7.capacity() == 0);
			pt_check(s7.reserved() == 0);
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
			sequence s9 = {{ 1, 2, 3, 4 }};
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
			sequence_view v3 = { 8, 9, 10 };
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
			// { 1, 4, 2, 6, 4, 5, 6, 7, 7, 7, 7, 7, 8, 9, 10 }
			// insert(w, val, count)
			s9.insert(s9.begin() + 1, 2, 2);
			pt_check(equal(s9.begin() + 1, s9.begin() + 3, 2));
			// { 1, 2, 2, 4, 2, 6, 4, 5, 6, 7, 7, 7, 7, 7, 8, 9, 10 }
			// insert(w, view) => insert(w, beg, end)
			s9.insert(s9.begin() + 0, v3);
			pt_check(equal(s9.begin(), s9.begin() + 3, v3.begin()));

			// remove_back
			s9.remove_back();
			pt_check(s9.back() == 9);
			// { 1, 2, 2, 4, 2, 6, 4, 5, 6, 7, 7, 7, 7, 7, 8, 9 }

			// remove(w)
			s9.remove(s9.begin() + 2);
			pt_check(s9[2] == 4);
			// { 1, 2, 4, 2, 6, 4, 5, 6, 7, 7, 7, 7, 7, 8, 9, 10 }

			// remove(beg, end)
			s9.remove(s9.begin() + 7, s9.begin() + 12);
			pt_check(s9[8] == 8);
			// { 1, 2, 4, 2, 6, 4, 5, 6, 8, 9, 10 }

			// replace(w, ...)
			// replace(w, val, count)
			// replace(w, beg, end)
			// replace(w, v)

			// replace(wbeg, wend, val, count)
			// replace(wbeg, wend, ibeg, iend)

			// shrink(val)
			// shrink_to_fit()
			// shrink_to_magnifier()

			// reserve(c)
			// reserve(c, align)
			// reserve_with_magnifier(c)
			// reserve_with_magnifier(c, align)

			// resize(val, count)

			// realign(align)

			// clear()

			// begin()
			// end()
			// rbegin()
			// rend()

			// data()

			// size()

			// count()

			// reserved()

			// capacity()

			// alignment()

			// view()

			// magnifier()

			// allocator()

			// is_allocated()

			// is_empty()

			// sequence -> sequence_view by CTAD
			v2 = s1;
		}
	}
}