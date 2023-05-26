/*! @file   utf8_unit.cpp
 *  @author Louis-Quentin Noé (noe.louis-quentin@hotmail.com)
 *  @brief
 *  @date   11-04-2023
 *
 *  @copyright Copyright (c) 2023 - Pulsar Software
 *
 *  @since 0.1.6
 */

// Include: Pulsar
#include "pulsar/char.hpp"

// Include: Pulsar -> Tester
#include "pulsar_tester/pulsar_tester.hpp"

// Pulsar
namespace pul
{

	// encodings
	pt_pack(encoding_pack)
	{
		// -> Ascii
		pt_unit(ascii_validate_unit)
		{
			const char_t s1[] = "This is an ascii string. It doesn't contains any unicode characters!";
			auto v1						= ascii::validate(&s1[0], ascii::lenof(s1));	// USE AVX2
			pt_check(v1.code == char_error_code::success);

			const char_t s2[] = "This isn't an ascii string. It contains a 😄.";
			auto v2						= ascii::validate(&s2[0], ascii::lenof(s2));	// USE SSE2
			pt_check(v2.code == char_error_code::too_large);
			pt_check(v2.position == 42);

			const char_t s3[] = "This is ascii.";
			auto v3						= ascii::validate(&s3[0], ascii::lenof(s3));
			pt_check(v3.code == char_error_code::success);
			pt_check(v3.position == sizeof(s3));
		}
		pt_unit(ascii_lenof_unit)
		{
			const char_t s1[] = "A test string.";
			pt_check(sizeof(s1) == ascii::lenof(s1));
		}
		pt_unit(ascii_to_lower_and_upper_unit)
		{
			const char_t s1[] = "aAbBcCdDeEfFgGhHiIjJkKlLmMnNoOpPqQrRsStTuUvVwWxXyYzZwWù$^*¨%£µ/";
			const char_t s2[] = "aabbccddeeffgghhiijjkkllmmnnooppqqrrssttuuvvwwxxyyzzwwù$^*¨%£µ/";
			const char_t s3[] = "AABBCCDDEEFFGGHHIIJJKKLLMMNNOOPPQQRRSSTTUUVVWWXXYYZZWWù$^*¨%£µ/";
			char_t s4[sizeof(s1)];
			char_t s5[sizeof(s1)];
			ascii::to_lower(s1, ascii::lenof(s1), s4);
			pt_check(equal(begin(s4), end(s4), begin(s2)));
			ascii::to_upper(s1, ascii::lenof(s1), s5);
			pt_check(equal(begin(s5), end(s5), begin(s3)));
		}

		pt_unit(ascii_to_scalar_unit)
		{
			// 2
			const char_t s1[] = "110101100";
			uint32_t val1;
			ascii::to_scalar(s1, ascii::lenof(s1), val1, 2);

			// 10
			const char_t s2[] = "180848247";
			uint32_t val2;
			ascii::to_scalar(s2, ascii::lenof(s2), val2, 10);

			// 16
			const char_t s3[] = "9248FAD3";
			uint64_t val3;
			ascii::to_scalar(s3, ascii::lenof(s3), val3, 16);
		}
		pt_unit(ascii_to_chars_unit)
		{
			// 2
			char_t buf1[16] = { '\0' };
			int32_t val1		= -72;
			ascii::to_chars(&buf1[0], sizeof(buf1), val1, 2);

			// 10
			char_t buf2[16] = { '\0' };
			int32_t val2		= -243'842;
			ascii::to_chars(&buf2[0], sizeof(buf2), val2, 10);

			// 16
			char_t buf3[16] = { '\0' };
			int32_t val3		= -248'109'344;
			ascii::to_chars(&buf3[0], sizeof(buf3), val3, 16);
		}

		// -> Utf8
		pt_unit(utf8_validate_unit)
		{
			// Str1
			const u8char_t str1[] = "This is ascii encoded string! A verrrrrrrrrrrrrrrrrrrrryyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyy long end of string.";
			auto v1								= utf8::validate(&str1[0], utf8::lenof(str1));
			pt_check(v1.code == char_error_code::success);

			// Str2
			const u8char_t str2[] = "This is an unicode string that have a 😀. It is well encoded in utf8.  A verrrrrrrrrrrrrrrrrrrrryyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyy long end of string.";
			auto v2								= utf8::validate(&str2[0], utf8::lenof(str2));
			pt_check(v2.code == char_error_code::success);

			// Str3
			const u8char_t str3[] = "This is an unicode string that have a wrong encoded with \xC0\xC0 that is too short!  A verrrrrrrrrrrrrrrrrrrrryyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyy long end of string.";
			auto v3								= utf8::validate(&str3[0], utf8::lenof(str3));
			pt_check(v3.code == char_error_code::too_short);

			// Str4
			const u8char_t str4[] = "This is a 😀 in a utf-8 encoded string.  A verrrrrrrrrrrrrrrrrrrrryyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyy long end of string. 😀.";
			auto v4								= utf8::validate(&str4[0], utf8::lenof(str4));
			pt_check(v4.code == char_error_code::success);
		}

		pt_unit(utf8_to_lower_unit)
		{
			// Str1
			const u8char_t str1[]				= "AaBbCcDdEeFfGgHhIiJjKkLlMmNnOoPpQqRrSsTtUuVvWwXxYyZz";
			u8char_t buf1[sizeof(str1)] = { '\0' };
			utf8::to_lower(&str1[0], utf8::lenof(str1), &buf1[0]);
		}
		pt_unit(utf8_to_upper_unit)
		{
			// Str1
			const u8char_t str1[]				= "AaBbCcDdEeFfGgHhIiJjKkLlMmNnOoPpQqRrSsTtUuVvWwXxYyZz";
			u8char_t buf1[sizeof(str1)] = { '\0' };
			utf8::to_upper(&str1[0], utf8::lenof(str1), &buf1[0]);
		}

		pt_unit(utf8_to_scalar_unit)
		{
			// 2
			char_t buf1[16] = { '\0' };
			int32_t val1		= -72;
			ascii::to_chars(&buf1[0], sizeof(buf1), val1, 2);

			// 10
			char_t buf2[16] = { '\0' };
			int32_t val2		= -243'842;
			ascii::to_chars(&buf2[0], sizeof(buf2), val2, 10);

			// 16
			char_t buf3[16] = { '\0' };
			int32_t val3		= -248'109'344;
			ascii::to_chars(&buf3[0], sizeof(buf3), val3, 16);
		}
		pt_unit(utf8_to_chars_unit)
		{
			// 2
			char_t buf1[16] = { '\0' };
			int32_t val1		= -72;
			ascii::to_chars(&buf1[0], sizeof(buf1), val1, 2);

			// 10
			char_t buf2[16] = { '\0' };
			int32_t val2		= -243'842;
			ascii::to_chars(&buf2[0], sizeof(buf2), val2, 10);

			// 16
			char_t buf3[16] = { '\0' };
			int32_t val3		= -248'109'344;
			ascii::to_chars(&buf3[0], sizeof(buf3), val3, 16);
		}

		// -> Utf8 -> Utf16le
		pt_unit(utf8_to_utf16le_unit)
		{
			const u8char_t str1[] = "This is a 😀 in a utf-8 encoded string. 😀.";	// len = 48
			pt_check(utf8::lenof(str1) == 48);
			pt_check(utf8_to_utf16le::required_count(&str1[0], utf8::lenof(&str1[0])) == 44);
			u16char_t *str2 = new_construct<u16char_t[]>(utf8_to_utf16le::required_count(&str1[0], utf8::lenof(&str1[0])));
			auto v1					= utf8_to_utf16le::convert(&str1[0], utf8::lenof(&str1[0]), str2);
			pt_check(v1.code == char_error_code::success);
			destroy_delete<u16char_t[]>(str2);
		}

		// -> Utf16le -> Utf8
		pt_unit(utf16le_to_utf8_unit)
		{
			const u16char_t str1[] = u"This is a \xD83D\xDE00 in a utf-16 encoded string. \xD83D\xDE00.";	 // len = 45
			pt_check(utf16le::lenof(str1) == 45);
			pt_check(utf16le_to_utf8::required_count(&str1[0], utf16le::lenof(&str1[0])) == 49);
			u8char_t *str2 = new_construct<u8char_t[]>(utf16le_to_utf8::required_count(&str1[0], utf16le::lenof(&str1[0])));
			auto v1				 = utf16le_to_utf8::convert(&str1[0], utf16le::lenof(&str1[0]), str2);
			pt_check(v1.code == char_error_code::success);
			destroy_delete<u8char_t[]>(str2);
		}
	}




	// utf8
	pt_pack(utf8_types_pack)
	{
		// -> Viewing
		pt_unit(u8utility_unit)
		{
			u8string_view v1 = "This is a view!";

			u8char_t arr1[]	 = "This is an array!";
			u8string_view v2 = arr1;
			auto rb					 = v2.code_rbegin();
			auto rb2				 = rb + 18;
			auto re					 = v2.code_rend();
			for(; rb != re; ++rb)
				;	 // Prev tests
			pt_check(rb == re);
			pt_check(rb2 == re);

			array<u8char_t, 31> arr2 = "This is 😀 a C++ like array!";
			u8string_view v3				 = arr2;	// Next tests
			pt_check(v3[15] == 'C');
			pt_check(v3(12) == uint32_t('C'));
		}

		// -> String
		pt_unit(u8string_constructors_unit)
		{
			// u8string()
			u8string<> str1;
			// u8string(char, num)
			u8string<> str2('a', 16);
			// u8string(code, num)
			u8string<> str3(0x1'F6'00u, 16);
			// u8string((char) arr)
			u8char_t arr1[] = "This is a c++ string!";
			u8string<> str4(arr1);
			// u8string((code) arr)
			u8code_t arr2[] = { 0x1'F6'00u, 0x1'F6'03u, 0x1'F6'04u };
			u8string<> str5(arr2);
			// u8string(cstr)
			u8string<> str6 = "This is a c style string!";
		}
	}
}	 // namespace pul
