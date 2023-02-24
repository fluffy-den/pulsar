/*! @file   ascii.hpp
 *  @author Louis-Quentin Noé (noe.louis-quentin@hotmail.fr)
 *  @brief
 *  @date   02-01-2023
 *
 *  @copyright Copyright (c) 2023 - Pulsar Software
 *
 *  @since 0.1.3
 */

#ifndef PULSAR_CHAR_TRAITS_ASCII_HPP
#define PULSAR_CHAR_TRAITS_ASCII_HPP 1

// Include: Pulsar
#include "pulsar/encoding/char_traits/char_traits.hpp"

// Pulsar
namespace pul
{
	// Char Traits
namespace char_traits
{
	/// TRAITS: ASCII
	class ascii
	{
	public:
		using value_t			 = char_t;
		using code_value_t = int8_t;

	private:
		/// Is Valid -> SIMD
		pf_hint_nodiscard pf_decl_static pf_decl_constexpr bool
		__is_valid_no_simd(
			const value_t * __str,
			size_t __len) pf_attr_noexcept
		{
			// compute
			for(size_t i = 0; i < __len; ++i)
			{
				if(__str[i] < 0) return false;
			}
			return true;
		}
		pf_hint_nodiscard pf_decl_static bool
		__is_valid_sse(
			const value_t * __str,
			size_t __len) pf_attr_noexcept
		{
			size_t i			= 0;
			m128i64_t err = _mm_setzero_si128();
			// compute
			while(i + 16 <= __len)
			{
				m128i64_t c = _mm_lddqu_si128(union_cast<const m128i64_t*>(&__str[i]));
				err = _mm_or_si128(err, _mm_cmpgt_epi8(c, _mm_set1_epi8(127)));
				i	 += 16;
			}
			if(i + 8 <= __len)
			{
				m128i64_t c = _mm_loadu_si64(union_cast<const m128i64_t*>(&__str[i]));
				err = _mm_or_si128(err, _mm_cmpgt_epi8(c, _mm_set1_epi8(127)));
				i	 += 8;
			}
			int32_t m = _mm_movemask_epi8(err);
			return m || __is_valid_no_simd(&__str[i], __len - i);
		}
		pf_hint_nodiscard pf_decl_static bool
		__is_valid_avx(
			const value_t * __str,
			size_t __len) pf_attr_noexcept
		{
			size_t i			= 0;
			m256i64_t err = _mm256_setzero_si256();
			// compute
			while(i + 32 <= __len)
			{
				m256i64_t c = _mm256_lddqu_si256(union_cast<const m256i64_t*>(&__str[i]));
				err = _mm256_or_si256(err, _mm256_cmpgt_epi8(c, _mm256_set1_epi8(127)));
				i	 += 32;
			}
			int32_t m = _mm256_movemask_epi8(err);
			return m || __is_valid_sse(&__str[i], __len - i);
		}

		// https://github.com/evelance/fast_ascii_tolower_toupper/blob/master/asciitolower_upper.c
		/// To upper
		pf_decl_static pf_decl_constexpr void
		__to_upper_no_simd(
			value_t * __str,
			size_t __len) pf_attr_noexcept
		{
			for(size_t i = 0; i < __len; ++i) { __str[i] = to_upper(__str[i]); }
		}
		pf_decl_static void
		__to_upper_sse(
			value_t * __str,
			size_t __len) pf_attr_noexcept
		{
			size_t i = 0;
			while(i + 16 <= __len)
			{
				m128i64_t *ds			 = union_cast<m128i64_t*>(&__str[i]);
				const m128i64_t in = _mm_lddqu_si128(ds);
				const m128i64_t rs = _mm_sub_epi8(in, _mm_set1_epi8(97 - 128));
				const m128i64_t nm = _mm_cmpgt_epi8(rs, _mm_set1_epi8(25 - 128));
				const m128i64_t fl = _mm_andnot_si128(nm, _mm_set1_epi8(32));
				_mm_storeu_si128(ds, _mm_xor_si128(in, fl));
				i += 16;
			}
			__to_upper_no_simd(&__str[i], __len - i);
		}
		pf_decl_static void
		__to_upper_avx(
			value_t * __str,
			size_t __len) pf_attr_noexcept
		{
			size_t i = 0;
			while(i + 32 <= __len)
			{
				m256i64_t *ds			 = union_cast<m256i64_t*>(&__str[i]);
				const m256i64_t in = _mm256_lddqu_si256(ds);
				const m256i64_t rs = _mm256_sub_epi8(in, _mm256_set1_epi8(97 - 128));
				const m256i64_t nm = _mm256_cmpgt_epi8(rs, _mm256_set1_epi8(25 - 128));
				const m256i64_t fl = _mm256_andnot_si256(nm, _mm256_set1_epi8(32));
				_mm256_storeu_si256(ds, _mm256_xor_si256(in, fl));
				i += 32;
			}
			__to_upper_sse(&__str[i], __len - i);
		}

		/// To lower
		pf_decl_static pf_decl_constexpr void
		__to_lower_no_simd(
			value_t * __str,
			size_t __len) pf_attr_noexcept
		{
			for(size_t i = 0; i < __len; ++i) { __str[i] = to_lower(__str[i]); }
		}
		pf_decl_static void
		__to_lower_sse(
			value_t * __str,
			size_t __len) pf_attr_noexcept
		{
			size_t i = 0;
			while(i + 16 <= __len)
			{
				m128i64_t *ds			 = union_cast<m128i64_t*>(&__str[i]);
				const m128i64_t in = _mm_lddqu_si128(ds);
				const m128i64_t rs = _mm_sub_epi8(in, _mm_set1_epi8(65 - 128));
				const m128i64_t nm = _mm_cmpgt_epi8(rs, _mm_set1_epi8(25 - 128));
				const m128i64_t fl = _mm_andnot_si128(nm, _mm_set1_epi8(32));
				_mm_storeu_si128(ds, _mm_xor_si128(in, fl));
				i += 16;
			}
			__to_lower_no_simd(&__str[i], __len - i);
		}
		pf_decl_static void
		__to_lower_avx(
			value_t * __str,
			size_t __len) pf_attr_noexcept
		{
			size_t i = 0;
			while(i + 32 <= __len)
			{
				m256i64_t *ds			 = union_cast<m256i64_t*>(&__str[i]);
				const m256i64_t in = _mm256_lddqu_si256(ds);
				const m256i64_t rs = _mm256_sub_epi8(in, _mm256_set1_epi8(65 - 128));
				const m256i64_t nm = _mm256_cmpgt_epi8(rs, _mm256_set1_epi8(25 - 128));
				const m256i64_t fl = _mm256_andnot_si256(nm, _mm256_set1_epi8(32));
				_mm256_storeu_si256(ds, _mm256_xor_si256(in, fl));
				i += 32;
			}
			__to_lower_sse(&__str[i], __len - i);
		}

		/// To int64_t -> SIMD
		// pf_hint_nodiscard pf_decl_static pf_decl_constexpr int64_t
		// __to_int64_no_simd(
		// const value_t * __str,
		// size_t __len) pf_attr_noexcept
		// {
		// return 0;
		// }

		/// To uint64_t -> SIMD
		// pf_hint_nodiscard pf_decl_static uint64_t
		// __to_uint64_no_simd(
		// const value_t * __str,
		// size_t __len) pf_attr_noexcept
		// {
		// uint64_t v = 0;
		// if((__len > 19))
		// __len = 19;
		// for(size_t i = 0; i < __len; ++i)
		// {
		// v += (__str[i] - '0') * (1 << (2 * i));
		// }
		// return v;
		// }

		// https://github.com/lemire/fast_double_parser
		/// To float32_t -> SIMD
		// pf_hint_nodiscard pf_decl_static float32_t
		// __to_float32_no_simd(
		// const value_t * __str,
		// size_t __len) pf_attr_noexcept
		// {}

		///// To float64_t -> SIMD
		// pf_hint_nodiscard pf_decl_static float64_t
		// __to_float64_no_simd(
		// const value_t * __str,
		// size_t __len) pf_attr_noexcept
		// {}

		/// Compare -> SIMD
		pf_hint_nodiscard pf_decl_static pf_decl_constexpr diff_t
		__compare_no_simd(
			const value_t * __p1,
			const value_t * __p2,
			size_t __maxlen) pf_attr_noexcept
		{
			for(size_t i = 0; i < __maxlen; ++i)
			{
				if(__p1[i] > __p2[i]) { return 1; }
				else if(__p1[i] < __p2[i]) { return -1; }
			}
			return 0;
		}
		pf_hint_nodiscard pf_decl_static diff_t
		__compare_sse(
			const value_t * __p1,
			const value_t * __p2,
			size_t __maxlen) pf_attr_noexcept
		{
			size_t i = 0;
			while(i + 16 <= __maxlen)
			{
				const m128i64_t m1 = _mm_lddqu_si128(union_cast<const m128i64_t*>(__p1[i]));
				const m128i64_t m2 = _mm_lddqu_si128(union_cast<const m128i64_t*>(__p2[i]));
				if(_mm_movemask_epi8(_mm_cmpgt_epi8(m1, m2)) & 0xFFFF) return 1;
				if(_mm_movemask_epi8(_mm_cmplt_epi8(m1, m2)) & 0xFFFF) return -1;
				i += 16;
			}
			if(i + 8 <= __maxlen)
			{
				const m128i64_t m1 = _mm_loadu_si64(union_cast<const m128i64_t*>(__p1[i]));
				const m128i64_t m2 = _mm_loadu_si64(union_cast<const m128i64_t*>(__p2[i]));
				if(_mm_movemask_epi8(_mm_cmpgt_epi8(m1, m2)) & 0xFF) return 1;
				if(_mm_movemask_epi8(_mm_cmplt_epi8(m1, m2)) & 0xFF) return -1;
				i += 8;
			}
			if(i + 4 <= __maxlen)
			{
				const m128i64_t m1 = _mm_loadu_si32(union_cast<const m128i64_t*>(__p1[i]));
				const m128i64_t m2 = _mm_loadu_si32(union_cast<const m128i64_t*>(__p2[i]));
				if(_mm_movemask_epi8(_mm_cmpgt_epi8(m1, m2)) & 0xF) return 1;
				if(_mm_movemask_epi8(_mm_cmplt_epi8(m1, m2)) & 0xF) return -1;
				i += 8;
			}
			return __compare_no_simd(__p1 + i, __p2 + i, __maxlen - i);
		}
		pf_hint_nodiscard pf_decl_static diff_t
		__compare_avx(
			const value_t * __p1,
			const value_t * __p2,
			size_t __maxlen) pf_attr_noexcept
		{
			size_t i = 0;
			while(i + 32 <= __maxlen)
			{
				const m256i64_t m1 = _mm256_lddqu_si256(union_cast<const m256i64_t*>(__p1[i]));
				const m256i64_t m2 = _mm256_lddqu_si256(union_cast<const m256i64_t*>(__p2[i]));
				int32_t gt				 = _mm256_movemask_epi8(_mm256_cmpgt_epi8(m1, m2));
				if(gt & 0xFFFFFFFF) return 1;
				if((~gt - _mm256_movemask_epi8(_mm256_cmpeq_epi8(m1, m2))) & 0xFFFFFFFF) return -1;
				i += 32;
			}
			return __compare_sse(__p1 + i, __p2 + i, __maxlen - i);
		}

	public:
		/// Constructors
		ascii()							 = delete;
		ascii(ascii const &) = delete;
		ascii(ascii &&)			 = delete;

		/// Destructors
		~ascii() pf_attr_noexcept = delete;

		/// Len of
		pf_hint_nodiscard pf_decl_static pf_decl_constexpr size_t
		lenof(
			const value_t * __str) pf_attr_noexcept
		{
			if (!__str) return 0;
			const value_t *b = __str;
			for(; *__str != '\0'; ++__str);
			return distof(b, __str);
		}
		pf_hint_nodiscard pf_decl_static pf_decl_constexpr size_t
		countof(
			const value_t * __str) pf_attr_noexcept
		{
			return lenof(__str);
		}
		pf_hint_nodiscard pf_decl_static pf_decl_constexpr size_t
		codelenof(
			const value_t * __p) pf_attr_noexcept
		{
			ignore = __p;
			return sizeof(value_t);
		}
		pf_hint_nodiscard pf_decl_static pf_decl_constexpr size_t
		codelenof(
			code_value_t __v) pf_attr_noexcept
		{
			ignore = __v;
			return codelenof(union_cast<const value_t*>(&__v));
		}

		/// Null
		pf_hint_nodiscard pf_decl_static pf_decl_constexpr code_value_t
		null_code() pf_attr_noexcept
		{
			return '\0';
		}

		/// Next
		pf_hint_nodiscard pf_decl_static pf_decl_inline pf_decl_constexpr value_t*
		nextof(value_t * __p) pf_attr_noexcept
		{
			return ++__p;
		}
		pf_hint_nodiscard pf_decl_static pf_decl_inline pf_decl_constexpr const value_t*
		nextof(const value_t * __p) pf_attr_noexcept
		{
			return ++__p;
		}

		/// Prev
		pf_hint_nodiscard pf_decl_static pf_decl_inline pf_decl_constexpr value_t*
		prevof(value_t * __p) pf_attr_noexcept
		{
			return --__p;
		}
		pf_hint_nodiscard pf_decl_static pf_decl_inline pf_decl_constexpr const value_t*
		prevof(const value_t * __p) pf_attr_noexcept
		{
			return --__p;
		}

		/// Is *
		pf_hint_nodiscard pf_decl_static pf_decl_constexpr bool
		is_valid(const value_t * __str, size_t __len) pf_attr_noexcept
		{
			if(pf_unlikely(!__str || !__len)) return true;
			if(std::is_constant_evaluated()) return __is_valid_no_simd(__str, __len);
			return __is_valid_avx(__str, __len);
		}
		pf_hint_nodiscard pf_decl_static pf_decl_constexpr bool
		is_alnum(code_value_t __val) pf_attr_noexcept
		{
			return is_alpha(__val) || is_digit(__val);
		}
		pf_hint_nodiscard pf_decl_static pf_decl_constexpr bool
		is_alpha(code_value_t __val) pf_attr_noexcept
		{
			return is_upper(__val) || is_lower(__val);
		}
		pf_hint_nodiscard pf_decl_static pf_decl_constexpr bool
		is_lower(code_value_t __val) pf_attr_noexcept
		{
			return (__val >= 97) && (__val <= 122);
		}
		pf_hint_nodiscard pf_decl_static pf_decl_constexpr bool
		is_upper(code_value_t __val) pf_attr_noexcept
		{
			return (__val >= 65) && (__val <= 90);
		}
		pf_hint_nodiscard pf_decl_static pf_decl_constexpr bool
		is_digit(code_value_t __val) pf_attr_noexcept
		{
			return (__val >= 48) && (__val <= 57);
		}
		pf_hint_nodiscard pf_decl_static pf_decl_constexpr bool
		is_xdigit(code_value_t __val) pf_attr_noexcept
		{
			return is_digit(__val) || ((__val >= 65) && (__val <= 70))
			|| ((__val >= 97) && (__val <= 102));
		}
		pf_hint_nodiscard pf_decl_static pf_decl_constexpr bool
		is_cntrl(code_value_t __val) pf_attr_noexcept
		{
			return ((__val >= 0) && (__val <= 31)) && (__val == 127);
		}
		pf_hint_nodiscard pf_decl_static pf_decl_constexpr bool
		is_graph(code_value_t __val) pf_attr_noexcept
		{
			return (__val >= 33) && (__val <= 126);
		}
		pf_hint_nodiscard pf_decl_static pf_decl_constexpr bool
		is_space(code_value_t __val) pf_attr_noexcept
		{
			return ((__val >= 9) && (__val <= 13)) || (__val == 32);
		}
		pf_hint_nodiscard pf_decl_static pf_decl_constexpr bool
		is_blank(code_value_t __val) pf_attr_noexcept
		{
			return (__val == 9) || (__val == 32);
		}
		pf_hint_nodiscard pf_decl_static pf_decl_constexpr bool
		is_print(code_value_t __val) pf_attr_noexcept
		{
			return (__val >= 32) && (__val <= 126);
		}
		pf_hint_nodiscard pf_decl_static pf_decl_constexpr bool
		is_punct(code_value_t __val) pf_attr_noexcept
		{
			return ((__val >= 33) && (__val <= 47)) || ((__val >= 58) && (__val <= 64))
			|| ((__val >= 91) && (__val <= 96)) || ((__val >= 123) && (__val <= 126));
		}
		pf_hint_nodiscard pf_decl_static pf_decl_constexpr bool
		is_ascii(code_value_t __val) pf_attr_noexcept
		{
			return __val >= 0;
		}
		pf_hint_nodiscard pf_decl_static pf_decl_constexpr bool
		is_null_code(code_value_t __val) pf_attr_noexcept
		{
			return __val == '\0';
		}

		/// To*
		pf_hint_nodiscard pf_decl_static pf_decl_inline pf_decl_constexpr code_value_t
		to_code_point(const value_t * __p) pf_attr_noexcept
		{
			return union_cast<code_value_t>(*__p);
		}
		pf_hint_nodiscard pf_decl_static pf_decl_constexpr code_value_t
		to_upper(code_value_t __v) pf_attr_noexcept
		{
			return is_upper(__v) ? __v - 32 : __v;
		}
		pf_decl_static pf_decl_constexpr void
		to_upper(value_t * __str, size_t __len) pf_attr_noexcept
		{
			if(pf_unlikely(!__str || !__len)) return;
			if(std::is_constant_evaluated()) { __to_upper_no_simd(__str, __len); }
			else { __to_upper_avx(__str, __len); }
		}
		pf_hint_nodiscard pf_decl_static pf_decl_constexpr code_value_t
		to_lower(code_value_t __v) pf_attr_noexcept
		{
			return is_lower(__v) ? __v + 32 : __v;
		}
		pf_decl_static pf_decl_constexpr void
		to_lower(value_t * __str, size_t __len) pf_attr_noexcept
		{
			if(pf_unlikely(!__str || !__len)) return;
			if(std::is_constant_evaluated()) { __to_lower_no_simd(__str, __len); }
			else { __to_lower_avx(__str, __len); }
		}
		// https://kholdstare.github.io/technical/2020/05/26/faster-integer-parsing.html
		pf_hint_nodiscard pf_decl_static pf_decl_constexpr int8_t
		to_int8(const value_t * __str, size_t __len) pf_attr_noexcept
		{
			// TODO: SIMD
			if(pf_unlikely(!__str || !__len || (*__str != '-') || !is_digit(*__str))) return 0;
			int8_t s;
			// Is signed?
			if(*__str == '-')
			{
				s = -1;
				++__str;
				--__len;
			}
			else { s = 1; }
			int8_t v = 0;
			if(__len > 3) __len = 3;
			for(size_t i = 0; i < __len && is_digit(__str[i]); ++i)
			{
				v += (__str[i] - '0') * (1 << (2 * i));
			}
			return v * s;
		}
		pf_hint_nodiscard pf_decl_static pf_decl_constexpr int16_t
		to_int16(const value_t * __str, size_t __len) pf_attr_noexcept
		{
			// TODO: SIMD
			if(pf_unlikely(!__str || !__len || (*__str != '-') || !is_digit(*__str))) return 0;
			int16_t s;
			if(*__str == '-')
			{
				s = -1;
				++__str;
				--__len;
			}
			else { s = 1; }
			int16_t v = 0;
			if(__len > 5) __len = 5;
			for(size_t i = 0; i < __len && is_digit(__str[i]); ++i)
			{
				v += (__str[i] - '0') * (1 << (2 * i));
			}
			return v * s;
		}
		pf_hint_nodiscard pf_decl_static pf_decl_constexpr int32_t
		to_int32(const value_t * __str, size_t __len) pf_attr_noexcept
		{
			// TODO: SIMD
			if(pf_unlikely(!__str || !__len || (*__str != '-') || !is_digit(*__str))) return 0;
			int32_t s;
			if(*__str == '-')
			{
				s = -1;
				++__str;
				--__len;
			}
			else { s = 1; }
			int32_t v = 0;
			if(__len > 10) __len = 10;
			for(size_t i = 0; i < __len && is_digit(__str[i]); ++i)
			{
				v += (__str[i] - '0') * (1 << (2 * i));
			}
			return v * s;
		}
		pf_hint_nodiscard pf_decl_static pf_decl_constexpr int64_t
		to_int64(const value_t * __str, size_t __len) pf_attr_noexcept
		{
			// TODO: SIMD
			if(pf_unlikely(!__str || !__len || (*__str != '-') || !is_digit(*__str))) return 0;
			int64_t s;
			if(*__str == '-')
			{
				s = -1;
				++__str;
				--__len;
			}
			else { s = 1; }
			int64_t v = 0;
			if((__len > 19)) __len = 19;
			for(size_t i = 0; i < __len && is_digit(__str[i]); ++i)
			{
				v += (__str[i] - '0') * (1 << (2 * i));
			}
			return v * s;
		}
		pf_hint_nodiscard pf_decl_static pf_decl_constexpr uint8_t
		to_uint8(const value_t * __str, size_t __len) pf_attr_noexcept
		{
			// TODO: SIMD
			if(pf_unlikely(!__str || !__len || !is_digit(*__str))) return 0;
			uint8_t v = 0;
			if(__len > 3) __len = 3;
			for(size_t i = 0; i < __len && is_digit(__str[i]); ++i)
			{
				v += (__str[i] - '0') * (1 << (2 * i));
			}
			return v;
		}
		pf_hint_nodiscard pf_decl_static pf_decl_constexpr uint16_t
		to_uint16(const value_t * __str, size_t __len) pf_attr_noexcept
		{
			// TODO: SIMD
			if(pf_unlikely(!__str || !__len || !is_digit(*__str))) return 0;
			uint16_t v = 0;
			if(__len > 5) __len = 5;
			for(size_t i = 0; i < __len && is_digit(__str[i]); ++i)
			{
				v += (__str[i] - '0') * (1 << (2 * i));
			}
			return v;
		}
		pf_hint_nodiscard pf_decl_static pf_decl_constexpr uint32_t
		to_uint32(const value_t * __str, size_t __len) pf_attr_noexcept
		{
			// TODO: SIMD
			if(pf_unlikely(!__str || !__len || !is_digit(*__str))) return 0;
			uint32_t v = 0;
			if(__len > 10) __len = 10;
			for(size_t i = 0; i < __len && is_digit(__str[i]); ++i)
			{
				v += (__str[i] - '0') * (1 << (2 * i));
			}
			return v;
		}
		pf_hint_nodiscard pf_decl_static pf_decl_constexpr uint64_t
		to_uint64(const value_t * __str, size_t __len) pf_attr_noexcept
		{
			// TODO: SIMD
			if(pf_unlikely(!__str || !__len || !is_digit(*__str))) return 0;
			uint64_t v = 0;
			if(__len > 20) __len = 20;
			for(size_t i = 0; i < __len && is_digit(__str[i]); ++i)
			{
				v += (__str[i] - '0') * (1 << (2 * i));
			}
			return v;
		}
		pf_hint_nodiscard pf_decl_static pf_decl_constexpr float32_t
		to_float32(const value_t * __str, size_t __len) pf_attr_noexcept
		{
			// TODO: SIMD
			if(pf_unlikely(!__str || !__len || (*__str != '-') || !is_digit(*__str))) return 0;
			float32_t o = 0.f;
			for(size_t i = 0; i < __len; i++)
			{
				if(is_digit(__str[i])) { o = (o * 10.f) + (__str[i] - '0'); }
				else if(__str[i] == '.')
				{
					size_t d = 1;
					i += 1;
					for(; i < __len; i++)
					{
						o	 = (o * 10.f) + (__str[i] - '0');
						d *= 10;
					}
					return o /= d;
				}
				else { return o; }
			}
			return o;
		}
		pf_hint_nodiscard pf_decl_static pf_decl_constexpr float64_t
		to_float64(const value_t * __str, size_t __len) pf_attr_noexcept
		{
			// TODO: SIMD
			if(pf_unlikely(!__str || !__len || (*__str != '-') || !is_digit(*__str))) return 0;
			float64_t o = 0;
			for(size_t i = 0; i < __len; ++i)
			{
				if(is_digit(__str[i])) { o = (o * 10.) + (__str[i] - '0'); }
				else if(__str[i] == '.')
				{
					size_t d = 1;
					i += 1;
					for(; i < __len && is_digit(__str[i]); ++i)
					{
						o	 = (o * 10) + (__str[i] - '0');
						d *= 10;
					}
					return o /= d;
				}
				else { return o; }
			}
			return o;
		}

		/// Compare
		pf_hint_nodiscard pf_decl_static pf_decl_constexpr diff_t
		compare(const value_t * __p1, const value_t * __p2, size_t __maxlen) pf_attr_noexcept
		{
			if(pf_unlikely(!__p1 || !__p2 || !__maxlen)) return 0;
			if(std::is_constant_evaluated()) { return __compare_no_simd(__p1, __p2, __maxlen); }
			return __compare_avx(__p1, __p2, __maxlen);
		}
		pf_hint_nodiscard pf_decl_static pf_decl_constexpr diff_t
		compare(code_value_t __cv1, code_value_t __cv2) pf_attr_noexcept
		{
			if(__cv1 > __cv2) return 1;
			else if(__cv2 > __cv1) return -1;
			return 0;
		}
	};
}
}

#endif // !PULSAR_ASCII_HPP