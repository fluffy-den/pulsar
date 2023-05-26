/*! @file   char.hpp
 *  @author Louis-Quentin Noé (noe.louis-quentin@hotmail.fr)
 *  @brief
 *  @date   24-03-2023
 *
 *  @copyright Copyright (c) 2023 - Pulsar Software
 *
 *  @since 0.1.7
 */

#ifndef PULSAR_CHAR_HPP
#define PULSAR_CHAR_HPP 1

// Include: Pulsar
#include "pulsar/pulsar.hpp"
#include "pulsar/iterable.hpp"
#include "pulsar/debug.hpp"
#include "pulsar/intrin.hpp"
#include "pulsar/system.hpp"
#include "pulsar/tuple.hpp"

// Include: C++
#include <charconv>	 // NOTE: Introduce Lemire fastest solution with GCC!

// Pulsar
namespace pul
{
	/// CHAR: Debug -> Code
	enum class char_error_code
	{
		success,
		header_bits,
		too_short,
		too_long,
		overlong,
		too_large,
		surrogate
	};

	/// CHAR: Debug -> Category
	class dbg_category_char_t : public dbg_category
	{
	public:
		/// Constructors
		pf_decl_constexpr
		dbg_category_char_t() pf_attr_noexcept = default;

		/// Name
		pf_hint_nodiscard pf_decl_inline dbg_u8string_view
		name() const pf_attr_noexcept pf_attr_override
		{
			return "char encoding";
		}

		/// Message
		pf_hint_nodiscard pf_decl_inline dbg_u8string
		message(
		 uint32_t __val) const pf_attr_noexcept pf_attr_override
		{
			switch(union_cast<char_error_code>(__val))
			{
				case char_error_code::header_bits:
					return "Any byte must have fewer than 5 header bits";
				case char_error_code::too_short:
					return "The leading byte must be followed by N-1 continuation bytes, where N is the UTF-8 character length (This is also the error when the input is truncated)";
				case char_error_code::too_long:
					return "We either have too many consecutive continuation bytes or the string starts with a continuation byte";
				case char_error_code::overlong:
					return "The decoded character must be above U+7F for two-byte characters, U+7FF for three-byte characters and U+FFFF for four-byte characters";
				case char_error_code::too_large:
					return "The decoded character must be less than or equal to U+10FFFF OR less than or equal than U+7F for ASCII";
				case char_error_code::surrogate:
					return "The decoded character must be not be in U+D800...DFFF (UTF-8 or UTF-32) OR a high surrogate must be followed by a low surrogate and a low surrogate must be preceded by a high surrogate (UTF-16)";
				default: return "Unknown";
			};
		}
	};

	/// CHAR: Debug -> Category -> Instance
	pf_decl_static pf_decl_inline dbg_category_char_t __dbg_category_char_instance;

	/// CHAR: Debug -> Category -> Retrieve
	pf_decl_inline dbg_category *
	dbg_category_char() pf_attr_noexcept
	{
		return &__dbg_category_char_instance;
	}

	/// CHAR: Debug -> Error
	struct char_error_t
	{
		char_error_code code;
		size_t position;
	};


	// https://github.com/simdutf/simdutf
	/// ASCII
	namespace ascii
	{
		/// ASCII: Is valid
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr char_error_t
		validate(
		 const char_t *__str,
		 size_t __len) pf_attr_noexcept
		{
			size_t p = 0;

			/// 512
			// NOTE: In the future -> AVX-512 ascii::validate

			/// 256 (AVX2)
			if(!std::is_constant_evaluated() && (instruction_set::simd & instruction_set::AVX2_BIT))
			{
				for(; (p + 32) <= __len; p += 32)
				{
					const m256i64_t pck = _mm256_lddqu_si256(union_cast<const m256i64_t *>(&__str[p]));
					const m256i64_t cmp = _mm256_cmpgt_epi8(_mm256_set1_epi8(0), pck);
					const uint32_t msk	= _mm256_movemask_epi8(cmp);
					if(msk != 0)
					{
						for(; p < __len; ++p)
						{
							if(__str[p] < 0) return char_error_t { char_error_code::too_large, p };
						}
					}
				}
				if(p < __len)
				{
					const size_t r = __len - p;
					m256i64_t pck	 = _mm256_setzero_si256();
					memcpy(&pck, &__str[p], r);
					const m256i64_t cmp = _mm256_cmpgt_epi8(_mm256_set1_epi8(0), pck);
					const uint32_t msk	= _mm256_movemask_epi8(cmp);
					if(msk != 0)
					{
						for(; p < __len; ++p)
						{
							if(__str[p] < 0) return char_error_t { char_error_code::too_large, p };
						}
					}
				}
			}

			/// 128 (SSE2)
			if(!std::is_constant_evaluated() && (instruction_set::simd & instruction_set::SSE2_BIT))
			{
				for(; (p + 16) <= __len; p += 16)
				{
					const m128i64_t pck = _mm_lddqu_si128(union_cast<const m128i64_t *>(&__str[p]));
					const m128i64_t cmp = _mm_cmplt_epi8(pck, _mm_set1_epi8(0));
					const int32_t msk		= _mm_movemask_epi8(cmp);
					if(msk != 0)
					{
						for(; p < __len; ++p)
						{
							if(__str[p] < 0) return char_error_t { char_error_code::too_large, p };
						}
					}
				}
				if(p < __len)
				{
					const size_t r = __len - p;
					m128i64_t pck	 = _mm_setzero_si128();
					memcpy(&pck, &__str[p], r);
					const m128i64_t cmp = _mm_cmplt_epi8(pck, _mm_set1_epi8(0));
					const int32_t msk		= _mm_movemask_epi8(cmp);
					if(msk != 0)
					{
						for(; p < __len; ++p)
						{
							if(__str[p] < 0) return char_error_t { char_error_code::too_large, p };
						}
					}
				}
			}

			/// 64 * 2 (Scalar fallback)
			for(; (p + 16) <= __len; p += 16)
			{
				uint64_t buffer[2];
				memcpy(&buffer[0], &__str[p], (__len - p) * sizeof(u8char_t));
				const uint64_t bit = buffer[0] | buffer[1];
				if((bit & 0x80'80'80'80'80'80'80'80) != 0)
				{
					for(; p < __len; ++p)
					{
						if(__str[p] < 0) return char_error_t { char_error_code::too_large, p };
					}
				}
			}
			if(p != __len)
			{
				uint64_t buffer[2];
				memset(&buffer[0], 0, sizeof(u8char_t) * 16);
				memcpy(&buffer[0], &__str[p], (__len - p) * sizeof(u8char_t));
				const uint64_t bit = buffer[0] | buffer[1];
				if((bit & 0x80'80'80'80'80'80'80'80) != 0)
				{
					for(; p < __len; ++p)
					{
						if(__str[p] < 0) return char_error_t { char_error_code::too_large, p };
					}
				}
				p = __len;
			}

			/// Valid!
			return char_error_t { char_error_code::success, p };
		}

		/// ASCII: Length
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr size_t
		lenof(
		 const char_t *__str) pf_attr_noexcept
		{
			size_t c = 0;
			while(__str[c] != '\0') ++c;
			return c + 1;
		}
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr size_t
		countof(
		 const char_t *__str,
		 size_t __len) pf_attr_noexcept
		{
			ignore = __len;
			return lenof(__str);
		}

		/// ASCII: Is*
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr bool
		is_cntrl(
		 char_t __c) pf_attr_noexcept
		{
			return (__c >= 0 && __c <= 31)
					|| __c == 127;
		}
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr bool
		is_print(
		 char_t __c) pf_attr_noexcept
		{
			return __c >= 32 && __c <= 126;
		}
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr bool
		is_space(
		 char_t __c) pf_attr_noexcept
		{
			return (__c >= 9 && __c <= 13)
					|| __c == 32;
		}
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr bool
		is_blank(
		 char_t __c) pf_attr_noexcept
		{
			return __c == 9 || __c == 32;
		}
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr bool
		is_graph(
		 char_t __c) pf_attr_noexcept
		{
			return __c >= 33 && __c <= 126;
		}
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr bool
		is_punct(
		 char_t __c) pf_attr_noexcept
		{
			return (__c >= 33 && __c <= 47)
					|| (__c >= 58 && __c <= 64)
					|| (__c >= 91 && __c <= 96)
					|| (__c >= 123 && __c <= 126);
		}
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr bool
		is_upper(
		 char_t __c) pf_attr_noexcept
		{
			return __c >= 65 && __c <= 90;	// 90 >= __c <=> 91 > __c
		}
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr bool
		is_lower(
		 char_t __c) pf_attr_noexcept
		{
			return __c >= 97 && __c <= 122;	 // 122 >= __c <=> 123 > __c
		}
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr bool
		is_alpha(
		 char_t __c) pf_attr_noexcept
		{
			return is_upper(__c)
					|| is_lower(__c);
		}
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr bool
		is_alnum(
		 char_t __c) pf_attr_noexcept
		{
			return (__c >= 48 && __c <= 57)
					|| is_alpha(__c);
		}
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr bool
		is_digit(
		 char_t __c) pf_attr_noexcept
		{
			return __c >= 48 && __c <= 57;
		}
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr bool
		is_xdigit(
		 char_t __c) pf_attr_noexcept
		{
			return is_digit(__c)
					|| (__c >= 65 && __c <= 70)
					|| (__c >= 97 && __c <= 102);
		}

		/// ASCII: To*
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr char_t
		to_lower(
		 char_t __c) pf_attr_noexcept
		{
			if(is_lower(__c)) return __c + 32;
		}
		pf_decl_inline pf_decl_constexpr void
		to_lower(
		 const char_t *__in,
		 size_t __len,
		 char_t *__dst) pf_attr_noexcept
		{
			size_t p = 0;

			if(!std::is_constant_evaluated())
			{
				/// 512
				// NOTE: In the future -> AVX-512 ascii::to_lower

				/// 256 (AVX2)
				if(instruction_set::simd & instruction_set::AVX2_BIT)
				{
					for(; (p + 32) <= __len; p += 32)
					{
						const m256i64_t seq = _mm256_lddqu_si256(union_cast<const m256i64_t *>(&__in[p]));
						const m256i64_t msk = _mm256_and_si256(_mm256_cmpgt_epi8(seq, _mm256_set1_epi8(64)), _mm256_cmpgt_epi8(_mm256_set1_epi8(91), seq));
						const m256i64_t res = _mm256_blendv_epi8(seq, _mm256_add_epi8(seq, _mm256_set1_epi8(32)), msk);
						_mm256_storeu_si256(union_cast<m256i64_t *>(&__dst[p]), res);
					}
					if(p < __len)
					{
						m256i64_t seq	 = _mm256_setzero_si256();
						const size_t r = __len - p;
						memcpy(&seq, &__in[p], r);
						const m256i64_t msk = _mm256_and_si256(_mm256_cmpgt_epi8(seq, _mm256_set1_epi8(64)), _mm256_cmpgt_epi8(_mm256_set1_epi8(91), seq));
						const m256i64_t res = _mm256_blendv_epi8(seq, _mm256_add_epi8(seq, _mm256_set1_epi8(32)), msk);
						memcpy(&__dst[p], &res, r);
					}
					return;
				}

				/// 128 (SSE2)
				if(instruction_set::simd & instruction_set::SSE2_BIT)
				{
					for(; (p + 16) <= __len; p += 16)
					{
						const m128i64_t seq = _mm_lddqu_si128(union_cast<const m128i64_t *>(&__in[p]));
						const m128i64_t msk = _mm_and_si128(_mm_cmpgt_epi8(seq, _mm_set1_epi8(64)), _mm_cmplt_epi8(seq, _mm_set1_epi8(91)));
						const m128i64_t res = _mm_blendv_epi8(seq, _mm_add_epi8(seq, _mm_set1_epi8(32)), msk);
						_mm_storeu_si128(union_cast<m128i64_t *>(&__dst[p]), res);
					}
					if(p < __len)
					{
						m128i64_t seq	 = _mm_setzero_si128();
						const size_t r = __len - p;
						memcpy(&seq, &__in[p], r);
						const m128i64_t msk = _mm_and_si128(_mm_cmpgt_epi8(seq, _mm_set1_epi8(64)), _mm_cmplt_epi8(seq, _mm_set1_epi8(91)));
						const m128i64_t res = _mm_blendv_epi8(seq, _mm_add_epi8(seq, _mm_set1_epi8(32)), msk);
						memcpy(&__dst[p], &res, r);
					}
					return;
				}
			}

			/// 64
			for(; p < __len; ++p)
			{
				char_t c = __in[p];
				if(c >= 65 && c <= 90)
				{
					__dst[p] = c + 32;
				}
				else
				{
					__dst[p] = c;
				}
			}
		}
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr char_t
		to_upper(
		 char_t __c) pf_attr_noexcept
		{
			if(is_upper(__c)) return __c - 32;
		}
		pf_decl_inline pf_decl_constexpr void
		to_upper(
		 const char_t *__in,
		 size_t __len,
		 char_t *__dst) pf_attr_noexcept
		{
			size_t p = 0;

			if(!std::is_constant_evaluated())
			{
				/// 512
				// NOTE: In the future -> AVX-512 ascii::to_upper

				/// 256 (AVX2)
				if(instruction_set::simd & instruction_set::AVX2_BIT)
				{
					for(; (p + 32) <= __len; p += 32)
					{
						const m256i64_t seq = _mm256_lddqu_si256(union_cast<const m256i64_t *>(&__in[p]));
						const m256i64_t msk = _mm256_and_si256(_mm256_cmpgt_epi8(seq, _mm256_set1_epi8(96)), _mm256_cmpgt_epi8(_mm256_set1_epi8(123), seq));
						const m256i64_t res = _mm256_blendv_epi8(seq, _mm256_add_epi8(seq, _mm256_set1_epi8(-32)), msk);
						_mm256_storeu_si256(union_cast<m256i64_t *>(&__dst[p]), res);
					}
					if(p < __len)
					{
						m256i64_t seq	 = _mm256_setzero_si256();
						const size_t r = __len - p;
						memcpy(&seq, &__in[p], r);
						const m256i64_t msk = _mm256_and_si256(_mm256_cmpgt_epi8(seq, _mm256_set1_epi8(96)), _mm256_cmpgt_epi8(_mm256_set1_epi8(123), seq));
						const m256i64_t res = _mm256_blendv_epi8(seq, _mm256_add_epi8(seq, _mm256_set1_epi8(-32)), msk);
						memcpy(&__dst[p], &res, r);
					}
					return;
				}

				/// 128 (SSE2)
				if(instruction_set::simd & instruction_set::SSE2_BIT)
				{
					for(; (p + 32) <= __len; p += 32)
					{
						const m128i64_t seq = _mm_lddqu_si128(union_cast<const m128i64_t *>(&__in[p]));
						const m128i64_t msk = _mm_and_si128(_mm_cmpgt_epi8(seq, _mm_set1_epi8(96)), _mm_cmplt_epi8(seq, _mm_set1_epi8(123)));
						const m128i64_t res = _mm_blendv_epi8(seq, _mm_add_epi8(seq, _mm_set1_epi8(-32)), msk);
						_mm_storeu_si128(union_cast<m128i64_t *>(&__dst[p]), res);
					}
					if(p < __len)
					{
						m128i64_t seq	 = _mm_setzero_si128();
						const size_t r = __len - p;
						memcpy(&seq, &__in[p], r);
						const m128i64_t msk = _mm_and_si128(_mm_cmpgt_epi8(seq, _mm_set1_epi8(96)), _mm_cmplt_epi8(seq, _mm_set1_epi8(123)));
						const m128i64_t res = _mm_blendv_epi8(seq, _mm_add_epi8(seq, _mm_set1_epi8(-32)), msk);
						memcpy(&__dst[p], &res, r);
					}
					return;
				}
			}

			/// 64
			for(; p < __len; ++p)
			{
				char_t c = __in[p];
				if(c >= 97 && c <= 122)
				{
					__dst[p] = c - 32;
				}
				else
				{
					__dst[p] = c;
				}
			}
		}

		/// ASCII: Errc => Char Error Code
		pf_hint_nodiscard pf_decl_inline char_error_code
		__std_error_to_char_error_code(
		 std::errc __stderr) pf_attr_noexcept
		{
			char_error_code c;
			switch(__stderr)
			{
				case std::errc::value_too_large: c = char_error_code::too_large; break;
				default: c = char_error_code::success; break;
			};
			return c;
		}

		/// ASCII: To Scalar
		template<typename _Ty>
		pf_decl_inline char_error_t
		to_scalar(
		 const char_t *__str,
		 size_t __len,
		 _Ty &__scalar,
		 int32_t __base = 10) pf_attr_noexcept
			requires(std::is_arithmetic_v<_Ty>)
		{
			auto r = std::from_chars<_Ty>(__str, __str + __len, __scalar, __base);
			return char_error_t { __std_error_to_char_error_code(r.ec), distof(__str, r.ptr) };
		}

		/// ASCII: To Chars
		template<typename _Ty>
		pf_decl_inline char_error_t
		to_chars(
		 char_t *__str,
		 size_t __len,
		 _Ty __v,
		 uint32_t __base = 10)
			requires(std::is_arithmetic_v<_Ty>)
		{
			auto r = std::to_chars(__str, __str + __len, __v, __base);
			return char_error_t { __std_error_to_char_error_code(r.ec), distof(__str, r.ptr) };
		}
	}	 // namespace ascii




	/// UTF8
	namespace utf8
	{
		// AVX2
		pf_hint_nodiscard pf_decl_always_inline bool
		__is_ascii_avx2(
		 m256i64_t __in) pf_attr_noexcept
		{
			const m256i64_t cmp = _mm256_cmpgt_epi8(_mm256_set1_epi8(0), __in);
			const int32_t msk		= _mm256_movemask_epi8(cmp);
			return msk == 0;
		}
		pf_hint_nodiscard pf_decl_always_inline m256i64_t
		__gt_bits_avx2(
		 const m256i64_t __l,
		 const m256i64_t __r) pf_attr_noexcept
		{
			return _mm256_subs_epu8(__l, __r);
		}
		pf_hint_nodiscard pf_decl_always_inline m256i64_t
		__is_incomplete_avx2(
		 const m256i64_t __in) pf_attr_noexcept
		{
			pf_alignas(32) pf_decl_static const uint8_t maxarr[32] = {
				255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 0b11110000u - 1, 0b11100000u - 1, 0b11000000u - 1
			};
			m256i64_t maxval = _mm256_load_si256(union_cast<const m256i64_t *>(&maxarr[0]));
			return __gt_bits_avx2(__in, maxval);
		}
		template<size_t _N = 1>
		pf_hint_nodiscard pf_decl_always_inline m256i64_t
		__prev_avx2(
		 const m256i64_t __in,
		 const m256i64_t __prevInput) pf_attr_noexcept
		{
			return _mm256_alignr_epi8(__in, _mm256_permute2x128_si256(__prevInput, __in, 0x21), 16 - _N);
		}
		template<size_t _N = 1>
		pf_hint_nodiscard pf_decl_always_inline m256i64_t
		__shr_avx2(
		 const m256i64_t __in) pf_attr_noexcept
		{
			return _mm256_and_si256(_mm256_srli_epi16(__in, _N), _mm256_set1_epi8(uint8_t(0xFFu >> _N)));
		}
		pf_hint_nodiscard pf_decl_always_inline m256i64_t
		__lookup_16_avx2(
		 const m256i64_t __in,
		 const m256i64_t __table) pf_attr_noexcept
		{
			return _mm256_shuffle_epi8(__table, __in);
		}
		pf_hint_nodiscard pf_decl_always_inline m256i64_t
		__store_16_repeat_epu8_avx2(
		 const uint8_t __v0,
		 const uint8_t __v1,
		 const uint8_t __v2,
		 const uint8_t __v3,
		 const uint8_t __v4,
		 const uint8_t __v5,
		 const uint8_t __v6,
		 const uint8_t __v7,
		 const uint8_t __v8,
		 const uint8_t __v9,
		 const uint8_t __v10,
		 const uint8_t __v11,
		 const uint8_t __v12,
		 const uint8_t __v13,
		 const uint8_t __v14,
		 const uint8_t __v15) pf_attr_noexcept
		{
			return _mm256_setr_epi8(__v0, __v1, __v2, __v3, __v4, __v5, __v6, __v7, __v8, __v9, __v10, __v11, __v12, __v13, __v14, __v15, __v0, __v1, __v2, __v3, __v4, __v5, __v6, __v7, __v8, __v9, __v10, __v11, __v12, __v13, __v14, __v15);
		}
		pf_hint_nodiscard pf_decl_always_inline m256i64_t
		__check_special_cases_avx2(
		 const m256i64_t __in,
		 const m256i64_t __prev1) pf_attr_noexcept
		{
			pf_decl_constexpr const uint8_t TOO_SHORT			 = 1 << 0;	// 11______ 0_______
																																// 11______ 11______
			pf_decl_constexpr const uint8_t TOO_LONG			 = 1 << 1;	// 0_______ 10______
			pf_decl_constexpr const uint8_t OVERLONG_3		 = 1 << 2;	// 11100000 100_____
			pf_decl_constexpr const uint8_t SURROGATE			 = 1 << 4;	// 11101101 101_____
			pf_decl_constexpr const uint8_t OVERLONG_2		 = 1 << 5;	// 1100000_ 10______
			pf_decl_constexpr const uint8_t TWO_CONTS			 = 1 << 7;	// 10______ 10______
			pf_decl_constexpr const uint8_t TOO_LARGE			 = 1 << 3;	// 11110100 1001____
																																// 11110100 101_____
																																// 11110101 1001____
																																// 11110101 101_____
																																// 1111011_ 1001____
																																// 1111011_ 101_____
																																// 11111___ 1001____
																																// 11111___ 101_____
			pf_decl_constexpr const uint8_t TOO_LARGE_1000 = 1 << 6;
			// 11110101 1000____
			// 1111011_ 1000____
			// 11111___ 1000____
			pf_decl_constexpr const uint8_t OVERLONG_4		 = 1 << 6;	// 11110000 1000____

			// byte_1_high
			const m256i64_t byte_1_high = __lookup_16_avx2(
			 __shr_avx2<4>(__prev1),
			 __store_16_repeat_epu8_avx2(
				// 0_______ ________ <ASCII in byte 1>
				TOO_LONG,
				TOO_LONG,
				TOO_LONG,
				TOO_LONG,
				TOO_LONG,
				TOO_LONG,
				TOO_LONG,
				TOO_LONG,
				// 10______ ________ <continuation in byte 1>
				TWO_CONTS,
				TWO_CONTS,
				TWO_CONTS,
				TWO_CONTS,
				// 1100____ ________ <two byte lead in byte 1>
				TOO_SHORT | OVERLONG_2,
				// 1101____ ________ <two byte lead in byte 1>
				TOO_SHORT,
				// 1110____ ________ <three byte lead in byte 1>
				TOO_SHORT | OVERLONG_3 | SURROGATE,
				// 1111____ ________ <four+ byte lead in byte 1>
				TOO_SHORT | TOO_LARGE | TOO_LARGE_1000 | OVERLONG_4));
			constexpr const uint8_t CARRY = TOO_SHORT | TOO_LONG | TWO_CONTS;	 // These all have ____ in byte 1.

			// byte_1_low
			const m256i64_t byte_1_low = __lookup_16_avx2(
			 _mm256_and_si256(__prev1, _mm256_set1_epi8(0x0F)),
			 __store_16_repeat_epu8_avx2(
				// ____0000 ________
				CARRY | OVERLONG_3 | OVERLONG_2 | OVERLONG_4,
				// ____0001 ________
				CARRY | OVERLONG_2,
				// ____001_ ________
				CARRY,
				CARRY,

				// ____0100 ________
				CARRY | TOO_LARGE,
				// ____0101 ________
				CARRY | TOO_LARGE | TOO_LARGE_1000,
				// ____011_ ________
				CARRY | TOO_LARGE | TOO_LARGE_1000,
				CARRY | TOO_LARGE | TOO_LARGE_1000,

				// ____1___ ________
				CARRY | TOO_LARGE | TOO_LARGE_1000,
				CARRY | TOO_LARGE | TOO_LARGE_1000,
				CARRY | TOO_LARGE | TOO_LARGE_1000,
				CARRY | TOO_LARGE | TOO_LARGE_1000,
				CARRY | TOO_LARGE | TOO_LARGE_1000,
				// ____1101 ________
				CARRY | TOO_LARGE | TOO_LARGE_1000 | SURROGATE,
				CARRY | TOO_LARGE | TOO_LARGE_1000,
				CARRY | TOO_LARGE | TOO_LARGE_1000));

			// byte_2_high
			const m256i64_t byte_2_high = __lookup_16_avx2(
			 __shr_avx2<4>(__in),
			 __store_16_repeat_epu8_avx2(
				// ________ 0_______ <ASCII in byte 2>
				TOO_SHORT,
				TOO_SHORT,
				TOO_SHORT,
				TOO_SHORT,
				TOO_SHORT,
				TOO_SHORT,
				TOO_SHORT,
				TOO_SHORT,

				// ________ 1000____
				TOO_LONG | OVERLONG_2 | TWO_CONTS | OVERLONG_3 | TOO_LARGE_1000 | OVERLONG_4,
				// ________ 1001____
				TOO_LONG | OVERLONG_2 | TWO_CONTS | OVERLONG_3 | TOO_LARGE,
				// ________ 101_____
				TOO_LONG | OVERLONG_2 | TWO_CONTS | SURROGATE | TOO_LARGE,
				TOO_LONG | OVERLONG_2 | TWO_CONTS | SURROGATE | TOO_LARGE,

				// ________ 11______
				TOO_SHORT,
				TOO_SHORT,
				TOO_SHORT,
				TOO_SHORT));

			// end
			return _mm256_and_si256(_mm256_and_si256(byte_1_high, byte_1_low), byte_2_high);
		}
		pf_decl_always_inline m256i64_t
		__must_be_2_3_continuation_avx2(
		 const m256i64_t __prev2,
		 const m256i64_t __prev3) pf_attr_noexcept
		{
			const m256i64_t is3byte = _mm256_subs_epu8(__prev2, _mm256_set1_epi8(-33));
			const m256i64_t is4byte = _mm256_subs_epu8(__prev3, _mm256_set1_epi8(-17));
			return _mm256_cmpgt_epi8(_mm256_or_si256(is3byte, is4byte), _mm256_set1_epi8(0));
		}
		pf_decl_always_inline m256i64_t
		__check_multibyte_lengths_avx2(
		 const m256i64_t __in,
		 const m256i64_t __prev,
		 const m256i64_t __sc) pf_attr_noexcept
		{
			m256i64_t prev2			= __prev_avx2<2>(__in, __prev);
			m256i64_t prev3			= __prev_avx2<3>(__in, __prev);
			m256i64_t must23		= __must_be_2_3_continuation_avx2(prev2, prev3);
			m256i64_t must23_80 = _mm256_and_si256(must23, _mm256_set1_epi8(-128));
			return _mm256_xor_si256(must23_80, __sc);
		}
		pf_decl_always_inline void
		__check_bytes_avx2(
		 const m256i64_t __in,
		 m256i64_t &__error,
		 const m256i64_t __prevInput) pf_attr_noexcept
		{
			const m256i64_t prev1 = __prev_avx2<1>(__in, __prevInput);
			const m256i64_t sc		= __check_special_cases_avx2(__in, prev1);
			__error								= _mm256_or_si256(__error, __check_multibyte_lengths_avx2(__in, __prevInput, sc));
		}
		pf_decl_always_inline void
		__check_next_input_avx2(
		 const m256i64_t __in,
		 m256i64_t &__error,
		 m256i64_t &__prevInput,
		 m256i64_t &__prevIncomplete) pf_attr_noexcept
		{
			// Is Ascii
			if(pf_likely(__is_ascii_avx2(__in)))
			{
				__error = _mm256_or_si256(__error, __prevIncomplete);
			}
			else
			{
				__check_bytes_avx2(__in, __error, __prevInput);
			}
			__prevIncomplete = __is_incomplete_avx2(__in);
			__prevInput			 = __in;
		}
		pf_decl_always_inline void
		__check_eof_avx2(
		 m256i64_t &__error,
		 const m256i64_t __prevIncomplete) pf_attr_noexcept
		{
			__error = _mm256_or_si256(__error, __prevIncomplete);
		}

		// >=
		pf_hint_nodiscard pf_decl_always_inline m256i64_t
		__greater_equal_avx2(
		 const m256i64_t __in1,
		 const m256i64_t __in2) pf_attr_noexcept
		{
			return _mm256_cmpeq_epi8(_mm256_min_epu8(__in1, __in2), __in2);
		}

		// <=
		pf_hint_nodiscard pf_decl_always_inline m256i64_t
		__smaller_equal_avx2(
		 const m256i64_t __in1,
		 const m256i64_t __in2) pf_attr_noexcept
		{
			return _mm256_cmpeq_epi8(_mm256_max_epu8(__in2, __in1), __in2);
		}

		// SSE2
		pf_hint_nodiscard pf_decl_always_inline bool
		__is_ascii_sse2(
		 m128i64_t __in) pf_attr_noexcept
		{
			const m128i64_t cmp = _mm_cmpgt_epi8(_mm_set1_epi8(0), __in);
			const uint32_t msk	= _mm_movemask_epi8(cmp);
			return msk == 0;
		}
		pf_hint_nodiscard pf_decl_always_inline m128i64_t
		__gt_bits_sse2(
		 const m128i64_t __l,
		 const m128i64_t __r) pf_attr_noexcept
		{
			return _mm_subs_epu8(__l, __r);
		}
		pf_hint_nodiscard pf_decl_always_inline m128i64_t
		__is_incomplete_sse2(
		 const m128i64_t __in) pf_attr_noexcept
		{
			pf_alignas(16) pf_decl_static const uint8_t maxarr[32] = {
				255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 0b11110000u - 1, 0b11100000u - 1, 0b11000000u - 1
			};
			m128i64_t maxval = _mm_load_si128(union_cast<const m128i64_t *>(&maxarr[0]));
			return __gt_bits_sse2(__in, maxval);
		}
		template<size_t _N = 1>
		pf_hint_nodiscard pf_decl_always_inline m128i64_t
		__prev_sse2(
		 const m128i64_t __in,
		 const m128i64_t __prevInput) pf_attr_noexcept
		{
			return _mm_alignr_epi8(__in, __prevInput, 16 - _N);
		}
		template<size_t _N = 1>
		pf_hint_nodiscard pf_decl_always_inline m128i64_t
		__shr_sse2(
		 const m128i64_t __in) pf_attr_noexcept
		{
			return _mm_and_si128(_mm_srli_epi16(__in, _N), _mm_set1_epi8(uint8_t(0xFFu >> _N)));
		}
		pf_hint_nodiscard pf_decl_always_inline m128i64_t
		__lookup_16_sse2(
		 const m128i64_t __in,
		 const m128i64_t __table) pf_attr_noexcept
		{
			return _mm_shuffle_epi8(__table, __in);
		}
		pf_hint_nodiscard pf_decl_always_inline m128i64_t
		__store_16_repeat_epu8_sse2(
		 const uint8_t __v0,
		 const uint8_t __v1,
		 const uint8_t __v2,
		 const uint8_t __v3,
		 const uint8_t __v4,
		 const uint8_t __v5,
		 const uint8_t __v6,
		 const uint8_t __v7,
		 const uint8_t __v8,
		 const uint8_t __v9,
		 const uint8_t __v10,
		 const uint8_t __v11,
		 const uint8_t __v12,
		 const uint8_t __v13,
		 const uint8_t __v14,
		 const uint8_t __v15) pf_attr_noexcept
		{
			return _mm_setr_epi8(__v0, __v1, __v2, __v3, __v4, __v5, __v6, __v7, __v8, __v9, __v10, __v11, __v12, __v13, __v14, __v15);
		}
		pf_hint_nodiscard pf_decl_always_inline m128i64_t
		__check_special_cases_sse2(
		 const m128i64_t __in,
		 const m128i64_t __prev1) pf_attr_noexcept
		{
			pf_decl_constexpr const uint8_t TOO_SHORT			 = 1 << 0;	// 11______ 0_______
																																// 11______ 11______
			pf_decl_constexpr const uint8_t TOO_LONG			 = 1 << 1;	// 0_______ 10______
			pf_decl_constexpr const uint8_t OVERLONG_3		 = 1 << 2;	// 11100000 100_____
			pf_decl_constexpr const uint8_t SURROGATE			 = 1 << 4;	// 11101101 101_____
			pf_decl_constexpr const uint8_t OVERLONG_2		 = 1 << 5;	// 1100000_ 10______
			pf_decl_constexpr const uint8_t TWO_CONTS			 = 1 << 7;	// 10______ 10______
			pf_decl_constexpr const uint8_t TOO_LARGE			 = 1 << 3;	// 11110100 1001____
																																// 11110100 101_____
																																// 11110101 1001____
																																// 11110101 101_____
																																// 1111011_ 1001____
																																// 1111011_ 101_____
																																// 11111___ 1001____
																																// 11111___ 101_____
			pf_decl_constexpr const uint8_t TOO_LARGE_1000 = 1 << 6;
			// 11110101 1000____
			// 1111011_ 1000____
			// 11111___ 1000____
			pf_decl_constexpr const uint8_t OVERLONG_4		 = 1 << 6;	// 11110000 1000____

			// byte_1_high
			const m128i64_t byte_1_high = __lookup_16_sse2(
			 __shr_sse2<4>(__prev1),
			 __store_16_repeat_epu8_sse2(
				// 0_______ ________ <ASCII in byte 1>
				TOO_LONG,
				TOO_LONG,
				TOO_LONG,
				TOO_LONG,
				TOO_LONG,
				TOO_LONG,
				TOO_LONG,
				TOO_LONG,
				// 10______ ________ <continuation in byte 1>
				TWO_CONTS,
				TWO_CONTS,
				TWO_CONTS,
				TWO_CONTS,
				// 1100____ ________ <two byte lead in byte 1>
				TOO_SHORT | OVERLONG_2,
				// 1101____ ________ <two byte lead in byte 1>
				TOO_SHORT,
				// 1110____ ________ <three byte lead in byte 1>
				TOO_SHORT | OVERLONG_3 | SURROGATE,
				// 1111____ ________ <four+ byte lead in byte 1>
				TOO_SHORT | TOO_LARGE | TOO_LARGE_1000 | OVERLONG_4));
			constexpr const uint8_t CARRY = TOO_SHORT | TOO_LONG | TWO_CONTS;	 // These all have ____ in byte 1.

			// byte_1_low
			const m128i64_t byte_1_low = __lookup_16_sse2(
			 _mm_and_si128(__prev1, _mm_set1_epi8(0x0F)),
			 __store_16_repeat_epu8_sse2(
				// ____0000 ________
				CARRY | OVERLONG_3 | OVERLONG_2 | OVERLONG_4,
				// ____0001 ________
				CARRY | OVERLONG_2,
				// ____001_ ________
				CARRY,
				CARRY,

				// ____0100 ________
				CARRY | TOO_LARGE,
				// ____0101 ________
				CARRY | TOO_LARGE | TOO_LARGE_1000,
				// ____011_ ________
				CARRY | TOO_LARGE | TOO_LARGE_1000,
				CARRY | TOO_LARGE | TOO_LARGE_1000,

				// ____1___ ________
				CARRY | TOO_LARGE | TOO_LARGE_1000,
				CARRY | TOO_LARGE | TOO_LARGE_1000,
				CARRY | TOO_LARGE | TOO_LARGE_1000,
				CARRY | TOO_LARGE | TOO_LARGE_1000,
				CARRY | TOO_LARGE | TOO_LARGE_1000,
				// ____1101 ________
				CARRY | TOO_LARGE | TOO_LARGE_1000 | SURROGATE,
				CARRY | TOO_LARGE | TOO_LARGE_1000,
				CARRY | TOO_LARGE | TOO_LARGE_1000));

			// byte_2_high
			const m128i64_t byte_2_high = __lookup_16_sse2(
			 __shr_sse2<4>(__in),
			 __store_16_repeat_epu8_sse2(
				// ________ 0_______ <ASCII in byte 2>
				TOO_SHORT,
				TOO_SHORT,
				TOO_SHORT,
				TOO_SHORT,
				TOO_SHORT,
				TOO_SHORT,
				TOO_SHORT,
				TOO_SHORT,

				// ________ 1000____
				TOO_LONG | OVERLONG_2 | TWO_CONTS | OVERLONG_3 | TOO_LARGE_1000 | OVERLONG_4,
				// ________ 1001____
				TOO_LONG | OVERLONG_2 | TWO_CONTS | OVERLONG_3 | TOO_LARGE,
				// ________ 101_____
				TOO_LONG | OVERLONG_2 | TWO_CONTS | SURROGATE | TOO_LARGE,
				TOO_LONG | OVERLONG_2 | TWO_CONTS | SURROGATE | TOO_LARGE,

				// ________ 11______
				TOO_SHORT,
				TOO_SHORT,
				TOO_SHORT,
				TOO_SHORT));

			// end
			return _mm_and_si128(_mm_and_si128(byte_1_high, byte_1_low), byte_2_high);
		}
		pf_decl_always_inline m128i64_t
		__must_be_2_3_continuation_sse2(
		 const m128i64_t __prev2,
		 const m128i64_t __prev3) pf_attr_noexcept
		{
			const m128i64_t is3byte = _mm_subs_epu8(__prev2, _mm_set1_epi8(-33));
			const m128i64_t is4byte = _mm_subs_epu8(__prev3, _mm_set1_epi8(-17));
			return _mm_cmpgt_epi8(_mm_or_si128(is3byte, is4byte), _mm_set1_epi8(0));
		}
		pf_decl_always_inline m128i64_t
		__check_multibyte_lengths_sse2(
		 const m128i64_t __in,
		 const m128i64_t __prev,
		 const m128i64_t __sc) pf_attr_noexcept
		{
			m128i64_t prev2			= __prev_sse2<2>(__in, __prev);
			m128i64_t prev3			= __prev_sse2<3>(__in, __prev);
			m128i64_t must23		= __must_be_2_3_continuation_sse2(prev2, prev3);
			m128i64_t must23_80 = _mm_and_si128(must23, _mm_set1_epi8(-128));
			return _mm_xor_si128(must23_80, __sc);
		}
		pf_decl_always_inline void
		__check_bytes_sse2(
		 const m128i64_t __in,
		 m128i64_t &__error,
		 const m128i64_t __prevInput) pf_attr_noexcept
		{
			const m128i64_t prev1		= __prev_sse2<1>(__in, __prevInput);
			const m128i64_t special = __check_special_cases_sse2(__in, prev1);
			__error									= _mm_or_si128(__error, __check_multibyte_lengths_sse2(__in, __prevInput, special));
		}
		pf_decl_always_inline void
		__check_next_input_sse2(
		 const m128i64_t __in,
		 m128i64_t &__error,
		 m128i64_t &__prevInput,
		 m128i64_t &__prevIncomplete) pf_attr_noexcept
		{
			// Is Ascii
			if(pf_likely(__is_ascii_sse2(__in)))
			{
				__error = _mm_or_si128(__error, __prevIncomplete);
			}
			else
			{
				__check_bytes_sse2(__in, __error, __prevInput);
			}
			__prevIncomplete = __is_incomplete_sse2(__in);
			__prevInput			 = __in;
		}
		pf_decl_always_inline void
		__check_eof_sse2(
		 m128i64_t &__error,
		 const m128i64_t __prevIncomplete) pf_attr_noexcept
		{
			__error = _mm_or_si128(__error, __prevIncomplete);
		}

		// >=
		pf_hint_nodiscard pf_decl_always_inline m128i64_t
		__greater_equal_sse2(
		 const m128i64_t __in1,
		 const m128i64_t __in2) pf_attr_noexcept
		{
			return _mm_cmpeq_epi8(_mm_min_epu8(__in1, __in2), __in2);
		}

		// <=
		pf_hint_nodiscard pf_decl_always_inline m128i64_t
		__smaller_equal_sse2(
		 const m128i64_t __in1,
		 const m128i64_t __in2) pf_attr_noexcept
		{
			return _mm_cmpeq_epi8(_mm_max_epu8(__in1, __in2), __in2);
		}

		// Scalar
		pf_hint_nodiscard pf_decl_constexpr pf_decl_always_inline char_error_t
		__validate_with_errors_utf8(
		 const u8char_t *__str,
		 size_t __len) pf_attr_noexcept
		{
			// check of the next 8 bytes are ascii.
			size_t p		= 0;
			uint32_t cp = 0;
			while(p < __len)
			{
				size_t np = p + 16;
				if(np <= __len)
				{
					// if it is safe to read 8 more bytes, check that they are ascii
					uint64_t v1;
					memcpy(&v1, &__str[p], sizeof(uint64_t));
					uint64_t v2;
					memcpy(&v2, &__str[p] + sizeof(uint64_t), sizeof(uint64_t));
					uint64_t v { v1 | v2 };
					if((v & -12'880'808'080'808'080) == 0)
					{
						p = np;
						continue;
					}
				}
				u8char_t byte = __str[p];

				while((uint8_t)(byte) < 0b10000000)
				{
					if(++p == __len) return char_error_t { char_error_code::success, __len };
					byte = __str[p];
				}

				if((byte & 0b11100000) == 0b11000000)
				{
					np = p + 2;
					if(np > __len) return char_error_t { char_error_code::too_short, p };
					if((__str[p + 1] & 0b11000000) != 0b10000000) return char_error_t { char_error_code::too_short, p };
					// range check
					cp = (byte & 0b00011111) << 6 | (__str[p + 1] & 0b00111111);
					if((cp < 0x80) || (0x7'ff < cp)) return char_error_t { char_error_code::overlong, p };
				}
				else if((byte & 0b11110000) == 0b11100000)
				{
					np = p + 3;
					if(np > __len) return char_error_t { char_error_code::too_short, p };
					if((__str[p + 1] & 0b11000000) != 0b10000000) return char_error_t { char_error_code::too_short, p };
					if((__str[p + 2] & 0b11000000) != 0b10000000) return char_error_t { char_error_code::too_short, p };
					// range check
					cp = (byte & 0b00001111) << 12 | (__str[p + 1] & 0b00111111) << 6 | (__str[p + 2] & 0b00111111);
					if((cp < 0x8'00) || (0xff'ff < cp)) return char_error_t { char_error_code::overlong, p };
					if(0xd7'ff < cp && cp < 0xe0'00) return char_error_t { char_error_code::surrogate, p };
				}
				else if((byte & 0b11111000) == 0b11110000)
				{
					// 0b11110000
					np = p + 4;
					if(np > __len) return char_error_t { char_error_code::too_short, p };
					if((__str[p + 1] & 0b11000000) != 0b10000000) return char_error_t { char_error_code::too_short, p };
					if((__str[p + 2] & 0b11000000) != 0b10000000) return char_error_t { char_error_code::too_short, p };
					if((__str[p + 3] & 0b11000000) != 0b10000000) return char_error_t { char_error_code::too_short, p };
					// range check
					cp =
					 (byte & 0b00000111) << 18 | (__str[p + 1] & 0b00111111) << 12 | (__str[p + 2] & 0b00111111) << 6 | (__str[p + 3] & 0b00111111);
					if(cp <= 0xff'ff) return char_error_t { char_error_code::overlong, p };
					if(0x10'ff'ff < cp) return char_error_t { char_error_code::too_large, p };
				}
				else
				{
					// we either have too many continuation bytes or an invalid leading byte
					if((byte & 0b11000000) == 0b10000000)
						return char_error_t { char_error_code::too_long, p };
					else
						return char_error_t { char_error_code::header_bits, p };
				}

				p = np;
			}
			return char_error_t { char_error_code::success, __len };
		}
		pf_hint_nodiscard pf_decl_always_inline char_error_t
		__rewind_and_validate_with_errors_utf8(
		 const u8char_t *__str,
		 size_t __len) pf_attr_noexcept
		{
			size_t extralen = 0;
			for(size_t i = 0; i < 5; i++)
			{
				u8char_t byte = *__str;
				if((byte & 0b11000000) != 0b10000000)
				{
					break;
				}
				else
				{
					__str--;
					++extralen;
				}
			}
			char_error_t err = __validate_with_errors_utf8(__str, __len + extralen);
			err.position		-= extralen;
			return err;
		}
		pf_hint_nodiscard pf_decl_always_inline pf_decl_constexpr char_error_t
		validate(
		 const u8char_t *__str,
		 size_t __len) pf_attr_noexcept
		{
			size_t p = 0;
			//// https://github.com/simdutf

			if(!std::is_constant_evaluated())
			{
				/// AVX512
				// NOTE: AVX-512 SIMD utf8::validate

				/// AVX2
				if(instruction_set::simd & instruction_set::AVX2_BIT)
				{
					m256i64_t errmsk				 = _mm256_setzero_si256();
					m256i64_t previnput			 = _mm256_setzero_si256();
					m256i64_t previncomplete = _mm256_setzero_si256();
					for(; (p + 32) <= __len; p += 32)
					{
						const m256i64_t in = _mm256_lddqu_si256(union_cast<const m256i64_t *>(&__str[p]));
						__check_next_input_avx2(in, errmsk, previnput, previncomplete);
						// Error?
						if(!_mm256_testz_si256(errmsk, errmsk))
						{
							if(p != 0) --p;
							char_error_t err = __rewind_and_validate_with_errors_utf8(__str + p, __len - p);
							err.position		+= p;
							return err;
						}
					}
					if(p < __len)
					{
						m256i64_t in	 = _mm256_setzero_si256();
						const size_t r = __len - p;
						memcpy(&in, &__str[p], r);
						__check_next_input_avx2(in, errmsk, previnput, previncomplete);
						__check_eof_avx2(errmsk, previncomplete);
						if(!_mm256_testz_si256(errmsk, errmsk))
						{
							if(p != 0) --p;
							char_error_t err = __rewind_and_validate_with_errors_utf8(__str + p, __len - p);
							err.position		+= p;
							return err;
						}
					}
					return char_error_t { char_error_code::success, __len };
				}

				/// SSE2
				if(instruction_set::simd & instruction_set::SSE2_BIT)
				{
					m128i64_t errmsk				 = _mm_setzero_si128();
					m128i64_t previnput			 = _mm_setzero_si128();
					m128i64_t previncomplete = _mm_setzero_si128();
					for(; (p + 16) <= __len; p += 16)
					{
						const m128i64_t in = _mm_lddqu_si128(union_cast<const m128i64_t *>(&__str[p]));
						__check_next_input_sse2(in, errmsk, previnput, previncomplete);
						// Error?
						if(!_mm_testz_si128(errmsk, errmsk))
						{
							if(p != 0) --p;
							char_error_t err = __rewind_and_validate_with_errors_utf8(__str + p, __len - p);
							err.position		+= p;
							return err;
						}
					}
					if(p < __len)
					{
						m128i64_t in	 = _mm_setzero_si128();
						const size_t r = __len - p;
						memcpy(&in, &__str[p], r);
						__check_next_input_sse2(in, errmsk, previnput, previncomplete);
						__check_eof_sse2(errmsk, previncomplete);
						if(!_mm_testz_si128(errmsk, errmsk))
						{
							if(p != 0) --p;
							char_error_t err = __rewind_and_validate_with_errors_utf8(__str + p, __len - p);
							err.position		+= p;
							return err;
						}
					}
					return char_error_t { char_error_code::success, __len };
				}
			}

			/// 64
			// https://github.com/simdutf/simdutf/blob/master/src/scalar/utf8.h
			return __validate_with_errors_utf8(__str, __len);
		}

		/// UTF8: Code __len
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr size_t
		code_point_len(
		 u8code_t __c) pf_attr_noexcept
		{
			if(__c >= 0x1'00'00) return 4;
			if(__c >= 0x0'08'00) return 3;
			if(__c >= 0x0'00'80) return 2;
			return 1;
		}
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr size_t
		code_len(
		 const u8char_t *__val) pf_attr_noexcept
		{
			u8char_t val = *__val;
			if((val & 0b10000000) != 0)
			{
				if((val & 0b00100000) == 1)
				{
					if((val & 0b00010000) == 1)
					{
						return 4;
					}
					else
					{
						return 3;
					}
				}
				else
				{
					return 2;
				}
			}
			else
			{
				return 1;
			}
		}

		/// UTF8: To Code Count
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr u8code_t
		to_code_point(
		 const u8char_t *__val) pf_attr_noexcept
		{
			switch(code_len(__val))
			{
				case 4: return (__val[0] & 0x07) << 15
										 | (__val[1] & 0x3F) << 12
										 | (__val[2] & 0x3F) << 6
										 | (__val[3] & 0x3F);

				case 3: return (__val[0] & 0x0F) << 10
										 | (__val[1] & 0x3F) << 6
										 | (__val[2] & 0x3F);

				case 2: return (__val[0] & 0x1F) << 5
										 | (__val[1] & 0x3F);

				case 1: return (__val[0] & 0x7F);

				default:
				{
					pf_assert(false, "Incorrect unicode code point at location={}.", union_cast<const void *>(__val));
					return 0;
				}
			}
		}
		pf_decl_inline pf_decl_constexpr size_t
		to_code(
		 u8code_t __c,
		 u8char_t *__dst) pf_attr_noexcept	// 21
		{
			const size_t __len = code_point_len(__c);
			switch(__len)
			{
				case 4:
				{
					__dst[0] = union_cast<u8char_t>(0b11110000u + (__c & 0x07'00'00'00u));
					__dst[1] = union_cast<u8char_t>(0b10000000u + (__c & 0x00'3F'00'00u));
					__dst[2] = union_cast<u8char_t>(0b10000000u + (__c & 0x00'00'3F'00u));
					__dst[3] = union_cast<u8char_t>(0b10000000u + (__c & 0x00'00'00'3Fu));
					break;
				}
				case 3:
				{
					__dst[0] = union_cast<u8char_t>(0b11100000u + (__c & 0x0F'00'00u));
					__dst[1] = union_cast<u8char_t>(0b10000000u + (__c & 0x00'3F'00u));
					__dst[2] = union_cast<u8char_t>(0b10000000u + (__c & 0x00'00'3Fu));
					break;
				}
				case 2:
				{
					__dst[0] = union_cast<u8char_t>(0b11000000u + (__c & 0x1F'00u));
					__dst[1] = union_cast<u8char_t>(0b10000000u + (__c & 0x00'3Fu));
					break;
				}
				case 1:
				{
					__dst[0] = (__c & 0x7Fu);
					break;
				}
				default:
				{
					pf_assert(false, "Wrong code point length!");
					break;
				}
					return __len;
			}
		}

		/// UTF8: Length
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr size_t
		lenof(
		 const u8char_t *__str)
		{
			return ascii::lenof(union_cast<char_t *>(__str));
		}
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr size_t
		countof(
		 const u8char_t *__str,
		 size_t __len) pf_attr_noexcept
		{
			size_t p = 0;
			size_t c = 0;
			if(!std::is_constant_evaluated())
			{
				/// NOTE: 512

				/// 256
				if(instruction_set::simd & instruction_set::AVX2_BIT)
				{
					for(; p + 32 <= __len; p += 32)
					{
						const m256i64_t seq = _mm256_lddqu_si256(union_cast<const m256i64_t *>(&__str[p]));
						const m256i64_t cmp = ~_mm256_cmpeq_epi8(_mm256_and_si256(seq, _mm256_set1_epi8(-64)), _mm256_set1_epi8(-128));
						const int32_t mask	= _mm256_movemask_epi8(cmp);
						c									 += _popcnt32(mask);
					}
					if(p < __len)
					{
						m256i64_t seq	 = _mm256_set1_epi8(-128);
						const size_t r = __len - p;
						memcpy(&seq, &__str[p], r);
						const m256i64_t cmp = ~_mm256_cmpeq_epi8(_mm256_and_si256(seq, _mm256_set1_epi8(-64)), _mm256_set1_epi8(-128));
						const int32_t mask	= _mm256_movemask_epi8(cmp);
						c									 += _popcnt32(mask);
					}
					return c;
				}

				/// 128
				if(instruction_set::simd & instruction_set::SSE2_BIT)
				{
					for(; p + 16 <= __len; p += 16)
					{
						const m128i64_t seq = _mm_lddqu_si128(union_cast<const m128i64_t *>(&__str[p]));
						const m128i64_t cmp = ~_mm_cmpeq_epi8(_mm_and_si128(seq, _mm_set1_epi8(-64)), _mm_set1_epi8(-128));
						const int32_t mask	= _mm_movemask_epi8(cmp);
						c									 += _popcnt32(mask);
					}
					if(p < __len)
					{
						m128i64_t seq	 = _mm_set1_epi8(-128);
						const size_t r = __len - p;
						memcpy(&seq, &__str[p], r);
						const m128i64_t cmp = ~_mm_cmpeq_epi8(_mm_and_si128(seq, _mm_set1_epi8(-64)), _mm_set1_epi8(-128));
						const int32_t mask	= _mm_movemask_epi8(cmp);
						c									 += _popcnt32(mask);
					}
					return c;
				}
			}

			/// 64
			for(; p < __len; ++p)
			{
				if((__str[p] & 0b11000000) != 0b10000000) ++c;
			}
			return c;
		}

		/// UTF8: Next
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr size_t
		next(
		 const u8char_t *__val) pf_attr_noexcept
		{
			return code_len(__val);
		}
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr size_t
		next(
		 const u8char_t *__val,
		 size_t __len,
		 size_t __i) pf_attr_noexcept
		{
			pf_assert(__i < __len, "__i={} is greater or equal to __len={}!", __i, __len);
			size_t c = 0;
			size_t p = 0;
			if(!std::is_constant_evaluated())
			{
				/// NOTE: 512

				/// 256
				if(instruction_set::simd & instruction_set::AVX2_BIT)
				{
					for(; p + 32 <= __len; p += 32)
					{
						const m256i64_t seq = _mm256_lddqu_si256(union_cast<const m256i64_t *>(&__val[p]));
						const m256i64_t cmp = ~_mm256_cmpeq_epi8(_mm256_and_si256(seq, _mm256_set1_epi8(-64)), _mm256_set1_epi8(-128));
						const int32_t mask	= _mm256_movemask_epi8(cmp);
						const size_t k			= _popcnt32(mask);
						if(c + k >= __i)
						{
							for(size_t j = 0; c != __i; ++j, ++p)
							{
								if((mask >> j) & 0b1) ++c;
							}
							return p;
						}
						else
						{
							c += k;
						}
					}
					if(p < __len)
					{
						m256i64_t seq	 = _mm256_set1_epi8(-128);
						const size_t r = __len - p;
						memcpy(&seq, &__val[p], r);
						const m256i64_t cmp = ~_mm256_cmpeq_epi8(_mm256_and_si256(seq, _mm256_set1_epi8(-64)), _mm256_set1_epi8(-128));
						const int32_t mask	= _mm256_movemask_epi8(cmp);
						const size_t k			= _popcnt32(mask);
						if(c + k >= __i)
						{
							for(size_t j = 0; c != __i; ++j, ++p)
							{
								if((mask >> j) & 0b1) ++c;
							}
							return p;
						}
						else
						{
							c += k;
						}
					}
					return p;
				}

				/// 128
				if(instruction_set::simd & instruction_set::SSE2_BIT)
				{
					for(; p + 16 <= __len; p += 16)
					{
						const m128i64_t seq = _mm_lddqu_si128(union_cast<const m128i64_t *>(&__val[p]));
						const m128i64_t cmp = ~_mm_cmpeq_epi8(_mm_and_si128(seq, _mm_set1_epi8(-64)), _mm_set1_epi8(-128));
						const int32_t mask	= _mm_movemask_epi8(cmp);
						const size_t k			= _popcnt32(mask);
						if(c + k >= __i)
						{
							for(size_t j = 0; c != __i; ++j, ++p)
							{
								if((mask >> j) & 0b1) ++c;
							}
							return p;
						}
						else
						{
							c += k;
						}
					}
					if(p < __len)
					{
						m128i64_t seq	 = _mm_set1_epi8(-128);
						const size_t r = __len - p;
						memcpy(&seq, &__val[p], r);
						const m128i64_t cmp = ~_mm_cmpeq_epi8(_mm_and_si128(seq, _mm_set1_epi8(-64)), _mm_set1_epi8(-128));
						const int32_t mask	= _mm_movemask_epi8(cmp);
						const size_t k			= _popcnt32(mask);
						if(c + k >= __i)
						{
							for(size_t j = 0; c != __i; ++j, ++p)
							{
								if((mask >> j) & 0b1) ++c;
							}
							return p;
						}
						else
						{
							c += k;
						}
					}
					return p;
				}
			}

			/// 64
			for(; __i > 0; --__i)
			{
				p += next(__val);
			}
			return p;
		}

		/// UTF8: Prev
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr size_t
		prev(
		 const u8char_t *__val) pf_attr_noexcept
		{
			size_t p = 1;
			while((uint8_t(__val[-p]) & 0b11000000) == 0b10000000) ++p;
			return p;
		}
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr size_t
		prev(
		 const u8char_t *__val,
		 size_t __len,
		 size_t __i) pf_attr_noexcept
		{
			pf_assert(__i <= __len, "__i={} is greater than __len={}!", __i, __len);
			size_t c = 0;
			size_t p = 0;
			if(!std::is_constant_evaluated())
			{
				/// NOTE: 512

				/// 256
				if(instruction_set::simd & instruction_set::AVX2_BIT)
				{
					for(; p + 32 <= __len; p += 32)
					{
						const m256i64_t seq = _mm256_lddqu_si256(union_cast<const m256i64_t *>(&__val[-p - 32]));
						const m256i64_t cmp = ~_mm256_cmpeq_epi8(_mm256_and_si256(seq, _mm256_set1_epi8(-64)), _mm256_set1_epi8(-128));
						const int32_t mask	= _mm256_movemask_epi8(cmp);
						const size_t k			= _popcnt32(mask);
						if(c + k >= __i)
						{
							for(size_t j = 0; c != __i; ++j, ++p)
							{
								if((mask >> (32 - 1 - j)) & 0b1) ++c;
							}
							return p;
						}
						else
						{
							c += k;
						}
					}
					if(p < __len)
					{
						m256i64_t seq	 = _mm256_set1_epi8(-128);
						const size_t r = __len - p;
						memcpy(&seq, &__val[-p - r], r);
						const m256i64_t cmp = ~_mm256_cmpeq_epi8(_mm256_and_si256(seq, _mm256_set1_epi8(-64)), _mm256_set1_epi8(-128));
						const int32_t mask	= _mm256_movemask_epi8(cmp);
						const size_t k			= _popcnt32(mask);
						if(c + k >= __i)
						{
							for(size_t j = 0; c != __i; ++j, ++p)
							{
								if((mask >> (r - 1 - j)) & 0b1) ++c;
							}
							return p;
						}
						else
						{
							c += k;
						}
					}
					return p;
				}

				/// 128
				if(instruction_set::simd & instruction_set::SSE2_BIT)
				{
					for(; p + 16 <= __len; p += 16)
					{
						const m128i64_t seq = _mm_lddqu_si128(union_cast<const m128i64_t *>(&__val[-p - 16]));
						const m128i64_t cmp = ~_mm_cmpeq_epi8(_mm_and_si128(seq, _mm_set1_epi8(-64)), _mm_set1_epi8(-128));
						const int32_t mask	= _mm_movemask_epi8(cmp);
						const size_t k			= _popcnt32(mask);
						if(c + k >= __i)
						{
							for(size_t j = 0; c != __i; ++j, ++p)
							{
								if((mask >> (16 - 1 - j)) & 0b1) ++c;
							}
							return p;
						}
						else
						{
							c += k;
						}
					}
					if(p < __len)
					{
						m128i64_t seq	 = _mm_set1_epi8(-128);
						const size_t r = __len - p;
						memcpy(&seq, &__val[-p - r], r);
						const m128i64_t cmp = ~_mm_cmpeq_epi8(_mm_and_si128(seq, _mm_set1_epi8(-64)), _mm_set1_epi8(-128));
						const int32_t mask	= _mm_movemask_epi8(cmp);
						const size_t k			= _popcnt32(mask);
						if(c + k >= __i)
						{
							for(size_t j = 0; c != __i; ++j, ++p)
							{
								if((mask >> (r - 1 - j)) & 0b1) ++c;
							}
							return p;
						}
						else
						{
							c += k;
						}
					}
					return p;
				}
			}

			/// 64
			for(; __i > 0; --__i)
			{
				p += prev(__val);
			}
			return p;
		}

		/// UTF8: Compare
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr diff_t
		compare(
		 const u8char_t *__str1,
		 const u8char_t *__str2,
		 size_t __len) pf_attr_noexcept
		{
			return strncmp(union_cast<const char_t *>(__str1), union_cast<const char_t *>(__str2), __len);
		}

		/// UTF8: Is*
		// https://github.com/sheredom/utf8.h
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr bool
		is_ascii(
		 u8code_t __c) pf_attr_noexcept
		{
			return __c <= 127;
		}
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr bool
		is_cntrl(
		 u8code_t __c) pf_attr_noexcept
		{
			return __c <= 0x00'1F
					|| (__c >= 0x00'7F && __c <= 0x00'9F);
		}
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr bool
		is_space(
		 u8code_t __c) pf_attr_noexcept
		{
			return (__c >= 0x00'09 && __c <= 0x00'0D)
					|| __c == 0x00'20
					|| __c == 0x00'85
					|| __c == 0xC2'A0
					|| __c == 0x16'80
					|| (__c >= 0x20'00 && __c <= 0x20'0A)
					|| (__c >= 0x20'28 && __c <= 0x20'29)
					|| __c == 0x20'2F
					|| __c == 0x20'5F
					|| __c == 0x30'00;
		}
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr bool
		is_blank(
		 u8code_t __c) pf_attr_noexcept
		{
			return __c == 0x00'09
					|| __c == 0x00'20
					|| __c == 0x00'A0
					|| __c == 0x16'80
					|| (__c >= 0x20'00 && __c <= 0x20'0A)
					|| __c == 0x20'2F
					|| __c == 0x20'5F
					|| __c == 0x30'00;
		}
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr bool
		is_graph(
		 u8code_t __c) pf_attr_noexcept
		{
			return (__c >= 0x00'21 && __c <= 0x00'7E)
					|| (__c >= 0x00'A1 && __c <= 0x00'AC)
					|| (__c >= 0x00'AE && __c <= 0x10'03'FF);
		}
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr bool
		is_punct(
		 u8code_t __c) pf_attr_noexcept
		{
			return (__c >= 0x00'21 && __c <= 0x00'2F)
					|| (__c >= 0x00'3A && __c <= 0x00'3B)
					|| __c == 0x00'3F
					|| __c == 0x00'40
					|| (__c >= 0x00'5B && __c <= 0x00'5D)
					|| __c == 0x00'5F
					|| __c == 0x00'7B
					|| __c == 0x00'7D
					|| __c == 0x00'A1
					|| __c == 0x00'A7
					|| __c == 0x00'AB
					|| __c == 0x00'BB
					|| __c == 0x00'B6
					|| __c == 0x00'B7
					|| __c == 0x00'BF
					|| __c == 0x03'7E
					|| __c == 0x03'87
					|| (__c >= 0x05'5A && __c <= 0x05'5F)
					|| (__c >= 0x05'89 && __c <= 0x05'8A)
					|| __c == 0x05'BE
					|| __c == 0x05'C0
					|| __c == 0x05'C3
					|| __c == 0x05'C6
					|| (__c >= 0x05'F3 && __c <= 0x05'F4)
					|| (__c >= 0x06'09 && __c <= 0x06'0A)
					|| __c == 0x06'0C
					|| __c == 0x06'0D
					|| __c == 0x06'1B
					|| __c == 0x06'1E
					|| __c == 0x06'1F
					|| (__c >= 0x06'6A && __c <= 0x06'6D)
					|| __c == 0x06'D4
					|| (__c >= 0x07'00 && __c <= 0x07'0D)
					|| (__c >= 0x07'F7 && __c <= 0x07'F9)
					|| (__c >= 0x08'30 && __c <= 0x08'3E)
					|| __c == 0x08'5E
					|| (__c >= 0x09'64 && __c <= 0x09'65)
					|| __c == 0x09'70
					|| __c == 0x09'FD
					|| __c == 0x0A'76
					|| __c == 0x0A'F0
					|| __c == 0x0C'77
					|| __c == 0x0C'84
					|| __c == 0x0D'F4
					|| __c == 0x0E'4F
					|| __c == 0x0E'5A
					|| __c == 0x0E'5B
					|| (__c >= 0x0F'04 && __c <= 0x0F'12)
					|| __c == 0x0F'14
					|| (__c >= 0x0F'3A && __c <= 0x0F'3D)
					|| __c == 0x0F'85
					|| (__c >= 0x0F'D0 && __c <= 0x0F'DA)
					|| (__c >= 0x10'4A && __c <= 0x10'4F)
					|| __c == 0x10'FB
					|| (__c >= 0x13'60 && __c <= 0x13'68)
					|| __c == 0x14'00
					|| __c == 0x16'6E
					|| (__c >= 0x16'9B && __c <= 0x16'9C)
					|| (__c >= 0x16'EB && __c <= 0x16'ED)
					|| (__c >= 0x17'35 && __c <= 0x17'36)
					|| (__c >= 0x17'D4 && __c <= 0x17'D6)
					|| (__c >= 0x17'D8 && __c <= 0x17'DA)
					|| (__c >= 0x18'00 && __c <= 0x18'0A)
					|| (__c >= 0x19'44 && __c <= 0x19'45)
					|| (__c >= 0x1A'1E && __c <= 0x1A'1F)
					|| (__c >= 0x1A'A0 && __c <= 0x1A'A6)
					|| (__c >= 0x1A'A8 && __c <= 0x1A'AD)
					|| (__c >= 0x1B'5A && __c <= 0x1B'60)
					|| (__c >= 0x1B'FC && __c <= 0x1B'FF)
					|| (__c >= 0x1C'3B && __c <= 0x1C'3F)
					|| (__c >= 0x1C'7E && __c <= 0x1C'7F)
					|| (__c >= 0x1C'C0 && __c <= 0x1C'C7)
					|| __c == 0x1C'D3
					|| (__c >= 0x20'10 && __c <= 0x20'17)
					|| (__c >= 0x20'19 && __c <= 0x20'27)
					|| (__c >= 0x20'30 && __c <= 0x20'43)
					|| (__c >= 0x20'45 && __c <= 0x20'51)
					|| (__c >= 0x20'53 && __c <= 0x20'5E)
					|| (__c >= 0x20'7D && __c <= 0x20'7E)
					|| (__c >= 0x20'8D && __c <= 0x20'8E)
					|| (__c >= 0x23'08 && __c <= 0x23'0B)
					|| (__c >= 0x23'29 && __c <= 0x23'2A)
					|| (__c >= 0x27'68 && __c <= 0x27'6F)
					|| (__c >= 0x27'70 && __c <= 0x27'75)
					|| (__c >= 0x27'C5 && __c <= 0x27'C6)
					|| (__c >= 0x27'E6 && __c <= 0x27'EF)
					|| (__c >= 0x29'83 && __c <= 0x29'98)
					|| (__c >= 0x29'D8 && __c <= 0x29'DB)
					|| (__c >= 0x29'FC && __c <= 0x29'FD)
					|| (__c >= 0x2E'02 && __c <= 0x2E'05)
					|| (__c >= 0x2E'09 && __c <= 0x2E'0A)
					|| __c == 0x2E'0C
					|| __c == 0x2E'0D
					|| __c == 0x2E'1A
					|| __c == 0x2E'1C
					|| __c == 0x2E'1D
					|| __c == 0x2E'17
					|| (__c >= 0x2E'20 && __c <= 0x2E'29)
					|| (__c >= 0x2E'3A && __c <= 0x2E'3B)
					|| __c == 0x2E'40
					|| (__c >= 0x2C'F9 && __c <= 0x2C'FC)
					|| (__c >= 0x2C'FE && __c <= 0x2C'FF)
					|| __c == 0x2D'70
					|| (__c >= 0x2E'00 && __c <= 0x2E'01)
					|| (__c >= 0x2E'06 && __c <= 0x2E'08)
					|| __c == 0x2E'0B
					|| (__c >= 0x2E'0E && __c <= 0x2E'16)
					|| (__c >= 0x2E'18 && __c <= 0x2E'19)
					|| __c == 0x2E'1B
					|| (__c >= 0x2E'1E && __c <= 0x2E'1F)
					|| (__c >= 0x2E'2A && __c <= 0x2E'2E)
					|| (__c >= 0x2E'30 && __c <= 0x2E'39)
					|| (__c >= 0x2E'3C && __c <= 0x2E'3F)
					|| __c == 0x2E'41
					|| (__c >= 0x2E'43 && __c <= 0x2E'4F)
					|| __c == 0x2E'52
					|| (__c >= 0x30'01 && __c <= 0x30'03)
					|| (__c >= 0x30'08 && __c <= 0x30'11)
					|| (__c >= 0x30'14 && __c <= 0x30'1B)
					|| (__c >= 0x30'1D && __c <= 0x30'1F)
					|| __c == 0x30'30
					|| __c == 0x30'3D
					|| __c == 0x30'A0
					|| __c == 0x30'FB
					|| (__c >= 0xA4'FE && __c <= 0xA4'FF)
					|| (__c >= 0xA6'0D && __c <= 0xA6'0F)
					|| __c == 0xA6'73
					|| __c == 0xA6'7E
					|| (__c >= 0xA6'F2 && __c <= 0xA6'F7)
					|| (__c >= 0xA8'74 && __c <= 0xA8'77)
					|| (__c >= 0xA8'CE && __c <= 0xA8'CF)
					|| (__c >= 0xA8'F8 && __c <= 0xA8'FA)
					|| __c == 0xA8'FC
					|| (__c >= 0xA9'2E && __c <= 0xA9'2F)
					|| __c == 0xA9'5F
					|| (__c >= 0xA9'C1 && __c <= 0xA9'DF)
					|| (__c >= 0xAA'5C && __c <= 0xAA'5F)
					|| (__c >= 0xAA'DE && __c <= 0xAA'DF)
					|| (__c >= 0xAA'F0 && __c <= 0xAA'F1)
					|| __c == 0xAB'EB
					|| (__c >= 0xFD'3E && __c <= 0xFD'3F)
					|| (__c >= 0xFE'10 && __c <= 0xFE'19)
					|| (__c >= 0xFE'30 && __c <= 0xFE'52)
					|| (__c >= 0xFE'54 && __c <= 0xFE'61)
					|| __c == 0xFE'63
					|| __c == 0xFE'68
					|| __c == 0xFE'6A
					|| __c == 0xFE'6B
					|| (__c >= 0xFF'01 && __c <= 0xFF'03)
					|| (__c >= 0xFF'05 && __c <= 0xFF'0A)
					|| (__c >= 0xFF'0C && __c <= 0xFF'1B)
					|| __c == 0xFF'1F
					|| __c == 0xFF'20
					|| (__c >= 0xFF'3B && __c <= 0xFF'3D)
					|| __c == 0xFF'3F
					|| __c == 0xFF'5B
					|| __c == 0xFF'5D
					|| (__c >= 0xFF'5F && __c <= 0xFF'65)
					|| (__c >= 0x01'01'00 && __c <= 0x01'01'02)
					|| __c == 0x01'03'9F
					|| __c == 0x01'03'D0
					|| __c == 0x01'05'6F
					|| __c == 0x01'08'57
					|| __c == 0x01'09'1F
					|| __c == 0x01'09'3F
					|| (__c >= 0x01'0A'50 && __c <= 0x01'0A'58)
					|| __c == 0x01'0A'7F
					|| (__c >= 0x01'0A'F0 && __c <= 0x01'0A'F6)
					|| (__c >= 0x01'0B'39 && __c <= 0x01'0B'3F)
					|| (__c >= 0x01'0B'99 && __c <= 0x01'0B'9C)
					|| __c == 0x01'0E'AD
					|| (__c >= 0x01'0F'55 && __c <= 0x01'0F'59)
					|| (__c >= 0x01'10'47 && __c <= 0x01'10'4D)
					|| (__c >= 0x01'10'BB && __c <= 0x01'10'BC)
					|| (__c >= 0x01'10'BE && __c <= 0x01'10'BF)
					|| (__c >= 0x01'10'C0 && __c <= 0x01'10'C1)
					|| (__c >= 0x01'11'40 && __c <= 0x01'11'43)
					|| (__c >= 0x01'11'74 && __c <= 0x01'11'75)
					|| (__c >= 0x01'11'C5 && __c <= 0x01'11'C8)
					|| __c == 0x01'11'CD
					|| __c == 0x01'11'DB
					|| (__c >= 0x01'11'DD && __c <= 0x01'11'DF)
					|| (__c >= 0x01'12'38 && __c <= 0x01'12'3D)
					|| __c == 0x01'12'A9
					|| (__c >= 0x01'14'4B && __c <= 0x01'14'4F)
					|| (__c >= 0x01'14'5A && __c <= 0x01'14'5B)
					|| __c == 0x01'14'5D
					|| __c == 0x01'14'C6
					|| (__c >= 0x01'15'C1 && __c <= 0x01'15'D7)
					|| (__c >= 0x01'16'41 && __c <= 0x01'16'43)
					|| (__c >= 0x01'16'60 && __c <= 0x01'16'6C)
					|| (__c >= 0x01'17'3C && __c <= 0x01'17'3E)
					|| __c == 0x01'18'3B
					|| (__c >= 0x01'19'44 && __c <= 0x01'19'46)
					|| __c == 0x01'19'E2
					|| __c == 0x01'1A'3D
					|| (__c >= 0x01'1A'40 && __c <= 0x01'1A'46)
					|| (__c >= 0x01'1A'9A && __c <= 0x0A'A1'9C)
					|| (__c >= 0x01'1A'9E && __c <= 0x0A'A1'9F)
					|| (__c >= 0x01'1A'A0 && __c <= 0x01'1A'A2)
					|| (__c >= 0x0A'AC'41 && __c <= 0x0A'AC'45)
					|| (__c >= 0x0A'AC'70 && __c <= 0x0A'AC'71)
					|| (__c >= 0x01'1E'F7 && __c <= 0x01'1E'F8)
					|| __c == 0x01'1F'FF
					|| (__c >= 0x01'24'70 && __c <= 0x01'24'74)
					|| (__c >= 0x01'6A'6E && __c <= 0x01'6A'6F)
					|| __c == 0x01'6A'F5
					|| (__c >= 0x01'6B'37 && __c <= 0x01'6B'3B)
					|| __c == 0x01'6B'44
					|| (__c >= 0x01'6E'97 && __c <= 0x01'6E'9A)
					|| __c == 0x01'6F'E2
					|| __c == 0x01'BC'9F
					|| (__c >= 0x01'DA'87 && __c <= 0x01'DA'8B)
					|| (__c >= 0x01'E9'5E && __c <= 0x01'E9'5F);
		}
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr bool
		is_upper(
		 u8code_t __c) pf_attr_noexcept
		{
			return __c == 0x0'00'41
					|| __c == 0x0'00'42
					|| __c == 0x0'00'43
					|| __c == 0x0'00'44
					|| __c == 0x0'00'45
					|| __c == 0x0'00'46
					|| __c == 0x0'00'47
					|| __c == 0x0'00'48
					|| __c == 0x0'00'49
					|| __c == 0x0'00'4A
					|| __c == 0x0'00'4B
					|| __c == 0x0'00'4C
					|| __c == 0x0'00'4D
					|| __c == 0x0'00'4E
					|| __c == 0x0'00'4F
					|| __c == 0x0'00'50
					|| __c == 0x0'00'51
					|| __c == 0x0'00'52
					|| __c == 0x0'00'53
					|| __c == 0x0'00'54
					|| __c == 0x0'00'55
					|| __c == 0x0'00'56
					|| __c == 0x0'00'57
					|| __c == 0x0'00'58
					|| __c == 0x0'00'59
					|| __c == 0x0'00'5A
					|| __c == 0x0'00'B5
					|| __c == 0x0'00'C0
					|| __c == 0x0'00'C1
					|| __c == 0x0'00'C2
					|| __c == 0x0'00'C3
					|| __c == 0x0'00'C4
					|| __c == 0x0'00'C5
					|| __c == 0x0'00'C6
					|| __c == 0x0'00'C7
					|| __c == 0x0'00'C8
					|| __c == 0x0'00'C9
					|| __c == 0x0'00'CA
					|| __c == 0x0'00'CB
					|| __c == 0x0'00'CC
					|| __c == 0x0'00'CD
					|| __c == 0x0'00'CE
					|| __c == 0x0'00'CF
					|| __c == 0x0'00'D0
					|| __c == 0x0'00'D1
					|| __c == 0x0'00'D2
					|| __c == 0x0'00'D3
					|| __c == 0x0'00'D4
					|| __c == 0x0'00'D5
					|| __c == 0x0'00'D6
					|| __c == 0x0'00'D8
					|| __c == 0x0'00'D9
					|| __c == 0x0'00'DA
					|| __c == 0x0'00'DB
					|| __c == 0x0'00'DC
					|| __c == 0x0'00'DD
					|| __c == 0x0'00'DE
					|| __c == 0x0'01'00
					|| __c == 0x0'01'02
					|| __c == 0x0'01'04
					|| __c == 0x0'01'06
					|| __c == 0x0'01'08
					|| __c == 0x0'01'0A
					|| __c == 0x0'01'0C
					|| __c == 0x0'01'0E
					|| __c == 0x0'01'10
					|| __c == 0x0'01'12
					|| __c == 0x0'01'14
					|| __c == 0x0'01'16
					|| __c == 0x0'01'18
					|| __c == 0x0'01'1A
					|| __c == 0x0'01'1C
					|| __c == 0x0'01'1E
					|| __c == 0x0'01'20
					|| __c == 0x0'01'22
					|| __c == 0x0'01'24
					|| __c == 0x0'01'26
					|| __c == 0x0'01'28
					|| __c == 0x0'01'2A
					|| __c == 0x0'01'2C
					|| __c == 0x0'01'2E
					|| __c == 0x0'01'32
					|| __c == 0x0'01'34
					|| __c == 0x0'01'36
					|| __c == 0x0'01'39
					|| __c == 0x0'01'3B
					|| __c == 0x0'01'3D
					|| __c == 0x0'01'3F
					|| __c == 0x0'01'41
					|| __c == 0x0'01'43
					|| __c == 0x0'01'45
					|| __c == 0x0'01'47
					|| __c == 0x0'01'4A
					|| __c == 0x0'01'4C
					|| __c == 0x0'01'4E
					|| __c == 0x0'01'50
					|| __c == 0x0'01'52
					|| __c == 0x0'01'54
					|| __c == 0x0'01'56
					|| __c == 0x0'01'58
					|| __c == 0x0'01'5A
					|| __c == 0x0'01'5C
					|| __c == 0x0'01'5E
					|| __c == 0x0'01'60
					|| __c == 0x0'01'62
					|| __c == 0x0'01'64
					|| __c == 0x0'01'66
					|| __c == 0x0'01'68
					|| __c == 0x0'01'6A
					|| __c == 0x0'01'6C
					|| __c == 0x0'01'6E
					|| __c == 0x0'01'70
					|| __c == 0x0'01'72
					|| __c == 0x0'01'74
					|| __c == 0x0'01'76
					|| __c == 0x0'01'78
					|| __c == 0x0'01'79
					|| __c == 0x0'01'7B
					|| __c == 0x0'01'7D
					|| __c == 0x0'01'7F
					|| __c == 0x0'01'81
					|| __c == 0x0'01'82
					|| __c == 0x0'01'84
					|| __c == 0x0'01'86
					|| __c == 0x0'01'87
					|| __c == 0x0'01'89
					|| __c == 0x0'01'8A
					|| __c == 0x0'01'8B
					|| __c == 0x0'01'8E
					|| __c == 0x0'01'8F
					|| __c == 0x0'01'90
					|| __c == 0x0'01'91
					|| __c == 0x0'01'93
					|| __c == 0x0'01'94
					|| __c == 0x0'01'96
					|| __c == 0x0'01'97
					|| __c == 0x0'01'98
					|| __c == 0x0'01'9C
					|| __c == 0x0'01'9D
					|| __c == 0x0'01'9F
					|| __c == 0x0'01'A0
					|| __c == 0x0'01'A2
					|| __c == 0x0'01'A4
					|| __c == 0x0'01'A6
					|| __c == 0x0'01'A7
					|| __c == 0x0'01'A9
					|| __c == 0x0'01'AC
					|| __c == 0x0'01'AE
					|| __c == 0x0'01'AF
					|| __c == 0x0'01'B1
					|| __c == 0x0'01'B2
					|| __c == 0x0'01'B3
					|| __c == 0x0'01'B5
					|| __c == 0x0'01'B7
					|| __c == 0x0'01'B8
					|| __c == 0x0'01'BC
					|| __c == 0x0'01'C4
					|| __c == 0x0'01'C5
					|| __c == 0x0'01'C7
					|| __c == 0x0'01'C8
					|| __c == 0x0'01'CA
					|| __c == 0x0'01'CB
					|| __c == 0x0'01'CD
					|| __c == 0x0'01'CF
					|| __c == 0x0'01'D1
					|| __c == 0x0'01'D3
					|| __c == 0x0'01'D5
					|| __c == 0x0'01'D7
					|| __c == 0x0'01'D9
					|| __c == 0x0'01'DB
					|| __c == 0x0'01'DE
					|| __c == 0x0'01'E0
					|| __c == 0x0'01'E2
					|| __c == 0x0'01'E4
					|| __c == 0x0'01'E6
					|| __c == 0x0'01'E8
					|| __c == 0x0'01'EA
					|| __c == 0x0'01'EC
					|| __c == 0x0'01'EE
					|| __c == 0x0'01'F1
					|| __c == 0x0'01'F2
					|| __c == 0x0'01'F4
					|| __c == 0x0'01'F6
					|| __c == 0x0'01'F7
					|| __c == 0x0'01'F8
					|| __c == 0x0'01'FA
					|| __c == 0x0'01'FC
					|| __c == 0x0'01'FE
					|| __c == 0x0'02'00
					|| __c == 0x0'02'02
					|| __c == 0x0'02'04
					|| __c == 0x0'02'06
					|| __c == 0x0'02'08
					|| __c == 0x0'02'0A
					|| __c == 0x0'02'0C
					|| __c == 0x0'02'0E
					|| __c == 0x0'02'10
					|| __c == 0x0'02'12
					|| __c == 0x0'02'14
					|| __c == 0x0'02'16
					|| __c == 0x0'02'18
					|| __c == 0x0'02'1A
					|| __c == 0x0'02'1C
					|| __c == 0x0'02'1E
					|| __c == 0x0'02'20
					|| __c == 0x0'02'22
					|| __c == 0x0'02'24
					|| __c == 0x0'02'26
					|| __c == 0x0'02'28
					|| __c == 0x0'02'2A
					|| __c == 0x0'02'2C
					|| __c == 0x0'02'2E
					|| __c == 0x0'02'30
					|| __c == 0x0'02'32
					|| __c == 0x0'02'3A
					|| __c == 0x0'02'3B
					|| __c == 0x0'02'3D
					|| __c == 0x0'02'3E
					|| __c == 0x0'02'41
					|| __c == 0x0'02'43
					|| __c == 0x0'02'44
					|| __c == 0x0'02'45
					|| __c == 0x0'02'46
					|| __c == 0x0'02'48
					|| __c == 0x0'02'4A
					|| __c == 0x0'02'4C
					|| __c == 0x0'02'4E
					|| __c == 0x0'03'45
					|| __c == 0x0'03'70
					|| __c == 0x0'03'72
					|| __c == 0x0'03'76
					|| __c == 0x0'03'7F
					|| __c == 0x0'03'86
					|| __c == 0x0'03'88
					|| __c == 0x0'03'89
					|| __c == 0x0'03'8A
					|| __c == 0x0'03'8C
					|| __c == 0x0'03'8E
					|| __c == 0x0'03'8F
					|| __c == 0x0'03'91
					|| __c == 0x0'03'92
					|| __c == 0x0'03'93
					|| __c == 0x0'03'94
					|| __c == 0x0'03'95
					|| __c == 0x0'03'96
					|| __c == 0x0'03'97
					|| __c == 0x0'03'98
					|| __c == 0x0'03'99
					|| __c == 0x0'03'9A
					|| __c == 0x0'03'9B
					|| __c == 0x0'03'9C
					|| __c == 0x0'03'9D
					|| __c == 0x0'03'9E
					|| __c == 0x0'03'9F
					|| __c == 0x0'03'A0
					|| __c == 0x0'03'A1
					|| __c == 0x0'03'A3
					|| __c == 0x0'03'A4
					|| __c == 0x0'03'A5
					|| __c == 0x0'03'A6
					|| __c == 0x0'03'A7
					|| __c == 0x0'03'A8
					|| __c == 0x0'03'A9
					|| __c == 0x0'03'AA
					|| __c == 0x0'03'AB
					|| __c == 0x0'03'C2
					|| __c == 0x0'03'CF
					|| __c == 0x0'03'D0
					|| __c == 0x0'03'D1
					|| __c == 0x0'03'D5
					|| __c == 0x0'03'D6
					|| __c == 0x0'03'D8
					|| __c == 0x0'03'DA
					|| __c == 0x0'03'DC
					|| __c == 0x0'03'DE
					|| __c == 0x0'03'E0
					|| __c == 0x0'03'E2
					|| __c == 0x0'03'E4
					|| __c == 0x0'03'E6
					|| __c == 0x0'03'E8
					|| __c == 0x0'03'EA
					|| __c == 0x0'03'EC
					|| __c == 0x0'03'EE
					|| __c == 0x0'03'F0
					|| __c == 0x0'03'F1
					|| __c == 0x0'03'F4
					|| __c == 0x0'03'F5
					|| __c == 0x0'03'F7
					|| __c == 0x0'03'F9
					|| __c == 0x0'03'FA
					|| __c == 0x0'03'FD
					|| __c == 0x0'03'FE
					|| __c == 0x0'03'FF
					|| __c == 0x0'04'00
					|| __c == 0x0'04'01
					|| __c == 0x0'04'02
					|| __c == 0x0'04'03
					|| __c == 0x0'04'04
					|| __c == 0x0'04'05
					|| __c == 0x0'04'06
					|| __c == 0x0'04'07
					|| __c == 0x0'04'08
					|| __c == 0x0'04'09
					|| __c == 0x0'04'0A
					|| __c == 0x0'04'0B
					|| __c == 0x0'04'0C
					|| __c == 0x0'04'0D
					|| __c == 0x0'04'0E
					|| __c == 0x0'04'0F
					|| __c == 0x0'04'10
					|| __c == 0x0'04'11
					|| __c == 0x0'04'12
					|| __c == 0x0'04'13
					|| __c == 0x0'04'14
					|| __c == 0x0'04'15
					|| __c == 0x0'04'16
					|| __c == 0x0'04'17
					|| __c == 0x0'04'18
					|| __c == 0x0'04'19
					|| __c == 0x0'04'1A
					|| __c == 0x0'04'1B
					|| __c == 0x0'04'1C
					|| __c == 0x0'04'1D
					|| __c == 0x0'04'1E
					|| __c == 0x0'04'1F
					|| __c == 0x0'04'20
					|| __c == 0x0'04'21
					|| __c == 0x0'04'22
					|| __c == 0x0'04'23
					|| __c == 0x0'04'24
					|| __c == 0x0'04'25
					|| __c == 0x0'04'26
					|| __c == 0x0'04'27
					|| __c == 0x0'04'28
					|| __c == 0x0'04'29
					|| __c == 0x0'04'2A
					|| __c == 0x0'04'2B
					|| __c == 0x0'04'2C
					|| __c == 0x0'04'2D
					|| __c == 0x0'04'2E
					|| __c == 0x0'04'2F
					|| __c == 0x0'04'60
					|| __c == 0x0'04'62
					|| __c == 0x0'04'64
					|| __c == 0x0'04'66
					|| __c == 0x0'04'68
					|| __c == 0x0'04'6A
					|| __c == 0x0'04'6C
					|| __c == 0x0'04'6E
					|| __c == 0x0'04'70
					|| __c == 0x0'04'72
					|| __c == 0x0'04'74
					|| __c == 0x0'04'76
					|| __c == 0x0'04'78
					|| __c == 0x0'04'7A
					|| __c == 0x0'04'7C
					|| __c == 0x0'04'7E
					|| __c == 0x0'04'80
					|| __c == 0x0'04'8A
					|| __c == 0x0'04'8C
					|| __c == 0x0'04'8E
					|| __c == 0x0'04'90
					|| __c == 0x0'04'92
					|| __c == 0x0'04'94
					|| __c == 0x0'04'96
					|| __c == 0x0'04'98
					|| __c == 0x0'04'9A
					|| __c == 0x0'04'9C
					|| __c == 0x0'04'9E
					|| __c == 0x0'04'A0
					|| __c == 0x0'04'A2
					|| __c == 0x0'04'A4
					|| __c == 0x0'04'A6
					|| __c == 0x0'04'A8
					|| __c == 0x0'04'AA
					|| __c == 0x0'04'AC
					|| __c == 0x0'04'AE
					|| __c == 0x0'04'B0
					|| __c == 0x0'04'B2
					|| __c == 0x0'04'B4
					|| __c == 0x0'04'B6
					|| __c == 0x0'04'B8
					|| __c == 0x0'04'BA
					|| __c == 0x0'04'BC
					|| __c == 0x0'04'BE
					|| __c == 0x0'04'C0
					|| __c == 0x0'04'C1
					|| __c == 0x0'04'C3
					|| __c == 0x0'04'C5
					|| __c == 0x0'04'C7
					|| __c == 0x0'04'C9
					|| __c == 0x0'04'CB
					|| __c == 0x0'04'CD
					|| __c == 0x0'04'D0
					|| __c == 0x0'04'D2
					|| __c == 0x0'04'D4
					|| __c == 0x0'04'D6
					|| __c == 0x0'04'D8
					|| __c == 0x0'04'DA
					|| __c == 0x0'04'DC
					|| __c == 0x0'04'DE
					|| __c == 0x0'04'E0
					|| __c == 0x0'04'E2
					|| __c == 0x0'04'E4
					|| __c == 0x0'04'E6
					|| __c == 0x0'04'E8
					|| __c == 0x0'04'EA
					|| __c == 0x0'04'EC
					|| __c == 0x0'04'EE
					|| __c == 0x0'04'F0
					|| __c == 0x0'04'F2
					|| __c == 0x0'04'F4
					|| __c == 0x0'04'F6
					|| __c == 0x0'04'F8
					|| __c == 0x0'04'FA
					|| __c == 0x0'04'FC
					|| __c == 0x0'04'FE
					|| __c == 0x0'05'00
					|| __c == 0x0'05'02
					|| __c == 0x0'05'04
					|| __c == 0x0'05'06
					|| __c == 0x0'05'08
					|| __c == 0x0'05'0A
					|| __c == 0x0'05'0C
					|| __c == 0x0'05'0E
					|| __c == 0x0'05'10
					|| __c == 0x0'05'12
					|| __c == 0x0'05'14
					|| __c == 0x0'05'16
					|| __c == 0x0'05'18
					|| __c == 0x0'05'1A
					|| __c == 0x0'05'1C
					|| __c == 0x0'05'1E
					|| __c == 0x0'05'20
					|| __c == 0x0'05'22
					|| __c == 0x0'05'24
					|| __c == 0x0'05'26
					|| __c == 0x0'05'28
					|| __c == 0x0'05'2A
					|| __c == 0x0'05'2C
					|| __c == 0x0'05'2E
					|| __c == 0x0'05'31
					|| __c == 0x0'05'32
					|| __c == 0x0'05'33
					|| __c == 0x0'05'34
					|| __c == 0x0'05'35
					|| __c == 0x0'05'36
					|| __c == 0x0'05'37
					|| __c == 0x0'05'38
					|| __c == 0x0'05'39
					|| __c == 0x0'05'3A
					|| __c == 0x0'05'3B
					|| __c == 0x0'05'3C
					|| __c == 0x0'05'3D
					|| __c == 0x0'05'3E
					|| __c == 0x0'05'3F
					|| __c == 0x0'05'40
					|| __c == 0x0'05'41
					|| __c == 0x0'05'42
					|| __c == 0x0'05'43
					|| __c == 0x0'05'44
					|| __c == 0x0'05'45
					|| __c == 0x0'05'46
					|| __c == 0x0'05'47
					|| __c == 0x0'05'48
					|| __c == 0x0'05'49
					|| __c == 0x0'05'4A
					|| __c == 0x0'05'4B
					|| __c == 0x0'05'4C
					|| __c == 0x0'05'4D
					|| __c == 0x0'05'4E
					|| __c == 0x0'05'4F
					|| __c == 0x0'05'50
					|| __c == 0x0'05'51
					|| __c == 0x0'05'52
					|| __c == 0x0'05'53
					|| __c == 0x0'05'54
					|| __c == 0x0'05'55
					|| __c == 0x0'05'56
					|| __c == 0x0'10'A0
					|| __c == 0x0'10'A1
					|| __c == 0x0'10'A2
					|| __c == 0x0'10'A3
					|| __c == 0x0'10'A4
					|| __c == 0x0'10'A5
					|| __c == 0x0'10'A6
					|| __c == 0x0'10'A7
					|| __c == 0x0'10'A8
					|| __c == 0x0'10'A9
					|| __c == 0x0'10'AA
					|| __c == 0x0'10'AB
					|| __c == 0x0'10'AC
					|| __c == 0x0'10'AD
					|| __c == 0x0'10'AE
					|| __c == 0x0'10'AF
					|| __c == 0x0'10'B0
					|| __c == 0x0'10'B1
					|| __c == 0x0'10'B2
					|| __c == 0x0'10'B3
					|| __c == 0x0'10'B4
					|| __c == 0x0'10'B5
					|| __c == 0x0'10'B6
					|| __c == 0x0'10'B7
					|| __c == 0x0'10'B8
					|| __c == 0x0'10'B9
					|| __c == 0x0'10'BA
					|| __c == 0x0'10'BB
					|| __c == 0x0'10'BC
					|| __c == 0x0'10'BD
					|| __c == 0x0'10'BE
					|| __c == 0x0'10'BF
					|| __c == 0x0'10'C0
					|| __c == 0x0'10'C1
					|| __c == 0x0'10'C2
					|| __c == 0x0'10'C3
					|| __c == 0x0'10'C4
					|| __c == 0x0'10'C5
					|| __c == 0x0'10'C7
					|| __c == 0x0'10'CD
					|| __c == 0x0'13'F8
					|| __c == 0x0'13'F9
					|| __c == 0x0'13'FA
					|| __c == 0x0'13'FB
					|| __c == 0x0'13'FC
					|| __c == 0x0'13'FD
					|| __c == 0x0'1C'80
					|| __c == 0x0'1C'81
					|| __c == 0x0'1C'82
					|| __c == 0x0'1C'83
					|| __c == 0x0'1C'84
					|| __c == 0x0'1C'85
					|| __c == 0x0'1C'86
					|| __c == 0x0'1C'87
					|| __c == 0x0'1C'88
					|| __c == 0x0'1C'90
					|| __c == 0x0'1C'91
					|| __c == 0x0'1C'92
					|| __c == 0x0'1C'93
					|| __c == 0x0'1C'94
					|| __c == 0x0'1C'95
					|| __c == 0x0'1C'96
					|| __c == 0x0'1C'97
					|| __c == 0x0'1C'98
					|| __c == 0x0'1C'99
					|| __c == 0x0'1C'9A
					|| __c == 0x0'1C'9B
					|| __c == 0x0'1C'9C
					|| __c == 0x0'1C'9D
					|| __c == 0x0'1C'9E
					|| __c == 0x0'1C'9F
					|| __c == 0x0'1C'A0
					|| __c == 0x0'1C'A1
					|| __c == 0x0'1C'A2
					|| __c == 0x0'1C'A3
					|| __c == 0x0'1C'A4
					|| __c == 0x0'1C'A5
					|| __c == 0x0'1C'A6
					|| __c == 0x0'1C'A7
					|| __c == 0x0'1C'A8
					|| __c == 0x0'1C'A9
					|| __c == 0x0'1C'AA
					|| __c == 0x0'1C'AB
					|| __c == 0x0'1C'AC
					|| __c == 0x0'1C'AD
					|| __c == 0x0'1C'AE
					|| __c == 0x0'1C'AF
					|| __c == 0x0'1C'B0
					|| __c == 0x0'1C'B1
					|| __c == 0x0'1C'B2
					|| __c == 0x0'1C'B3
					|| __c == 0x0'1C'B4
					|| __c == 0x0'1C'B5
					|| __c == 0x0'1C'B6
					|| __c == 0x0'1C'B7
					|| __c == 0x0'1C'B8
					|| __c == 0x0'1C'B9
					|| __c == 0x0'1C'BA
					|| __c == 0x0'1C'BD
					|| __c == 0x0'1C'BE
					|| __c == 0x0'1C'BF
					|| __c == 0x0'1E'00
					|| __c == 0x0'1E'02
					|| __c == 0x0'1E'04
					|| __c == 0x0'1E'06
					|| __c == 0x0'1E'08
					|| __c == 0x0'1E'0A
					|| __c == 0x0'1E'0C
					|| __c == 0x0'1E'0E
					|| __c == 0x0'1E'10
					|| __c == 0x0'1E'12
					|| __c == 0x0'1E'14
					|| __c == 0x0'1E'16
					|| __c == 0x0'1E'18
					|| __c == 0x0'1E'1A
					|| __c == 0x0'1E'1C
					|| __c == 0x0'1E'1E
					|| __c == 0x0'1E'20
					|| __c == 0x0'1E'22
					|| __c == 0x0'1E'24
					|| __c == 0x0'1E'26
					|| __c == 0x0'1E'28
					|| __c == 0x0'1E'2A
					|| __c == 0x0'1E'2C
					|| __c == 0x0'1E'2E
					|| __c == 0x0'1E'30
					|| __c == 0x0'1E'32
					|| __c == 0x0'1E'34
					|| __c == 0x0'1E'36
					|| __c == 0x0'1E'38
					|| __c == 0x0'1E'3A
					|| __c == 0x0'1E'3C
					|| __c == 0x0'1E'3E
					|| __c == 0x0'1E'40
					|| __c == 0x0'1E'42
					|| __c == 0x0'1E'44
					|| __c == 0x0'1E'46
					|| __c == 0x0'1E'48
					|| __c == 0x0'1E'4A
					|| __c == 0x0'1E'4C
					|| __c == 0x0'1E'4E
					|| __c == 0x0'1E'50
					|| __c == 0x0'1E'52
					|| __c == 0x0'1E'54
					|| __c == 0x0'1E'56
					|| __c == 0x0'1E'58
					|| __c == 0x0'1E'5A
					|| __c == 0x0'1E'5C
					|| __c == 0x0'1E'5E
					|| __c == 0x0'1E'60
					|| __c == 0x0'1E'62
					|| __c == 0x0'1E'64
					|| __c == 0x0'1E'66
					|| __c == 0x0'1E'68
					|| __c == 0x0'1E'6A
					|| __c == 0x0'1E'6C
					|| __c == 0x0'1E'6E
					|| __c == 0x0'1E'70
					|| __c == 0x0'1E'72
					|| __c == 0x0'1E'74
					|| __c == 0x0'1E'76
					|| __c == 0x0'1E'78
					|| __c == 0x0'1E'7A
					|| __c == 0x0'1E'7C
					|| __c == 0x0'1E'7E
					|| __c == 0x0'1E'80
					|| __c == 0x0'1E'82
					|| __c == 0x0'1E'84
					|| __c == 0x0'1E'86
					|| __c == 0x0'1E'88
					|| __c == 0x0'1E'8A
					|| __c == 0x0'1E'8C
					|| __c == 0x0'1E'8E
					|| __c == 0x0'1E'90
					|| __c == 0x0'1E'92
					|| __c == 0x0'1E'94
					|| __c == 0x0'1E'9B
					|| __c == 0x0'1E'9E
					|| __c == 0x0'1E'A0
					|| __c == 0x0'1E'A2
					|| __c == 0x0'1E'A4
					|| __c == 0x0'1E'A6
					|| __c == 0x0'1E'A8
					|| __c == 0x0'1E'AA
					|| __c == 0x0'1E'AC
					|| __c == 0x0'1E'AE
					|| __c == 0x0'1E'B0
					|| __c == 0x0'1E'B2
					|| __c == 0x0'1E'B4
					|| __c == 0x0'1E'B6
					|| __c == 0x0'1E'B8
					|| __c == 0x0'1E'BA
					|| __c == 0x0'1E'BC
					|| __c == 0x0'1E'BE
					|| __c == 0x0'1E'C0
					|| __c == 0x0'1E'C2
					|| __c == 0x0'1E'C4
					|| __c == 0x0'1E'C6
					|| __c == 0x0'1E'C8
					|| __c == 0x0'1E'CA
					|| __c == 0x0'1E'CC
					|| __c == 0x0'1E'CE
					|| __c == 0x0'1E'D0
					|| __c == 0x0'1E'D2
					|| __c == 0x0'1E'D4
					|| __c == 0x0'1E'D6
					|| __c == 0x0'1E'D8
					|| __c == 0x0'1E'DA
					|| __c == 0x0'1E'DC
					|| __c == 0x0'1E'DE
					|| __c == 0x0'1E'E0
					|| __c == 0x0'1E'E2
					|| __c == 0x0'1E'E4
					|| __c == 0x0'1E'E6
					|| __c == 0x0'1E'E8
					|| __c == 0x0'1E'EA
					|| __c == 0x0'1E'EC
					|| __c == 0x0'1E'EE
					|| __c == 0x0'1E'F0
					|| __c == 0x0'1E'F2
					|| __c == 0x0'1E'F4
					|| __c == 0x0'1E'F6
					|| __c == 0x0'1E'F8
					|| __c == 0x0'1E'FA
					|| __c == 0x0'1E'FC
					|| __c == 0x0'1E'FE
					|| __c == 0x0'1F'08
					|| __c == 0x0'1F'09
					|| __c == 0x0'1F'0A
					|| __c == 0x0'1F'0B
					|| __c == 0x0'1F'0C
					|| __c == 0x0'1F'0D
					|| __c == 0x0'1F'0E
					|| __c == 0x0'1F'0F
					|| __c == 0x0'1F'18
					|| __c == 0x0'1F'19
					|| __c == 0x0'1F'1A
					|| __c == 0x0'1F'1B
					|| __c == 0x0'1F'1C
					|| __c == 0x0'1F'1D
					|| __c == 0x0'1F'28
					|| __c == 0x0'1F'29
					|| __c == 0x0'1F'2A
					|| __c == 0x0'1F'2B
					|| __c == 0x0'1F'2C
					|| __c == 0x0'1F'2D
					|| __c == 0x0'1F'2E
					|| __c == 0x0'1F'2F
					|| __c == 0x0'1F'38
					|| __c == 0x0'1F'39
					|| __c == 0x0'1F'3A
					|| __c == 0x0'1F'3B
					|| __c == 0x0'1F'3C
					|| __c == 0x0'1F'3D
					|| __c == 0x0'1F'3E
					|| __c == 0x0'1F'3F
					|| __c == 0x0'1F'48
					|| __c == 0x0'1F'49
					|| __c == 0x0'1F'4A
					|| __c == 0x0'1F'4B
					|| __c == 0x0'1F'4C
					|| __c == 0x0'1F'4D
					|| __c == 0x0'1F'59
					|| __c == 0x0'1F'5B
					|| __c == 0x0'1F'5D
					|| __c == 0x0'1F'5F
					|| __c == 0x0'1F'68
					|| __c == 0x0'1F'69
					|| __c == 0x0'1F'6A
					|| __c == 0x0'1F'6B
					|| __c == 0x0'1F'6C
					|| __c == 0x0'1F'6D
					|| __c == 0x0'1F'6E
					|| __c == 0x0'1F'6F
					|| __c == 0x0'1F'88
					|| __c == 0x0'1F'89
					|| __c == 0x0'1F'8A
					|| __c == 0x0'1F'8B
					|| __c == 0x0'1F'8C
					|| __c == 0x0'1F'8D
					|| __c == 0x0'1F'8E
					|| __c == 0x0'1F'8F
					|| __c == 0x0'1F'98
					|| __c == 0x0'1F'99
					|| __c == 0x0'1F'9A
					|| __c == 0x0'1F'9B
					|| __c == 0x0'1F'9C
					|| __c == 0x0'1F'9D
					|| __c == 0x0'1F'9E
					|| __c == 0x0'1F'9F
					|| __c == 0x0'1F'A8
					|| __c == 0x0'1F'A9
					|| __c == 0x0'1F'AA
					|| __c == 0x0'1F'AB
					|| __c == 0x0'1F'AC
					|| __c == 0x0'1F'AD
					|| __c == 0x0'1F'AE
					|| __c == 0x0'1F'AF
					|| __c == 0x0'1F'B8
					|| __c == 0x0'1F'B9
					|| __c == 0x0'1F'BA
					|| __c == 0x0'1F'BB
					|| __c == 0x0'1F'BC
					|| __c == 0x0'1F'BE
					|| __c == 0x0'1F'C8
					|| __c == 0x0'1F'C9
					|| __c == 0x0'1F'CA
					|| __c == 0x0'1F'CB
					|| __c == 0x0'1F'CC
					|| __c == 0x0'1F'D8
					|| __c == 0x0'1F'D9
					|| __c == 0x0'1F'DA
					|| __c == 0x0'1F'DB
					|| __c == 0x0'1F'E8
					|| __c == 0x0'1F'E9
					|| __c == 0x0'1F'EA
					|| __c == 0x0'1F'EB
					|| __c == 0x0'1F'EC
					|| __c == 0x0'1F'F8
					|| __c == 0x0'1F'F9
					|| __c == 0x0'1F'FA
					|| __c == 0x0'1F'FB
					|| __c == 0x0'1F'FC
					|| __c == 0x0'21'26
					|| __c == 0x0'21'2A
					|| __c == 0x0'21'2B
					|| __c == 0x0'21'32
					|| __c == 0x0'21'60
					|| __c == 0x0'21'61
					|| __c == 0x0'21'62
					|| __c == 0x0'21'63
					|| __c == 0x0'21'64
					|| __c == 0x0'21'65
					|| __c == 0x0'21'66
					|| __c == 0x0'21'67
					|| __c == 0x0'21'68
					|| __c == 0x0'21'69
					|| __c == 0x0'21'6A
					|| __c == 0x0'21'6B
					|| __c == 0x0'21'6C
					|| __c == 0x0'21'6D
					|| __c == 0x0'21'6E
					|| __c == 0x0'21'6F
					|| __c == 0x0'21'83
					|| __c == 0x0'24'B6
					|| __c == 0x0'24'B7
					|| __c == 0x0'24'B8
					|| __c == 0x0'24'B9
					|| __c == 0x0'24'BA
					|| __c == 0x0'24'BB
					|| __c == 0x0'24'BC
					|| __c == 0x0'24'BD
					|| __c == 0x0'24'BE
					|| __c == 0x0'24'BF
					|| __c == 0x0'24'C0
					|| __c == 0x0'24'C1
					|| __c == 0x0'24'C2
					|| __c == 0x0'24'C3
					|| __c == 0x0'24'C4
					|| __c == 0x0'24'C5
					|| __c == 0x0'24'C6
					|| __c == 0x0'24'C7
					|| __c == 0x0'24'C8
					|| __c == 0x0'24'C9
					|| __c == 0x0'24'CA
					|| __c == 0x0'24'CB
					|| __c == 0x0'24'CC
					|| __c == 0x0'24'CD
					|| __c == 0x0'24'CE
					|| __c == 0x0'24'CF
					|| __c == 0x0'2C'00
					|| __c == 0x0'2C'01
					|| __c == 0x0'2C'02
					|| __c == 0x0'2C'03
					|| __c == 0x0'2C'04
					|| __c == 0x0'2C'05
					|| __c == 0x0'2C'06
					|| __c == 0x0'2C'07
					|| __c == 0x0'2C'08
					|| __c == 0x0'2C'09
					|| __c == 0x0'2C'0A
					|| __c == 0x0'2C'0B
					|| __c == 0x0'2C'0C
					|| __c == 0x0'2C'0D
					|| __c == 0x0'2C'0E
					|| __c == 0x0'2C'0F
					|| __c == 0x0'2C'10
					|| __c == 0x0'2C'11
					|| __c == 0x0'2C'12
					|| __c == 0x0'2C'13
					|| __c == 0x0'2C'14
					|| __c == 0x0'2C'15
					|| __c == 0x0'2C'16
					|| __c == 0x0'2C'17
					|| __c == 0x0'2C'18
					|| __c == 0x0'2C'19
					|| __c == 0x0'2C'1A
					|| __c == 0x0'2C'1B
					|| __c == 0x0'2C'1C
					|| __c == 0x0'2C'1D
					|| __c == 0x0'2C'1E
					|| __c == 0x0'2C'1F
					|| __c == 0x0'2C'20
					|| __c == 0x0'2C'21
					|| __c == 0x0'2C'22
					|| __c == 0x0'2C'23
					|| __c == 0x0'2C'24
					|| __c == 0x0'2C'25
					|| __c == 0x0'2C'26
					|| __c == 0x0'2C'27
					|| __c == 0x0'2C'28
					|| __c == 0x0'2C'29
					|| __c == 0x0'2C'2A
					|| __c == 0x0'2C'2B
					|| __c == 0x0'2C'2C
					|| __c == 0x0'2C'2D
					|| __c == 0x0'2C'2E
					|| __c == 0x0'2C'60
					|| __c == 0x0'2C'62
					|| __c == 0x0'2C'63
					|| __c == 0x0'2C'64
					|| __c == 0x0'2C'67
					|| __c == 0x0'2C'69
					|| __c == 0x0'2C'6B
					|| __c == 0x0'2C'6D
					|| __c == 0x0'2C'6E
					|| __c == 0x0'2C'6F
					|| __c == 0x0'2C'70
					|| __c == 0x0'2C'72
					|| __c == 0x0'2C'75
					|| __c == 0x0'2C'7E
					|| __c == 0x0'2C'7F
					|| __c == 0x0'2C'80
					|| __c == 0x0'2C'82
					|| __c == 0x0'2C'84
					|| __c == 0x0'2C'86
					|| __c == 0x0'2C'88
					|| __c == 0x0'2C'8A
					|| __c == 0x0'2C'8C
					|| __c == 0x0'2C'8E
					|| __c == 0x0'2C'90
					|| __c == 0x0'2C'92
					|| __c == 0x0'2C'94
					|| __c == 0x0'2C'96
					|| __c == 0x0'2C'98
					|| __c == 0x0'2C'9A
					|| __c == 0x0'2C'9C
					|| __c == 0x0'2C'9E
					|| __c == 0x0'2C'A0
					|| __c == 0x0'2C'A2
					|| __c == 0x0'2C'A4
					|| __c == 0x0'2C'A6
					|| __c == 0x0'2C'A8
					|| __c == 0x0'2C'AA
					|| __c == 0x0'2C'AC
					|| __c == 0x0'2C'AE
					|| __c == 0x0'2C'B0
					|| __c == 0x0'2C'B2
					|| __c == 0x0'2C'B4
					|| __c == 0x0'2C'B6
					|| __c == 0x0'2C'B8
					|| __c == 0x0'2C'BA
					|| __c == 0x0'2C'BC
					|| __c == 0x0'2C'BE
					|| __c == 0x0'2C'C0
					|| __c == 0x0'2C'C2
					|| __c == 0x0'2C'C4
					|| __c == 0x0'2C'C6
					|| __c == 0x0'2C'C8
					|| __c == 0x0'2C'CA
					|| __c == 0x0'2C'CC
					|| __c == 0x0'2C'CE
					|| __c == 0x0'2C'D0
					|| __c == 0x0'2C'D2
					|| __c == 0x0'2C'D4
					|| __c == 0x0'2C'D6
					|| __c == 0x0'2C'D8
					|| __c == 0x0'2C'DA
					|| __c == 0x0'2C'DC
					|| __c == 0x0'2C'DE
					|| __c == 0x0'2C'E0
					|| __c == 0x0'2C'E2
					|| __c == 0x0'2C'EB
					|| __c == 0x0'2C'ED
					|| __c == 0x0'2C'F2
					|| __c == 0x0'A6'40
					|| __c == 0x0'A6'42
					|| __c == 0x0'A6'44
					|| __c == 0x0'A6'46
					|| __c == 0x0'A6'48
					|| __c == 0x0'A6'4A
					|| __c == 0x0'A6'4C
					|| __c == 0x0'A6'4E
					|| __c == 0x0'A6'50
					|| __c == 0x0'A6'52
					|| __c == 0x0'A6'54
					|| __c == 0x0'A6'56
					|| __c == 0x0'A6'58
					|| __c == 0x0'A6'5A
					|| __c == 0x0'A6'5C
					|| __c == 0x0'A6'5E
					|| __c == 0x0'A6'60
					|| __c == 0x0'A6'62
					|| __c == 0x0'A6'64
					|| __c == 0x0'A6'66
					|| __c == 0x0'A6'68
					|| __c == 0x0'A6'6A
					|| __c == 0x0'A6'6C
					|| __c == 0x0'A6'80
					|| __c == 0x0'A6'82
					|| __c == 0x0'A6'84
					|| __c == 0x0'A6'86
					|| __c == 0x0'A6'88
					|| __c == 0x0'A6'8A
					|| __c == 0x0'A6'8C
					|| __c == 0x0'A6'8E
					|| __c == 0x0'A6'90
					|| __c == 0x0'A6'92
					|| __c == 0x0'A6'94
					|| __c == 0x0'A6'96
					|| __c == 0x0'A6'98
					|| __c == 0x0'A6'9A
					|| __c == 0x0'A7'22
					|| __c == 0x0'A7'24
					|| __c == 0x0'A7'26
					|| __c == 0x0'A7'28
					|| __c == 0x0'A7'2A
					|| __c == 0x0'A7'2C
					|| __c == 0x0'A7'2E
					|| __c == 0x0'A7'32
					|| __c == 0x0'A7'34
					|| __c == 0x0'A7'36
					|| __c == 0x0'A7'38
					|| __c == 0x0'A7'3A
					|| __c == 0x0'A7'3C
					|| __c == 0x0'A7'3E
					|| __c == 0x0'A7'40
					|| __c == 0x0'A7'42
					|| __c == 0x0'A7'44
					|| __c == 0x0'A7'46
					|| __c == 0x0'A7'48
					|| __c == 0x0'A7'4A
					|| __c == 0x0'A7'4C
					|| __c == 0x0'A7'4E
					|| __c == 0x0'A7'50
					|| __c == 0x0'A7'52
					|| __c == 0x0'A7'54
					|| __c == 0x0'A7'56
					|| __c == 0x0'A7'58
					|| __c == 0x0'A7'5A
					|| __c == 0x0'A7'5C
					|| __c == 0x0'A7'5E
					|| __c == 0x0'A7'60
					|| __c == 0x0'A7'62
					|| __c == 0x0'A7'64
					|| __c == 0x0'A7'66
					|| __c == 0x0'A7'68
					|| __c == 0x0'A7'6A
					|| __c == 0x0'A7'6C
					|| __c == 0x0'A7'6E
					|| __c == 0x0'A7'79
					|| __c == 0x0'A7'7B
					|| __c == 0x0'A7'7D
					|| __c == 0x0'A7'7E
					|| __c == 0x0'A7'80
					|| __c == 0x0'A7'82
					|| __c == 0x0'A7'84
					|| __c == 0x0'A7'86
					|| __c == 0x0'A7'8B
					|| __c == 0x0'A7'8D
					|| __c == 0x0'A7'90
					|| __c == 0x0'A7'92
					|| __c == 0x0'A7'96
					|| __c == 0x0'A7'98
					|| __c == 0x0'A7'9A
					|| __c == 0x0'A7'9C
					|| __c == 0x0'A7'9E
					|| __c == 0x0'A7'A0
					|| __c == 0x0'A7'A2
					|| __c == 0x0'A7'A4
					|| __c == 0x0'A7'A6
					|| __c == 0x0'A7'A8
					|| __c == 0x0'A7'AA
					|| __c == 0x0'A7'AB
					|| __c == 0x0'A7'AC
					|| __c == 0x0'A7'AD
					|| __c == 0x0'A7'AE
					|| __c == 0x0'A7'B0
					|| __c == 0x0'A7'B1
					|| __c == 0x0'A7'B2
					|| __c == 0x0'A7'B3
					|| __c == 0x0'A7'B4
					|| __c == 0x0'A7'B6
					|| __c == 0x0'A7'B8
					|| __c == 0x0'A7'BA
					|| __c == 0x0'A7'BC
					|| __c == 0x0'A7'BE
					|| __c == 0x0'A7'C2
					|| __c == 0x0'A7'C4
					|| __c == 0x0'A7'C5
					|| __c == 0x0'A7'C6
					|| __c == 0x0'AB'70
					|| __c == 0x0'AB'71
					|| __c == 0x0'AB'72
					|| __c == 0x0'AB'73
					|| __c == 0x0'AB'74
					|| __c == 0x0'AB'75
					|| __c == 0x0'AB'76
					|| __c == 0x0'AB'77
					|| __c == 0x0'AB'78
					|| __c == 0x0'AB'79
					|| __c == 0x0'AB'7A
					|| __c == 0x0'AB'7B
					|| __c == 0x0'AB'7C
					|| __c == 0x0'AB'7D
					|| __c == 0x0'AB'7E
					|| __c == 0x0'AB'7F
					|| __c == 0x0'AB'80
					|| __c == 0x0'AB'81
					|| __c == 0x0'AB'82
					|| __c == 0x0'AB'83
					|| __c == 0x0'AB'84
					|| __c == 0x0'AB'85
					|| __c == 0x0'AB'86
					|| __c == 0x0'AB'87
					|| __c == 0x0'AB'88
					|| __c == 0x0'AB'89
					|| __c == 0x0'AB'8A
					|| __c == 0x0'AB'8B
					|| __c == 0x0'AB'8C
					|| __c == 0x0'AB'8D
					|| __c == 0x0'AB'8E
					|| __c == 0x0'AB'8F
					|| __c == 0x0'AB'90
					|| __c == 0x0'AB'91
					|| __c == 0x0'AB'92
					|| __c == 0x0'AB'93
					|| __c == 0x0'AB'94
					|| __c == 0x0'AB'95
					|| __c == 0x0'AB'96
					|| __c == 0x0'AB'97
					|| __c == 0x0'AB'98
					|| __c == 0x0'AB'99
					|| __c == 0x0'AB'9A
					|| __c == 0x0'AB'9B
					|| __c == 0x0'AB'9C
					|| __c == 0x0'AB'9D
					|| __c == 0x0'AB'9E
					|| __c == 0x0'AB'9F
					|| __c == 0x0'AB'A0
					|| __c == 0x0'AB'A1
					|| __c == 0x0'AB'A2
					|| __c == 0x0'AB'A3
					|| __c == 0x0'AB'A4
					|| __c == 0x0'AB'A5
					|| __c == 0x0'AB'A6
					|| __c == 0x0'AB'A7
					|| __c == 0x0'AB'A8
					|| __c == 0x0'AB'A9
					|| __c == 0x0'AB'AA
					|| __c == 0x0'AB'AB
					|| __c == 0x0'AB'AC
					|| __c == 0x0'AB'AD
					|| __c == 0x0'AB'AE
					|| __c == 0x0'AB'AF
					|| __c == 0x0'AB'B0
					|| __c == 0x0'AB'B1
					|| __c == 0x0'AB'B2
					|| __c == 0x0'AB'B3
					|| __c == 0x0'AB'B4
					|| __c == 0x0'AB'B5
					|| __c == 0x0'AB'B6
					|| __c == 0x0'AB'B7
					|| __c == 0x0'AB'B8
					|| __c == 0x0'AB'B9
					|| __c == 0x0'AB'BA
					|| __c == 0x0'AB'BB
					|| __c == 0x0'AB'BC
					|| __c == 0x0'AB'BD
					|| __c == 0x0'AB'BE
					|| __c == 0x0'AB'BF
					|| __c == 0x0'FF'21
					|| __c == 0x0'FF'22
					|| __c == 0x0'FF'23
					|| __c == 0x0'FF'24
					|| __c == 0x0'FF'25
					|| __c == 0x0'FF'26
					|| __c == 0x0'FF'27
					|| __c == 0x0'FF'28
					|| __c == 0x0'FF'29
					|| __c == 0x0'FF'2A
					|| __c == 0x0'FF'2B
					|| __c == 0x0'FF'2C
					|| __c == 0x0'FF'2D
					|| __c == 0x0'FF'2E
					|| __c == 0x0'FF'2F
					|| __c == 0x0'FF'30
					|| __c == 0x0'FF'31
					|| __c == 0x0'FF'32
					|| __c == 0x0'FF'33
					|| __c == 0x0'FF'34
					|| __c == 0x0'FF'35
					|| __c == 0x0'FF'36
					|| __c == 0x0'FF'37
					|| __c == 0x0'FF'38
					|| __c == 0x0'FF'39
					|| __c == 0x0'FF'3A
					|| __c == 0x1'04'00
					|| __c == 0x1'04'01
					|| __c == 0x1'04'02
					|| __c == 0x1'04'03
					|| __c == 0x1'04'04
					|| __c == 0x1'04'05
					|| __c == 0x1'04'06
					|| __c == 0x1'04'07
					|| __c == 0x1'04'08
					|| __c == 0x1'04'09
					|| __c == 0x1'04'0A
					|| __c == 0x1'04'0B
					|| __c == 0x1'04'0C
					|| __c == 0x1'04'0D
					|| __c == 0x1'04'0E
					|| __c == 0x1'04'0F
					|| __c == 0x1'04'10
					|| __c == 0x1'04'11
					|| __c == 0x1'04'12
					|| __c == 0x1'04'13
					|| __c == 0x1'04'14
					|| __c == 0x1'04'15
					|| __c == 0x1'04'16
					|| __c == 0x1'04'17
					|| __c == 0x1'04'18
					|| __c == 0x1'04'19
					|| __c == 0x1'04'1A
					|| __c == 0x1'04'1B
					|| __c == 0x1'04'1C
					|| __c == 0x1'04'1D
					|| __c == 0x1'04'1E
					|| __c == 0x1'04'1F
					|| __c == 0x1'04'20
					|| __c == 0x1'04'21
					|| __c == 0x1'04'22
					|| __c == 0x1'04'23
					|| __c == 0x1'04'24
					|| __c == 0x1'04'25
					|| __c == 0x1'04'26
					|| __c == 0x1'04'27
					|| __c == 0x1'04'B0
					|| __c == 0x1'04'B1
					|| __c == 0x1'04'B2
					|| __c == 0x1'04'B3
					|| __c == 0x1'04'B4
					|| __c == 0x1'04'B5
					|| __c == 0x1'04'B6
					|| __c == 0x1'04'B7
					|| __c == 0x1'04'B8
					|| __c == 0x1'04'B9
					|| __c == 0x1'04'BA
					|| __c == 0x1'04'BB
					|| __c == 0x1'04'BC
					|| __c == 0x1'04'BD
					|| __c == 0x1'04'BE
					|| __c == 0x1'04'BF
					|| __c == 0x1'04'C0
					|| __c == 0x1'04'C1
					|| __c == 0x1'04'C2
					|| __c == 0x1'04'C3
					|| __c == 0x1'04'C4
					|| __c == 0x1'04'C5
					|| __c == 0x1'04'C6
					|| __c == 0x1'04'C7
					|| __c == 0x1'04'C8
					|| __c == 0x1'04'C9
					|| __c == 0x1'04'CA
					|| __c == 0x1'04'CB
					|| __c == 0x1'04'CC
					|| __c == 0x1'04'CD
					|| __c == 0x1'04'CE
					|| __c == 0x1'04'CF
					|| __c == 0x1'04'D0
					|| __c == 0x1'04'D1
					|| __c == 0x1'04'D2
					|| __c == 0x1'04'D3
					|| __c == 0x1'0C'80
					|| __c == 0x1'0C'81
					|| __c == 0x1'0C'82
					|| __c == 0x1'0C'83
					|| __c == 0x1'0C'84
					|| __c == 0x1'0C'85
					|| __c == 0x1'0C'86
					|| __c == 0x1'0C'87
					|| __c == 0x1'0C'88
					|| __c == 0x1'0C'89
					|| __c == 0x1'0C'8A
					|| __c == 0x1'0C'8B
					|| __c == 0x1'0C'8C
					|| __c == 0x1'0C'8D
					|| __c == 0x1'0C'8E
					|| __c == 0x1'0C'8F
					|| __c == 0x1'0C'90
					|| __c == 0x1'0C'91
					|| __c == 0x1'0C'92
					|| __c == 0x1'0C'93
					|| __c == 0x1'0C'94
					|| __c == 0x1'0C'95
					|| __c == 0x1'0C'96
					|| __c == 0x1'0C'97
					|| __c == 0x1'0C'98
					|| __c == 0x1'0C'99
					|| __c == 0x1'0C'9A
					|| __c == 0x1'0C'9B
					|| __c == 0x1'0C'9C
					|| __c == 0x1'0C'9D
					|| __c == 0x1'0C'9E
					|| __c == 0x1'0C'9F
					|| __c == 0x1'0C'A0
					|| __c == 0x1'0C'A1
					|| __c == 0x1'0C'A2
					|| __c == 0x1'0C'A3
					|| __c == 0x1'0C'A4
					|| __c == 0x1'0C'A5
					|| __c == 0x1'0C'A6
					|| __c == 0x1'0C'A7
					|| __c == 0x1'0C'A8
					|| __c == 0x1'0C'A9
					|| __c == 0x1'0C'AA
					|| __c == 0x1'0C'AB
					|| __c == 0x1'0C'AC
					|| __c == 0x1'0C'AD
					|| __c == 0x1'0C'AE
					|| __c == 0x1'0C'AF
					|| __c == 0x1'0C'B0
					|| __c == 0x1'0C'B1
					|| __c == 0x1'0C'B2
					|| __c == 0x1'18'A0
					|| __c == 0x1'18'A1
					|| __c == 0x1'18'A2
					|| __c == 0x1'18'A3
					|| __c == 0x1'18'A4
					|| __c == 0x1'18'A5
					|| __c == 0x1'18'A6
					|| __c == 0x1'18'A7
					|| __c == 0x1'18'A8
					|| __c == 0x1'18'A9
					|| __c == 0x1'18'AA
					|| __c == 0x1'18'AB
					|| __c == 0x1'18'AC
					|| __c == 0x1'18'AD
					|| __c == 0x1'18'AE
					|| __c == 0x1'18'AF
					|| __c == 0x1'18'B0
					|| __c == 0x1'18'B1
					|| __c == 0x1'18'B2
					|| __c == 0x1'18'B3
					|| __c == 0x1'18'B4
					|| __c == 0x1'18'B5
					|| __c == 0x1'18'B6
					|| __c == 0x1'18'B7
					|| __c == 0x1'18'B8
					|| __c == 0x1'18'B9
					|| __c == 0x1'18'BA
					|| __c == 0x1'18'BB
					|| __c == 0x1'18'BC
					|| __c == 0x1'18'BD
					|| __c == 0x1'18'BE
					|| __c == 0x1'18'BF
					|| __c == 0x1'6E'40
					|| __c == 0x1'6E'41
					|| __c == 0x1'6E'42
					|| __c == 0x1'6E'43
					|| __c == 0x1'6E'44
					|| __c == 0x1'6E'45
					|| __c == 0x1'6E'46
					|| __c == 0x1'6E'47
					|| __c == 0x1'6E'48
					|| __c == 0x1'6E'49
					|| __c == 0x1'6E'4A
					|| __c == 0x1'6E'4B
					|| __c == 0x1'6E'4C
					|| __c == 0x1'6E'4D
					|| __c == 0x1'6E'4E
					|| __c == 0x1'6E'4F
					|| __c == 0x1'6E'50
					|| __c == 0x1'6E'51
					|| __c == 0x1'6E'52
					|| __c == 0x1'6E'53
					|| __c == 0x1'6E'54
					|| __c == 0x1'6E'55
					|| __c == 0x1'6E'56
					|| __c == 0x1'6E'57
					|| __c == 0x1'6E'58
					|| __c == 0x1'6E'59
					|| __c == 0x1'6E'5A
					|| __c == 0x1'6E'5B
					|| __c == 0x1'6E'5C
					|| __c == 0x1'6E'5D
					|| __c == 0x1'6E'5E
					|| __c == 0x1'6E'5F
					|| __c == 0x1'E9'00
					|| __c == 0x1'E9'01
					|| __c == 0x1'E9'02
					|| __c == 0x1'E9'03
					|| __c == 0x1'E9'04
					|| __c == 0x1'E9'05
					|| __c == 0x1'E9'06
					|| __c == 0x1'E9'07
					|| __c == 0x1'E9'08
					|| __c == 0x1'E9'09
					|| __c == 0x1'E9'0A
					|| __c == 0x1'E9'0B
					|| __c == 0x1'E9'0C
					|| __c == 0x1'E9'0D
					|| __c == 0x1'E9'0E
					|| __c == 0x1'E9'0F
					|| __c == 0x1'E9'10
					|| __c == 0x1'E9'11
					|| __c == 0x1'E9'12
					|| __c == 0x1'E9'13
					|| __c == 0x1'E9'14
					|| __c == 0x1'E9'15
					|| __c == 0x1'E9'16
					|| __c == 0x1'E9'17
					|| __c == 0x1'E9'18
					|| __c == 0x1'E9'19
					|| __c == 0x1'E9'1A
					|| __c == 0x1'E9'1B
					|| __c == 0x1'E9'1C
					|| __c == 0x1'E9'1D
					|| __c == 0x1'E9'1E
					|| __c == 0x1'E9'1F
					|| __c == 0x1'E9'20
					|| __c == 0x1'E9'21;
		}
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr bool
		is_lower(
		 u8code_t __c) pf_attr_noexcept
		{
			return __c == 0x0'00'61
					|| __c == 0x0'00'62
					|| __c == 0x0'00'63
					|| __c == 0x0'00'64
					|| __c == 0x0'00'65
					|| __c == 0x0'00'66
					|| __c == 0x0'00'67
					|| __c == 0x0'00'68
					|| __c == 0x0'00'69
					|| __c == 0x0'00'6a
					|| __c == 0x0'00'6b
					|| __c == 0x0'00'6c
					|| __c == 0x0'00'6d
					|| __c == 0x0'00'6e
					|| __c == 0x0'00'6f
					|| __c == 0x0'00'70
					|| __c == 0x0'00'71
					|| __c == 0x0'00'72
					|| __c == 0x0'00'73
					|| __c == 0x0'00'74
					|| __c == 0x0'00'75
					|| __c == 0x0'00'76
					|| __c == 0x0'00'77
					|| __c == 0x0'00'78
					|| __c == 0x0'00'79
					|| __c == 0x0'00'7a
					|| __c == 0x0'00'df
					|| __c == 0x0'00'e0
					|| __c == 0x0'00'e1
					|| __c == 0x0'00'e2
					|| __c == 0x0'00'e3
					|| __c == 0x0'00'e4
					|| __c == 0x0'00'e5
					|| __c == 0x0'00'e6
					|| __c == 0x0'00'e7
					|| __c == 0x0'00'e8
					|| __c == 0x0'00'e9
					|| __c == 0x0'00'ea
					|| __c == 0x0'00'eb
					|| __c == 0x0'00'ec
					|| __c == 0x0'00'ed
					|| __c == 0x0'00'ee
					|| __c == 0x0'00'ef
					|| __c == 0x0'00'f0
					|| __c == 0x0'00'f1
					|| __c == 0x0'00'f2
					|| __c == 0x0'00'f3
					|| __c == 0x0'00'f4
					|| __c == 0x0'00'f5
					|| __c == 0x0'00'f6
					|| __c == 0x0'00'f8
					|| __c == 0x0'00'f9
					|| __c == 0x0'00'fa
					|| __c == 0x0'00'fb
					|| __c == 0x0'00'fc
					|| __c == 0x0'00'fd
					|| __c == 0x0'00'fe
					|| __c == 0x0'00'ff
					|| __c == 0x0'01'01
					|| __c == 0x0'01'03
					|| __c == 0x0'01'05
					|| __c == 0x0'01'07
					|| __c == 0x0'01'09
					|| __c == 0x0'01'0b
					|| __c == 0x0'01'0d
					|| __c == 0x0'01'0f
					|| __c == 0x0'01'11
					|| __c == 0x0'01'13
					|| __c == 0x0'01'15
					|| __c == 0x0'01'17
					|| __c == 0x0'01'19
					|| __c == 0x0'01'1b
					|| __c == 0x0'01'1d
					|| __c == 0x0'01'1f
					|| __c == 0x0'01'21
					|| __c == 0x0'01'23
					|| __c == 0x0'01'25
					|| __c == 0x0'01'27
					|| __c == 0x0'01'29
					|| __c == 0x0'01'2b
					|| __c == 0x0'01'2d
					|| __c == 0x0'01'2f
					|| __c == 0x0'01'33
					|| __c == 0x0'01'35
					|| __c == 0x0'01'37
					|| __c == 0x0'01'3a
					|| __c == 0x0'01'3c
					|| __c == 0x0'01'3e
					|| __c == 0x0'01'40
					|| __c == 0x0'01'42
					|| __c == 0x0'01'44
					|| __c == 0x0'01'46
					|| __c == 0x0'01'48
					|| __c == 0x0'01'4b
					|| __c == 0x0'01'4d
					|| __c == 0x0'01'4f
					|| __c == 0x0'01'51
					|| __c == 0x0'01'53
					|| __c == 0x0'01'55
					|| __c == 0x0'01'57
					|| __c == 0x0'01'59
					|| __c == 0x0'01'5b
					|| __c == 0x0'01'5d
					|| __c == 0x0'01'5f
					|| __c == 0x0'01'61
					|| __c == 0x0'01'63
					|| __c == 0x0'01'65
					|| __c == 0x0'01'67
					|| __c == 0x0'01'69
					|| __c == 0x0'01'6b
					|| __c == 0x0'01'6d
					|| __c == 0x0'01'6f
					|| __c == 0x0'01'71
					|| __c == 0x0'01'73
					|| __c == 0x0'01'75
					|| __c == 0x0'01'77
					|| __c == 0x0'01'7a
					|| __c == 0x0'01'7c
					|| __c == 0x0'01'7e
					|| __c == 0x0'01'80
					|| __c == 0x0'01'83
					|| __c == 0x0'01'85
					|| __c == 0x0'01'88
					|| __c == 0x0'01'8c
					|| __c == 0x0'01'92
					|| __c == 0x0'01'95
					|| __c == 0x0'01'99
					|| __c == 0x0'01'9a
					|| __c == 0x0'01'9e
					|| __c == 0x0'01'a1
					|| __c == 0x0'01'a3
					|| __c == 0x0'01'a5
					|| __c == 0x0'01'a8
					|| __c == 0x0'01'ad
					|| __c == 0x0'01'b0
					|| __c == 0x0'01'b4
					|| __c == 0x0'01'b6
					|| __c == 0x0'01'b9
					|| __c == 0x0'01'bd
					|| __c == 0x0'01'bf
					|| __c == 0x0'01'c6
					|| __c == 0x0'01'c9
					|| __c == 0x0'01'cc
					|| __c == 0x0'01'ce
					|| __c == 0x0'01'd0
					|| __c == 0x0'01'd2
					|| __c == 0x0'01'd4
					|| __c == 0x0'01'd6
					|| __c == 0x0'01'd8
					|| __c == 0x0'01'da
					|| __c == 0x0'01'dc
					|| __c == 0x0'01'dd
					|| __c == 0x0'01'df
					|| __c == 0x0'01'e1
					|| __c == 0x0'01'e3
					|| __c == 0x0'01'e5
					|| __c == 0x0'01'e7
					|| __c == 0x0'01'e9
					|| __c == 0x0'01'eb
					|| __c == 0x0'01'ed
					|| __c == 0x0'01'ef
					|| __c == 0x0'01'f3
					|| __c == 0x0'01'f5
					|| __c == 0x0'01'f9
					|| __c == 0x0'01'fb
					|| __c == 0x0'01'fd
					|| __c == 0x0'01'ff
					|| __c == 0x0'02'01
					|| __c == 0x0'02'03
					|| __c == 0x0'02'05
					|| __c == 0x0'02'07
					|| __c == 0x0'02'09
					|| __c == 0x0'02'0b
					|| __c == 0x0'02'0d
					|| __c == 0x0'02'0f
					|| __c == 0x0'02'11
					|| __c == 0x0'02'13
					|| __c == 0x0'02'15
					|| __c == 0x0'02'17
					|| __c == 0x0'02'19
					|| __c == 0x0'02'1b
					|| __c == 0x0'02'1d
					|| __c == 0x0'02'1f
					|| __c == 0x0'02'23
					|| __c == 0x0'02'25
					|| __c == 0x0'02'27
					|| __c == 0x0'02'29
					|| __c == 0x0'02'2b
					|| __c == 0x0'02'2d
					|| __c == 0x0'02'2f
					|| __c == 0x0'02'31
					|| __c == 0x0'02'33
					|| __c == 0x0'02'3c
					|| __c == 0x0'02'3f
					|| __c == 0x0'02'40
					|| __c == 0x0'02'42
					|| __c == 0x0'02'47
					|| __c == 0x0'02'49
					|| __c == 0x0'02'4b
					|| __c == 0x0'02'4d
					|| __c == 0x0'02'4f
					|| __c == 0x0'02'50
					|| __c == 0x0'02'51
					|| __c == 0x0'02'52
					|| __c == 0x0'02'53
					|| __c == 0x0'02'54
					|| __c == 0x0'02'56
					|| __c == 0x0'02'57
					|| __c == 0x0'02'59
					|| __c == 0x0'02'5b
					|| __c == 0x0'02'5c
					|| __c == 0x0'02'60
					|| __c == 0x0'02'61
					|| __c == 0x0'02'63
					|| __c == 0x0'02'65
					|| __c == 0x0'02'66
					|| __c == 0x0'02'68
					|| __c == 0x0'02'69
					|| __c == 0x0'02'6a
					|| __c == 0x0'02'6b
					|| __c == 0x0'02'6c
					|| __c == 0x0'02'6f
					|| __c == 0x0'02'71
					|| __c == 0x0'02'72
					|| __c == 0x0'02'75
					|| __c == 0x0'02'7d
					|| __c == 0x0'02'80
					|| __c == 0x0'02'82
					|| __c == 0x0'02'83
					|| __c == 0x0'02'87
					|| __c == 0x0'02'88
					|| __c == 0x0'02'89
					|| __c == 0x0'02'8a
					|| __c == 0x0'02'8b
					|| __c == 0x0'02'8c
					|| __c == 0x0'02'92
					|| __c == 0x0'02'9d
					|| __c == 0x0'02'9e
					|| __c == 0x0'03'71
					|| __c == 0x0'03'73
					|| __c == 0x0'03'77
					|| __c == 0x0'03'7b
					|| __c == 0x0'03'7c
					|| __c == 0x0'03'7d
					|| __c == 0x0'03'ac
					|| __c == 0x0'03'ad
					|| __c == 0x0'03'ae
					|| __c == 0x0'03'af
					|| __c == 0x0'03'b1
					|| __c == 0x0'03'b2
					|| __c == 0x0'03'b3
					|| __c == 0x0'03'b4
					|| __c == 0x0'03'b5
					|| __c == 0x0'03'b6
					|| __c == 0x0'03'b7
					|| __c == 0x0'03'b8
					|| __c == 0x0'03'b9
					|| __c == 0x0'03'ba
					|| __c == 0x0'03'bb
					|| __c == 0x0'03'bc
					|| __c == 0x0'03'bd
					|| __c == 0x0'03'be
					|| __c == 0x0'03'bf
					|| __c == 0x0'03'c0
					|| __c == 0x0'03'c1
					|| __c == 0x0'03'c3
					|| __c == 0x0'03'c4
					|| __c == 0x0'03'c5
					|| __c == 0x0'03'c6
					|| __c == 0x0'03'c7
					|| __c == 0x0'03'c8
					|| __c == 0x0'03'c9
					|| __c == 0x0'03'ca
					|| __c == 0x0'03'cb
					|| __c == 0x0'03'cc
					|| __c == 0x0'03'cd
					|| __c == 0x0'03'ce
					|| __c == 0x0'03'd7
					|| __c == 0x0'03'd9
					|| __c == 0x0'03'db
					|| __c == 0x0'03'dd
					|| __c == 0x0'03'df
					|| __c == 0x0'03'e1
					|| __c == 0x0'03'e3
					|| __c == 0x0'03'e5
					|| __c == 0x0'03'e7
					|| __c == 0x0'03'e9
					|| __c == 0x0'03'eb
					|| __c == 0x0'03'ed
					|| __c == 0x0'03'ef
					|| __c == 0x0'03'f2
					|| __c == 0x0'03'f3
					|| __c == 0x0'03'f8
					|| __c == 0x0'03'fb
					|| __c == 0x0'04'30
					|| __c == 0x0'04'31
					|| __c == 0x0'04'32
					|| __c == 0x0'04'33
					|| __c == 0x0'04'34
					|| __c == 0x0'04'35
					|| __c == 0x0'04'36
					|| __c == 0x0'04'37
					|| __c == 0x0'04'38
					|| __c == 0x0'04'39
					|| __c == 0x0'04'3a
					|| __c == 0x0'04'3b
					|| __c == 0x0'04'3c
					|| __c == 0x0'04'3d
					|| __c == 0x0'04'3e
					|| __c == 0x0'04'3f
					|| __c == 0x0'04'40
					|| __c == 0x0'04'41
					|| __c == 0x0'04'42
					|| __c == 0x0'04'42
					|| __c == 0x0'04'43
					|| __c == 0x0'04'44
					|| __c == 0x0'04'45
					|| __c == 0x0'04'46
					|| __c == 0x0'04'47
					|| __c == 0x0'04'48
					|| __c == 0x0'04'49
					|| __c == 0x0'04'4a
					|| __c == 0x0'04'4b
					|| __c == 0x0'04'4c
					|| __c == 0x0'04'4d
					|| __c == 0x0'04'4e
					|| __c == 0x0'04'4f
					|| __c == 0x0'04'50
					|| __c == 0x0'04'51
					|| __c == 0x0'04'52
					|| __c == 0x0'04'53
					|| __c == 0x0'04'54
					|| __c == 0x0'04'55
					|| __c == 0x0'04'56
					|| __c == 0x0'04'57
					|| __c == 0x0'04'58
					|| __c == 0x0'04'59
					|| __c == 0x0'04'5a
					|| __c == 0x0'04'5b
					|| __c == 0x0'04'5c
					|| __c == 0x0'04'5d
					|| __c == 0x0'04'5e
					|| __c == 0x0'04'5f
					|| __c == 0x0'04'61
					|| __c == 0x0'04'63
					|| __c == 0x0'04'65
					|| __c == 0x0'04'67
					|| __c == 0x0'04'69
					|| __c == 0x0'04'6b
					|| __c == 0x0'04'6d
					|| __c == 0x0'04'6f
					|| __c == 0x0'04'71
					|| __c == 0x0'04'73
					|| __c == 0x0'04'75
					|| __c == 0x0'04'77
					|| __c == 0x0'04'79
					|| __c == 0x0'04'7b
					|| __c == 0x0'04'7d
					|| __c == 0x0'04'7f
					|| __c == 0x0'04'81
					|| __c == 0x0'04'8b
					|| __c == 0x0'04'8d
					|| __c == 0x0'04'8f
					|| __c == 0x0'04'91
					|| __c == 0x0'04'93
					|| __c == 0x0'04'95
					|| __c == 0x0'04'97
					|| __c == 0x0'04'99
					|| __c == 0x0'04'9b
					|| __c == 0x0'04'9d
					|| __c == 0x0'04'9f
					|| __c == 0x0'04'a1
					|| __c == 0x0'04'a3
					|| __c == 0x0'04'a5
					|| __c == 0x0'04'a7
					|| __c == 0x0'04'a9
					|| __c == 0x0'04'ab
					|| __c == 0x0'04'ad
					|| __c == 0x0'04'af
					|| __c == 0x0'04'b1
					|| __c == 0x0'04'b3
					|| __c == 0x0'04'b5
					|| __c == 0x0'04'b7
					|| __c == 0x0'04'b9
					|| __c == 0x0'04'bb
					|| __c == 0x0'04'bd
					|| __c == 0x0'04'bf
					|| __c == 0x0'04'c2
					|| __c == 0x0'04'c4
					|| __c == 0x0'04'c6
					|| __c == 0x0'04'c8
					|| __c == 0x0'04'ca
					|| __c == 0x0'04'cc
					|| __c == 0x0'04'ce
					|| __c == 0x0'04'cf
					|| __c == 0x0'04'd1
					|| __c == 0x0'04'd3
					|| __c == 0x0'04'd5
					|| __c == 0x0'04'd7
					|| __c == 0x0'04'd9
					|| __c == 0x0'04'db
					|| __c == 0x0'04'dd
					|| __c == 0x0'04'df
					|| __c == 0x0'04'e1
					|| __c == 0x0'04'e3
					|| __c == 0x0'04'e5
					|| __c == 0x0'04'e7
					|| __c == 0x0'04'e9
					|| __c == 0x0'04'eb
					|| __c == 0x0'04'ed
					|| __c == 0x0'04'ef
					|| __c == 0x0'04'f1
					|| __c == 0x0'04'f3
					|| __c == 0x0'04'f5
					|| __c == 0x0'04'f7
					|| __c == 0x0'04'f9
					|| __c == 0x0'04'fb
					|| __c == 0x0'04'fd
					|| __c == 0x0'04'ff
					|| __c == 0x0'05'01
					|| __c == 0x0'05'03
					|| __c == 0x0'05'05
					|| __c == 0x0'05'07
					|| __c == 0x0'05'09
					|| __c == 0x0'05'0b
					|| __c == 0x0'05'0d
					|| __c == 0x0'05'0f
					|| __c == 0x0'05'11
					|| __c == 0x0'05'13
					|| __c == 0x0'05'15
					|| __c == 0x0'05'17
					|| __c == 0x0'05'19
					|| __c == 0x0'05'1b
					|| __c == 0x0'05'1d
					|| __c == 0x0'05'1f
					|| __c == 0x0'05'21
					|| __c == 0x0'05'23
					|| __c == 0x0'05'25
					|| __c == 0x0'05'27
					|| __c == 0x0'05'29
					|| __c == 0x0'05'2b
					|| __c == 0x0'05'2d
					|| __c == 0x0'05'2f
					|| __c == 0x0'05'61
					|| __c == 0x0'05'62
					|| __c == 0x0'05'63
					|| __c == 0x0'05'64
					|| __c == 0x0'05'65
					|| __c == 0x0'05'66
					|| __c == 0x0'05'67
					|| __c == 0x0'05'68
					|| __c == 0x0'05'69
					|| __c == 0x0'05'6a
					|| __c == 0x0'05'6b
					|| __c == 0x0'05'6c
					|| __c == 0x0'05'6d
					|| __c == 0x0'05'6e
					|| __c == 0x0'05'6f
					|| __c == 0x0'05'70
					|| __c == 0x0'05'71
					|| __c == 0x0'05'72
					|| __c == 0x0'05'73
					|| __c == 0x0'05'74
					|| __c == 0x0'05'75
					|| __c == 0x0'05'76
					|| __c == 0x0'05'77
					|| __c == 0x0'05'78
					|| __c == 0x0'05'79
					|| __c == 0x0'05'7a
					|| __c == 0x0'05'7b
					|| __c == 0x0'05'7c
					|| __c == 0x0'05'7d
					|| __c == 0x0'05'7e
					|| __c == 0x0'05'7f
					|| __c == 0x0'05'80
					|| __c == 0x0'05'81
					|| __c == 0x0'05'82
					|| __c == 0x0'05'83
					|| __c == 0x0'05'84
					|| __c == 0x0'05'85
					|| __c == 0x0'05'86
					|| __c == 0x0'10'd0
					|| __c == 0x0'10'd1
					|| __c == 0x0'10'd2
					|| __c == 0x0'10'd3
					|| __c == 0x0'10'd4
					|| __c == 0x0'10'd5
					|| __c == 0x0'10'd6
					|| __c == 0x0'10'd7
					|| __c == 0x0'10'd8
					|| __c == 0x0'10'd9
					|| __c == 0x0'10'da
					|| __c == 0x0'10'db
					|| __c == 0x0'10'dc
					|| __c == 0x0'10'dd
					|| __c == 0x0'10'de
					|| __c == 0x0'10'df
					|| __c == 0x0'10'e0
					|| __c == 0x0'10'e1
					|| __c == 0x0'10'e2
					|| __c == 0x0'10'e3
					|| __c == 0x0'10'e4
					|| __c == 0x0'10'e5
					|| __c == 0x0'10'e6
					|| __c == 0x0'10'e7
					|| __c == 0x0'10'e8
					|| __c == 0x0'10'e9
					|| __c == 0x0'10'ea
					|| __c == 0x0'10'eb
					|| __c == 0x0'10'ec
					|| __c == 0x0'10'ed
					|| __c == 0x0'10'ee
					|| __c == 0x0'10'ef
					|| __c == 0x0'10'f0
					|| __c == 0x0'10'f1
					|| __c == 0x0'10'f2
					|| __c == 0x0'10'f3
					|| __c == 0x0'10'f4
					|| __c == 0x0'10'f5
					|| __c == 0x0'10'f6
					|| __c == 0x0'10'f7
					|| __c == 0x0'10'f8
					|| __c == 0x0'10'f9
					|| __c == 0x0'10'fa
					|| __c == 0x0'10'fd
					|| __c == 0x0'10'fe
					|| __c == 0x0'10'ff
					|| __c == 0x0'13'a0
					|| __c == 0x0'13'a1
					|| __c == 0x0'13'a2
					|| __c == 0x0'13'a3
					|| __c == 0x0'13'a4
					|| __c == 0x0'13'a5
					|| __c == 0x0'13'a6
					|| __c == 0x0'13'a7
					|| __c == 0x0'13'a8
					|| __c == 0x0'13'a9
					|| __c == 0x0'13'aa
					|| __c == 0x0'13'ab
					|| __c == 0x0'13'ac
					|| __c == 0x0'13'ad
					|| __c == 0x0'13'ae
					|| __c == 0x0'13'af
					|| __c == 0x0'13'b0
					|| __c == 0x0'13'b1
					|| __c == 0x0'13'b2
					|| __c == 0x0'13'b3
					|| __c == 0x0'13'b4
					|| __c == 0x0'13'b5
					|| __c == 0x0'13'b6
					|| __c == 0x0'13'b7
					|| __c == 0x0'13'b8
					|| __c == 0x0'13'b9
					|| __c == 0x0'13'ba
					|| __c == 0x0'13'bb
					|| __c == 0x0'13'bc
					|| __c == 0x0'13'bd
					|| __c == 0x0'13'be
					|| __c == 0x0'13'bf
					|| __c == 0x0'13'c0
					|| __c == 0x0'13'c1
					|| __c == 0x0'13'c2
					|| __c == 0x0'13'c3
					|| __c == 0x0'13'c4
					|| __c == 0x0'13'c5
					|| __c == 0x0'13'c6
					|| __c == 0x0'13'c7
					|| __c == 0x0'13'c8
					|| __c == 0x0'13'c9
					|| __c == 0x0'13'ca
					|| __c == 0x0'13'cb
					|| __c == 0x0'13'cc
					|| __c == 0x0'13'cd
					|| __c == 0x0'13'ce
					|| __c == 0x0'13'cf
					|| __c == 0x0'13'd0
					|| __c == 0x0'13'd1
					|| __c == 0x0'13'd2
					|| __c == 0x0'13'd3
					|| __c == 0x0'13'd4
					|| __c == 0x0'13'd5
					|| __c == 0x0'13'd6
					|| __c == 0x0'13'd7
					|| __c == 0x0'13'd8
					|| __c == 0x0'13'd9
					|| __c == 0x0'13'da
					|| __c == 0x0'13'db
					|| __c == 0x0'13'dc
					|| __c == 0x0'13'dd
					|| __c == 0x0'13'de
					|| __c == 0x0'13'df
					|| __c == 0x0'13'e0
					|| __c == 0x0'13'e1
					|| __c == 0x0'13'e2
					|| __c == 0x0'13'e3
					|| __c == 0x0'13'e4
					|| __c == 0x0'13'e5
					|| __c == 0x0'13'e6
					|| __c == 0x0'13'e7
					|| __c == 0x0'13'e8
					|| __c == 0x0'13'e9
					|| __c == 0x0'13'ea
					|| __c == 0x0'13'eb
					|| __c == 0x0'13'ec
					|| __c == 0x0'13'ed
					|| __c == 0x0'13'ee
					|| __c == 0x0'13'ef
					|| __c == 0x0'13'f0
					|| __c == 0x0'13'f1
					|| __c == 0x0'13'f2
					|| __c == 0x0'13'f3
					|| __c == 0x0'13'f4
					|| __c == 0x0'13'f5
					|| __c == 0x0'1d'79
					|| __c == 0x0'1d'7d
					|| __c == 0x0'1d'8e
					|| __c == 0x0'1e'01
					|| __c == 0x0'1e'03
					|| __c == 0x0'1e'05
					|| __c == 0x0'1e'07
					|| __c == 0x0'1e'09
					|| __c == 0x0'1e'0b
					|| __c == 0x0'1e'0d
					|| __c == 0x0'1e'0f
					|| __c == 0x0'1e'11
					|| __c == 0x0'1e'13
					|| __c == 0x0'1e'15
					|| __c == 0x0'1e'17
					|| __c == 0x0'1e'19
					|| __c == 0x0'1e'1b
					|| __c == 0x0'1e'1d
					|| __c == 0x0'1e'1f
					|| __c == 0x0'1e'21
					|| __c == 0x0'1e'23
					|| __c == 0x0'1e'25
					|| __c == 0x0'1e'27
					|| __c == 0x0'1e'29
					|| __c == 0x0'1e'2b
					|| __c == 0x0'1e'2d
					|| __c == 0x0'1e'2f
					|| __c == 0x0'1e'31
					|| __c == 0x0'1e'33
					|| __c == 0x0'1e'35
					|| __c == 0x0'1e'37
					|| __c == 0x0'1e'39
					|| __c == 0x0'1e'3b
					|| __c == 0x0'1e'3d
					|| __c == 0x0'1e'3f
					|| __c == 0x0'1e'41
					|| __c == 0x0'1e'43
					|| __c == 0x0'1e'45
					|| __c == 0x0'1e'47
					|| __c == 0x0'1e'49
					|| __c == 0x0'1e'4b
					|| __c == 0x0'1e'4d
					|| __c == 0x0'1e'4f
					|| __c == 0x0'1e'51
					|| __c == 0x0'1e'53
					|| __c == 0x0'1e'55
					|| __c == 0x0'1e'57
					|| __c == 0x0'1e'59
					|| __c == 0x0'1e'5b
					|| __c == 0x0'1e'5d
					|| __c == 0x0'1e'5f
					|| __c == 0x0'1e'61
					|| __c == 0x0'1e'63
					|| __c == 0x0'1e'65
					|| __c == 0x0'1e'67
					|| __c == 0x0'1e'69
					|| __c == 0x0'1e'6b
					|| __c == 0x0'1e'6d
					|| __c == 0x0'1e'6f
					|| __c == 0x0'1e'71
					|| __c == 0x0'1e'73
					|| __c == 0x0'1e'75
					|| __c == 0x0'1e'77
					|| __c == 0x0'1e'79
					|| __c == 0x0'1e'7b
					|| __c == 0x0'1e'7d
					|| __c == 0x0'1e'7f
					|| __c == 0x0'1e'81
					|| __c == 0x0'1e'83
					|| __c == 0x0'1e'85
					|| __c == 0x0'1e'87
					|| __c == 0x0'1e'89
					|| __c == 0x0'1e'8b
					|| __c == 0x0'1e'8d
					|| __c == 0x0'1e'8f
					|| __c == 0x0'1e'91
					|| __c == 0x0'1e'93
					|| __c == 0x0'1e'95
					|| __c == 0x0'1e'a1
					|| __c == 0x0'1e'a3
					|| __c == 0x0'1e'a5
					|| __c == 0x0'1e'a7
					|| __c == 0x0'1e'a9
					|| __c == 0x0'1e'ab
					|| __c == 0x0'1e'ad
					|| __c == 0x0'1e'af
					|| __c == 0x0'1e'b1
					|| __c == 0x0'1e'b3
					|| __c == 0x0'1e'b5
					|| __c == 0x0'1e'b7
					|| __c == 0x0'1e'b9
					|| __c == 0x0'1e'bb
					|| __c == 0x0'1e'bd
					|| __c == 0x0'1e'bf
					|| __c == 0x0'1e'c1
					|| __c == 0x0'1e'c3
					|| __c == 0x0'1e'c5
					|| __c == 0x0'1e'c7
					|| __c == 0x0'1e'c9
					|| __c == 0x0'1e'cb
					|| __c == 0x0'1e'cd
					|| __c == 0x0'1e'cf
					|| __c == 0x0'1e'd1
					|| __c == 0x0'1e'd3
					|| __c == 0x0'1e'd5
					|| __c == 0x0'1e'd7
					|| __c == 0x0'1e'd9
					|| __c == 0x0'1e'db
					|| __c == 0x0'1e'dd
					|| __c == 0x0'1e'df
					|| __c == 0x0'1e'e1
					|| __c == 0x0'1e'e3
					|| __c == 0x0'1e'e5
					|| __c == 0x0'1e'e7
					|| __c == 0x0'1e'e9
					|| __c == 0x0'1e'eb
					|| __c == 0x0'1e'ed
					|| __c == 0x0'1e'ef
					|| __c == 0x0'1e'f1
					|| __c == 0x0'1e'f3
					|| __c == 0x0'1e'f5
					|| __c == 0x0'1e'f7
					|| __c == 0x0'1e'f9
					|| __c == 0x0'1e'fb
					|| __c == 0x0'1e'fd
					|| __c == 0x0'1e'ff
					|| __c == 0x0'1f'00
					|| __c == 0x0'1f'01
					|| __c == 0x0'1f'02
					|| __c == 0x0'1f'03
					|| __c == 0x0'1f'04
					|| __c == 0x0'1f'05
					|| __c == 0x0'1f'06
					|| __c == 0x0'1f'07
					|| __c == 0x0'1f'10
					|| __c == 0x0'1f'11
					|| __c == 0x0'1f'12
					|| __c == 0x0'1f'13
					|| __c == 0x0'1f'14
					|| __c == 0x0'1f'15
					|| __c == 0x0'1f'20
					|| __c == 0x0'1f'21
					|| __c == 0x0'1f'22
					|| __c == 0x0'1f'23
					|| __c == 0x0'1f'24
					|| __c == 0x0'1f'25
					|| __c == 0x0'1f'26
					|| __c == 0x0'1f'27
					|| __c == 0x0'1f'30
					|| __c == 0x0'1f'31
					|| __c == 0x0'1f'32
					|| __c == 0x0'1f'33
					|| __c == 0x0'1f'34
					|| __c == 0x0'1f'35
					|| __c == 0x0'1f'36
					|| __c == 0x0'1f'37
					|| __c == 0x0'1f'40
					|| __c == 0x0'1f'41
					|| __c == 0x0'1f'42
					|| __c == 0x0'1f'43
					|| __c == 0x0'1f'44
					|| __c == 0x0'1f'45
					|| __c == 0x0'1f'51
					|| __c == 0x0'1f'53
					|| __c == 0x0'1f'55
					|| __c == 0x0'1f'57
					|| __c == 0x0'1f'60
					|| __c == 0x0'1f'61
					|| __c == 0x0'1f'62
					|| __c == 0x0'1f'63
					|| __c == 0x0'1f'64
					|| __c == 0x0'1f'65
					|| __c == 0x0'1f'66
					|| __c == 0x0'1f'67
					|| __c == 0x0'1f'70
					|| __c == 0x0'1f'71
					|| __c == 0x0'1f'72
					|| __c == 0x0'1f'73
					|| __c == 0x0'1f'74
					|| __c == 0x0'1f'75
					|| __c == 0x0'1f'76
					|| __c == 0x0'1f'77
					|| __c == 0x0'1f'78
					|| __c == 0x0'1f'79
					|| __c == 0x0'1f'7a
					|| __c == 0x0'1f'7b
					|| __c == 0x0'1f'7c
					|| __c == 0x0'1f'7d
					|| __c == 0x0'1f'80
					|| __c == 0x0'1f'81
					|| __c == 0x0'1f'82
					|| __c == 0x0'1f'83
					|| __c == 0x0'1f'84
					|| __c == 0x0'1f'85
					|| __c == 0x0'1f'86
					|| __c == 0x0'1f'87
					|| __c == 0x0'1f'90
					|| __c == 0x0'1f'91
					|| __c == 0x0'1f'92
					|| __c == 0x0'1f'93
					|| __c == 0x0'1f'94
					|| __c == 0x0'1f'95
					|| __c == 0x0'1f'96
					|| __c == 0x0'1f'97
					|| __c == 0x0'1f'a0
					|| __c == 0x0'1f'a1
					|| __c == 0x0'1f'a2
					|| __c == 0x0'1f'a3
					|| __c == 0x0'1f'a4
					|| __c == 0x0'1f'a5
					|| __c == 0x0'1f'a6
					|| __c == 0x0'1f'a7
					|| __c == 0x0'1f'b0
					|| __c == 0x0'1f'b1
					|| __c == 0x0'1f'b3
					|| __c == 0x0'1f'c3
					|| __c == 0x0'1f'd0
					|| __c == 0x0'1f'd1
					|| __c == 0x0'1f'e0
					|| __c == 0x0'1f'e1
					|| __c == 0x0'1f'e5
					|| __c == 0x0'1f'f3
					|| __c == 0x0'21'4e
					|| __c == 0x0'21'70
					|| __c == 0x0'21'71
					|| __c == 0x0'21'72
					|| __c == 0x0'21'73
					|| __c == 0x0'21'74
					|| __c == 0x0'21'75
					|| __c == 0x0'21'76
					|| __c == 0x0'21'77
					|| __c == 0x0'21'78
					|| __c == 0x0'21'79
					|| __c == 0x0'21'7a
					|| __c == 0x0'21'7b
					|| __c == 0x0'21'7c
					|| __c == 0x0'21'7d
					|| __c == 0x0'21'7e
					|| __c == 0x0'21'7f
					|| __c == 0x0'21'84
					|| __c == 0x0'24'd0
					|| __c == 0x0'24'd1
					|| __c == 0x0'24'd2
					|| __c == 0x0'24'd3
					|| __c == 0x0'24'd4
					|| __c == 0x0'24'd5
					|| __c == 0x0'24'd6
					|| __c == 0x0'24'd7
					|| __c == 0x0'24'd8
					|| __c == 0x0'24'd9
					|| __c == 0x0'24'da
					|| __c == 0x0'24'db
					|| __c == 0x0'24'dc
					|| __c == 0x0'24'dd
					|| __c == 0x0'24'de
					|| __c == 0x0'24'df
					|| __c == 0x0'24'e0
					|| __c == 0x0'24'e1
					|| __c == 0x0'24'e2
					|| __c == 0x0'24'e3
					|| __c == 0x0'24'e4
					|| __c == 0x0'24'e5
					|| __c == 0x0'24'e6
					|| __c == 0x0'24'e7
					|| __c == 0x0'24'e8
					|| __c == 0x0'24'e9
					|| __c == 0x0'2c'30
					|| __c == 0x0'2c'31
					|| __c == 0x0'2c'32
					|| __c == 0x0'2c'33
					|| __c == 0x0'2c'34
					|| __c == 0x0'2c'35
					|| __c == 0x0'2c'36
					|| __c == 0x0'2c'37
					|| __c == 0x0'2c'38
					|| __c == 0x0'2c'39
					|| __c == 0x0'2c'3a
					|| __c == 0x0'2c'3b
					|| __c == 0x0'2c'3c
					|| __c == 0x0'2c'3d
					|| __c == 0x0'2c'3e
					|| __c == 0x0'2c'3f
					|| __c == 0x0'2c'40
					|| __c == 0x0'2c'41
					|| __c == 0x0'2c'42
					|| __c == 0x0'2c'43
					|| __c == 0x0'2c'44
					|| __c == 0x0'2c'45
					|| __c == 0x0'2c'46
					|| __c == 0x0'2c'47
					|| __c == 0x0'2c'48
					|| __c == 0x0'2c'49
					|| __c == 0x0'2c'4a
					|| __c == 0x0'2c'4b
					|| __c == 0x0'2c'4c
					|| __c == 0x0'2c'4d
					|| __c == 0x0'2c'4e
					|| __c == 0x0'2c'4f
					|| __c == 0x0'2c'50
					|| __c == 0x0'2c'51
					|| __c == 0x0'2c'52
					|| __c == 0x0'2c'53
					|| __c == 0x0'2c'54
					|| __c == 0x0'2c'55
					|| __c == 0x0'2c'56
					|| __c == 0x0'2c'57
					|| __c == 0x0'2c'58
					|| __c == 0x0'2c'59
					|| __c == 0x0'2c'5a
					|| __c == 0x0'2c'5b
					|| __c == 0x0'2c'5c
					|| __c == 0x0'2c'5d
					|| __c == 0x0'2c'5e
					|| __c == 0x0'2c'61
					|| __c == 0x0'2c'65
					|| __c == 0x0'2c'66
					|| __c == 0x0'2c'68
					|| __c == 0x0'2c'6a
					|| __c == 0x0'2c'6c
					|| __c == 0x0'2c'73
					|| __c == 0x0'2c'76
					|| __c == 0x0'2c'81
					|| __c == 0x0'2c'83
					|| __c == 0x0'2c'85
					|| __c == 0x0'2c'87
					|| __c == 0x0'2c'89
					|| __c == 0x0'2c'8b
					|| __c == 0x0'2c'8d
					|| __c == 0x0'2c'8f
					|| __c == 0x0'2c'91
					|| __c == 0x0'2c'93
					|| __c == 0x0'2c'95
					|| __c == 0x0'2c'97
					|| __c == 0x0'2c'99
					|| __c == 0x0'2c'9b
					|| __c == 0x0'2c'9d
					|| __c == 0x0'2c'9f
					|| __c == 0x0'2c'a1
					|| __c == 0x0'2c'a3
					|| __c == 0x0'2c'a5
					|| __c == 0x0'2c'a7
					|| __c == 0x0'2c'a9
					|| __c == 0x0'2c'ab
					|| __c == 0x0'2c'ad
					|| __c == 0x0'2c'af
					|| __c == 0x0'2c'b1
					|| __c == 0x0'2c'b3
					|| __c == 0x0'2c'b5
					|| __c == 0x0'2c'b7
					|| __c == 0x0'2c'b9
					|| __c == 0x0'2c'bb
					|| __c == 0x0'2c'bd
					|| __c == 0x0'2c'bf
					|| __c == 0x0'2c'c1
					|| __c == 0x0'2c'c3
					|| __c == 0x0'2c'c5
					|| __c == 0x0'2c'c7
					|| __c == 0x0'2c'c9
					|| __c == 0x0'2c'cb
					|| __c == 0x0'2c'cd
					|| __c == 0x0'2c'cf
					|| __c == 0x0'2c'd1
					|| __c == 0x0'2c'd3
					|| __c == 0x0'2c'd5
					|| __c == 0x0'2c'd7
					|| __c == 0x0'2c'd9
					|| __c == 0x0'2c'db
					|| __c == 0x0'2c'dd
					|| __c == 0x0'2c'df
					|| __c == 0x0'2c'e1
					|| __c == 0x0'2c'e3
					|| __c == 0x0'2c'ec
					|| __c == 0x0'2c'ee
					|| __c == 0x0'2c'f3
					|| __c == 0x0'2d'00
					|| __c == 0x0'2d'01
					|| __c == 0x0'2d'02
					|| __c == 0x0'2d'03
					|| __c == 0x0'2d'04
					|| __c == 0x0'2d'05
					|| __c == 0x0'2d'06
					|| __c == 0x0'2d'07
					|| __c == 0x0'2d'08
					|| __c == 0x0'2d'09
					|| __c == 0x0'2d'0a
					|| __c == 0x0'2d'0b
					|| __c == 0x0'2d'0c
					|| __c == 0x0'2d'0d
					|| __c == 0x0'2d'0e
					|| __c == 0x0'2d'0f
					|| __c == 0x0'2d'10
					|| __c == 0x0'2d'11
					|| __c == 0x0'2d'12
					|| __c == 0x0'2d'13
					|| __c == 0x0'2d'14
					|| __c == 0x0'2d'15
					|| __c == 0x0'2d'16
					|| __c == 0x0'2d'17
					|| __c == 0x0'2d'18
					|| __c == 0x0'2d'19
					|| __c == 0x0'2d'1a
					|| __c == 0x0'2d'1b
					|| __c == 0x0'2d'1c
					|| __c == 0x0'2d'1d
					|| __c == 0x0'2d'1e
					|| __c == 0x0'2d'1f
					|| __c == 0x0'2d'20
					|| __c == 0x0'2d'21
					|| __c == 0x0'2d'22
					|| __c == 0x0'2d'23
					|| __c == 0x0'2d'24
					|| __c == 0x0'2d'25
					|| __c == 0x0'2d'27
					|| __c == 0x0'2d'2d
					|| __c == 0x0'a6'41
					|| __c == 0x0'a6'43
					|| __c == 0x0'a6'45
					|| __c == 0x0'a6'47
					|| __c == 0x0'a6'49
					|| __c == 0x0'a6'4b
					|| __c == 0x0'a6'4d
					|| __c == 0x0'a6'4f
					|| __c == 0x0'a6'51
					|| __c == 0x0'a6'53
					|| __c == 0x0'a6'55
					|| __c == 0x0'a6'57
					|| __c == 0x0'a6'59
					|| __c == 0x0'a6'5b
					|| __c == 0x0'a6'5d
					|| __c == 0x0'a6'5f
					|| __c == 0x0'a6'61
					|| __c == 0x0'a6'63
					|| __c == 0x0'a6'65
					|| __c == 0x0'a6'67
					|| __c == 0x0'a6'69
					|| __c == 0x0'a6'6b
					|| __c == 0x0'a6'6d
					|| __c == 0x0'a6'81
					|| __c == 0x0'a6'83
					|| __c == 0x0'a6'85
					|| __c == 0x0'a6'87
					|| __c == 0x0'a6'89
					|| __c == 0x0'a6'8b
					|| __c == 0x0'a6'8d
					|| __c == 0x0'a6'8f
					|| __c == 0x0'a6'91
					|| __c == 0x0'a6'93
					|| __c == 0x0'a6'95
					|| __c == 0x0'a6'97
					|| __c == 0x0'a6'99
					|| __c == 0x0'a6'9b
					|| __c == 0x0'a7'23
					|| __c == 0x0'a7'25
					|| __c == 0x0'a7'27
					|| __c == 0x0'a7'29
					|| __c == 0x0'a7'2b
					|| __c == 0x0'a7'2d
					|| __c == 0x0'a7'2f
					|| __c == 0x0'a7'33
					|| __c == 0x0'a7'35
					|| __c == 0x0'a7'37
					|| __c == 0x0'a7'39
					|| __c == 0x0'a7'3b
					|| __c == 0x0'a7'3d
					|| __c == 0x0'a7'3f
					|| __c == 0x0'a7'41
					|| __c == 0x0'a7'43
					|| __c == 0x0'a7'45
					|| __c == 0x0'a7'47
					|| __c == 0x0'a7'49
					|| __c == 0x0'a7'4b
					|| __c == 0x0'a7'4d
					|| __c == 0x0'a7'4f
					|| __c == 0x0'a7'51
					|| __c == 0x0'a7'53
					|| __c == 0x0'a7'55
					|| __c == 0x0'a7'57
					|| __c == 0x0'a7'59
					|| __c == 0x0'a7'5b
					|| __c == 0x0'a7'5d
					|| __c == 0x0'a7'5f
					|| __c == 0x0'a7'61
					|| __c == 0x0'a7'63
					|| __c == 0x0'a7'65
					|| __c == 0x0'a7'67
					|| __c == 0x0'a7'69
					|| __c == 0x0'a7'6b
					|| __c == 0x0'a7'6d
					|| __c == 0x0'a7'6f
					|| __c == 0x0'a7'7a
					|| __c == 0x0'a7'7c
					|| __c == 0x0'a7'7f
					|| __c == 0x0'a7'81
					|| __c == 0x0'a7'83
					|| __c == 0x0'a7'85
					|| __c == 0x0'a7'87
					|| __c == 0x0'a7'8c
					|| __c == 0x0'a7'91
					|| __c == 0x0'a7'93
					|| __c == 0x0'a7'94
					|| __c == 0x0'a7'97
					|| __c == 0x0'a7'99
					|| __c == 0x0'a7'9b
					|| __c == 0x0'a7'9d
					|| __c == 0x0'a7'9f
					|| __c == 0x0'a7'a1
					|| __c == 0x0'a7'a3
					|| __c == 0x0'a7'a5
					|| __c == 0x0'a7'a7
					|| __c == 0x0'a7'a9
					|| __c == 0x0'a7'b5
					|| __c == 0x0'a7'b7
					|| __c == 0x0'a7'b9
					|| __c == 0x0'a7'bb
					|| __c == 0x0'a7'bd
					|| __c == 0x0'a7'bf
					|| __c == 0x0'a7'c3
					|| __c == 0x0'ab'53
					|| __c == 0x0'ff'41
					|| __c == 0x0'ff'42
					|| __c == 0x0'ff'43
					|| __c == 0x0'ff'44
					|| __c == 0x0'ff'45
					|| __c == 0x0'ff'46
					|| __c == 0x0'ff'47
					|| __c == 0x0'ff'48
					|| __c == 0x0'ff'49
					|| __c == 0x0'ff'4a
					|| __c == 0x0'ff'4b
					|| __c == 0x0'ff'4c
					|| __c == 0x0'ff'4d
					|| __c == 0x0'ff'4e
					|| __c == 0x0'ff'4f
					|| __c == 0x0'ff'50
					|| __c == 0x0'ff'51
					|| __c == 0x0'ff'52
					|| __c == 0x0'ff'53
					|| __c == 0x0'ff'54
					|| __c == 0x0'ff'55
					|| __c == 0x0'ff'56
					|| __c == 0x0'ff'57
					|| __c == 0x0'ff'58
					|| __c == 0x0'ff'59
					|| __c == 0x0'ff'5a
					|| __c == 0x1'04'28
					|| __c == 0x1'04'29
					|| __c == 0x1'04'2a
					|| __c == 0x1'04'2b
					|| __c == 0x1'04'2c
					|| __c == 0x1'04'2d
					|| __c == 0x1'04'2e
					|| __c == 0x1'04'2f
					|| __c == 0x1'04'30
					|| __c == 0x1'04'31
					|| __c == 0x1'04'32
					|| __c == 0x1'04'33
					|| __c == 0x1'04'34
					|| __c == 0x1'04'35
					|| __c == 0x1'04'36
					|| __c == 0x1'04'37
					|| __c == 0x1'04'38
					|| __c == 0x1'04'39
					|| __c == 0x1'04'3a
					|| __c == 0x1'04'3b
					|| __c == 0x1'04'3c
					|| __c == 0x1'04'3d
					|| __c == 0x1'04'3e
					|| __c == 0x1'04'3f
					|| __c == 0x1'04'40
					|| __c == 0x1'04'41
					|| __c == 0x1'04'42
					|| __c == 0x1'04'43
					|| __c == 0x1'04'44
					|| __c == 0x1'04'45
					|| __c == 0x1'04'46
					|| __c == 0x1'04'47
					|| __c == 0x1'04'48
					|| __c == 0x1'04'49
					|| __c == 0x1'04'4a
					|| __c == 0x1'04'4b
					|| __c == 0x1'04'4c
					|| __c == 0x1'04'4d
					|| __c == 0x1'04'4e
					|| __c == 0x1'04'4f
					|| __c == 0x1'04'd8
					|| __c == 0x1'04'd9
					|| __c == 0x1'04'da
					|| __c == 0x1'04'db
					|| __c == 0x1'04'dc
					|| __c == 0x1'04'dd
					|| __c == 0x1'04'de
					|| __c == 0x1'04'df
					|| __c == 0x1'04'e0
					|| __c == 0x1'04'e1
					|| __c == 0x1'04'e2
					|| __c == 0x1'04'e3
					|| __c == 0x1'04'e4
					|| __c == 0x1'04'e5
					|| __c == 0x1'04'e6
					|| __c == 0x1'04'e7
					|| __c == 0x1'04'e8
					|| __c == 0x1'04'e9
					|| __c == 0x1'04'ea
					|| __c == 0x1'04'eb
					|| __c == 0x1'04'ec
					|| __c == 0x1'04'ed
					|| __c == 0x1'04'ee
					|| __c == 0x1'04'ef
					|| __c == 0x1'04'f0
					|| __c == 0x1'04'f1
					|| __c == 0x1'04'f2
					|| __c == 0x1'04'f3
					|| __c == 0x1'04'f4
					|| __c == 0x1'04'f5
					|| __c == 0x1'04'f6
					|| __c == 0x1'04'f7
					|| __c == 0x1'04'f8
					|| __c == 0x1'04'f9
					|| __c == 0x1'04'fa
					|| __c == 0x1'04'fb
					|| __c == 0x1'0c'c0
					|| __c == 0x1'0c'c1
					|| __c == 0x1'0c'c2
					|| __c == 0x1'0c'c3
					|| __c == 0x1'0c'c4
					|| __c == 0x1'0c'c5
					|| __c == 0x1'0c'c6
					|| __c == 0x1'0c'c7
					|| __c == 0x1'0c'c8
					|| __c == 0x1'0c'c9
					|| __c == 0x1'0c'ca
					|| __c == 0x1'0c'cb
					|| __c == 0x1'0c'cc
					|| __c == 0x1'0c'cd
					|| __c == 0x1'0c'ce
					|| __c == 0x1'0c'cf
					|| __c == 0x1'0c'd0
					|| __c == 0x1'0c'd1
					|| __c == 0x1'0c'd2
					|| __c == 0x1'0c'd3
					|| __c == 0x1'0c'd4
					|| __c == 0x1'0c'd5
					|| __c == 0x1'0c'd6
					|| __c == 0x1'0c'd7
					|| __c == 0x1'0c'd8
					|| __c == 0x1'0c'd9
					|| __c == 0x1'0c'da
					|| __c == 0x1'0c'db
					|| __c == 0x1'0c'dc
					|| __c == 0x1'0c'dd
					|| __c == 0x1'0c'de
					|| __c == 0x1'0c'df
					|| __c == 0x1'0c'e0
					|| __c == 0x1'0c'e1
					|| __c == 0x1'0c'e2
					|| __c == 0x1'0c'e3
					|| __c == 0x1'0c'e4
					|| __c == 0x1'0c'e5
					|| __c == 0x1'0c'e6
					|| __c == 0x1'0c'e7
					|| __c == 0x1'0c'e8
					|| __c == 0x1'0c'e9
					|| __c == 0x1'0c'ea
					|| __c == 0x1'0c'eb
					|| __c == 0x1'0c'ec
					|| __c == 0x1'0c'ed
					|| __c == 0x1'0c'ee
					|| __c == 0x1'0c'ef
					|| __c == 0x1'0c'f0
					|| __c == 0x1'0c'f1
					|| __c == 0x1'0c'f2
					|| __c == 0x1'18'c0
					|| __c == 0x1'18'c1
					|| __c == 0x1'18'c2
					|| __c == 0x1'18'c3
					|| __c == 0x1'18'c4
					|| __c == 0x1'18'c5
					|| __c == 0x1'18'c6
					|| __c == 0x1'18'c7
					|| __c == 0x1'18'c8
					|| __c == 0x1'18'c9
					|| __c == 0x1'18'ca
					|| __c == 0x1'18'cb
					|| __c == 0x1'18'cc
					|| __c == 0x1'18'cd
					|| __c == 0x1'18'ce
					|| __c == 0x1'18'cf
					|| __c == 0x1'18'd0
					|| __c == 0x1'18'd1
					|| __c == 0x1'18'd2
					|| __c == 0x1'18'd3
					|| __c == 0x1'18'd4
					|| __c == 0x1'18'd5
					|| __c == 0x1'18'd6
					|| __c == 0x1'18'd7
					|| __c == 0x1'18'd8
					|| __c == 0x1'18'd9
					|| __c == 0x1'18'da
					|| __c == 0x1'18'db
					|| __c == 0x1'18'dc
					|| __c == 0x1'18'dd
					|| __c == 0x1'18'de
					|| __c == 0x1'18'df
					|| __c == 0x1'6e'60
					|| __c == 0x1'6e'61
					|| __c == 0x1'6e'62
					|| __c == 0x1'6e'63
					|| __c == 0x1'6e'64
					|| __c == 0x1'6e'65
					|| __c == 0x1'6e'66
					|| __c == 0x1'6e'67
					|| __c == 0x1'6e'68
					|| __c == 0x1'6e'69
					|| __c == 0x1'6e'6a
					|| __c == 0x1'6e'6b
					|| __c == 0x1'6e'6c
					|| __c == 0x1'6e'6d
					|| __c == 0x1'6e'6e
					|| __c == 0x1'6e'6f
					|| __c == 0x1'6e'70
					|| __c == 0x1'6e'71
					|| __c == 0x1'6e'72
					|| __c == 0x1'6e'73
					|| __c == 0x1'6e'74
					|| __c == 0x1'6e'75
					|| __c == 0x1'6e'76
					|| __c == 0x1'6e'77
					|| __c == 0x1'6e'78
					|| __c == 0x1'6e'79
					|| __c == 0x1'6e'7a
					|| __c == 0x1'6e'7b
					|| __c == 0x1'6e'7c
					|| __c == 0x1'6e'7d
					|| __c == 0x1'6e'7e
					|| __c == 0x1'6e'7f
					|| __c == 0x1'e9'22
					|| __c == 0x1'e9'23
					|| __c == 0x1'e9'24
					|| __c == 0x1'e9'25
					|| __c == 0x1'e9'26
					|| __c == 0x1'e9'27
					|| __c == 0x1'e9'28
					|| __c == 0x1'e9'29
					|| __c == 0x1'e9'2a
					|| __c == 0x1'e9'2b
					|| __c == 0x1'e9'2c
					|| __c == 0x1'e9'2d
					|| __c == 0x1'e9'2e
					|| __c == 0x1'e9'2f
					|| __c == 0x1'e9'30
					|| __c == 0x1'e9'31
					|| __c == 0x1'e9'32
					|| __c == 0x1'e9'33
					|| __c == 0x1'e9'34
					|| __c == 0x1'e9'35
					|| __c == 0x1'e9'36
					|| __c == 0x1'e9'37
					|| __c == 0x1'e9'38
					|| __c == 0x1'e9'39
					|| __c == 0x1'e9'3a
					|| __c == 0x1'e9'3b
					|| __c == 0x1'e9'3c
					|| __c == 0x1'e9'3d
					|| __c == 0x1'e9'3e
					|| __c == 0x1'e9'3f
					|| __c == 0x1'e9'40
					|| __c == 0x1'e9'41
					|| __c == 0x1'e9'42
					|| __c == 0x1'e9'43;
		}
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr bool
		is_print(
		 u8code_t __c) pf_attr_noexcept
		{
			return !is_cntrl(__c);
		}
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr bool
		is_decimal(
		 u8code_t __c) pf_attr_noexcept
		{
			return (__c >= 0x0'00'30 && __c <= 0x0'00'39)
					|| (__c >= 0x0'06'60 && __c <= 0x0'06'69)
					|| (__c >= 0x0'06'F0 && __c <= 0x0'06'F9)
					|| (__c >= 0x0'07'C0 && __c <= 0x0'07'C9)
					|| (__c >= 0x0'09'66 && __c <= 0x0'09'6F)
					|| (__c >= 0x0'09'E6 && __c <= 0x0'09'EF)
					|| (__c >= 0x0'0A'66 && __c <= 0x0'0A'6F)
					|| (__c >= 0x0'0A'E6 && __c <= 0x0'0A'EF)
					|| (__c >= 0x0'0B'66 && __c <= 0x0'0B'6F)
					|| (__c >= 0x0'0B'E6 && __c <= 0x0'0B'EF)
					|| (__c >= 0x0'0C'66 && __c <= 0x0'0C'6F)
					|| (__c >= 0x0'0C'E6 && __c <= 0x0'0C'EF)
					|| (__c >= 0x0'0D'66 && __c <= 0x0'0D'EF)
					|| (__c >= 0x0'0E'50 && __c <= 0x0'0E'59)
					|| (__c >= 0x0'0E'D0 && __c <= 0x0'0E'D9)
					|| (__c >= 0x0'0F'20 && __c <= 0x0'0F'29)
					|| (__c >= 0x0'10'40 && __c <= 0x0'10'49)
					|| (__c >= 0x0'10'90 && __c <= 0x0'10'99)
					|| (__c >= 0x0'17'E0 && __c <= 0x0'17'E9)
					|| (__c >= 0x0'18'10 && __c <= 0x0'18'19)
					|| (__c >= 0x0'19'46 && __c <= 0x0'19'4F)
					|| (__c >= 0x0'19'D0 && __c <= 0x0'19'D9)
					|| (__c >= 0x0'1A'80 && __c <= 0x0'1A'89)
					|| (__c >= 0x0'1A'90 && __c <= 0x0'1A'99)
					|| (__c >= 0x0'1B'50 && __c <= 0x0'1B'59)
					|| (__c >= 0x0'1B'B0 && __c <= 0x0'1B'B9)
					|| (__c >= 0x0'1C'40 && __c <= 0x0'1C'59)
					|| (__c >= 0x0'A6'20 && __c <= 0x0'A6'29)
					|| (__c >= 0x0'A8'D0 && __c <= 0x0'A8'D9)
					|| (__c >= 0x0'A9'00 && __c <= 0x0'A9'09)
					|| (__c >= 0x0'A9'D0 && __c <= 0x0'A9'D9)
					|| (__c >= 0x0'A9'F0 && __c <= 0x0'A9'F9)
					|| (__c >= 0x0'AA'50 && __c <= 0x0'AA'59)
					|| (__c >= 0x0'AB'F0 && __c <= 0x0'AB'F9)
					|| (__c >= 0x0'FF'10 && __c <= 0x0'FF'19)
					|| (__c >= 0x1'04'A0 && __c <= 0x1'04'A9)
					|| (__c >= 0x1'0D'30 && __c <= 0x1'0D'39)
					|| (__c >= 0x1'10'66 && __c <= 0x1'10'6F)
					|| (__c >= 0x1'10'F0 && __c <= 0x1'10'F9)
					|| (__c >= 0x1'11'36 && __c <= 0x1'11'3F)
					|| (__c >= 0x1'11'D0 && __c <= 0x1'11'D9)
					|| (__c >= 0x1'12'F0 && __c <= 0x1'12'F9)
					|| (__c >= 0x1'14'50 && __c <= 0x1'14'59)
					|| (__c >= 0x1'14'D0 && __c <= 0x1'14'D9)
					|| (__c >= 0x1'16'55 && __c <= 0x1'16'59)
					|| (__c >= 0x1'16'C0 && __c <= 0x1'16'C9)
					|| (__c >= 0x1'17'30 && __c <= 0x1'17'39)
					|| (__c >= 0x1'18'E0 && __c <= 0x1'18'E9)
					|| (__c >= 0x1'19'50 && __c <= 0x1'19'59)
					|| (__c >= 0x1'1C'50 && __c <= 0x1'1C'59)
					|| (__c >= 0x1'1D'50 && __c <= 0x1'1D'59)
					|| (__c >= 0x1'1D'A0 && __c <= 0x1'1D'A9)
					|| (__c >= 0x1'6A'60 && __c <= 0x1'6A'69)
					|| (__c >= 0x1'6B'50 && __c <= 0x1'6B'59)
					|| (__c >= 0x1'D7'CE && __c <= 0x1'D7'FF)
					|| (__c >= 0x1'E1'40 && __c <= 0x1'E1'49)
					|| (__c >= 0x1'E2'F0 && __c <= 0x1'E2'F9)
					|| (__c >= 0x1'E9'50 && __c <= 0x1'E9'59)
					|| (__c >= 0x1'FB'F0 && __c <= 0x1'FB'F9);
		}

		/// UTF8: To*
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr u8code_t
		to_lower(
		 u8code_t __c) pf_attr_noexcept
		{
			switch(__c)
			{
				case 0x0'00'41: return 0x0'00'61;
				case 0x0'00'42: return 0x0'00'62;
				case 0x0'00'43: return 0x0'00'63;
				case 0x0'00'44: return 0x0'00'64;
				case 0x0'00'45: return 0x0'00'65;
				case 0x0'00'46: return 0x0'00'66;
				case 0x0'00'47: return 0x0'00'67;
				case 0x0'00'48: return 0x0'00'68;
				case 0x0'00'49: return 0x0'00'69;
				case 0x0'00'4a: return 0x0'00'6a;
				case 0x0'00'4b: return 0x0'00'6b;
				case 0x0'00'4c: return 0x0'00'6c;
				case 0x0'00'4d: return 0x0'00'6d;
				case 0x0'00'4e: return 0x0'00'6e;
				case 0x0'00'4f: return 0x0'00'6f;
				case 0x0'00'50: return 0x0'00'70;
				case 0x0'00'51: return 0x0'00'71;
				case 0x0'00'52: return 0x0'00'72;
				case 0x0'00'53: return 0x0'00'73;
				case 0x0'00'54: return 0x0'00'74;
				case 0x0'00'55: return 0x0'00'75;
				case 0x0'00'56: return 0x0'00'76;
				case 0x0'00'57: return 0x0'00'77;
				case 0x0'00'58: return 0x0'00'78;
				case 0x0'00'59: return 0x0'00'79;
				case 0x0'00'5a: return 0x0'00'7a;
				case 0x0'1e'9e: return 0x0'00'df;
				case 0x0'00'c0: return 0x0'00'e0;
				case 0x0'00'c1: return 0x0'00'e1;
				case 0x0'00'c2: return 0x0'00'e2;
				case 0x0'00'c3: return 0x0'00'e3;
				case 0x0'00'c4: return 0x0'00'e4;
				case 0x0'00'c5: return 0x0'00'e5;
				case 0x0'00'c6: return 0x0'00'e6;
				case 0x0'00'c7: return 0x0'00'e7;
				case 0x0'00'c8: return 0x0'00'e8;
				case 0x0'00'c9: return 0x0'00'e9;
				case 0x0'00'ca: return 0x0'00'ea;
				case 0x0'00'cb: return 0x0'00'eb;
				case 0x0'00'cc: return 0x0'00'ec;
				case 0x0'00'cd: return 0x0'00'ed;
				case 0x0'00'ce: return 0x0'00'ee;
				case 0x0'00'cf: return 0x0'00'ef;
				case 0x0'00'd0: return 0x0'00'f0;
				case 0x0'00'd1: return 0x0'00'f1;
				case 0x0'00'd2: return 0x0'00'f2;
				case 0x0'00'd3: return 0x0'00'f3;
				case 0x0'00'd4: return 0x0'00'f4;
				case 0x0'00'd5: return 0x0'00'f5;
				case 0x0'00'd6: return 0x0'00'f6;
				case 0x0'00'd8: return 0x0'00'f8;
				case 0x0'00'd9: return 0x0'00'f9;
				case 0x0'00'da: return 0x0'00'fa;
				case 0x0'00'db: return 0x0'00'fb;
				case 0x0'00'dc: return 0x0'00'fc;
				case 0x0'00'dd: return 0x0'00'fd;
				case 0x0'00'de: return 0x0'00'fe;
				case 0x0'01'78: return 0x0'00'ff;
				case 0x0'01'00: return 0x0'01'01;
				case 0x0'01'02: return 0x0'01'03;
				case 0x0'01'04: return 0x0'01'05;
				case 0x0'01'06: return 0x0'01'07;
				case 0x0'01'08: return 0x0'01'09;
				case 0x0'01'0a: return 0x0'01'0b;
				case 0x0'01'0c: return 0x0'01'0d;
				case 0x0'01'0e: return 0x0'01'0f;
				case 0x0'01'10: return 0x0'01'11;
				case 0x0'01'12: return 0x0'01'13;
				case 0x0'01'14: return 0x0'01'15;
				case 0x0'01'16: return 0x0'01'17;
				case 0x0'01'18: return 0x0'01'19;
				case 0x0'01'1a: return 0x0'01'1b;
				case 0x0'01'1c: return 0x0'01'1d;
				case 0x0'01'1e: return 0x0'01'1f;
				case 0x0'01'20: return 0x0'01'21;
				case 0x0'01'22: return 0x0'01'23;
				case 0x0'01'24: return 0x0'01'25;
				case 0x0'01'26: return 0x0'01'27;
				case 0x0'01'28: return 0x0'01'29;
				case 0x0'01'2a: return 0x0'01'2b;
				case 0x0'01'2c: return 0x0'01'2d;
				case 0x0'01'2e: return 0x0'01'2f;
				case 0x0'01'32: return 0x0'01'33;
				case 0x0'01'34: return 0x0'01'35;
				case 0x0'01'36: return 0x0'01'37;
				case 0x0'01'39: return 0x0'01'3a;
				case 0x0'01'3b: return 0x0'01'3c;
				case 0x0'01'3d: return 0x0'01'3e;
				case 0x0'01'3f: return 0x0'01'40;
				case 0x0'01'41: return 0x0'01'42;
				case 0x0'01'43: return 0x0'01'44;
				case 0x0'01'45: return 0x0'01'46;
				case 0x0'01'47: return 0x0'01'48;
				case 0x0'01'4a: return 0x0'01'4b;
				case 0x0'01'4c: return 0x0'01'4d;
				case 0x0'01'4e: return 0x0'01'4f;
				case 0x0'01'50: return 0x0'01'51;
				case 0x0'01'52: return 0x0'01'53;
				case 0x0'01'54: return 0x0'01'55;
				case 0x0'01'56: return 0x0'01'57;
				case 0x0'01'58: return 0x0'01'59;
				case 0x0'01'5a: return 0x0'01'5b;
				case 0x0'01'5c: return 0x0'01'5d;
				case 0x0'01'5e: return 0x0'01'5f;
				case 0x0'01'60: return 0x0'01'61;
				case 0x0'01'62: return 0x0'01'63;
				case 0x0'01'64: return 0x0'01'65;
				case 0x0'01'66: return 0x0'01'67;
				case 0x0'01'68: return 0x0'01'69;
				case 0x0'01'6a: return 0x0'01'6b;
				case 0x0'01'6c: return 0x0'01'6d;
				case 0x0'01'6e: return 0x0'01'6f;
				case 0x0'01'70: return 0x0'01'71;
				case 0x0'01'72: return 0x0'01'73;
				case 0x0'01'74: return 0x0'01'75;
				case 0x0'01'76: return 0x0'01'77;
				case 0x0'01'79: return 0x0'01'7a;
				case 0x0'01'7b: return 0x0'01'7c;
				case 0x0'01'7d: return 0x0'01'7e;
				case 0x0'02'43: return 0x0'01'80;
				case 0x0'01'82: return 0x0'01'83;
				case 0x0'01'84: return 0x0'01'85;
				case 0x0'01'87: return 0x0'01'88;
				case 0x0'01'8b: return 0x0'01'8c;
				case 0x0'01'91: return 0x0'01'92;
				case 0x0'01'f6: return 0x0'01'95;
				case 0x0'01'98: return 0x0'01'99;
				case 0x0'02'3d: return 0x0'01'9a;
				case 0x0'02'20: return 0x0'01'9e;
				case 0x0'01'a0: return 0x0'01'a1;
				case 0x0'01'a2: return 0x0'01'a3;
				case 0x0'01'a4: return 0x0'01'a5;
				case 0x0'01'a7: return 0x0'01'a8;
				case 0x0'01'ac: return 0x0'01'ad;
				case 0x0'01'af: return 0x0'01'b0;
				case 0x0'01'b3: return 0x0'01'b4;
				case 0x0'01'b5: return 0x0'01'b6;
				case 0x0'01'b8: return 0x0'01'b9;
				case 0x0'01'bc: return 0x0'01'bd;
				case 0x0'01'f7: return 0x0'01'bf;
				case 0x0'01'c4: return 0x0'01'c6;
				case 0x0'01'c7: return 0x0'01'c9;
				case 0x0'01'ca: return 0x0'01'cc;
				case 0x0'01'cd: return 0x0'01'ce;
				case 0x0'01'cf: return 0x0'01'd0;
				case 0x0'01'd1: return 0x0'01'd2;
				case 0x0'01'd3: return 0x0'01'd4;
				case 0x0'01'd5: return 0x0'01'd6;
				case 0x0'01'd7: return 0x0'01'd8;
				case 0x0'01'd9: return 0x0'01'da;
				case 0x0'01'db: return 0x0'01'dc;
				case 0x0'01'8e: return 0x0'01'dd;
				case 0x0'01'de: return 0x0'01'df;
				case 0x0'01'e0: return 0x0'01'e1;
				case 0x0'01'e2: return 0x0'01'e3;
				case 0x0'01'e4: return 0x0'01'e5;
				case 0x0'01'e6: return 0x0'01'e7;
				case 0x0'01'e8: return 0x0'01'e9;
				case 0x0'01'ea: return 0x0'01'eb;
				case 0x0'01'ec: return 0x0'01'ed;
				case 0x0'01'ee: return 0x0'01'ef;
				case 0x0'01'f1: return 0x0'01'f3;
				case 0x0'01'f4: return 0x0'01'f5;
				case 0x0'01'f8: return 0x0'01'f9;
				case 0x0'01'fa: return 0x0'01'fb;
				case 0x0'01'fc: return 0x0'01'fd;
				case 0x0'01'fe: return 0x0'01'ff;
				case 0x0'02'00: return 0x0'02'01;
				case 0x0'02'02: return 0x0'02'03;
				case 0x0'02'04: return 0x0'02'05;
				case 0x0'02'06: return 0x0'02'07;
				case 0x0'02'08: return 0x0'02'09;
				case 0x0'02'0a: return 0x0'02'0b;
				case 0x0'02'0c: return 0x0'02'0d;
				case 0x0'02'0e: return 0x0'02'0f;
				case 0x0'02'10: return 0x0'02'11;
				case 0x0'02'12: return 0x0'02'13;
				case 0x0'02'14: return 0x0'02'15;
				case 0x0'02'16: return 0x0'02'17;
				case 0x0'02'18: return 0x0'02'19;
				case 0x0'02'1a: return 0x0'02'1b;
				case 0x0'02'1c: return 0x0'02'1d;
				case 0x0'02'1e: return 0x0'02'1f;
				case 0x0'02'22: return 0x0'02'23;
				case 0x0'02'24: return 0x0'02'25;
				case 0x0'02'26: return 0x0'02'27;
				case 0x0'02'28: return 0x0'02'29;
				case 0x0'02'2a: return 0x0'02'2b;
				case 0x0'02'2c: return 0x0'02'2d;
				case 0x0'02'2e: return 0x0'02'2f;
				case 0x0'02'30: return 0x0'02'31;
				case 0x0'02'32: return 0x0'02'33;
				case 0x0'02'3b: return 0x0'02'3c;
				case 0x0'2c'7e: return 0x0'02'3f;
				case 0x0'2c'7f: return 0x0'02'40;
				case 0x0'02'41: return 0x0'02'42;
				case 0x0'02'46: return 0x0'02'47;
				case 0x0'02'48: return 0x0'02'49;
				case 0x0'02'4a: return 0x0'02'4b;
				case 0x0'02'4c: return 0x0'02'4d;
				case 0x0'02'4e: return 0x0'02'4f;
				case 0x0'2c'6f: return 0x0'02'50;
				case 0x0'2c'6d: return 0x0'02'51;
				case 0x0'2c'70: return 0x0'02'52;
				case 0x0'01'81: return 0x0'02'53;
				case 0x0'01'86: return 0x0'02'54;
				case 0x0'01'89: return 0x0'02'56;
				case 0x0'01'8a: return 0x0'02'57;
				case 0x0'01'8f: return 0x0'02'59;
				case 0x0'01'90: return 0x0'02'5b;
				case 0x0'a7'ab: return 0x0'02'5c;
				case 0x0'01'93: return 0x0'02'60;
				case 0x0'a7'ac: return 0x0'02'61;
				case 0x0'01'94: return 0x0'02'63;
				case 0x0'a7'8d: return 0x0'02'65;
				case 0x0'a7'aa: return 0x0'02'66;
				case 0x0'01'97: return 0x0'02'68;
				case 0x0'01'96: return 0x0'02'69;
				case 0x0'a7'ae: return 0x0'02'6a;
				case 0x0'2c'62: return 0x0'02'6b;
				case 0x0'a7'ad: return 0x0'02'6c;
				case 0x0'01'9c: return 0x0'02'6f;
				case 0x0'2c'6e: return 0x0'02'71;
				case 0x0'01'9d: return 0x0'02'72;
				case 0x0'01'9f: return 0x0'02'75;
				case 0x0'2c'64: return 0x0'02'7d;
				case 0x0'01'a6: return 0x0'02'80;
				case 0x0'a7'c5: return 0x0'02'82;
				case 0x0'01'a9: return 0x0'02'83;
				case 0x0'a7'b1: return 0x0'02'87;
				case 0x0'01'ae: return 0x0'02'88;
				case 0x0'02'44: return 0x0'02'89;
				case 0x0'01'b1: return 0x0'02'8a;
				case 0x0'01'b2: return 0x0'02'8b;
				case 0x0'02'45: return 0x0'02'8c;
				case 0x0'01'b7: return 0x0'02'92;
				case 0x0'a7'b2: return 0x0'02'9d;
				case 0x0'a7'b0: return 0x0'02'9e;
				case 0x0'03'70: return 0x0'03'71;
				case 0x0'03'72: return 0x0'03'73;
				case 0x0'03'76: return 0x0'03'77;
				case 0x0'03'fd: return 0x0'03'7b;
				case 0x0'03'fe: return 0x0'03'7c;
				case 0x0'03'ff: return 0x0'03'7d;
				case 0x0'03'86: return 0x0'03'ac;
				case 0x0'03'88: return 0x0'03'ad;
				case 0x0'03'89: return 0x0'03'ae;
				case 0x0'03'8a: return 0x0'03'af;
				case 0x0'03'91: return 0x0'03'b1;
				case 0x0'03'92: return 0x0'03'b2;
				case 0x0'03'93: return 0x0'03'b3;
				case 0x0'03'94: return 0x0'03'b4;
				case 0x0'03'95: return 0x0'03'b5;
				case 0x0'03'96: return 0x0'03'b6;
				case 0x0'03'97: return 0x0'03'b7;
				case 0x0'03'98: return 0x0'03'b8;
				case 0x0'03'f4: return 0x0'03'b8;
				case 0x0'03'45: return 0x0'03'b9;
				case 0x0'1f'be: return 0x0'03'b9;
				case 0x0'03'9a: return 0x0'03'ba;
				case 0x0'03'9b: return 0x0'03'bb;
				case 0x0'03'9c: return 0x0'03'bc;
				case 0x0'03'9d: return 0x0'03'bd;
				case 0x0'03'9e: return 0x0'03'be;
				case 0x0'03'9f: return 0x0'03'bf;
				case 0x0'03'd6: return 0x0'03'c0;
				case 0x0'03'f1: return 0x0'03'c1;
				case 0x0'03'a3: return 0x0'03'c3;
				case 0x0'03'a4: return 0x0'03'c4;
				case 0x0'03'a5: return 0x0'03'c5;
				case 0x0'03'a6: return 0x0'03'c6;
				case 0x0'03'a7: return 0x0'03'c7;
				case 0x0'03'a8: return 0x0'03'c8;
				case 0x0'21'26: return 0x0'03'c9;
				case 0x0'03'aa: return 0x0'03'ca;
				case 0x0'03'ab: return 0x0'03'cb;
				case 0x0'03'8c: return 0x0'03'cc;
				case 0x0'03'8e: return 0x0'03'cd;
				case 0x0'03'8f: return 0x0'03'ce;
				case 0x0'03'cf: return 0x0'03'd7;
				case 0x0'03'd8: return 0x0'03'd9;
				case 0x0'03'da: return 0x0'03'db;
				case 0x0'03'dc: return 0x0'03'dd;
				case 0x0'03'de: return 0x0'03'df;
				case 0x0'03'e0: return 0x0'03'e1;
				case 0x0'03'e2: return 0x0'03'e3;
				case 0x0'03'e4: return 0x0'03'e5;
				case 0x0'03'e6: return 0x0'03'e7;
				case 0x0'03'e8: return 0x0'03'e9;
				case 0x0'03'ea: return 0x0'03'eb;
				case 0x0'03'ec: return 0x0'03'ed;
				case 0x0'03'ee: return 0x0'03'ef;
				case 0x0'03'f9: return 0x0'03'f2;
				case 0x0'03'7f: return 0x0'03'f3;
				case 0x0'03'f7: return 0x0'03'f8;
				case 0x0'03'fa: return 0x0'03'fb;
				case 0x0'04'10: return 0x0'04'30;
				case 0x0'04'11: return 0x0'04'31;
				case 0x0'1c'80: return 0x0'04'32;
				case 0x0'04'13: return 0x0'04'33;
				case 0x0'04'14: return 0x0'04'34;
				case 0x0'04'15: return 0x0'04'35;
				case 0x0'04'16: return 0x0'04'36;
				case 0x0'04'17: return 0x0'04'37;
				case 0x0'04'18: return 0x0'04'38;
				case 0x0'04'19: return 0x0'04'39;
				case 0x0'04'1a: return 0x0'04'3a;
				case 0x0'04'1b: return 0x0'04'3b;
				case 0x0'04'1c: return 0x0'04'3c;
				case 0x0'04'1d: return 0x0'04'3d;
				case 0x0'04'1e: return 0x0'04'3e;
				case 0x0'04'1f: return 0x0'04'3f;
				case 0x0'04'20: return 0x0'04'40;
				case 0x0'04'21: return 0x0'04'41;
				case 0x0'04'22: return 0x0'04'42;
				case 0x0'1c'85: return 0x0'04'42;
				case 0x0'04'23: return 0x0'04'43;
				case 0x0'04'24: return 0x0'04'44;
				case 0x0'04'25: return 0x0'04'45;
				case 0x0'04'26: return 0x0'04'46;
				case 0x0'04'27: return 0x0'04'47;
				case 0x0'04'28: return 0x0'04'48;
				case 0x0'04'29: return 0x0'04'49;
				case 0x0'1c'86: return 0x0'04'4a;
				case 0x0'04'2b: return 0x0'04'4b;
				case 0x0'04'2c: return 0x0'04'4c;
				case 0x0'04'2d: return 0x0'04'4d;
				case 0x0'04'2e: return 0x0'04'4e;
				case 0x0'04'2f: return 0x0'04'4f;
				case 0x0'04'00: return 0x0'04'50;
				case 0x0'04'01: return 0x0'04'51;
				case 0x0'04'02: return 0x0'04'52;
				case 0x0'04'03: return 0x0'04'53;
				case 0x0'04'04: return 0x0'04'54;
				case 0x0'04'05: return 0x0'04'55;
				case 0x0'04'06: return 0x0'04'56;
				case 0x0'04'07: return 0x0'04'57;
				case 0x0'04'08: return 0x0'04'58;
				case 0x0'04'09: return 0x0'04'59;
				case 0x0'04'0a: return 0x0'04'5a;
				case 0x0'04'0b: return 0x0'04'5b;
				case 0x0'04'0c: return 0x0'04'5c;
				case 0x0'04'0d: return 0x0'04'5d;
				case 0x0'04'0e: return 0x0'04'5e;
				case 0x0'04'0f: return 0x0'04'5f;
				case 0x0'04'60: return 0x0'04'61;
				case 0x0'04'62: return 0x0'04'63;
				case 0x0'04'64: return 0x0'04'65;
				case 0x0'04'66: return 0x0'04'67;
				case 0x0'04'68: return 0x0'04'69;
				case 0x0'04'6a: return 0x0'04'6b;
				case 0x0'04'6c: return 0x0'04'6d;
				case 0x0'04'6e: return 0x0'04'6f;
				case 0x0'04'70: return 0x0'04'71;
				case 0x0'04'72: return 0x0'04'73;
				case 0x0'04'74: return 0x0'04'75;
				case 0x0'04'76: return 0x0'04'77;
				case 0x0'04'78: return 0x0'04'79;
				case 0x0'04'7a: return 0x0'04'7b;
				case 0x0'04'7c: return 0x0'04'7d;
				case 0x0'04'7e: return 0x0'04'7f;
				case 0x0'04'80: return 0x0'04'81;
				case 0x0'04'8a: return 0x0'04'8b;
				case 0x0'04'8c: return 0x0'04'8d;
				case 0x0'04'8e: return 0x0'04'8f;
				case 0x0'04'90: return 0x0'04'91;
				case 0x0'04'92: return 0x0'04'93;
				case 0x0'04'94: return 0x0'04'95;
				case 0x0'04'96: return 0x0'04'97;
				case 0x0'04'98: return 0x0'04'99;
				case 0x0'04'9a: return 0x0'04'9b;
				case 0x0'04'9c: return 0x0'04'9d;
				case 0x0'04'9e: return 0x0'04'9f;
				case 0x0'04'a0: return 0x0'04'a1;
				case 0x0'04'a2: return 0x0'04'a3;
				case 0x0'04'a4: return 0x0'04'a5;
				case 0x0'04'a6: return 0x0'04'a7;
				case 0x0'04'a8: return 0x0'04'a9;
				case 0x0'04'aa: return 0x0'04'ab;
				case 0x0'04'ac: return 0x0'04'ad;
				case 0x0'04'ae: return 0x0'04'af;
				case 0x0'04'b0: return 0x0'04'b1;
				case 0x0'04'b2: return 0x0'04'b3;
				case 0x0'04'b4: return 0x0'04'b5;
				case 0x0'04'b6: return 0x0'04'b7;
				case 0x0'04'b8: return 0x0'04'b9;
				case 0x0'04'ba: return 0x0'04'bb;
				case 0x0'04'bc: return 0x0'04'bd;
				case 0x0'04'be: return 0x0'04'bf;
				case 0x0'04'c1: return 0x0'04'c2;
				case 0x0'04'c3: return 0x0'04'c4;
				case 0x0'04'c5: return 0x0'04'c6;
				case 0x0'04'c7: return 0x0'04'c8;
				case 0x0'04'c9: return 0x0'04'ca;
				case 0x0'04'cb: return 0x0'04'cc;
				case 0x0'04'cd: return 0x0'04'ce;
				case 0x0'04'c0: return 0x0'04'cf;
				case 0x0'04'd0: return 0x0'04'd1;
				case 0x0'04'd2: return 0x0'04'd3;
				case 0x0'04'd4: return 0x0'04'd5;
				case 0x0'04'd6: return 0x0'04'd7;
				case 0x0'04'd8: return 0x0'04'd9;
				case 0x0'04'da: return 0x0'04'db;
				case 0x0'04'dc: return 0x0'04'dd;
				case 0x0'04'de: return 0x0'04'df;
				case 0x0'04'e0: return 0x0'04'e1;
				case 0x0'04'e2: return 0x0'04'e3;
				case 0x0'04'e4: return 0x0'04'e5;
				case 0x0'04'e6: return 0x0'04'e7;
				case 0x0'04'e8: return 0x0'04'e9;
				case 0x0'04'ea: return 0x0'04'eb;
				case 0x0'04'ec: return 0x0'04'ed;
				case 0x0'04'ee: return 0x0'04'ef;
				case 0x0'04'f0: return 0x0'04'f1;
				case 0x0'04'f2: return 0x0'04'f3;
				case 0x0'04'f4: return 0x0'04'f5;
				case 0x0'04'f6: return 0x0'04'f7;
				case 0x0'04'f8: return 0x0'04'f9;
				case 0x0'04'fa: return 0x0'04'fb;
				case 0x0'04'fc: return 0x0'04'fd;
				case 0x0'04'fe: return 0x0'04'ff;
				case 0x0'05'00: return 0x0'05'01;
				case 0x0'05'02: return 0x0'05'03;
				case 0x0'05'04: return 0x0'05'05;
				case 0x0'05'06: return 0x0'05'07;
				case 0x0'05'08: return 0x0'05'09;
				case 0x0'05'0a: return 0x0'05'0b;
				case 0x0'05'0c: return 0x0'05'0d;
				case 0x0'05'0e: return 0x0'05'0f;
				case 0x0'05'10: return 0x0'05'11;
				case 0x0'05'12: return 0x0'05'13;
				case 0x0'05'14: return 0x0'05'15;
				case 0x0'05'16: return 0x0'05'17;
				case 0x0'05'18: return 0x0'05'19;
				case 0x0'05'1a: return 0x0'05'1b;
				case 0x0'05'1c: return 0x0'05'1d;
				case 0x0'05'1e: return 0x0'05'1f;
				case 0x0'05'20: return 0x0'05'21;
				case 0x0'05'22: return 0x0'05'23;
				case 0x0'05'24: return 0x0'05'25;
				case 0x0'05'26: return 0x0'05'27;
				case 0x0'05'28: return 0x0'05'29;
				case 0x0'05'2a: return 0x0'05'2b;
				case 0x0'05'2c: return 0x0'05'2d;
				case 0x0'05'2e: return 0x0'05'2f;
				case 0x0'05'31: return 0x0'05'61;
				case 0x0'05'32: return 0x0'05'62;
				case 0x0'05'33: return 0x0'05'63;
				case 0x0'05'34: return 0x0'05'64;
				case 0x0'05'35: return 0x0'05'65;
				case 0x0'05'36: return 0x0'05'66;
				case 0x0'05'37: return 0x0'05'67;
				case 0x0'05'38: return 0x0'05'68;
				case 0x0'05'39: return 0x0'05'69;
				case 0x0'05'3a: return 0x0'05'6a;
				case 0x0'05'3b: return 0x0'05'6b;
				case 0x0'05'3c: return 0x0'05'6c;
				case 0x0'05'3d: return 0x0'05'6d;
				case 0x0'05'3e: return 0x0'05'6e;
				case 0x0'05'3f: return 0x0'05'6f;
				case 0x0'05'40: return 0x0'05'70;
				case 0x0'05'41: return 0x0'05'71;
				case 0x0'05'42: return 0x0'05'72;
				case 0x0'05'43: return 0x0'05'73;
				case 0x0'05'44: return 0x0'05'74;
				case 0x0'05'45: return 0x0'05'75;
				case 0x0'05'46: return 0x0'05'76;
				case 0x0'05'47: return 0x0'05'77;
				case 0x0'05'48: return 0x0'05'78;
				case 0x0'05'49: return 0x0'05'79;
				case 0x0'05'4a: return 0x0'05'7a;
				case 0x0'05'4b: return 0x0'05'7b;
				case 0x0'05'4c: return 0x0'05'7c;
				case 0x0'05'4d: return 0x0'05'7d;
				case 0x0'05'4e: return 0x0'05'7e;
				case 0x0'05'4f: return 0x0'05'7f;
				case 0x0'05'50: return 0x0'05'80;
				case 0x0'05'51: return 0x0'05'81;
				case 0x0'05'52: return 0x0'05'82;
				case 0x0'05'53: return 0x0'05'83;
				case 0x0'05'54: return 0x0'05'84;
				case 0x0'05'55: return 0x0'05'85;
				case 0x0'05'56: return 0x0'05'86;
				case 0x0'1c'90: return 0x0'10'd0;
				case 0x0'1c'91: return 0x0'10'd1;
				case 0x0'1c'92: return 0x0'10'd2;
				case 0x0'1c'93: return 0x0'10'd3;
				case 0x0'1c'94: return 0x0'10'd4;
				case 0x0'1c'95: return 0x0'10'd5;
				case 0x0'1c'96: return 0x0'10'd6;
				case 0x0'1c'97: return 0x0'10'd7;
				case 0x0'1c'98: return 0x0'10'd8;
				case 0x0'1c'99: return 0x0'10'd9;
				case 0x0'1c'9a: return 0x0'10'da;
				case 0x0'1c'9b: return 0x0'10'db;
				case 0x0'1c'9c: return 0x0'10'dc;
				case 0x0'1c'9d: return 0x0'10'dd;
				case 0x0'1c'9e: return 0x0'10'de;
				case 0x0'1c'9f: return 0x0'10'df;
				case 0x0'1c'a0: return 0x0'10'e0;
				case 0x0'1c'a1: return 0x0'10'e1;
				case 0x0'1c'a2: return 0x0'10'e2;
				case 0x0'1c'a3: return 0x0'10'e3;
				case 0x0'1c'a4: return 0x0'10'e4;
				case 0x0'1c'a5: return 0x0'10'e5;
				case 0x0'1c'a6: return 0x0'10'e6;
				case 0x0'1c'a7: return 0x0'10'e7;
				case 0x0'1c'a8: return 0x0'10'e8;
				case 0x0'1c'a9: return 0x0'10'e9;
				case 0x0'1c'aa: return 0x0'10'ea;
				case 0x0'1c'ab: return 0x0'10'eb;
				case 0x0'1c'ac: return 0x0'10'ec;
				case 0x0'1c'ad: return 0x0'10'ed;
				case 0x0'1c'ae: return 0x0'10'ee;
				case 0x0'1c'af: return 0x0'10'ef;
				case 0x0'1c'b0: return 0x0'10'f0;
				case 0x0'1c'b1: return 0x0'10'f1;
				case 0x0'1c'b2: return 0x0'10'f2;
				case 0x0'1c'b3: return 0x0'10'f3;
				case 0x0'1c'b4: return 0x0'10'f4;
				case 0x0'1c'b5: return 0x0'10'f5;
				case 0x0'1c'b6: return 0x0'10'f6;
				case 0x0'1c'b7: return 0x0'10'f7;
				case 0x0'1c'b8: return 0x0'10'f8;
				case 0x0'1c'b9: return 0x0'10'f9;
				case 0x0'1c'ba: return 0x0'10'fa;
				case 0x0'1c'bd: return 0x0'10'fd;
				case 0x0'1c'be: return 0x0'10'fe;
				case 0x0'1c'bf: return 0x0'10'ff;
				case 0x0'ab'70: return 0x0'13'a0;
				case 0x0'ab'71: return 0x0'13'a1;
				case 0x0'ab'72: return 0x0'13'a2;
				case 0x0'ab'73: return 0x0'13'a3;
				case 0x0'ab'74: return 0x0'13'a4;
				case 0x0'ab'75: return 0x0'13'a5;
				case 0x0'ab'76: return 0x0'13'a6;
				case 0x0'ab'77: return 0x0'13'a7;
				case 0x0'ab'78: return 0x0'13'a8;
				case 0x0'ab'79: return 0x0'13'a9;
				case 0x0'ab'7a: return 0x0'13'aa;
				case 0x0'ab'7b: return 0x0'13'ab;
				case 0x0'ab'7c: return 0x0'13'ac;
				case 0x0'ab'7d: return 0x0'13'ad;
				case 0x0'ab'7e: return 0x0'13'ae;
				case 0x0'ab'7f: return 0x0'13'af;
				case 0x0'ab'80: return 0x0'13'b0;
				case 0x0'ab'81: return 0x0'13'b1;
				case 0x0'ab'82: return 0x0'13'b2;
				case 0x0'ab'83: return 0x0'13'b3;
				case 0x0'ab'84: return 0x0'13'b4;
				case 0x0'ab'85: return 0x0'13'b5;
				case 0x0'ab'86: return 0x0'13'b6;
				case 0x0'ab'87: return 0x0'13'b7;
				case 0x0'ab'88: return 0x0'13'b8;
				case 0x0'ab'89: return 0x0'13'b9;
				case 0x0'ab'8a: return 0x0'13'ba;
				case 0x0'ab'8b: return 0x0'13'bb;
				case 0x0'ab'8c: return 0x0'13'bc;
				case 0x0'ab'8d: return 0x0'13'bd;
				case 0x0'ab'8e: return 0x0'13'be;
				case 0x0'ab'8f: return 0x0'13'bf;
				case 0x0'ab'90: return 0x0'13'c0;
				case 0x0'ab'91: return 0x0'13'c1;
				case 0x0'ab'92: return 0x0'13'c2;
				case 0x0'ab'93: return 0x0'13'c3;
				case 0x0'ab'94: return 0x0'13'c4;
				case 0x0'ab'95: return 0x0'13'c5;
				case 0x0'ab'96: return 0x0'13'c6;
				case 0x0'ab'97: return 0x0'13'c7;
				case 0x0'ab'98: return 0x0'13'c8;
				case 0x0'ab'99: return 0x0'13'c9;
				case 0x0'ab'9a: return 0x0'13'ca;
				case 0x0'ab'9b: return 0x0'13'cb;
				case 0x0'ab'9c: return 0x0'13'cc;
				case 0x0'ab'9d: return 0x0'13'cd;
				case 0x0'ab'9e: return 0x0'13'ce;
				case 0x0'ab'9f: return 0x0'13'cf;
				case 0x0'ab'a0: return 0x0'13'd0;
				case 0x0'ab'a1: return 0x0'13'd1;
				case 0x0'ab'a2: return 0x0'13'd2;
				case 0x0'ab'a3: return 0x0'13'd3;
				case 0x0'ab'a4: return 0x0'13'd4;
				case 0x0'ab'a5: return 0x0'13'd5;
				case 0x0'ab'a6: return 0x0'13'd6;
				case 0x0'ab'a7: return 0x0'13'd7;
				case 0x0'ab'a8: return 0x0'13'd8;
				case 0x0'ab'a9: return 0x0'13'd9;
				case 0x0'ab'aa: return 0x0'13'da;
				case 0x0'ab'ab: return 0x0'13'db;
				case 0x0'ab'ac: return 0x0'13'dc;
				case 0x0'ab'ad: return 0x0'13'dd;
				case 0x0'ab'ae: return 0x0'13'de;
				case 0x0'ab'af: return 0x0'13'df;
				case 0x0'ab'b0: return 0x0'13'e0;
				case 0x0'ab'b1: return 0x0'13'e1;
				case 0x0'ab'b2: return 0x0'13'e2;
				case 0x0'ab'b3: return 0x0'13'e3;
				case 0x0'ab'b4: return 0x0'13'e4;
				case 0x0'ab'b5: return 0x0'13'e5;
				case 0x0'ab'b6: return 0x0'13'e6;
				case 0x0'ab'b7: return 0x0'13'e7;
				case 0x0'ab'b8: return 0x0'13'e8;
				case 0x0'ab'b9: return 0x0'13'e9;
				case 0x0'ab'ba: return 0x0'13'ea;
				case 0x0'ab'bb: return 0x0'13'eb;
				case 0x0'ab'bc: return 0x0'13'ec;
				case 0x0'ab'bd: return 0x0'13'ed;
				case 0x0'ab'be: return 0x0'13'ee;
				case 0x0'ab'bf: return 0x0'13'ef;
				case 0x0'13'f8: return 0x0'13'f0;
				case 0x0'13'f9: return 0x0'13'f1;
				case 0x0'13'fa: return 0x0'13'f2;
				case 0x0'13'fb: return 0x0'13'f3;
				case 0x0'13'fc: return 0x0'13'f4;
				case 0x0'13'fd: return 0x0'13'f5;
				case 0x0'a7'7d: return 0x0'1d'79;
				case 0x0'2c'63: return 0x0'1d'7d;
				case 0x0'a7'c6: return 0x0'1d'8e;
				case 0x0'1e'00: return 0x0'1e'01;
				case 0x0'1e'02: return 0x0'1e'03;
				case 0x0'1e'04: return 0x0'1e'05;
				case 0x0'1e'06: return 0x0'1e'07;
				case 0x0'1e'08: return 0x0'1e'09;
				case 0x0'1e'0a: return 0x0'1e'0b;
				case 0x0'1e'0c: return 0x0'1e'0d;
				case 0x0'1e'0e: return 0x0'1e'0f;
				case 0x0'1e'10: return 0x0'1e'11;
				case 0x0'1e'12: return 0x0'1e'13;
				case 0x0'1e'14: return 0x0'1e'15;
				case 0x0'1e'16: return 0x0'1e'17;
				case 0x0'1e'18: return 0x0'1e'19;
				case 0x0'1e'1a: return 0x0'1e'1b;
				case 0x0'1e'1c: return 0x0'1e'1d;
				case 0x0'1e'1e: return 0x0'1e'1f;
				case 0x0'1e'20: return 0x0'1e'21;
				case 0x0'1e'22: return 0x0'1e'23;
				case 0x0'1e'24: return 0x0'1e'25;
				case 0x0'1e'26: return 0x0'1e'27;
				case 0x0'1e'28: return 0x0'1e'29;
				case 0x0'1e'2a: return 0x0'1e'2b;
				case 0x0'1e'2c: return 0x0'1e'2d;
				case 0x0'1e'2e: return 0x0'1e'2f;
				case 0x0'1e'30: return 0x0'1e'31;
				case 0x0'1e'32: return 0x0'1e'33;
				case 0x0'1e'34: return 0x0'1e'35;
				case 0x0'1e'36: return 0x0'1e'37;
				case 0x0'1e'38: return 0x0'1e'39;
				case 0x0'1e'3a: return 0x0'1e'3b;
				case 0x0'1e'3c: return 0x0'1e'3d;
				case 0x0'1e'3e: return 0x0'1e'3f;
				case 0x0'1e'40: return 0x0'1e'41;
				case 0x0'1e'42: return 0x0'1e'43;
				case 0x0'1e'44: return 0x0'1e'45;
				case 0x0'1e'46: return 0x0'1e'47;
				case 0x0'1e'48: return 0x0'1e'49;
				case 0x0'1e'4a: return 0x0'1e'4b;
				case 0x0'1e'4c: return 0x0'1e'4d;
				case 0x0'1e'4e: return 0x0'1e'4f;
				case 0x0'1e'50: return 0x0'1e'51;
				case 0x0'1e'52: return 0x0'1e'53;
				case 0x0'1e'54: return 0x0'1e'55;
				case 0x0'1e'56: return 0x0'1e'57;
				case 0x0'1e'58: return 0x0'1e'59;
				case 0x0'1e'5a: return 0x0'1e'5b;
				case 0x0'1e'5c: return 0x0'1e'5d;
				case 0x0'1e'5e: return 0x0'1e'5f;
				case 0x0'1e'60: return 0x0'1e'61;
				case 0x0'1e'62: return 0x0'1e'63;
				case 0x0'1e'64: return 0x0'1e'65;
				case 0x0'1e'66: return 0x0'1e'67;
				case 0x0'1e'68: return 0x0'1e'69;
				case 0x0'1e'6a: return 0x0'1e'6b;
				case 0x0'1e'6c: return 0x0'1e'6d;
				case 0x0'1e'6e: return 0x0'1e'6f;
				case 0x0'1e'70: return 0x0'1e'71;
				case 0x0'1e'72: return 0x0'1e'73;
				case 0x0'1e'74: return 0x0'1e'75;
				case 0x0'1e'76: return 0x0'1e'77;
				case 0x0'1e'78: return 0x0'1e'79;
				case 0x0'1e'7a: return 0x0'1e'7b;
				case 0x0'1e'7c: return 0x0'1e'7d;
				case 0x0'1e'7e: return 0x0'1e'7f;
				case 0x0'1e'80: return 0x0'1e'81;
				case 0x0'1e'82: return 0x0'1e'83;
				case 0x0'1e'84: return 0x0'1e'85;
				case 0x0'1e'86: return 0x0'1e'87;
				case 0x0'1e'88: return 0x0'1e'89;
				case 0x0'1e'8a: return 0x0'1e'8b;
				case 0x0'1e'8c: return 0x0'1e'8d;
				case 0x0'1e'8e: return 0x0'1e'8f;
				case 0x0'1e'90: return 0x0'1e'91;
				case 0x0'1e'92: return 0x0'1e'93;
				case 0x0'1e'94: return 0x0'1e'95;
				case 0x0'1e'a0: return 0x0'1e'a1;
				case 0x0'1e'a2: return 0x0'1e'a3;
				case 0x0'1e'a4: return 0x0'1e'a5;
				case 0x0'1e'a6: return 0x0'1e'a7;
				case 0x0'1e'a8: return 0x0'1e'a9;
				case 0x0'1e'aa: return 0x0'1e'ab;
				case 0x0'1e'ac: return 0x0'1e'ad;
				case 0x0'1e'ae: return 0x0'1e'af;
				case 0x0'1e'b0: return 0x0'1e'b1;
				case 0x0'1e'b2: return 0x0'1e'b3;
				case 0x0'1e'b4: return 0x0'1e'b5;
				case 0x0'1e'b6: return 0x0'1e'b7;
				case 0x0'1e'b8: return 0x0'1e'b9;
				case 0x0'1e'ba: return 0x0'1e'bb;
				case 0x0'1e'bc: return 0x0'1e'bd;
				case 0x0'1e'be: return 0x0'1e'bf;
				case 0x0'1e'c0: return 0x0'1e'c1;
				case 0x0'1e'c2: return 0x0'1e'c3;
				case 0x0'1e'c4: return 0x0'1e'c5;
				case 0x0'1e'c6: return 0x0'1e'c7;
				case 0x0'1e'c8: return 0x0'1e'c9;
				case 0x0'1e'ca: return 0x0'1e'cb;
				case 0x0'1e'cc: return 0x0'1e'cd;
				case 0x0'1e'ce: return 0x0'1e'cf;
				case 0x0'1e'd0: return 0x0'1e'd1;
				case 0x0'1e'd2: return 0x0'1e'd3;
				case 0x0'1e'd4: return 0x0'1e'd5;
				case 0x0'1e'd6: return 0x0'1e'd7;
				case 0x0'1e'd8: return 0x0'1e'd9;
				case 0x0'1e'da: return 0x0'1e'db;
				case 0x0'1e'dc: return 0x0'1e'dd;
				case 0x0'1e'de: return 0x0'1e'df;
				case 0x0'1e'e0: return 0x0'1e'e1;
				case 0x0'1e'e2: return 0x0'1e'e3;
				case 0x0'1e'e4: return 0x0'1e'e5;
				case 0x0'1e'e6: return 0x0'1e'e7;
				case 0x0'1e'e8: return 0x0'1e'e9;
				case 0x0'1e'ea: return 0x0'1e'eb;
				case 0x0'1e'ec: return 0x0'1e'ed;
				case 0x0'1e'ee: return 0x0'1e'ef;
				case 0x0'1e'f0: return 0x0'1e'f1;
				case 0x0'1e'f2: return 0x0'1e'f3;
				case 0x0'1e'f4: return 0x0'1e'f5;
				case 0x0'1e'f6: return 0x0'1e'f7;
				case 0x0'1e'f8: return 0x0'1e'f9;
				case 0x0'1e'fa: return 0x0'1e'fb;
				case 0x0'1e'fc: return 0x0'1e'fd;
				case 0x0'1e'fe: return 0x0'1e'ff;
				case 0x0'1f'08: return 0x0'1f'00;
				case 0x0'1f'09: return 0x0'1f'01;
				case 0x0'1f'0a: return 0x0'1f'02;
				case 0x0'1f'0b: return 0x0'1f'03;
				case 0x0'1f'0c: return 0x0'1f'04;
				case 0x0'1f'0d: return 0x0'1f'05;
				case 0x0'1f'0e: return 0x0'1f'06;
				case 0x0'1f'0f: return 0x0'1f'07;
				case 0x0'1f'18: return 0x0'1f'10;
				case 0x0'1f'19: return 0x0'1f'11;
				case 0x0'1f'1a: return 0x0'1f'12;
				case 0x0'1f'1b: return 0x0'1f'13;
				case 0x0'1f'1c: return 0x0'1f'14;
				case 0x0'1f'1d: return 0x0'1f'15;
				case 0x0'1f'28: return 0x0'1f'20;
				case 0x0'1f'29: return 0x0'1f'21;
				case 0x0'1f'2a: return 0x0'1f'22;
				case 0x0'1f'2b: return 0x0'1f'23;
				case 0x0'1f'2c: return 0x0'1f'24;
				case 0x0'1f'2d: return 0x0'1f'25;
				case 0x0'1f'2e: return 0x0'1f'26;
				case 0x0'1f'2f: return 0x0'1f'27;
				case 0x0'1f'38: return 0x0'1f'30;
				case 0x0'1f'39: return 0x0'1f'31;
				case 0x0'1f'3a: return 0x0'1f'32;
				case 0x0'1f'3b: return 0x0'1f'33;
				case 0x0'1f'3c: return 0x0'1f'34;
				case 0x0'1f'3d: return 0x0'1f'35;
				case 0x0'1f'3e: return 0x0'1f'36;
				case 0x0'1f'3f: return 0x0'1f'37;
				case 0x0'1f'48: return 0x0'1f'40;
				case 0x0'1f'49: return 0x0'1f'41;
				case 0x0'1f'4a: return 0x0'1f'42;
				case 0x0'1f'4b: return 0x0'1f'43;
				case 0x0'1f'4c: return 0x0'1f'44;
				case 0x0'1f'4d: return 0x0'1f'45;
				case 0x0'1f'59: return 0x0'1f'51;
				case 0x0'1f'5b: return 0x0'1f'53;
				case 0x0'1f'5d: return 0x0'1f'55;
				case 0x0'1f'5f: return 0x0'1f'57;
				case 0x0'1f'68: return 0x0'1f'60;
				case 0x0'1f'69: return 0x0'1f'61;
				case 0x0'1f'6a: return 0x0'1f'62;
				case 0x0'1f'6b: return 0x0'1f'63;
				case 0x0'1f'6c: return 0x0'1f'64;
				case 0x0'1f'6d: return 0x0'1f'65;
				case 0x0'1f'6e: return 0x0'1f'66;
				case 0x0'1f'6f: return 0x0'1f'67;
				case 0x0'1f'ba: return 0x0'1f'70;
				case 0x0'1f'bb: return 0x0'1f'71;
				case 0x0'1f'c8: return 0x0'1f'72;
				case 0x0'1f'c9: return 0x0'1f'73;
				case 0x0'1f'ca: return 0x0'1f'74;
				case 0x0'1f'cb: return 0x0'1f'75;
				case 0x0'1f'da: return 0x0'1f'76;
				case 0x0'1f'db: return 0x0'1f'77;
				case 0x0'1f'f8: return 0x0'1f'78;
				case 0x0'1f'f9: return 0x0'1f'79;
				case 0x0'1f'ea: return 0x0'1f'7a;
				case 0x0'1f'eb: return 0x0'1f'7b;
				case 0x0'1f'fa: return 0x0'1f'7c;
				case 0x0'1f'fb: return 0x0'1f'7d;
				case 0x0'1f'88: return 0x0'1f'80;
				case 0x0'1f'89: return 0x0'1f'81;
				case 0x0'1f'8a: return 0x0'1f'82;
				case 0x0'1f'8b: return 0x0'1f'83;
				case 0x0'1f'8c: return 0x0'1f'84;
				case 0x0'1f'8d: return 0x0'1f'85;
				case 0x0'1f'8e: return 0x0'1f'86;
				case 0x0'1f'8f: return 0x0'1f'87;
				case 0x0'1f'98: return 0x0'1f'90;
				case 0x0'1f'99: return 0x0'1f'91;
				case 0x0'1f'9a: return 0x0'1f'92;
				case 0x0'1f'9b: return 0x0'1f'93;
				case 0x0'1f'9c: return 0x0'1f'94;
				case 0x0'1f'9d: return 0x0'1f'95;
				case 0x0'1f'9e: return 0x0'1f'96;
				case 0x0'1f'9f: return 0x0'1f'97;
				case 0x0'1f'a8: return 0x0'1f'a0;
				case 0x0'1f'a9: return 0x0'1f'a1;
				case 0x0'1f'aa: return 0x0'1f'a2;
				case 0x0'1f'ab: return 0x0'1f'a3;
				case 0x0'1f'ac: return 0x0'1f'a4;
				case 0x0'1f'ad: return 0x0'1f'a5;
				case 0x0'1f'ae: return 0x0'1f'a6;
				case 0x0'1f'af: return 0x0'1f'a7;
				case 0x0'1f'b8: return 0x0'1f'b0;
				case 0x0'1f'b9: return 0x0'1f'b1;
				case 0x0'1f'bc: return 0x0'1f'b3;
				case 0x0'1f'cc: return 0x0'1f'c3;
				case 0x0'1f'd8: return 0x0'1f'd0;
				case 0x0'1f'd9: return 0x0'1f'd1;
				case 0x0'1f'e8: return 0x0'1f'e0;
				case 0x0'1f'e9: return 0x0'1f'e1;
				case 0x0'1f'ec: return 0x0'1f'e5;
				case 0x0'1f'fc: return 0x0'1f'f3;
				case 0x0'21'32: return 0x0'21'4e;
				case 0x0'21'60: return 0x0'21'70;
				case 0x0'21'61: return 0x0'21'71;
				case 0x0'21'62: return 0x0'21'72;
				case 0x0'21'63: return 0x0'21'73;
				case 0x0'21'64: return 0x0'21'74;
				case 0x0'21'65: return 0x0'21'75;
				case 0x0'21'66: return 0x0'21'76;
				case 0x0'21'67: return 0x0'21'77;
				case 0x0'21'68: return 0x0'21'78;
				case 0x0'21'69: return 0x0'21'79;
				case 0x0'21'6a: return 0x0'21'7a;
				case 0x0'21'6b: return 0x0'21'7b;
				case 0x0'21'6c: return 0x0'21'7c;
				case 0x0'21'6d: return 0x0'21'7d;
				case 0x0'21'6e: return 0x0'21'7e;
				case 0x0'21'6f: return 0x0'21'7f;
				case 0x0'21'83: return 0x0'21'84;
				case 0x0'24'b6: return 0x0'24'd0;
				case 0x0'24'b7: return 0x0'24'd1;
				case 0x0'24'b8: return 0x0'24'd2;
				case 0x0'24'b9: return 0x0'24'd3;
				case 0x0'24'ba: return 0x0'24'd4;
				case 0x0'24'bb: return 0x0'24'd5;
				case 0x0'24'bc: return 0x0'24'd6;
				case 0x0'24'bd: return 0x0'24'd7;
				case 0x0'24'be: return 0x0'24'd8;
				case 0x0'24'bf: return 0x0'24'd9;
				case 0x0'24'c0: return 0x0'24'da;
				case 0x0'24'c1: return 0x0'24'db;
				case 0x0'24'c2: return 0x0'24'dc;
				case 0x0'24'c3: return 0x0'24'dd;
				case 0x0'24'c4: return 0x0'24'de;
				case 0x0'24'c5: return 0x0'24'df;
				case 0x0'24'c6: return 0x0'24'e0;
				case 0x0'24'c7: return 0x0'24'e1;
				case 0x0'24'c8: return 0x0'24'e2;
				case 0x0'24'c9: return 0x0'24'e3;
				case 0x0'24'ca: return 0x0'24'e4;
				case 0x0'24'cb: return 0x0'24'e5;
				case 0x0'24'cc: return 0x0'24'e6;
				case 0x0'24'cd: return 0x0'24'e7;
				case 0x0'24'ce: return 0x0'24'e8;
				case 0x0'24'cf: return 0x0'24'e9;
				case 0x0'2c'00: return 0x0'2c'30;
				case 0x0'2c'01: return 0x0'2c'31;
				case 0x0'2c'02: return 0x0'2c'32;
				case 0x0'2c'03: return 0x0'2c'33;
				case 0x0'2c'04: return 0x0'2c'34;
				case 0x0'2c'05: return 0x0'2c'35;
				case 0x0'2c'06: return 0x0'2c'36;
				case 0x0'2c'07: return 0x0'2c'37;
				case 0x0'2c'08: return 0x0'2c'38;
				case 0x0'2c'09: return 0x0'2c'39;
				case 0x0'2c'0a: return 0x0'2c'3a;
				case 0x0'2c'0b: return 0x0'2c'3b;
				case 0x0'2c'0c: return 0x0'2c'3c;
				case 0x0'2c'0d: return 0x0'2c'3d;
				case 0x0'2c'0e: return 0x0'2c'3e;
				case 0x0'2c'0f: return 0x0'2c'3f;
				case 0x0'2c'10: return 0x0'2c'40;
				case 0x0'2c'11: return 0x0'2c'41;
				case 0x0'2c'12: return 0x0'2c'42;
				case 0x0'2c'13: return 0x0'2c'43;
				case 0x0'2c'14: return 0x0'2c'44;
				case 0x0'2c'15: return 0x0'2c'45;
				case 0x0'2c'16: return 0x0'2c'46;
				case 0x0'2c'17: return 0x0'2c'47;
				case 0x0'2c'18: return 0x0'2c'48;
				case 0x0'2c'19: return 0x0'2c'49;
				case 0x0'2c'1a: return 0x0'2c'4a;
				case 0x0'2c'1b: return 0x0'2c'4b;
				case 0x0'2c'1c: return 0x0'2c'4c;
				case 0x0'2c'1d: return 0x0'2c'4d;
				case 0x0'2c'1e: return 0x0'2c'4e;
				case 0x0'2c'1f: return 0x0'2c'4f;
				case 0x0'2c'20: return 0x0'2c'50;
				case 0x0'2c'21: return 0x0'2c'51;
				case 0x0'2c'22: return 0x0'2c'52;
				case 0x0'2c'23: return 0x0'2c'53;
				case 0x0'2c'24: return 0x0'2c'54;
				case 0x0'2c'25: return 0x0'2c'55;
				case 0x0'2c'26: return 0x0'2c'56;
				case 0x0'2c'27: return 0x0'2c'57;
				case 0x0'2c'28: return 0x0'2c'58;
				case 0x0'2c'29: return 0x0'2c'59;
				case 0x0'2c'2a: return 0x0'2c'5a;
				case 0x0'2c'2b: return 0x0'2c'5b;
				case 0x0'2c'2c: return 0x0'2c'5c;
				case 0x0'2c'2d: return 0x0'2c'5d;
				case 0x0'2c'2e: return 0x0'2c'5e;
				case 0x0'2c'60: return 0x0'2c'61;
				case 0x0'02'3a: return 0x0'2c'65;
				case 0x0'02'3e: return 0x0'2c'66;
				case 0x0'2c'67: return 0x0'2c'68;
				case 0x0'2c'69: return 0x0'2c'6a;
				case 0x0'2c'6b: return 0x0'2c'6c;
				case 0x0'2c'72: return 0x0'2c'73;
				case 0x0'2c'75: return 0x0'2c'76;
				case 0x0'2c'80: return 0x0'2c'81;
				case 0x0'2c'82: return 0x0'2c'83;
				case 0x0'2c'84: return 0x0'2c'85;
				case 0x0'2c'86: return 0x0'2c'87;
				case 0x0'2c'88: return 0x0'2c'89;
				case 0x0'2c'8a: return 0x0'2c'8b;
				case 0x0'2c'8c: return 0x0'2c'8d;
				case 0x0'2c'8e: return 0x0'2c'8f;
				case 0x0'2c'90: return 0x0'2c'91;
				case 0x0'2c'92: return 0x0'2c'93;
				case 0x0'2c'94: return 0x0'2c'95;
				case 0x0'2c'96: return 0x0'2c'97;
				case 0x0'2c'98: return 0x0'2c'99;
				case 0x0'2c'9a: return 0x0'2c'9b;
				case 0x0'2c'9c: return 0x0'2c'9d;
				case 0x0'2c'9e: return 0x0'2c'9f;
				case 0x0'2c'a0: return 0x0'2c'a1;
				case 0x0'2c'a2: return 0x0'2c'a3;
				case 0x0'2c'a4: return 0x0'2c'a5;
				case 0x0'2c'a6: return 0x0'2c'a7;
				case 0x0'2c'a8: return 0x0'2c'a9;
				case 0x0'2c'aa: return 0x0'2c'ab;
				case 0x0'2c'ac: return 0x0'2c'ad;
				case 0x0'2c'ae: return 0x0'2c'af;
				case 0x0'2c'b0: return 0x0'2c'b1;
				case 0x0'2c'b2: return 0x0'2c'b3;
				case 0x0'2c'b4: return 0x0'2c'b5;
				case 0x0'2c'b6: return 0x0'2c'b7;
				case 0x0'2c'b8: return 0x0'2c'b9;
				case 0x0'2c'ba: return 0x0'2c'bb;
				case 0x0'2c'bc: return 0x0'2c'bd;
				case 0x0'2c'be: return 0x0'2c'bf;
				case 0x0'2c'c0: return 0x0'2c'c1;
				case 0x0'2c'c2: return 0x0'2c'c3;
				case 0x0'2c'c4: return 0x0'2c'c5;
				case 0x0'2c'c6: return 0x0'2c'c7;
				case 0x0'2c'c8: return 0x0'2c'c9;
				case 0x0'2c'ca: return 0x0'2c'cb;
				case 0x0'2c'cc: return 0x0'2c'cd;
				case 0x0'2c'ce: return 0x0'2c'cf;
				case 0x0'2c'd0: return 0x0'2c'd1;
				case 0x0'2c'd2: return 0x0'2c'd3;
				case 0x0'2c'd4: return 0x0'2c'd5;
				case 0x0'2c'd6: return 0x0'2c'd7;
				case 0x0'2c'd8: return 0x0'2c'd9;
				case 0x0'2c'da: return 0x0'2c'db;
				case 0x0'2c'dc: return 0x0'2c'dd;
				case 0x0'2c'de: return 0x0'2c'df;
				case 0x0'2c'e0: return 0x0'2c'e1;
				case 0x0'2c'e2: return 0x0'2c'e3;
				case 0x0'2c'eb: return 0x0'2c'ec;
				case 0x0'2c'ed: return 0x0'2c'ee;
				case 0x0'2c'f2: return 0x0'2c'f3;
				case 0x0'10'a0: return 0x0'2d'00;
				case 0x0'10'a1: return 0x0'2d'01;
				case 0x0'10'a2: return 0x0'2d'02;
				case 0x0'10'a3: return 0x0'2d'03;
				case 0x0'10'a4: return 0x0'2d'04;
				case 0x0'10'a5: return 0x0'2d'05;
				case 0x0'10'a6: return 0x0'2d'06;
				case 0x0'10'a7: return 0x0'2d'07;
				case 0x0'10'a8: return 0x0'2d'08;
				case 0x0'10'a9: return 0x0'2d'09;
				case 0x0'10'aa: return 0x0'2d'0a;
				case 0x0'10'ab: return 0x0'2d'0b;
				case 0x0'10'ac: return 0x0'2d'0c;
				case 0x0'10'ad: return 0x0'2d'0d;
				case 0x0'10'ae: return 0x0'2d'0e;
				case 0x0'10'af: return 0x0'2d'0f;
				case 0x0'10'b0: return 0x0'2d'10;
				case 0x0'10'b1: return 0x0'2d'11;
				case 0x0'10'b2: return 0x0'2d'12;
				case 0x0'10'b3: return 0x0'2d'13;
				case 0x0'10'b4: return 0x0'2d'14;
				case 0x0'10'b5: return 0x0'2d'15;
				case 0x0'10'b6: return 0x0'2d'16;
				case 0x0'10'b7: return 0x0'2d'17;
				case 0x0'10'b8: return 0x0'2d'18;
				case 0x0'10'b9: return 0x0'2d'19;
				case 0x0'10'ba: return 0x0'2d'1a;
				case 0x0'10'bb: return 0x0'2d'1b;
				case 0x0'10'bc: return 0x0'2d'1c;
				case 0x0'10'bd: return 0x0'2d'1d;
				case 0x0'10'be: return 0x0'2d'1e;
				case 0x0'10'bf: return 0x0'2d'1f;
				case 0x0'10'c0: return 0x0'2d'20;
				case 0x0'10'c1: return 0x0'2d'21;
				case 0x0'10'c2: return 0x0'2d'22;
				case 0x0'10'c3: return 0x0'2d'23;
				case 0x0'10'c4: return 0x0'2d'24;
				case 0x0'10'c5: return 0x0'2d'25;
				case 0x0'10'c7: return 0x0'2d'27;
				case 0x0'10'cd: return 0x0'2d'2d;
				case 0x0'a6'40: return 0x0'a6'41;
				case 0x0'a6'42: return 0x0'a6'43;
				case 0x0'a6'44: return 0x0'a6'45;
				case 0x0'a6'46: return 0x0'a6'47;
				case 0x0'a6'48: return 0x0'a6'49;
				case 0x0'1c'88: return 0x0'a6'4b;
				case 0x0'a6'4c: return 0x0'a6'4d;
				case 0x0'a6'4e: return 0x0'a6'4f;
				case 0x0'a6'50: return 0x0'a6'51;
				case 0x0'a6'52: return 0x0'a6'53;
				case 0x0'a6'54: return 0x0'a6'55;
				case 0x0'a6'56: return 0x0'a6'57;
				case 0x0'a6'58: return 0x0'a6'59;
				case 0x0'a6'5a: return 0x0'a6'5b;
				case 0x0'a6'5c: return 0x0'a6'5d;
				case 0x0'a6'5e: return 0x0'a6'5f;
				case 0x0'a6'60: return 0x0'a6'61;
				case 0x0'a6'62: return 0x0'a6'63;
				case 0x0'a6'64: return 0x0'a6'65;
				case 0x0'a6'66: return 0x0'a6'67;
				case 0x0'a6'68: return 0x0'a6'69;
				case 0x0'a6'6a: return 0x0'a6'6b;
				case 0x0'a6'6c: return 0x0'a6'6d;
				case 0x0'a6'80: return 0x0'a6'81;
				case 0x0'a6'82: return 0x0'a6'83;
				case 0x0'a6'84: return 0x0'a6'85;
				case 0x0'a6'86: return 0x0'a6'87;
				case 0x0'a6'88: return 0x0'a6'89;
				case 0x0'a6'8a: return 0x0'a6'8b;
				case 0x0'a6'8c: return 0x0'a6'8d;
				case 0x0'a6'8e: return 0x0'a6'8f;
				case 0x0'a6'90: return 0x0'a6'91;
				case 0x0'a6'92: return 0x0'a6'93;
				case 0x0'a6'94: return 0x0'a6'95;
				case 0x0'a6'96: return 0x0'a6'97;
				case 0x0'a6'98: return 0x0'a6'99;
				case 0x0'a6'9a: return 0x0'a6'9b;
				case 0x0'a7'22: return 0x0'a7'23;
				case 0x0'a7'24: return 0x0'a7'25;
				case 0x0'a7'26: return 0x0'a7'27;
				case 0x0'a7'28: return 0x0'a7'29;
				case 0x0'a7'2a: return 0x0'a7'2b;
				case 0x0'a7'2c: return 0x0'a7'2d;
				case 0x0'a7'2e: return 0x0'a7'2f;
				case 0x0'a7'32: return 0x0'a7'33;
				case 0x0'a7'34: return 0x0'a7'35;
				case 0x0'a7'36: return 0x0'a7'37;
				case 0x0'a7'38: return 0x0'a7'39;
				case 0x0'a7'3a: return 0x0'a7'3b;
				case 0x0'a7'3c: return 0x0'a7'3d;
				case 0x0'a7'3e: return 0x0'a7'3f;
				case 0x0'a7'40: return 0x0'a7'41;
				case 0x0'a7'42: return 0x0'a7'43;
				case 0x0'a7'44: return 0x0'a7'45;
				case 0x0'a7'46: return 0x0'a7'47;
				case 0x0'a7'48: return 0x0'a7'49;
				case 0x0'a7'4a: return 0x0'a7'4b;
				case 0x0'a7'4c: return 0x0'a7'4d;
				case 0x0'a7'4e: return 0x0'a7'4f;
				case 0x0'a7'50: return 0x0'a7'51;
				case 0x0'a7'52: return 0x0'a7'53;
				case 0x0'a7'54: return 0x0'a7'55;
				case 0x0'a7'56: return 0x0'a7'57;
				case 0x0'a7'58: return 0x0'a7'59;
				case 0x0'a7'5a: return 0x0'a7'5b;
				case 0x0'a7'5c: return 0x0'a7'5d;
				case 0x0'a7'5e: return 0x0'a7'5f;
				case 0x0'a7'60: return 0x0'a7'61;
				case 0x0'a7'62: return 0x0'a7'63;
				case 0x0'a7'64: return 0x0'a7'65;
				case 0x0'a7'66: return 0x0'a7'67;
				case 0x0'a7'68: return 0x0'a7'69;
				case 0x0'a7'6a: return 0x0'a7'6b;
				case 0x0'a7'6c: return 0x0'a7'6d;
				case 0x0'a7'6e: return 0x0'a7'6f;
				case 0x0'a7'79: return 0x0'a7'7a;
				case 0x0'a7'7b: return 0x0'a7'7c;
				case 0x0'a7'7e: return 0x0'a7'7f;
				case 0x0'a7'80: return 0x0'a7'81;
				case 0x0'a7'82: return 0x0'a7'83;
				case 0x0'a7'84: return 0x0'a7'85;
				case 0x0'a7'86: return 0x0'a7'87;
				case 0x0'a7'8b: return 0x0'a7'8c;
				case 0x0'a7'90: return 0x0'a7'91;
				case 0x0'a7'92: return 0x0'a7'93;
				case 0x0'a7'c4: return 0x0'a7'94;
				case 0x0'a7'96: return 0x0'a7'97;
				case 0x0'a7'98: return 0x0'a7'99;
				case 0x0'a7'9a: return 0x0'a7'9b;
				case 0x0'a7'9c: return 0x0'a7'9d;
				case 0x0'a7'9e: return 0x0'a7'9f;
				case 0x0'a7'a0: return 0x0'a7'a1;
				case 0x0'a7'a2: return 0x0'a7'a3;
				case 0x0'a7'a4: return 0x0'a7'a5;
				case 0x0'a7'a6: return 0x0'a7'a7;
				case 0x0'a7'a8: return 0x0'a7'a9;
				case 0x0'a7'b4: return 0x0'a7'b5;
				case 0x0'a7'b6: return 0x0'a7'b7;
				case 0x0'a7'b8: return 0x0'a7'b9;
				case 0x0'a7'ba: return 0x0'a7'bb;
				case 0x0'a7'bc: return 0x0'a7'bd;
				case 0x0'a7'be: return 0x0'a7'bf;
				case 0x0'a7'c2: return 0x0'a7'c3;
				case 0x0'a7'b3: return 0x0'ab'53;
				case 0x0'ff'21: return 0x0'ff'41;
				case 0x0'ff'22: return 0x0'ff'42;
				case 0x0'ff'23: return 0x0'ff'43;
				case 0x0'ff'24: return 0x0'ff'44;
				case 0x0'ff'25: return 0x0'ff'45;
				case 0x0'ff'26: return 0x0'ff'46;
				case 0x0'ff'27: return 0x0'ff'47;
				case 0x0'ff'28: return 0x0'ff'48;
				case 0x0'ff'29: return 0x0'ff'49;
				case 0x0'ff'2a: return 0x0'ff'4a;
				case 0x0'ff'2b: return 0x0'ff'4b;
				case 0x0'ff'2c: return 0x0'ff'4c;
				case 0x0'ff'2d: return 0x0'ff'4d;
				case 0x0'ff'2e: return 0x0'ff'4e;
				case 0x0'ff'2f: return 0x0'ff'4f;
				case 0x0'ff'30: return 0x0'ff'50;
				case 0x0'ff'31: return 0x0'ff'51;
				case 0x0'ff'32: return 0x0'ff'52;
				case 0x0'ff'33: return 0x0'ff'53;
				case 0x0'ff'34: return 0x0'ff'54;
				case 0x0'ff'35: return 0x0'ff'55;
				case 0x0'ff'36: return 0x0'ff'56;
				case 0x0'ff'37: return 0x0'ff'57;
				case 0x0'ff'38: return 0x0'ff'58;
				case 0x0'ff'39: return 0x0'ff'59;
				case 0x0'ff'3a: return 0x0'ff'5a;
				case 0x1'04'00: return 0x1'04'28;
				case 0x1'04'01: return 0x1'04'29;
				case 0x1'04'02: return 0x1'04'2a;
				case 0x1'04'03: return 0x1'04'2b;
				case 0x1'04'04: return 0x1'04'2c;
				case 0x1'04'05: return 0x1'04'2d;
				case 0x1'04'06: return 0x1'04'2e;
				case 0x1'04'07: return 0x1'04'2f;
				case 0x1'04'08: return 0x1'04'30;
				case 0x1'04'09: return 0x1'04'31;
				case 0x1'04'0a: return 0x1'04'32;
				case 0x1'04'0b: return 0x1'04'33;
				case 0x1'04'0c: return 0x1'04'34;
				case 0x1'04'0d: return 0x1'04'35;
				case 0x1'04'0e: return 0x1'04'36;
				case 0x1'04'0f: return 0x1'04'37;
				case 0x1'04'10: return 0x1'04'38;
				case 0x1'04'11: return 0x1'04'39;
				case 0x1'04'12: return 0x1'04'3a;
				case 0x1'04'13: return 0x1'04'3b;
				case 0x1'04'14: return 0x1'04'3c;
				case 0x1'04'15: return 0x1'04'3d;
				case 0x1'04'16: return 0x1'04'3e;
				case 0x1'04'17: return 0x1'04'3f;
				case 0x1'04'18: return 0x1'04'40;
				case 0x1'04'19: return 0x1'04'41;
				case 0x1'04'1a: return 0x1'04'42;
				case 0x1'04'1b: return 0x1'04'43;
				case 0x1'04'1c: return 0x1'04'44;
				case 0x1'04'1d: return 0x1'04'45;
				case 0x1'04'1e: return 0x1'04'46;
				case 0x1'04'1f: return 0x1'04'47;
				case 0x1'04'20: return 0x1'04'48;
				case 0x1'04'21: return 0x1'04'49;
				case 0x1'04'22: return 0x1'04'4a;
				case 0x1'04'23: return 0x1'04'4b;
				case 0x1'04'24: return 0x1'04'4c;
				case 0x1'04'25: return 0x1'04'4d;
				case 0x1'04'26: return 0x1'04'4e;
				case 0x1'04'27: return 0x1'04'4f;
				case 0x1'04'b0: return 0x1'04'd8;
				case 0x1'04'b1: return 0x1'04'd9;
				case 0x1'04'b2: return 0x1'04'da;
				case 0x1'04'b3: return 0x1'04'db;
				case 0x1'04'b4: return 0x1'04'dc;
				case 0x1'04'b5: return 0x1'04'dd;
				case 0x1'04'b6: return 0x1'04'de;
				case 0x1'04'b7: return 0x1'04'df;
				case 0x1'04'b8: return 0x1'04'e0;
				case 0x1'04'b9: return 0x1'04'e1;
				case 0x1'04'ba: return 0x1'04'e2;
				case 0x1'04'bb: return 0x1'04'e3;
				case 0x1'04'bc: return 0x1'04'e4;
				case 0x1'04'bd: return 0x1'04'e5;
				case 0x1'04'be: return 0x1'04'e6;
				case 0x1'04'bf: return 0x1'04'e7;
				case 0x1'04'c0: return 0x1'04'e8;
				case 0x1'04'c1: return 0x1'04'e9;
				case 0x1'04'c2: return 0x1'04'ea;
				case 0x1'04'c3: return 0x1'04'eb;
				case 0x1'04'c4: return 0x1'04'ec;
				case 0x1'04'c5: return 0x1'04'ed;
				case 0x1'04'c6: return 0x1'04'ee;
				case 0x1'04'c7: return 0x1'04'ef;
				case 0x1'04'c8: return 0x1'04'f0;
				case 0x1'04'c9: return 0x1'04'f1;
				case 0x1'04'ca: return 0x1'04'f2;
				case 0x1'04'cb: return 0x1'04'f3;
				case 0x1'04'cc: return 0x1'04'f4;
				case 0x1'04'cd: return 0x1'04'f5;
				case 0x1'04'ce: return 0x1'04'f6;
				case 0x1'04'cf: return 0x1'04'f7;
				case 0x1'04'd0: return 0x1'04'f8;
				case 0x1'04'd1: return 0x1'04'f9;
				case 0x1'04'd2: return 0x1'04'fa;
				case 0x1'04'd3: return 0x1'04'fb;
				case 0x1'0c'80: return 0x1'0c'c0;
				case 0x1'0c'81: return 0x1'0c'c1;
				case 0x1'0c'82: return 0x1'0c'c2;
				case 0x1'0c'83: return 0x1'0c'c3;
				case 0x1'0c'84: return 0x1'0c'c4;
				case 0x1'0c'85: return 0x1'0c'c5;
				case 0x1'0c'86: return 0x1'0c'c6;
				case 0x1'0c'87: return 0x1'0c'c7;
				case 0x1'0c'88: return 0x1'0c'c8;
				case 0x1'0c'89: return 0x1'0c'c9;
				case 0x1'0c'8a: return 0x1'0c'ca;
				case 0x1'0c'8b: return 0x1'0c'cb;
				case 0x1'0c'8c: return 0x1'0c'cc;
				case 0x1'0c'8d: return 0x1'0c'cd;
				case 0x1'0c'8e: return 0x1'0c'ce;
				case 0x1'0c'8f: return 0x1'0c'cf;
				case 0x1'0c'90: return 0x1'0c'd0;
				case 0x1'0c'91: return 0x1'0c'd1;
				case 0x1'0c'92: return 0x1'0c'd2;
				case 0x1'0c'93: return 0x1'0c'd3;
				case 0x1'0c'94: return 0x1'0c'd4;
				case 0x1'0c'95: return 0x1'0c'd5;
				case 0x1'0c'96: return 0x1'0c'd6;
				case 0x1'0c'97: return 0x1'0c'd7;
				case 0x1'0c'98: return 0x1'0c'd8;
				case 0x1'0c'99: return 0x1'0c'd9;
				case 0x1'0c'9a: return 0x1'0c'da;
				case 0x1'0c'9b: return 0x1'0c'db;
				case 0x1'0c'9c: return 0x1'0c'dc;
				case 0x1'0c'9d: return 0x1'0c'dd;
				case 0x1'0c'9e: return 0x1'0c'de;
				case 0x1'0c'9f: return 0x1'0c'df;
				case 0x1'0c'a0: return 0x1'0c'e0;
				case 0x1'0c'a1: return 0x1'0c'e1;
				case 0x1'0c'a2: return 0x1'0c'e2;
				case 0x1'0c'a3: return 0x1'0c'e3;
				case 0x1'0c'a4: return 0x1'0c'e4;
				case 0x1'0c'a5: return 0x1'0c'e5;
				case 0x1'0c'a6: return 0x1'0c'e6;
				case 0x1'0c'a7: return 0x1'0c'e7;
				case 0x1'0c'a8: return 0x1'0c'e8;
				case 0x1'0c'a9: return 0x1'0c'e9;
				case 0x1'0c'aa: return 0x1'0c'ea;
				case 0x1'0c'ab: return 0x1'0c'eb;
				case 0x1'0c'ac: return 0x1'0c'ec;
				case 0x1'0c'ad: return 0x1'0c'ed;
				case 0x1'0c'ae: return 0x1'0c'ee;
				case 0x1'0c'af: return 0x1'0c'ef;
				case 0x1'0c'b0: return 0x1'0c'f0;
				case 0x1'0c'b1: return 0x1'0c'f1;
				case 0x1'0c'b2: return 0x1'0c'f2;
				case 0x1'18'a0: return 0x1'18'c0;
				case 0x1'18'a1: return 0x1'18'c1;
				case 0x1'18'a2: return 0x1'18'c2;
				case 0x1'18'a3: return 0x1'18'c3;
				case 0x1'18'a4: return 0x1'18'c4;
				case 0x1'18'a5: return 0x1'18'c5;
				case 0x1'18'a6: return 0x1'18'c6;
				case 0x1'18'a7: return 0x1'18'c7;
				case 0x1'18'a8: return 0x1'18'c8;
				case 0x1'18'a9: return 0x1'18'c9;
				case 0x1'18'aa: return 0x1'18'ca;
				case 0x1'18'ab: return 0x1'18'cb;
				case 0x1'18'ac: return 0x1'18'cc;
				case 0x1'18'ad: return 0x1'18'cd;
				case 0x1'18'ae: return 0x1'18'ce;
				case 0x1'18'af: return 0x1'18'cf;
				case 0x1'18'b0: return 0x1'18'd0;
				case 0x1'18'b1: return 0x1'18'd1;
				case 0x1'18'b2: return 0x1'18'd2;
				case 0x1'18'b3: return 0x1'18'd3;
				case 0x1'18'b4: return 0x1'18'd4;
				case 0x1'18'b5: return 0x1'18'd5;
				case 0x1'18'b6: return 0x1'18'd6;
				case 0x1'18'b7: return 0x1'18'd7;
				case 0x1'18'b8: return 0x1'18'd8;
				case 0x1'18'b9: return 0x1'18'd9;
				case 0x1'18'ba: return 0x1'18'da;
				case 0x1'18'bb: return 0x1'18'db;
				case 0x1'18'bc: return 0x1'18'dc;
				case 0x1'18'bd: return 0x1'18'dd;
				case 0x1'18'be: return 0x1'18'de;
				case 0x1'18'bf: return 0x1'18'df;
				case 0x1'6e'40: return 0x1'6e'60;
				case 0x1'6e'41: return 0x1'6e'61;
				case 0x1'6e'42: return 0x1'6e'62;
				case 0x1'6e'43: return 0x1'6e'63;
				case 0x1'6e'44: return 0x1'6e'64;
				case 0x1'6e'45: return 0x1'6e'65;
				case 0x1'6e'46: return 0x1'6e'66;
				case 0x1'6e'47: return 0x1'6e'67;
				case 0x1'6e'48: return 0x1'6e'68;
				case 0x1'6e'49: return 0x1'6e'69;
				case 0x1'6e'4a: return 0x1'6e'6a;
				case 0x1'6e'4b: return 0x1'6e'6b;
				case 0x1'6e'4c: return 0x1'6e'6c;
				case 0x1'6e'4d: return 0x1'6e'6d;
				case 0x1'6e'4e: return 0x1'6e'6e;
				case 0x1'6e'4f: return 0x1'6e'6f;
				case 0x1'6e'50: return 0x1'6e'70;
				case 0x1'6e'51: return 0x1'6e'71;
				case 0x1'6e'52: return 0x1'6e'72;
				case 0x1'6e'53: return 0x1'6e'73;
				case 0x1'6e'54: return 0x1'6e'74;
				case 0x1'6e'55: return 0x1'6e'75;
				case 0x1'6e'56: return 0x1'6e'76;
				case 0x1'6e'57: return 0x1'6e'77;
				case 0x1'6e'58: return 0x1'6e'78;
				case 0x1'6e'59: return 0x1'6e'79;
				case 0x1'6e'5a: return 0x1'6e'7a;
				case 0x1'6e'5b: return 0x1'6e'7b;
				case 0x1'6e'5c: return 0x1'6e'7c;
				case 0x1'6e'5d: return 0x1'6e'7d;
				case 0x1'6e'5e: return 0x1'6e'7e;
				case 0x1'6e'5f: return 0x1'6e'7f;
				case 0x1'e9'00: return 0x1'e9'22;
				case 0x1'e9'01: return 0x1'e9'23;
				case 0x1'e9'02: return 0x1'e9'24;
				case 0x1'e9'03: return 0x1'e9'25;
				case 0x1'e9'04: return 0x1'e9'26;
				case 0x1'e9'05: return 0x1'e9'27;
				case 0x1'e9'06: return 0x1'e9'28;
				case 0x1'e9'07: return 0x1'e9'29;
				case 0x1'e9'08: return 0x1'e9'2a;
				case 0x1'e9'09: return 0x1'e9'2b;
				case 0x1'e9'0a: return 0x1'e9'2c;
				case 0x1'e9'0b: return 0x1'e9'2d;
				case 0x1'e9'0c: return 0x1'e9'2e;
				case 0x1'e9'0d: return 0x1'e9'2f;
				case 0x1'e9'0e: return 0x1'e9'30;
				case 0x1'e9'0f: return 0x1'e9'31;
				case 0x1'e9'10: return 0x1'e9'32;
				case 0x1'e9'11: return 0x1'e9'33;
				case 0x1'e9'12: return 0x1'e9'34;
				case 0x1'e9'13: return 0x1'e9'35;
				case 0x1'e9'14: return 0x1'e9'36;
				case 0x1'e9'15: return 0x1'e9'37;
				case 0x1'e9'16: return 0x1'e9'38;
				case 0x1'e9'17: return 0x1'e9'39;
				case 0x1'e9'18: return 0x1'e9'3a;
				case 0x1'e9'19: return 0x1'e9'3b;
				case 0x1'e9'1a: return 0x1'e9'3c;
				case 0x1'e9'1b: return 0x1'e9'3d;
				case 0x1'e9'1c: return 0x1'e9'3e;
				case 0x1'e9'1d: return 0x1'e9'3f;
				case 0x1'e9'1e: return 0x1'e9'40;
				case 0x1'e9'1f: return 0x1'e9'41;
				case 0x1'e9'20: return 0x1'e9'42;
				case 0x1'e9'21: return 0x1'e9'43;
				default: return __c;
			};
		}
		pf_decl_inline pf_decl_constexpr size_t
		required_len_to_lower(
		 const u8char_t *__in,
		 size_t __len) pf_attr_noexcept
		{
			size_t s = 0;
			for(size_t i = 0; i < __len;)
			{
				s += code_point_len(to_lower(to_code_point(&__in[i])));
				i += code_len(&__in[i]);
			}
			return s;
		}
		pf_decl_inline pf_decl_constexpr void
		to_lower(
		 const u8char_t *__in,
		 size_t __len,
		 u8char_t *__dst) pf_attr_noexcept
		{
			for(size_t i = 0, o = 0; i < __len;)
			{
				u8code_t c = to_code_point(&__in[i]);
				u8code_t l = to_lower(c);
				to_code(l, &__dst[o]);
				i += code_point_len(c);
				o += code_point_len(l);
			}
		}

		pf_decl_inline pf_decl_constexpr u8code_t
		to_upper(
		 u8code_t __c) pf_attr_noexcept
		{
			switch(__c)
			{
				case 0x0'00'61: return 0x0'00'41;
				case 0x0'00'62: return 0x0'00'42;
				case 0x0'00'63: return 0x0'00'43;
				case 0x0'00'64: return 0x0'00'44;
				case 0x0'00'65: return 0x0'00'45;
				case 0x0'00'66: return 0x0'00'46;
				case 0x0'00'67: return 0x0'00'47;
				case 0x0'00'68: return 0x0'00'48;
				case 0x0'00'69: return 0x0'00'49;
				case 0x0'00'6a: return 0x0'00'4a;
				case 0x0'00'6b: return 0x0'00'4b;
				case 0x0'00'6c: return 0x0'00'4c;
				case 0x0'00'6d: return 0x0'00'4d;
				case 0x0'00'6e: return 0x0'00'4e;
				case 0x0'00'6f: return 0x0'00'4f;
				case 0x0'00'70: return 0x0'00'50;
				case 0x0'00'71: return 0x0'00'51;
				case 0x0'00'72: return 0x0'00'52;
				case 0x0'00'73: return 0x0'00'53;
				case 0x0'00'74: return 0x0'00'54;
				case 0x0'00'75: return 0x0'00'55;
				case 0x0'00'76: return 0x0'00'56;
				case 0x0'00'77: return 0x0'00'57;
				case 0x0'00'78: return 0x0'00'58;
				case 0x0'00'79: return 0x0'00'59;
				case 0x0'00'7a: return 0x0'00'5a;
				case 0x0'00'e0: return 0x0'00'c0;
				case 0x0'00'e1: return 0x0'00'c1;
				case 0x0'00'e2: return 0x0'00'c2;
				case 0x0'00'e3: return 0x0'00'c3;
				case 0x0'00'e4: return 0x0'00'c4;
				case 0x0'00'e5: return 0x0'00'c5;
				case 0x0'00'e6: return 0x0'00'c6;
				case 0x0'00'e7: return 0x0'00'c7;
				case 0x0'00'e8: return 0x0'00'c8;
				case 0x0'00'e9: return 0x0'00'c9;
				case 0x0'00'ea: return 0x0'00'ca;
				case 0x0'00'eb: return 0x0'00'cb;
				case 0x0'00'ec: return 0x0'00'cc;
				case 0x0'00'ed: return 0x0'00'cd;
				case 0x0'00'ee: return 0x0'00'ce;
				case 0x0'00'ef: return 0x0'00'cf;
				case 0x0'00'f0: return 0x0'00'd0;
				case 0x0'00'f1: return 0x0'00'd1;
				case 0x0'00'f2: return 0x0'00'd2;
				case 0x0'00'f3: return 0x0'00'd3;
				case 0x0'00'f4: return 0x0'00'd4;
				case 0x0'00'f5: return 0x0'00'd5;
				case 0x0'00'f6: return 0x0'00'd6;
				case 0x0'00'f8: return 0x0'00'd8;
				case 0x0'00'f9: return 0x0'00'd9;
				case 0x0'00'fa: return 0x0'00'da;
				case 0x0'00'fb: return 0x0'00'db;
				case 0x0'00'fc: return 0x0'00'dc;
				case 0x0'00'fd: return 0x0'00'dd;
				case 0x0'00'fe: return 0x0'00'de;
				case 0x0'01'01: return 0x0'01'00;
				case 0x0'01'03: return 0x0'01'02;
				case 0x0'01'05: return 0x0'01'04;
				case 0x0'01'07: return 0x0'01'06;
				case 0x0'01'09: return 0x0'01'08;
				case 0x0'01'0b: return 0x0'01'0a;
				case 0x0'01'0d: return 0x0'01'0c;
				case 0x0'01'0f: return 0x0'01'0e;
				case 0x0'01'11: return 0x0'01'10;
				case 0x0'01'13: return 0x0'01'12;
				case 0x0'01'15: return 0x0'01'14;
				case 0x0'01'17: return 0x0'01'16;
				case 0x0'01'19: return 0x0'01'18;
				case 0x0'01'1b: return 0x0'01'1a;
				case 0x0'01'1d: return 0x0'01'1c;
				case 0x0'01'1f: return 0x0'01'1e;
				case 0x0'01'21: return 0x0'01'20;
				case 0x0'01'23: return 0x0'01'22;
				case 0x0'01'25: return 0x0'01'24;
				case 0x0'01'27: return 0x0'01'26;
				case 0x0'01'29: return 0x0'01'28;
				case 0x0'01'2b: return 0x0'01'2a;
				case 0x0'01'2d: return 0x0'01'2c;
				case 0x0'01'2f: return 0x0'01'2e;
				case 0x0'01'33: return 0x0'01'32;
				case 0x0'01'35: return 0x0'01'34;
				case 0x0'01'37: return 0x0'01'36;
				case 0x0'01'3a: return 0x0'01'39;
				case 0x0'01'3c: return 0x0'01'3b;
				case 0x0'01'3e: return 0x0'01'3d;
				case 0x0'01'40: return 0x0'01'3f;
				case 0x0'01'42: return 0x0'01'41;
				case 0x0'01'44: return 0x0'01'43;
				case 0x0'01'46: return 0x0'01'45;
				case 0x0'01'48: return 0x0'01'47;
				case 0x0'01'4b: return 0x0'01'4a;
				case 0x0'01'4d: return 0x0'01'4c;
				case 0x0'01'4f: return 0x0'01'4e;
				case 0x0'01'51: return 0x0'01'50;
				case 0x0'01'53: return 0x0'01'52;
				case 0x0'01'55: return 0x0'01'54;
				case 0x0'01'57: return 0x0'01'56;
				case 0x0'01'59: return 0x0'01'58;
				case 0x0'01'5b: return 0x0'01'5a;
				case 0x0'01'5d: return 0x0'01'5c;
				case 0x0'01'5f: return 0x0'01'5e;
				case 0x0'01'61: return 0x0'01'60;
				case 0x0'01'63: return 0x0'01'62;
				case 0x0'01'65: return 0x0'01'64;
				case 0x0'01'67: return 0x0'01'66;
				case 0x0'01'69: return 0x0'01'68;
				case 0x0'01'6b: return 0x0'01'6a;
				case 0x0'01'6d: return 0x0'01'6c;
				case 0x0'01'6f: return 0x0'01'6e;
				case 0x0'01'71: return 0x0'01'70;
				case 0x0'01'73: return 0x0'01'72;
				case 0x0'01'75: return 0x0'01'74;
				case 0x0'01'77: return 0x0'01'76;
				case 0x0'00'ff: return 0x0'01'78;
				case 0x0'01'7a: return 0x0'01'79;
				case 0x0'01'7c: return 0x0'01'7b;
				case 0x0'01'7e: return 0x0'01'7d;
				case 0x0'02'53: return 0x0'01'81;
				case 0x0'01'83: return 0x0'01'82;
				case 0x0'01'85: return 0x0'01'84;
				case 0x0'02'54: return 0x0'01'86;
				case 0x0'01'88: return 0x0'01'87;
				case 0x0'02'56: return 0x0'01'89;
				case 0x0'02'57: return 0x0'01'8a;
				case 0x0'01'8c: return 0x0'01'8b;
				case 0x0'01'dd: return 0x0'01'8e;
				case 0x0'02'59: return 0x0'01'8f;
				case 0x0'02'5b: return 0x0'01'90;
				case 0x0'01'92: return 0x0'01'91;
				case 0x0'02'60: return 0x0'01'93;
				case 0x0'02'63: return 0x0'01'94;
				case 0x0'02'69: return 0x0'01'96;
				case 0x0'02'68: return 0x0'01'97;
				case 0x0'01'99: return 0x0'01'98;
				case 0x0'02'6f: return 0x0'01'9c;
				case 0x0'02'72: return 0x0'01'9d;
				case 0x0'02'75: return 0x0'01'9f;
				case 0x0'01'a1: return 0x0'01'a0;
				case 0x0'01'a3: return 0x0'01'a2;
				case 0x0'01'a5: return 0x0'01'a4;
				case 0x0'02'80: return 0x0'01'a6;
				case 0x0'01'a8: return 0x0'01'a7;
				case 0x0'02'83: return 0x0'01'a9;
				case 0x0'01'ad: return 0x0'01'ac;
				case 0x0'02'88: return 0x0'01'ae;
				case 0x0'01'b0: return 0x0'01'af;
				case 0x0'02'8a: return 0x0'01'b1;
				case 0x0'02'8b: return 0x0'01'b2;
				case 0x0'01'b4: return 0x0'01'b3;
				case 0x0'01'b6: return 0x0'01'b5;
				case 0x0'02'92: return 0x0'01'b7;
				case 0x0'01'b9: return 0x0'01'b8;
				case 0x0'01'bd: return 0x0'01'bc;
				case 0x0'01'c6: return 0x0'01'c4;
				case 0x0'01'c9: return 0x0'01'c7;
				case 0x0'01'cc: return 0x0'01'ca;
				case 0x0'01'ce: return 0x0'01'cd;
				case 0x0'01'd0: return 0x0'01'cf;
				case 0x0'01'd2: return 0x0'01'd1;
				case 0x0'01'd4: return 0x0'01'd3;
				case 0x0'01'd6: return 0x0'01'd5;
				case 0x0'01'd8: return 0x0'01'd7;
				case 0x0'01'da: return 0x0'01'd9;
				case 0x0'01'dc: return 0x0'01'db;
				case 0x0'01'df: return 0x0'01'de;
				case 0x0'01'e1: return 0x0'01'e0;
				case 0x0'01'e3: return 0x0'01'e2;
				case 0x0'01'e5: return 0x0'01'e4;
				case 0x0'01'e7: return 0x0'01'e6;
				case 0x0'01'e9: return 0x0'01'e8;
				case 0x0'01'eb: return 0x0'01'ea;
				case 0x0'01'ed: return 0x0'01'ec;
				case 0x0'01'ef: return 0x0'01'ee;
				case 0x0'01'f3: return 0x0'01'f1;
				case 0x0'01'f5: return 0x0'01'f4;
				case 0x0'01'95: return 0x0'01'f6;
				case 0x0'01'bf: return 0x0'01'f7;
				case 0x0'01'f9: return 0x0'01'f8;
				case 0x0'01'fb: return 0x0'01'fa;
				case 0x0'01'fd: return 0x0'01'fc;
				case 0x0'01'ff: return 0x0'01'fe;
				case 0x0'02'01: return 0x0'02'00;
				case 0x0'02'03: return 0x0'02'02;
				case 0x0'02'05: return 0x0'02'04;
				case 0x0'02'07: return 0x0'02'06;
				case 0x0'02'09: return 0x0'02'08;
				case 0x0'02'0b: return 0x0'02'0a;
				case 0x0'02'0d: return 0x0'02'0c;
				case 0x0'02'0f: return 0x0'02'0e;
				case 0x0'02'11: return 0x0'02'10;
				case 0x0'02'13: return 0x0'02'12;
				case 0x0'02'15: return 0x0'02'14;
				case 0x0'02'17: return 0x0'02'16;
				case 0x0'02'19: return 0x0'02'18;
				case 0x0'02'1b: return 0x0'02'1a;
				case 0x0'02'1d: return 0x0'02'1c;
				case 0x0'02'1f: return 0x0'02'1e;
				case 0x0'01'9e: return 0x0'02'20;
				case 0x0'02'23: return 0x0'02'22;
				case 0x0'02'25: return 0x0'02'24;
				case 0x0'02'27: return 0x0'02'26;
				case 0x0'02'29: return 0x0'02'28;
				case 0x0'02'2b: return 0x0'02'2a;
				case 0x0'02'2d: return 0x0'02'2c;
				case 0x0'02'2f: return 0x0'02'2e;
				case 0x0'02'31: return 0x0'02'30;
				case 0x0'02'33: return 0x0'02'32;
				case 0x0'2c'65: return 0x0'02'3a;
				case 0x0'02'3c: return 0x0'02'3b;
				case 0x0'01'9a: return 0x0'02'3d;
				case 0x0'2c'66: return 0x0'02'3e;
				case 0x0'02'42: return 0x0'02'41;
				case 0x0'01'80: return 0x0'02'43;
				case 0x0'02'89: return 0x0'02'44;
				case 0x0'02'8c: return 0x0'02'45;
				case 0x0'02'47: return 0x0'02'46;
				case 0x0'02'49: return 0x0'02'48;
				case 0x0'02'4b: return 0x0'02'4a;
				case 0x0'02'4d: return 0x0'02'4c;
				case 0x0'02'4f: return 0x0'02'4e;
				case 0x0'03'71: return 0x0'03'70;
				case 0x0'03'73: return 0x0'03'72;
				case 0x0'03'77: return 0x0'03'76;
				case 0x0'03'f3: return 0x0'03'7f;
				case 0x0'03'ac: return 0x0'03'86;
				case 0x0'03'ad: return 0x0'03'88;
				case 0x0'03'ae: return 0x0'03'89;
				case 0x0'03'af: return 0x0'03'8a;
				case 0x0'03'cc: return 0x0'03'8c;
				case 0x0'03'cd: return 0x0'03'8e;
				case 0x0'03'ce: return 0x0'03'8f;
				case 0x0'03'b1: return 0x0'03'91;
				case 0x0'03'b2: return 0x0'03'92;
				case 0x0'03'b3: return 0x0'03'93;
				case 0x0'03'b4: return 0x0'03'94;
				case 0x0'03'b5: return 0x0'03'95;
				case 0x0'03'b6: return 0x0'03'96;
				case 0x0'03'b7: return 0x0'03'97;
				case 0x0'03'b8: return 0x0'03'98;
				case 0x0'03'b9: return 0x0'03'99;
				case 0x0'03'ba: return 0x0'03'9a;
				case 0x0'03'bb: return 0x0'03'9b;
				case 0x0'03'bc: return 0x0'03'9c;
				case 0x0'03'bd: return 0x0'03'9d;
				case 0x0'03'be: return 0x0'03'9e;
				case 0x0'03'bf: return 0x0'03'9f;
				case 0x0'03'c0: return 0x0'03'a0;
				case 0x0'03'c1: return 0x0'03'a1;
				case 0x0'03'c3: return 0x0'03'a3;
				case 0x0'03'c4: return 0x0'03'a4;
				case 0x0'03'c5: return 0x0'03'a5;
				case 0x0'03'c6: return 0x0'03'a6;
				case 0x0'03'c7: return 0x0'03'a7;
				case 0x0'03'c8: return 0x0'03'a8;
				case 0x0'03'c9: return 0x0'03'a9;
				case 0x0'03'ca: return 0x0'03'aa;
				case 0x0'03'cb: return 0x0'03'ab;
				case 0x0'03'd7: return 0x0'03'cf;
				case 0x0'03'd9: return 0x0'03'd8;
				case 0x0'03'db: return 0x0'03'da;
				case 0x0'03'dd: return 0x0'03'dc;
				case 0x0'03'df: return 0x0'03'de;
				case 0x0'03'e1: return 0x0'03'e0;
				case 0x0'03'e3: return 0x0'03'e2;
				case 0x0'03'e5: return 0x0'03'e4;
				case 0x0'03'e7: return 0x0'03'e6;
				case 0x0'03'e9: return 0x0'03'e8;
				case 0x0'03'eb: return 0x0'03'ea;
				case 0x0'03'ed: return 0x0'03'ec;
				case 0x0'03'ef: return 0x0'03'ee;
				case 0x0'03'f8: return 0x0'03'f7;
				case 0x0'03'f2: return 0x0'03'f9;
				case 0x0'03'fb: return 0x0'03'fa;
				case 0x0'03'7b: return 0x0'03'fd;
				case 0x0'03'7c: return 0x0'03'fe;
				case 0x0'03'7d: return 0x0'03'ff;
				case 0x0'04'50: return 0x0'04'00;
				case 0x0'04'51: return 0x0'04'01;
				case 0x0'04'52: return 0x0'04'02;
				case 0x0'04'53: return 0x0'04'03;
				case 0x0'04'54: return 0x0'04'04;
				case 0x0'04'55: return 0x0'04'05;
				case 0x0'04'56: return 0x0'04'06;
				case 0x0'04'57: return 0x0'04'07;
				case 0x0'04'58: return 0x0'04'08;
				case 0x0'04'59: return 0x0'04'09;
				case 0x0'04'5a: return 0x0'04'0a;
				case 0x0'04'5b: return 0x0'04'0b;
				case 0x0'04'5c: return 0x0'04'0c;
				case 0x0'04'5d: return 0x0'04'0d;
				case 0x0'04'5e: return 0x0'04'0e;
				case 0x0'04'5f: return 0x0'04'0f;
				case 0x0'04'30: return 0x0'04'10;
				case 0x0'04'31: return 0x0'04'11;
				case 0x0'04'32: return 0x0'04'12;
				case 0x0'04'33: return 0x0'04'13;
				case 0x0'04'34: return 0x0'04'14;
				case 0x0'04'35: return 0x0'04'15;
				case 0x0'04'36: return 0x0'04'16;
				case 0x0'04'37: return 0x0'04'17;
				case 0x0'04'38: return 0x0'04'18;
				case 0x0'04'39: return 0x0'04'19;
				case 0x0'04'3a: return 0x0'04'1a;
				case 0x0'04'3b: return 0x0'04'1b;
				case 0x0'04'3c: return 0x0'04'1c;
				case 0x0'04'3d: return 0x0'04'1d;
				case 0x0'04'3e: return 0x0'04'1e;
				case 0x0'04'3f: return 0x0'04'1f;
				case 0x0'04'40: return 0x0'04'20;
				case 0x0'04'41: return 0x0'04'21;
				case 0x0'04'42: return 0x0'04'22;
				case 0x0'04'43: return 0x0'04'23;
				case 0x0'04'44: return 0x0'04'24;
				case 0x0'04'45: return 0x0'04'25;
				case 0x0'04'46: return 0x0'04'26;
				case 0x0'04'47: return 0x0'04'27;
				case 0x0'04'48: return 0x0'04'28;
				case 0x0'04'49: return 0x0'04'29;
				case 0x0'04'4a: return 0x0'04'2a;
				case 0x0'04'4b: return 0x0'04'2b;
				case 0x0'04'4c: return 0x0'04'2c;
				case 0x0'04'4d: return 0x0'04'2d;
				case 0x0'04'4e: return 0x0'04'2e;
				case 0x0'04'4f: return 0x0'04'2f;
				case 0x0'04'61: return 0x0'04'60;
				case 0x0'04'63: return 0x0'04'62;
				case 0x0'04'65: return 0x0'04'64;
				case 0x0'04'67: return 0x0'04'66;
				case 0x0'04'69: return 0x0'04'68;
				case 0x0'04'6b: return 0x0'04'6a;
				case 0x0'04'6d: return 0x0'04'6c;
				case 0x0'04'6f: return 0x0'04'6e;
				case 0x0'04'71: return 0x0'04'70;
				case 0x0'04'73: return 0x0'04'72;
				case 0x0'04'75: return 0x0'04'74;
				case 0x0'04'77: return 0x0'04'76;
				case 0x0'04'79: return 0x0'04'78;
				case 0x0'04'7b: return 0x0'04'7a;
				case 0x0'04'7d: return 0x0'04'7c;
				case 0x0'04'7f: return 0x0'04'7e;
				case 0x0'04'81: return 0x0'04'80;
				case 0x0'04'8b: return 0x0'04'8a;
				case 0x0'04'8d: return 0x0'04'8c;
				case 0x0'04'8f: return 0x0'04'8e;
				case 0x0'04'91: return 0x0'04'90;
				case 0x0'04'93: return 0x0'04'92;
				case 0x0'04'95: return 0x0'04'94;
				case 0x0'04'97: return 0x0'04'96;
				case 0x0'04'99: return 0x0'04'98;
				case 0x0'04'9b: return 0x0'04'9a;
				case 0x0'04'9d: return 0x0'04'9c;
				case 0x0'04'9f: return 0x0'04'9e;
				case 0x0'04'a1: return 0x0'04'a0;
				case 0x0'04'a3: return 0x0'04'a2;
				case 0x0'04'a5: return 0x0'04'a4;
				case 0x0'04'a7: return 0x0'04'a6;
				case 0x0'04'a9: return 0x0'04'a8;
				case 0x0'04'ab: return 0x0'04'aa;
				case 0x0'04'ad: return 0x0'04'ac;
				case 0x0'04'af: return 0x0'04'ae;
				case 0x0'04'b1: return 0x0'04'b0;
				case 0x0'04'b3: return 0x0'04'b2;
				case 0x0'04'b5: return 0x0'04'b4;
				case 0x0'04'b7: return 0x0'04'b6;
				case 0x0'04'b9: return 0x0'04'b8;
				case 0x0'04'bb: return 0x0'04'ba;
				case 0x0'04'bd: return 0x0'04'bc;
				case 0x0'04'bf: return 0x0'04'be;
				case 0x0'04'cf: return 0x0'04'c0;
				case 0x0'04'c2: return 0x0'04'c1;
				case 0x0'04'c4: return 0x0'04'c3;
				case 0x0'04'c6: return 0x0'04'c5;
				case 0x0'04'c8: return 0x0'04'c7;
				case 0x0'04'ca: return 0x0'04'c9;
				case 0x0'04'cc: return 0x0'04'cb;
				case 0x0'04'ce: return 0x0'04'cd;
				case 0x0'04'd1: return 0x0'04'd0;
				case 0x0'04'd3: return 0x0'04'd2;
				case 0x0'04'd5: return 0x0'04'd4;
				case 0x0'04'd7: return 0x0'04'd6;
				case 0x0'04'd9: return 0x0'04'd8;
				case 0x0'04'db: return 0x0'04'da;
				case 0x0'04'dd: return 0x0'04'dc;
				case 0x0'04'df: return 0x0'04'de;
				case 0x0'04'e1: return 0x0'04'e0;
				case 0x0'04'e3: return 0x0'04'e2;
				case 0x0'04'e5: return 0x0'04'e4;
				case 0x0'04'e7: return 0x0'04'e6;
				case 0x0'04'e9: return 0x0'04'e8;
				case 0x0'04'eb: return 0x0'04'ea;
				case 0x0'04'ed: return 0x0'04'ec;
				case 0x0'04'ef: return 0x0'04'ee;
				case 0x0'04'f1: return 0x0'04'f0;
				case 0x0'04'f3: return 0x0'04'f2;
				case 0x0'04'f5: return 0x0'04'f4;
				case 0x0'04'f7: return 0x0'04'f6;
				case 0x0'04'f9: return 0x0'04'f8;
				case 0x0'04'fb: return 0x0'04'fa;
				case 0x0'04'fd: return 0x0'04'fc;
				case 0x0'04'ff: return 0x0'04'fe;
				case 0x0'05'01: return 0x0'05'00;
				case 0x0'05'03: return 0x0'05'02;
				case 0x0'05'05: return 0x0'05'04;
				case 0x0'05'07: return 0x0'05'06;
				case 0x0'05'09: return 0x0'05'08;
				case 0x0'05'0b: return 0x0'05'0a;
				case 0x0'05'0d: return 0x0'05'0c;
				case 0x0'05'0f: return 0x0'05'0e;
				case 0x0'05'11: return 0x0'05'10;
				case 0x0'05'13: return 0x0'05'12;
				case 0x0'05'15: return 0x0'05'14;
				case 0x0'05'17: return 0x0'05'16;
				case 0x0'05'19: return 0x0'05'18;
				case 0x0'05'1b: return 0x0'05'1a;
				case 0x0'05'1d: return 0x0'05'1c;
				case 0x0'05'1f: return 0x0'05'1e;
				case 0x0'05'21: return 0x0'05'20;
				case 0x0'05'23: return 0x0'05'22;
				case 0x0'05'25: return 0x0'05'24;
				case 0x0'05'27: return 0x0'05'26;
				case 0x0'05'29: return 0x0'05'28;
				case 0x0'05'2b: return 0x0'05'2a;
				case 0x0'05'2d: return 0x0'05'2c;
				case 0x0'05'2f: return 0x0'05'2e;
				case 0x0'05'61: return 0x0'05'31;
				case 0x0'05'62: return 0x0'05'32;
				case 0x0'05'63: return 0x0'05'33;
				case 0x0'05'64: return 0x0'05'34;
				case 0x0'05'65: return 0x0'05'35;
				case 0x0'05'66: return 0x0'05'36;
				case 0x0'05'67: return 0x0'05'37;
				case 0x0'05'68: return 0x0'05'38;
				case 0x0'05'69: return 0x0'05'39;
				case 0x0'05'6a: return 0x0'05'3a;
				case 0x0'05'6b: return 0x0'05'3b;
				case 0x0'05'6c: return 0x0'05'3c;
				case 0x0'05'6d: return 0x0'05'3d;
				case 0x0'05'6e: return 0x0'05'3e;
				case 0x0'05'6f: return 0x0'05'3f;
				case 0x0'05'70: return 0x0'05'40;
				case 0x0'05'71: return 0x0'05'41;
				case 0x0'05'72: return 0x0'05'42;
				case 0x0'05'73: return 0x0'05'43;
				case 0x0'05'74: return 0x0'05'44;
				case 0x0'05'75: return 0x0'05'45;
				case 0x0'05'76: return 0x0'05'46;
				case 0x0'05'77: return 0x0'05'47;
				case 0x0'05'78: return 0x0'05'48;
				case 0x0'05'79: return 0x0'05'49;
				case 0x0'05'7a: return 0x0'05'4a;
				case 0x0'05'7b: return 0x0'05'4b;
				case 0x0'05'7c: return 0x0'05'4c;
				case 0x0'05'7d: return 0x0'05'4d;
				case 0x0'05'7e: return 0x0'05'4e;
				case 0x0'05'7f: return 0x0'05'4f;
				case 0x0'05'80: return 0x0'05'50;
				case 0x0'05'81: return 0x0'05'51;
				case 0x0'05'82: return 0x0'05'52;
				case 0x0'05'83: return 0x0'05'53;
				case 0x0'05'84: return 0x0'05'54;
				case 0x0'05'85: return 0x0'05'55;
				case 0x0'05'86: return 0x0'05'56;
				case 0x0'2d'00: return 0x0'10'a0;
				case 0x0'2d'01: return 0x0'10'a1;
				case 0x0'2d'02: return 0x0'10'a2;
				case 0x0'2d'03: return 0x0'10'a3;
				case 0x0'2d'04: return 0x0'10'a4;
				case 0x0'2d'05: return 0x0'10'a5;
				case 0x0'2d'06: return 0x0'10'a6;
				case 0x0'2d'07: return 0x0'10'a7;
				case 0x0'2d'08: return 0x0'10'a8;
				case 0x0'2d'09: return 0x0'10'a9;
				case 0x0'2d'0a: return 0x0'10'aa;
				case 0x0'2d'0b: return 0x0'10'ab;
				case 0x0'2d'0c: return 0x0'10'ac;
				case 0x0'2d'0d: return 0x0'10'ad;
				case 0x0'2d'0e: return 0x0'10'ae;
				case 0x0'2d'0f: return 0x0'10'af;
				case 0x0'2d'10: return 0x0'10'b0;
				case 0x0'2d'11: return 0x0'10'b1;
				case 0x0'2d'12: return 0x0'10'b2;
				case 0x0'2d'13: return 0x0'10'b3;
				case 0x0'2d'14: return 0x0'10'b4;
				case 0x0'2d'15: return 0x0'10'b5;
				case 0x0'2d'16: return 0x0'10'b6;
				case 0x0'2d'17: return 0x0'10'b7;
				case 0x0'2d'18: return 0x0'10'b8;
				case 0x0'2d'19: return 0x0'10'b9;
				case 0x0'2d'1a: return 0x0'10'ba;
				case 0x0'2d'1b: return 0x0'10'bb;
				case 0x0'2d'1c: return 0x0'10'bc;
				case 0x0'2d'1d: return 0x0'10'bd;
				case 0x0'2d'1e: return 0x0'10'be;
				case 0x0'2d'1f: return 0x0'10'bf;
				case 0x0'2d'20: return 0x0'10'c0;
				case 0x0'2d'21: return 0x0'10'c1;
				case 0x0'2d'22: return 0x0'10'c2;
				case 0x0'2d'23: return 0x0'10'c3;
				case 0x0'2d'24: return 0x0'10'c4;
				case 0x0'2d'25: return 0x0'10'c5;
				case 0x0'2d'27: return 0x0'10'c7;
				case 0x0'2d'2d: return 0x0'10'cd;
				case 0x0'13'f0: return 0x0'13'f8;
				case 0x0'13'f1: return 0x0'13'f9;
				case 0x0'13'f2: return 0x0'13'fa;
				case 0x0'13'f3: return 0x0'13'fb;
				case 0x0'13'f4: return 0x0'13'fc;
				case 0x0'13'f5: return 0x0'13'fd;
				case 0x0'10'd0: return 0x0'1c'90;
				case 0x0'10'd1: return 0x0'1c'91;
				case 0x0'10'd2: return 0x0'1c'92;
				case 0x0'10'd3: return 0x0'1c'93;
				case 0x0'10'd4: return 0x0'1c'94;
				case 0x0'10'd5: return 0x0'1c'95;
				case 0x0'10'd6: return 0x0'1c'96;
				case 0x0'10'd7: return 0x0'1c'97;
				case 0x0'10'd8: return 0x0'1c'98;
				case 0x0'10'd9: return 0x0'1c'99;
				case 0x0'10'da: return 0x0'1c'9a;
				case 0x0'10'db: return 0x0'1c'9b;
				case 0x0'10'dc: return 0x0'1c'9c;
				case 0x0'10'dd: return 0x0'1c'9d;
				case 0x0'10'de: return 0x0'1c'9e;
				case 0x0'10'df: return 0x0'1c'9f;
				case 0x0'10'e0: return 0x0'1c'a0;
				case 0x0'10'e1: return 0x0'1c'a1;
				case 0x0'10'e2: return 0x0'1c'a2;
				case 0x0'10'e3: return 0x0'1c'a3;
				case 0x0'10'e4: return 0x0'1c'a4;
				case 0x0'10'e5: return 0x0'1c'a5;
				case 0x0'10'e6: return 0x0'1c'a6;
				case 0x0'10'e7: return 0x0'1c'a7;
				case 0x0'10'e8: return 0x0'1c'a8;
				case 0x0'10'e9: return 0x0'1c'a9;
				case 0x0'10'ea: return 0x0'1c'aa;
				case 0x0'10'eb: return 0x0'1c'ab;
				case 0x0'10'ec: return 0x0'1c'ac;
				case 0x0'10'ed: return 0x0'1c'ad;
				case 0x0'10'ee: return 0x0'1c'ae;
				case 0x0'10'ef: return 0x0'1c'af;
				case 0x0'10'f0: return 0x0'1c'b0;
				case 0x0'10'f1: return 0x0'1c'b1;
				case 0x0'10'f2: return 0x0'1c'b2;
				case 0x0'10'f3: return 0x0'1c'b3;
				case 0x0'10'f4: return 0x0'1c'b4;
				case 0x0'10'f5: return 0x0'1c'b5;
				case 0x0'10'f6: return 0x0'1c'b6;
				case 0x0'10'f7: return 0x0'1c'b7;
				case 0x0'10'f8: return 0x0'1c'b8;
				case 0x0'10'f9: return 0x0'1c'b9;
				case 0x0'10'fa: return 0x0'1c'ba;
				case 0x0'10'fd: return 0x0'1c'bd;
				case 0x0'10'fe: return 0x0'1c'be;
				case 0x0'10'ff: return 0x0'1c'bf;
				case 0x0'1e'01: return 0x0'1e'00;
				case 0x0'1e'03: return 0x0'1e'02;
				case 0x0'1e'05: return 0x0'1e'04;
				case 0x0'1e'07: return 0x0'1e'06;
				case 0x0'1e'09: return 0x0'1e'08;
				case 0x0'1e'0b: return 0x0'1e'0a;
				case 0x0'1e'0d: return 0x0'1e'0c;
				case 0x0'1e'0f: return 0x0'1e'0e;
				case 0x0'1e'11: return 0x0'1e'10;
				case 0x0'1e'13: return 0x0'1e'12;
				case 0x0'1e'15: return 0x0'1e'14;
				case 0x0'1e'17: return 0x0'1e'16;
				case 0x0'1e'19: return 0x0'1e'18;
				case 0x0'1e'1b: return 0x0'1e'1a;
				case 0x0'1e'1d: return 0x0'1e'1c;
				case 0x0'1e'1f: return 0x0'1e'1e;
				case 0x0'1e'21: return 0x0'1e'20;
				case 0x0'1e'23: return 0x0'1e'22;
				case 0x0'1e'25: return 0x0'1e'24;
				case 0x0'1e'27: return 0x0'1e'26;
				case 0x0'1e'29: return 0x0'1e'28;
				case 0x0'1e'2b: return 0x0'1e'2a;
				case 0x0'1e'2d: return 0x0'1e'2c;
				case 0x0'1e'2f: return 0x0'1e'2e;
				case 0x0'1e'31: return 0x0'1e'30;
				case 0x0'1e'33: return 0x0'1e'32;
				case 0x0'1e'35: return 0x0'1e'34;
				case 0x0'1e'37: return 0x0'1e'36;
				case 0x0'1e'39: return 0x0'1e'38;
				case 0x0'1e'3b: return 0x0'1e'3a;
				case 0x0'1e'3d: return 0x0'1e'3c;
				case 0x0'1e'3f: return 0x0'1e'3e;
				case 0x0'1e'41: return 0x0'1e'40;
				case 0x0'1e'43: return 0x0'1e'42;
				case 0x0'1e'45: return 0x0'1e'44;
				case 0x0'1e'47: return 0x0'1e'46;
				case 0x0'1e'49: return 0x0'1e'48;
				case 0x0'1e'4b: return 0x0'1e'4a;
				case 0x0'1e'4d: return 0x0'1e'4c;
				case 0x0'1e'4f: return 0x0'1e'4e;
				case 0x0'1e'51: return 0x0'1e'50;
				case 0x0'1e'53: return 0x0'1e'52;
				case 0x0'1e'55: return 0x0'1e'54;
				case 0x0'1e'57: return 0x0'1e'56;
				case 0x0'1e'59: return 0x0'1e'58;
				case 0x0'1e'5b: return 0x0'1e'5a;
				case 0x0'1e'5d: return 0x0'1e'5c;
				case 0x0'1e'5f: return 0x0'1e'5e;
				case 0x0'1e'61: return 0x0'1e'60;
				case 0x0'1e'63: return 0x0'1e'62;
				case 0x0'1e'65: return 0x0'1e'64;
				case 0x0'1e'67: return 0x0'1e'66;
				case 0x0'1e'69: return 0x0'1e'68;
				case 0x0'1e'6b: return 0x0'1e'6a;
				case 0x0'1e'6d: return 0x0'1e'6c;
				case 0x0'1e'6f: return 0x0'1e'6e;
				case 0x0'1e'71: return 0x0'1e'70;
				case 0x0'1e'73: return 0x0'1e'72;
				case 0x0'1e'75: return 0x0'1e'74;
				case 0x0'1e'77: return 0x0'1e'76;
				case 0x0'1e'79: return 0x0'1e'78;
				case 0x0'1e'7b: return 0x0'1e'7a;
				case 0x0'1e'7d: return 0x0'1e'7c;
				case 0x0'1e'7f: return 0x0'1e'7e;
				case 0x0'1e'81: return 0x0'1e'80;
				case 0x0'1e'83: return 0x0'1e'82;
				case 0x0'1e'85: return 0x0'1e'84;
				case 0x0'1e'87: return 0x0'1e'86;
				case 0x0'1e'89: return 0x0'1e'88;
				case 0x0'1e'8b: return 0x0'1e'8a;
				case 0x0'1e'8d: return 0x0'1e'8c;
				case 0x0'1e'8f: return 0x0'1e'8e;
				case 0x0'1e'91: return 0x0'1e'90;
				case 0x0'1e'93: return 0x0'1e'92;
				case 0x0'1e'95: return 0x0'1e'94;
				case 0x0'00'df: return 0x0'1e'9e;
				case 0x0'1e'a1: return 0x0'1e'a0;
				case 0x0'1e'a3: return 0x0'1e'a2;
				case 0x0'1e'a5: return 0x0'1e'a4;
				case 0x0'1e'a7: return 0x0'1e'a6;
				case 0x0'1e'a9: return 0x0'1e'a8;
				case 0x0'1e'ab: return 0x0'1e'aa;
				case 0x0'1e'ad: return 0x0'1e'ac;
				case 0x0'1e'af: return 0x0'1e'ae;
				case 0x0'1e'b1: return 0x0'1e'b0;
				case 0x0'1e'b3: return 0x0'1e'b2;
				case 0x0'1e'b5: return 0x0'1e'b4;
				case 0x0'1e'b7: return 0x0'1e'b6;
				case 0x0'1e'b9: return 0x0'1e'b8;
				case 0x0'1e'bb: return 0x0'1e'ba;
				case 0x0'1e'bd: return 0x0'1e'bc;
				case 0x0'1e'bf: return 0x0'1e'be;
				case 0x0'1e'c1: return 0x0'1e'c0;
				case 0x0'1e'c3: return 0x0'1e'c2;
				case 0x0'1e'c5: return 0x0'1e'c4;
				case 0x0'1e'c7: return 0x0'1e'c6;
				case 0x0'1e'c9: return 0x0'1e'c8;
				case 0x0'1e'cb: return 0x0'1e'ca;
				case 0x0'1e'cd: return 0x0'1e'cc;
				case 0x0'1e'cf: return 0x0'1e'ce;
				case 0x0'1e'd1: return 0x0'1e'd0;
				case 0x0'1e'd3: return 0x0'1e'd2;
				case 0x0'1e'd5: return 0x0'1e'd4;
				case 0x0'1e'd7: return 0x0'1e'd6;
				case 0x0'1e'd9: return 0x0'1e'd8;
				case 0x0'1e'db: return 0x0'1e'da;
				case 0x0'1e'dd: return 0x0'1e'dc;
				case 0x0'1e'df: return 0x0'1e'de;
				case 0x0'1e'e1: return 0x0'1e'e0;
				case 0x0'1e'e3: return 0x0'1e'e2;
				case 0x0'1e'e5: return 0x0'1e'e4;
				case 0x0'1e'e7: return 0x0'1e'e6;
				case 0x0'1e'e9: return 0x0'1e'e8;
				case 0x0'1e'eb: return 0x0'1e'ea;
				case 0x0'1e'ed: return 0x0'1e'ec;
				case 0x0'1e'ef: return 0x0'1e'ee;
				case 0x0'1e'f1: return 0x0'1e'f0;
				case 0x0'1e'f3: return 0x0'1e'f2;
				case 0x0'1e'f5: return 0x0'1e'f4;
				case 0x0'1e'f7: return 0x0'1e'f6;
				case 0x0'1e'f9: return 0x0'1e'f8;
				case 0x0'1e'fb: return 0x0'1e'fa;
				case 0x0'1e'fd: return 0x0'1e'fc;
				case 0x0'1e'ff: return 0x0'1e'fe;
				case 0x0'1f'00: return 0x0'1f'08;
				case 0x0'1f'01: return 0x0'1f'09;
				case 0x0'1f'02: return 0x0'1f'0a;
				case 0x0'1f'03: return 0x0'1f'0b;
				case 0x0'1f'04: return 0x0'1f'0c;
				case 0x0'1f'05: return 0x0'1f'0d;
				case 0x0'1f'06: return 0x0'1f'0e;
				case 0x0'1f'07: return 0x0'1f'0f;
				case 0x0'1f'10: return 0x0'1f'18;
				case 0x0'1f'11: return 0x0'1f'19;
				case 0x0'1f'12: return 0x0'1f'1a;
				case 0x0'1f'13: return 0x0'1f'1b;
				case 0x0'1f'14: return 0x0'1f'1c;
				case 0x0'1f'15: return 0x0'1f'1d;
				case 0x0'1f'20: return 0x0'1f'28;
				case 0x0'1f'21: return 0x0'1f'29;
				case 0x0'1f'22: return 0x0'1f'2a;
				case 0x0'1f'23: return 0x0'1f'2b;
				case 0x0'1f'24: return 0x0'1f'2c;
				case 0x0'1f'25: return 0x0'1f'2d;
				case 0x0'1f'26: return 0x0'1f'2e;
				case 0x0'1f'27: return 0x0'1f'2f;
				case 0x0'1f'30: return 0x0'1f'38;
				case 0x0'1f'31: return 0x0'1f'39;
				case 0x0'1f'32: return 0x0'1f'3a;
				case 0x0'1f'33: return 0x0'1f'3b;
				case 0x0'1f'34: return 0x0'1f'3c;
				case 0x0'1f'35: return 0x0'1f'3d;
				case 0x0'1f'36: return 0x0'1f'3e;
				case 0x0'1f'37: return 0x0'1f'3f;
				case 0x0'1f'40: return 0x0'1f'48;
				case 0x0'1f'41: return 0x0'1f'49;
				case 0x0'1f'42: return 0x0'1f'4a;
				case 0x0'1f'43: return 0x0'1f'4b;
				case 0x0'1f'44: return 0x0'1f'4c;
				case 0x0'1f'45: return 0x0'1f'4d;
				case 0x0'1f'51: return 0x0'1f'59;
				case 0x0'1f'53: return 0x0'1f'5b;
				case 0x0'1f'55: return 0x0'1f'5d;
				case 0x0'1f'57: return 0x0'1f'5f;
				case 0x0'1f'60: return 0x0'1f'68;
				case 0x0'1f'61: return 0x0'1f'69;
				case 0x0'1f'62: return 0x0'1f'6a;
				case 0x0'1f'63: return 0x0'1f'6b;
				case 0x0'1f'64: return 0x0'1f'6c;
				case 0x0'1f'65: return 0x0'1f'6d;
				case 0x0'1f'66: return 0x0'1f'6e;
				case 0x0'1f'67: return 0x0'1f'6f;
				case 0x0'1f'80: return 0x0'1f'88;
				case 0x0'1f'81: return 0x0'1f'89;
				case 0x0'1f'82: return 0x0'1f'8a;
				case 0x0'1f'83: return 0x0'1f'8b;
				case 0x0'1f'84: return 0x0'1f'8c;
				case 0x0'1f'85: return 0x0'1f'8d;
				case 0x0'1f'86: return 0x0'1f'8e;
				case 0x0'1f'87: return 0x0'1f'8f;
				case 0x0'1f'90: return 0x0'1f'98;
				case 0x0'1f'91: return 0x0'1f'99;
				case 0x0'1f'92: return 0x0'1f'9a;
				case 0x0'1f'93: return 0x0'1f'9b;
				case 0x0'1f'94: return 0x0'1f'9c;
				case 0x0'1f'95: return 0x0'1f'9d;
				case 0x0'1f'96: return 0x0'1f'9e;
				case 0x0'1f'97: return 0x0'1f'9f;
				case 0x0'1f'a0: return 0x0'1f'a8;
				case 0x0'1f'a1: return 0x0'1f'a9;
				case 0x0'1f'a2: return 0x0'1f'aa;
				case 0x0'1f'a3: return 0x0'1f'ab;
				case 0x0'1f'a4: return 0x0'1f'ac;
				case 0x0'1f'a5: return 0x0'1f'ad;
				case 0x0'1f'a6: return 0x0'1f'ae;
				case 0x0'1f'a7: return 0x0'1f'af;
				case 0x0'1f'b0: return 0x0'1f'b8;
				case 0x0'1f'b1: return 0x0'1f'b9;
				case 0x0'1f'70: return 0x0'1f'ba;
				case 0x0'1f'71: return 0x0'1f'bb;
				case 0x0'1f'b3: return 0x0'1f'bc;
				case 0x0'1f'72: return 0x0'1f'c8;
				case 0x0'1f'73: return 0x0'1f'c9;
				case 0x0'1f'74: return 0x0'1f'ca;
				case 0x0'1f'75: return 0x0'1f'cb;
				case 0x0'1f'c3: return 0x0'1f'cc;
				case 0x0'1f'd0: return 0x0'1f'd8;
				case 0x0'1f'd1: return 0x0'1f'd9;
				case 0x0'1f'76: return 0x0'1f'da;
				case 0x0'1f'77: return 0x0'1f'db;
				case 0x0'1f'e0: return 0x0'1f'e8;
				case 0x0'1f'e1: return 0x0'1f'e9;
				case 0x0'1f'7a: return 0x0'1f'ea;
				case 0x0'1f'7b: return 0x0'1f'eb;
				case 0x0'1f'e5: return 0x0'1f'ec;
				case 0x0'1f'78: return 0x0'1f'f8;
				case 0x0'1f'79: return 0x0'1f'f9;
				case 0x0'1f'7c: return 0x0'1f'fa;
				case 0x0'1f'7d: return 0x0'1f'fb;
				case 0x0'1f'f3: return 0x0'1f'fc;
				case 0x0'21'4e: return 0x0'21'32;
				case 0x0'21'70: return 0x0'21'60;
				case 0x0'21'71: return 0x0'21'61;
				case 0x0'21'72: return 0x0'21'62;
				case 0x0'21'73: return 0x0'21'63;
				case 0x0'21'74: return 0x0'21'64;
				case 0x0'21'75: return 0x0'21'65;
				case 0x0'21'76: return 0x0'21'66;
				case 0x0'21'77: return 0x0'21'67;
				case 0x0'21'78: return 0x0'21'68;
				case 0x0'21'79: return 0x0'21'69;
				case 0x0'21'7a: return 0x0'21'6a;
				case 0x0'21'7b: return 0x0'21'6b;
				case 0x0'21'7c: return 0x0'21'6c;
				case 0x0'21'7d: return 0x0'21'6d;
				case 0x0'21'7e: return 0x0'21'6e;
				case 0x0'21'7f: return 0x0'21'6f;
				case 0x0'21'84: return 0x0'21'83;
				case 0x0'24'd0: return 0x0'24'b6;
				case 0x0'24'd1: return 0x0'24'b7;
				case 0x0'24'd2: return 0x0'24'b8;
				case 0x0'24'd3: return 0x0'24'b9;
				case 0x0'24'd4: return 0x0'24'ba;
				case 0x0'24'd5: return 0x0'24'bb;
				case 0x0'24'd6: return 0x0'24'bc;
				case 0x0'24'd7: return 0x0'24'bd;
				case 0x0'24'd8: return 0x0'24'be;
				case 0x0'24'd9: return 0x0'24'bf;
				case 0x0'24'da: return 0x0'24'c0;
				case 0x0'24'db: return 0x0'24'c1;
				case 0x0'24'dc: return 0x0'24'c2;
				case 0x0'24'dd: return 0x0'24'c3;
				case 0x0'24'de: return 0x0'24'c4;
				case 0x0'24'df: return 0x0'24'c5;
				case 0x0'24'e0: return 0x0'24'c6;
				case 0x0'24'e1: return 0x0'24'c7;
				case 0x0'24'e2: return 0x0'24'c8;
				case 0x0'24'e3: return 0x0'24'c9;
				case 0x0'24'e4: return 0x0'24'ca;
				case 0x0'24'e5: return 0x0'24'cb;
				case 0x0'24'e6: return 0x0'24'cc;
				case 0x0'24'e7: return 0x0'24'cd;
				case 0x0'24'e8: return 0x0'24'ce;
				case 0x0'24'e9: return 0x0'24'cf;
				case 0x0'2c'30: return 0x0'2c'00;
				case 0x0'2c'31: return 0x0'2c'01;
				case 0x0'2c'32: return 0x0'2c'02;
				case 0x0'2c'33: return 0x0'2c'03;
				case 0x0'2c'34: return 0x0'2c'04;
				case 0x0'2c'35: return 0x0'2c'05;
				case 0x0'2c'36: return 0x0'2c'06;
				case 0x0'2c'37: return 0x0'2c'07;
				case 0x0'2c'38: return 0x0'2c'08;
				case 0x0'2c'39: return 0x0'2c'09;
				case 0x0'2c'3a: return 0x0'2c'0a;
				case 0x0'2c'3b: return 0x0'2c'0b;
				case 0x0'2c'3c: return 0x0'2c'0c;
				case 0x0'2c'3d: return 0x0'2c'0d;
				case 0x0'2c'3e: return 0x0'2c'0e;
				case 0x0'2c'3f: return 0x0'2c'0f;
				case 0x0'2c'40: return 0x0'2c'10;
				case 0x0'2c'41: return 0x0'2c'11;
				case 0x0'2c'42: return 0x0'2c'12;
				case 0x0'2c'43: return 0x0'2c'13;
				case 0x0'2c'44: return 0x0'2c'14;
				case 0x0'2c'45: return 0x0'2c'15;
				case 0x0'2c'46: return 0x0'2c'16;
				case 0x0'2c'47: return 0x0'2c'17;
				case 0x0'2c'48: return 0x0'2c'18;
				case 0x0'2c'49: return 0x0'2c'19;
				case 0x0'2c'4a: return 0x0'2c'1a;
				case 0x0'2c'4b: return 0x0'2c'1b;
				case 0x0'2c'4c: return 0x0'2c'1c;
				case 0x0'2c'4d: return 0x0'2c'1d;
				case 0x0'2c'4e: return 0x0'2c'1e;
				case 0x0'2c'4f: return 0x0'2c'1f;
				case 0x0'2c'50: return 0x0'2c'20;
				case 0x0'2c'51: return 0x0'2c'21;
				case 0x0'2c'52: return 0x0'2c'22;
				case 0x0'2c'53: return 0x0'2c'23;
				case 0x0'2c'54: return 0x0'2c'24;
				case 0x0'2c'55: return 0x0'2c'25;
				case 0x0'2c'56: return 0x0'2c'26;
				case 0x0'2c'57: return 0x0'2c'27;
				case 0x0'2c'58: return 0x0'2c'28;
				case 0x0'2c'59: return 0x0'2c'29;
				case 0x0'2c'5a: return 0x0'2c'2a;
				case 0x0'2c'5b: return 0x0'2c'2b;
				case 0x0'2c'5c: return 0x0'2c'2c;
				case 0x0'2c'5d: return 0x0'2c'2d;
				case 0x0'2c'5e: return 0x0'2c'2e;
				case 0x0'2c'61: return 0x0'2c'60;
				case 0x0'02'6b: return 0x0'2c'62;
				case 0x0'1d'7d: return 0x0'2c'63;
				case 0x0'02'7d: return 0x0'2c'64;
				case 0x0'2c'68: return 0x0'2c'67;
				case 0x0'2c'6a: return 0x0'2c'69;
				case 0x0'2c'6c: return 0x0'2c'6b;
				case 0x0'02'51: return 0x0'2c'6d;
				case 0x0'02'71: return 0x0'2c'6e;
				case 0x0'02'50: return 0x0'2c'6f;
				case 0x0'02'52: return 0x0'2c'70;
				case 0x0'2c'73: return 0x0'2c'72;
				case 0x0'2c'76: return 0x0'2c'75;
				case 0x0'02'3f: return 0x0'2c'7e;
				case 0x0'02'40: return 0x0'2c'7f;
				case 0x0'2c'81: return 0x0'2c'80;
				case 0x0'2c'83: return 0x0'2c'82;
				case 0x0'2c'85: return 0x0'2c'84;
				case 0x0'2c'87: return 0x0'2c'86;
				case 0x0'2c'89: return 0x0'2c'88;
				case 0x0'2c'8b: return 0x0'2c'8a;
				case 0x0'2c'8d: return 0x0'2c'8c;
				case 0x0'2c'8f: return 0x0'2c'8e;
				case 0x0'2c'91: return 0x0'2c'90;
				case 0x0'2c'93: return 0x0'2c'92;
				case 0x0'2c'95: return 0x0'2c'94;
				case 0x0'2c'97: return 0x0'2c'96;
				case 0x0'2c'99: return 0x0'2c'98;
				case 0x0'2c'9b: return 0x0'2c'9a;
				case 0x0'2c'9d: return 0x0'2c'9c;
				case 0x0'2c'9f: return 0x0'2c'9e;
				case 0x0'2c'a1: return 0x0'2c'a0;
				case 0x0'2c'a3: return 0x0'2c'a2;
				case 0x0'2c'a5: return 0x0'2c'a4;
				case 0x0'2c'a7: return 0x0'2c'a6;
				case 0x0'2c'a9: return 0x0'2c'a8;
				case 0x0'2c'ab: return 0x0'2c'aa;
				case 0x0'2c'ad: return 0x0'2c'ac;
				case 0x0'2c'af: return 0x0'2c'ae;
				case 0x0'2c'b1: return 0x0'2c'b0;
				case 0x0'2c'b3: return 0x0'2c'b2;
				case 0x0'2c'b5: return 0x0'2c'b4;
				case 0x0'2c'b7: return 0x0'2c'b6;
				case 0x0'2c'b9: return 0x0'2c'b8;
				case 0x0'2c'bb: return 0x0'2c'ba;
				case 0x0'2c'bd: return 0x0'2c'bc;
				case 0x0'2c'bf: return 0x0'2c'be;
				case 0x0'2c'c1: return 0x0'2c'c0;
				case 0x0'2c'c3: return 0x0'2c'c2;
				case 0x0'2c'c5: return 0x0'2c'c4;
				case 0x0'2c'c7: return 0x0'2c'c6;
				case 0x0'2c'c9: return 0x0'2c'c8;
				case 0x0'2c'cb: return 0x0'2c'ca;
				case 0x0'2c'cd: return 0x0'2c'cc;
				case 0x0'2c'cf: return 0x0'2c'ce;
				case 0x0'2c'd1: return 0x0'2c'd0;
				case 0x0'2c'd3: return 0x0'2c'd2;
				case 0x0'2c'd5: return 0x0'2c'd4;
				case 0x0'2c'd7: return 0x0'2c'd6;
				case 0x0'2c'd9: return 0x0'2c'd8;
				case 0x0'2c'db: return 0x0'2c'da;
				case 0x0'2c'dd: return 0x0'2c'dc;
				case 0x0'2c'df: return 0x0'2c'de;
				case 0x0'2c'e1: return 0x0'2c'e0;
				case 0x0'2c'e3: return 0x0'2c'e2;
				case 0x0'2c'ec: return 0x0'2c'eb;
				case 0x0'2c'ee: return 0x0'2c'ed;
				case 0x0'2c'f3: return 0x0'2c'f2;
				case 0x0'a6'41: return 0x0'a6'40;
				case 0x0'a6'43: return 0x0'a6'42;
				case 0x0'a6'45: return 0x0'a6'44;
				case 0x0'a6'47: return 0x0'a6'46;
				case 0x0'a6'49: return 0x0'a6'48;
				case 0x0'a6'4b: return 0x0'a6'4a;
				case 0x0'a6'4d: return 0x0'a6'4c;
				case 0x0'a6'4f: return 0x0'a6'4e;
				case 0x0'a6'51: return 0x0'a6'50;
				case 0x0'a6'53: return 0x0'a6'52;
				case 0x0'a6'55: return 0x0'a6'54;
				case 0x0'a6'57: return 0x0'a6'56;
				case 0x0'a6'59: return 0x0'a6'58;
				case 0x0'a6'5b: return 0x0'a6'5a;
				case 0x0'a6'5d: return 0x0'a6'5c;
				case 0x0'a6'5f: return 0x0'a6'5e;
				case 0x0'a6'61: return 0x0'a6'60;
				case 0x0'a6'63: return 0x0'a6'62;
				case 0x0'a6'65: return 0x0'a6'64;
				case 0x0'a6'67: return 0x0'a6'66;
				case 0x0'a6'69: return 0x0'a6'68;
				case 0x0'a6'6b: return 0x0'a6'6a;
				case 0x0'a6'6d: return 0x0'a6'6c;
				case 0x0'a6'81: return 0x0'a6'80;
				case 0x0'a6'83: return 0x0'a6'82;
				case 0x0'a6'85: return 0x0'a6'84;
				case 0x0'a6'87: return 0x0'a6'86;
				case 0x0'a6'89: return 0x0'a6'88;
				case 0x0'a6'8b: return 0x0'a6'8a;
				case 0x0'a6'8d: return 0x0'a6'8c;
				case 0x0'a6'8f: return 0x0'a6'8e;
				case 0x0'a6'91: return 0x0'a6'90;
				case 0x0'a6'93: return 0x0'a6'92;
				case 0x0'a6'95: return 0x0'a6'94;
				case 0x0'a6'97: return 0x0'a6'96;
				case 0x0'a6'99: return 0x0'a6'98;
				case 0x0'a6'9b: return 0x0'a6'9a;
				case 0x0'a7'23: return 0x0'a7'22;
				case 0x0'a7'25: return 0x0'a7'24;
				case 0x0'a7'27: return 0x0'a7'26;
				case 0x0'a7'29: return 0x0'a7'28;
				case 0x0'a7'2b: return 0x0'a7'2a;
				case 0x0'a7'2d: return 0x0'a7'2c;
				case 0x0'a7'2f: return 0x0'a7'2e;
				case 0x0'a7'33: return 0x0'a7'32;
				case 0x0'a7'35: return 0x0'a7'34;
				case 0x0'a7'37: return 0x0'a7'36;
				case 0x0'a7'39: return 0x0'a7'38;
				case 0x0'a7'3b: return 0x0'a7'3a;
				case 0x0'a7'3d: return 0x0'a7'3c;
				case 0x0'a7'3f: return 0x0'a7'3e;
				case 0x0'a7'41: return 0x0'a7'40;
				case 0x0'a7'43: return 0x0'a7'42;
				case 0x0'a7'45: return 0x0'a7'44;
				case 0x0'a7'47: return 0x0'a7'46;
				case 0x0'a7'49: return 0x0'a7'48;
				case 0x0'a7'4b: return 0x0'a7'4a;
				case 0x0'a7'4d: return 0x0'a7'4c;
				case 0x0'a7'4f: return 0x0'a7'4e;
				case 0x0'a7'51: return 0x0'a7'50;
				case 0x0'a7'53: return 0x0'a7'52;
				case 0x0'a7'55: return 0x0'a7'54;
				case 0x0'a7'57: return 0x0'a7'56;
				case 0x0'a7'59: return 0x0'a7'58;
				case 0x0'a7'5b: return 0x0'a7'5a;
				case 0x0'a7'5d: return 0x0'a7'5c;
				case 0x0'a7'5f: return 0x0'a7'5e;
				case 0x0'a7'61: return 0x0'a7'60;
				case 0x0'a7'63: return 0x0'a7'62;
				case 0x0'a7'65: return 0x0'a7'64;
				case 0x0'a7'67: return 0x0'a7'66;
				case 0x0'a7'69: return 0x0'a7'68;
				case 0x0'a7'6b: return 0x0'a7'6a;
				case 0x0'a7'6d: return 0x0'a7'6c;
				case 0x0'a7'6f: return 0x0'a7'6e;
				case 0x0'a7'7a: return 0x0'a7'79;
				case 0x0'a7'7c: return 0x0'a7'7b;
				case 0x0'1d'79: return 0x0'a7'7d;
				case 0x0'a7'7f: return 0x0'a7'7e;
				case 0x0'a7'81: return 0x0'a7'80;
				case 0x0'a7'83: return 0x0'a7'82;
				case 0x0'a7'85: return 0x0'a7'84;
				case 0x0'a7'87: return 0x0'a7'86;
				case 0x0'a7'8c: return 0x0'a7'8b;
				case 0x0'02'65: return 0x0'a7'8d;
				case 0x0'a7'91: return 0x0'a7'90;
				case 0x0'a7'93: return 0x0'a7'92;
				case 0x0'a7'97: return 0x0'a7'96;
				case 0x0'a7'99: return 0x0'a7'98;
				case 0x0'a7'9b: return 0x0'a7'9a;
				case 0x0'a7'9d: return 0x0'a7'9c;
				case 0x0'a7'9f: return 0x0'a7'9e;
				case 0x0'a7'a1: return 0x0'a7'a0;
				case 0x0'a7'a3: return 0x0'a7'a2;
				case 0x0'a7'a5: return 0x0'a7'a4;
				case 0x0'a7'a7: return 0x0'a7'a6;
				case 0x0'a7'a9: return 0x0'a7'a8;
				case 0x0'02'66: return 0x0'a7'aa;
				case 0x0'02'5c: return 0x0'a7'ab;
				case 0x0'02'61: return 0x0'a7'ac;
				case 0x0'02'6c: return 0x0'a7'ad;
				case 0x0'02'6a: return 0x0'a7'ae;
				case 0x0'02'9e: return 0x0'a7'b0;
				case 0x0'02'87: return 0x0'a7'b1;
				case 0x0'02'9d: return 0x0'a7'b2;
				case 0x0'ab'53: return 0x0'a7'b3;
				case 0x0'a7'b5: return 0x0'a7'b4;
				case 0x0'a7'b7: return 0x0'a7'b6;
				case 0x0'a7'b9: return 0x0'a7'b8;
				case 0x0'a7'bb: return 0x0'a7'ba;
				case 0x0'a7'bd: return 0x0'a7'bc;
				case 0x0'a7'bf: return 0x0'a7'be;
				case 0x0'a7'c3: return 0x0'a7'c2;
				case 0x0'a7'94: return 0x0'a7'c4;
				case 0x0'02'82: return 0x0'a7'c5;
				case 0x0'1d'8e: return 0x0'a7'c6;
				case 0x0'13'a0: return 0x0'ab'70;
				case 0x0'13'a1: return 0x0'ab'71;
				case 0x0'13'a2: return 0x0'ab'72;
				case 0x0'13'a3: return 0x0'ab'73;
				case 0x0'13'a4: return 0x0'ab'74;
				case 0x0'13'a5: return 0x0'ab'75;
				case 0x0'13'a6: return 0x0'ab'76;
				case 0x0'13'a7: return 0x0'ab'77;
				case 0x0'13'a8: return 0x0'ab'78;
				case 0x0'13'a9: return 0x0'ab'79;
				case 0x0'13'aa: return 0x0'ab'7a;
				case 0x0'13'ab: return 0x0'ab'7b;
				case 0x0'13'ac: return 0x0'ab'7c;
				case 0x0'13'ad: return 0x0'ab'7d;
				case 0x0'13'ae: return 0x0'ab'7e;
				case 0x0'13'af: return 0x0'ab'7f;
				case 0x0'13'b0: return 0x0'ab'80;
				case 0x0'13'b1: return 0x0'ab'81;
				case 0x0'13'b2: return 0x0'ab'82;
				case 0x0'13'b3: return 0x0'ab'83;
				case 0x0'13'b4: return 0x0'ab'84;
				case 0x0'13'b5: return 0x0'ab'85;
				case 0x0'13'b6: return 0x0'ab'86;
				case 0x0'13'b7: return 0x0'ab'87;
				case 0x0'13'b8: return 0x0'ab'88;
				case 0x0'13'b9: return 0x0'ab'89;
				case 0x0'13'ba: return 0x0'ab'8a;
				case 0x0'13'bb: return 0x0'ab'8b;
				case 0x0'13'bc: return 0x0'ab'8c;
				case 0x0'13'bd: return 0x0'ab'8d;
				case 0x0'13'be: return 0x0'ab'8e;
				case 0x0'13'bf: return 0x0'ab'8f;
				case 0x0'13'c0: return 0x0'ab'90;
				case 0x0'13'c1: return 0x0'ab'91;
				case 0x0'13'c2: return 0x0'ab'92;
				case 0x0'13'c3: return 0x0'ab'93;
				case 0x0'13'c4: return 0x0'ab'94;
				case 0x0'13'c5: return 0x0'ab'95;
				case 0x0'13'c6: return 0x0'ab'96;
				case 0x0'13'c7: return 0x0'ab'97;
				case 0x0'13'c8: return 0x0'ab'98;
				case 0x0'13'c9: return 0x0'ab'99;
				case 0x0'13'ca: return 0x0'ab'9a;
				case 0x0'13'cb: return 0x0'ab'9b;
				case 0x0'13'cc: return 0x0'ab'9c;
				case 0x0'13'cd: return 0x0'ab'9d;
				case 0x0'13'ce: return 0x0'ab'9e;
				case 0x0'13'cf: return 0x0'ab'9f;
				case 0x0'13'd0: return 0x0'ab'a0;
				case 0x0'13'd1: return 0x0'ab'a1;
				case 0x0'13'd2: return 0x0'ab'a2;
				case 0x0'13'd3: return 0x0'ab'a3;
				case 0x0'13'd4: return 0x0'ab'a4;
				case 0x0'13'd5: return 0x0'ab'a5;
				case 0x0'13'd6: return 0x0'ab'a6;
				case 0x0'13'd7: return 0x0'ab'a7;
				case 0x0'13'd8: return 0x0'ab'a8;
				case 0x0'13'd9: return 0x0'ab'a9;
				case 0x0'13'da: return 0x0'ab'aa;
				case 0x0'13'db: return 0x0'ab'ab;
				case 0x0'13'dc: return 0x0'ab'ac;
				case 0x0'13'dd: return 0x0'ab'ad;
				case 0x0'13'de: return 0x0'ab'ae;
				case 0x0'13'df: return 0x0'ab'af;
				case 0x0'13'e0: return 0x0'ab'b0;
				case 0x0'13'e1: return 0x0'ab'b1;
				case 0x0'13'e2: return 0x0'ab'b2;
				case 0x0'13'e3: return 0x0'ab'b3;
				case 0x0'13'e4: return 0x0'ab'b4;
				case 0x0'13'e5: return 0x0'ab'b5;
				case 0x0'13'e6: return 0x0'ab'b6;
				case 0x0'13'e7: return 0x0'ab'b7;
				case 0x0'13'e8: return 0x0'ab'b8;
				case 0x0'13'e9: return 0x0'ab'b9;
				case 0x0'13'ea: return 0x0'ab'ba;
				case 0x0'13'eb: return 0x0'ab'bb;
				case 0x0'13'ec: return 0x0'ab'bc;
				case 0x0'13'ed: return 0x0'ab'bd;
				case 0x0'13'ee: return 0x0'ab'be;
				case 0x0'13'ef: return 0x0'ab'bf;
				case 0x0'ff'41: return 0x0'ff'21;
				case 0x0'ff'42: return 0x0'ff'22;
				case 0x0'ff'43: return 0x0'ff'23;
				case 0x0'ff'44: return 0x0'ff'24;
				case 0x0'ff'45: return 0x0'ff'25;
				case 0x0'ff'46: return 0x0'ff'26;
				case 0x0'ff'47: return 0x0'ff'27;
				case 0x0'ff'48: return 0x0'ff'28;
				case 0x0'ff'49: return 0x0'ff'29;
				case 0x0'ff'4a: return 0x0'ff'2a;
				case 0x0'ff'4b: return 0x0'ff'2b;
				case 0x0'ff'4c: return 0x0'ff'2c;
				case 0x0'ff'4d: return 0x0'ff'2d;
				case 0x0'ff'4e: return 0x0'ff'2e;
				case 0x0'ff'4f: return 0x0'ff'2f;
				case 0x0'ff'50: return 0x0'ff'30;
				case 0x0'ff'51: return 0x0'ff'31;
				case 0x0'ff'52: return 0x0'ff'32;
				case 0x0'ff'53: return 0x0'ff'33;
				case 0x0'ff'54: return 0x0'ff'34;
				case 0x0'ff'55: return 0x0'ff'35;
				case 0x0'ff'56: return 0x0'ff'36;
				case 0x0'ff'57: return 0x0'ff'37;
				case 0x0'ff'58: return 0x0'ff'38;
				case 0x0'ff'59: return 0x0'ff'39;
				case 0x0'ff'5a: return 0x0'ff'3a;
				case 0x1'04'28: return 0x1'04'00;
				case 0x1'04'29: return 0x1'04'01;
				case 0x1'04'2a: return 0x1'04'02;
				case 0x1'04'2b: return 0x1'04'03;
				case 0x1'04'2c: return 0x1'04'04;
				case 0x1'04'2d: return 0x1'04'05;
				case 0x1'04'2e: return 0x1'04'06;
				case 0x1'04'2f: return 0x1'04'07;
				case 0x1'04'30: return 0x1'04'08;
				case 0x1'04'31: return 0x1'04'09;
				case 0x1'04'32: return 0x1'04'0a;
				case 0x1'04'33: return 0x1'04'0b;
				case 0x1'04'34: return 0x1'04'0c;
				case 0x1'04'35: return 0x1'04'0d;
				case 0x1'04'36: return 0x1'04'0e;
				case 0x1'04'37: return 0x1'04'0f;
				case 0x1'04'38: return 0x1'04'10;
				case 0x1'04'39: return 0x1'04'11;
				case 0x1'04'3a: return 0x1'04'12;
				case 0x1'04'3b: return 0x1'04'13;
				case 0x1'04'3c: return 0x1'04'14;
				case 0x1'04'3d: return 0x1'04'15;
				case 0x1'04'3e: return 0x1'04'16;
				case 0x1'04'3f: return 0x1'04'17;
				case 0x1'04'40: return 0x1'04'18;
				case 0x1'04'41: return 0x1'04'19;
				case 0x1'04'42: return 0x1'04'1a;
				case 0x1'04'43: return 0x1'04'1b;
				case 0x1'04'44: return 0x1'04'1c;
				case 0x1'04'45: return 0x1'04'1d;
				case 0x1'04'46: return 0x1'04'1e;
				case 0x1'04'47: return 0x1'04'1f;
				case 0x1'04'48: return 0x1'04'20;
				case 0x1'04'49: return 0x1'04'21;
				case 0x1'04'4a: return 0x1'04'22;
				case 0x1'04'4b: return 0x1'04'23;
				case 0x1'04'4c: return 0x1'04'24;
				case 0x1'04'4d: return 0x1'04'25;
				case 0x1'04'4e: return 0x1'04'26;
				case 0x1'04'4f: return 0x1'04'27;
				case 0x1'04'd8: return 0x1'04'b0;
				case 0x1'04'd9: return 0x1'04'b1;
				case 0x1'04'da: return 0x1'04'b2;
				case 0x1'04'db: return 0x1'04'b3;
				case 0x1'04'dc: return 0x1'04'b4;
				case 0x1'04'dd: return 0x1'04'b5;
				case 0x1'04'de: return 0x1'04'b6;
				case 0x1'04'df: return 0x1'04'b7;
				case 0x1'04'e0: return 0x1'04'b8;
				case 0x1'04'e1: return 0x1'04'b9;
				case 0x1'04'e2: return 0x1'04'ba;
				case 0x1'04'e3: return 0x1'04'bb;
				case 0x1'04'e4: return 0x1'04'bc;
				case 0x1'04'e5: return 0x1'04'bd;
				case 0x1'04'e6: return 0x1'04'be;
				case 0x1'04'e7: return 0x1'04'bf;
				case 0x1'04'e8: return 0x1'04'c0;
				case 0x1'04'e9: return 0x1'04'c1;
				case 0x1'04'ea: return 0x1'04'c2;
				case 0x1'04'eb: return 0x1'04'c3;
				case 0x1'04'ec: return 0x1'04'c4;
				case 0x1'04'ed: return 0x1'04'c5;
				case 0x1'04'ee: return 0x1'04'c6;
				case 0x1'04'ef: return 0x1'04'c7;
				case 0x1'04'f0: return 0x1'04'c8;
				case 0x1'04'f1: return 0x1'04'c9;
				case 0x1'04'f2: return 0x1'04'ca;
				case 0x1'04'f3: return 0x1'04'cb;
				case 0x1'04'f4: return 0x1'04'cc;
				case 0x1'04'f5: return 0x1'04'cd;
				case 0x1'04'f6: return 0x1'04'ce;
				case 0x1'04'f7: return 0x1'04'cf;
				case 0x1'04'f8: return 0x1'04'd0;
				case 0x1'04'f9: return 0x1'04'd1;
				case 0x1'04'fa: return 0x1'04'd2;
				case 0x1'04'fb: return 0x1'04'd3;
				case 0x1'0c'c0: return 0x1'0c'80;
				case 0x1'0c'c1: return 0x1'0c'81;
				case 0x1'0c'c2: return 0x1'0c'82;
				case 0x1'0c'c3: return 0x1'0c'83;
				case 0x1'0c'c4: return 0x1'0c'84;
				case 0x1'0c'c5: return 0x1'0c'85;
				case 0x1'0c'c6: return 0x1'0c'86;
				case 0x1'0c'c7: return 0x1'0c'87;
				case 0x1'0c'c8: return 0x1'0c'88;
				case 0x1'0c'c9: return 0x1'0c'89;
				case 0x1'0c'ca: return 0x1'0c'8a;
				case 0x1'0c'cb: return 0x1'0c'8b;
				case 0x1'0c'cc: return 0x1'0c'8c;
				case 0x1'0c'cd: return 0x1'0c'8d;
				case 0x1'0c'ce: return 0x1'0c'8e;
				case 0x1'0c'cf: return 0x1'0c'8f;
				case 0x1'0c'd0: return 0x1'0c'90;
				case 0x1'0c'd1: return 0x1'0c'91;
				case 0x1'0c'd2: return 0x1'0c'92;
				case 0x1'0c'd3: return 0x1'0c'93;
				case 0x1'0c'd4: return 0x1'0c'94;
				case 0x1'0c'd5: return 0x1'0c'95;
				case 0x1'0c'd6: return 0x1'0c'96;
				case 0x1'0c'd7: return 0x1'0c'97;
				case 0x1'0c'd8: return 0x1'0c'98;
				case 0x1'0c'd9: return 0x1'0c'99;
				case 0x1'0c'da: return 0x1'0c'9a;
				case 0x1'0c'db: return 0x1'0c'9b;
				case 0x1'0c'dc: return 0x1'0c'9c;
				case 0x1'0c'dd: return 0x1'0c'9d;
				case 0x1'0c'de: return 0x1'0c'9e;
				case 0x1'0c'df: return 0x1'0c'9f;
				case 0x1'0c'e0: return 0x1'0c'a0;
				case 0x1'0c'e1: return 0x1'0c'a1;
				case 0x1'0c'e2: return 0x1'0c'a2;
				case 0x1'0c'e3: return 0x1'0c'a3;
				case 0x1'0c'e4: return 0x1'0c'a4;
				case 0x1'0c'e5: return 0x1'0c'a5;
				case 0x1'0c'e6: return 0x1'0c'a6;
				case 0x1'0c'e7: return 0x1'0c'a7;
				case 0x1'0c'e8: return 0x1'0c'a8;
				case 0x1'0c'e9: return 0x1'0c'a9;
				case 0x1'0c'ea: return 0x1'0c'aa;
				case 0x1'0c'eb: return 0x1'0c'ab;
				case 0x1'0c'ec: return 0x1'0c'ac;
				case 0x1'0c'ed: return 0x1'0c'ad;
				case 0x1'0c'ee: return 0x1'0c'ae;
				case 0x1'0c'ef: return 0x1'0c'af;
				case 0x1'0c'f0: return 0x1'0c'b0;
				case 0x1'0c'f1: return 0x1'0c'b1;
				case 0x1'0c'f2: return 0x1'0c'b2;
				case 0x1'18'c0: return 0x1'18'a0;
				case 0x1'18'c1: return 0x1'18'a1;
				case 0x1'18'c2: return 0x1'18'a2;
				case 0x1'18'c3: return 0x1'18'a3;
				case 0x1'18'c4: return 0x1'18'a4;
				case 0x1'18'c5: return 0x1'18'a5;
				case 0x1'18'c6: return 0x1'18'a6;
				case 0x1'18'c7: return 0x1'18'a7;
				case 0x1'18'c8: return 0x1'18'a8;
				case 0x1'18'c9: return 0x1'18'a9;
				case 0x1'18'ca: return 0x1'18'aa;
				case 0x1'18'cb: return 0x1'18'ab;
				case 0x1'18'cc: return 0x1'18'ac;
				case 0x1'18'cd: return 0x1'18'ad;
				case 0x1'18'ce: return 0x1'18'ae;
				case 0x1'18'cf: return 0x1'18'af;
				case 0x1'18'd0: return 0x1'18'b0;
				case 0x1'18'd1: return 0x1'18'b1;
				case 0x1'18'd2: return 0x1'18'b2;
				case 0x1'18'd3: return 0x1'18'b3;
				case 0x1'18'd4: return 0x1'18'b4;
				case 0x1'18'd5: return 0x1'18'b5;
				case 0x1'18'd6: return 0x1'18'b6;
				case 0x1'18'd7: return 0x1'18'b7;
				case 0x1'18'd8: return 0x1'18'b8;
				case 0x1'18'd9: return 0x1'18'b9;
				case 0x1'18'da: return 0x1'18'ba;
				case 0x1'18'db: return 0x1'18'bb;
				case 0x1'18'dc: return 0x1'18'bc;
				case 0x1'18'dd: return 0x1'18'bd;
				case 0x1'18'de: return 0x1'18'be;
				case 0x1'18'df: return 0x1'18'bf;
				case 0x1'6e'60: return 0x1'6e'40;
				case 0x1'6e'61: return 0x1'6e'41;
				case 0x1'6e'62: return 0x1'6e'42;
				case 0x1'6e'63: return 0x1'6e'43;
				case 0x1'6e'64: return 0x1'6e'44;
				case 0x1'6e'65: return 0x1'6e'45;
				case 0x1'6e'66: return 0x1'6e'46;
				case 0x1'6e'67: return 0x1'6e'47;
				case 0x1'6e'68: return 0x1'6e'48;
				case 0x1'6e'69: return 0x1'6e'49;
				case 0x1'6e'6a: return 0x1'6e'4a;
				case 0x1'6e'6b: return 0x1'6e'4b;
				case 0x1'6e'6c: return 0x1'6e'4c;
				case 0x1'6e'6d: return 0x1'6e'4d;
				case 0x1'6e'6e: return 0x1'6e'4e;
				case 0x1'6e'6f: return 0x1'6e'4f;
				case 0x1'6e'70: return 0x1'6e'50;
				case 0x1'6e'71: return 0x1'6e'51;
				case 0x1'6e'72: return 0x1'6e'52;
				case 0x1'6e'73: return 0x1'6e'53;
				case 0x1'6e'74: return 0x1'6e'54;
				case 0x1'6e'75: return 0x1'6e'55;
				case 0x1'6e'76: return 0x1'6e'56;
				case 0x1'6e'77: return 0x1'6e'57;
				case 0x1'6e'78: return 0x1'6e'58;
				case 0x1'6e'79: return 0x1'6e'59;
				case 0x1'6e'7a: return 0x1'6e'5a;
				case 0x1'6e'7b: return 0x1'6e'5b;
				case 0x1'6e'7c: return 0x1'6e'5c;
				case 0x1'6e'7d: return 0x1'6e'5d;
				case 0x1'6e'7e: return 0x1'6e'5e;
				case 0x1'6e'7f: return 0x1'6e'5f;
				case 0x1'e9'22: return 0x1'e9'00;
				case 0x1'e9'23: return 0x1'e9'01;
				case 0x1'e9'24: return 0x1'e9'02;
				case 0x1'e9'25: return 0x1'e9'03;
				case 0x1'e9'26: return 0x1'e9'04;
				case 0x1'e9'27: return 0x1'e9'05;
				case 0x1'e9'28: return 0x1'e9'06;
				case 0x1'e9'29: return 0x1'e9'07;
				case 0x1'e9'2a: return 0x1'e9'08;
				case 0x1'e9'2b: return 0x1'e9'09;
				case 0x1'e9'2c: return 0x1'e9'0a;
				case 0x1'e9'2d: return 0x1'e9'0b;
				case 0x1'e9'2e: return 0x1'e9'0c;
				case 0x1'e9'2f: return 0x1'e9'0d;
				case 0x1'e9'30: return 0x1'e9'0e;
				case 0x1'e9'31: return 0x1'e9'0f;
				case 0x1'e9'32: return 0x1'e9'10;
				case 0x1'e9'33: return 0x1'e9'11;
				case 0x1'e9'34: return 0x1'e9'12;
				case 0x1'e9'35: return 0x1'e9'13;
				case 0x1'e9'36: return 0x1'e9'14;
				case 0x1'e9'37: return 0x1'e9'15;
				case 0x1'e9'38: return 0x1'e9'16;
				case 0x1'e9'39: return 0x1'e9'17;
				case 0x1'e9'3a: return 0x1'e9'18;
				case 0x1'e9'3b: return 0x1'e9'19;
				case 0x1'e9'3c: return 0x1'e9'1a;
				case 0x1'e9'3d: return 0x1'e9'1b;
				case 0x1'e9'3e: return 0x1'e9'1c;
				case 0x1'e9'3f: return 0x1'e9'1d;
				case 0x1'e9'40: return 0x1'e9'1e;
				case 0x1'e9'41: return 0x1'e9'1f;
				case 0x1'e9'42: return 0x1'e9'20;
				case 0x1'e9'43: return 0x1'e9'21;
				default: return __c;
			};
		}
		pf_decl_inline pf_decl_constexpr size_t
		required_len_to_upper(
		 const u8char_t *__in,
		 size_t __len)
		{
			size_t s = 0;
			for(size_t i = 0; i < __len;)
			{
				s += code_point_len(to_upper(to_code_point(&__in[i])));
				i += code_len(&__in[i]);
			}
			return s;
		}
		pf_decl_inline pf_decl_constexpr void
		to_upper(
		 const u8char_t *__in,
		 size_t __len,
		 u8char_t *__dst)
		{
			for(size_t i = 0, o = 0; i < __len;)
			{
				u8code_t c = to_code_point(&__in[i]);
				u8code_t l = to_upper(c);
				to_code(l, &__dst[o]);
				i += code_point_len(c);
				o += code_point_len(l);
			}
		}

		template<typename _Ty>
		pf_decl_inline char_error_t
		to_scalar(
		 const u8char_t *__str,
		 size_t __len,
		 _Ty &__val,
		 int32_t __base = 10) pf_attr_noexcept
			requires(std::is_arithmetic_v<_Ty>)
		{
			return ascii::to_scalar<_Ty>(union_cast<const char_t *>(__str), __len, __val, __base);
		}
		template<typename _Ty>
		pf_decl_inline char_error_t
		to_chars(
		 u8char_t *__str,
		 size_t __len,
		 _Ty __v,
		 uint32_t __base = 10) pf_attr_noexcept
			requires(std::is_arithmetic_v<_Ty>)
		{
			return ascii::to_chars<_Ty>(union_cast<char_t *>(__str), __len, __v, __base);
		}
	}	 // namespace utf8

	/// UTF16-LE
	namespace utf16le
	{
		// >=
		pf_hint_nodiscard pf_decl_always_inline m256i64_t
		__greater_equal_avx2(
		 const m256i64_t __in1,
		 const m256i64_t __in2) pf_attr_noexcept
		{
			return _mm256_cmpeq_epi16(_mm256_min_epu16(__in2, __in1), __in2);
		}
		pf_hint_nodiscard pf_decl_always_inline m128i64_t
		__greater_equal_sse2(
		 const m128i64_t __in1,
		 const m128i64_t __in2) pf_attr_noexcept
		{
			return _mm_cmpeq_epi16(_mm_min_epu16(__in2, __in1), __in2);
		}

		// <=
		pf_hint_nodiscard pf_decl_always_inline m256i64_t
		__smaller_equal_avx2(
		 const m256i64_t __in1,
		 const m256i64_t __in2) pf_attr_noexcept
		{
			return _mm256_cmpeq_epi16(_mm256_max_epu16(__in2, __in1), __in2);
		}
		pf_hint_nodiscard pf_decl_always_inline m128i64_t
		__smaller_equal_sse2(
		 const m128i64_t __in1,
		 const m128i64_t __in2) pf_attr_noexcept
		{
			return _mm_cmpeq_epi16(_mm_max_epu16(__in2, __in1), __in2);
		}

		// not_in_range
		pf_hint_nodiscard pf_decl_always_inline m256i64_t
		__not_in_range_avx2(
		 const m256i64_t __in,
		 const uint16_t __low,
		 const uint16_t __high) pf_attr_noexcept
		{
			const m256i64_t low_mask	= _mm256_set1_epi16(static_cast<uint16_t>(__low - 1));
			const m256i64_t high_mask = _mm256_set1_epi16(static_cast<uint16_t>(__high + 1));
			return _mm256_or_si256(__greater_equal_avx2(__in, high_mask), __smaller_equal_avx2(__in, low_mask));
		}
		pf_hint_nodiscard pf_decl_always_inline m128i64_t
		__not_in_range_sse2(
		 const m128i64_t __in,
		 const uint16_t __low,
		 const uint16_t __high) pf_attr_noexcept
		{
			const m128i64_t low_mask	= _mm_set1_epi16(static_cast<uint16_t>(__low - 1));
			const m128i64_t high_mask = _mm_set1_epi16(static_cast<uint16_t>(__high + 1));
			return _mm_or_si128(__greater_equal_sse2(__in, high_mask), __smaller_equal_sse2(__in, low_mask));
		}

		// Lenof
		pf_hint_nodiscard pf_decl_always_inline size_t
		lenof(
		 const u16char_t *__in) pf_attr_noexcept
		{
			size_t c = 0;
			while(__in[c] != u'\x0000') ++c;
			return (c + 1);
		}

		// Countof
		pf_hint_nodiscard pf_decl_always_inline pf_decl_constexpr size_t
		countof(
		 const u16char_t *__in,
		 size_t __len) pf_attr_noexcept
		{
			size_t p = 0;
			size_t c = 0;
			if(!std::is_constant_evaluated())
			{
				// 256
				for(; p + 16 <= __len; p += 16)
				{
					const m256i64_t seq		 = _mm256_lddqu_si256(union_cast<const m256i64_t *>(&__in[p]));
					const int32_t not_pair = _mm256_movemask_epi8(__not_in_range_avx2(seq, 0xDC'00, 0xDF'FF));
					c											+= _popcnt32(not_pair) / 2;
				}
				// 128
				for(; p + 8 <= __len; p += 8)
				{
					const m128i64_t seq		 = _mm_lddqu_si128(union_cast<const m128i64_t *>(&__in[p]));
					const int32_t not_pair = _mm_movemask_epi8(__not_in_range_sse2(seq, 0xDC'00, 0xDF'FF));
					c											+= _popcnt32(not_pair) / 2;
				}
			}

			// 64
			for(; p < __len; ++p)
			{
				uint16_t w = uint16_t(__in[p]);
				c					+= ((w & 0xFC'00) != 0xDC'00);
			}
			return c;
		}
	}	 // namespace utf16le

	/// UTF8 -> UTF16-LE
	namespace utf8_to_utf16le
	{
		// Required
		pf_hint_nodiscard pf_decl_always_inline pf_decl_constexpr size_t
		required_count(
		 const u8char_t *__in,
		 size_t __len) pf_attr_noexcept
		{
			size_t p = 0;
			size_t c = 0;
			if(!std::is_constant_evaluated())
			{
				// 256
				for(; p + 32 <= __len; p += 32)
				{
					const m256i64_t seq							= _mm256_lddqu_si256(union_cast<const m256i64_t *>(&__in[p]));
					const int32_t continuation_mask = _mm256_movemask_epi8(_mm256_cmpgt_epi8(_mm256_set1_epi8(-64), seq));
					c															 += 32 - _popcnt32(continuation_mask);
					const int32_t byte4_mask				= _mm256_movemask_epi8(utf8::__greater_equal_avx2(seq, _mm256_set1_epi8(-16)));
					c															 += _popcnt32(byte4_mask);
				}

				// 128
				for(; p + 16 <= __len; p += 16)
				{
					const m128i64_t seq							= _mm_lddqu_si128(union_cast<const m128i64_t *>(&__in[p]));
					const int32_t continuation_mask = _mm_movemask_epi8(_mm_cmplt_epi8(seq, _mm_set1_epi8(-64)));
					c															 += 16 - _popcnt32(continuation_mask);
					const int32_t byte4_mask				= _mm_movemask_epi8(utf8::__greater_equal_sse2(seq, _mm_set1_epi8(-16)));
					c															 += _popcnt32(byte4_mask);
				}
			}

			// 64
			for(; p < __len; ++p)
			{
				if(__in[p] > -65) ++c;
				if(uint8_t(__in[p]) >= 240) ++c;
			}

			return c;
		}

		// Convert
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr char_error_t
		__convert_with_errors_utf8_to_utf16le(
		 const u8char_t *__in,
		 size_t __len,
		 u16char_t *__out) pf_attr_noexcept
		{
			size_t p				 = 0;
			u16char_t *start = __out;
			while(p < __len)
			{
				// try to convert the next block of 16 ASCII bytes
				if(p + 16 <= __len)
				{	 // if it is safe to read 16 more bytes, check that they are ascii
					uint64_t v1;
					memcpy(&v1, &__in[p], sizeof(uint64_t));
					uint64_t v2;
					memcpy(&v2, &__in[p] + sizeof(uint64_t), sizeof(uint64_t));
					uint64_t v { v1 | v2 };
					if((v & 0x80'80'80'80'80'80'80'80) == 0)
					{
						size_t fp = p + 16;
						while(p < fp)
						{
							*__out++ = char16_t(__in[p]);
							++p;
						}
						continue;
					}
				}
				uint8_t lb = uint8_t(__in[p]);	// leading byte
				if(lb < 0b10000000)
				{
					// converting one ASCII byte !!!
					*__out++ = char16_t(lb);
					++p;
				}
				else if((lb & 0b11100000) == 0b11000000)
				{
					// We have a two-byte UTF-8, it should become
					// a single UTF-16 w.
					if(p + 1 >= __len) return char_error_t { char_error_code::too_short, p };	 // minimal bound checking
					if((uint8_t(__in[p + 1]) & 0b11000000) != 0b10000000) return char_error_t { char_error_code::too_short, p };
					// range check
					uint32_t cp = (lb & 0b00011111) << 6 | (uint8_t(__in[p + 1]) & 0b00111111);
					if(cp < 0x80 || 0x7'ff < cp) return char_error_t { char_error_code::overlong, p };
					*__out++ = u16char_t(cp);
					p				+= 2;
				}
				else if((lb & 0b11110000) == 0b11100000)
				{
					// We have a three-byte UTF-8, it should become
					// a single UTF-16 w.
					if(p + 2 >= __len) return char_error_t { char_error_code::too_short, p };	 // minimal bound checking

					if((uint8_t(__in[p + 1]) & 0b11000000) != 0b10000000) return char_error_t { char_error_code::too_short, p };
					if((uint8_t(__in[p + 2]) & 0b11000000) != 0b10000000) return char_error_t { char_error_code::too_short, p };
					// range check
					uint32_t cp = (lb & 0b00001111) << 12 | (uint8_t(__in[p + 1]) & 0b00111111) << 6 | (uint8_t(__in[p + 2]) & 0b00111111);
					if((cp < 0x8'00) || (0xff'ff < cp)) return char_error_t { char_error_code::overlong, p };
					if(0xd7'ff < cp && cp < 0xe0'00) return char_error_t { char_error_code::surrogate, p };
					*__out++ = u16char_t(cp);
					p				+= 3;
				}
				else if((lb & 0b11111000) == 0b11110000)
				{	 // 0b11110000
					// we have a 4-byte UTF-8 w.
					if(p + 3 >= __len) return char_error_t { char_error_code::too_short, p };
					// minimal bound checking
					if((uint8_t(__in[p + 1]) & 0b11000000) != 0b10000000) return char_error_t { char_error_code::too_short, p };
					if((uint8_t(__in[p + 2]) & 0b11000000) != 0b10000000) return char_error_t { char_error_code::too_short, p };
					if((uint8_t(__in[p + 3]) & 0b11000000) != 0b10000000) return char_error_t { char_error_code::too_short, p };

					// range check
					uint32_t cp =
					 (lb & 0b00000111) << 18 | (uint8_t(__in[p + 1]) & 0b00111111) << 12 | (uint8_t(__in[p + 2]) & 0b00111111) << 6 | (uint8_t(__in[p + 3]) & 0b00111111);
					if(cp <= 0xff'ff) return char_error_t { char_error_code::overlong, p };
					if(0x10'ff'ff < cp) return char_error_t { char_error_code::too_large, p };
					cp										 -= 0x1'00'00;
					uint16_t high_surrogate = uint16_t(0xD8'00 + (cp >> 10));
					uint16_t low_surrogate	= uint16_t(0xDC'00 + (cp & 0x3'FF));
					*__out++								= u16char_t(high_surrogate);
					*__out++								= u16char_t(low_surrogate);
					p											 += 4;
				}
				else
				{
					// we either have too many continuation bytes or an invalid leading byte
					if((lb & 0b11000000) == 0b10000000)
						return char_error_t { char_error_code::too_long, p };
					else
						return char_error_t { char_error_code::header_bits, p };
				}
			}
			return char_error_t { char_error_code::success, countof(start, __out) };
		}
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr char_error_t
		__rewind_and_convert_with_errors_utf8_to_utf16le(
		 size_t __prior,
		 const u8char_t *__in,
		 size_t __len,
		 u16char_t *__out) pf_attr_noexcept
		{
			size_t exlen				= 0;
			// We potentially need to go back in time and find a leading byte.
			// In theory '3' would be sufficient, but sometimes the error can go back quite far.
			size_t how_far_back = __prior;
			// size_t how_far_back = 3; // 3 bytes in the past + current position
			// if(how_far_back >= __prior) { how_far_back = __prior; }
			bool found_lbs			= false;
			// important: it is i <= how_far_back and not 'i < how_far_back'.
			for(size_t i = 0; i <= how_far_back; ++i)
			{
				uint8_t byte = uint8_t(__in[-i]);
				found_lbs		 = ((byte & 0b11000000) != 0b10000000);
				if(found_lbs)
				{
					__in -= i;
					exlen = i;
					break;
				}
			}
			//
			// It is possible for this function to return a negative count in its result.
			// C++ Standard Section 18.1 defines size_t is in <cstddef> which is described in C Standard as <stddef.h>.
			// C Standard Section 4.1.5 defines size_t as an unsigned integral type of the result of the sizeof operator
			//
			// An unsigned type will simply wrap round arithmetically (well defined).
			//
			if(!found_lbs)
			{
				// If how_far_back == 3, we may have four consecutive continuation bytes!!!
				// [....] [continuation] [continuation] [continuation] | [__in is continuation]
				// Or we possibly have a stream that does not start with a leading byte.
				return char_error_t { char_error_code::too_long, -how_far_back };
			}
			char_error_t res = __convert_with_errors_utf8_to_utf16le(__in, __len + exlen, __out);
			if(res.code != char_error_code::success)
			{
				res.position -= exlen;
			}
			return res;
		}

		// Tables
		const uint8_t __shut_utf8_to_utf16[209][16] = {
			{0,	 255, 1,	 255, 2, 255, 3,	 255, 4,	 255, 5,	 255, 0,	 0,		0,	 0	 },
			{ 0, 255, 1,	 255, 2, 255, 3,	 255, 4,	 255, 6,	 5,		0,	 0,		0,	 0	 },
			{ 0, 255, 1,	 255, 2, 255, 3,	 255, 5,	 4,		6,	 255, 0,	 0,		0,	 0	 },
			{ 0, 255, 1,	 255, 2, 255, 3,	 255, 5,	 4,		7,	 6,		0,	 0,		0,	 0	 },
			{ 0, 255, 1,	 255, 2, 255, 4,	 3,		5,	 255, 6,	 255, 0,	 0,		0,	 0	 },
			{ 0, 255, 1,	 255, 2, 255, 4,	 3,		5,	 255, 7,	 6,		0,	 0,		0,	 0	 },
			{ 0, 255, 1,	 255, 2, 255, 4,	 3,		6,	 5,		7,	 255, 0,	 0,		0,	 0	 },
			{ 0, 255, 1,	 255, 2, 255, 4,	 3,		6,	 5,		8,	 7,		0,	 0,		0,	 0	 },
			{ 0, 255, 1,	 255, 3, 2,		4,	 255, 5,	 255, 6,	 255, 0,	 0,		0,	 0	 },
			{ 0, 255, 1,	 255, 3, 2,		4,	 255, 5,	 255, 7,	 6,		0,	 0,		0,	 0	 },
			{ 0, 255, 1,	 255, 3, 2,		4,	 255, 6,	 5,		7,	 255, 0,	 0,		0,	 0	 },
			{ 0, 255, 1,	 255, 3, 2,		4,	 255, 6,	 5,		8,	 7,		0,	 0,		0,	 0	 },
			{ 0, 255, 1,	 255, 3, 2,		5,	 4,		6,	 255, 7,	 255, 0,	 0,		0,	 0	 },
			{ 0, 255, 1,	 255, 3, 2,		5,	 4,		6,	 255, 8,	 7,		0,	 0,		0,	 0	 },
			{ 0, 255, 1,	 255, 3, 2,		5,	 4,		7,	 6,		8,	 255, 0,	 0,		0,	 0	 },
			{ 0, 255, 1,	 255, 3, 2,		5,	 4,		7,	 6,		9,	 8,		0,	 0,		0,	 0	 },
			{ 0, 255, 2,	 1,		3, 255, 4,	 255, 5,	 255, 6,	 255, 0,	 0,		0,	 0	 },
			{ 0, 255, 2,	 1,		3, 255, 4,	 255, 5,	 255, 7,	 6,		0,	 0,		0,	 0	 },
			{ 0, 255, 2,	 1,		3, 255, 4,	 255, 6,	 5,		7,	 255, 0,	 0,		0,	 0	 },
			{ 0, 255, 2,	 1,		3, 255, 4,	 255, 6,	 5,		8,	 7,		0,	 0,		0,	 0	 },
			{ 0, 255, 2,	 1,		3, 255, 5,	 4,		6,	 255, 7,	 255, 0,	 0,		0,	 0	 },
			{ 0, 255, 2,	 1,		3, 255, 5,	 4,		6,	 255, 8,	 7,		0,	 0,		0,	 0	 },
			{ 0, 255, 2,	 1,		3, 255, 5,	 4,		7,	 6,		8,	 255, 0,	 0,		0,	 0	 },
			{ 0, 255, 2,	 1,		3, 255, 5,	 4,		7,	 6,		9,	 8,		0,	 0,		0,	 0	 },
			{ 0, 255, 2,	 1,		4, 3,		5,	 255, 6,	 255, 7,	 255, 0,	 0,		0,	 0	 },
			{ 0, 255, 2,	 1,		4, 3,		5,	 255, 6,	 255, 8,	 7,		0,	 0,		0,	 0	 },
			{ 0, 255, 2,	 1,		4, 3,		5,	 255, 7,	 6,		8,	 255, 0,	 0,		0,	 0	 },
			{ 0, 255, 2,	 1,		4, 3,		5,	 255, 7,	 6,		9,	 8,		0,	 0,		0,	 0	 },
			{ 0, 255, 2,	 1,		4, 3,		6,	 5,		7,	 255, 8,	 255, 0,	 0,		0,	 0	 },
			{ 0, 255, 2,	 1,		4, 3,		6,	 5,		7,	 255, 9,	 8,		0,	 0,		0,	 0	 },
			{ 0, 255, 2,	 1,		4, 3,		6,	 5,		8,	 7,		9,	 255, 0,	 0,		0,	 0	 },
			{ 0, 255, 2,	 1,		4, 3,		6,	 5,		8,	 7,		10,	9,	 0,	0,	 0,		0	},
			{ 1, 0,		2,	 255, 3, 255, 4,	 255, 5,	 255, 6,	 255, 0,	 0,		0,	 0	 },
			{ 1, 0,		2,	 255, 3, 255, 4,	 255, 5,	 255, 7,	 6,		0,	 0,		0,	 0	 },
			{ 1, 0,		2,	 255, 3, 255, 4,	 255, 6,	 5,		7,	 255, 0,	 0,		0,	 0	 },
			{ 1, 0,		2,	 255, 3, 255, 4,	 255, 6,	 5,		8,	 7,		0,	 0,		0,	 0	 },
			{ 1, 0,		2,	 255, 3, 255, 5,	 4,		6,	 255, 7,	 255, 0,	 0,		0,	 0	 },
			{ 1, 0,		2,	 255, 3, 255, 5,	 4,		6,	 255, 8,	 7,		0,	 0,		0,	 0	 },
			{ 1, 0,		2,	 255, 3, 255, 5,	 4,		7,	 6,		8,	 255, 0,	 0,		0,	 0	 },
			{ 1, 0,		2,	 255, 3, 255, 5,	 4,		7,	 6,		9,	 8,		0,	 0,		0,	 0	 },
			{ 1, 0,		2,	 255, 4, 3,		5,	 255, 6,	 255, 7,	 255, 0,	 0,		0,	 0	 },
			{ 1, 0,		2,	 255, 4, 3,		5,	 255, 6,	 255, 8,	 7,		0,	 0,		0,	 0	 },
			{ 1, 0,		2,	 255, 4, 3,		5,	 255, 7,	 6,		8,	 255, 0,	 0,		0,	 0	 },
			{ 1, 0,		2,	 255, 4, 3,		5,	 255, 7,	 6,		9,	 8,		0,	 0,		0,	 0	 },
			{ 1, 0,		2,	 255, 4, 3,		6,	 5,		7,	 255, 8,	 255, 0,	 0,		0,	 0	 },
			{ 1, 0,		2,	 255, 4, 3,		6,	 5,		7,	 255, 9,	 8,		0,	 0,		0,	 0	 },
			{ 1, 0,		2,	 255, 4, 3,		6,	 5,		8,	 7,		9,	 255, 0,	 0,		0,	 0	 },
			{ 1, 0,		2,	 255, 4, 3,		6,	 5,		8,	 7,		10,	9,	 0,	0,	 0,		0	},
			{ 1, 0,		3,	 2,		4, 255, 5,	 255, 6,	 255, 7,	 255, 0,	 0,		0,	 0	 },
			{ 1, 0,		3,	 2,		4, 255, 5,	 255, 6,	 255, 8,	 7,		0,	 0,		0,	 0	 },
			{ 1, 0,		3,	 2,		4, 255, 5,	 255, 7,	 6,		8,	 255, 0,	 0,		0,	 0	 },
			{ 1, 0,		3,	 2,		4, 255, 5,	 255, 7,	 6,		9,	 8,		0,	 0,		0,	 0	 },
			{ 1, 0,		3,	 2,		4, 255, 6,	 5,		7,	 255, 8,	 255, 0,	 0,		0,	 0	 },
			{ 1, 0,		3,	 2,		4, 255, 6,	 5,		7,	 255, 9,	 8,		0,	 0,		0,	 0	 },
			{ 1, 0,		3,	 2,		4, 255, 6,	 5,		8,	 7,		9,	 255, 0,	 0,		0,	 0	 },
			{ 1, 0,		3,	 2,		4, 255, 6,	 5,		8,	 7,		10,	9,	 0,	0,	 0,		0	},
			{ 1, 0,		3,	 2,		5, 4,		6,	 255, 7,	 255, 8,	 255, 0,	 0,		0,	 0	 },
			{ 1, 0,		3,	 2,		5, 4,		6,	 255, 7,	 255, 9,	 8,		0,	 0,		0,	 0	 },
			{ 1, 0,		3,	 2,		5, 4,		6,	 255, 8,	 7,		9,	 255, 0,	 0,		0,	 0	 },
			{ 1, 0,		3,	 2,		5, 4,		6,	 255, 8,	 7,		10,	9,	 0,	0,	 0,		0	},
			{ 1, 0,		3,	 2,		5, 4,		7,	 6,		8,	 255, 9,	 255, 0,	 0,		0,	 0	 },
			{ 1, 0,		3,	 2,		5, 4,		7,	 6,		8,	 255, 10,	9,	 0,	0,	 0,		0	},
			{ 1, 0,		3,	 2,		5, 4,		7,	 6,		9,	 8,		10,	255, 0,	0,	 0,		0	},
			{ 1, 0,		3,	 2,		5, 4,		7,	 6,		9,	 8,		11,	10,	0,	 0,		0,	 0	 },
			{ 0, 255, 255, 255, 1, 255, 255, 255, 2,	 255, 255, 255, 3,	 255, 255, 255},
			{ 0, 255, 255, 255, 1, 255, 255, 255, 2,	 255, 255, 255, 4,	 3,		255, 255},
			{ 0, 255, 255, 255, 1, 255, 255, 255, 2,	 255, 255, 255, 5,	 4,		3,	 255},
			{ 0, 255, 255, 255, 1, 255, 255, 255, 3,	 2,		255, 255, 4,	 255, 255, 255},
			{ 0, 255, 255, 255, 1, 255, 255, 255, 3,	 2,		255, 255, 5,	 4,		255, 255},
			{ 0, 255, 255, 255, 1, 255, 255, 255, 3,	 2,		255, 255, 6,	 5,		4,	 255},
			{ 0, 255, 255, 255, 1, 255, 255, 255, 4,	 3,		2,	 255, 5,	 255, 255, 255},
			{ 0, 255, 255, 255, 1, 255, 255, 255, 4,	 3,		2,	 255, 6,	 5,		255, 255},
			{ 0, 255, 255, 255, 1, 255, 255, 255, 4,	 3,		2,	 255, 7,	 6,		5,	 255},
			{ 0, 255, 255, 255, 2, 1,		255, 255, 3,	 255, 255, 255, 4,	 255, 255, 255},
			{ 0, 255, 255, 255, 2, 1,		255, 255, 3,	 255, 255, 255, 5,	 4,		255, 255},
			{ 0, 255, 255, 255, 2, 1,		255, 255, 3,	 255, 255, 255, 6,	 5,		4,	 255},
			{ 0, 255, 255, 255, 2, 1,		255, 255, 4,	 3,		255, 255, 5,	 255, 255, 255},
			{ 0, 255, 255, 255, 2, 1,		255, 255, 4,	 3,		255, 255, 6,	 5,		255, 255},
			{ 0, 255, 255, 255, 2, 1,		255, 255, 4,	 3,		255, 255, 7,	 6,		5,	 255},
			{ 0, 255, 255, 255, 2, 1,		255, 255, 5,	 4,		3,	 255, 6,	 255, 255, 255},
			{ 0, 255, 255, 255, 2, 1,		255, 255, 5,	 4,		3,	 255, 7,	 6,		255, 255},
			{ 0, 255, 255, 255, 2, 1,		255, 255, 5,	 4,		3,	 255, 8,	 7,		6,	 255},
			{ 0, 255, 255, 255, 3, 2,		1,	 255, 4,	 255, 255, 255, 5,	 255, 255, 255},
			{ 0, 255, 255, 255, 3, 2,		1,	 255, 4,	 255, 255, 255, 6,	 5,		255, 255},
			{ 0, 255, 255, 255, 3, 2,		1,	 255, 4,	 255, 255, 255, 7,	 6,		5,	 255},
			{ 0, 255, 255, 255, 3, 2,		1,	 255, 5,	 4,		255, 255, 6,	 255, 255, 255},
			{ 0, 255, 255, 255, 3, 2,		1,	 255, 5,	 4,		255, 255, 7,	 6,		255, 255},
			{ 0, 255, 255, 255, 3, 2,		1,	 255, 5,	 4,		255, 255, 8,	 7,		6,	 255},
			{ 0, 255, 255, 255, 3, 2,		1,	 255, 6,	 5,		4,	 255, 7,	 255, 255, 255},
			{ 0, 255, 255, 255, 3, 2,		1,	 255, 6,	 5,		4,	 255, 8,	 7,		255, 255},
			{ 0, 255, 255, 255, 3, 2,		1,	 255, 6,	 5,		4,	 255, 9,	 8,		7,	 255},
			{ 1, 0,		255, 255, 2, 255, 255, 255, 3,	 255, 255, 255, 4,	 255, 255, 255},
			{ 1, 0,		255, 255, 2, 255, 255, 255, 3,	 255, 255, 255, 5,	 4,		255, 255},
			{ 1, 0,		255, 255, 2, 255, 255, 255, 3,	 255, 255, 255, 6,	 5,		4,	 255},
			{ 1, 0,		255, 255, 2, 255, 255, 255, 4,	 3,		255, 255, 5,	 255, 255, 255},
			{ 1, 0,		255, 255, 2, 255, 255, 255, 4,	 3,		255, 255, 6,	 5,		255, 255},
			{ 1, 0,		255, 255, 2, 255, 255, 255, 4,	 3,		255, 255, 7,	 6,		5,	 255},
			{ 1, 0,		255, 255, 2, 255, 255, 255, 5,	 4,		3,	 255, 6,	 255, 255, 255},
			{ 1, 0,		255, 255, 2, 255, 255, 255, 5,	 4,		3,	 255, 7,	 6,		255, 255},
			{ 1, 0,		255, 255, 2, 255, 255, 255, 5,	 4,		3,	 255, 8,	 7,		6,	 255},
			{ 1, 0,		255, 255, 3, 2,		255, 255, 4,	 255, 255, 255, 5,	 255, 255, 255},
			{ 1, 0,		255, 255, 3, 2,		255, 255, 4,	 255, 255, 255, 6,	 5,		255, 255},
			{ 1, 0,		255, 255, 3, 2,		255, 255, 4,	 255, 255, 255, 7,	 6,		5,	 255},
			{ 1, 0,		255, 255, 3, 2,		255, 255, 5,	 4,		255, 255, 6,	 255, 255, 255},
			{ 1, 0,		255, 255, 3, 2,		255, 255, 5,	 4,		255, 255, 7,	 6,		255, 255},
			{ 1, 0,		255, 255, 3, 2,		255, 255, 5,	 4,		255, 255, 8,	 7,		6,	 255},
			{ 1, 0,		255, 255, 3, 2,		255, 255, 6,	 5,		4,	 255, 7,	 255, 255, 255},
			{ 1, 0,		255, 255, 3, 2,		255, 255, 6,	 5,		4,	 255, 8,	 7,		255, 255},
			{ 1, 0,		255, 255, 3, 2,		255, 255, 6,	 5,		4,	 255, 9,	 8,		7,	 255},
			{ 1, 0,		255, 255, 4, 3,		2,	 255, 5,	 255, 255, 255, 6,	 255, 255, 255},
			{ 1, 0,		255, 255, 4, 3,		2,	 255, 5,	 255, 255, 255, 7,	 6,		255, 255},
			{ 1, 0,		255, 255, 4, 3,		2,	 255, 5,	 255, 255, 255, 8,	 7,		6,	 255},
			{ 1, 0,		255, 255, 4, 3,		2,	 255, 6,	 5,		255, 255, 7,	 255, 255, 255},
			{ 1, 0,		255, 255, 4, 3,		2,	 255, 6,	 5,		255, 255, 8,	 7,		255, 255},
			{ 1, 0,		255, 255, 4, 3,		2,	 255, 6,	 5,		255, 255, 9,	 8,		7,	 255},
			{ 1, 0,		255, 255, 4, 3,		2,	 255, 7,	 6,		5,	 255, 8,	 255, 255, 255},
			{ 1, 0,		255, 255, 4, 3,		2,	 255, 7,	 6,		5,	 255, 9,	 8,		255, 255},
			{ 1, 0,		255, 255, 4, 3,		2,	 255, 7,	 6,		5,	 255, 10, 9,	 8,		255},
			{ 2, 1,		0,	 255, 3, 255, 255, 255, 4,	 255, 255, 255, 5,	 255, 255, 255},
			{ 2, 1,		0,	 255, 3, 255, 255, 255, 4,	 255, 255, 255, 6,	 5,		255, 255},
			{ 2, 1,		0,	 255, 3, 255, 255, 255, 4,	 255, 255, 255, 7,	 6,		5,	 255},
			{ 2, 1,		0,	 255, 3, 255, 255, 255, 5,	 4,		255, 255, 6,	 255, 255, 255},
			{ 2, 1,		0,	 255, 3, 255, 255, 255, 5,	 4,		255, 255, 7,	 6,		255, 255},
			{ 2, 1,		0,	 255, 3, 255, 255, 255, 5,	 4,		255, 255, 8,	 7,		6,	 255},
			{ 2, 1,		0,	 255, 3, 255, 255, 255, 6,	 5,		4,	 255, 7,	 255, 255, 255},
			{ 2, 1,		0,	 255, 3, 255, 255, 255, 6,	 5,		4,	 255, 8,	 7,		255, 255},
			{ 2, 1,		0,	 255, 3, 255, 255, 255, 6,	 5,		4,	 255, 9,	 8,		7,	 255},
			{ 2, 1,		0,	 255, 4, 3,		255, 255, 5,	 255, 255, 255, 6,	 255, 255, 255},
			{ 2, 1,		0,	 255, 4, 3,		255, 255, 5,	 255, 255, 255, 7,	 6,		255, 255},
			{ 2, 1,		0,	 255, 4, 3,		255, 255, 5,	 255, 255, 255, 8,	 7,		6,	 255},
			{ 2, 1,		0,	 255, 4, 3,		255, 255, 6,	 5,		255, 255, 7,	 255, 255, 255},
			{ 2, 1,		0,	 255, 4, 3,		255, 255, 6,	 5,		255, 255, 8,	 7,		255, 255},
			{ 2, 1,		0,	 255, 4, 3,		255, 255, 6,	 5,		255, 255, 9,	 8,		7,	 255},
			{ 2, 1,		0,	 255, 4, 3,		255, 255, 7,	 6,		5,	 255, 8,	 255, 255, 255},
			{ 2, 1,		0,	 255, 4, 3,		255, 255, 7,	 6,		5,	 255, 9,	 8,		255, 255},
			{ 2, 1,		0,	 255, 4, 3,		255, 255, 7,	 6,		5,	 255, 10, 9,	 8,		255},
			{ 2, 1,		0,	 255, 5, 4,		3,	 255, 6,	 255, 255, 255, 7,	 255, 255, 255},
			{ 2, 1,		0,	 255, 5, 4,		3,	 255, 6,	 255, 255, 255, 8,	 7,		255, 255},
			{ 2, 1,		0,	 255, 5, 4,		3,	 255, 6,	 255, 255, 255, 9,	 8,		7,	 255},
			{ 2, 1,		0,	 255, 5, 4,		3,	 255, 7,	 6,		255, 255, 8,	 255, 255, 255},
			{ 2, 1,		0,	 255, 5, 4,		3,	 255, 7,	 6,		255, 255, 9,	 8,		255, 255},
			{ 2, 1,		0,	 255, 5, 4,		3,	 255, 7,	 6,		255, 255, 10, 9,	 8,		255},
			{ 2, 1,		0,	 255, 5, 4,		3,	 255, 8,	 7,		6,	 255, 9,	 255, 255, 255},
			{ 2, 1,		0,	 255, 5, 4,		3,	 255, 8,	 7,		6,	 255, 10, 9,	 255, 255},
			{ 2, 1,		0,	 255, 5, 4,		3,	 255, 8,	 7,		6,	 255, 11, 10,	9,	 255},
			{ 0, 255, 255, 255, 1, 255, 255, 255, 2,	 255, 255, 255, 0,	 0,		0,	 0	 },
			{ 0, 255, 255, 255, 1, 255, 255, 255, 3,	 2,		255, 255, 0,	 0,		0,	 0	 },
			{ 0, 255, 255, 255, 1, 255, 255, 255, 4,	 3,		2,	 255, 0,	 0,		0,	 0	 },
			{ 0, 255, 255, 255, 1, 255, 255, 255, 5,	 4,		3,	 2,		0,	 0,		0,	 0	 },
			{ 0, 255, 255, 255, 2, 1,		255, 255, 3,	 255, 255, 255, 0,	 0,		0,	 0	 },
			{ 0, 255, 255, 255, 2, 1,		255, 255, 4,	 3,		255, 255, 0,	 0,		0,	 0	 },
			{ 0, 255, 255, 255, 2, 1,		255, 255, 5,	 4,		3,	 255, 0,	 0,		0,	 0	 },
			{ 0, 255, 255, 255, 2, 1,		255, 255, 6,	 5,		4,	 3,		0,	 0,		0,	 0	 },
			{ 0, 255, 255, 255, 3, 2,		1,	 255, 4,	 255, 255, 255, 0,	 0,		0,	 0	 },
			{ 0, 255, 255, 255, 3, 2,		1,	 255, 5,	 4,		255, 255, 0,	 0,		0,	 0	 },
			{ 0, 255, 255, 255, 3, 2,		1,	 255, 6,	 5,		4,	 255, 0,	 0,		0,	 0	 },
			{ 0, 255, 255, 255, 3, 2,		1,	 255, 7,	 6,		5,	 4,		0,	 0,		0,	 0	 },
			{ 0, 255, 255, 255, 4, 3,		2,	 1,		5,	 255, 255, 255, 0,	 0,		0,	 0	 },
			{ 0, 255, 255, 255, 4, 3,		2,	 1,		6,	 5,		255, 255, 0,	 0,		0,	 0	 },
			{ 0, 255, 255, 255, 4, 3,		2,	 1,		7,	 6,		5,	 255, 0,	 0,		0,	 0	 },
			{ 0, 255, 255, 255, 4, 3,		2,	 1,		8,	 7,		6,	 5,		0,	 0,		0,	 0	 },
			{ 1, 0,		255, 255, 2, 255, 255, 255, 3,	 255, 255, 255, 0,	 0,		0,	 0	 },
			{ 1, 0,		255, 255, 2, 255, 255, 255, 4,	 3,		255, 255, 0,	 0,		0,	 0	 },
			{ 1, 0,		255, 255, 2, 255, 255, 255, 5,	 4,		3,	 255, 0,	 0,		0,	 0	 },
			{ 1, 0,		255, 255, 2, 255, 255, 255, 6,	 5,		4,	 3,		0,	 0,		0,	 0	 },
			{ 1, 0,		255, 255, 3, 2,		255, 255, 4,	 255, 255, 255, 0,	 0,		0,	 0	 },
			{ 1, 0,		255, 255, 3, 2,		255, 255, 5,	 4,		255, 255, 0,	 0,		0,	 0	 },
			{ 1, 0,		255, 255, 3, 2,		255, 255, 6,	 5,		4,	 255, 0,	 0,		0,	 0	 },
			{ 1, 0,		255, 255, 3, 2,		255, 255, 7,	 6,		5,	 4,		0,	 0,		0,	 0	 },
			{ 1, 0,		255, 255, 4, 3,		2,	 255, 5,	 255, 255, 255, 0,	 0,		0,	 0	 },
			{ 1, 0,		255, 255, 4, 3,		2,	 255, 6,	 5,		255, 255, 0,	 0,		0,	 0	 },
			{ 1, 0,		255, 255, 4, 3,		2,	 255, 7,	 6,		5,	 255, 0,	 0,		0,	 0	 },
			{ 1, 0,		255, 255, 4, 3,		2,	 255, 8,	 7,		6,	 5,		0,	 0,		0,	 0	 },
			{ 1, 0,		255, 255, 5, 4,		3,	 2,		6,	 255, 255, 255, 0,	 0,		0,	 0	 },
			{ 1, 0,		255, 255, 5, 4,		3,	 2,		7,	 6,		255, 255, 0,	 0,		0,	 0	 },
			{ 1, 0,		255, 255, 5, 4,		3,	 2,		8,	 7,		6,	 255, 0,	 0,		0,	 0	 },
			{ 1, 0,		255, 255, 5, 4,		3,	 2,		9,	 8,		7,	 6,		0,	 0,		0,	 0	 },
			{ 2, 1,		0,	 255, 3, 255, 255, 255, 4,	 255, 255, 255, 0,	 0,		0,	 0	 },
			{ 2, 1,		0,	 255, 3, 255, 255, 255, 5,	 4,		255, 255, 0,	 0,		0,	 0	 },
			{ 2, 1,		0,	 255, 3, 255, 255, 255, 6,	 5,		4,	 255, 0,	 0,		0,	 0	 },
			{ 2, 1,		0,	 255, 3, 255, 255, 255, 7,	 6,		5,	 4,		0,	 0,		0,	 0	 },
			{ 2, 1,		0,	 255, 4, 3,		255, 255, 5,	 255, 255, 255, 0,	 0,		0,	 0	 },
			{ 2, 1,		0,	 255, 4, 3,		255, 255, 6,	 5,		255, 255, 0,	 0,		0,	 0	 },
			{ 2, 1,		0,	 255, 4, 3,		255, 255, 7,	 6,		5,	 255, 0,	 0,		0,	 0	 },
			{ 2, 1,		0,	 255, 4, 3,		255, 255, 8,	 7,		6,	 5,		0,	 0,		0,	 0	 },
			{ 2, 1,		0,	 255, 5, 4,		3,	 255, 6,	 255, 255, 255, 0,	 0,		0,	 0	 },
			{ 2, 1,		0,	 255, 5, 4,		3,	 255, 7,	 6,		255, 255, 0,	 0,		0,	 0	 },
			{ 2, 1,		0,	 255, 5, 4,		3,	 255, 8,	 7,		6,	 255, 0,	 0,		0,	 0	 },
			{ 2, 1,		0,	 255, 5, 4,		3,	 255, 9,	 8,		7,	 6,		0,	 0,		0,	 0	 },
			{ 2, 1,		0,	 255, 6, 5,		4,	 3,		7,	 255, 255, 255, 0,	 0,		0,	 0	 },
			{ 2, 1,		0,	 255, 6, 5,		4,	 3,		8,	 7,		255, 255, 0,	 0,		0,	 0	 },
			{ 2, 1,		0,	 255, 6, 5,		4,	 3,		9,	 8,		7,	 255, 0,	 0,		0,	 0	 },
			{ 2, 1,		0,	 255, 6, 5,		4,	 3,		10, 9,	 8,		7,	 0,	0,	 0,		0	},
			{ 3, 2,		1,	 0,		4, 255, 255, 255, 5,	 255, 255, 255, 0,	 0,		0,	 0	 },
			{ 3, 2,		1,	 0,		4, 255, 255, 255, 6,	 5,		255, 255, 0,	 0,		0,	 0	 },
			{ 3, 2,		1,	 0,		4, 255, 255, 255, 7,	 6,		5,	 255, 0,	 0,		0,	 0	 },
			{ 3, 2,		1,	 0,		4, 255, 255, 255, 8,	 7,		6,	 5,		0,	 0,		0,	 0	 },
			{ 3, 2,		1,	 0,		5, 4,		255, 255, 6,	 255, 255, 255, 0,	 0,		0,	 0	 },
			{ 3, 2,		1,	 0,		5, 4,		255, 255, 7,	 6,		255, 255, 0,	 0,		0,	 0	 },
			{ 3, 2,		1,	 0,		5, 4,		255, 255, 8,	 7,		6,	 255, 0,	 0,		0,	 0	 },
			{ 3, 2,		1,	 0,		5, 4,		255, 255, 9,	 8,		7,	 6,		0,	 0,		0,	 0	 },
			{ 3, 2,		1,	 0,		6, 5,		4,	 255, 7,	 255, 255, 255, 0,	 0,		0,	 0	 },
			{ 3, 2,		1,	 0,		6, 5,		4,	 255, 8,	 7,		255, 255, 0,	 0,		0,	 0	 },
			{ 3, 2,		1,	 0,		6, 5,		4,	 255, 9,	 8,		7,	 255, 0,	 0,		0,	 0	 },
			{ 3, 2,		1,	 0,		6, 5,		4,	 255, 10, 9,	 8,		7,	 0,	0,	 0,		0	},
			{ 3, 2,		1,	 0,		7, 6,		5,	 4,		8,	 255, 255, 255, 0,	 0,		0,	 0	 },
			{ 3, 2,		1,	 0,		7, 6,		5,	 4,		9,	 8,		255, 255, 0,	 0,		0,	 0	 },
			{ 3, 2,		1,	 0,		7, 6,		5,	 4,		10, 9,	 8,		255, 0,	0,	 0,		0	},
			{ 3, 2,		1,	 0,		7, 6,		5,	 4,		11, 10,	9,	 8,		0,	 0,		0,	 0	 }
		};
		/* number of two bytes : 64 */
		/* number of two + three bytes : 145 */
		/* number of two + three + four bytes : 209 */
		const uint8_t __big_index_utf8_to_utf_16[4'096][2] = {
			{209,	 12},
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 145, 3 },
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 146, 4 },
			{ 209, 12},
			{ 149, 4 },
			{ 161, 4 },
			{ 64,	4 },
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 147, 5 },
			{ 209, 12},
			{ 150, 5 },
			{ 162, 5 },
			{ 65,	5 },
			{ 209, 12},
			{ 153, 5 },
			{ 165, 5 },
			{ 67,	5 },
			{ 177, 5 },
			{ 73,	5 },
			{ 91,	5 },
			{ 64,	4 },
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 148, 6 },
			{ 209, 12},
			{ 151, 6 },
			{ 163, 6 },
			{ 66,	6 },
			{ 209, 12},
			{ 154, 6 },
			{ 166, 6 },
			{ 68,	6 },
			{ 178, 6 },
			{ 74,	6 },
			{ 92,	6 },
			{ 64,	4 },
			{ 209, 12},
			{ 157, 6 },
			{ 169, 6 },
			{ 70,	6 },
			{ 181, 6 },
			{ 76,	6 },
			{ 94,	6 },
			{ 65,	5 },
			{ 193, 6 },
			{ 82,	6 },
			{ 100, 6 },
			{ 67,	5 },
			{ 118, 6 },
			{ 73,	5 },
			{ 91,	5 },
			{ 0,	 6 },
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 152, 7 },
			{ 164, 7 },
			{ 145, 3 },
			{ 209, 12},
			{ 155, 7 },
			{ 167, 7 },
			{ 69,	7 },
			{ 179, 7 },
			{ 75,	7 },
			{ 93,	7 },
			{ 64,	4 },
			{ 209, 12},
			{ 158, 7 },
			{ 170, 7 },
			{ 71,	7 },
			{ 182, 7 },
			{ 77,	7 },
			{ 95,	7 },
			{ 65,	5 },
			{ 194, 7 },
			{ 83,	7 },
			{ 101, 7 },
			{ 67,	5 },
			{ 119, 7 },
			{ 73,	5 },
			{ 91,	5 },
			{ 1,	 7 },
			{ 209, 12},
			{ 209, 12},
			{ 173, 7 },
			{ 148, 6 },
			{ 185, 7 },
			{ 79,	7 },
			{ 97,	7 },
			{ 66,	6 },
			{ 197, 7 },
			{ 85,	7 },
			{ 103, 7 },
			{ 68,	6 },
			{ 121, 7 },
			{ 74,	6 },
			{ 92,	6 },
			{ 2,	 7 },
			{ 209, 12},
			{ 157, 6 },
			{ 109, 7 },
			{ 70,	6 },
			{ 127, 7 },
			{ 76,	6 },
			{ 94,	6 },
			{ 4,	 7 },
			{ 193, 6 },
			{ 82,	6 },
			{ 100, 6 },
			{ 8,	 7 },
			{ 118, 6 },
			{ 16,	7 },
			{ 32,	7 },
			{ 0,	 6 },
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 145, 3 },
			{ 209, 12},
			{ 156, 8 },
			{ 168, 8 },
			{ 146, 4 },
			{ 180, 8 },
			{ 149, 4 },
			{ 161, 4 },
			{ 64,	4 },
			{ 209, 12},
			{ 159, 8 },
			{ 171, 8 },
			{ 72,	8 },
			{ 183, 8 },
			{ 78,	8 },
			{ 96,	8 },
			{ 65,	5 },
			{ 195, 8 },
			{ 84,	8 },
			{ 102, 8 },
			{ 67,	5 },
			{ 120, 8 },
			{ 73,	5 },
			{ 91,	5 },
			{ 64,	4 },
			{ 209, 12},
			{ 209, 12},
			{ 174, 8 },
			{ 148, 6 },
			{ 186, 8 },
			{ 80,	8 },
			{ 98,	8 },
			{ 66,	6 },
			{ 198, 8 },
			{ 86,	8 },
			{ 104, 8 },
			{ 68,	6 },
			{ 122, 8 },
			{ 74,	6 },
			{ 92,	6 },
			{ 3,	 8 },
			{ 209, 12},
			{ 157, 6 },
			{ 110, 8 },
			{ 70,	6 },
			{ 128, 8 },
			{ 76,	6 },
			{ 94,	6 },
			{ 5,	 8 },
			{ 193, 6 },
			{ 82,	6 },
			{ 100, 6 },
			{ 9,	 8 },
			{ 118, 6 },
			{ 17,	8 },
			{ 33,	8 },
			{ 0,	 6 },
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 189, 8 },
			{ 152, 7 },
			{ 164, 7 },
			{ 145, 3 },
			{ 201, 8 },
			{ 88,	8 },
			{ 106, 8 },
			{ 69,	7 },
			{ 124, 8 },
			{ 75,	7 },
			{ 93,	7 },
			{ 64,	4 },
			{ 209, 12},
			{ 158, 7 },
			{ 112, 8 },
			{ 71,	7 },
			{ 130, 8 },
			{ 77,	7 },
			{ 95,	7 },
			{ 6,	 8 },
			{ 194, 7 },
			{ 83,	7 },
			{ 101, 7 },
			{ 10,	8 },
			{ 119, 7 },
			{ 18,	8 },
			{ 34,	8 },
			{ 1,	 7 },
			{ 209, 12},
			{ 209, 12},
			{ 173, 7 },
			{ 148, 6 },
			{ 136, 8 },
			{ 79,	7 },
			{ 97,	7 },
			{ 66,	6 },
			{ 197, 7 },
			{ 85,	7 },
			{ 103, 7 },
			{ 12,	8 },
			{ 121, 7 },
			{ 20,	8 },
			{ 36,	8 },
			{ 2,	 7 },
			{ 209, 12},
			{ 157, 6 },
			{ 109, 7 },
			{ 70,	6 },
			{ 127, 7 },
			{ 24,	8 },
			{ 40,	8 },
			{ 4,	 7 },
			{ 193, 6 },
			{ 82,	6 },
			{ 48,	8 },
			{ 8,	 7 },
			{ 118, 6 },
			{ 16,	7 },
			{ 32,	7 },
			{ 0,	 6 },
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 145, 3 },
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 146, 4 },
			{ 209, 12},
			{ 149, 4 },
			{ 161, 4 },
			{ 64,	4 },
			{ 209, 12},
			{ 160, 9 },
			{ 172, 9 },
			{ 147, 5 },
			{ 184, 9 },
			{ 150, 5 },
			{ 162, 5 },
			{ 65,	5 },
			{ 196, 9 },
			{ 153, 5 },
			{ 165, 5 },
			{ 67,	5 },
			{ 177, 5 },
			{ 73,	5 },
			{ 91,	5 },
			{ 64,	4 },
			{ 209, 12},
			{ 209, 12},
			{ 175, 9 },
			{ 148, 6 },
			{ 187, 9 },
			{ 81,	9 },
			{ 99,	9 },
			{ 66,	6 },
			{ 199, 9 },
			{ 87,	9 },
			{ 105, 9 },
			{ 68,	6 },
			{ 123, 9 },
			{ 74,	6 },
			{ 92,	6 },
			{ 64,	4 },
			{ 209, 12},
			{ 157, 6 },
			{ 111, 9 },
			{ 70,	6 },
			{ 129, 9 },
			{ 76,	6 },
			{ 94,	6 },
			{ 65,	5 },
			{ 193, 6 },
			{ 82,	6 },
			{ 100, 6 },
			{ 67,	5 },
			{ 118, 6 },
			{ 73,	5 },
			{ 91,	5 },
			{ 0,	 6 },
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 190, 9 },
			{ 152, 7 },
			{ 164, 7 },
			{ 145, 3 },
			{ 202, 9 },
			{ 89,	9 },
			{ 107, 9 },
			{ 69,	7 },
			{ 125, 9 },
			{ 75,	7 },
			{ 93,	7 },
			{ 64,	4 },
			{ 209, 12},
			{ 158, 7 },
			{ 113, 9 },
			{ 71,	7 },
			{ 131, 9 },
			{ 77,	7 },
			{ 95,	7 },
			{ 7,	 9 },
			{ 194, 7 },
			{ 83,	7 },
			{ 101, 7 },
			{ 11,	9 },
			{ 119, 7 },
			{ 19,	9 },
			{ 35,	9 },
			{ 1,	 7 },
			{ 209, 12},
			{ 209, 12},
			{ 173, 7 },
			{ 148, 6 },
			{ 137, 9 },
			{ 79,	7 },
			{ 97,	7 },
			{ 66,	6 },
			{ 197, 7 },
			{ 85,	7 },
			{ 103, 7 },
			{ 13,	9 },
			{ 121, 7 },
			{ 21,	9 },
			{ 37,	9 },
			{ 2,	 7 },
			{ 209, 12},
			{ 157, 6 },
			{ 109, 7 },
			{ 70,	6 },
			{ 127, 7 },
			{ 25,	9 },
			{ 41,	9 },
			{ 4,	 7 },
			{ 193, 6 },
			{ 82,	6 },
			{ 49,	9 },
			{ 8,	 7 },
			{ 118, 6 },
			{ 16,	7 },
			{ 32,	7 },
			{ 0,	 6 },
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 145, 3 },
			{ 205, 9 },
			{ 156, 8 },
			{ 168, 8 },
			{ 146, 4 },
			{ 180, 8 },
			{ 149, 4 },
			{ 161, 4 },
			{ 64,	4 },
			{ 209, 12},
			{ 159, 8 },
			{ 115, 9 },
			{ 72,	8 },
			{ 133, 9 },
			{ 78,	8 },
			{ 96,	8 },
			{ 65,	5 },
			{ 195, 8 },
			{ 84,	8 },
			{ 102, 8 },
			{ 67,	5 },
			{ 120, 8 },
			{ 73,	5 },
			{ 91,	5 },
			{ 64,	4 },
			{ 209, 12},
			{ 209, 12},
			{ 174, 8 },
			{ 148, 6 },
			{ 139, 9 },
			{ 80,	8 },
			{ 98,	8 },
			{ 66,	6 },
			{ 198, 8 },
			{ 86,	8 },
			{ 104, 8 },
			{ 14,	9 },
			{ 122, 8 },
			{ 22,	9 },
			{ 38,	9 },
			{ 3,	 8 },
			{ 209, 12},
			{ 157, 6 },
			{ 110, 8 },
			{ 70,	6 },
			{ 128, 8 },
			{ 26,	9 },
			{ 42,	9 },
			{ 5,	 8 },
			{ 193, 6 },
			{ 82,	6 },
			{ 50,	9 },
			{ 9,	 8 },
			{ 118, 6 },
			{ 17,	8 },
			{ 33,	8 },
			{ 0,	 6 },
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 189, 8 },
			{ 152, 7 },
			{ 164, 7 },
			{ 145, 3 },
			{ 201, 8 },
			{ 88,	8 },
			{ 106, 8 },
			{ 69,	7 },
			{ 124, 8 },
			{ 75,	7 },
			{ 93,	7 },
			{ 64,	4 },
			{ 209, 12},
			{ 158, 7 },
			{ 112, 8 },
			{ 71,	7 },
			{ 130, 8 },
			{ 28,	9 },
			{ 44,	9 },
			{ 6,	 8 },
			{ 194, 7 },
			{ 83,	7 },
			{ 52,	9 },
			{ 10,	8 },
			{ 119, 7 },
			{ 18,	8 },
			{ 34,	8 },
			{ 1,	 7 },
			{ 209, 12},
			{ 209, 12},
			{ 173, 7 },
			{ 148, 6 },
			{ 136, 8 },
			{ 79,	7 },
			{ 97,	7 },
			{ 66,	6 },
			{ 197, 7 },
			{ 85,	7 },
			{ 56,	9 },
			{ 12,	8 },
			{ 121, 7 },
			{ 20,	8 },
			{ 36,	8 },
			{ 2,	 7 },
			{ 209, 12},
			{ 157, 6 },
			{ 109, 7 },
			{ 70,	6 },
			{ 127, 7 },
			{ 24,	8 },
			{ 40,	8 },
			{ 4,	 7 },
			{ 193, 6 },
			{ 82,	6 },
			{ 48,	8 },
			{ 8,	 7 },
			{ 118, 6 },
			{ 16,	7 },
			{ 32,	7 },
			{ 0,	 6 },
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 145, 3 },
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 146, 4 },
			{ 209, 12},
			{ 149, 4 },
			{ 161, 4 },
			{ 64,	4 },
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 147, 5 },
			{ 209, 12},
			{ 150, 5 },
			{ 162, 5 },
			{ 65,	5 },
			{ 209, 12},
			{ 153, 5 },
			{ 165, 5 },
			{ 67,	5 },
			{ 177, 5 },
			{ 73,	5 },
			{ 91,	5 },
			{ 64,	4 },
			{ 209, 12},
			{ 209, 12},
			{ 176, 10},
			{ 148, 6 },
			{ 188, 10},
			{ 151, 6 },
			{ 163, 6 },
			{ 66,	6 },
			{ 200, 10},
			{ 154, 6 },
			{ 166, 6 },
			{ 68,	6 },
			{ 178, 6 },
			{ 74,	6 },
			{ 92,	6 },
			{ 64,	4 },
			{ 209, 12},
			{ 157, 6 },
			{ 169, 6 },
			{ 70,	6 },
			{ 181, 6 },
			{ 76,	6 },
			{ 94,	6 },
			{ 65,	5 },
			{ 193, 6 },
			{ 82,	6 },
			{ 100, 6 },
			{ 67,	5 },
			{ 118, 6 },
			{ 73,	5 },
			{ 91,	5 },
			{ 0,	 6 },
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 191, 10},
			{ 152, 7 },
			{ 164, 7 },
			{ 145, 3 },
			{ 203, 10},
			{ 90,	10},
			{ 108, 10},
			{ 69,	7 },
			{ 126, 10},
			{ 75,	7 },
			{ 93,	7 },
			{ 64,	4 },
			{ 209, 12},
			{ 158, 7 },
			{ 114, 10},
			{ 71,	7 },
			{ 132, 10},
			{ 77,	7 },
			{ 95,	7 },
			{ 65,	5 },
			{ 194, 7 },
			{ 83,	7 },
			{ 101, 7 },
			{ 67,	5 },
			{ 119, 7 },
			{ 73,	5 },
			{ 91,	5 },
			{ 1,	 7 },
			{ 209, 12},
			{ 209, 12},
			{ 173, 7 },
			{ 148, 6 },
			{ 138, 10},
			{ 79,	7 },
			{ 97,	7 },
			{ 66,	6 },
			{ 197, 7 },
			{ 85,	7 },
			{ 103, 7 },
			{ 68,	6 },
			{ 121, 7 },
			{ 74,	6 },
			{ 92,	6 },
			{ 2,	 7 },
			{ 209, 12},
			{ 157, 6 },
			{ 109, 7 },
			{ 70,	6 },
			{ 127, 7 },
			{ 76,	6 },
			{ 94,	6 },
			{ 4,	 7 },
			{ 193, 6 },
			{ 82,	6 },
			{ 100, 6 },
			{ 8,	 7 },
			{ 118, 6 },
			{ 16,	7 },
			{ 32,	7 },
			{ 0,	 6 },
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 145, 3 },
			{ 206, 10},
			{ 156, 8 },
			{ 168, 8 },
			{ 146, 4 },
			{ 180, 8 },
			{ 149, 4 },
			{ 161, 4 },
			{ 64,	4 },
			{ 209, 12},
			{ 159, 8 },
			{ 116, 10},
			{ 72,	8 },
			{ 134, 10},
			{ 78,	8 },
			{ 96,	8 },
			{ 65,	5 },
			{ 195, 8 },
			{ 84,	8 },
			{ 102, 8 },
			{ 67,	5 },
			{ 120, 8 },
			{ 73,	5 },
			{ 91,	5 },
			{ 64,	4 },
			{ 209, 12},
			{ 209, 12},
			{ 174, 8 },
			{ 148, 6 },
			{ 140, 10},
			{ 80,	8 },
			{ 98,	8 },
			{ 66,	6 },
			{ 198, 8 },
			{ 86,	8 },
			{ 104, 8 },
			{ 15,	10},
			{ 122, 8 },
			{ 23,	10},
			{ 39,	10},
			{ 3,	 8 },
			{ 209, 12},
			{ 157, 6 },
			{ 110, 8 },
			{ 70,	6 },
			{ 128, 8 },
			{ 27,	10},
			{ 43,	10},
			{ 5,	 8 },
			{ 193, 6 },
			{ 82,	6 },
			{ 51,	10},
			{ 9,	 8 },
			{ 118, 6 },
			{ 17,	8 },
			{ 33,	8 },
			{ 0,	 6 },
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 189, 8 },
			{ 152, 7 },
			{ 164, 7 },
			{ 145, 3 },
			{ 201, 8 },
			{ 88,	8 },
			{ 106, 8 },
			{ 69,	7 },
			{ 124, 8 },
			{ 75,	7 },
			{ 93,	7 },
			{ 64,	4 },
			{ 209, 12},
			{ 158, 7 },
			{ 112, 8 },
			{ 71,	7 },
			{ 130, 8 },
			{ 29,	10},
			{ 45,	10},
			{ 6,	 8 },
			{ 194, 7 },
			{ 83,	7 },
			{ 53,	10},
			{ 10,	8 },
			{ 119, 7 },
			{ 18,	8 },
			{ 34,	8 },
			{ 1,	 7 },
			{ 209, 12},
			{ 209, 12},
			{ 173, 7 },
			{ 148, 6 },
			{ 136, 8 },
			{ 79,	7 },
			{ 97,	7 },
			{ 66,	6 },
			{ 197, 7 },
			{ 85,	7 },
			{ 57,	10},
			{ 12,	8 },
			{ 121, 7 },
			{ 20,	8 },
			{ 36,	8 },
			{ 2,	 7 },
			{ 209, 12},
			{ 157, 6 },
			{ 109, 7 },
			{ 70,	6 },
			{ 127, 7 },
			{ 24,	8 },
			{ 40,	8 },
			{ 4,	 7 },
			{ 193, 6 },
			{ 82,	6 },
			{ 48,	8 },
			{ 8,	 7 },
			{ 118, 6 },
			{ 16,	7 },
			{ 32,	7 },
			{ 0,	 6 },
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 145, 3 },
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 146, 4 },
			{ 209, 12},
			{ 149, 4 },
			{ 161, 4 },
			{ 64,	4 },
			{ 209, 12},
			{ 160, 9 },
			{ 172, 9 },
			{ 147, 5 },
			{ 184, 9 },
			{ 150, 5 },
			{ 162, 5 },
			{ 65,	5 },
			{ 196, 9 },
			{ 153, 5 },
			{ 165, 5 },
			{ 67,	5 },
			{ 177, 5 },
			{ 73,	5 },
			{ 91,	5 },
			{ 64,	4 },
			{ 209, 12},
			{ 209, 12},
			{ 175, 9 },
			{ 148, 6 },
			{ 142, 10},
			{ 81,	9 },
			{ 99,	9 },
			{ 66,	6 },
			{ 199, 9 },
			{ 87,	9 },
			{ 105, 9 },
			{ 68,	6 },
			{ 123, 9 },
			{ 74,	6 },
			{ 92,	6 },
			{ 64,	4 },
			{ 209, 12},
			{ 157, 6 },
			{ 111, 9 },
			{ 70,	6 },
			{ 129, 9 },
			{ 76,	6 },
			{ 94,	6 },
			{ 65,	5 },
			{ 193, 6 },
			{ 82,	6 },
			{ 100, 6 },
			{ 67,	5 },
			{ 118, 6 },
			{ 73,	5 },
			{ 91,	5 },
			{ 0,	 6 },
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 190, 9 },
			{ 152, 7 },
			{ 164, 7 },
			{ 145, 3 },
			{ 202, 9 },
			{ 89,	9 },
			{ 107, 9 },
			{ 69,	7 },
			{ 125, 9 },
			{ 75,	7 },
			{ 93,	7 },
			{ 64,	4 },
			{ 209, 12},
			{ 158, 7 },
			{ 113, 9 },
			{ 71,	7 },
			{ 131, 9 },
			{ 30,	10},
			{ 46,	10},
			{ 7,	 9 },
			{ 194, 7 },
			{ 83,	7 },
			{ 54,	10},
			{ 11,	9 },
			{ 119, 7 },
			{ 19,	9 },
			{ 35,	9 },
			{ 1,	 7 },
			{ 209, 12},
			{ 209, 12},
			{ 173, 7 },
			{ 148, 6 },
			{ 137, 9 },
			{ 79,	7 },
			{ 97,	7 },
			{ 66,	6 },
			{ 197, 7 },
			{ 85,	7 },
			{ 58,	10},
			{ 13,	9 },
			{ 121, 7 },
			{ 21,	9 },
			{ 37,	9 },
			{ 2,	 7 },
			{ 209, 12},
			{ 157, 6 },
			{ 109, 7 },
			{ 70,	6 },
			{ 127, 7 },
			{ 25,	9 },
			{ 41,	9 },
			{ 4,	 7 },
			{ 193, 6 },
			{ 82,	6 },
			{ 49,	9 },
			{ 8,	 7 },
			{ 118, 6 },
			{ 16,	7 },
			{ 32,	7 },
			{ 0,	 6 },
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 145, 3 },
			{ 205, 9 },
			{ 156, 8 },
			{ 168, 8 },
			{ 146, 4 },
			{ 180, 8 },
			{ 149, 4 },
			{ 161, 4 },
			{ 64,	4 },
			{ 209, 12},
			{ 159, 8 },
			{ 115, 9 },
			{ 72,	8 },
			{ 133, 9 },
			{ 78,	8 },
			{ 96,	8 },
			{ 65,	5 },
			{ 195, 8 },
			{ 84,	8 },
			{ 102, 8 },
			{ 67,	5 },
			{ 120, 8 },
			{ 73,	5 },
			{ 91,	5 },
			{ 64,	4 },
			{ 209, 12},
			{ 209, 12},
			{ 174, 8 },
			{ 148, 6 },
			{ 139, 9 },
			{ 80,	8 },
			{ 98,	8 },
			{ 66,	6 },
			{ 198, 8 },
			{ 86,	8 },
			{ 60,	10},
			{ 14,	9 },
			{ 122, 8 },
			{ 22,	9 },
			{ 38,	9 },
			{ 3,	 8 },
			{ 209, 12},
			{ 157, 6 },
			{ 110, 8 },
			{ 70,	6 },
			{ 128, 8 },
			{ 26,	9 },
			{ 42,	9 },
			{ 5,	 8 },
			{ 193, 6 },
			{ 82,	6 },
			{ 50,	9 },
			{ 9,	 8 },
			{ 118, 6 },
			{ 17,	8 },
			{ 33,	8 },
			{ 0,	 6 },
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 189, 8 },
			{ 152, 7 },
			{ 164, 7 },
			{ 145, 3 },
			{ 201, 8 },
			{ 88,	8 },
			{ 106, 8 },
			{ 69,	7 },
			{ 124, 8 },
			{ 75,	7 },
			{ 93,	7 },
			{ 64,	4 },
			{ 209, 12},
			{ 158, 7 },
			{ 112, 8 },
			{ 71,	7 },
			{ 130, 8 },
			{ 28,	9 },
			{ 44,	9 },
			{ 6,	 8 },
			{ 194, 7 },
			{ 83,	7 },
			{ 52,	9 },
			{ 10,	8 },
			{ 119, 7 },
			{ 18,	8 },
			{ 34,	8 },
			{ 1,	 7 },
			{ 209, 12},
			{ 209, 12},
			{ 173, 7 },
			{ 148, 6 },
			{ 136, 8 },
			{ 79,	7 },
			{ 97,	7 },
			{ 66,	6 },
			{ 197, 7 },
			{ 85,	7 },
			{ 56,	9 },
			{ 12,	8 },
			{ 121, 7 },
			{ 20,	8 },
			{ 36,	8 },
			{ 2,	 7 },
			{ 209, 12},
			{ 157, 6 },
			{ 109, 7 },
			{ 70,	6 },
			{ 127, 7 },
			{ 24,	8 },
			{ 40,	8 },
			{ 4,	 7 },
			{ 193, 6 },
			{ 82,	6 },
			{ 48,	8 },
			{ 8,	 7 },
			{ 118, 6 },
			{ 16,	7 },
			{ 32,	7 },
			{ 0,	 6 },
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 145, 3 },
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 146, 4 },
			{ 209, 12},
			{ 149, 4 },
			{ 161, 4 },
			{ 64,	4 },
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 147, 5 },
			{ 209, 12},
			{ 150, 5 },
			{ 162, 5 },
			{ 65,	5 },
			{ 209, 12},
			{ 153, 5 },
			{ 165, 5 },
			{ 67,	5 },
			{ 177, 5 },
			{ 73,	5 },
			{ 91,	5 },
			{ 64,	4 },
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 148, 6 },
			{ 209, 12},
			{ 151, 6 },
			{ 163, 6 },
			{ 66,	6 },
			{ 209, 12},
			{ 154, 6 },
			{ 166, 6 },
			{ 68,	6 },
			{ 178, 6 },
			{ 74,	6 },
			{ 92,	6 },
			{ 64,	4 },
			{ 209, 12},
			{ 157, 6 },
			{ 169, 6 },
			{ 70,	6 },
			{ 181, 6 },
			{ 76,	6 },
			{ 94,	6 },
			{ 65,	5 },
			{ 193, 6 },
			{ 82,	6 },
			{ 100, 6 },
			{ 67,	5 },
			{ 118, 6 },
			{ 73,	5 },
			{ 91,	5 },
			{ 0,	 6 },
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 192, 11},
			{ 152, 7 },
			{ 164, 7 },
			{ 145, 3 },
			{ 204, 11},
			{ 155, 7 },
			{ 167, 7 },
			{ 69,	7 },
			{ 179, 7 },
			{ 75,	7 },
			{ 93,	7 },
			{ 64,	4 },
			{ 209, 12},
			{ 158, 7 },
			{ 170, 7 },
			{ 71,	7 },
			{ 182, 7 },
			{ 77,	7 },
			{ 95,	7 },
			{ 65,	5 },
			{ 194, 7 },
			{ 83,	7 },
			{ 101, 7 },
			{ 67,	5 },
			{ 119, 7 },
			{ 73,	5 },
			{ 91,	5 },
			{ 1,	 7 },
			{ 209, 12},
			{ 209, 12},
			{ 173, 7 },
			{ 148, 6 },
			{ 185, 7 },
			{ 79,	7 },
			{ 97,	7 },
			{ 66,	6 },
			{ 197, 7 },
			{ 85,	7 },
			{ 103, 7 },
			{ 68,	6 },
			{ 121, 7 },
			{ 74,	6 },
			{ 92,	6 },
			{ 2,	 7 },
			{ 209, 12},
			{ 157, 6 },
			{ 109, 7 },
			{ 70,	6 },
			{ 127, 7 },
			{ 76,	6 },
			{ 94,	6 },
			{ 4,	 7 },
			{ 193, 6 },
			{ 82,	6 },
			{ 100, 6 },
			{ 8,	 7 },
			{ 118, 6 },
			{ 16,	7 },
			{ 32,	7 },
			{ 0,	 6 },
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 145, 3 },
			{ 207, 11},
			{ 156, 8 },
			{ 168, 8 },
			{ 146, 4 },
			{ 180, 8 },
			{ 149, 4 },
			{ 161, 4 },
			{ 64,	4 },
			{ 209, 12},
			{ 159, 8 },
			{ 117, 11},
			{ 72,	8 },
			{ 135, 11},
			{ 78,	8 },
			{ 96,	8 },
			{ 65,	5 },
			{ 195, 8 },
			{ 84,	8 },
			{ 102, 8 },
			{ 67,	5 },
			{ 120, 8 },
			{ 73,	5 },
			{ 91,	5 },
			{ 64,	4 },
			{ 209, 12},
			{ 209, 12},
			{ 174, 8 },
			{ 148, 6 },
			{ 141, 11},
			{ 80,	8 },
			{ 98,	8 },
			{ 66,	6 },
			{ 198, 8 },
			{ 86,	8 },
			{ 104, 8 },
			{ 68,	6 },
			{ 122, 8 },
			{ 74,	6 },
			{ 92,	6 },
			{ 3,	 8 },
			{ 209, 12},
			{ 157, 6 },
			{ 110, 8 },
			{ 70,	6 },
			{ 128, 8 },
			{ 76,	6 },
			{ 94,	6 },
			{ 5,	 8 },
			{ 193, 6 },
			{ 82,	6 },
			{ 100, 6 },
			{ 9,	 8 },
			{ 118, 6 },
			{ 17,	8 },
			{ 33,	8 },
			{ 0,	 6 },
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 189, 8 },
			{ 152, 7 },
			{ 164, 7 },
			{ 145, 3 },
			{ 201, 8 },
			{ 88,	8 },
			{ 106, 8 },
			{ 69,	7 },
			{ 124, 8 },
			{ 75,	7 },
			{ 93,	7 },
			{ 64,	4 },
			{ 209, 12},
			{ 158, 7 },
			{ 112, 8 },
			{ 71,	7 },
			{ 130, 8 },
			{ 77,	7 },
			{ 95,	7 },
			{ 6,	 8 },
			{ 194, 7 },
			{ 83,	7 },
			{ 101, 7 },
			{ 10,	8 },
			{ 119, 7 },
			{ 18,	8 },
			{ 34,	8 },
			{ 1,	 7 },
			{ 209, 12},
			{ 209, 12},
			{ 173, 7 },
			{ 148, 6 },
			{ 136, 8 },
			{ 79,	7 },
			{ 97,	7 },
			{ 66,	6 },
			{ 197, 7 },
			{ 85,	7 },
			{ 103, 7 },
			{ 12,	8 },
			{ 121, 7 },
			{ 20,	8 },
			{ 36,	8 },
			{ 2,	 7 },
			{ 209, 12},
			{ 157, 6 },
			{ 109, 7 },
			{ 70,	6 },
			{ 127, 7 },
			{ 24,	8 },
			{ 40,	8 },
			{ 4,	 7 },
			{ 193, 6 },
			{ 82,	6 },
			{ 48,	8 },
			{ 8,	 7 },
			{ 118, 6 },
			{ 16,	7 },
			{ 32,	7 },
			{ 0,	 6 },
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 145, 3 },
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 146, 4 },
			{ 209, 12},
			{ 149, 4 },
			{ 161, 4 },
			{ 64,	4 },
			{ 209, 12},
			{ 160, 9 },
			{ 172, 9 },
			{ 147, 5 },
			{ 184, 9 },
			{ 150, 5 },
			{ 162, 5 },
			{ 65,	5 },
			{ 196, 9 },
			{ 153, 5 },
			{ 165, 5 },
			{ 67,	5 },
			{ 177, 5 },
			{ 73,	5 },
			{ 91,	5 },
			{ 64,	4 },
			{ 209, 12},
			{ 209, 12},
			{ 175, 9 },
			{ 148, 6 },
			{ 143, 11},
			{ 81,	9 },
			{ 99,	9 },
			{ 66,	6 },
			{ 199, 9 },
			{ 87,	9 },
			{ 105, 9 },
			{ 68,	6 },
			{ 123, 9 },
			{ 74,	6 },
			{ 92,	6 },
			{ 64,	4 },
			{ 209, 12},
			{ 157, 6 },
			{ 111, 9 },
			{ 70,	6 },
			{ 129, 9 },
			{ 76,	6 },
			{ 94,	6 },
			{ 65,	5 },
			{ 193, 6 },
			{ 82,	6 },
			{ 100, 6 },
			{ 67,	5 },
			{ 118, 6 },
			{ 73,	5 },
			{ 91,	5 },
			{ 0,	 6 },
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 190, 9 },
			{ 152, 7 },
			{ 164, 7 },
			{ 145, 3 },
			{ 202, 9 },
			{ 89,	9 },
			{ 107, 9 },
			{ 69,	7 },
			{ 125, 9 },
			{ 75,	7 },
			{ 93,	7 },
			{ 64,	4 },
			{ 209, 12},
			{ 158, 7 },
			{ 113, 9 },
			{ 71,	7 },
			{ 131, 9 },
			{ 31,	11},
			{ 47,	11},
			{ 7,	 9 },
			{ 194, 7 },
			{ 83,	7 },
			{ 55,	11},
			{ 11,	9 },
			{ 119, 7 },
			{ 19,	9 },
			{ 35,	9 },
			{ 1,	 7 },
			{ 209, 12},
			{ 209, 12},
			{ 173, 7 },
			{ 148, 6 },
			{ 137, 9 },
			{ 79,	7 },
			{ 97,	7 },
			{ 66,	6 },
			{ 197, 7 },
			{ 85,	7 },
			{ 59,	11},
			{ 13,	9 },
			{ 121, 7 },
			{ 21,	9 },
			{ 37,	9 },
			{ 2,	 7 },
			{ 209, 12},
			{ 157, 6 },
			{ 109, 7 },
			{ 70,	6 },
			{ 127, 7 },
			{ 25,	9 },
			{ 41,	9 },
			{ 4,	 7 },
			{ 193, 6 },
			{ 82,	6 },
			{ 49,	9 },
			{ 8,	 7 },
			{ 118, 6 },
			{ 16,	7 },
			{ 32,	7 },
			{ 0,	 6 },
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 145, 3 },
			{ 205, 9 },
			{ 156, 8 },
			{ 168, 8 },
			{ 146, 4 },
			{ 180, 8 },
			{ 149, 4 },
			{ 161, 4 },
			{ 64,	4 },
			{ 209, 12},
			{ 159, 8 },
			{ 115, 9 },
			{ 72,	8 },
			{ 133, 9 },
			{ 78,	8 },
			{ 96,	8 },
			{ 65,	5 },
			{ 195, 8 },
			{ 84,	8 },
			{ 102, 8 },
			{ 67,	5 },
			{ 120, 8 },
			{ 73,	5 },
			{ 91,	5 },
			{ 64,	4 },
			{ 209, 12},
			{ 209, 12},
			{ 174, 8 },
			{ 148, 6 },
			{ 139, 9 },
			{ 80,	8 },
			{ 98,	8 },
			{ 66,	6 },
			{ 198, 8 },
			{ 86,	8 },
			{ 61,	11},
			{ 14,	9 },
			{ 122, 8 },
			{ 22,	9 },
			{ 38,	9 },
			{ 3,	 8 },
			{ 209, 12},
			{ 157, 6 },
			{ 110, 8 },
			{ 70,	6 },
			{ 128, 8 },
			{ 26,	9 },
			{ 42,	9 },
			{ 5,	 8 },
			{ 193, 6 },
			{ 82,	6 },
			{ 50,	9 },
			{ 9,	 8 },
			{ 118, 6 },
			{ 17,	8 },
			{ 33,	8 },
			{ 0,	 6 },
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 189, 8 },
			{ 152, 7 },
			{ 164, 7 },
			{ 145, 3 },
			{ 201, 8 },
			{ 88,	8 },
			{ 106, 8 },
			{ 69,	7 },
			{ 124, 8 },
			{ 75,	7 },
			{ 93,	7 },
			{ 64,	4 },
			{ 209, 12},
			{ 158, 7 },
			{ 112, 8 },
			{ 71,	7 },
			{ 130, 8 },
			{ 28,	9 },
			{ 44,	9 },
			{ 6,	 8 },
			{ 194, 7 },
			{ 83,	7 },
			{ 52,	9 },
			{ 10,	8 },
			{ 119, 7 },
			{ 18,	8 },
			{ 34,	8 },
			{ 1,	 7 },
			{ 209, 12},
			{ 209, 12},
			{ 173, 7 },
			{ 148, 6 },
			{ 136, 8 },
			{ 79,	7 },
			{ 97,	7 },
			{ 66,	6 },
			{ 197, 7 },
			{ 85,	7 },
			{ 56,	9 },
			{ 12,	8 },
			{ 121, 7 },
			{ 20,	8 },
			{ 36,	8 },
			{ 2,	 7 },
			{ 209, 12},
			{ 157, 6 },
			{ 109, 7 },
			{ 70,	6 },
			{ 127, 7 },
			{ 24,	8 },
			{ 40,	8 },
			{ 4,	 7 },
			{ 193, 6 },
			{ 82,	6 },
			{ 48,	8 },
			{ 8,	 7 },
			{ 118, 6 },
			{ 16,	7 },
			{ 32,	7 },
			{ 0,	 6 },
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 145, 3 },
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 146, 4 },
			{ 209, 12},
			{ 149, 4 },
			{ 161, 4 },
			{ 64,	4 },
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 147, 5 },
			{ 209, 12},
			{ 150, 5 },
			{ 162, 5 },
			{ 65,	5 },
			{ 209, 12},
			{ 153, 5 },
			{ 165, 5 },
			{ 67,	5 },
			{ 177, 5 },
			{ 73,	5 },
			{ 91,	5 },
			{ 64,	4 },
			{ 209, 12},
			{ 209, 12},
			{ 176, 10},
			{ 148, 6 },
			{ 188, 10},
			{ 151, 6 },
			{ 163, 6 },
			{ 66,	6 },
			{ 200, 10},
			{ 154, 6 },
			{ 166, 6 },
			{ 68,	6 },
			{ 178, 6 },
			{ 74,	6 },
			{ 92,	6 },
			{ 64,	4 },
			{ 209, 12},
			{ 157, 6 },
			{ 169, 6 },
			{ 70,	6 },
			{ 181, 6 },
			{ 76,	6 },
			{ 94,	6 },
			{ 65,	5 },
			{ 193, 6 },
			{ 82,	6 },
			{ 100, 6 },
			{ 67,	5 },
			{ 118, 6 },
			{ 73,	5 },
			{ 91,	5 },
			{ 0,	 6 },
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 191, 10},
			{ 152, 7 },
			{ 164, 7 },
			{ 145, 3 },
			{ 203, 10},
			{ 90,	10},
			{ 108, 10},
			{ 69,	7 },
			{ 126, 10},
			{ 75,	7 },
			{ 93,	7 },
			{ 64,	4 },
			{ 209, 12},
			{ 158, 7 },
			{ 114, 10},
			{ 71,	7 },
			{ 132, 10},
			{ 77,	7 },
			{ 95,	7 },
			{ 65,	5 },
			{ 194, 7 },
			{ 83,	7 },
			{ 101, 7 },
			{ 67,	5 },
			{ 119, 7 },
			{ 73,	5 },
			{ 91,	5 },
			{ 1,	 7 },
			{ 209, 12},
			{ 209, 12},
			{ 173, 7 },
			{ 148, 6 },
			{ 138, 10},
			{ 79,	7 },
			{ 97,	7 },
			{ 66,	6 },
			{ 197, 7 },
			{ 85,	7 },
			{ 103, 7 },
			{ 68,	6 },
			{ 121, 7 },
			{ 74,	6 },
			{ 92,	6 },
			{ 2,	 7 },
			{ 209, 12},
			{ 157, 6 },
			{ 109, 7 },
			{ 70,	6 },
			{ 127, 7 },
			{ 76,	6 },
			{ 94,	6 },
			{ 4,	 7 },
			{ 193, 6 },
			{ 82,	6 },
			{ 100, 6 },
			{ 8,	 7 },
			{ 118, 6 },
			{ 16,	7 },
			{ 32,	7 },
			{ 0,	 6 },
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 145, 3 },
			{ 206, 10},
			{ 156, 8 },
			{ 168, 8 },
			{ 146, 4 },
			{ 180, 8 },
			{ 149, 4 },
			{ 161, 4 },
			{ 64,	4 },
			{ 209, 12},
			{ 159, 8 },
			{ 116, 10},
			{ 72,	8 },
			{ 134, 10},
			{ 78,	8 },
			{ 96,	8 },
			{ 65,	5 },
			{ 195, 8 },
			{ 84,	8 },
			{ 102, 8 },
			{ 67,	5 },
			{ 120, 8 },
			{ 73,	5 },
			{ 91,	5 },
			{ 64,	4 },
			{ 209, 12},
			{ 209, 12},
			{ 174, 8 },
			{ 148, 6 },
			{ 140, 10},
			{ 80,	8 },
			{ 98,	8 },
			{ 66,	6 },
			{ 198, 8 },
			{ 86,	8 },
			{ 62,	11},
			{ 15,	10},
			{ 122, 8 },
			{ 23,	10},
			{ 39,	10},
			{ 3,	 8 },
			{ 209, 12},
			{ 157, 6 },
			{ 110, 8 },
			{ 70,	6 },
			{ 128, 8 },
			{ 27,	10},
			{ 43,	10},
			{ 5,	 8 },
			{ 193, 6 },
			{ 82,	6 },
			{ 51,	10},
			{ 9,	 8 },
			{ 118, 6 },
			{ 17,	8 },
			{ 33,	8 },
			{ 0,	 6 },
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 189, 8 },
			{ 152, 7 },
			{ 164, 7 },
			{ 145, 3 },
			{ 201, 8 },
			{ 88,	8 },
			{ 106, 8 },
			{ 69,	7 },
			{ 124, 8 },
			{ 75,	7 },
			{ 93,	7 },
			{ 64,	4 },
			{ 209, 12},
			{ 158, 7 },
			{ 112, 8 },
			{ 71,	7 },
			{ 130, 8 },
			{ 29,	10},
			{ 45,	10},
			{ 6,	 8 },
			{ 194, 7 },
			{ 83,	7 },
			{ 53,	10},
			{ 10,	8 },
			{ 119, 7 },
			{ 18,	8 },
			{ 34,	8 },
			{ 1,	 7 },
			{ 209, 12},
			{ 209, 12},
			{ 173, 7 },
			{ 148, 6 },
			{ 136, 8 },
			{ 79,	7 },
			{ 97,	7 },
			{ 66,	6 },
			{ 197, 7 },
			{ 85,	7 },
			{ 57,	10},
			{ 12,	8 },
			{ 121, 7 },
			{ 20,	8 },
			{ 36,	8 },
			{ 2,	 7 },
			{ 209, 12},
			{ 157, 6 },
			{ 109, 7 },
			{ 70,	6 },
			{ 127, 7 },
			{ 24,	8 },
			{ 40,	8 },
			{ 4,	 7 },
			{ 193, 6 },
			{ 82,	6 },
			{ 48,	8 },
			{ 8,	 7 },
			{ 118, 6 },
			{ 16,	7 },
			{ 32,	7 },
			{ 0,	 6 },
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 145, 3 },
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 146, 4 },
			{ 209, 12},
			{ 149, 4 },
			{ 161, 4 },
			{ 64,	4 },
			{ 209, 12},
			{ 160, 9 },
			{ 172, 9 },
			{ 147, 5 },
			{ 184, 9 },
			{ 150, 5 },
			{ 162, 5 },
			{ 65,	5 },
			{ 196, 9 },
			{ 153, 5 },
			{ 165, 5 },
			{ 67,	5 },
			{ 177, 5 },
			{ 73,	5 },
			{ 91,	5 },
			{ 64,	4 },
			{ 209, 12},
			{ 209, 12},
			{ 175, 9 },
			{ 148, 6 },
			{ 142, 10},
			{ 81,	9 },
			{ 99,	9 },
			{ 66,	6 },
			{ 199, 9 },
			{ 87,	9 },
			{ 105, 9 },
			{ 68,	6 },
			{ 123, 9 },
			{ 74,	6 },
			{ 92,	6 },
			{ 64,	4 },
			{ 209, 12},
			{ 157, 6 },
			{ 111, 9 },
			{ 70,	6 },
			{ 129, 9 },
			{ 76,	6 },
			{ 94,	6 },
			{ 65,	5 },
			{ 193, 6 },
			{ 82,	6 },
			{ 100, 6 },
			{ 67,	5 },
			{ 118, 6 },
			{ 73,	5 },
			{ 91,	5 },
			{ 0,	 6 },
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 190, 9 },
			{ 152, 7 },
			{ 164, 7 },
			{ 145, 3 },
			{ 202, 9 },
			{ 89,	9 },
			{ 107, 9 },
			{ 69,	7 },
			{ 125, 9 },
			{ 75,	7 },
			{ 93,	7 },
			{ 64,	4 },
			{ 209, 12},
			{ 158, 7 },
			{ 113, 9 },
			{ 71,	7 },
			{ 131, 9 },
			{ 30,	10},
			{ 46,	10},
			{ 7,	 9 },
			{ 194, 7 },
			{ 83,	7 },
			{ 54,	10},
			{ 11,	9 },
			{ 119, 7 },
			{ 19,	9 },
			{ 35,	9 },
			{ 1,	 7 },
			{ 209, 12},
			{ 209, 12},
			{ 173, 7 },
			{ 148, 6 },
			{ 137, 9 },
			{ 79,	7 },
			{ 97,	7 },
			{ 66,	6 },
			{ 197, 7 },
			{ 85,	7 },
			{ 58,	10},
			{ 13,	9 },
			{ 121, 7 },
			{ 21,	9 },
			{ 37,	9 },
			{ 2,	 7 },
			{ 209, 12},
			{ 157, 6 },
			{ 109, 7 },
			{ 70,	6 },
			{ 127, 7 },
			{ 25,	9 },
			{ 41,	9 },
			{ 4,	 7 },
			{ 193, 6 },
			{ 82,	6 },
			{ 49,	9 },
			{ 8,	 7 },
			{ 118, 6 },
			{ 16,	7 },
			{ 32,	7 },
			{ 0,	 6 },
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 145, 3 },
			{ 205, 9 },
			{ 156, 8 },
			{ 168, 8 },
			{ 146, 4 },
			{ 180, 8 },
			{ 149, 4 },
			{ 161, 4 },
			{ 64,	4 },
			{ 209, 12},
			{ 159, 8 },
			{ 115, 9 },
			{ 72,	8 },
			{ 133, 9 },
			{ 78,	8 },
			{ 96,	8 },
			{ 65,	5 },
			{ 195, 8 },
			{ 84,	8 },
			{ 102, 8 },
			{ 67,	5 },
			{ 120, 8 },
			{ 73,	5 },
			{ 91,	5 },
			{ 64,	4 },
			{ 209, 12},
			{ 209, 12},
			{ 174, 8 },
			{ 148, 6 },
			{ 139, 9 },
			{ 80,	8 },
			{ 98,	8 },
			{ 66,	6 },
			{ 198, 8 },
			{ 86,	8 },
			{ 60,	10},
			{ 14,	9 },
			{ 122, 8 },
			{ 22,	9 },
			{ 38,	9 },
			{ 3,	 8 },
			{ 209, 12},
			{ 157, 6 },
			{ 110, 8 },
			{ 70,	6 },
			{ 128, 8 },
			{ 26,	9 },
			{ 42,	9 },
			{ 5,	 8 },
			{ 193, 6 },
			{ 82,	6 },
			{ 50,	9 },
			{ 9,	 8 },
			{ 118, 6 },
			{ 17,	8 },
			{ 33,	8 },
			{ 0,	 6 },
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 189, 8 },
			{ 152, 7 },
			{ 164, 7 },
			{ 145, 3 },
			{ 201, 8 },
			{ 88,	8 },
			{ 106, 8 },
			{ 69,	7 },
			{ 124, 8 },
			{ 75,	7 },
			{ 93,	7 },
			{ 64,	4 },
			{ 209, 12},
			{ 158, 7 },
			{ 112, 8 },
			{ 71,	7 },
			{ 130, 8 },
			{ 28,	9 },
			{ 44,	9 },
			{ 6,	 8 },
			{ 194, 7 },
			{ 83,	7 },
			{ 52,	9 },
			{ 10,	8 },
			{ 119, 7 },
			{ 18,	8 },
			{ 34,	8 },
			{ 1,	 7 },
			{ 209, 12},
			{ 209, 12},
			{ 173, 7 },
			{ 148, 6 },
			{ 136, 8 },
			{ 79,	7 },
			{ 97,	7 },
			{ 66,	6 },
			{ 197, 7 },
			{ 85,	7 },
			{ 56,	9 },
			{ 12,	8 },
			{ 121, 7 },
			{ 20,	8 },
			{ 36,	8 },
			{ 2,	 7 },
			{ 209, 12},
			{ 157, 6 },
			{ 109, 7 },
			{ 70,	6 },
			{ 127, 7 },
			{ 24,	8 },
			{ 40,	8 },
			{ 4,	 7 },
			{ 193, 6 },
			{ 82,	6 },
			{ 48,	8 },
			{ 8,	 7 },
			{ 118, 6 },
			{ 16,	7 },
			{ 32,	7 },
			{ 0,	 6 },
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 145, 3 },
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 146, 4 },
			{ 209, 12},
			{ 149, 4 },
			{ 161, 4 },
			{ 64,	4 },
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 147, 5 },
			{ 209, 12},
			{ 150, 5 },
			{ 162, 5 },
			{ 65,	5 },
			{ 209, 12},
			{ 153, 5 },
			{ 165, 5 },
			{ 67,	5 },
			{ 177, 5 },
			{ 73,	5 },
			{ 91,	5 },
			{ 64,	4 },
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 148, 6 },
			{ 209, 12},
			{ 151, 6 },
			{ 163, 6 },
			{ 66,	6 },
			{ 209, 12},
			{ 154, 6 },
			{ 166, 6 },
			{ 68,	6 },
			{ 178, 6 },
			{ 74,	6 },
			{ 92,	6 },
			{ 64,	4 },
			{ 209, 12},
			{ 157, 6 },
			{ 169, 6 },
			{ 70,	6 },
			{ 181, 6 },
			{ 76,	6 },
			{ 94,	6 },
			{ 65,	5 },
			{ 193, 6 },
			{ 82,	6 },
			{ 100, 6 },
			{ 67,	5 },
			{ 118, 6 },
			{ 73,	5 },
			{ 91,	5 },
			{ 0,	 6 },
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 152, 7 },
			{ 164, 7 },
			{ 145, 3 },
			{ 209, 12},
			{ 155, 7 },
			{ 167, 7 },
			{ 69,	7 },
			{ 179, 7 },
			{ 75,	7 },
			{ 93,	7 },
			{ 64,	4 },
			{ 209, 12},
			{ 158, 7 },
			{ 170, 7 },
			{ 71,	7 },
			{ 182, 7 },
			{ 77,	7 },
			{ 95,	7 },
			{ 65,	5 },
			{ 194, 7 },
			{ 83,	7 },
			{ 101, 7 },
			{ 67,	5 },
			{ 119, 7 },
			{ 73,	5 },
			{ 91,	5 },
			{ 1,	 7 },
			{ 209, 12},
			{ 209, 12},
			{ 173, 7 },
			{ 148, 6 },
			{ 185, 7 },
			{ 79,	7 },
			{ 97,	7 },
			{ 66,	6 },
			{ 197, 7 },
			{ 85,	7 },
			{ 103, 7 },
			{ 68,	6 },
			{ 121, 7 },
			{ 74,	6 },
			{ 92,	6 },
			{ 2,	 7 },
			{ 209, 12},
			{ 157, 6 },
			{ 109, 7 },
			{ 70,	6 },
			{ 127, 7 },
			{ 76,	6 },
			{ 94,	6 },
			{ 4,	 7 },
			{ 193, 6 },
			{ 82,	6 },
			{ 100, 6 },
			{ 8,	 7 },
			{ 118, 6 },
			{ 16,	7 },
			{ 32,	7 },
			{ 0,	 6 },
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 145, 3 },
			{ 208, 12},
			{ 156, 8 },
			{ 168, 8 },
			{ 146, 4 },
			{ 180, 8 },
			{ 149, 4 },
			{ 161, 4 },
			{ 64,	4 },
			{ 209, 12},
			{ 159, 8 },
			{ 171, 8 },
			{ 72,	8 },
			{ 183, 8 },
			{ 78,	8 },
			{ 96,	8 },
			{ 65,	5 },
			{ 195, 8 },
			{ 84,	8 },
			{ 102, 8 },
			{ 67,	5 },
			{ 120, 8 },
			{ 73,	5 },
			{ 91,	5 },
			{ 64,	4 },
			{ 209, 12},
			{ 209, 12},
			{ 174, 8 },
			{ 148, 6 },
			{ 186, 8 },
			{ 80,	8 },
			{ 98,	8 },
			{ 66,	6 },
			{ 198, 8 },
			{ 86,	8 },
			{ 104, 8 },
			{ 68,	6 },
			{ 122, 8 },
			{ 74,	6 },
			{ 92,	6 },
			{ 3,	 8 },
			{ 209, 12},
			{ 157, 6 },
			{ 110, 8 },
			{ 70,	6 },
			{ 128, 8 },
			{ 76,	6 },
			{ 94,	6 },
			{ 5,	 8 },
			{ 193, 6 },
			{ 82,	6 },
			{ 100, 6 },
			{ 9,	 8 },
			{ 118, 6 },
			{ 17,	8 },
			{ 33,	8 },
			{ 0,	 6 },
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 189, 8 },
			{ 152, 7 },
			{ 164, 7 },
			{ 145, 3 },
			{ 201, 8 },
			{ 88,	8 },
			{ 106, 8 },
			{ 69,	7 },
			{ 124, 8 },
			{ 75,	7 },
			{ 93,	7 },
			{ 64,	4 },
			{ 209, 12},
			{ 158, 7 },
			{ 112, 8 },
			{ 71,	7 },
			{ 130, 8 },
			{ 77,	7 },
			{ 95,	7 },
			{ 6,	 8 },
			{ 194, 7 },
			{ 83,	7 },
			{ 101, 7 },
			{ 10,	8 },
			{ 119, 7 },
			{ 18,	8 },
			{ 34,	8 },
			{ 1,	 7 },
			{ 209, 12},
			{ 209, 12},
			{ 173, 7 },
			{ 148, 6 },
			{ 136, 8 },
			{ 79,	7 },
			{ 97,	7 },
			{ 66,	6 },
			{ 197, 7 },
			{ 85,	7 },
			{ 103, 7 },
			{ 12,	8 },
			{ 121, 7 },
			{ 20,	8 },
			{ 36,	8 },
			{ 2,	 7 },
			{ 209, 12},
			{ 157, 6 },
			{ 109, 7 },
			{ 70,	6 },
			{ 127, 7 },
			{ 24,	8 },
			{ 40,	8 },
			{ 4,	 7 },
			{ 193, 6 },
			{ 82,	6 },
			{ 48,	8 },
			{ 8,	 7 },
			{ 118, 6 },
			{ 16,	7 },
			{ 32,	7 },
			{ 0,	 6 },
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 145, 3 },
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 146, 4 },
			{ 209, 12},
			{ 149, 4 },
			{ 161, 4 },
			{ 64,	4 },
			{ 209, 12},
			{ 160, 9 },
			{ 172, 9 },
			{ 147, 5 },
			{ 184, 9 },
			{ 150, 5 },
			{ 162, 5 },
			{ 65,	5 },
			{ 196, 9 },
			{ 153, 5 },
			{ 165, 5 },
			{ 67,	5 },
			{ 177, 5 },
			{ 73,	5 },
			{ 91,	5 },
			{ 64,	4 },
			{ 209, 12},
			{ 209, 12},
			{ 175, 9 },
			{ 148, 6 },
			{ 144, 12},
			{ 81,	9 },
			{ 99,	9 },
			{ 66,	6 },
			{ 199, 9 },
			{ 87,	9 },
			{ 105, 9 },
			{ 68,	6 },
			{ 123, 9 },
			{ 74,	6 },
			{ 92,	6 },
			{ 64,	4 },
			{ 209, 12},
			{ 157, 6 },
			{ 111, 9 },
			{ 70,	6 },
			{ 129, 9 },
			{ 76,	6 },
			{ 94,	6 },
			{ 65,	5 },
			{ 193, 6 },
			{ 82,	6 },
			{ 100, 6 },
			{ 67,	5 },
			{ 118, 6 },
			{ 73,	5 },
			{ 91,	5 },
			{ 0,	 6 },
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 190, 9 },
			{ 152, 7 },
			{ 164, 7 },
			{ 145, 3 },
			{ 202, 9 },
			{ 89,	9 },
			{ 107, 9 },
			{ 69,	7 },
			{ 125, 9 },
			{ 75,	7 },
			{ 93,	7 },
			{ 64,	4 },
			{ 209, 12},
			{ 158, 7 },
			{ 113, 9 },
			{ 71,	7 },
			{ 131, 9 },
			{ 77,	7 },
			{ 95,	7 },
			{ 7,	 9 },
			{ 194, 7 },
			{ 83,	7 },
			{ 101, 7 },
			{ 11,	9 },
			{ 119, 7 },
			{ 19,	9 },
			{ 35,	9 },
			{ 1,	 7 },
			{ 209, 12},
			{ 209, 12},
			{ 173, 7 },
			{ 148, 6 },
			{ 137, 9 },
			{ 79,	7 },
			{ 97,	7 },
			{ 66,	6 },
			{ 197, 7 },
			{ 85,	7 },
			{ 103, 7 },
			{ 13,	9 },
			{ 121, 7 },
			{ 21,	9 },
			{ 37,	9 },
			{ 2,	 7 },
			{ 209, 12},
			{ 157, 6 },
			{ 109, 7 },
			{ 70,	6 },
			{ 127, 7 },
			{ 25,	9 },
			{ 41,	9 },
			{ 4,	 7 },
			{ 193, 6 },
			{ 82,	6 },
			{ 49,	9 },
			{ 8,	 7 },
			{ 118, 6 },
			{ 16,	7 },
			{ 32,	7 },
			{ 0,	 6 },
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 145, 3 },
			{ 205, 9 },
			{ 156, 8 },
			{ 168, 8 },
			{ 146, 4 },
			{ 180, 8 },
			{ 149, 4 },
			{ 161, 4 },
			{ 64,	4 },
			{ 209, 12},
			{ 159, 8 },
			{ 115, 9 },
			{ 72,	8 },
			{ 133, 9 },
			{ 78,	8 },
			{ 96,	8 },
			{ 65,	5 },
			{ 195, 8 },
			{ 84,	8 },
			{ 102, 8 },
			{ 67,	5 },
			{ 120, 8 },
			{ 73,	5 },
			{ 91,	5 },
			{ 64,	4 },
			{ 209, 12},
			{ 209, 12},
			{ 174, 8 },
			{ 148, 6 },
			{ 139, 9 },
			{ 80,	8 },
			{ 98,	8 },
			{ 66,	6 },
			{ 198, 8 },
			{ 86,	8 },
			{ 104, 8 },
			{ 14,	9 },
			{ 122, 8 },
			{ 22,	9 },
			{ 38,	9 },
			{ 3,	 8 },
			{ 209, 12},
			{ 157, 6 },
			{ 110, 8 },
			{ 70,	6 },
			{ 128, 8 },
			{ 26,	9 },
			{ 42,	9 },
			{ 5,	 8 },
			{ 193, 6 },
			{ 82,	6 },
			{ 50,	9 },
			{ 9,	 8 },
			{ 118, 6 },
			{ 17,	8 },
			{ 33,	8 },
			{ 0,	 6 },
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 189, 8 },
			{ 152, 7 },
			{ 164, 7 },
			{ 145, 3 },
			{ 201, 8 },
			{ 88,	8 },
			{ 106, 8 },
			{ 69,	7 },
			{ 124, 8 },
			{ 75,	7 },
			{ 93,	7 },
			{ 64,	4 },
			{ 209, 12},
			{ 158, 7 },
			{ 112, 8 },
			{ 71,	7 },
			{ 130, 8 },
			{ 28,	9 },
			{ 44,	9 },
			{ 6,	 8 },
			{ 194, 7 },
			{ 83,	7 },
			{ 52,	9 },
			{ 10,	8 },
			{ 119, 7 },
			{ 18,	8 },
			{ 34,	8 },
			{ 1,	 7 },
			{ 209, 12},
			{ 209, 12},
			{ 173, 7 },
			{ 148, 6 },
			{ 136, 8 },
			{ 79,	7 },
			{ 97,	7 },
			{ 66,	6 },
			{ 197, 7 },
			{ 85,	7 },
			{ 56,	9 },
			{ 12,	8 },
			{ 121, 7 },
			{ 20,	8 },
			{ 36,	8 },
			{ 2,	 7 },
			{ 209, 12},
			{ 157, 6 },
			{ 109, 7 },
			{ 70,	6 },
			{ 127, 7 },
			{ 24,	8 },
			{ 40,	8 },
			{ 4,	 7 },
			{ 193, 6 },
			{ 82,	6 },
			{ 48,	8 },
			{ 8,	 7 },
			{ 118, 6 },
			{ 16,	7 },
			{ 32,	7 },
			{ 0,	 6 },
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 145, 3 },
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 146, 4 },
			{ 209, 12},
			{ 149, 4 },
			{ 161, 4 },
			{ 64,	4 },
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 147, 5 },
			{ 209, 12},
			{ 150, 5 },
			{ 162, 5 },
			{ 65,	5 },
			{ 209, 12},
			{ 153, 5 },
			{ 165, 5 },
			{ 67,	5 },
			{ 177, 5 },
			{ 73,	5 },
			{ 91,	5 },
			{ 64,	4 },
			{ 209, 12},
			{ 209, 12},
			{ 176, 10},
			{ 148, 6 },
			{ 188, 10},
			{ 151, 6 },
			{ 163, 6 },
			{ 66,	6 },
			{ 200, 10},
			{ 154, 6 },
			{ 166, 6 },
			{ 68,	6 },
			{ 178, 6 },
			{ 74,	6 },
			{ 92,	6 },
			{ 64,	4 },
			{ 209, 12},
			{ 157, 6 },
			{ 169, 6 },
			{ 70,	6 },
			{ 181, 6 },
			{ 76,	6 },
			{ 94,	6 },
			{ 65,	5 },
			{ 193, 6 },
			{ 82,	6 },
			{ 100, 6 },
			{ 67,	5 },
			{ 118, 6 },
			{ 73,	5 },
			{ 91,	5 },
			{ 0,	 6 },
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 191, 10},
			{ 152, 7 },
			{ 164, 7 },
			{ 145, 3 },
			{ 203, 10},
			{ 90,	10},
			{ 108, 10},
			{ 69,	7 },
			{ 126, 10},
			{ 75,	7 },
			{ 93,	7 },
			{ 64,	4 },
			{ 209, 12},
			{ 158, 7 },
			{ 114, 10},
			{ 71,	7 },
			{ 132, 10},
			{ 77,	7 },
			{ 95,	7 },
			{ 65,	5 },
			{ 194, 7 },
			{ 83,	7 },
			{ 101, 7 },
			{ 67,	5 },
			{ 119, 7 },
			{ 73,	5 },
			{ 91,	5 },
			{ 1,	 7 },
			{ 209, 12},
			{ 209, 12},
			{ 173, 7 },
			{ 148, 6 },
			{ 138, 10},
			{ 79,	7 },
			{ 97,	7 },
			{ 66,	6 },
			{ 197, 7 },
			{ 85,	7 },
			{ 103, 7 },
			{ 68,	6 },
			{ 121, 7 },
			{ 74,	6 },
			{ 92,	6 },
			{ 2,	 7 },
			{ 209, 12},
			{ 157, 6 },
			{ 109, 7 },
			{ 70,	6 },
			{ 127, 7 },
			{ 76,	6 },
			{ 94,	6 },
			{ 4,	 7 },
			{ 193, 6 },
			{ 82,	6 },
			{ 100, 6 },
			{ 8,	 7 },
			{ 118, 6 },
			{ 16,	7 },
			{ 32,	7 },
			{ 0,	 6 },
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 145, 3 },
			{ 206, 10},
			{ 156, 8 },
			{ 168, 8 },
			{ 146, 4 },
			{ 180, 8 },
			{ 149, 4 },
			{ 161, 4 },
			{ 64,	4 },
			{ 209, 12},
			{ 159, 8 },
			{ 116, 10},
			{ 72,	8 },
			{ 134, 10},
			{ 78,	8 },
			{ 96,	8 },
			{ 65,	5 },
			{ 195, 8 },
			{ 84,	8 },
			{ 102, 8 },
			{ 67,	5 },
			{ 120, 8 },
			{ 73,	5 },
			{ 91,	5 },
			{ 64,	4 },
			{ 209, 12},
			{ 209, 12},
			{ 174, 8 },
			{ 148, 6 },
			{ 140, 10},
			{ 80,	8 },
			{ 98,	8 },
			{ 66,	6 },
			{ 198, 8 },
			{ 86,	8 },
			{ 63,	12},
			{ 15,	10},
			{ 122, 8 },
			{ 23,	10},
			{ 39,	10},
			{ 3,	 8 },
			{ 209, 12},
			{ 157, 6 },
			{ 110, 8 },
			{ 70,	6 },
			{ 128, 8 },
			{ 27,	10},
			{ 43,	10},
			{ 5,	 8 },
			{ 193, 6 },
			{ 82,	6 },
			{ 51,	10},
			{ 9,	 8 },
			{ 118, 6 },
			{ 17,	8 },
			{ 33,	8 },
			{ 0,	 6 },
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 189, 8 },
			{ 152, 7 },
			{ 164, 7 },
			{ 145, 3 },
			{ 201, 8 },
			{ 88,	8 },
			{ 106, 8 },
			{ 69,	7 },
			{ 124, 8 },
			{ 75,	7 },
			{ 93,	7 },
			{ 64,	4 },
			{ 209, 12},
			{ 158, 7 },
			{ 112, 8 },
			{ 71,	7 },
			{ 130, 8 },
			{ 29,	10},
			{ 45,	10},
			{ 6,	 8 },
			{ 194, 7 },
			{ 83,	7 },
			{ 53,	10},
			{ 10,	8 },
			{ 119, 7 },
			{ 18,	8 },
			{ 34,	8 },
			{ 1,	 7 },
			{ 209, 12},
			{ 209, 12},
			{ 173, 7 },
			{ 148, 6 },
			{ 136, 8 },
			{ 79,	7 },
			{ 97,	7 },
			{ 66,	6 },
			{ 197, 7 },
			{ 85,	7 },
			{ 57,	10},
			{ 12,	8 },
			{ 121, 7 },
			{ 20,	8 },
			{ 36,	8 },
			{ 2,	 7 },
			{ 209, 12},
			{ 157, 6 },
			{ 109, 7 },
			{ 70,	6 },
			{ 127, 7 },
			{ 24,	8 },
			{ 40,	8 },
			{ 4,	 7 },
			{ 193, 6 },
			{ 82,	6 },
			{ 48,	8 },
			{ 8,	 7 },
			{ 118, 6 },
			{ 16,	7 },
			{ 32,	7 },
			{ 0,	 6 },
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 145, 3 },
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 146, 4 },
			{ 209, 12},
			{ 149, 4 },
			{ 161, 4 },
			{ 64,	4 },
			{ 209, 12},
			{ 160, 9 },
			{ 172, 9 },
			{ 147, 5 },
			{ 184, 9 },
			{ 150, 5 },
			{ 162, 5 },
			{ 65,	5 },
			{ 196, 9 },
			{ 153, 5 },
			{ 165, 5 },
			{ 67,	5 },
			{ 177, 5 },
			{ 73,	5 },
			{ 91,	5 },
			{ 64,	4 },
			{ 209, 12},
			{ 209, 12},
			{ 175, 9 },
			{ 148, 6 },
			{ 142, 10},
			{ 81,	9 },
			{ 99,	9 },
			{ 66,	6 },
			{ 199, 9 },
			{ 87,	9 },
			{ 105, 9 },
			{ 68,	6 },
			{ 123, 9 },
			{ 74,	6 },
			{ 92,	6 },
			{ 64,	4 },
			{ 209, 12},
			{ 157, 6 },
			{ 111, 9 },
			{ 70,	6 },
			{ 129, 9 },
			{ 76,	6 },
			{ 94,	6 },
			{ 65,	5 },
			{ 193, 6 },
			{ 82,	6 },
			{ 100, 6 },
			{ 67,	5 },
			{ 118, 6 },
			{ 73,	5 },
			{ 91,	5 },
			{ 0,	 6 },
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 190, 9 },
			{ 152, 7 },
			{ 164, 7 },
			{ 145, 3 },
			{ 202, 9 },
			{ 89,	9 },
			{ 107, 9 },
			{ 69,	7 },
			{ 125, 9 },
			{ 75,	7 },
			{ 93,	7 },
			{ 64,	4 },
			{ 209, 12},
			{ 158, 7 },
			{ 113, 9 },
			{ 71,	7 },
			{ 131, 9 },
			{ 30,	10},
			{ 46,	10},
			{ 7,	 9 },
			{ 194, 7 },
			{ 83,	7 },
			{ 54,	10},
			{ 11,	9 },
			{ 119, 7 },
			{ 19,	9 },
			{ 35,	9 },
			{ 1,	 7 },
			{ 209, 12},
			{ 209, 12},
			{ 173, 7 },
			{ 148, 6 },
			{ 137, 9 },
			{ 79,	7 },
			{ 97,	7 },
			{ 66,	6 },
			{ 197, 7 },
			{ 85,	7 },
			{ 58,	10},
			{ 13,	9 },
			{ 121, 7 },
			{ 21,	9 },
			{ 37,	9 },
			{ 2,	 7 },
			{ 209, 12},
			{ 157, 6 },
			{ 109, 7 },
			{ 70,	6 },
			{ 127, 7 },
			{ 25,	9 },
			{ 41,	9 },
			{ 4,	 7 },
			{ 193, 6 },
			{ 82,	6 },
			{ 49,	9 },
			{ 8,	 7 },
			{ 118, 6 },
			{ 16,	7 },
			{ 32,	7 },
			{ 0,	 6 },
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 145, 3 },
			{ 205, 9 },
			{ 156, 8 },
			{ 168, 8 },
			{ 146, 4 },
			{ 180, 8 },
			{ 149, 4 },
			{ 161, 4 },
			{ 64,	4 },
			{ 209, 12},
			{ 159, 8 },
			{ 115, 9 },
			{ 72,	8 },
			{ 133, 9 },
			{ 78,	8 },
			{ 96,	8 },
			{ 65,	5 },
			{ 195, 8 },
			{ 84,	8 },
			{ 102, 8 },
			{ 67,	5 },
			{ 120, 8 },
			{ 73,	5 },
			{ 91,	5 },
			{ 64,	4 },
			{ 209, 12},
			{ 209, 12},
			{ 174, 8 },
			{ 148, 6 },
			{ 139, 9 },
			{ 80,	8 },
			{ 98,	8 },
			{ 66,	6 },
			{ 198, 8 },
			{ 86,	8 },
			{ 60,	10},
			{ 14,	9 },
			{ 122, 8 },
			{ 22,	9 },
			{ 38,	9 },
			{ 3,	 8 },
			{ 209, 12},
			{ 157, 6 },
			{ 110, 8 },
			{ 70,	6 },
			{ 128, 8 },
			{ 26,	9 },
			{ 42,	9 },
			{ 5,	 8 },
			{ 193, 6 },
			{ 82,	6 },
			{ 50,	9 },
			{ 9,	 8 },
			{ 118, 6 },
			{ 17,	8 },
			{ 33,	8 },
			{ 0,	 6 },
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 189, 8 },
			{ 152, 7 },
			{ 164, 7 },
			{ 145, 3 },
			{ 201, 8 },
			{ 88,	8 },
			{ 106, 8 },
			{ 69,	7 },
			{ 124, 8 },
			{ 75,	7 },
			{ 93,	7 },
			{ 64,	4 },
			{ 209, 12},
			{ 158, 7 },
			{ 112, 8 },
			{ 71,	7 },
			{ 130, 8 },
			{ 28,	9 },
			{ 44,	9 },
			{ 6,	 8 },
			{ 194, 7 },
			{ 83,	7 },
			{ 52,	9 },
			{ 10,	8 },
			{ 119, 7 },
			{ 18,	8 },
			{ 34,	8 },
			{ 1,	 7 },
			{ 209, 12},
			{ 209, 12},
			{ 173, 7 },
			{ 148, 6 },
			{ 136, 8 },
			{ 79,	7 },
			{ 97,	7 },
			{ 66,	6 },
			{ 197, 7 },
			{ 85,	7 },
			{ 56,	9 },
			{ 12,	8 },
			{ 121, 7 },
			{ 20,	8 },
			{ 36,	8 },
			{ 2,	 7 },
			{ 209, 12},
			{ 157, 6 },
			{ 109, 7 },
			{ 70,	6 },
			{ 127, 7 },
			{ 24,	8 },
			{ 40,	8 },
			{ 4,	 7 },
			{ 193, 6 },
			{ 82,	6 },
			{ 48,	8 },
			{ 8,	 7 },
			{ 118, 6 },
			{ 16,	7 },
			{ 32,	7 },
			{ 0,	 6 },
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 145, 3 },
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 146, 4 },
			{ 209, 12},
			{ 149, 4 },
			{ 161, 4 },
			{ 64,	4 },
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 147, 5 },
			{ 209, 12},
			{ 150, 5 },
			{ 162, 5 },
			{ 65,	5 },
			{ 209, 12},
			{ 153, 5 },
			{ 165, 5 },
			{ 67,	5 },
			{ 177, 5 },
			{ 73,	5 },
			{ 91,	5 },
			{ 64,	4 },
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 148, 6 },
			{ 209, 12},
			{ 151, 6 },
			{ 163, 6 },
			{ 66,	6 },
			{ 209, 12},
			{ 154, 6 },
			{ 166, 6 },
			{ 68,	6 },
			{ 178, 6 },
			{ 74,	6 },
			{ 92,	6 },
			{ 64,	4 },
			{ 209, 12},
			{ 157, 6 },
			{ 169, 6 },
			{ 70,	6 },
			{ 181, 6 },
			{ 76,	6 },
			{ 94,	6 },
			{ 65,	5 },
			{ 193, 6 },
			{ 82,	6 },
			{ 100, 6 },
			{ 67,	5 },
			{ 118, 6 },
			{ 73,	5 },
			{ 91,	5 },
			{ 0,	 6 },
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 192, 11},
			{ 152, 7 },
			{ 164, 7 },
			{ 145, 3 },
			{ 204, 11},
			{ 155, 7 },
			{ 167, 7 },
			{ 69,	7 },
			{ 179, 7 },
			{ 75,	7 },
			{ 93,	7 },
			{ 64,	4 },
			{ 209, 12},
			{ 158, 7 },
			{ 170, 7 },
			{ 71,	7 },
			{ 182, 7 },
			{ 77,	7 },
			{ 95,	7 },
			{ 65,	5 },
			{ 194, 7 },
			{ 83,	7 },
			{ 101, 7 },
			{ 67,	5 },
			{ 119, 7 },
			{ 73,	5 },
			{ 91,	5 },
			{ 1,	 7 },
			{ 209, 12},
			{ 209, 12},
			{ 173, 7 },
			{ 148, 6 },
			{ 185, 7 },
			{ 79,	7 },
			{ 97,	7 },
			{ 66,	6 },
			{ 197, 7 },
			{ 85,	7 },
			{ 103, 7 },
			{ 68,	6 },
			{ 121, 7 },
			{ 74,	6 },
			{ 92,	6 },
			{ 2,	 7 },
			{ 209, 12},
			{ 157, 6 },
			{ 109, 7 },
			{ 70,	6 },
			{ 127, 7 },
			{ 76,	6 },
			{ 94,	6 },
			{ 4,	 7 },
			{ 193, 6 },
			{ 82,	6 },
			{ 100, 6 },
			{ 8,	 7 },
			{ 118, 6 },
			{ 16,	7 },
			{ 32,	7 },
			{ 0,	 6 },
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 145, 3 },
			{ 207, 11},
			{ 156, 8 },
			{ 168, 8 },
			{ 146, 4 },
			{ 180, 8 },
			{ 149, 4 },
			{ 161, 4 },
			{ 64,	4 },
			{ 209, 12},
			{ 159, 8 },
			{ 117, 11},
			{ 72,	8 },
			{ 135, 11},
			{ 78,	8 },
			{ 96,	8 },
			{ 65,	5 },
			{ 195, 8 },
			{ 84,	8 },
			{ 102, 8 },
			{ 67,	5 },
			{ 120, 8 },
			{ 73,	5 },
			{ 91,	5 },
			{ 64,	4 },
			{ 209, 12},
			{ 209, 12},
			{ 174, 8 },
			{ 148, 6 },
			{ 141, 11},
			{ 80,	8 },
			{ 98,	8 },
			{ 66,	6 },
			{ 198, 8 },
			{ 86,	8 },
			{ 104, 8 },
			{ 68,	6 },
			{ 122, 8 },
			{ 74,	6 },
			{ 92,	6 },
			{ 3,	 8 },
			{ 209, 12},
			{ 157, 6 },
			{ 110, 8 },
			{ 70,	6 },
			{ 128, 8 },
			{ 76,	6 },
			{ 94,	6 },
			{ 5,	 8 },
			{ 193, 6 },
			{ 82,	6 },
			{ 100, 6 },
			{ 9,	 8 },
			{ 118, 6 },
			{ 17,	8 },
			{ 33,	8 },
			{ 0,	 6 },
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 189, 8 },
			{ 152, 7 },
			{ 164, 7 },
			{ 145, 3 },
			{ 201, 8 },
			{ 88,	8 },
			{ 106, 8 },
			{ 69,	7 },
			{ 124, 8 },
			{ 75,	7 },
			{ 93,	7 },
			{ 64,	4 },
			{ 209, 12},
			{ 158, 7 },
			{ 112, 8 },
			{ 71,	7 },
			{ 130, 8 },
			{ 77,	7 },
			{ 95,	7 },
			{ 6,	 8 },
			{ 194, 7 },
			{ 83,	7 },
			{ 101, 7 },
			{ 10,	8 },
			{ 119, 7 },
			{ 18,	8 },
			{ 34,	8 },
			{ 1,	 7 },
			{ 209, 12},
			{ 209, 12},
			{ 173, 7 },
			{ 148, 6 },
			{ 136, 8 },
			{ 79,	7 },
			{ 97,	7 },
			{ 66,	6 },
			{ 197, 7 },
			{ 85,	7 },
			{ 103, 7 },
			{ 12,	8 },
			{ 121, 7 },
			{ 20,	8 },
			{ 36,	8 },
			{ 2,	 7 },
			{ 209, 12},
			{ 157, 6 },
			{ 109, 7 },
			{ 70,	6 },
			{ 127, 7 },
			{ 24,	8 },
			{ 40,	8 },
			{ 4,	 7 },
			{ 193, 6 },
			{ 82,	6 },
			{ 48,	8 },
			{ 8,	 7 },
			{ 118, 6 },
			{ 16,	7 },
			{ 32,	7 },
			{ 0,	 6 },
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 145, 3 },
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 146, 4 },
			{ 209, 12},
			{ 149, 4 },
			{ 161, 4 },
			{ 64,	4 },
			{ 209, 12},
			{ 160, 9 },
			{ 172, 9 },
			{ 147, 5 },
			{ 184, 9 },
			{ 150, 5 },
			{ 162, 5 },
			{ 65,	5 },
			{ 196, 9 },
			{ 153, 5 },
			{ 165, 5 },
			{ 67,	5 },
			{ 177, 5 },
			{ 73,	5 },
			{ 91,	5 },
			{ 64,	4 },
			{ 209, 12},
			{ 209, 12},
			{ 175, 9 },
			{ 148, 6 },
			{ 143, 11},
			{ 81,	9 },
			{ 99,	9 },
			{ 66,	6 },
			{ 199, 9 },
			{ 87,	9 },
			{ 105, 9 },
			{ 68,	6 },
			{ 123, 9 },
			{ 74,	6 },
			{ 92,	6 },
			{ 64,	4 },
			{ 209, 12},
			{ 157, 6 },
			{ 111, 9 },
			{ 70,	6 },
			{ 129, 9 },
			{ 76,	6 },
			{ 94,	6 },
			{ 65,	5 },
			{ 193, 6 },
			{ 82,	6 },
			{ 100, 6 },
			{ 67,	5 },
			{ 118, 6 },
			{ 73,	5 },
			{ 91,	5 },
			{ 0,	 6 },
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 190, 9 },
			{ 152, 7 },
			{ 164, 7 },
			{ 145, 3 },
			{ 202, 9 },
			{ 89,	9 },
			{ 107, 9 },
			{ 69,	7 },
			{ 125, 9 },
			{ 75,	7 },
			{ 93,	7 },
			{ 64,	4 },
			{ 209, 12},
			{ 158, 7 },
			{ 113, 9 },
			{ 71,	7 },
			{ 131, 9 },
			{ 31,	11},
			{ 47,	11},
			{ 7,	 9 },
			{ 194, 7 },
			{ 83,	7 },
			{ 55,	11},
			{ 11,	9 },
			{ 119, 7 },
			{ 19,	9 },
			{ 35,	9 },
			{ 1,	 7 },
			{ 209, 12},
			{ 209, 12},
			{ 173, 7 },
			{ 148, 6 },
			{ 137, 9 },
			{ 79,	7 },
			{ 97,	7 },
			{ 66,	6 },
			{ 197, 7 },
			{ 85,	7 },
			{ 59,	11},
			{ 13,	9 },
			{ 121, 7 },
			{ 21,	9 },
			{ 37,	9 },
			{ 2,	 7 },
			{ 209, 12},
			{ 157, 6 },
			{ 109, 7 },
			{ 70,	6 },
			{ 127, 7 },
			{ 25,	9 },
			{ 41,	9 },
			{ 4,	 7 },
			{ 193, 6 },
			{ 82,	6 },
			{ 49,	9 },
			{ 8,	 7 },
			{ 118, 6 },
			{ 16,	7 },
			{ 32,	7 },
			{ 0,	 6 },
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 145, 3 },
			{ 205, 9 },
			{ 156, 8 },
			{ 168, 8 },
			{ 146, 4 },
			{ 180, 8 },
			{ 149, 4 },
			{ 161, 4 },
			{ 64,	4 },
			{ 209, 12},
			{ 159, 8 },
			{ 115, 9 },
			{ 72,	8 },
			{ 133, 9 },
			{ 78,	8 },
			{ 96,	8 },
			{ 65,	5 },
			{ 195, 8 },
			{ 84,	8 },
			{ 102, 8 },
			{ 67,	5 },
			{ 120, 8 },
			{ 73,	5 },
			{ 91,	5 },
			{ 64,	4 },
			{ 209, 12},
			{ 209, 12},
			{ 174, 8 },
			{ 148, 6 },
			{ 139, 9 },
			{ 80,	8 },
			{ 98,	8 },
			{ 66,	6 },
			{ 198, 8 },
			{ 86,	8 },
			{ 61,	11},
			{ 14,	9 },
			{ 122, 8 },
			{ 22,	9 },
			{ 38,	9 },
			{ 3,	 8 },
			{ 209, 12},
			{ 157, 6 },
			{ 110, 8 },
			{ 70,	6 },
			{ 128, 8 },
			{ 26,	9 },
			{ 42,	9 },
			{ 5,	 8 },
			{ 193, 6 },
			{ 82,	6 },
			{ 50,	9 },
			{ 9,	 8 },
			{ 118, 6 },
			{ 17,	8 },
			{ 33,	8 },
			{ 0,	 6 },
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 189, 8 },
			{ 152, 7 },
			{ 164, 7 },
			{ 145, 3 },
			{ 201, 8 },
			{ 88,	8 },
			{ 106, 8 },
			{ 69,	7 },
			{ 124, 8 },
			{ 75,	7 },
			{ 93,	7 },
			{ 64,	4 },
			{ 209, 12},
			{ 158, 7 },
			{ 112, 8 },
			{ 71,	7 },
			{ 130, 8 },
			{ 28,	9 },
			{ 44,	9 },
			{ 6,	 8 },
			{ 194, 7 },
			{ 83,	7 },
			{ 52,	9 },
			{ 10,	8 },
			{ 119, 7 },
			{ 18,	8 },
			{ 34,	8 },
			{ 1,	 7 },
			{ 209, 12},
			{ 209, 12},
			{ 173, 7 },
			{ 148, 6 },
			{ 136, 8 },
			{ 79,	7 },
			{ 97,	7 },
			{ 66,	6 },
			{ 197, 7 },
			{ 85,	7 },
			{ 56,	9 },
			{ 12,	8 },
			{ 121, 7 },
			{ 20,	8 },
			{ 36,	8 },
			{ 2,	 7 },
			{ 209, 12},
			{ 157, 6 },
			{ 109, 7 },
			{ 70,	6 },
			{ 127, 7 },
			{ 24,	8 },
			{ 40,	8 },
			{ 4,	 7 },
			{ 193, 6 },
			{ 82,	6 },
			{ 48,	8 },
			{ 8,	 7 },
			{ 118, 6 },
			{ 16,	7 },
			{ 32,	7 },
			{ 0,	 6 },
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 145, 3 },
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 146, 4 },
			{ 209, 12},
			{ 149, 4 },
			{ 161, 4 },
			{ 64,	4 },
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 147, 5 },
			{ 209, 12},
			{ 150, 5 },
			{ 162, 5 },
			{ 65,	5 },
			{ 209, 12},
			{ 153, 5 },
			{ 165, 5 },
			{ 67,	5 },
			{ 177, 5 },
			{ 73,	5 },
			{ 91,	5 },
			{ 64,	4 },
			{ 209, 12},
			{ 209, 12},
			{ 176, 10},
			{ 148, 6 },
			{ 188, 10},
			{ 151, 6 },
			{ 163, 6 },
			{ 66,	6 },
			{ 200, 10},
			{ 154, 6 },
			{ 166, 6 },
			{ 68,	6 },
			{ 178, 6 },
			{ 74,	6 },
			{ 92,	6 },
			{ 64,	4 },
			{ 209, 12},
			{ 157, 6 },
			{ 169, 6 },
			{ 70,	6 },
			{ 181, 6 },
			{ 76,	6 },
			{ 94,	6 },
			{ 65,	5 },
			{ 193, 6 },
			{ 82,	6 },
			{ 100, 6 },
			{ 67,	5 },
			{ 118, 6 },
			{ 73,	5 },
			{ 91,	5 },
			{ 0,	 6 },
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 191, 10},
			{ 152, 7 },
			{ 164, 7 },
			{ 145, 3 },
			{ 203, 10},
			{ 90,	10},
			{ 108, 10},
			{ 69,	7 },
			{ 126, 10},
			{ 75,	7 },
			{ 93,	7 },
			{ 64,	4 },
			{ 209, 12},
			{ 158, 7 },
			{ 114, 10},
			{ 71,	7 },
			{ 132, 10},
			{ 77,	7 },
			{ 95,	7 },
			{ 65,	5 },
			{ 194, 7 },
			{ 83,	7 },
			{ 101, 7 },
			{ 67,	5 },
			{ 119, 7 },
			{ 73,	5 },
			{ 91,	5 },
			{ 1,	 7 },
			{ 209, 12},
			{ 209, 12},
			{ 173, 7 },
			{ 148, 6 },
			{ 138, 10},
			{ 79,	7 },
			{ 97,	7 },
			{ 66,	6 },
			{ 197, 7 },
			{ 85,	7 },
			{ 103, 7 },
			{ 68,	6 },
			{ 121, 7 },
			{ 74,	6 },
			{ 92,	6 },
			{ 2,	 7 },
			{ 209, 12},
			{ 157, 6 },
			{ 109, 7 },
			{ 70,	6 },
			{ 127, 7 },
			{ 76,	6 },
			{ 94,	6 },
			{ 4,	 7 },
			{ 193, 6 },
			{ 82,	6 },
			{ 100, 6 },
			{ 8,	 7 },
			{ 118, 6 },
			{ 16,	7 },
			{ 32,	7 },
			{ 0,	 6 },
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 145, 3 },
			{ 206, 10},
			{ 156, 8 },
			{ 168, 8 },
			{ 146, 4 },
			{ 180, 8 },
			{ 149, 4 },
			{ 161, 4 },
			{ 64,	4 },
			{ 209, 12},
			{ 159, 8 },
			{ 116, 10},
			{ 72,	8 },
			{ 134, 10},
			{ 78,	8 },
			{ 96,	8 },
			{ 65,	5 },
			{ 195, 8 },
			{ 84,	8 },
			{ 102, 8 },
			{ 67,	5 },
			{ 120, 8 },
			{ 73,	5 },
			{ 91,	5 },
			{ 64,	4 },
			{ 209, 12},
			{ 209, 12},
			{ 174, 8 },
			{ 148, 6 },
			{ 140, 10},
			{ 80,	8 },
			{ 98,	8 },
			{ 66,	6 },
			{ 198, 8 },
			{ 86,	8 },
			{ 62,	11},
			{ 15,	10},
			{ 122, 8 },
			{ 23,	10},
			{ 39,	10},
			{ 3,	 8 },
			{ 209, 12},
			{ 157, 6 },
			{ 110, 8 },
			{ 70,	6 },
			{ 128, 8 },
			{ 27,	10},
			{ 43,	10},
			{ 5,	 8 },
			{ 193, 6 },
			{ 82,	6 },
			{ 51,	10},
			{ 9,	 8 },
			{ 118, 6 },
			{ 17,	8 },
			{ 33,	8 },
			{ 0,	 6 },
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 189, 8 },
			{ 152, 7 },
			{ 164, 7 },
			{ 145, 3 },
			{ 201, 8 },
			{ 88,	8 },
			{ 106, 8 },
			{ 69,	7 },
			{ 124, 8 },
			{ 75,	7 },
			{ 93,	7 },
			{ 64,	4 },
			{ 209, 12},
			{ 158, 7 },
			{ 112, 8 },
			{ 71,	7 },
			{ 130, 8 },
			{ 29,	10},
			{ 45,	10},
			{ 6,	 8 },
			{ 194, 7 },
			{ 83,	7 },
			{ 53,	10},
			{ 10,	8 },
			{ 119, 7 },
			{ 18,	8 },
			{ 34,	8 },
			{ 1,	 7 },
			{ 209, 12},
			{ 209, 12},
			{ 173, 7 },
			{ 148, 6 },
			{ 136, 8 },
			{ 79,	7 },
			{ 97,	7 },
			{ 66,	6 },
			{ 197, 7 },
			{ 85,	7 },
			{ 57,	10},
			{ 12,	8 },
			{ 121, 7 },
			{ 20,	8 },
			{ 36,	8 },
			{ 2,	 7 },
			{ 209, 12},
			{ 157, 6 },
			{ 109, 7 },
			{ 70,	6 },
			{ 127, 7 },
			{ 24,	8 },
			{ 40,	8 },
			{ 4,	 7 },
			{ 193, 6 },
			{ 82,	6 },
			{ 48,	8 },
			{ 8,	 7 },
			{ 118, 6 },
			{ 16,	7 },
			{ 32,	7 },
			{ 0,	 6 },
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 145, 3 },
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 146, 4 },
			{ 209, 12},
			{ 149, 4 },
			{ 161, 4 },
			{ 64,	4 },
			{ 209, 12},
			{ 160, 9 },
			{ 172, 9 },
			{ 147, 5 },
			{ 184, 9 },
			{ 150, 5 },
			{ 162, 5 },
			{ 65,	5 },
			{ 196, 9 },
			{ 153, 5 },
			{ 165, 5 },
			{ 67,	5 },
			{ 177, 5 },
			{ 73,	5 },
			{ 91,	5 },
			{ 64,	4 },
			{ 209, 12},
			{ 209, 12},
			{ 175, 9 },
			{ 148, 6 },
			{ 142, 10},
			{ 81,	9 },
			{ 99,	9 },
			{ 66,	6 },
			{ 199, 9 },
			{ 87,	9 },
			{ 105, 9 },
			{ 68,	6 },
			{ 123, 9 },
			{ 74,	6 },
			{ 92,	6 },
			{ 64,	4 },
			{ 209, 12},
			{ 157, 6 },
			{ 111, 9 },
			{ 70,	6 },
			{ 129, 9 },
			{ 76,	6 },
			{ 94,	6 },
			{ 65,	5 },
			{ 193, 6 },
			{ 82,	6 },
			{ 100, 6 },
			{ 67,	5 },
			{ 118, 6 },
			{ 73,	5 },
			{ 91,	5 },
			{ 0,	 6 },
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 190, 9 },
			{ 152, 7 },
			{ 164, 7 },
			{ 145, 3 },
			{ 202, 9 },
			{ 89,	9 },
			{ 107, 9 },
			{ 69,	7 },
			{ 125, 9 },
			{ 75,	7 },
			{ 93,	7 },
			{ 64,	4 },
			{ 209, 12},
			{ 158, 7 },
			{ 113, 9 },
			{ 71,	7 },
			{ 131, 9 },
			{ 30,	10},
			{ 46,	10},
			{ 7,	 9 },
			{ 194, 7 },
			{ 83,	7 },
			{ 54,	10},
			{ 11,	9 },
			{ 119, 7 },
			{ 19,	9 },
			{ 35,	9 },
			{ 1,	 7 },
			{ 209, 12},
			{ 209, 12},
			{ 173, 7 },
			{ 148, 6 },
			{ 137, 9 },
			{ 79,	7 },
			{ 97,	7 },
			{ 66,	6 },
			{ 197, 7 },
			{ 85,	7 },
			{ 58,	10},
			{ 13,	9 },
			{ 121, 7 },
			{ 21,	9 },
			{ 37,	9 },
			{ 2,	 7 },
			{ 209, 12},
			{ 157, 6 },
			{ 109, 7 },
			{ 70,	6 },
			{ 127, 7 },
			{ 25,	9 },
			{ 41,	9 },
			{ 4,	 7 },
			{ 193, 6 },
			{ 82,	6 },
			{ 49,	9 },
			{ 8,	 7 },
			{ 118, 6 },
			{ 16,	7 },
			{ 32,	7 },
			{ 0,	 6 },
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 145, 3 },
			{ 205, 9 },
			{ 156, 8 },
			{ 168, 8 },
			{ 146, 4 },
			{ 180, 8 },
			{ 149, 4 },
			{ 161, 4 },
			{ 64,	4 },
			{ 209, 12},
			{ 159, 8 },
			{ 115, 9 },
			{ 72,	8 },
			{ 133, 9 },
			{ 78,	8 },
			{ 96,	8 },
			{ 65,	5 },
			{ 195, 8 },
			{ 84,	8 },
			{ 102, 8 },
			{ 67,	5 },
			{ 120, 8 },
			{ 73,	5 },
			{ 91,	5 },
			{ 64,	4 },
			{ 209, 12},
			{ 209, 12},
			{ 174, 8 },
			{ 148, 6 },
			{ 139, 9 },
			{ 80,	8 },
			{ 98,	8 },
			{ 66,	6 },
			{ 198, 8 },
			{ 86,	8 },
			{ 60,	10},
			{ 14,	9 },
			{ 122, 8 },
			{ 22,	9 },
			{ 38,	9 },
			{ 3,	 8 },
			{ 209, 12},
			{ 157, 6 },
			{ 110, 8 },
			{ 70,	6 },
			{ 128, 8 },
			{ 26,	9 },
			{ 42,	9 },
			{ 5,	 8 },
			{ 193, 6 },
			{ 82,	6 },
			{ 50,	9 },
			{ 9,	 8 },
			{ 118, 6 },
			{ 17,	8 },
			{ 33,	8 },
			{ 0,	 6 },
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 209, 12},
			{ 189, 8 },
			{ 152, 7 },
			{ 164, 7 },
			{ 145, 3 },
			{ 201, 8 },
			{ 88,	8 },
			{ 106, 8 },
			{ 69,	7 },
			{ 124, 8 },
			{ 75,	7 },
			{ 93,	7 },
			{ 64,	4 },
			{ 209, 12},
			{ 158, 7 },
			{ 112, 8 },
			{ 71,	7 },
			{ 130, 8 },
			{ 28,	9 },
			{ 44,	9 },
			{ 6,	 8 },
			{ 194, 7 },
			{ 83,	7 },
			{ 52,	9 },
			{ 10,	8 },
			{ 119, 7 },
			{ 18,	8 },
			{ 34,	8 },
			{ 1,	 7 },
			{ 209, 12},
			{ 209, 12},
			{ 173, 7 },
			{ 148, 6 },
			{ 136, 8 },
			{ 79,	7 },
			{ 97,	7 },
			{ 66,	6 },
			{ 197, 7 },
			{ 85,	7 },
			{ 56,	9 },
			{ 12,	8 },
			{ 121, 7 },
			{ 20,	8 },
			{ 36,	8 },
			{ 2,	 7 },
			{ 209, 12},
			{ 157, 6 },
			{ 109, 7 },
			{ 70,	6 },
			{ 127, 7 },
			{ 24,	8 },
			{ 40,	8 },
			{ 4,	 7 },
			{ 193, 6 },
			{ 82,	6 },
			{ 48,	8 },
			{ 8,	 7 },
			{ 118, 6 },
			{ 16,	7 },
			{ 32,	7 },
			{ 0,	 6 }
		};

		pf_hint_nodiscard pf_decl_inline size_t
		__convert_masked_utf8_to_utf16le_avx2(
		 const u8char_t *__in,
		 int32_t __end_of_code_point_mask,
		 u16char_t *&__out) pf_attr_noexcept
		{
			// we use an approach where we try to process up to 12 input bytes.
			// Why 12 input bytes and not 16? Because we are concerned with the size of
			// the lookup tables. Also 12 is nicely divisible by two and three.
			//
			//
			// Optimization note: our main path below is load-latency dependent. Thus it is maybe
			// beneficial to have fast paths that depend on branch prediction but have less latency.
			// This results in more instructions but, potentially, also higher speeds.
			//
			// We first try a few fast paths.
			const m128i64_t in = _mm_loadu_si128(union_cast<m128i64_t *>(__in));
			const uint16_t input_utf8_end_of_code_point_mask =
			 __end_of_code_point_mask & 0xf'ff;
			if(((__end_of_code_point_mask & 0xff'ff) == 0xff'ff))
			{
				// We process the data in chunks of 16 bytes.
				m256i64_t asciidata = _mm256_cvtepu8_epi16(in);
				_mm256_storeu_si256(union_cast<m256i64_t *>(__out), asciidata);
				__out += 16;	// We wrote 16 16-bit characters.
				return 16;		// We consumed 16 bytes.
			}
			if(((__end_of_code_point_mask & 0xff'ff) == 0xaa'aa))
			{
				// We want to take 8 2-byte UTF-8 ws and turn them into 8 2-byte UTF-16 ws.
				// There is probably a more efficient sequence, but the following might do.
				const m128i64_t sh				= _mm_setr_epi8(1, 0, 3, 2, 5, 4, 7, 6, 9, 8, 11, 10, 13, 12, 15, 14);
				const m128i64_t perm			= _mm_shuffle_epi8(in, sh);
				const m128i64_t asciidata = _mm_and_si128(perm, _mm_set1_epi16(0x7f));
				const m128i64_t highbyte	= _mm_and_si128(perm, _mm_set1_epi16(0x1f'00));
				m128i64_t composed				= _mm_or_si128(asciidata, _mm_srli_epi16(highbyte, 2));
				_mm_storeu_si128(union_cast<m128i64_t *>(__out), composed);
				__out += 8;	 // We wrote 16 bytes, 8 code points.
				return 16;
			}
			if(input_utf8_end_of_code_point_mask == 0x9'24)
			{
				// We want to take 4 3-byte UTF-8 ws and turn them into 4 2-byte UTF-16 ws.
				// There is probably a more efficient sequence, but the following might do.
				const m128i64_t sh	 = _mm_setr_epi8(2, 1, 0, -1, 5, 4, 3, -1, 8, 7, 6, -1, 11, 10, 9, -1);
				const m128i64_t perm = _mm_shuffle_epi8(in, sh);
				const m128i64_t asciidata =
				 _mm_and_si128(perm, _mm_set1_epi32(0x7f));			// 7 or 6 bits
				const m128i64_t middlebyte =
				 _mm_and_si128(perm, _mm_set1_epi32(0x3f'00));	// 5 or 6 bits
				const m128i64_t middlebyte_shifted = _mm_srli_epi32(middlebyte, 2);
				const m128i64_t highbyte =
				 _mm_and_si128(perm, _mm_set1_epi32(0x0f'00'00));	 // 4 bits
				const m128i64_t highbyte_shifted = _mm_srli_epi32(highbyte, 4);
				const m128i64_t composed =
				 _mm_or_si128(_mm_or_si128(asciidata, middlebyte_shifted), highbyte_shifted);
				m128i64_t composed_repacked = _mm_packus_epi32(composed, composed);
				_mm_storeu_si128(union_cast<m128i64_t *>(__out), composed_repacked);
				__out += 4;
				return 12;
			}

			const uint8_t idx			 = __big_index_utf8_to_utf_16[input_utf8_end_of_code_point_mask][0];
			const uint8_t consumed = __big_index_utf8_to_utf_16[input_utf8_end_of_code_point_mask][1];
			if(idx < 64)
			{
				// SIX (6) input code-ws
				// this is a relatively easy scenario
				// we process SIX (6) input code-ws. The max length in bytes of six code
				// ws spanning between 1 and 2 bytes each is 12 bytes. On processors
				// where pdep/pext is fast, we might be able to use a small lookup table.
				const m128i64_t sh =
				 _mm_loadu_si128(union_cast<const m128i64_t *>(__shut_utf8_to_utf16[idx]));
				const m128i64_t perm			= _mm_shuffle_epi8(in, sh);
				const m128i64_t asciidata = _mm_and_si128(perm, _mm_set1_epi16(0x7f));
				const m128i64_t highbyte	= _mm_and_si128(perm, _mm_set1_epi16(0x1f'00));
				m128i64_t composed				= _mm_or_si128(asciidata, _mm_srli_epi16(highbyte, 2));
				_mm_storeu_si128(union_cast<m128i64_t *>(__out), composed);
				__out += 6;	 // We wrote 12 bytes, 6 code points. There is a potential overflow of 4 bytes.
			}
			else if(idx < 145)
			{
				// FOUR (4) input code-ws
				const m128i64_t sh =
				 _mm_loadu_si128(union_cast<const m128i64_t *>(__shut_utf8_to_utf16[idx]));
				const m128i64_t perm = _mm_shuffle_epi8(in, sh);
				const m128i64_t asciidata =
				 _mm_and_si128(perm, _mm_set1_epi32(0x7f));			// 7 or 6 bits
				const m128i64_t middlebyte =
				 _mm_and_si128(perm, _mm_set1_epi32(0x3f'00));	// 5 or 6 bits
				const m128i64_t middlebyte_shifted = _mm_srli_epi32(middlebyte, 2);
				const m128i64_t highbyte =
				 _mm_and_si128(perm, _mm_set1_epi32(0x0f'00'00));	 // 4 bits
				const m128i64_t highbyte_shifted = _mm_srli_epi32(highbyte, 4);
				const m128i64_t composed =
				 _mm_or_si128(_mm_or_si128(asciidata, middlebyte_shifted), highbyte_shifted);
				m128i64_t composed_repacked = _mm_packus_epi32(composed, composed);
				_mm_storeu_si128(union_cast<m128i64_t *>(__out), composed_repacked);
				__out += 4;	 // Here we overflow by 8 bytes.
			}
			else if(idx < 209)
			{
				// TWO (2) input code-ws
				//////////////
				// There might be garbage inputs where a leading byte mascarades as a four-byte
				// leading byte (by being followed by 3 continuation byte), but is not greater than
				// 0xf0. This could trigger a buffer overflow if we only counted leading
				// bytes of the form 0xf0 as generating surrogate pairs, without further UTF-8 validation.
				// Thus we must be careful to ensure that only leading bytes at least as large as 0xf0 generate surrogate pairs.
				// We do as at the cost of an extra mask.
				/////////////
				const m128i64_t sh =
				 _mm_loadu_si128(union_cast<const m128i64_t *>(__shut_utf8_to_utf16[idx]));
				const m128i64_t perm							 = _mm_shuffle_epi8(in, sh);
				const m128i64_t asciidata					 = _mm_and_si128(perm, _mm_set1_epi32(0x7f));
				const m128i64_t middlebyte				 = _mm_and_si128(perm, _mm_set1_epi32(0x3f'00));
				const m128i64_t middlebyte_shifted = _mm_srli_epi32(middlebyte, 2);
				m128i64_t middlehighbyte					 = _mm_and_si128(perm, _mm_set1_epi32(0x3f'00'00));
				// correct for spurious high bit
				const m128i64_t correct =
				 _mm_srli_epi32(_mm_and_si128(perm, _mm_set1_epi32(0x40'00'00)), 1);
				middlehighbyte												 = _mm_xor_si128(correct, middlehighbyte);
				const m128i64_t middlehighbyte_shifted = _mm_srli_epi32(middlehighbyte, 4);
				// We deliberately carry the leading four bits in highbyte if they are present,
				// we remove them later when computing hightenbits.
				const m128i64_t highbyte							 = _mm_and_si128(perm, _mm_set1_epi32(0xff'00'00'00));
				const m128i64_t highbyte_shifted			 = _mm_srli_epi32(highbyte, 6);
				// When we need to generate a surrogate pair (leading byte > 0xF0), then
				// the corresponding 32-bit value in 'composed'  will be greater than
				// > (0xff00000>>6) or > 0x3c00000. This can be used later to identify the
				// location of the surrogate pairs.
				const m128i64_t composed =
				 _mm_or_si128(_mm_or_si128(asciidata, middlebyte_shifted), _mm_or_si128(highbyte_shifted, middlehighbyte_shifted));
				const m128i64_t composedminus =
				 _mm_sub_epi32(composed, _mm_set1_epi32(0x1'00'00));
				const m128i64_t lowtenbits =
				 _mm_and_si128(composedminus, _mm_set1_epi32(0x3'ff));
				// Notice the 0x3ff mask:
				const m128i64_t hightenbits = _mm_and_si128(_mm_srli_epi32(composedminus, 10), _mm_set1_epi32(0x3'ff));
				const m128i64_t lowtenbitsadd =
				 _mm_add_epi32(lowtenbits, _mm_set1_epi32(0xDC'00));
				const m128i64_t hightenbitsadd =
				 _mm_add_epi32(hightenbits, _mm_set1_epi32(0xD8'00));
				const m128i64_t lowtenbitsaddshifted = _mm_slli_epi32(lowtenbitsadd, 16);
				m128i64_t surrogates =
				 _mm_or_si128(hightenbitsadd, lowtenbitsaddshifted);
				uint32_t basic_buffer[4];
				_mm_storeu_si128(union_cast<m128i64_t *>(basic_buffer), composed);
				uint32_t surrogate_buffer[4];
				_mm_storeu_si128(union_cast<m128i64_t *>(surrogate_buffer), surrogates);
				for(size_t i = 0; i < 3; i++)
				{
					if(basic_buffer[i] > 0x3'c0'00'00)
					{
						__out[0] = uint16_t(surrogate_buffer[i] & 0xff'ff);
						__out[1] = uint16_t(surrogate_buffer[i] >> 16);
						__out		+= 2;
					}
					else
					{
						__out[0] = uint16_t(basic_buffer[i]);
						__out++;
					}
				}
			}
			return consumed;
		}
		pf_hint_nodiscard pf_decl_inline size_t
		__convert_masked_utf8_to_utf16le_sse2(
		 const u8char_t *__in,
		 int32_t __end_of_code_point_mask,
		 u16char_t *&__out) pf_attr_noexcept
		{
			// we use an approach where we try to process up to 12 input bytes.
			// Why 12 input bytes and not 16? Because we are concerned with the size of
			// the lookup tables. Also 12 is nicely divisible by two and three.
			//
			//
			// Optimization note: our main path below is load-latency dependent. Thus it is maybe
			// beneficial to have fast paths that depend on branch prediction but have less latency.
			// This results in more instructions but, potentially, also higher speeds.
			//
			// We first try a few fast paths.
			const m128i64_t in = _mm_loadu_si128(union_cast<m128i64_t *>(__in));
			const uint16_t input_utf8_end_of_code_point_mask =
			 __end_of_code_point_mask & 0xf'ff;
			if(((__end_of_code_point_mask & 0xff'ff) == 0xff'ff))
			{
				// We process the data in chunks of 16 bytes.
				m128i64_t ascii_first	 = _mm_cvtepu8_epi16(in);
				m128i64_t ascii_second = _mm_cvtepu8_epi16(_mm_srli_si128(in, 8));
				_mm_storeu_si128(union_cast<m128i64_t *>(__out), ascii_first);
				_mm_storeu_si128(union_cast<m128i64_t *>(__out + 8), ascii_second);
				__out += 16;	// We wrote 16 16-bit characters.
				return 16;		// We consumed 16 bytes.
			}
			if(((__end_of_code_point_mask & 0xFF'FF) == 0xaa'aa))
			{
				// We want to take 8 2-byte UTF-8 ws and turn them into 8 2-byte UTF-16 ws.
				// There is probably a more efficient sequence, but the following might do.
				const m128i64_t sh			 = _mm_setr_epi8(1, 0, 3, 2, 5, 4, 7, 6, 9, 8, 11, 10, 13, 12, 15, 14);
				const m128i64_t perm		 = _mm_shuffle_epi8(in, sh);
				const m128i64_t ascii		 = _mm_and_si128(perm, _mm_set1_epi16(0x7f));
				const m128i64_t highbyte = _mm_and_si128(perm, _mm_set1_epi16(0x1f'00));
				m128i64_t composed			 = _mm_or_si128(ascii, _mm_srli_epi16(highbyte, 2));
				_mm_storeu_si128(union_cast<m128i64_t *>(__out), composed);
				__out += 8;	 // We wrote 16 bytes, 8 code points.
				return 16;
			}
			if(input_utf8_end_of_code_point_mask == 0x9'24)
			{
				// We want to take 4 3-byte UTF-8 ws and turn them into 4 2-byte UTF-16 ws.
				// There is probably a more efficient sequence, but the following might do.
				const m128i64_t sh	 = _mm_setr_epi8(2, 1, 0, -1, 5, 4, 3, -1, 8, 7, 6, -1, 11, 10, 9, -1);
				const m128i64_t perm = _mm_shuffle_epi8(in, sh);
				const m128i64_t ascii =
				 _mm_and_si128(perm, _mm_set1_epi32(0x7f));			// 7 or 6 bits
				const m128i64_t middlebyte =
				 _mm_and_si128(perm, _mm_set1_epi32(0x3f'00));	// 5 or 6 bits
				const m128i64_t middlebyte_shifted = _mm_srli_epi32(middlebyte, 2);
				const m128i64_t highbyte =
				 _mm_and_si128(perm, _mm_set1_epi32(0x0f'00'00));	 // 4 bits
				const m128i64_t highbyte_shifted = _mm_srli_epi32(highbyte, 4);
				const m128i64_t composed =
				 _mm_or_si128(_mm_or_si128(ascii, middlebyte_shifted), highbyte_shifted);
				m128i64_t composed_repacked = _mm_packus_epi32(composed, composed);
				_mm_storeu_si128(union_cast<m128i64_t *>(__out), composed_repacked);
				__out += 4;
				return 12;
			}
			/// We do not have a fast path available, so we fallback.

			const uint8_t idx			 = __big_index_utf8_to_utf_16[input_utf8_end_of_code_point_mask][0];
			const uint8_t consumed = __big_index_utf8_to_utf_16[input_utf8_end_of_code_point_mask][1];
			if(idx < 64)
			{
				// SIX (6) input code-ws
				// this is a relatively easy scenario
				// we process SIX (6) input code-ws. The max length in bytes of six code
				// ws spanning between 1 and 2 bytes each is 12 bytes. On processors
				// where pdep/pext is fast, we might be able to use a small lookup table.
				const m128i64_t sh =
				 _mm_loadu_si128(union_cast<const m128i64_t *>(__shut_utf8_to_utf16[idx]));
				const m128i64_t perm		 = _mm_shuffle_epi8(in, sh);
				const m128i64_t ascii		 = _mm_and_si128(perm, _mm_set1_epi16(0x7f));
				const m128i64_t highbyte = _mm_and_si128(perm, _mm_set1_epi16(0x1f'00));
				m128i64_t composed			 = _mm_or_si128(ascii, _mm_srli_epi16(highbyte, 2));
				_mm_storeu_si128(union_cast<m128i64_t *>(__out), composed);
				__out += 6;	 // We wrote 12 bytes, 6 code points.
			}
			else if(idx < 145)
			{
				// FOUR (4) input code-ws
				const m128i64_t sh =
				 _mm_loadu_si128(union_cast<const m128i64_t *>(__shut_utf8_to_utf16[idx]));
				const m128i64_t perm = _mm_shuffle_epi8(in, sh);
				const m128i64_t ascii =
				 _mm_and_si128(perm, _mm_set1_epi32(0x7f));			// 7 or 6 bits
				const m128i64_t middlebyte =
				 _mm_and_si128(perm, _mm_set1_epi32(0x3f'00));	// 5 or 6 bits
				const m128i64_t middlebyte_shifted = _mm_srli_epi32(middlebyte, 2);
				const m128i64_t highbyte =
				 _mm_and_si128(perm, _mm_set1_epi32(0x0f'00'00));	 // 4 bits
				const m128i64_t highbyte_shifted = _mm_srli_epi32(highbyte, 4);
				const m128i64_t composed =
				 _mm_or_si128(_mm_or_si128(ascii, middlebyte_shifted), highbyte_shifted);
				m128i64_t composed_repacked = _mm_packus_epi32(composed, composed);
				_mm_storeu_si128(union_cast<m128i64_t *>(__out), composed_repacked);
				__out += 4;
			}
			else if(idx < 209)
			{
				// TWO (2) input code-ws
				//////////////
				// There might be garbage inputs where a leading byte mascarades as a four-byte
				// leading byte (by being followed by 3 continuation byte), but is not greater than
				// 0xf0. This could trigger a buffer overflow if we only counted leading
				// bytes of the form 0xf0 as generating surrogate pairs, without further UTF-8 validation.
				// Thus we must be careful to ensure that only leading bytes at least as large as 0xf0 generate surrogate pairs.
				// We do as at the cost of an extra mask.
				/////////////
				const m128i64_t sh =
				 _mm_loadu_si128(union_cast<const m128i64_t *>(__shut_utf8_to_utf16[idx]));
				const m128i64_t perm							 = _mm_shuffle_epi8(in, sh);
				const m128i64_t ascii							 = _mm_and_si128(perm, _mm_set1_epi32(0x7f));
				const m128i64_t middlebyte				 = _mm_and_si128(perm, _mm_set1_epi32(0x3f'00));
				const m128i64_t middlebyte_shifted = _mm_srli_epi32(middlebyte, 2);
				m128i64_t middlehighbyte					 = _mm_and_si128(perm, _mm_set1_epi32(0x3f'00'00));
				// correct for spurious high bit
				const m128i64_t correct =
				 _mm_srli_epi32(_mm_and_si128(perm, _mm_set1_epi32(0x40'00'00)), 1);
				middlehighbyte												 = _mm_xor_si128(correct, middlehighbyte);
				const m128i64_t middlehighbyte_shifted = _mm_srli_epi32(middlehighbyte, 4);
				// We deliberately carry the leading four bits in highbyte if they are present,
				// we remove them later when computing hightenbits.
				const m128i64_t highbyte							 = _mm_and_si128(perm, _mm_set1_epi32(0xff'00'00'00));
				const m128i64_t highbyte_shifted			 = _mm_srli_epi32(highbyte, 6);
				// When we need to generate a surrogate pair (leading byte > 0xF0), then
				// the corresponding 32-bit value in 'composed'  will be greater than
				// > (0xff00000>>6) or > 0x3c00000. This can be used later to identify the
				// location of the surrogate pairs.
				const m128i64_t composed =
				 _mm_or_si128(_mm_or_si128(ascii, middlebyte_shifted), _mm_or_si128(highbyte_shifted, middlehighbyte_shifted));
				const m128i64_t composedminus =
				 _mm_sub_epi32(composed, _mm_set1_epi32(0x1'00'00));
				const m128i64_t lowtenbits =
				 _mm_and_si128(composedminus, _mm_set1_epi32(0x3'ff));
				// Notice the 0x3ff mask:
				const m128i64_t hightenbits = _mm_and_si128(_mm_srli_epi32(composedminus, 10), _mm_set1_epi32(0x3'ff));
				const m128i64_t lowtenbitsadd =
				 _mm_add_epi32(lowtenbits, _mm_set1_epi32(0xDC'00));
				const m128i64_t hightenbitsadd =
				 _mm_add_epi32(hightenbits, _mm_set1_epi32(0xD8'00));
				const m128i64_t lowtenbitsaddshifted = _mm_slli_epi32(lowtenbitsadd, 16);
				m128i64_t surrogates =
				 _mm_or_si128(hightenbitsadd, lowtenbitsaddshifted);
				uint32_t basic_buffer[4];
				_mm_storeu_si128(union_cast<m128i64_t *>(basic_buffer), composed);
				uint32_t surrogate_buffer[4];
				_mm_storeu_si128(union_cast<m128i64_t *>(surrogate_buffer), surrogates);
				for(size_t i = 0; i < 3; i++)
				{
					if(basic_buffer[i] > 0x3'c0'00'00)
					{
						__out[0] = uint16_t(surrogate_buffer[i] & 0xff'ff);
						__out[1] = uint16_t(surrogate_buffer[i] >> 16);
						__out		+= 2;
					}
					else
					{
						__out[0] = uint16_t(basic_buffer[i]);
						__out++;
					}
				}
			}
			return consumed;
		}
		pf_hint_nodiscard pf_decl_always_inline pf_decl_constexpr char_error_t
		convert(
		 const u8char_t *__in,
		 size_t __len,
		 u16char_t *__out) pf_attr_noexcept
		{
			size_t p				 = 0;
			u16char_t *start = __out;

			// In the worst case, we have the haswell kernel which can cause an overflow of
			// 8 bytes when calling convert_masked_utf8_to_utf16. If you skip the last 16 bytes,
			// and if the data is valid, then it is entirely safe because 16 UTF-8 bytes generate
			// much more than 8 bytes. However, you cannot generally assume that you have valid
			// UTF-8 input, so we are going to go back from the end counting 8 leading bytes,
			// to give us a good margin.
			size_t lb			= 0;
			size_t margin = __len;
			for(; margin > 0 && lb < 8; --margin)
			{
				lb += (int8_t(__in[margin - 1]) > -65);
			}
			// If the input is long enough, then we have that margin-1 is the eight last leading byte.
			const size_t safety_margin = __len - margin + 1;	// to avoid overruns!
			if(!std::is_constant_evaluated())
			{
				/// NOTE: 512

				// 256
				if(instruction_set::simd & instruction_set::AVX2_BIT)
				{
					m256i64_t prev = _mm256_setzero_si256();
					m256i64_t err	 = _mm256_setzero_si256();
					for(; p + 32 + safety_margin <= __len;)
					{
						const m256i64_t seq = _mm256_lddqu_si256(union_cast<const m256i64_t *>(&__in[p]));
						if(utf8::__is_ascii_avx2(seq))
						{
							// Store ascii as utf16
							const m256i64_t first	 = _mm256_cvtepu8_epi16(_mm256_castsi256_si128(seq));
							const m256i64_t second = _mm256_cvtepu8_epi16(_mm256_extractf128_si256(seq, 1));
							_mm256_storeu_si256(union_cast<m256i64_t *>(__out), first);
							_mm256_storeu_si256(union_cast<m256i64_t *>(__out + 16), second);
							__out += 32;
							p			+= 32;
						}
						else
						{
							utf8::__check_bytes_avx2(seq, err, prev);
							if(!_mm256_testz_si256(err, err))
							{
								char_error_t res = __rewind_and_convert_with_errors_utf8_to_utf16le(p, __in + p, __len - p, __out);
								res.position		+= p;
								return res;
							}
							int32_t continuation_mask			 = _mm256_movemask_epi8(_mm256_cmpgt_epi8(_mm256_set1_epi8(-64), seq));
							int32_t leading_mask					 = ~continuation_mask;
							int32_t end_of_code_point_mask = leading_mask >> 1;
							// We process in blocks of up to 12 bytes except possibly
							// for fast paths which may process up to 16 bytes. For the
							// slow path to work, we should have at least 12 input bytes left.
							size_t max_starting_point			 = (p + 32) - 12;
							// Next loop is going to run at least five times.
							while(p < max_starting_point)
							{
								// Performance note: our ability to compute 'consumed' and
								// then shift and recompute is critical. If there is a
								// latency of, say, 4 cycles on getting 'consumed', then
								// the inner loop might have a total latency of about 6 cycles.
								// Yet we process between 6 to 12 inputs bytes, thus we get
								// a speed limit between 1 cycle/byte and 0.5 cycle/byte
								// for this section of the code. Hence, there is a limit
								// to how much we can further increase this latency before
								// it seriously harms performance.
								size_t consumed					 = __convert_masked_utf8_to_utf16le_avx2(__in + p, end_of_code_point_mask, __out);
								p												+= consumed;
								end_of_code_point_mask >>= consumed;
							}
							// At this point there may remain between 0 and 12 bytes in the
							// 64-byte block. These bytes will be processed again. So we have an
							// 80% efficiency (in the worst case). In practice we expect an
							// 85% to 90% efficiency.
						}
						prev = seq;
					}
					if(!_mm256_testz_si256(err, err))
					{
						char_error_t res = __rewind_and_convert_with_errors_utf8_to_utf16le(p, __in + p, __len - p, __out);
						res.position		+= p;
						return res;
					}
					if(p < __len)
					{
						char_error_t res = __rewind_and_convert_with_errors_utf8_to_utf16le(p, __in + p, __len - p, __out);
						if(res.code != char_error_code::success)
						{	 // In case of error, we want the error position
							res.position += p;
							return res;
						}
						else
						{	 // In case of success, we want the number of w written
							__out += res.position;
						}
					}
					return char_error_t { char_error_code::success, countof(start, __out) };
				}

				// 128
				if(instruction_set::simd & instruction_set::SSE2_BIT)
				{
					m128i64_t prev = _mm_setzero_si128();
					m128i64_t err	 = _mm_setzero_si128();
					for(; p + 16 + safety_margin <= __len;)
					{
						m128i64_t seq = _mm_lddqu_si128(union_cast<const m128i64_t *>(&__in[p]));
						if(utf8::__is_ascii_sse2(seq))
						{
							m128i64_t first = _mm_cvtepu8_epi16(seq);
							_mm_storeu_si128(union_cast<m128i64_t *>(__out), first);
							m128i64_t second = _mm_cvtepu8_epi16(_mm_srli_si128(seq, 8));
							_mm_storeu_si128(union_cast<m128i64_t *>(__out + 8), second);
							__out += 16;
							p			+= 16;
						}
						else
						{
							utf8::__check_bytes_sse2(seq, err, prev);
							if(!_mm_testz_si128(err, err))
							{
								char_error_t res = __rewind_and_convert_with_errors_utf8_to_utf16le(p, __in + p, __len - p, __out);
								res.position		+= p;
								return res;
							}
							int32_t continuation_mask			 = _mm_movemask_epi8(_mm_cmpgt_epi8(_mm_set1_epi8(-64), seq));
							int32_t leading_mask					 = ~continuation_mask;
							int32_t end_of_code_point_mask = leading_mask >> 1;
							size_t max_starting_point			 = (p + 16) - 12;
							while(p < max_starting_point)
							{
								size_t consumed					 = __convert_masked_utf8_to_utf16le_sse2(__in + p, end_of_code_point_mask, __out);
								p												+= consumed;
								end_of_code_point_mask >>= consumed;
							}
						}
						prev = seq;
					}
					if(!_mm_testz_si128(err, err))
					{
						char_error_t res = __rewind_and_convert_with_errors_utf8_to_utf16le(p, __in + p, __len - p, __out);
						res.position		+= p;
						return res;
					}
					if(p < __len)
					{
						char_error_t res = __rewind_and_convert_with_errors_utf8_to_utf16le(p, __in + p, __len - p, __out);
						if(res.code != char_error_code::success)
						{	 // In case of error, we want the error position
							res.position += p;
							return res;
						}
						else
						{	 // In case of success, we want the number of w written
							__out += res.position;
						}
					}
					return char_error_t { char_error_code::success, countof(start, __out) };
				}
			}

			// 64
			return __convert_with_errors_utf8_to_utf16le(__in, __len, __out);
		}
	}	 // namespace utf8_to_utf16le


	/// UTF16-LE -> UTF8
	namespace utf16le_to_utf8
	{
		// Required
		pf_hint_nodiscard pf_decl_always_inline pf_decl_constexpr size_t
		required_count(
		 const u16char_t *__in,
		 size_t __len) pf_attr_noexcept
		{
			size_t p = 0;
			size_t c = 0;
			if(!std::is_constant_evaluated())
			{
				// 256
				for(; p + 16 <= __len; p += 16)
				{
					const m256i64_t seq						= _mm256_lddqu_si256(union_cast<const m256i64_t *>(&__in[p]));
					const int32_t is_ascii_mask		= _mm256_movemask_epi8(utf16le::__smaller_equal_avx2(seq, _mm256_set1_epi16(127)));
					const int32_t two_byte_mask		= _mm256_movemask_epi8(utf16le::__smaller_equal_avx2(seq, _mm256_set1_epi16(2'047)));
					const int32_t three_byte_mask = _mm256_movemask_epi8(_mm256_or_si256(utf16le::__smaller_equal_avx2(seq, _mm256_set1_epi16(-10'241)), utf16le::__greater_equal_avx2(seq, _mm256_set1_epi16(-8'192))));

					const int32_t ascii_count			 = _popcnt32(is_ascii_mask) / 2;
					const int32_t two_byte_count	 = _popcnt32(two_byte_mask & ~is_ascii_mask) / 2;
					const int32_t three_byte_count = _popcnt32(three_byte_mask & ~two_byte_mask & ~is_ascii_mask) / 2;
					const int32_t not_pair_count	 = _popcnt32(~three_byte_mask & ~two_byte_mask & ~is_ascii_mask) / 2;

					c += ascii_count + 2 * (two_byte_count + not_pair_count) + 3 * three_byte_count;
				}

				// 128
				for(; p + 8 <= __len; p += 8)
				{
					const m128i64_t seq						= _mm_lddqu_si128(union_cast<const m128i64_t *>(&__in[p]));
					const int32_t is_ascii_mask		= _mm_movemask_epi8(utf16le::__smaller_equal_sse2(seq, _mm_set1_epi16(127)));
					const int32_t two_byte_mask		= _mm_movemask_epi8(utf16le::__smaller_equal_sse2(seq, _mm_set1_epi16(2'047))) & ~is_ascii_mask;
					const int32_t three_byte_mask = _mm_movemask_epi8(_mm_or_si128(utf16le::__smaller_equal_sse2(seq, _mm_set1_epi16(-10'241)), utf16le::__greater_equal_sse2(seq, _mm_set1_epi16(-8'192)))) & ~two_byte_mask;

					const int32_t ascii_count			 = _popcnt32(is_ascii_mask) / 2;
					const int32_t two_byte_count	 = _popcnt32(two_byte_mask & ~is_ascii_mask) / 2;
					const int32_t three_byte_count = _popcnt32(three_byte_mask & ~two_byte_mask & ~is_ascii_mask) / 2;
					const int32_t not_pair_count	 = _popcnt32(~three_byte_mask & ~two_byte_mask & ~is_ascii_mask & 0xFF'FF) / 2;

					c += ascii_count + 2 * (two_byte_count + not_pair_count) + 3 * three_byte_count;
				}
			}

			// 64
			for(; p < __len; ++p)
			{
				uint16_t w = uint16_t(__in[p]);
				if(w <= 0x7F) { ++c; }
				else if(w <= 0x7'FF) { c += 2; }
				else if((w <= 0xD7'FF) || (w >= 0xE0'00)) { c += 3; }
				else { c += 2; }
			}
			return c;
		}

		// Convert
		// 1 byte for length, 16 bytes for mask
		const uint8_t __pack_1_2_utf16_to_utf8[256][17] = {
			{16,	1, 0, 3, 2, 5, 4,	7,	 6,	9,		 8,		11,		10,		13,		12,		15,		14	 },
			{ 15, 0, 3, 2, 5, 4, 7,	6,	 9,	8,		 11,	 10,	 13,	 12,	 15,	 14,	 0x80},
			{ 15, 1, 0, 3, 2, 5, 4,	7,	 6,	8,		 11,	 10,	 13,	 12,	 15,	 14,	 0x80},
			{ 14, 0, 3, 2, 5, 4, 7,	6,	 8,	11,		10,		13,		12,		15,		14,		0x80, 0x80},
			{ 15, 1, 0, 2, 5, 4, 7,	6,	 9,	8,		 11,	 10,	 13,	 12,	 15,	 14,	 0x80},
			{ 14, 0, 2, 5, 4, 7, 6,	9,	 8,	11,		10,		13,		12,		15,		14,		0x80, 0x80},
			{ 14, 1, 0, 2, 5, 4, 7,	6,	 8,	11,		10,		13,		12,		15,		14,		0x80, 0x80},
			{ 13, 0, 2, 5, 4, 7, 6,	8,	 11, 10,	 13,	 12,	 15,	 14,	 0x80, 0x80, 0x80},
			{ 15, 1, 0, 3, 2, 5, 4,	7,	 6,	9,		 8,		10,		13,		12,		15,		14,		0x80},
			{ 14, 0, 3, 2, 5, 4, 7,	6,	 9,	8,		 10,	 13,	 12,	 15,	 14,	 0x80, 0x80},
			{ 14, 1, 0, 3, 2, 5, 4,	7,	 6,	8,		 10,	 13,	 12,	 15,	 14,	 0x80, 0x80},
			{ 13, 0, 3, 2, 5, 4, 7,	6,	 8,	10,		13,		12,		15,		14,		0x80, 0x80, 0x80},
			{ 14, 1, 0, 2, 5, 4, 7,	6,	 9,	8,		 10,	 13,	 12,	 15,	 14,	 0x80, 0x80},
			{ 13, 0, 2, 5, 4, 7, 6,	9,	 8,	10,		13,		12,		15,		14,		0x80, 0x80, 0x80},
			{ 13, 1, 0, 2, 5, 4, 7,	6,	 8,	10,		13,		12,		15,		14,		0x80, 0x80, 0x80},
			{ 12, 0, 2, 5, 4, 7, 6,	8,	 10, 13,	 12,	 15,	 14,	 0x80, 0x80, 0x80, 0x80},
			{ 15, 1, 0, 3, 2, 4, 7,	6,	 9,	8,		 11,	 10,	 13,	 12,	 15,	 14,	 0x80},
			{ 14, 0, 3, 2, 4, 7, 6,	9,	 8,	11,		10,		13,		12,		15,		14,		0x80, 0x80},
			{ 14, 1, 0, 3, 2, 4, 7,	6,	 8,	11,		10,		13,		12,		15,		14,		0x80, 0x80},
			{ 13, 0, 3, 2, 4, 7, 6,	8,	 11, 10,	 13,	 12,	 15,	 14,	 0x80, 0x80, 0x80},
			{ 14, 1, 0, 2, 4, 7, 6,	9,	 8,	11,		10,		13,		12,		15,		14,		0x80, 0x80},
			{ 13, 0, 2, 4, 7, 6, 9,	8,	 11, 10,	 13,	 12,	 15,	 14,	 0x80, 0x80, 0x80},
			{ 13, 1, 0, 2, 4, 7, 6,	8,	 11, 10,	 13,	 12,	 15,	 14,	 0x80, 0x80, 0x80},
			{ 12, 0, 2, 4, 7, 6, 8,	11, 10, 13,		12,		15,		14,		0x80, 0x80, 0x80, 0x80},
			{ 14, 1, 0, 3, 2, 4, 7,	6,	 9,	8,		 10,	 13,	 12,	 15,	 14,	 0x80, 0x80},
			{ 13, 0, 3, 2, 4, 7, 6,	9,	 8,	10,		13,		12,		15,		14,		0x80, 0x80, 0x80},
			{ 13, 1, 0, 3, 2, 4, 7,	6,	 8,	10,		13,		12,		15,		14,		0x80, 0x80, 0x80},
			{ 12, 0, 3, 2, 4, 7, 6,	8,	 10, 13,	 12,	 15,	 14,	 0x80, 0x80, 0x80, 0x80},
			{ 13, 1, 0, 2, 4, 7, 6,	9,	 8,	10,		13,		12,		15,		14,		0x80, 0x80, 0x80},
			{ 12, 0, 2, 4, 7, 6, 9,	8,	 10, 13,	 12,	 15,	 14,	 0x80, 0x80, 0x80, 0x80},
			{ 12, 1, 0, 2, 4, 7, 6,	8,	 10, 13,	 12,	 15,	 14,	 0x80, 0x80, 0x80, 0x80},
			{ 11, 0, 2, 4, 7, 6, 8,	10, 13, 12,		15,		14,		0x80, 0x80, 0x80, 0x80, 0x80},
			{ 15, 1, 0, 3, 2, 5, 4,	7,	 6,	9,		 8,		11,		10,		12,		15,		14,		0x80},
			{ 14, 0, 3, 2, 5, 4, 7,	6,	 9,	8,		 11,	 10,	 12,	 15,	 14,	 0x80, 0x80},
			{ 14, 1, 0, 3, 2, 5, 4,	7,	 6,	8,		 11,	 10,	 12,	 15,	 14,	 0x80, 0x80},
			{ 13, 0, 3, 2, 5, 4, 7,	6,	 8,	11,		10,		12,		15,		14,		0x80, 0x80, 0x80},
			{ 14, 1, 0, 2, 5, 4, 7,	6,	 9,	8,		 11,	 10,	 12,	 15,	 14,	 0x80, 0x80},
			{ 13, 0, 2, 5, 4, 7, 6,	9,	 8,	11,		10,		12,		15,		14,		0x80, 0x80, 0x80},
			{ 13, 1, 0, 2, 5, 4, 7,	6,	 8,	11,		10,		12,		15,		14,		0x80, 0x80, 0x80},
			{ 12, 0, 2, 5, 4, 7, 6,	8,	 11, 10,	 12,	 15,	 14,	 0x80, 0x80, 0x80, 0x80},
			{ 14, 1, 0, 3, 2, 5, 4,	7,	 6,	9,		 8,		10,		12,		15,		14,		0x80, 0x80},
			{ 13, 0, 3, 2, 5, 4, 7,	6,	 9,	8,		 10,	 12,	 15,	 14,	 0x80, 0x80, 0x80},
			{ 13, 1, 0, 3, 2, 5, 4,	7,	 6,	8,		 10,	 12,	 15,	 14,	 0x80, 0x80, 0x80},
			{ 12, 0, 3, 2, 5, 4, 7,	6,	 8,	10,		12,		15,		14,		0x80, 0x80, 0x80, 0x80},
			{ 13, 1, 0, 2, 5, 4, 7,	6,	 9,	8,		 10,	 12,	 15,	 14,	 0x80, 0x80, 0x80},
			{ 12, 0, 2, 5, 4, 7, 6,	9,	 8,	10,		12,		15,		14,		0x80, 0x80, 0x80, 0x80},
			{ 12, 1, 0, 2, 5, 4, 7,	6,	 8,	10,		12,		15,		14,		0x80, 0x80, 0x80, 0x80},
			{ 11, 0, 2, 5, 4, 7, 6,	8,	 10, 12,	 15,	 14,	 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 14, 1, 0, 3, 2, 4, 7,	6,	 9,	8,		 11,	 10,	 12,	 15,	 14,	 0x80, 0x80},
			{ 13, 0, 3, 2, 4, 7, 6,	9,	 8,	11,		10,		12,		15,		14,		0x80, 0x80, 0x80},
			{ 13, 1, 0, 3, 2, 4, 7,	6,	 8,	11,		10,		12,		15,		14,		0x80, 0x80, 0x80},
			{ 12, 0, 3, 2, 4, 7, 6,	8,	 11, 10,	 12,	 15,	 14,	 0x80, 0x80, 0x80, 0x80},
			{ 13, 1, 0, 2, 4, 7, 6,	9,	 8,	11,		10,		12,		15,		14,		0x80, 0x80, 0x80},
			{ 12, 0, 2, 4, 7, 6, 9,	8,	 11, 10,	 12,	 15,	 14,	 0x80, 0x80, 0x80, 0x80},
			{ 12, 1, 0, 2, 4, 7, 6,	8,	 11, 10,	 12,	 15,	 14,	 0x80, 0x80, 0x80, 0x80},
			{ 11, 0, 2, 4, 7, 6, 8,	11, 10, 12,		15,		14,		0x80, 0x80, 0x80, 0x80, 0x80},
			{ 13, 1, 0, 3, 2, 4, 7,	6,	 9,	8,		 10,	 12,	 15,	 14,	 0x80, 0x80, 0x80},
			{ 12, 0, 3, 2, 4, 7, 6,	9,	 8,	10,		12,		15,		14,		0x80, 0x80, 0x80, 0x80},
			{ 12, 1, 0, 3, 2, 4, 7,	6,	 8,	10,		12,		15,		14,		0x80, 0x80, 0x80, 0x80},
			{ 11, 0, 3, 2, 4, 7, 6,	8,	 10, 12,	 15,	 14,	 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 12, 1, 0, 2, 4, 7, 6,	9,	 8,	10,		12,		15,		14,		0x80, 0x80, 0x80, 0x80},
			{ 11, 0, 2, 4, 7, 6, 9,	8,	 10, 12,	 15,	 14,	 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 11, 1, 0, 2, 4, 7, 6,	8,	 10, 12,	 15,	 14,	 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 10, 0, 2, 4, 7, 6, 8,	10, 12, 15,		14,		0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 15, 1, 0, 3, 2, 5, 4,	6,	 9,	8,		 11,	 10,	 13,	 12,	 15,	 14,	 0x80},
			{ 14, 0, 3, 2, 5, 4, 6,	9,	 8,	11,		10,		13,		12,		15,		14,		0x80, 0x80},
			{ 14, 1, 0, 3, 2, 5, 4,	6,	 8,	11,		10,		13,		12,		15,		14,		0x80, 0x80},
			{ 13, 0, 3, 2, 5, 4, 6,	8,	 11, 10,	 13,	 12,	 15,	 14,	 0x80, 0x80, 0x80},
			{ 14, 1, 0, 2, 5, 4, 6,	9,	 8,	11,		10,		13,		12,		15,		14,		0x80, 0x80},
			{ 13, 0, 2, 5, 4, 6, 9,	8,	 11, 10,	 13,	 12,	 15,	 14,	 0x80, 0x80, 0x80},
			{ 13, 1, 0, 2, 5, 4, 6,	8,	 11, 10,	 13,	 12,	 15,	 14,	 0x80, 0x80, 0x80},
			{ 12, 0, 2, 5, 4, 6, 8,	11, 10, 13,		12,		15,		14,		0x80, 0x80, 0x80, 0x80},
			{ 14, 1, 0, 3, 2, 5, 4,	6,	 9,	8,		 10,	 13,	 12,	 15,	 14,	 0x80, 0x80},
			{ 13, 0, 3, 2, 5, 4, 6,	9,	 8,	10,		13,		12,		15,		14,		0x80, 0x80, 0x80},
			{ 13, 1, 0, 3, 2, 5, 4,	6,	 8,	10,		13,		12,		15,		14,		0x80, 0x80, 0x80},
			{ 12, 0, 3, 2, 5, 4, 6,	8,	 10, 13,	 12,	 15,	 14,	 0x80, 0x80, 0x80, 0x80},
			{ 13, 1, 0, 2, 5, 4, 6,	9,	 8,	10,		13,		12,		15,		14,		0x80, 0x80, 0x80},
			{ 12, 0, 2, 5, 4, 6, 9,	8,	 10, 13,	 12,	 15,	 14,	 0x80, 0x80, 0x80, 0x80},
			{ 12, 1, 0, 2, 5, 4, 6,	8,	 10, 13,	 12,	 15,	 14,	 0x80, 0x80, 0x80, 0x80},
			{ 11, 0, 2, 5, 4, 6, 8,	10, 13, 12,		15,		14,		0x80, 0x80, 0x80, 0x80, 0x80},
			{ 14, 1, 0, 3, 2, 4, 6,	9,	 8,	11,		10,		13,		12,		15,		14,		0x80, 0x80},
			{ 13, 0, 3, 2, 4, 6, 9,	8,	 11, 10,	 13,	 12,	 15,	 14,	 0x80, 0x80, 0x80},
			{ 13, 1, 0, 3, 2, 4, 6,	8,	 11, 10,	 13,	 12,	 15,	 14,	 0x80, 0x80, 0x80},
			{ 12, 0, 3, 2, 4, 6, 8,	11, 10, 13,		12,		15,		14,		0x80, 0x80, 0x80, 0x80},
			{ 13, 1, 0, 2, 4, 6, 9,	8,	 11, 10,	 13,	 12,	 15,	 14,	 0x80, 0x80, 0x80},
			{ 12, 0, 2, 4, 6, 9, 8,	11, 10, 13,		12,		15,		14,		0x80, 0x80, 0x80, 0x80},
			{ 12, 1, 0, 2, 4, 6, 8,	11, 10, 13,		12,		15,		14,		0x80, 0x80, 0x80, 0x80},
			{ 11, 0, 2, 4, 6, 8, 11, 10, 13, 12,	 15,	 14,	 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 13, 1, 0, 3, 2, 4, 6,	9,	 8,	10,		13,		12,		15,		14,		0x80, 0x80, 0x80},
			{ 12, 0, 3, 2, 4, 6, 9,	8,	 10, 13,	 12,	 15,	 14,	 0x80, 0x80, 0x80, 0x80},
			{ 12, 1, 0, 3, 2, 4, 6,	8,	 10, 13,	 12,	 15,	 14,	 0x80, 0x80, 0x80, 0x80},
			{ 11, 0, 3, 2, 4, 6, 8,	10, 13, 12,		15,		14,		0x80, 0x80, 0x80, 0x80, 0x80},
			{ 12, 1, 0, 2, 4, 6, 9,	8,	 10, 13,	 12,	 15,	 14,	 0x80, 0x80, 0x80, 0x80},
			{ 11, 0, 2, 4, 6, 9, 8,	10, 13, 12,		15,		14,		0x80, 0x80, 0x80, 0x80, 0x80},
			{ 11, 1, 0, 2, 4, 6, 8,	10, 13, 12,		15,		14,		0x80, 0x80, 0x80, 0x80, 0x80},
			{ 10, 0, 2, 4, 6, 8, 10, 13, 12, 15,	 14,	 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 14, 1, 0, 3, 2, 5, 4,	6,	 9,	8,		 11,	 10,	 12,	 15,	 14,	 0x80, 0x80},
			{ 13, 0, 3, 2, 5, 4, 6,	9,	 8,	11,		10,		12,		15,		14,		0x80, 0x80, 0x80},
			{ 13, 1, 0, 3, 2, 5, 4,	6,	 8,	11,		10,		12,		15,		14,		0x80, 0x80, 0x80},
			{ 12, 0, 3, 2, 5, 4, 6,	8,	 11, 10,	 12,	 15,	 14,	 0x80, 0x80, 0x80, 0x80},
			{ 13, 1, 0, 2, 5, 4, 6,	9,	 8,	11,		10,		12,		15,		14,		0x80, 0x80, 0x80},
			{ 12, 0, 2, 5, 4, 6, 9,	8,	 11, 10,	 12,	 15,	 14,	 0x80, 0x80, 0x80, 0x80},
			{ 12, 1, 0, 2, 5, 4, 6,	8,	 11, 10,	 12,	 15,	 14,	 0x80, 0x80, 0x80, 0x80},
			{ 11, 0, 2, 5, 4, 6, 8,	11, 10, 12,		15,		14,		0x80, 0x80, 0x80, 0x80, 0x80},
			{ 13, 1, 0, 3, 2, 5, 4,	6,	 9,	8,		 10,	 12,	 15,	 14,	 0x80, 0x80, 0x80},
			{ 12, 0, 3, 2, 5, 4, 6,	9,	 8,	10,		12,		15,		14,		0x80, 0x80, 0x80, 0x80},
			{ 12, 1, 0, 3, 2, 5, 4,	6,	 8,	10,		12,		15,		14,		0x80, 0x80, 0x80, 0x80},
			{ 11, 0, 3, 2, 5, 4, 6,	8,	 10, 12,	 15,	 14,	 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 12, 1, 0, 2, 5, 4, 6,	9,	 8,	10,		12,		15,		14,		0x80, 0x80, 0x80, 0x80},
			{ 11, 0, 2, 5, 4, 6, 9,	8,	 10, 12,	 15,	 14,	 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 11, 1, 0, 2, 5, 4, 6,	8,	 10, 12,	 15,	 14,	 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 10, 0, 2, 5, 4, 6, 8,	10, 12, 15,		14,		0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 13, 1, 0, 3, 2, 4, 6,	9,	 8,	11,		10,		12,		15,		14,		0x80, 0x80, 0x80},
			{ 12, 0, 3, 2, 4, 6, 9,	8,	 11, 10,	 12,	 15,	 14,	 0x80, 0x80, 0x80, 0x80},
			{ 12, 1, 0, 3, 2, 4, 6,	8,	 11, 10,	 12,	 15,	 14,	 0x80, 0x80, 0x80, 0x80},
			{ 11, 0, 3, 2, 4, 6, 8,	11, 10, 12,		15,		14,		0x80, 0x80, 0x80, 0x80, 0x80},
			{ 12, 1, 0, 2, 4, 6, 9,	8,	 11, 10,	 12,	 15,	 14,	 0x80, 0x80, 0x80, 0x80},
			{ 11, 0, 2, 4, 6, 9, 8,	11, 10, 12,		15,		14,		0x80, 0x80, 0x80, 0x80, 0x80},
			{ 11, 1, 0, 2, 4, 6, 8,	11, 10, 12,		15,		14,		0x80, 0x80, 0x80, 0x80, 0x80},
			{ 10, 0, 2, 4, 6, 8, 11, 10, 12, 15,	 14,	 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 12, 1, 0, 3, 2, 4, 6,	9,	 8,	10,		12,		15,		14,		0x80, 0x80, 0x80, 0x80},
			{ 11, 0, 3, 2, 4, 6, 9,	8,	 10, 12,	 15,	 14,	 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 11, 1, 0, 3, 2, 4, 6,	8,	 10, 12,	 15,	 14,	 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 10, 0, 3, 2, 4, 6, 8,	10, 12, 15,		14,		0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 11, 1, 0, 2, 4, 6, 9,	8,	 10, 12,	 15,	 14,	 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 10, 0, 2, 4, 6, 9, 8,	10, 12, 15,		14,		0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 10, 1, 0, 2, 4, 6, 8,	10, 12, 15,		14,		0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 9,	 0, 2, 4, 6, 8, 10, 12, 15, 14,		0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 15, 1, 0, 3, 2, 5, 4,	7,	 6,	9,		 8,		11,		10,		13,		12,		14,		0x80},
			{ 14, 0, 3, 2, 5, 4, 7,	6,	 9,	8,		 11,	 10,	 13,	 12,	 14,	 0x80, 0x80},
			{ 14, 1, 0, 3, 2, 5, 4,	7,	 6,	8,		 11,	 10,	 13,	 12,	 14,	 0x80, 0x80},
			{ 13, 0, 3, 2, 5, 4, 7,	6,	 8,	11,		10,		13,		12,		14,		0x80, 0x80, 0x80},
			{ 14, 1, 0, 2, 5, 4, 7,	6,	 9,	8,		 11,	 10,	 13,	 12,	 14,	 0x80, 0x80},
			{ 13, 0, 2, 5, 4, 7, 6,	9,	 8,	11,		10,		13,		12,		14,		0x80, 0x80, 0x80},
			{ 13, 1, 0, 2, 5, 4, 7,	6,	 8,	11,		10,		13,		12,		14,		0x80, 0x80, 0x80},
			{ 12, 0, 2, 5, 4, 7, 6,	8,	 11, 10,	 13,	 12,	 14,	 0x80, 0x80, 0x80, 0x80},
			{ 14, 1, 0, 3, 2, 5, 4,	7,	 6,	9,		 8,		10,		13,		12,		14,		0x80, 0x80},
			{ 13, 0, 3, 2, 5, 4, 7,	6,	 9,	8,		 10,	 13,	 12,	 14,	 0x80, 0x80, 0x80},
			{ 13, 1, 0, 3, 2, 5, 4,	7,	 6,	8,		 10,	 13,	 12,	 14,	 0x80, 0x80, 0x80},
			{ 12, 0, 3, 2, 5, 4, 7,	6,	 8,	10,		13,		12,		14,		0x80, 0x80, 0x80, 0x80},
			{ 13, 1, 0, 2, 5, 4, 7,	6,	 9,	8,		 10,	 13,	 12,	 14,	 0x80, 0x80, 0x80},
			{ 12, 0, 2, 5, 4, 7, 6,	9,	 8,	10,		13,		12,		14,		0x80, 0x80, 0x80, 0x80},
			{ 12, 1, 0, 2, 5, 4, 7,	6,	 8,	10,		13,		12,		14,		0x80, 0x80, 0x80, 0x80},
			{ 11, 0, 2, 5, 4, 7, 6,	8,	 10, 13,	 12,	 14,	 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 14, 1, 0, 3, 2, 4, 7,	6,	 9,	8,		 11,	 10,	 13,	 12,	 14,	 0x80, 0x80},
			{ 13, 0, 3, 2, 4, 7, 6,	9,	 8,	11,		10,		13,		12,		14,		0x80, 0x80, 0x80},
			{ 13, 1, 0, 3, 2, 4, 7,	6,	 8,	11,		10,		13,		12,		14,		0x80, 0x80, 0x80},
			{ 12, 0, 3, 2, 4, 7, 6,	8,	 11, 10,	 13,	 12,	 14,	 0x80, 0x80, 0x80, 0x80},
			{ 13, 1, 0, 2, 4, 7, 6,	9,	 8,	11,		10,		13,		12,		14,		0x80, 0x80, 0x80},
			{ 12, 0, 2, 4, 7, 6, 9,	8,	 11, 10,	 13,	 12,	 14,	 0x80, 0x80, 0x80, 0x80},
			{ 12, 1, 0, 2, 4, 7, 6,	8,	 11, 10,	 13,	 12,	 14,	 0x80, 0x80, 0x80, 0x80},
			{ 11, 0, 2, 4, 7, 6, 8,	11, 10, 13,		12,		14,		0x80, 0x80, 0x80, 0x80, 0x80},
			{ 13, 1, 0, 3, 2, 4, 7,	6,	 9,	8,		 10,	 13,	 12,	 14,	 0x80, 0x80, 0x80},
			{ 12, 0, 3, 2, 4, 7, 6,	9,	 8,	10,		13,		12,		14,		0x80, 0x80, 0x80, 0x80},
			{ 12, 1, 0, 3, 2, 4, 7,	6,	 8,	10,		13,		12,		14,		0x80, 0x80, 0x80, 0x80},
			{ 11, 0, 3, 2, 4, 7, 6,	8,	 10, 13,	 12,	 14,	 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 12, 1, 0, 2, 4, 7, 6,	9,	 8,	10,		13,		12,		14,		0x80, 0x80, 0x80, 0x80},
			{ 11, 0, 2, 4, 7, 6, 9,	8,	 10, 13,	 12,	 14,	 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 11, 1, 0, 2, 4, 7, 6,	8,	 10, 13,	 12,	 14,	 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 10, 0, 2, 4, 7, 6, 8,	10, 13, 12,		14,		0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 14, 1, 0, 3, 2, 5, 4,	7,	 6,	9,		 8,		11,		10,		12,		14,		0x80, 0x80},
			{ 13, 0, 3, 2, 5, 4, 7,	6,	 9,	8,		 11,	 10,	 12,	 14,	 0x80, 0x80, 0x80},
			{ 13, 1, 0, 3, 2, 5, 4,	7,	 6,	8,		 11,	 10,	 12,	 14,	 0x80, 0x80, 0x80},
			{ 12, 0, 3, 2, 5, 4, 7,	6,	 8,	11,		10,		12,		14,		0x80, 0x80, 0x80, 0x80},
			{ 13, 1, 0, 2, 5, 4, 7,	6,	 9,	8,		 11,	 10,	 12,	 14,	 0x80, 0x80, 0x80},
			{ 12, 0, 2, 5, 4, 7, 6,	9,	 8,	11,		10,		12,		14,		0x80, 0x80, 0x80, 0x80},
			{ 12, 1, 0, 2, 5, 4, 7,	6,	 8,	11,		10,		12,		14,		0x80, 0x80, 0x80, 0x80},
			{ 11, 0, 2, 5, 4, 7, 6,	8,	 11, 10,	 12,	 14,	 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 13, 1, 0, 3, 2, 5, 4,	7,	 6,	9,		 8,		10,		12,		14,		0x80, 0x80, 0x80},
			{ 12, 0, 3, 2, 5, 4, 7,	6,	 9,	8,		 10,	 12,	 14,	 0x80, 0x80, 0x80, 0x80},
			{ 12, 1, 0, 3, 2, 5, 4,	7,	 6,	8,		 10,	 12,	 14,	 0x80, 0x80, 0x80, 0x80},
			{ 11, 0, 3, 2, 5, 4, 7,	6,	 8,	10,		12,		14,		0x80, 0x80, 0x80, 0x80, 0x80},
			{ 12, 1, 0, 2, 5, 4, 7,	6,	 9,	8,		 10,	 12,	 14,	 0x80, 0x80, 0x80, 0x80},
			{ 11, 0, 2, 5, 4, 7, 6,	9,	 8,	10,		12,		14,		0x80, 0x80, 0x80, 0x80, 0x80},
			{ 11, 1, 0, 2, 5, 4, 7,	6,	 8,	10,		12,		14,		0x80, 0x80, 0x80, 0x80, 0x80},
			{ 10, 0, 2, 5, 4, 7, 6,	8,	 10, 12,	 14,	 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 13, 1, 0, 3, 2, 4, 7,	6,	 9,	8,		 11,	 10,	 12,	 14,	 0x80, 0x80, 0x80},
			{ 12, 0, 3, 2, 4, 7, 6,	9,	 8,	11,		10,		12,		14,		0x80, 0x80, 0x80, 0x80},
			{ 12, 1, 0, 3, 2, 4, 7,	6,	 8,	11,		10,		12,		14,		0x80, 0x80, 0x80, 0x80},
			{ 11, 0, 3, 2, 4, 7, 6,	8,	 11, 10,	 12,	 14,	 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 12, 1, 0, 2, 4, 7, 6,	9,	 8,	11,		10,		12,		14,		0x80, 0x80, 0x80, 0x80},
			{ 11, 0, 2, 4, 7, 6, 9,	8,	 11, 10,	 12,	 14,	 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 11, 1, 0, 2, 4, 7, 6,	8,	 11, 10,	 12,	 14,	 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 10, 0, 2, 4, 7, 6, 8,	11, 10, 12,		14,		0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 12, 1, 0, 3, 2, 4, 7,	6,	 9,	8,		 10,	 12,	 14,	 0x80, 0x80, 0x80, 0x80},
			{ 11, 0, 3, 2, 4, 7, 6,	9,	 8,	10,		12,		14,		0x80, 0x80, 0x80, 0x80, 0x80},
			{ 11, 1, 0, 3, 2, 4, 7,	6,	 8,	10,		12,		14,		0x80, 0x80, 0x80, 0x80, 0x80},
			{ 10, 0, 3, 2, 4, 7, 6,	8,	 10, 12,	 14,	 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 11, 1, 0, 2, 4, 7, 6,	9,	 8,	10,		12,		14,		0x80, 0x80, 0x80, 0x80, 0x80},
			{ 10, 0, 2, 4, 7, 6, 9,	8,	 10, 12,	 14,	 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 10, 1, 0, 2, 4, 7, 6,	8,	 10, 12,	 14,	 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 9,	 0, 2, 4, 7, 6, 8,	 10, 12, 14,	 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 14, 1, 0, 3, 2, 5, 4,	6,	 9,	8,		 11,	 10,	 13,	 12,	 14,	 0x80, 0x80},
			{ 13, 0, 3, 2, 5, 4, 6,	9,	 8,	11,		10,		13,		12,		14,		0x80, 0x80, 0x80},
			{ 13, 1, 0, 3, 2, 5, 4,	6,	 8,	11,		10,		13,		12,		14,		0x80, 0x80, 0x80},
			{ 12, 0, 3, 2, 5, 4, 6,	8,	 11, 10,	 13,	 12,	 14,	 0x80, 0x80, 0x80, 0x80},
			{ 13, 1, 0, 2, 5, 4, 6,	9,	 8,	11,		10,		13,		12,		14,		0x80, 0x80, 0x80},
			{ 12, 0, 2, 5, 4, 6, 9,	8,	 11, 10,	 13,	 12,	 14,	 0x80, 0x80, 0x80, 0x80},
			{ 12, 1, 0, 2, 5, 4, 6,	8,	 11, 10,	 13,	 12,	 14,	 0x80, 0x80, 0x80, 0x80},
			{ 11, 0, 2, 5, 4, 6, 8,	11, 10, 13,		12,		14,		0x80, 0x80, 0x80, 0x80, 0x80},
			{ 13, 1, 0, 3, 2, 5, 4,	6,	 9,	8,		 10,	 13,	 12,	 14,	 0x80, 0x80, 0x80},
			{ 12, 0, 3, 2, 5, 4, 6,	9,	 8,	10,		13,		12,		14,		0x80, 0x80, 0x80, 0x80},
			{ 12, 1, 0, 3, 2, 5, 4,	6,	 8,	10,		13,		12,		14,		0x80, 0x80, 0x80, 0x80},
			{ 11, 0, 3, 2, 5, 4, 6,	8,	 10, 13,	 12,	 14,	 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 12, 1, 0, 2, 5, 4, 6,	9,	 8,	10,		13,		12,		14,		0x80, 0x80, 0x80, 0x80},
			{ 11, 0, 2, 5, 4, 6, 9,	8,	 10, 13,	 12,	 14,	 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 11, 1, 0, 2, 5, 4, 6,	8,	 10, 13,	 12,	 14,	 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 10, 0, 2, 5, 4, 6, 8,	10, 13, 12,		14,		0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 13, 1, 0, 3, 2, 4, 6,	9,	 8,	11,		10,		13,		12,		14,		0x80, 0x80, 0x80},
			{ 12, 0, 3, 2, 4, 6, 9,	8,	 11, 10,	 13,	 12,	 14,	 0x80, 0x80, 0x80, 0x80},
			{ 12, 1, 0, 3, 2, 4, 6,	8,	 11, 10,	 13,	 12,	 14,	 0x80, 0x80, 0x80, 0x80},
			{ 11, 0, 3, 2, 4, 6, 8,	11, 10, 13,		12,		14,		0x80, 0x80, 0x80, 0x80, 0x80},
			{ 12, 1, 0, 2, 4, 6, 9,	8,	 11, 10,	 13,	 12,	 14,	 0x80, 0x80, 0x80, 0x80},
			{ 11, 0, 2, 4, 6, 9, 8,	11, 10, 13,		12,		14,		0x80, 0x80, 0x80, 0x80, 0x80},
			{ 11, 1, 0, 2, 4, 6, 8,	11, 10, 13,		12,		14,		0x80, 0x80, 0x80, 0x80, 0x80},
			{ 10, 0, 2, 4, 6, 8, 11, 10, 13, 12,	 14,	 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 12, 1, 0, 3, 2, 4, 6,	9,	 8,	10,		13,		12,		14,		0x80, 0x80, 0x80, 0x80},
			{ 11, 0, 3, 2, 4, 6, 9,	8,	 10, 13,	 12,	 14,	 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 11, 1, 0, 3, 2, 4, 6,	8,	 10, 13,	 12,	 14,	 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 10, 0, 3, 2, 4, 6, 8,	10, 13, 12,		14,		0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 11, 1, 0, 2, 4, 6, 9,	8,	 10, 13,	 12,	 14,	 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 10, 0, 2, 4, 6, 9, 8,	10, 13, 12,		14,		0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 10, 1, 0, 2, 4, 6, 8,	10, 13, 12,		14,		0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 9,	 0, 2, 4, 6, 8, 10, 13, 12, 14,		0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 13, 1, 0, 3, 2, 5, 4,	6,	 9,	8,		 11,	 10,	 12,	 14,	 0x80, 0x80, 0x80},
			{ 12, 0, 3, 2, 5, 4, 6,	9,	 8,	11,		10,		12,		14,		0x80, 0x80, 0x80, 0x80},
			{ 12, 1, 0, 3, 2, 5, 4,	6,	 8,	11,		10,		12,		14,		0x80, 0x80, 0x80, 0x80},
			{ 11, 0, 3, 2, 5, 4, 6,	8,	 11, 10,	 12,	 14,	 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 12, 1, 0, 2, 5, 4, 6,	9,	 8,	11,		10,		12,		14,		0x80, 0x80, 0x80, 0x80},
			{ 11, 0, 2, 5, 4, 6, 9,	8,	 11, 10,	 12,	 14,	 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 11, 1, 0, 2, 5, 4, 6,	8,	 11, 10,	 12,	 14,	 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 10, 0, 2, 5, 4, 6, 8,	11, 10, 12,		14,		0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 12, 1, 0, 3, 2, 5, 4,	6,	 9,	8,		 10,	 12,	 14,	 0x80, 0x80, 0x80, 0x80},
			{ 11, 0, 3, 2, 5, 4, 6,	9,	 8,	10,		12,		14,		0x80, 0x80, 0x80, 0x80, 0x80},
			{ 11, 1, 0, 3, 2, 5, 4,	6,	 8,	10,		12,		14,		0x80, 0x80, 0x80, 0x80, 0x80},
			{ 10, 0, 3, 2, 5, 4, 6,	8,	 10, 12,	 14,	 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 11, 1, 0, 2, 5, 4, 6,	9,	 8,	10,		12,		14,		0x80, 0x80, 0x80, 0x80, 0x80},
			{ 10, 0, 2, 5, 4, 6, 9,	8,	 10, 12,	 14,	 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 10, 1, 0, 2, 5, 4, 6,	8,	 10, 12,	 14,	 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 9,	 0, 2, 5, 4, 6, 8,	 10, 12, 14,	 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 12, 1, 0, 3, 2, 4, 6,	9,	 8,	11,		10,		12,		14,		0x80, 0x80, 0x80, 0x80},
			{ 11, 0, 3, 2, 4, 6, 9,	8,	 11, 10,	 12,	 14,	 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 11, 1, 0, 3, 2, 4, 6,	8,	 11, 10,	 12,	 14,	 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 10, 0, 3, 2, 4, 6, 8,	11, 10, 12,		14,		0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 11, 1, 0, 2, 4, 6, 9,	8,	 11, 10,	 12,	 14,	 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 10, 0, 2, 4, 6, 9, 8,	11, 10, 12,		14,		0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 10, 1, 0, 2, 4, 6, 8,	11, 10, 12,		14,		0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 9,	 0, 2, 4, 6, 8, 11, 10, 12, 14,		0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 11, 1, 0, 3, 2, 4, 6,	9,	 8,	10,		12,		14,		0x80, 0x80, 0x80, 0x80, 0x80},
			{ 10, 0, 3, 2, 4, 6, 9,	8,	 10, 12,	 14,	 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 10, 1, 0, 3, 2, 4, 6,	8,	 10, 12,	 14,	 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 9,	 0, 3, 2, 4, 6, 8,	 10, 12, 14,	 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 10, 1, 0, 2, 4, 6, 9,	8,	 10, 12,	 14,	 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 9,	 0, 2, 4, 6, 9, 8,	 10, 12, 14,	 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 9,	 1, 0, 2, 4, 6, 8,	 10, 12, 14,	 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 8,	 0, 2, 4, 6, 8, 10, 12, 14, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80}
		};

		// 1 byte for length, 16 bytes for mask
		const uint8_t __pack_1_2_3_utf16_to_utf8[256][17] = {
			{12,	2,		 3,		1,		 6,		7,		 5,		10,		11,		9,		 14,	 15,	 13,	 0x80, 0x80, 0x80, 0x80},
			{ 9,	 6,		7,		 5,		10,		11,		9,		 14,	 15,	 13,	 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 11, 3,		 1,		6,		 7,		5,		 10,	 11,	 9,		14,		15,		13,		0x80, 0x80, 0x80, 0x80, 0x80},
			{ 10, 0,		 6,		7,		 5,		10,		11,		9,		 14,	 15,	 13,	 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 9,	 2,		3,		 1,		10,		11,		9,		 14,	 15,	 13,	 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 6,	 10,	 11,	 9,		14,		15,		13,		0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 8,	 3,		1,		 10,	 11,	 9,		14,		15,		13,		0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 7,	 0,		10,		11,		9,		 14,	 15,	 13,	 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 11, 2,		 3,		1,		 7,		5,		 10,	 11,	 9,		14,		15,		13,		0x80, 0x80, 0x80, 0x80, 0x80},
			{ 8,	 7,		5,		 10,	 11,	 9,		14,		15,		13,		0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 10, 3,		 1,		7,		 5,		10,		11,		9,		 14,	 15,	 13,	 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 9,	 0,		7,		 5,		10,		11,		9,		 14,	 15,	 13,	 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 10, 2,		 3,		1,		 4,		10,		11,		9,		 14,	 15,	 13,	 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 7,	 4,		10,		11,		9,		 14,	 15,	 13,	 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 9,	 3,		1,		 4,		10,		11,		9,		 14,	 15,	 13,	 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 8,	 0,		4,		 10,	 11,	 9,		14,		15,		13,		0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 9,	 2,		3,		 1,		6,		 7,		5,		 14,	 15,	 13,	 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 6,	 6,		7,		 5,		14,		15,		13,		0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 8,	 3,		1,		 6,		7,		 5,		14,		15,		13,		0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 7,	 0,		6,		 7,		5,		 14,	 15,	 13,	 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 6,	 2,		3,		 1,		14,		15,		13,		0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 3,	 14,	 15,	 13,	 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 5,	 3,		1,		 14,	 15,	 13,	 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 4,	 0,		14,		15,		13,		0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 8,	 2,		3,		 1,		7,		 5,		14,		15,		13,		0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 5,	 7,		5,		 14,	 15,	 13,	 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 7,	 3,		1,		 7,		5,		 14,	 15,	 13,	 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 6,	 0,		7,		 5,		14,		15,		13,		0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 7,	 2,		3,		 1,		4,		 14,	 15,	 13,	 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 4,	 4,		14,		15,		13,		0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 6,	 3,		1,		 4,		14,		15,		13,		0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 5,	 0,		4,		 14,	 15,	 13,	 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 11, 2,		 3,		1,		 6,		7,		 5,		11,		9,		 14,	 15,	 13,	 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 8,	 6,		7,		 5,		11,		9,		 14,	 15,	 13,	 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 10, 3,		 1,		6,		 7,		5,		 11,	 9,		14,		15,		13,		0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 9,	 0,		6,		 7,		5,		 11,	 9,		14,		15,		13,		0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 8,	 2,		3,		 1,		11,		9,		 14,	 15,	 13,	 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 5,	 11,	 9,		14,		15,		13,		0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 7,	 3,		1,		 11,	 9,		14,		15,		13,		0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 6,	 0,		11,		9,		 14,	 15,	 13,	 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 10, 2,		 3,		1,		 7,		5,		 11,	 9,		14,		15,		13,		0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 7,	 7,		5,		 11,	 9,		14,		15,		13,		0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 9,	 3,		1,		 7,		5,		 11,	 9,		14,		15,		13,		0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 8,	 0,		7,		 5,		11,		9,		 14,	 15,	 13,	 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 9,	 2,		3,		 1,		4,		 11,	 9,		14,		15,		13,		0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 6,	 4,		11,		9,		 14,	 15,	 13,	 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 8,	 3,		1,		 4,		11,		9,		 14,	 15,	 13,	 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 7,	 0,		4,		 11,	 9,		14,		15,		13,		0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 10, 2,		 3,		1,		 6,		7,		 5,		8,		 14,	 15,	 13,	 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 7,	 6,		7,		 5,		8,		 14,	 15,	 13,	 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 9,	 3,		1,		 6,		7,		 5,		8,		 14,	 15,	 13,	 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 8,	 0,		6,		 7,		5,		 8,		14,		15,		13,		0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 7,	 2,		3,		 1,		8,		 14,	 15,	 13,	 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 4,	 8,		14,		15,		13,		0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 6,	 3,		1,		 8,		14,		15,		13,		0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 5,	 0,		8,		 14,	 15,	 13,	 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 9,	 2,		3,		 1,		7,		 5,		8,		 14,	 15,	 13,	 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 6,	 7,		5,		 8,		14,		15,		13,		0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 8,	 3,		1,		 7,		5,		 8,		14,		15,		13,		0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 7,	 0,		7,		 5,		8,		 14,	 15,	 13,	 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 8,	 2,		3,		 1,		4,		 8,		14,		15,		13,		0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 5,	 4,		8,		 14,	 15,	 13,	 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 7,	 3,		1,		 4,		8,		 14,	 15,	 13,	 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 6,	 0,		4,		 8,		14,		15,		13,		0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 9,	 2,		3,		 1,		6,		 7,		5,		 10,	 11,	 9,		0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 6,	 6,		7,		 5,		10,		11,		9,		 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 8,	 3,		1,		 6,		7,		 5,		10,		11,		9,		 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 7,	 0,		6,		 7,		5,		 10,	 11,	 9,		0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 6,	 2,		3,		 1,		10,		11,		9,		 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 3,	 10,	 11,	 9,		0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 5,	 3,		1,		 10,	 11,	 9,		0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 4,	 0,		10,		11,		9,		 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 8,	 2,		3,		 1,		7,		 5,		10,		11,		9,		 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 5,	 7,		5,		 10,	 11,	 9,		0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 7,	 3,		1,		 7,		5,		 10,	 11,	 9,		0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 6,	 0,		7,		 5,		10,		11,		9,		 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 7,	 2,		3,		 1,		4,		 10,	 11,	 9,		0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 4,	 4,		10,		11,		9,		 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 6,	 3,		1,		 4,		10,		11,		9,		 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 5,	 0,		4,		 10,	 11,	 9,		0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 6,	 2,		3,		 1,		6,		 7,		5,		 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 3,	 6,		7,		 5,		0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 5,	 3,		1,		 6,		7,		 5,		0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 4,	 0,		6,		 7,		5,		 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 3,	 2,		3,		 1,		0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 0,	 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 2,	 3,		1,		 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 1,	 0,		0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 5,	 2,		3,		 1,		7,		 5,		0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 2,	 7,		5,		 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 4,	 3,		1,		 7,		5,		 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 3,	 0,		7,		 5,		0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 4,	 2,		3,		 1,		4,		 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 1,	 4,		0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 3,	 3,		1,		 4,		0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 2,	 0,		4,		 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 8,	 2,		3,		 1,		6,		 7,		5,		 11,	 9,		0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 5,	 6,		7,		 5,		11,		9,		 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 7,	 3,		1,		 6,		7,		 5,		11,		9,		 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 6,	 0,		6,		 7,		5,		 11,	 9,		0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 5,	 2,		3,		 1,		11,		9,		 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 2,	 11,	 9,		0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 4,	 3,		1,		 11,	 9,		0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 3,	 0,		11,		9,		 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 7,	 2,		3,		 1,		7,		 5,		11,		9,		 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 4,	 7,		5,		 11,	 9,		0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 6,	 3,		1,		 7,		5,		 11,	 9,		0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 5,	 0,		7,		 5,		11,		9,		 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 6,	 2,		3,		 1,		4,		 11,	 9,		0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 3,	 4,		11,		9,		 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 5,	 3,		1,		 4,		11,		9,		 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 4,	 0,		4,		 11,	 9,		0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 7,	 2,		3,		 1,		6,		 7,		5,		 8,		0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 4,	 6,		7,		 5,		8,		 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 6,	 3,		1,		 6,		7,		 5,		8,		 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 5,	 0,		6,		 7,		5,		 8,		0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 4,	 2,		3,		 1,		8,		 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 1,	 8,		0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 3,	 3,		1,		 8,		0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 2,	 0,		8,		 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 6,	 2,		3,		 1,		7,		 5,		8,		 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 3,	 7,		5,		 8,		0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 5,	 3,		1,		 7,		5,		 8,		0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 4,	 0,		7,		 5,		8,		 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 5,	 2,		3,		 1,		4,		 8,		0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 2,	 4,		8,		 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 4,	 3,		1,		 4,		8,		 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 3,	 0,		4,		 8,		0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 11, 2,		 3,		1,		 6,		7,		 5,		10,		11,		9,		 15,	 13,	 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 8,	 6,		7,		 5,		10,		11,		9,		 15,	 13,	 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 10, 3,		 1,		6,		 7,		5,		 10,	 11,	 9,		15,		13,		0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 9,	 0,		6,		 7,		5,		 10,	 11,	 9,		15,		13,		0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 8,	 2,		3,		 1,		10,		11,		9,		 15,	 13,	 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 5,	 10,	 11,	 9,		15,		13,		0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 7,	 3,		1,		 10,	 11,	 9,		15,		13,		0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 6,	 0,		10,		11,		9,		 15,	 13,	 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 10, 2,		 3,		1,		 7,		5,		 10,	 11,	 9,		15,		13,		0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 7,	 7,		5,		 10,	 11,	 9,		15,		13,		0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 9,	 3,		1,		 7,		5,		 10,	 11,	 9,		15,		13,		0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 8,	 0,		7,		 5,		10,		11,		9,		 15,	 13,	 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 9,	 2,		3,		 1,		4,		 10,	 11,	 9,		15,		13,		0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 6,	 4,		10,		11,		9,		 15,	 13,	 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 8,	 3,		1,		 4,		10,		11,		9,		 15,	 13,	 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 7,	 0,		4,		 10,	 11,	 9,		15,		13,		0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 8,	 2,		3,		 1,		6,		 7,		5,		 15,	 13,	 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 5,	 6,		7,		 5,		15,		13,		0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 7,	 3,		1,		 6,		7,		 5,		15,		13,		0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 6,	 0,		6,		 7,		5,		 15,	 13,	 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 5,	 2,		3,		 1,		15,		13,		0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 2,	 15,	 13,	 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 4,	 3,		1,		 15,	 13,	 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 3,	 0,		15,		13,		0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 7,	 2,		3,		 1,		7,		 5,		15,		13,		0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 4,	 7,		5,		 15,	 13,	 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 6,	 3,		1,		 7,		5,		 15,	 13,	 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 5,	 0,		7,		 5,		15,		13,		0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 6,	 2,		3,		 1,		4,		 15,	 13,	 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 3,	 4,		15,		13,		0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 5,	 3,		1,		 4,		15,		13,		0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 4,	 0,		4,		 15,	 13,	 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 10, 2,		 3,		1,		 6,		7,		 5,		11,		9,		 15,	 13,	 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 7,	 6,		7,		 5,		11,		9,		 15,	 13,	 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 9,	 3,		1,		 6,		7,		 5,		11,		9,		 15,	 13,	 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 8,	 0,		6,		 7,		5,		 11,	 9,		15,		13,		0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 7,	 2,		3,		 1,		11,		9,		 15,	 13,	 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 4,	 11,	 9,		15,		13,		0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 6,	 3,		1,		 11,	 9,		15,		13,		0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 5,	 0,		11,		9,		 15,	 13,	 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 9,	 2,		3,		 1,		7,		 5,		11,		9,		 15,	 13,	 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 6,	 7,		5,		 11,	 9,		15,		13,		0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 8,	 3,		1,		 7,		5,		 11,	 9,		15,		13,		0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 7,	 0,		7,		 5,		11,		9,		 15,	 13,	 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 8,	 2,		3,		 1,		4,		 11,	 9,		15,		13,		0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 5,	 4,		11,		9,		 15,	 13,	 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 7,	 3,		1,		 4,		11,		9,		 15,	 13,	 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 6,	 0,		4,		 11,	 9,		15,		13,		0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 9,	 2,		3,		 1,		6,		 7,		5,		 8,		15,		13,		0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 6,	 6,		7,		 5,		8,		 15,	 13,	 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 8,	 3,		1,		 6,		7,		 5,		8,		 15,	 13,	 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 7,	 0,		6,		 7,		5,		 8,		15,		13,		0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 6,	 2,		3,		 1,		8,		 15,	 13,	 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 3,	 8,		15,		13,		0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 5,	 3,		1,		 8,		15,		13,		0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 4,	 0,		8,		 15,	 13,	 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 8,	 2,		3,		 1,		7,		 5,		8,		 15,	 13,	 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 5,	 7,		5,		 8,		15,		13,		0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 7,	 3,		1,		 7,		5,		 8,		15,		13,		0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 6,	 0,		7,		 5,		8,		 15,	 13,	 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 7,	 2,		3,		 1,		4,		 8,		15,		13,		0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 4,	 4,		8,		 15,	 13,	 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 6,	 3,		1,		 4,		8,		 15,	 13,	 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 5,	 0,		4,		 8,		15,		13,		0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 10, 2,		 3,		1,		 6,		7,		 5,		10,		11,		9,		 12,	 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 7,	 6,		7,		 5,		10,		11,		9,		 12,	 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 9,	 3,		1,		 6,		7,		 5,		10,		11,		9,		 12,	 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 8,	 0,		6,		 7,		5,		 10,	 11,	 9,		12,		0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 7,	 2,		3,		 1,		10,		11,		9,		 12,	 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 4,	 10,	 11,	 9,		12,		0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 6,	 3,		1,		 10,	 11,	 9,		12,		0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 5,	 0,		10,		11,		9,		 12,	 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 9,	 2,		3,		 1,		7,		 5,		10,		11,		9,		 12,	 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 6,	 7,		5,		 10,	 11,	 9,		12,		0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 8,	 3,		1,		 7,		5,		 10,	 11,	 9,		12,		0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 7,	 0,		7,		 5,		10,		11,		9,		 12,	 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 8,	 2,		3,		 1,		4,		 10,	 11,	 9,		12,		0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 5,	 4,		10,		11,		9,		 12,	 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 7,	 3,		1,		 4,		10,		11,		9,		 12,	 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 6,	 0,		4,		 10,	 11,	 9,		12,		0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 7,	 2,		3,		 1,		6,		 7,		5,		 12,	 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 4,	 6,		7,		 5,		12,		0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 6,	 3,		1,		 6,		7,		 5,		12,		0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 5,	 0,		6,		 7,		5,		 12,	 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 4,	 2,		3,		 1,		12,		0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 1,	 12,	 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 3,	 3,		1,		 12,	 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 2,	 0,		12,		0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 6,	 2,		3,		 1,		7,		 5,		12,		0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 3,	 7,		5,		 12,	 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 5,	 3,		1,		 7,		5,		 12,	 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 4,	 0,		7,		 5,		12,		0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 5,	 2,		3,		 1,		4,		 12,	 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 2,	 4,		12,		0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 4,	 3,		1,		 4,		12,		0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 3,	 0,		4,		 12,	 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 9,	 2,		3,		 1,		6,		 7,		5,		 11,	 9,		12,		0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 6,	 6,		7,		 5,		11,		9,		 12,	 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 8,	 3,		1,		 6,		7,		 5,		11,		9,		 12,	 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 7,	 0,		6,		 7,		5,		 11,	 9,		12,		0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 6,	 2,		3,		 1,		11,		9,		 12,	 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 3,	 11,	 9,		12,		0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 5,	 3,		1,		 11,	 9,		12,		0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 4,	 0,		11,		9,		 12,	 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 8,	 2,		3,		 1,		7,		 5,		11,		9,		 12,	 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 5,	 7,		5,		 11,	 9,		12,		0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 7,	 3,		1,		 7,		5,		 11,	 9,		12,		0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 6,	 0,		7,		 5,		11,		9,		 12,	 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 7,	 2,		3,		 1,		4,		 11,	 9,		12,		0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 4,	 4,		11,		9,		 12,	 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 6,	 3,		1,		 4,		11,		9,		 12,	 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 5,	 0,		4,		 11,	 9,		12,		0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 8,	 2,		3,		 1,		6,		 7,		5,		 8,		12,		0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 5,	 6,		7,		 5,		8,		 12,	 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 7,	 3,		1,		 6,		7,		 5,		8,		 12,	 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 6,	 0,		6,		 7,		5,		 8,		12,		0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 5,	 2,		3,		 1,		8,		 12,	 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 2,	 8,		12,		0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 4,	 3,		1,		 8,		12,		0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 3,	 0,		8,		 12,	 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 7,	 2,		3,		 1,		7,		 5,		8,		 12,	 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 4,	 7,		5,		 8,		12,		0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 6,	 3,		1,		 7,		5,		 8,		12,		0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 5,	 0,		7,		 5,		8,		 12,	 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 6,	 2,		3,		 1,		4,		 8,		12,		0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 3,	 4,		8,		 12,	 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 5,	 3,		1,		 4,		8,		 12,	 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
			{ 4,	 0,		4,		 8,		12,		0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80}
		};

		pf_hint_nodiscard pf_decl_constexpr pf_decl_inline char_error_t
		__convert_with_errors(
		 const u16char_t *__in,
		 size_t __len,
		 u8char_t *__out) pf_attr_noexcept
		{
			size_t p				= 0;
			u8char_t *start = __out;
			while(p < __len)
			{
				// try to convert the next block of 8 ASCII characters
				if(p + 4 <= __len)
				{	 // if it is safe to read 8 more bytes, check that they are ascii
					uint64_t v;
					memcpy(&v, &__in[p], sizeof(uint64_t));
					if((v & 0xFF'80'FF'80'FF'80'FF'80) == 0)
					{
						size_t fp = p + 4;
						while(p < fp)
						{
							*__out++ = u8char_t(__in[p]);
							++p;
						}
						continue;
					}
				}
				uint16_t w = uint8_t(__in[p]);
				if((w & 0xFF'80) == 0)
				{
					// will generate one UTF-8 bytes
					*__out++ = u8char_t(w);
					++p;
				}
				else if((w & 0xF8'00) == 0)
				{
					// will generate two UTF-8 bytes
					// we have 0b110XXXXX 0b10XXXXXX
					*__out++ = u8char_t((w >> 6) | 0b11000000);
					*__out++ = u8char_t((w & 0b111111) | 0b10000000);
					++p;
				}
				else if((w & 0xF8'00) != 0xD8'00)
				{
					// will generate three UTF-8 bytes
					// we have 0b1110XXXX 0b10XXXXXX 0b10XXXXXX
					*__out++ = u8char_t((w >> 12) | 0b11100000);
					*__out++ = u8char_t(((w >> 6) & 0b111111) | 0b10000000);
					*__out++ = u8char_t((w & 0b111111) | 0b10000000);
					++p;
				}
				else
				{
					// must be a surrogate pair
					if(p + 1 >= __len) return char_error_t { char_error_code::surrogate, p };
					uint16_t diff = uint16_t(w - 0xD8'00);
					if(diff > 0x3'FF) return char_error_t { char_error_code::surrogate, p };
					uint16_t nw		 = uint8_t(__in[p + 1]);
					uint16_t diff2 = uint16_t(nw - 0xDC'00);
					if(diff2 > 0x3'FF) return char_error_t { char_error_code::surrogate, p };
					uint32_t value = (diff << 10) + diff2 + 0x1'00'00;
					// will generate four UTF-8 bytes
					// we have 0b11110XXX 0b10XXXXXX 0b10XXXXXX 0b10XXXXXX
					*__out++			 = u8char_t((value >> 18) | 0b11110000);
					*__out++			 = u8char_t(((value >> 12) & 0b111111) | 0b10000000);
					*__out++			 = u8char_t(((value >> 6) & 0b111111) | 0b10000000);
					*__out++			 = u8char_t((value & 0b111111) | 0b10000000);
					p							+= 2;
				}
			}
			return char_error_t { char_error_code::success, distof(start, __out) };
		}
		pf_hint_nodiscard pf_decl_inline pair<char_error_t, u8char_t *>
		__convert_with_errors_avx2(
		 const u16char_t *__in,
		 size_t __len,
		 u8char_t *__out) pf_attr_noexcept
		{
			const u16char_t *start = __in;
			const u16char_t *end	 = __in + __len;

			const m256i64_t v_0000		 = _mm256_setzero_si256();
			const m256i64_t v_f800		 = _mm256_set1_epi16((int16_t)0xf8'00);
			const m256i64_t v_d800		 = _mm256_set1_epi16((int16_t)0xd8'00);
			const m256i64_t v_c080		 = _mm256_set1_epi16((int16_t)0xc0'80);
			const size_t safety_margin = 12;	// to avoid overruns, see issue https://github.com/simdutf/simdutf/issues/92

			while(__in + 16 + safety_margin <= end)
			{
				m256i64_t in					 = _mm256_loadu_si256(union_cast<m256i64_t *>(__in));
				// a single 16-bit UTF-16 word can yield 1, 2 or 3 UTF-8 bytes
				const m256i64_t v_ff80 = _mm256_set1_epi16((int16_t)0xff'80);
				if(_mm256_testz_si256(in, v_ff80))
				{	 // ASCII fast path!!!!
					// 1. pack the bytes
					const m128i64_t utf8_packed = _mm_packus_epi16(_mm256_castsi256_si128(in), _mm256_extractf128_si256(in, 1));
					// 2. store (16 bytes)
					_mm_storeu_si128(union_cast<m128i64_t *>(__out), utf8_packed);
					// 3. adjust pointers
					__in	+= 16;
					__out += 16;
					continue;	 // we are done for this round!
				}
				// no bits set above 7th bit
				const m256i64_t one_byte_bytemask = _mm256_cmpeq_epi16(_mm256_and_si256(in, v_ff80), v_0000);
				const uint32_t one_byte_bitmask		= union_cast<uint32_t>(_mm256_movemask_epi8(one_byte_bytemask));

				// no bits set above 11th bit
				const m256i64_t one_or_two_bytes_bytemask = _mm256_cmpeq_epi16(_mm256_and_si256(in, v_f800), v_0000);
				const uint32_t one_or_two_bytes_bitmask		= union_cast<uint32_t>(_mm256_movemask_epi8(one_or_two_bytes_bytemask));
				if(one_or_two_bytes_bitmask == 0xff'ff'ff'ff)
				{
					// 1. prepare 2-byte values
					// input 16-bit word : [0000|0aaa|aabb|bbbb] x 8
					// expected output   : [110a|aaaa|10bb|bbbb] x 8
					const m256i64_t v_1f00 = _mm256_set1_epi16((int16_t)0x1f'00);
					const m256i64_t v_003f = _mm256_set1_epi16((int16_t)0x00'3f);

					// t0 = [000a|aaaa|bbbb|bb00]
					const m256i64_t t0 = _mm256_slli_epi16(in, 2);
					// t1 = [000a|aaaa|0000|0000]
					const m256i64_t t1 = _mm256_and_si256(t0, v_1f00);
					// t2 = [0000|0000|00bb|bbbb]
					const m256i64_t t2 = _mm256_and_si256(in, v_003f);
					// t3 = [000a|aaaa|00bb|bbbb]
					const m256i64_t t3 = _mm256_or_si256(t1, t2);
					// t4 = [110a|aaaa|10bb|bbbb]
					const m256i64_t t4 = _mm256_or_si256(t3, v_c080);

					// 2. merge ASCII and 2-byte codewords
					const m256i64_t utf8_unpacked = _mm256_blendv_epi8(t4, in, one_byte_bytemask);

					// 3. prepare bitmask for 8-bit lookup
					const uint32_t M0 = one_byte_bitmask & 0x55'55'55'55;
					const uint32_t M1 = M0 >> 7;
					const uint32_t M2 = (M1 | M0) & 0x00'ff'00'ff;
					// 4. pack the bytes

					const uint8_t *row	 = &__pack_1_2_utf16_to_utf8[uint8_t(M2)][0];
					const uint8_t *row_2 = &__pack_1_2_utf16_to_utf8[uint8_t(M2 >> 16)][0];

					const m128i64_t shuffle		= _mm_loadu_si128(union_cast<m128i64_t *>(row + 1));
					const m128i64_t shuffle_2 = _mm_loadu_si128(union_cast<m128i64_t *>(row_2 + 1));

					const m256i64_t utf8_packed = _mm256_shuffle_epi8(utf8_unpacked, _mm256_setr_m128i(shuffle, shuffle_2));
					// 5. store bytes
					_mm_storeu_si128(union_cast<m128i64_t *>(__out), _mm256_castsi256_si128(utf8_packed));
					__out += row[0];
					_mm_storeu_si128(union_cast<m128i64_t *>(__out), _mm256_extractf128_si256(utf8_packed, 1));
					__out += row_2[0];

					// 6. adjust pointers
					__in += 16;
					continue;
				}
				// 1. Check if there are any surrogate word in the input chunk.
				//    We have also deal with situation when there is a surrogate word
				//    at the end of a chunk.
				const m256i64_t surrogates_bytemask = _mm256_cmpeq_epi16(_mm256_and_si256(in, v_f800), v_d800);

				// bitmask = 0x0000 if there are no surrogates
				//         = 0xc000 if the last word is a surrogate
				const uint32_t surrogates_bitmask = union_cast<uint32_t>(_mm256_movemask_epi8(surrogates_bytemask));
				// It might seem like checking for surrogates_bitmask == 0xc000 could help. However,
				// it is likely an uncommon occurrence.
				if(surrogates_bitmask == 0x00'00'00'00)
				{
					// case: words from register produce either 1, 2 or 3 UTF-8 bytes
					const m256i64_t dup_even = _mm256_setr_epi16(0x00'00, 0x02'02, 0x04'04, 0x06'06, 0x08'08, 0x0a'0a, 0x0c'0c, 0x0e'0e, 0x00'00, 0x02'02, 0x04'04, 0x06'06, 0x08'08, 0x0a'0a, 0x0c'0c, 0x0e'0e);

					/* In this branch we handle three cases:
				 1. [0000|0000|0ccc|cccc] => [0ccc|cccc]                           - single UFT-8 byte
				 2. [0000|0bbb|bbcc|cccc] => [110b|bbbb], [10cc|cccc]              - two UTF-8 bytes
				 3. [aaaa|bbbb|bbcc|cccc] => [1110|aaaa], [10bb|bbbb], [10cc|cccc] - three UTF-8 bytes

				We expand the input word (16-bit) into two words (32-bit), thus
				we have room for four bytes. However, we need five distinct bit
				layouts. Note that the last byte in cases #2 and #3 is the same.

				We precompute byte 1 for case #1 and the common byte for cases #2 & #3
				in register t2.

				We precompute byte 1 for case #3 and -- **conditionally** -- precompute
				either byte 1 for case #2 or byte 2 for case #3. Note that they
				differ by exactly one bit.

				Finally from these two words we build proper UTF-8 sequence, taking
				into account the case (i.e, the number of bytes to write).
			*/
					/**
					 * Given [aaaa|bbbb|bbcc|cccc] our goal is to produce:
					 * t2 => [0ccc|cccc] [10cc|cccc]
					 * s4 => [1110|aaaa] ([110b|bbbb] OR [10bb|bbbb])
					 */
					// [aaaa|bbbb|bbcc|cccc] => [bbcc|cccc|bbcc|cccc]
					const m256i64_t t0 = _mm256_shuffle_epi8(in, dup_even);
					// [bbcc|cccc|bbcc|cccc] => [00cc|cccc|0bcc|cccc]
					const m256i64_t t1 = _mm256_and_si256(t0, _mm256_set1_epi16(union_cast<uint16_t>(0b00111111'01111111)));
					// [00cc|cccc|0bcc|cccc] => [10cc|cccc|0bcc|cccc]
					const m256i64_t t2 = _mm256_or_si256(t1, _mm256_set1_epi16(union_cast<uint16_t>(0b10000000'00000000)));

					// [aaaa|bbbb|bbcc|cccc] =>  [0000|aaaa|bbbb|bbcc]
					const m256i64_t s0 = _mm256_srli_epi16(in, 4);
					// [0000|aaaa|bbbb|bbcc] => [0000|aaaa|bbbb|bb00]
					const m256i64_t s1 = _mm256_and_si256(s0, _mm256_set1_epi16(union_cast<uint16_t>(0b00001111'11111100)));
					// [0000|aaaa|bbbb|bb00] => [00bb|bbbb|0000|aaaa]
					const m256i64_t s2 = _mm256_maddubs_epi16(s1, _mm256_set1_epi16(union_cast<uint16_t>(0x01'40)));
					// [00bb|bbbb|0000|aaaa] => [11bb|bbbb|1110|aaaa]
					const m256i64_t s3 = _mm256_or_si256(s2, _mm256_set1_epi16(union_cast<uint16_t>(0b11000000'11100000)));
					const m256i64_t m0 = _mm256_andnot_si256(one_or_two_bytes_bytemask, _mm256_set1_epi16(union_cast<uint16_t>(0b01000000'00000000)));
					const m256i64_t s4 = _mm256_xor_si256(s3, m0);

					// 4. expand words 16-bit => 32-bit
					const m256i64_t out0 = _mm256_unpacklo_epi16(t2, s4);
					const m256i64_t out1 = _mm256_unpackhi_epi16(t2, s4);

					// 5. compress 32-bit words into 1, 2 or 3 bytes -- 2 x shuffle
					const uint32_t mask			 = (one_byte_bitmask & 0x55'55'55'55) | (one_or_two_bytes_bitmask & 0xaa'aa'aa'aa);
					const uint8_t mask0			 = uint8_t(mask);
					const uint8_t *row0			 = &__pack_1_2_3_utf16_to_utf8[mask0][0];
					const m128i64_t shuffle0 = _mm_loadu_si128(union_cast<m128i64_t *>(row0 + 1));
					const m128i64_t utf8_0	 = _mm_shuffle_epi8(_mm256_castsi256_si128(out0), shuffle0);

					const uint8_t mask1			 = uint8_t(mask >> 8);
					const uint8_t *row1			 = &__pack_1_2_3_utf16_to_utf8[mask1][0];
					const m128i64_t shuffle1 = _mm_loadu_si128(union_cast<m128i64_t *>(row1 + 1));
					const m128i64_t utf8_1	 = _mm_shuffle_epi8(_mm256_castsi256_si128(out1), shuffle1);

					const uint8_t mask2			 = uint8_t(mask >> 16);
					const uint8_t *row2			 = &__pack_1_2_3_utf16_to_utf8[mask2][0];
					const m128i64_t shuffle2 = _mm_loadu_si128(union_cast<m128i64_t *>(row2 + 1));
					const m128i64_t utf8_2	 = _mm_shuffle_epi8(_mm256_extractf128_si256(out0, 1), shuffle2);


					const uint8_t mask3			 = uint8_t(mask >> 24);
					const uint8_t *row3			 = &__pack_1_2_3_utf16_to_utf8[mask3][0];
					const m128i64_t shuffle3 = _mm_loadu_si128(union_cast<m128i64_t *>(row3 + 1));
					const m128i64_t utf8_3	 = _mm_shuffle_epi8(_mm256_extractf128_si256(out1, 1), shuffle3);

					_mm_storeu_si128(union_cast<m128i64_t *>(__out), utf8_0);
					__out += row0[0];
					_mm_storeu_si128(union_cast<m128i64_t *>(__out), utf8_1);
					__out += row1[0];
					_mm_storeu_si128(union_cast<m128i64_t *>(__out), utf8_2);
					__out += row2[0];
					_mm_storeu_si128(union_cast<m128i64_t *>(__out), utf8_3);
					__out += row3[0];
					__in	+= 16;
					// surrogate pair(s) in a register
				}
				else
				{
					// Let us do a scalar fallback.
					// It may seem wasteful to use scalar code, but being efficient with SIMD
					// in the presence of surrogate pairs may require non-trivial tables.
					size_t forward = 15;
					size_t k			 = 0;
					if(size_t(end - __in) < forward + 1) { forward = size_t(end - __in - 1); }
					for(; k < forward; ++k)
					{
						uint16_t word = uint16_t(__in[k]);
						if((word & 0xFF'80) == 0)
						{
							*__out++ = u8char_t(word);
						}
						else if((word & 0xF8'00) == 0)
						{
							*__out++ = u8char_t((word >> 6) | 0b11000000);
							*__out++ = u8char_t((word & 0b111111) | 0b10000000);
						}
						else if((word & 0xF8'00) != 0xD8'00)
						{
							*__out++ = u8char_t((word >> 12) | 0b11100000);
							*__out++ = u8char_t(((word >> 6) & 0b111111) | 0b10000000);
							*__out++ = u8char_t((word & 0b111111) | 0b10000000);
						}
						else
						{
							// must be a surrogate pair
							uint16_t diff			 = uint16_t(word - 0xD8'00);
							uint16_t next_word = uint16_t(__in[k + 1]);
							++k;
							uint16_t diff2 = uint16_t(next_word - 0xDC'00);
							if((diff | diff2) > 0x3'FF) return make_pair(char_error_t { char_error_code::surrogate, __in - start + k - 1 }, __out);
							uint32_t value = (diff << 10) + diff2 + 0x1'00'00;
							*__out++			 = u8char_t((value >> 18) | 0b11110000);
							*__out++			 = u8char_t(((value >> 12) & 0b111111) | 0b10000000);
							*__out++			 = u8char_t(((value >> 6) & 0b111111) | 0b10000000);
							*__out++			 = u8char_t((value & 0b111111) | 0b10000000);
						}
					}
					__in += k;
				}
			}	 // while
			return make_pair(char_error_t { char_error_code::success, union_cast<size_t>(__in - start) }, __out);
		}
		pf_hint_nodiscard pf_decl_inline pair<char_error_t, u8char_t *>
		__convert_with_errors_sse2(
		 const u16char_t *__in,
		 size_t __len,
		 u8char_t *__out) pf_attr_noexcept
		{
			const u16char_t *start = __in;
			const u16char_t *end	 = __in + __len;

			const m128i64_t v_0000		 = _mm_setzero_si128();
			const m128i64_t v_f800		 = _mm_set1_epi16((int16_t)0xf8'00);
			const m128i64_t v_d800		 = _mm_set1_epi16((int16_t)0xd8'00);
			const m128i64_t v_c080		 = _mm_set1_epi16((int16_t)0xc0'80);
			const size_t safety_margin = 12;	// to avoid overruns, see issue https://github.com/simdutf/simdutf/issues/92

			while(__in + 16 + safety_margin <= end)
			{
				m128i64_t in					 = _mm_loadu_si128(union_cast<m128i64_t *>(__in));
				// a single 16-bit UTF-16 word can yield 1, 2 or 3 UTF-8 bytes
				const m128i64_t v_ff80 = _mm_set1_epi16((int16_t)0xff'80);
				if(_mm_testz_si128(in, v_ff80))
				{	 // ASCII fast path!!!!
					m128i64_t nextin = _mm_loadu_si128(union_cast<m128i64_t *>(__in + 1));
					if(!_mm_testz_si128(nextin, v_ff80))
					{
						// 1. pack the bytes
						// obviously suboptimal.
						const m128i64_t utf8_packed = _mm_packus_epi16(in, in);
						// 2. store (16 bytes)
						_mm_storeu_si128(union_cast<m128i64_t *>(__out), utf8_packed);
						// 3. adjust pointers
						__in	+= 8;
						__out += 8;
						in		 = nextin;
					}
					else
					{
						// 1. pack the bytes
						// obviously suboptimal.
						const m128i64_t utf8_packed = _mm_packus_epi16(in, nextin);
						// 2. store (16 bytes)
						_mm_storeu_si128(union_cast<m128i64_t *>(__out), utf8_packed);
						// 3. adjust pointers
						__in	+= 16;
						__out += 16;
						continue;	 // we are done for this round!
					}
				}

				// no bits set above 7th bit
				const m128i64_t one_byte_bytemask = _mm_cmpeq_epi16(_mm_and_si128(in, v_ff80), v_0000);
				const uint16_t one_byte_bitmask		= union_cast<uint16_t>(_mm_movemask_epi8(one_byte_bytemask));

				// no bits set above 11th bit
				const m128i64_t one_or_two_bytes_bytemask = _mm_cmpeq_epi16(_mm_and_si128(in, v_f800), v_0000);
				const uint16_t one_or_two_bytes_bitmask		= union_cast<uint16_t>(_mm_movemask_epi8(one_or_two_bytes_bytemask));

				if(one_or_two_bytes_bitmask == 0xff'ff)
				{
					// 1. prepare 2-byte values
					// input 16-bit word : [0000|0aaa|aabb|bbbb] x 8
					// expected output   : [110a|aaaa|10bb|bbbb] x 8
					const m128i64_t v_1f00 = _mm_set1_epi16((int16_t)0x1f'00);
					const m128i64_t v_003f = _mm_set1_epi16((int16_t)0x00'3f);

					// t0 = [000a|aaaa|bbbb|bb00]
					const m128i64_t t0 = _mm_slli_epi16(in, 2);
					// t1 = [000a|aaaa|0000|0000]
					const m128i64_t t1 = _mm_and_si128(t0, v_1f00);
					// t2 = [0000|0000|00bb|bbbb]
					const m128i64_t t2 = _mm_and_si128(in, v_003f);
					// t3 = [000a|aaaa|00bb|bbbb]
					const m128i64_t t3 = _mm_or_si128(t1, t2);
					// t4 = [110a|aaaa|10bb|bbbb]
					const m128i64_t t4 = _mm_or_si128(t3, v_c080);

					// 2. merge ASCII and 2-byte codewords
					const m128i64_t utf8_unpacked = _mm_blendv_epi8(t4, in, one_byte_bytemask);

					// 3. prepare bitmask for 8-bit lookup
					//    one_byte_bitmask = hhggffeeddccbbaa -- the bits are doubled (h - MSB, a - LSB)
					const uint16_t m0						= one_byte_bitmask & 0x55'55;			// m0 = 0h0g0f0e0d0c0b0a
					const uint16_t m1						= union_cast<uint16_t>(m0 >> 7);	// m1 = 00000000h0g0f0e0
					const uint8_t m2						= uint8_t((m0 | m1) & 0xff);			// m2 =         hdgcfbea
					// 4. pack the bytes
					const uint8_t *row					= &__pack_1_2_utf16_to_utf8[m2][0];
					const m128i64_t shuffle			= _mm_loadu_si128(union_cast<m128i64_t *>(row + 1));
					const m128i64_t utf8_packed = _mm_shuffle_epi8(utf8_unpacked, shuffle);

					// 5. store bytes
					_mm_storeu_si128(union_cast<m128i64_t *>(__out), utf8_packed);

					// 6. adjust pointers
					__in	+= 8;
					__out += row[0];
					continue;
				}

				// 1. Check if there are any surrogate word in the input chunk.
				//    We have also deal with situation when there is a surrogate word
				//    at the end of a chunk.
				const m128i64_t surrogates_bytemask = _mm_cmpeq_epi16(_mm_and_si128(in, v_f800), v_d800);

				// bitmask = 0x0000 if there are no surrogates
				//         = 0xc000 if the last word is a surrogate
				const uint16_t surrogates_bitmask = union_cast<uint16_t>(_mm_movemask_epi8(surrogates_bytemask));
				// It might seem like checking for surrogates_bitmask == 0xc000 could help. However,
				// it is likely an uncommon occurrence.
				if(surrogates_bitmask == 0x00'00)
				{
					// case: words from register produce either 1, 2 or 3 UTF-8 bytes
					const m128i64_t dup_even = _mm_setr_epi16(0x00'00, 0x02'02, 0x04'04, 0x06'06, 0x08'08, 0x0a'0a, 0x0c'0c, 0x0e'0e);

					/* In this branch we handle three cases:
				 1. [0000|0000|0ccc|cccc] => [0ccc|cccc]                           - single UFT-8 byte
				 2. [0000|0bbb|bbcc|cccc] => [110b|bbbb], [10cc|cccc]              - two UTF-8 bytes
				 3. [aaaa|bbbb|bbcc|cccc] => [1110|aaaa], [10bb|bbbb], [10cc|cccc] - three UTF-8 bytes

				We expand the input word (16-bit) into two words (32-bit), thus
				we have room for four bytes. However, we need five distinct bit
				layouts. Note that the last byte in cases #2 and #3 is the same.

				We precompute byte 1 for case #1 and the common byte for cases #2 & #3
				in register t2.

				We precompute byte 1 for case #3 and -- **conditionally** -- precompute
				either byte 1 for case #2 or byte 2 for case #3. Note that they
				differ by exactly one bit.

				Finally from these two words we build proper UTF-8 sequence, taking
				into account the case (i.e, the number of bytes to write).
			*/
					/**
					 * Given [aaaa|bbbb|bbcc|cccc] our goal is to produce:
					 * t2 => [0ccc|cccc] [10cc|cccc]
					 * s4 => [1110|aaaa] ([110b|bbbb] OR [10bb|bbbb])
					 */
					// [aaaa|bbbb|bbcc|cccc] => [bbcc|cccc|bbcc|cccc]
					const m128i64_t t0 = _mm_shuffle_epi8(in, dup_even);
					// [bbcc|cccc|bbcc|cccc] => [00cc|cccc|0bcc|cccc]
					const m128i64_t t1 = _mm_and_si128(t0, _mm_set1_epi16(union_cast<uint16_t>(0b00111111'01111111)));
					// [00cc|cccc|0bcc|cccc] => [10cc|cccc|0bcc|cccc]
					const m128i64_t t2 = _mm_or_si128(t1, _mm_set1_epi16(union_cast<uint16_t>(0b10000000'00000000)));

					// [aaaa|bbbb|bbcc|cccc] =>  [0000|aaaa|bbbb|bbcc]
					const m128i64_t s0 = _mm_srli_epi16(in, 4);
					// [0000|aaaa|bbbb|bbcc] => [0000|aaaa|bbbb|bb00]
					const m128i64_t s1 = _mm_and_si128(s0, _mm_set1_epi16(union_cast<uint16_t>(0b00001111'11111100)));
					// [0000|aaaa|bbbb|bb00] => [00bb|bbbb|0000|aaaa]
					const m128i64_t s2 = _mm_maddubs_epi16(s1, _mm_set1_epi16(union_cast<uint16_t>(0x01'40)));
					// [00bb|bbbb|0000|aaaa] => [11bb|bbbb|1110|aaaa]
					const m128i64_t s3 = _mm_or_si128(s2, _mm_set1_epi16(union_cast<uint16_t>(0b11000000'11100000)));
					const m128i64_t m0 = _mm_andnot_si128(one_or_two_bytes_bytemask, _mm_set1_epi16(union_cast<uint16_t>(0b01000000'00000000)));
					const m128i64_t s4 = _mm_xor_si128(s3, m0);

					// 4. expand words 16-bit => 32-bit
					const m128i64_t out0 = _mm_unpacklo_epi16(t2, s4);
					const m128i64_t out1 = _mm_unpackhi_epi16(t2, s4);

					// 5. compress 32-bit words into 1, 2 or 3 bytes -- 2 x shuffle
					const uint16_t mask = (one_byte_bitmask & 0x55'55) | (one_or_two_bytes_bitmask & 0xaa'aa);
					if(mask == 0)
					{
						// We only have three-byte words. Use fast path.
						const m128i64_t shuffle = _mm_setr_epi8(2, 3, 1, 6, 7, 5, 10, 11, 9, 14, 15, 13, -1, -1, -1, -1);
						const m128i64_t utf8_0	= _mm_shuffle_epi8(out0, shuffle);
						const m128i64_t utf8_1	= _mm_shuffle_epi8(out1, shuffle);
						_mm_storeu_si128(union_cast<m128i64_t *>(__out), utf8_0);
						__out += 12;
						_mm_storeu_si128(union_cast<m128i64_t *>(__out), utf8_1);
						__out += 12;
						__in	+= 8;
						continue;
					}
					const uint8_t mask0 = uint8_t(mask);

					const uint8_t *row0			 = &__pack_1_2_3_utf16_to_utf8[mask0][0];
					const m128i64_t shuffle0 = _mm_loadu_si128(union_cast<m128i64_t *>(row0 + 1));
					const m128i64_t utf8_0	 = _mm_shuffle_epi8(out0, shuffle0);

					const uint8_t mask1 = uint8_t(mask >> 8);

					const uint8_t *row1			 = &__pack_1_2_3_utf16_to_utf8[mask1][0];
					const m128i64_t shuffle1 = _mm_loadu_si128(union_cast<m128i64_t *>(row1 + 1));
					const m128i64_t utf8_1	 = _mm_shuffle_epi8(out1, shuffle1);

					_mm_storeu_si128(union_cast<m128i64_t *>(__out), utf8_0);
					__out += row0[0];
					_mm_storeu_si128(union_cast<m128i64_t *>(__out), utf8_1);
					__out += row1[0];

					__in += 8;
					// surrogate pair(s) in a register
				}
				else
				{
					// Let us do a scalar fallback.
					// It may seem wasteful to use scalar code, but being efficient with SIMD
					// in the presence of surrogate pairs may require non-trivial tables.
					size_t forward = 15;
					size_t k			 = 0;
					if(size_t(end - __in) < forward + 1) { forward = size_t(end - __in - 1); }
					for(; k < forward; ++k)
					{
						uint16_t word = uint16_t(__in[k]);
						if((word & 0xFF'80) == 0)
						{
							*__out++ = u8char_t(word);
						}
						else if((word & 0xF8'00) == 0)
						{
							*__out++ = u8char_t((word >> 6) | 0b11000000);
							*__out++ = u8char_t((word & 0b111111) | 0b10000000);
						}
						else if((word & 0xF8'00) != 0xD8'00)
						{
							*__out++ = u8char_t((word >> 12) | 0b11100000);
							*__out++ = u8char_t(((word >> 6) & 0b111111) | 0b10000000);
							*__out++ = u8char_t((word & 0b111111) | 0b10000000);
						}
						else
						{
							// must be a surrogate pair
							uint16_t diff			 = uint16_t(word - 0xD8'00);
							uint16_t next_word = uint8_t(__in[k + 1]);
							++k;
							uint16_t diff2 = uint16_t(next_word - 0xDC'00);
							if((diff | diff2) > 0x3'FF) return make_pair(char_error_t { char_error_code::surrogate, __in - start + k - 1 }, __out);
							uint32_t value = (diff << 10) + diff2 + 0x1'00'00;
							*__out++			 = u8char_t((value >> 18) | 0b11110000);
							*__out++			 = u8char_t(((value >> 12) & 0b111111) | 0b10000000);
							*__out++			 = u8char_t(((value >> 6) & 0b111111) | 0b10000000);
							*__out++			 = u8char_t((value & 0b111111) | 0b10000000);
						}
					}
					__in += k;
				}
			}	 // while

			return make_pair(char_error_t { char_error_code::success, union_cast<size_t>(__in - start) }, __out);
		}

		pf_hint_nodiscard pf_decl_always_inline pf_decl_constexpr char_error_t
		convert(
		 const u16char_t *__in,
		 size_t __len,
		 u8char_t *__out) pf_attr_noexcept
		{
			if(!std::is_constant_evaluated())
			{
				/// NOTE: 512

				// 256
				if(instruction_set::simd & instruction_set::AVX2_BIT)
				{
					auto ret = __convert_with_errors_avx2(__in, __len, __out);
					if(ret.x.code != char_error_code::success) return ret.x;
					if(ret.x.position != __len)
					{
						char_error_t res = __convert_with_errors(__in + ret.x.position, __len - ret.x.position, ret.y);
						if(res.code != char_error_code::success)
						{
							res.position += ret.x.position;
							return res;
						}
						else
						{
							ret.y += res.position;
						}
					}
					ret.x.position = ret.y - __out;
					return ret.x;
				}

				// 128
				if(instruction_set::simd & instruction_set::SSE2_BIT)
				{
					auto ret = __convert_with_errors_sse2(__in, __len, __out);
					if(ret.x.code != char_error_code::success) return ret.x;
					if(ret.x.position != __len)
					{
						char_error_t res = __convert_with_errors(__in + ret.x.position, __len - ret.x.position, ret.y);
						if(res.code != char_error_code::success)
						{
							res.position += ret.x.position;
							return res;
						}
						else
						{
							ret.y += res.position;
						}
					}
					ret.x.position = ret.y - __out;
					return ret.x;
				}
			}

			// 64
			return __convert_with_errors(__in, __len, __out);
		}
	}	 // namespace utf16le_to_utf8


	/// CHAR: Utf8 - Types
	template<typename _Iterable>
	class __u8_value_wrapper;
	template<typename _Iterable>
	class __u8_const_value_wrapper;
	template<typename _Iterable>
	class u8iterator;
	template<typename _Iterable>
	class const_u8iterator;
	template<typename _Iterable>
	using reverse_u8iterator = reverse_iterator<u8iterator<_Iterable>>;
	template<typename _Iterable>
	using const_reverse_u8iterator = reverse_iterator<const_u8iterator<_Iterable>>;
	class u8string_view;
	template<
	 typename _Magnifier = magnifier_default,
	 typename _Allocator = allocator_default>
		requires(is_magnifier_v<_Magnifier> && is_allocator_v<_Allocator>)
	class u8string;

	/// CHAR: Utf8 - Value Wrapper
	template<typename _Iterable>
	class __u8_value_wrapper
	{
	public:
		/// Constructors
		pf_decl_inline pf_decl_constexpr
		__u8_value_wrapper(
		 u8char_t *__ptr,
		 _Iterable *__owner) pf_attr_noexcept
			: ptr_(__ptr)
			, owner_(__owner)
		{}
		__u8_value_wrapper(
		 __u8_value_wrapper<_Iterable> const &) = delete;
		__u8_value_wrapper(
		 __u8_value_wrapper<_Iterable> &&) = delete;

		/// Destructor
		pf_decl_inline pf_decl_constexpr ~__u8_value_wrapper() pf_attr_noexcept = default;

		/// Operator =
		__u8_value_wrapper<_Iterable> &
		operator=(
		 __u8_value_wrapper<_Iterable> const &) = delete;
		__u8_value_wrapper<_Iterable> &
		operator=(
		 __u8_value_wrapper<_Iterable> &&) = delete;
		pf_decl_inline pf_decl_constexpr __u8_value_wrapper<_Iterable> &
		operator=(
		 uint32_t __cp) const pf_attr_noexcept
		{
			this->owner_->replace(this->ptr_, __cp);
			return *this;
		}

		/// Operator (uint32_t)
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr
		operator uint32_t() const pf_attr_noexcept
		{
			return utf8::to_code_point(this->ptr_);
		}

	private:
		u8char_t *ptr_;
		_Iterable *owner_;
	};
	template<typename _Iterable>
	class __u8_const_value_wrapper
	{
	public:
		/// Constructors
		pf_decl_inline pf_decl_constexpr
		__u8_const_value_wrapper(
		 const u8char_t *__ptr) pf_attr_noexcept
			: ptr_(__ptr)
		{}
		__u8_const_value_wrapper(
		 __u8_const_value_wrapper<_Iterable> const &) = delete;
		__u8_const_value_wrapper(
		 __u8_const_value_wrapper<_Iterable> &&) = delete;

		/// Destructor
		pf_decl_inline pf_decl_constexpr ~__u8_const_value_wrapper() pf_attr_noexcept = default;

		/// Operator =
		__u8_const_value_wrapper<_Iterable> &
		operator=(
		 __u8_const_value_wrapper<_Iterable> const &) = delete;
		__u8_const_value_wrapper<_Iterable> &
		operator=(
		 __u8_const_value_wrapper<_Iterable> &&) = delete;

		/// Operator (uint32_t)
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr
		operator uint32_t() const pf_attr_noexcept
		{
			return utf8::to_code_point(this->ptr_);
		}

	private:
		const u8char_t *ptr_;
	};


	/// CHAR: Utf8 - Iterator
	template<typename _Iterable>
	class u8iterator
	{
		pf_decl_friend const_u8iterator<_Iterable>;

	public:
		using value_t	 = u8code_t;
		using category = iterator_sequence_tag_t;

		/// Constructors
		pf_decl_inline pf_decl_constexpr
		u8iterator(
		 u8char_t *__ptr		= nullptr,
		 _Iterable *__owner = nullptr) pf_attr_noexcept
			: ptr_(__ptr)
			, owner_(__owner)
		{
			pf_assert(__ptr == nullptr || __owner != nullptr, "Pointer isn't owned!, ptr={}, owner={}.", union_cast<const void *>(this->ptr_), union_cast<void *>(this->owner_));
		}
		pf_decl_inline pf_decl_constexpr
		u8iterator(
		 iterator<u8char_t> const &__r,
		 _Iterable *__owner) pf_attr_noexcept
			: u8iterator(__r.ptr_, __owner)
		{}
		pf_decl_inline pf_decl_constexpr
		u8iterator(
		 u8iterator<_Iterable> const &__it) pf_attr_noexcept
			: ptr_(__it.ptr_)
			, owner_(__it.owner_)
		{}

		/// Destructor
		pf_decl_inline pf_decl_constexpr ~u8iterator() pf_attr_noexcept = default;

		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr u8char_t *
		get() pf_attr_noexcept
		{
			return this->ptr_;
		}
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const u8char_t *
		get() const pf_attr_noexcept
		{
			return this->ptr_;
		}

		/// Owner
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr _Iterable *
		owner() pf_attr_noexcept
		{
			return this->owner_;
		}
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const _Iterable *
		owner() const pf_attr_noexcept
		{
			return this->owner_;
		}

		/// Operator =
		pf_decl_inline pf_decl_constexpr u8iterator<_Iterable> &
		operator=(
		 u8iterator<_Iterable> const &__r) pf_attr_noexcept = default;

		/// Operator +=
		pf_decl_inline pf_decl_constexpr u8iterator<_Iterable> &
		operator+=(
		 diff_t __i) pf_attr_noexcept
		{
			pf_assert(__i < 0 && this->ptr_ >= this->owner_->begin() || __i >= 0, "Previous pointer before beginning is undefined behavior!");
			this->ptr_ += __i >= 0
									 ? utf8::next(this->ptr_, distof(this->ptr_, this->end().get()), __i)
									: this->owner_->begin() == this->ptr_
									 ? -1
									 : -utf8::prev(this->ptr_, distof(this->ptr_, this->begin().get()), -__i);
			return *this;
		}

		/// Operator ++
		pf_decl_inline pf_decl_constexpr u8iterator<_Iterable>
		operator++(
		 int32_t) pf_attr_noexcept
		{
			u8iterator<_Iterable> it = { this->ptr_ + utf8::next(this->ptr_), this->owner_ };
			return it;
		}
		pf_decl_inline pf_decl_constexpr u8iterator<_Iterable> &
		operator++() pf_attr_noexcept
		{
			this->ptr_ += utf8::next(this->ptr_);
			return *this;
		}

		/// Operator -=
		pf_decl_inline pf_decl_constexpr u8iterator<_Iterable> &
		operator-=(
		 diff_t __i) pf_attr_noexcept
		{
			pf_assert(__i > 0 && this->ptr_ >= this->owner_->begin() || __i <= 0, "Previous pointer before beginning is undefined behavior!");
			this->ptr_ -= __i >= 0
									 ? this->owner_->begin() == this->ptr_
										? 1
										: utf8::prev(this->ptr_, this->owner_->length(), __i)
									 : -utf8::next(this->ptr_, this->owner_->length(), -__i);
			return *this;
		}

		/// Operator --
		pf_decl_inline pf_decl_constexpr u8iterator<_Iterable>
		operator--(
		 int32_t) pf_attr_noexcept
		{
			pf_assert(this->ptr_ >= this->owner_->begin(), "Previous pointer before beginning is undefined behavior!");
			u8iterator<_Iterable> it = { this->owner_->begin() == this->ptr_
																		? this->ptr_ - 1
																		: this->ptr_ - utf8::prev(this->ptr_),
																	 this->owner_ };
			return it;
		}
		pf_decl_inline pf_decl_constexpr u8iterator<_Iterable> &
		operator--() pf_attr_noexcept
		{
			pf_assert(this->ptr_ >= this->owner_->begin(), "Previous pointer before beginning is undefined behavior!");
			this->ptr_ -= this->owner_->begin() == this->ptr_
									 ? 1
									 : utf8::prev(this->ptr_);
			return *this;
		}

		/// Operator * -> (__u8_code_wrapper)
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr __u8_value_wrapper<_Iterable>
		operator*() pf_attr_noexcept
		{
			return __u8_value_wrapper<_Iterable>(this->ptr_, this->owner_);
		}
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr __u8_const_value_wrapper<_Iterable>
		operator*() const pf_attr_noexcept
		{
			return __u8_const_value_wrapper<_Iterable>(this->ptr_);
		}

		/// Operator (bool)
		pf_hint_nodiscard pf_decl_inline pf_decl_explicit pf_decl_constexpr
		operator bool() const pf_attr_noexcept
		{
			return this->get() != nullptr;
		}

		/// Operator (u8char_t *)
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr
		operator u8char_t *() pf_attr_noexcept
		{
			return this->get();
		}
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr
		operator const u8char_t *() const pf_attr_noexcept
		{
			return this->get();
		}

	private:
		u8char_t *ptr_;
		_Iterable *owner_;
	};


	/// CHAR: Utf8 - Const Iterator
	template<typename _Iterable>
	class const_u8iterator
	{
	public:
		using value_t	 = const u8code_t;	// Abstract
		using category = iterator_sequence_tag_t;

		/// Constructors
		pf_decl_inline pf_decl_constexpr
		const_u8iterator(
		 const u8char_t *__ptr		= nullptr,
		 const _Iterable *__owner = nullptr) pf_attr_noexcept
			: ptr_(__ptr)
			, owner_(__owner)
		{
			pf_assert(__ptr == nullptr || __owner != nullptr, "Pointer isn't owned!, ptr={}, owner={}.", union_cast<const void *>(this->ptr_), union_cast<void *>(this->owner_));
		}
		pf_decl_inline pf_decl_constexpr
		const_u8iterator(
		 const_iterator<u8char_t> const &__r,
		 const _Iterable *__owner) pf_attr_noexcept
			: const_u8iterator(__r.get(), __owner)
		{}
		pf_decl_inline pf_decl_constexpr
		const_u8iterator(
		 const_u8iterator<_Iterable> const &__it) pf_attr_noexcept
			: ptr_(__it.ptr_)
			, owner_(__it.owner_)
		{}
		pf_decl_inline pf_decl_constexpr
		const_u8iterator(
		 u8iterator<_Iterable> const &__it) pf_attr_noexcept
			: ptr_(__it.ptr_)
			, owner_(__it.owner_)
		{}

		/// Destructor
		pf_decl_inline pf_decl_constexpr ~const_u8iterator() pf_attr_noexcept = default;

		/// Get
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const u8char_t *
		get() const pf_attr_noexcept
		{
			return this->ptr_;
		}

		/// Owner
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const _Iterable *
		owner() const pf_attr_noexcept
		{
			return this->owner_;
		}

		/// Operator =
		pf_decl_inline pf_decl_constexpr const_u8iterator<_Iterable> &
		operator=(
		 const_u8iterator<_Iterable> const &__r) pf_attr_noexcept = default;

		/// Operator +=
		pf_decl_inline pf_decl_constexpr const_u8iterator<_Iterable> &
		operator+=(
		 diff_t __i) pf_attr_noexcept
		{
			pf_assert((__i >= 0) || (__i < 0 && this->ptr_ >= this->owner_->begin()), "Previous pointer before beginning is undefined behavior!");
			this->ptr_ += __i >= 0
									 ? utf8::next(this->ptr_, distof(this->ptr_, this->end().get()), __i)
									: this->owner_->begin() == this->ptr_
									 ? -1
									 : -utf8::prev(this->ptr_, distof(this->ptr_, this->begin().get()), -__i);
			return *this;
		}

		/// Operator ++
		pf_decl_inline pf_decl_constexpr const_u8iterator<_Iterable>
		operator++(
		 int32_t) pf_attr_noexcept
		{
			const_u8iterator<_Iterable> it = { utf8::next(this->ptr_), this->owner_ };
			return it;
		}
		pf_decl_inline pf_decl_constexpr const_u8iterator<_Iterable> &
		operator++() pf_attr_noexcept
		{
			this->ptr_ = utf8::next(this->ptr_);
			return *this;
		}

		/// Operator -=
		pf_decl_inline pf_decl_constexpr const_u8iterator<_Iterable> &
		operator-=(
		 diff_t __i) pf_attr_noexcept
		{
			pf_assert((__i <= 0) || (__i > 0 && this->ptr_ >= this->owner_->begin()), "Previous pointer before beginning is undefined behavior!");
			this->ptr_ -= __i >= 0
									 ? this->owner_->begin() == this->ptr_
										? 1
										: utf8::prev(this->ptr_, this->owner_->length(), __i)
									 : -utf8::next(this->ptr_, this->owner_->length(), -__i);
			return *this;
		}

		/// Operator --
		pf_decl_inline pf_decl_constexpr const_u8iterator<_Iterable>
		operator--(
		 int32_t) pf_attr_noexcept
		{
			pf_assert((this->ptr_ >= this->owner_->begin()), "Previous pointer before beginning is undefined behavior!");
			const_u8iterator<_Iterable> it = { this->owner_->begin() == this->ptr_
																					? this->ptr_ - 1
																					: this->ptr_ - utf8::prev(this->ptr_),
																				 this->owner_ };
			return it;
		}
		pf_decl_inline pf_decl_constexpr const_u8iterator<_Iterable> &
		operator--() pf_attr_noexcept
		{
			pf_assert((this->ptr_ >= this->owner_->begin()), "Previous pointer before beginning is undefined behavior!");
			this->ptr_ -= this->owner_->begin() == this->ptr_
									 ? 1
									 : utf8::prev(this->ptr_);
			return *this;
		}

		/// Operator * -> (__u8_code_wrapper)
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr __u8_const_value_wrapper<_Iterable>
		operator*() const pf_attr_noexcept
		{
			return __u8_const_value_wrapper<_Iterable>(this->ptr_);
		}

		/// Operator (bool)
		pf_hint_nodiscard pf_decl_inline pf_decl_explicit pf_decl_constexpr
		operator bool() const pf_attr_noexcept
		{
			return this->get() != nullptr;
		}

		/// Operator (u8char_t*)
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr
		operator const u8char_t *() const pf_attr_noexcept
		{
			return this->get();
		}

	private:
		const u8char_t *ptr_;
		const _Iterable *owner_;
	};

	/// CHAR: String -> View
	class u8string_view
	{
	public:
		using value_t										 = u8char_t;
		using const_iterator_t					 = const_iterator<u8char_t>;
		using const_reverse_iterator_t	 = reverse_iterator<const_iterator_t>;
		using const_u8iterator_t				 = const_u8iterator<u8string_view>;
		using const_reverse_u8iterator_t = reverse_iterator<const_u8iterator_t>;

		/// Constructors
		pf_decl_inline pf_decl_constexpr
		u8string_view() pf_attr_noexcept
			: data_(nullptr)
			, len_(0)
		{}
		pf_decl_inline pf_decl_constexpr
		u8string_view(
		 const u8char_t *__data,
		 size_t __len) pf_attr_noexcept
			: data_(__data)
			, len_(__len)
		{}
		pf_decl_inline pf_decl_constexpr
		u8string_view(
		 const u8char_t *__data) pf_attr_noexcept
			: data_(__data)
			, len_(utf8::lenof(__data))
		{}
		template<size_t _Num>
		pf_decl_inline pf_decl_constexpr
		u8string_view(
		 const u8char_t (&__arr)[_Num]) pf_attr_noexcept
			: data_(&__arr[0])
			, len_(_Num)
		{}
		template<size_t _Num>
		pf_decl_inline pf_decl_constexpr
		u8string_view(
		 const array<u8char_t, _Num> &__arr) pf_attr_noexcept
			: data_(&__arr[0])
			, len_(_Num)
		{}
		template<
		 typename _Magnifier,
		 typename _Allocator>
		pf_decl_inline pf_decl_constexpr
		u8string_view(
		 const u8string<_Magnifier, _Allocator> &__s) pf_attr_noexcept
			: data_(__s.data())
			, len_(__s.length())
		{}
		pf_decl_inline pf_decl_constexpr
		u8string_view(
		 const_iterator_t __beg,
		 const_iterator_t __end) pf_attr_noexcept
			: data_(__beg.get())
			, len_(distof(__beg, __end))
		{}
		pf_decl_inline pf_decl_constexpr
		u8string_view(
		 u8string_view const &__v) pf_attr_noexcept
			: data_(__v.data_)
			, len_(__v.len_)
		{}

		/// Destructor
		pf_decl_inline pf_decl_constexpr ~u8string_view() pf_attr_noexcept = default;

		/// Operator =
		pf_decl_inline pf_decl_constexpr u8string_view &
		operator=(
		 u8string_view const &__v) pf_attr_noexcept = default;

		/// Operator <=>
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr bool
		operator==(
		 u8string_view const &__v) const pf_attr_noexcept
		{
			return (this->len_ == __v.len_)
					&& (equal(this->cbegin(), this->cend(), __v.cbegin()));
		}
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr auto
		operator<=>(
		 u8string_view const &__v) const pf_attr_noexcept
		{
			size_t l1 = this->len_;
			size_t l2 = __v.len_;
			if(l1 < l2) return -1ll;
			if(l1 > l2) return 1ll;
			return utf8::compare(this->cbegin().get(), __v.cbegin().get(), l1);
		}

		/// Operator []
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const u8char_t &
		operator[](
		 size_t __index) const
		{
			pf_throw_if(
			 __index >= this->len_,
			 dbg_category_generic(),
			 dbg_code::invalid_argument,
			 dbg_flags::none,
			 "index is out of string view! len={}, index={}",
			 this->len_,
			 __index);
			return this->data_[__index];
		}

		/// Operator ()
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr __u8_const_value_wrapper<u8string_view>
		operator()(
		 size_t __index) const pf_attr_noexcept
		{
			return &this->operator[](utf8::next(this->data_, this->len_, __index));
		}

		/// Begin
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const_iterator_t
		begin() const pf_attr_noexcept
		{
			return this->data_;
		}
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const_iterator_t
		cbegin() const pf_attr_noexcept
		{
			return this->begin();
		}

		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const_u8iterator_t
		code_begin() const pf_attr_noexcept
		{
			return { this->data_, this };
		}
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const_u8iterator_t
		code_cbegin() const pf_attr_noexcept
		{
			return this->code_begin();
		}

		/// End
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const_iterator_t
		end() const pf_attr_noexcept
		{
			return this->data_ + this->len_;
		}
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const_iterator_t
		cend() const pf_attr_noexcept
		{
			return this->end();
		}

		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const_u8iterator_t
		code_end() const pf_attr_noexcept
		{
			return { this->data_ + this->len_, this };
		}
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const_u8iterator_t
		code_cend() const pf_attr_noexcept
		{
			return this->code_end();
		}

		/// Reverse Begin
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const_reverse_iterator_t
		rbegin() const pf_attr_noexcept
		{
			return this->data_ + this->len_ - 1;
		}
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const_reverse_iterator_t
		crbegin() const pf_attr_noexcept
		{
			return this->rbegin();
		}

		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const_reverse_u8iterator_t
		code_rbegin() const pf_attr_noexcept
		{
			const u8char_t *p = this->data_ + this->len_;
			return const_u8iterator_t { p - utf8::prev(p), this };
		}
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const_reverse_u8iterator_t
		code_crbegin() const pf_attr_noexcept
		{
			return this->code_rbegin();
		}

		/// Reverse End
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const_reverse_iterator_t
		rend() const pf_attr_noexcept
		{
			return this->data_ - 1;
		}
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const_reverse_iterator_t
		crend() const pf_attr_noexcept
		{
			return this->rend();
		}

		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const_reverse_u8iterator_t
		code_rend() const pf_attr_noexcept
		{
			return const_u8iterator_t { this->data_ - 1, this };
		}
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const_reverse_u8iterator_t
		code_crend() const pf_attr_noexcept
		{
			return this->code_rend();
		}

		/// Data
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const u8char_t *
		data() const pf_attr_noexcept
		{
			return this->data_;
		}

		/// Length
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr size_t
		length() const pf_attr_noexcept
		{
			return this->len_;
		}

		/// Count
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr size_t
		code_count() const pf_attr_noexcept
		{
			return utf8::countof(this->data_, this->len_);
		}

		/// Is Empty
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr bool
		is_empty() const pf_attr_noexcept
		{
			return this->data_ == nullptr;
		}

	private:
		const u8char_t *data_;
		size_t len_;
	};


	/// CHAR: String
	template<
	 typename _Magnifier,
	 typename _Allocator>
		requires(is_magnifier_v<_Magnifier> && is_allocator_v<_Allocator>)
	class u8string
	{
		template<typename _MagnifierR, typename _AllocatorR>
			requires(is_magnifier_v<_MagnifierR> && is_allocator_v<_AllocatorR>)
		pf_decl_friend class u8string;

		// __reallocate
		pf_decl_constexpr diff_t
		__reallocate_no_check(
		 size_t __len,
		 align_val_t __align)
		{
			this->data_ = union_cast<u8char_t *>(reallocate(
			 this->allocator_,
			 this->data_,
			 __len,
			 __align));
			pf_throw_if(
			 !this->data_,
			 dbg_category_generic(),
			 dbg_code::bad_alloc,
			 dbg_flags::dump_with_data_segs | dbg_flags::dump_with_handle_data,
			 "Cannot reallocate string sequence storage!");
			const diff_t d	= union_cast<diff_t>(__len) - this->capacity_;
			this->capacity_ = __len;
			this->align_		= __align;
			return d;
		}

		// __code_fill
		pf_decl_constexpr void
		__code_fill_1(
		 iterator_t __beg,
		 iterator_t __end,
		 uint32_t __code) pf_attr_noexcept
		{
			u8char_t arr[1];
			utf8::to_code(__code, &arr[0]);
			for(; __beg != __end; ++__beg)	// SIMD
			{
				*__beg = arr[0];
			}
		}
		pf_decl_constexpr void
		__code_fill_2(
		 iterator_t __beg,
		 iterator_t __end,
		 uint32_t __code) pf_attr_noexcept
		{
			pf_assert(distof(__beg, __end) % 2 == 0, "__beg to __end isn't multiple of 2!");
			u8char_t arr[2];
			utf8::to_code(__code, &arr[0]);
			for(; __beg != __end; __beg += 2)	 // SIMD
			{
				*(__beg + 0) = arr[0];
				*(__beg + 1) = arr[1];
			}
		}
		pf_decl_constexpr void
		__code_fill_3(
		 iterator_t __beg,
		 iterator_t __end,
		 uint32_t __code) pf_attr_noexcept
		{
			pf_assert(distof(__beg, __end) % 3 == 0, "__beg to __end isn't multiple of 3!");
			u8char_t arr[3];
			utf8::to_code(__code, &arr[0]);
			for(; __beg != __end; __beg += 3)	 // SIMD
			{
				*(__beg + 0) = arr[0];
				*(__beg + 1) = arr[1];
				*(__beg + 2) = arr[2];
			}
		}
		pf_decl_constexpr void
		__code_fill_4(
		 iterator_t __beg,
		 iterator_t __end,
		 uint32_t __code) pf_attr_noexcept
		{
			pf_assert(distof(__beg, __end) % 4 == 0, "__beg to __end isn't multiple of 4!");
			u8char_t arr[4];
			utf8::to_code(__code, &arr[0]);
			for(; __beg != __end; __beg += 4)	 // SIMD
			{
				*(__beg + 0) = arr[0];
				*(__beg + 1) = arr[1];
				*(__beg + 2) = arr[2];
				*(__beg + 3) = arr[3];
			}
		}
		pf_decl_constexpr void
		__code_fill(
		 iterator_t __beg,
		 iterator_t __end,
		 uint32_t __code,
		 size_t __codelen) pf_attr_noexcept
		{
			switch(__codelen)
			{
				case 1: __code_fill_1(__beg, __end, __code);
				case 2: __code_fill_2(__beg, __end, __code);
				case 3: __code_fill_3(__beg, __end, __code);
				case 4: __code_fill_4(__beg, __end, __code);
				default: pf_assert("__codelen={} isn't correct!", __codelen);
			};
		}

		// __input*
		template<typename _IteratorIn>
		pf_decl_constexpr void
		__input_required_length(
		 _IteratorIn __ibeg,
		 _IteratorIn __iend) pf_attr_noexcept
			requires(
			 is_iterator_v<_IteratorIn>
			 && std::is_same_v<typename _IteratorIn::value_t, u8code_t>)
		{
			size_t len = 0;
			for(; __ibeg != __iend; ++__ibeg)
			{
				len += utf8::code_len(*__ibeg);
			}
			return len;
		}
		template<typename _IteratorIn>
		pf_decl_constexpr void
		__input_required_length(
		 _IteratorIn __ibeg,
		 _IteratorIn __iend) pf_attr_noexcept
			requires(
			 is_iterator_v<_IteratorIn>
			 && std::is_same_v<typename _IteratorIn::value_t, u8char_t>)
		{
			return distof(__ibeg, __iend);
		}
		template<typename _IteratorIn>
		pf_decl_constexpr void
		__input_copy(
		 iterator_t __obeg,
		 _IteratorIn __ibeg,
		 _IteratorIn __iend) pf_attr_noexcept
			requires(
			 is_iterator_v<_IteratorIn>
			 && std::is_same_v<typename _IteratorIn::value_t, u8code_t>)
		{
			for(; __ibeg != __iend; ++__ibeg)
			{
				__obeg += utf8::to_code(*__ibeg, __obeg);
			}
		}
		template<typename _IteratorIn>
		pf_decl_constexpr void
		__input_copy(
		 iterator_t __obeg,
		 _IteratorIn __ibeg,
		 _IteratorIn __iend) pf_attr_noexcept
			requires(
			 is_iterator_v<_IteratorIn>
			 && std::is_same_v<typename _IteratorIn::value_t, u8char_t>)
		{
			memcpy(__obeg.get(), __ibeg.get(), distof(__ibeg, __iend));
		}

		// __push
		pf_hint_nodiscard pf_decl_constexpr void
		__push_insert_back(
		 u8char_t __val,
		 size_t __count)
		{
			if(pf_unlikely(!__count)) return;
			size_t l;
			if(pf_unlikely(!this->len_)) l = 1 else l = 0;
			this->reserve_with_magnifier(this->len_ + l + __count);
			auto e = this->begin() + this->len_ + l + __count - 1;
			construct(this->begin() + this->len_ + l - 1, e, __val);
			*e					= '\0';
			this->len_ += c;
		}
		pf_hint_nodiscard pf_decl_constexpr void
		__push_insert_back(
		 u8code_t __code,
		 size_t __count)
		{
			if(pf_unlikely(!__count)) return;
			size_t l;
			if(pf_unlikely(!this->len_)) l = 1 else l = 0;
			const size_t clen = utf8::code_len(__code);
			const size_t c		= __count * clen;
			this->reserve_with_magnifier(this->len_ + l + c);
			auto e = this->begin() + this->len_ + l + c - 1;
			this->__code_fill(this->begin() + this->len_ + l - 1, e, __code, clen);
			*e					= '\0';
			this->len_ += l + c;
		}
		template<typename _IteratorIn>
		pf_decl_constexpr void
		__push_insert_back(
		 _IteratorIn __beg,
		 _IteratorIn __end)
		{
			const size_t len = this->__input_required_length(__beg, __end);
			if(pf_unlikely(!len)) return;
			size_t l;
			if(pf_unlikely(!this->len_)) l = 1 else l = 0;
			this->reserve_with_magnifier(this->len_ + l + len);
			auto e = this->begin() + this->len_ + len + l - 1;
			this->__input_copy(this->begin() + this->len_ + l - 1, __beg, __end);
			*e					= '\0';
			this->len_ += c;
		}

	public:
		using value_t										 = u8char_t;
		using code_value_t							 = u8code_t;
		using iterator_t								 = iterator<u8char_t>;
		using const_iterator_t					 = const_iterator<u8char_t>;
		using reverse_iterator_t				 = reverse_iterator<iterator_t>;
		using const_reverse_iterator_t	 = reverse_iterator<const_iterator_t>;
		using u8iterator_t							 = u8iterator<u8string<_Magnifier, _Allocator>>;
		using const_u8iterator_t				 = const_u8iterator<u8string<_Magnifier, _Allocator>>;
		using reverse_u8iterator_t			 = reverse_iterator<u8iterator_t>;
		using const_reverse_u8iterator_t = reverse_iterator<const_u8iterator_t>;
		using view_t										 = u8string_view;

		/// Constructors
		pf_decl_constexpr
		u8string(
		 align_val_t __align			= ALIGN_DEFAULT,
		 _Magnifier &&__magnifier = _Magnifier(),
		 _Allocator &&__allocator = _Allocator()) pf_attr_noexcept
			: data_(nullptr)
			, capacity_(0)
			, align_(__align)
			, len_(0)
			, magnifier_(std::move(__magnifier))
			, allocator_(std::move(__allocator))
		{}
		pf_decl_constexpr
		u8string(
		 u8char_t __val,
		 size_t __count,
		 align_val_t __align			= ALIGN_DEFAULT,
		 _Magnifier &&__magnifier = _Magnifier(),
		 _Allocator &&__allocator = _Allocator())
			: u8string(__align, std::move(__magnifier), std::move(__allocator))
		{
			if(pf_unlikely(!__count)) return;
			this->__reallocate_no_check(this->magnifier_(__count + 1), __align);
			auto e = this->begin() + __count;
			construct(this->begin(), e, __val);
			*e				 = '\0';
			this->len_ = __count + 1;
		}
		pf_decl_constexpr
		u8string(
		 u8code_t __code,
		 size_t __count,
		 align_val_t __align			= ALIGN_DEFAULT,
		 _Magnifier &&__magnifier = _Magnifier(),
		 _Allocator &&__allocator = _Allocator())
			: u8string(__align, std::move(__magnifier), std::move(__allocator))
		{
			if(pf_unlikely(!__count)) return;
			const size_t clen = utf8::code_len(__code);
			const size_t len	= __count * clen;
			this->__reallocate_no_check(this->magnifier_(len + 1), __align);
			auto e = this->begin() + len;
			this->__code_fill(this->begin(), e, __code, clen);
			*e				 = '\0';
			this->len_ = len + 1;
		}
		template<typename _IteratorIn>
		pf_decl_constexpr
		u8string(
		 _IteratorIn __beg,
		 _IteratorIn __end,
		 align_val_t __align			= ALIGN_DEFAULT,
		 _Magnifier &&__magnifier = _Magnifier(),
		 _Allocator &&__allocator = _Allocator())
			requires(is_iterator_v<_IteratorIn>)
			: u8string(__align, std::move(__magnifier), std::move(__allocator))
		{
			const size_t c = this->__input_required_length(__beg, __end);
			this->__reallocate_no_check(this->magnifier_(c + 1), __align);
			auto l = this->begin() + c;
			this->__input_copy(this->begin(), __beg, __end);
			*l				 = '\0';
			this->len_ = __count + 1;
		}
		template<size_t _Num>
		pf_decl_constexpr
		u8string(
		 const u8char_t (&__arr)[_Num],
		 align_val_t __align			= ALIGN_DEFAULT,
		 _Magnifier &&__magnifier = _Magnifier(),
		 _Allocator &&__allocator = _Allocator())
			: u8string(pul::begin(__arr), pul::end(__arr), __align, std::move(__magnifier), std::move(__allocator))
		{}
		template<size_t _Num>
		pf_decl_constexpr
		u8string(
		 const u8code_t (&__arr)[_Num],
		 align_val_t __align			= ALIGN_DEFAULT,
		 _Magnifier &&__magnifier = _Magnifier(),
		 _Allocator &&__allocator = _Allocator())
			: u8string(pul::begin(__arr), pul::end(__arr), __align, std::move(__magnifier), std::move(__allocator))
		{}
		template<size_t _Num>
		pf_decl_constexpr
		u8string(
		 const array<u8char_t, _Num> &__arr,
		 align_val_t __align			= ALIGN_DEFAULT,
		 _Magnifier &&__magnifier = _Magnifier(),
		 _Allocator &&__allocator = _Allocator())
			: u8string(__arr.begin(), __arr.end(), __align, std::move(__magnifier), std::move(__allocator))
		{}
		template<size_t _Num>
		pf_decl_constexpr
		u8string(
		 const array<u8code_t, _Num> &__arr,
		 align_val_t __align			= ALIGN_DEFAULT,
		 _Magnifier &&__magnifier = _Magnifier(),
		 _Allocator &&__allocator = _Allocator())
			: u8string(__arr.begin(), __arr.end(), __align, std::move(__magnifier), std::move(__allocator))
		{}
		template<typename _View>
		pf_decl_constexpr
		u8string(
		 _View __view,
		 align_val_t __align			= ALIGN_DEFAULT,
		 _Magnifier &&__magnifier = _Magnifier(),
		 _Allocator &&__allocator = _Allocator())
			requires(is_view_v<_View>)
			: u8string(__view.begin(), __view.end(), __align, std::move(__magnifier), std::move(__allocator))
		{}
		pf_decl_constexpr
		u8string(
		 initializer_list<u8char_t> __list,
		 align_val_t __align			= ALIGN_DEFAULT,
		 _Magnifier &&__magnifier = _Magnifier(),
		 _Allocator &&__allocator = _Allocator())
			: u8string(iterator(__list.begin()), iterator(__list.end()), __align, std::move(__magnifier), std::move(__allocator))
		{}
		pf_decl_constexpr
		u8string(
		 initializer_list<u8code_t> __list,
		 align_val_t __align			= ALIGN_DEFAULT,
		 _Magnifier &&__magnifier = _Magnifier(),
		 _Allocator &&__allocator = _Allocator())
			: u8string(iterator(__list.begin()), iterator(__list.end()), __align, std::move(__magnifier), std::move(__allocator))
		{}
		pf_decl_constexpr
		u8string(
		 const u8char_t *__str,
		 size_t __len,
		 align_val_t __align			= ALIGN_DEFAULT,
		 _Magnifier &&__magnifier = _Magnifier(),
		 _Allocator &&__allocator = _Allocator())
			: u8string(iterator(__str), iterator(__str + __len), __align, std::move(__magnifier), std::move(__allocator))
		{}
		pf_decl_constexpr
		u8string(
		 const u8char_t *__str,
		 align_val_t __align			= ALIGN_DEFAULT,
		 _Magnifier &&__magnifier = _Magnifier(),
		 _Allocator &&__allocator = _Allocator())
			: u8string(__str, utf8::lenof(__str) - 1, std::move(__magnifier), std::move(__allocator))
		{}
		template<typename _MagnifierR, typename _AllocatorR>
		pf_decl_constexpr
		u8string(
		 const u8string<_MagnifierR, _AllocatorR> &__r,
		 align_val_t __align,
		 _Magnifier &&__magnifier,
		 _Allocator &&__allocator) pf_attr_noexcept
			: u8string(__r.begin(), __r.end(), __align, std::move(__magnifier))
		{}
		pf_decl_constexpr
		u8string(
		 const u8string<_Magnifier, _Allocator> &__r,
		 align_val_t __align) pf_attr_noexcept
			: u8string(__r.begin(), __r.end(), __align, _Magnifier(), _Allocator())
		{}
		pf_decl_constexpr
		u8string(
		 const u8string<_Magnifier, _Allocator> &__r) pf_attr_noexcept
			: u8string(__r.begin(), __r.end(), __r.align_, _Magnifier(), _Allocator())
		{}
		pf_decl_constexpr
		u8string(
		 u8string<_Magnifier, _Allocator> &&__r) pf_attr_noexcept
			: data_(__r.data_)
			, capacity_(__r.capacity_)
			, align_(__r.align_)
			, len_(__r.len_)
			, magnifier_(std::move(__r.magnifier_))
			, allocator_(std::move(__r.allocator_))
		{
			__r.data_			= nullptr;
			__r.capacity_ = 0;
			__r.len_			= 0;
		}

		/// Destructor
		pf_decl_constexpr ~u8string() pf_attr_noexcept
		{
			this->clear();
		}

		/// Operator =
		pf_decl_inline pf_decl_constexpr u8string<_Magnifier, _Allocator> &
		operator=(
		 const u8char_t *__str)
		{
			this->assign(__str);
			return *this;
		}
		template<typename _View>
		pf_decl_inline pf_decl_constexpr u8string<_Magnifier, _Allocator> &
		operator=(
		 _View __v)
			requires(is_view_v<_View>)
		{
			this->assign(__v);
			return *this;
		}
		template<typename _MagnifierR, typename _AllocatorR>
		pf_decl_inline pf_decl_constexpr u8string<_Magnifier, _Allocator> &
		operator=(
		 u8string<_MagnifierR, _AllocatorR> const &__r)
		{
			this->assign(__r);
			return *this;
		}
		template<size_t _Num>
		pf_decl_inline pf_decl_constexpr u8string<_Magnifier, _Allocator> &
		operator=(
		 const u8char_t (&__arr)[_Num])
		{
			this->assign(__arr);
			return *this;
		}
		template<size_t _Num>
		pf_decl_inline pf_decl_constexpr u8string<_Magnifier, _Allocator> &
		operator=(
		 const u8code_t (&__arr)[_Num])
		{
			this->assign(__arr);
			return *this;
		}
		template<size_t _Num>
		pf_decl_inline pf_decl_constexpr u8string<_Magnifier, _Allocator> &
		operator=(
		 array<u8char_t, _Num> const &__arr)
		{
			this->assign(__arr);
			return *this;
		}
		template<size_t _Num>
		pf_decl_inline pf_decl_constexpr u8string<_Magnifier, _Allocator> &
		operator=(
		 array<u8code_t, _Num> const &__arr)
		{
			this->assign(__arr);
			return *this;
		}
		pf_decl_inline pf_decl_constexpr u8string<_Magnifier, _Allocator> &
		operator=(
		 u8string<_Magnifier, _Allocator> &&__r)
		{
			this->assign(std::move(__r));
			return *this;
		}

		/// Operator (View)
		pf_hint_nodiscard pf_decl_explicit pf_decl_inline pf_decl_constexpr
		operator u8string_view() const pf_attr_noexcept
		{
			return this->view();
		}

		/// Operator []
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr u8char_t &
		operator[](
		 size_t __index)
		{
			pf_throw_if(
			 __index >= this->len_,
			 dbg_category_generic(),
			 dbg_code::invalid_argument,
			 dbg_flags::none,
			 "index is out of string sequence! len={}, index={}",
			 this->len_,
			 __index);
			return this->data_[__index];
		}
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const u8char_t &
		operator[](
		 size_t __index) const
		{
			pf_throw_if(
			 __index >= this->len_,
			 dbg_category_generic(),
			 dbg_code::invalid_argument,
			 dbg_flags::none,
			 "index is out of string sequence! len={}, index={}",
			 this->len_,
			 __index);
			return this->data_[__index];
		}

		/// Operator ()
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr __u8_value_wrapper<u8string<_Magnifier, _Allocator>>
		operator()(
		 size_t __index)
		{
			return { &this->operator[](utf8::next(this->data_, this->len_, __index)), this };
		}
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr __u8_const_value_wrapper<u8string<_Magnifier, _Allocator>>
		operator()(
		 size_t __index) const
		{
			return &this->operator[](utf8::next(this->data_, this->len_, __index));
		}

		/// Assign=
		pf_decl_constexpr void
		assign(
		 const u8char_t *__str,
		 size_t __len)
		{
			this->reserve_with_magnifier(__len + 1);
			memcpy(this->begin(), __str, __len);
			auto e		 = this->begin() + __len;
			*e				 = '\0';
			this->len_ = __len + 1;
		}
		pf_decl_inline pf_decl_constexpr void
		assign(
		 const u8char_t *__str)
		{
			this->assign(__str, utf8::lenof(__str) - 1);
		}
		pf_decl_constexpr void
		assign(
		 u8char_t __val,
		 size_t __count)
		{
			if(!pf_unlikely(!__count)) return 0;
			this->reserve_with_magnifier(__count + 1);
			auto e = this->begin() + __count;
			construct(this->begin(), e, __val);
			*e				 = '\0';
			this->len_ = __count + 1;
		}
		pf_decl_constexpr void
		assign(
		 u8code_t __code,
		 size_t __count)
		{
			if(!pf_unlikely(!__count)) return 0;
			const size_t clen = utf8::code_len(__code);
			const size_t len	= __count * clen;
			this->reserve_with_magnifier(len + 1);
			auto e = this->begin() + len;
			this->__fill(this->begin(), e, __code, clen);
			*e				 = '\0';
			this->len_ = len + 1;
		}
		template<typename _IteratorIn>
		pf_decl_constexpr void
		assign(
		 _IteratorIn __beg,
		 _IteratorIn __end)
			requires(is_iterator_v<_IteratorIn>)
		{
			const size_t len = __input_required_length(__beg, __end);
			if(pf_unlikely(!len)) return;
			this->reserve_with_magnifier(len + 1);
			this->__input_copy(this->begin(), __beg, __end);
			auto e		 = this->begin() + len;
			*e				 = '\0';
			this->len_ = len + 1;
		}
		pf_decl_inline pf_decl_constexpr void
		assign(
		 u8string_view __v)
		{
			this->assign(__v.begin(), __v.end());
		}
		pf_decl_inline pf_decl_constexpr void
		assign(
		 u8string<_Magnifier, _Allocator> const &__str)
		{
			if(pf_likely(this != &__str)) this->assign(__str.begin(), __str.end());
		}
		template<typename _MagnifierR, typename _AllocatorR>
		pf_decl_inline pf_decl_constexpr void
		assign(
		 u8string<_MagnifierR, _AllocatorR> const &__str)
		{
			this->assign(__str.begin(), __str.end());
		}
		template<size_t _Num>
		pf_decl_inline pf_decl_constexpr u8string<_Magnifier, _Allocator> &
		assign(
		 const u8char_t (&__arr)[_Num])
		{
			this->assign(pul::begin(__arr), pul::end(__arr));
		}
		template<size_t _Num>
		pf_decl_inline pf_decl_constexpr u8string<_Magnifier, _Allocator> &
		assign(
		 const u8code_t (&__arr)[_Num])
		{
			this->assign(pul::begin(__arr), pul::end(__arr));
		}
		template<size_t _Num>
		pf_decl_inline pf_decl_constexpr u8string<_Magnifier, _Allocator> &
		assign(
		 array<u8char_t, _Num> const &__arr)
		{
			this->assign(__arr.begin(), __arr.end());
		}
		template<size_t _Num>
		pf_decl_inline pf_decl_constexpr u8string<_Magnifier, _Allocator> &
		assign(
		 array<u8code_t, _Num> const &__arr)
		{
			this->assign(__arr.begin(), __arr.end());
		}
		template<typename _View>
		pf_decl_inline pf_decl_constexpr u8string<_Magnifier, _Allocator> &
		assign(
		 _View __v)
			requires(is_view_v<_View>)
		{
			this->assign(__v.begin(), __v.end());
		}
		pf_decl_inline pf_decl_constexpr u8string<_Magnifier, _Allocator> &
		assign(
		 initializer_list<u8char_t> __il)
		{
			this->assign(iterator(__il.begin()), iterator(__il.end()));
		}
		pf_decl_constexpr u8string<_Magnifier, _Allocator> &
		assign(
		 u8string<_Magnifier, _Allocator> &&__r) pf_attr_noexcept
		{
			if(pf_likely(this->data_ != __r.data_))
			{
				this->clear();
				this->data_			 = __r.data_;
				this->capacity_	 = __r.capacity_;
				this->align_		 = __r.align_;
				this->len_			 = __r.len_;
				this->magnifier_ = std::move(__r.magnifier_);
				this->allocator_ = std::move(__r.allocator_);
				__r.data_				 = nullptr;
				__r.capacity_		 = 0;
				__r.len_				 = 0;
			}
		}

		/// Push
		pf_decl_constexpr size_t
		push_back(
		 u8char_t __val,
		 size_t __count = 1)
		{
			this->__push_insert_back(__val, __count);
			if(pf_unlikely(!this->len_)) return 0 else return this->len_ - 1;
		}
		pf_decl_constexpr size_t
		push_back(
		 u8code_t __code,
		 size_t __count = 1)
		{
			this->__push_insert_back(__code, __count);
			if(pf_unlikely(!this->len_)) return 0 else return this->len_ - 1;
		}
		template<typename _IteratorIn>
		pf_decl_constexpr size_t
		push_back(
		 _IteratorIn __beg,
		 _IteratorIn __end)
			requires(is_iterator_v<_IteratorIn>)
		{
			this->__push_insert_back(__beg, __end);
			if(pf_unlikely(!this->len_)) return 0 else return this->len_ - 1;
		}
		template<typename _View>
		pf_decl_constexpr size_t
		push_back(
		 _View __v)
			requires(is_view_v<_View>)
		{
			return this->push_back(__v.begin(), __v.end());
		}
		pf_decl_constexpr size_t
		push_back(
		 initializer_list<u8char_t> __il)
		{
			return this->push_back(iterator(__il.begin()), iterator(__il.end()));
		}
		pf_decl_constexpr size_t
		push_back(
		 initializer_list<u8code_t> __il)
		{
			return this->push_back(iterator(__il.begin()), iterator(__il.end()));
		}

		pf_decl_constexpr size_t
		push(
		 size_t __w,
		 u8char_t __val,
		 size_t __count = 1)
		{
			if(pf_unlikely(!__count)) return 0;
			iterator_t w = this->begin() + __w;
			pf_throw_if(
			 w >= this->end(),
			 dbg_category_generic(),
			 dbg_code::invalid_argument,
			 dbg_flags::none,
			 "Trying to insert outside of sequence range! end={}, where={}",
			 union_cast<const void *>(this->end().get()),
			 union_cast<const void *>(__w.get()));
			size_t l;
			if(pf_unlikely(!this->len_)) l = 1 else l = 0;
			this->reserve_with_magnifier(this->len_ + l + __count);
		}
		pf_decl_constexpr size_t
		push(
		 size_t __w,
		 u8code_t __code,
		 size_t __count = 1)
		{
			// TODO push_back __code, __count
		}
		template<typename _IteratorIn>
		pf_decl_constexpr size_t
		push(
		 size_t __w,
		 _IteratorIn __beg,
		 _IteratorIn __end)
			requires(is_iterator_v<_IteratorIn>)
		{
			// TODO push_back __beg, __end
		}
		template<typename _View>
		pf_decl_inline pf_decl_constexpr size_t
		push(
		 size_t __w,
		 _View __v)
			requires(is_view_v<_View>)
		{
			return this->push(__w, __v.begin(), __v.end());
		}
		pf_decl_inline pf_decl_constexpr size_t
		push(
		 size_t __w,
		 initializer_list<u8char_t> __il)
		{
			return this->push(__w, iterator(__il.begin()), iterator(__il.end()));
		}
		pf_decl_inline pf_decl_constexpr size_t
		push(
		 size_t __w,
		 initializer_list<u8code_t> __il)
		{
			return this->push(__w, iterator(__il.begin()), iterator(__il.end()));
		}

		/// Insert
		pf_decl_constexpr iterator_t
		insert_back(
		 u8char_t __val,
		 size_t __count = 1)
		{
			this->__push_insert_back(__val, __count);
			if(pf_unlikely(!this->len_)) return this->begin() else return this->begin() + this->len_ - 1;
		}
		pf_decl_constexpr iterator_t
		insert_back(
		 u8code_t __code,
		 size_t __count = 1)
		{
			this->__push_insert_back(__code, __count);
			if(pf_unlikely(!this->len_)) return this->begin() else return this->begin() + this->len_ - 1;
		}
		template<typename _IteratorIn>
		pf_decl_constexpr iterator_t
		insert_back(
		 _IteratorIn __beg,
		 _IteratorIn __end)
			requires(is_iterator_v<_IteratorIn>)
		{
			this->__push_insert_back(__beg, __end);
			if(pf_unlikely(!this->len_)) return this->begin() else return this->begin() + this->len_ - 1;
		}
		template<typename _View>
		pf_decl_inline pf_decl_constexpr iterator_t
		insert_back(
		 _View __v)
			requires(is_view_v<_View>)
		{
			return this->insert_back(__v.begin(), __v.end());
		}
		pf_decl_inline pf_decl_constexpr iterator_t
		insert_back(
		 initializer_list<u8char_t> __il)
		{
			return this->insert_back(iterator(__il.begin()), iterator(__il.end()));
		}
		pf_decl_inline pf_decl_constexpr iterator_t
		insert_back(
		 initializer_list<u8code_t> __il)
		{
			return this->insert_back(iterator(__il.begin()), iterator(__il.end()));
		}

		pf_decl_constexpr iterator_t
		insert(
		 iterator_t __w,
		 u8char_t __val,
		 size_t __count = 1)
		{
			// TODO: insert
		}
		pf_decl_constexpr iterator_t
		insert(
		 iterator_t __w,
		 u8code_t __code,
		 size_t __count = 1)
		{
			// TODO: insert
		}
		template<typename _IteratorIn>
		pf_decl_constexpr iterator_t
		insert(
		 iterator_t __w,
		 _IteratorIn __beg,
		 _IteratorIn __end)
			requires(is_iterator_v<_IteratorIn>)
		{
			// TODO: insert
		}
		pf_decl_inline pf_decl_constexpr iterator_t
		insert(
		 iterator_t __w,
		 _View __v)
			requires(is_view_v<_View>)
		{
			return this->insert(__w, __v.begin(), __v.end());
		}
		pf_decl_inline pf_decl_constexpr iterator_t
		insert(
		 iterator_t __w,
		 initializer_list<u8char_t> __il)
		{
			return this->insert(__w, iterator(__il.begin()), iterator(__il.end()));
		}
		pf_decl_inline pf_decl_constexpr iterator_t
		insert(
		 iterator_t __w,
		 initializer_list<u8code_t> __il)
		{
			return this->insert(__w, iterator(__il.begin()), iterator(__il.end()));
		}

		/// Pop
		pf_decl_constexpr size_t
		pop_back() pf_attr_noexcept
		{
			// TODO pop_back
		}

		pf_decl_constexpr size_t
		pop(
		 size_t __w) pf_attr_noexcept
		{
			// TODO pop __w
		}
		pf_decl_constexpr size_t
		pop(
		 size_t __beg,
		 size_t __end) pf_attr_noexcept
		{
			// TODO: pop __beg, __end
		}

		/// Remove
		pf_hint_nodiscard pf_decl_constexpr iterator_t
		remove_back() pf_attr_noexcept
		{
			return this->begin() + this->pop_back();
		}

		/// Replace
		pf_decl_constexpr size_t
		replace(
		 size_t __w,
		 u8char_t __val,
		 size_t __count = 1)
		{
			// TODO: replace
		}
		pf_decl_constexpr size_t
		replace(
		 size_t __w,
		 u8code_t __code,
		 size_t __count = 1)
		{
			// TODO: replace
		}
		template<typename _IteratorIn>
		pf_decl_constexpr size_t
		replace(
		 size_t __w,
		 _IteratorIn __beg,
		 _IteratorIn __end)
			requires(is_iterator_v<_IteratorIn>)
		{
			// TODO: replace
		}
		template<typename _View>
		pf_decl_inline pf_decl_constexpr size_t
		replace(
		 size_t __w,
		 _View __v)
			requires(is_view_v<_View>)
		{
			return this->replace(__w, __v.begin(), __v.end());
		}
		pf_decl_inline pf_decl_constexpr size_t
		replace(
		 size_t __w,
		 initializer_list<u8char_t> __il)
		{
			return this->replace(__w, iterator(__il.begin()), iterator(__il.end()));
		}
		pf_decl_inline pf_decl_constexpr size_t
		replace(
		 size_t __w,
		 initializer_list<u8code_t> __il)
		{
			return this->replace(__w, iterator(__il.begin()), iterator(__il.end()));
		}
		pf_decl_constexpr size_t
		replace(
		 size_t __wbeg,
		 size_t __wend,
		 u8char_t __val,
		 size_t __count = 1)
		{
			// TODO: replace
		}
		pf_decl_constexpr size_t
		replace(
		 size_t __wbeg,
		 size_t __wend,
		 u8code_t __code,
		 size_t __count = 1)
		{
			// TODO: replace
		}
		pf_decl_constexpr size_t
		replace(
		 size_t __wbeg,
		 size_t __wend,
		 _IteratorIn __beg,
		 _IteratorIn __end)
			requires(is_iterator_v<_IteratorIn>)
		{
			// TODO: replace
		}
		template<typename _View>
		pf_decl_inline pf_decl_constexpr size_t
		replace(
		 size_t __wbeg,
		 size_t __wend,
		 _View __view)
			requires(is_view_v<_View>)
		{
			return this->replace(__wbeg, __wend, __view.begin(), __view.end());
		}
		pf_decl_inline pf_decl_constexpr size_t
		replace(
		 size_t __wbeg,
		 size_t __wend,
		 initializer_list<u8char_t> __il)
		{
			return this->replace(__wbeg, __wend, iterator(__il.begin()), iterator(__il.end()));
		}
		pf_decl_inline pf_decl_constexpr size_t
		replace(
		 size_t __wbeg,
		 size_t __wend,
		 initializer_list<u8code_t> __il)
		{
			return this->replace(__wbeg, __wend, iterator(__il.begin()), iterator(__il.end()));
		}

		pf_decl_constexpr iterator_t
		replace(
		 iterator_t __w,
		 u8char_t __val,
		 size_t __count = 1)
		{
			// TODO: replace
		}
		pf_decl_constexpr iterator_t
		replace(
		 iterator_t __w,
		 u8code_t __code,
		 size_t __count = 1)
		{
			// TODO: replace
		}
		template<typename _IteratorIn>
		pf_decl_constexpr iterator_t
		replace(
		 iterator_t __w,
		 _IteratorIn __beg,
		 _IteratorIn __end)
			requires(is_iterator_v<_IteratorIn>)
		{
			// TODO: replace
		}
		template<typename _View>
		pf_decl_inline pf_decl_constexpr iterator_t
		replace(
		 iterator_t __w,
		 _View __v)
			requires(is_view_v<_View>)
		{
			return this->replace(__w, __v.begin(), __v.end());
		}
		pf_decl_inline pf_decl_constexpr iterator_t
		replace(
		 iterator_t __w,
		 initializer_list<u8char_t> __il)
		{
			return this->replace(__w, iterator(__il.begin()), iterator(__il.end()));
		}
		pf_decl_inline pf_decl_constexpr iterator_t
		replace(
		 iterator_t __w,
		 initializer_list<u8code_t> __il)
		{
			return this->replace(__w, iterator(__il.begin()), iterator(__il.end()));
		}
		pf_decl_constexpr iterator_t
		replace(
		 iterator_t __wbeg,
		 iterator_t __wend,
		 u8char_t __val,
		 size_t __count = 1)
		{
			// TODO: replace
		}
		pf_decl_constexpr iterator_t
		replace(
		 iterator_t __wbeg,
		 iterator_t __wend,
		 u8code_t __val,
		 size_t __count = 1)
		{
			// TODO: replace
		}
		template<typename _IteratorIn>
		pf_decl_constexpr iterator_t
		replace(
		 iterator_t __wbeg,
		 iterator_t __wend,
		 _IteratorIn __beg,
		 _IteratorIn __end)
			requires(is_iterator_v<_IteratorIn>)
		{
			// TODO: replace
		}
		template<typename _View>
		pf_decl_inline pf_decl_constexpr iterator_t
		replace(
		 iterator_t __wbeg,
		 iterator_t __wend,
		 _View __v)
			requires(is_view_v<_View>)
		{
			return this->replace(__wbeg, __wend, __v.begin(), __v.end());
		}
		pf_decl_inline pf_decl_constexpr iterator_t
		replace(
		 iterator_t __wbeg,
		 iterator_t __wend,
		 initializer_list<u8char_t> __il)
		{
			return this->replace(__wbeg, __wend, iterator(__il.begin()), iterator(__il.end()));
		}
		pf_decl_inline pf_decl_constexpr iterator_t
		replace(
		 iterator_t __wbeg,
		 iterator_t __wend,
		 initializer_list<u8code_t> __il)
		{
			return this->replace(__wbeg, __wend, iterator(__il.begin()), iterator(__il.end()));
		}

		/// Shrink
		pf_decl_constexpr size_t
		shrink(
		 size_t __nc)
		{
			if(pf_unlikely(__nc < this->len_)) __nc = this->len_;
			if(__nc < this->capacity_) return -this->__reallocate_no_check(__nc, this->align_);
			return 0;
		}
		pf_decl_constexpr size_t
		shrink_to_fit()
		{
			return this->shrink(this->len_);
		}
		pf_decl_constexpr size_t
		shrink_to_magnifier()
		{
			return this->shrink(this->magnifier_(this->len_));
		}

		/// Reserve
		pf_decl_constexpr size_t
		reserve(
		 size_t __len)
		{
			if(__len > this->capacity_)
			{
				return this->__reallocate_no_check(__len, this->align_);
			}
			return 0;
		}
		pf_decl_constexpr size_t
		reserve(
		 size_t __len,
		 align_val_t __align)
		{
			if(__len > this->capacity_)
			{
				return this->__reallocate_no_check(__len, __align);
			}
			if(__align != this->align_)
			{
				this->realign(__align);
			}
			return 0;
		}
		pf_decl_constexpr size_t
		reserve_with_magnifier(
		 size_t __len)
		{
			if(__len > this->capacity_)
			{
				return this->__reallocate_no_check(this->magnifier_(__len), this->align_);
			}
			return 0;
		}
		pf_decl_constexpr size_t
		reserve_with_magnifier(
		 size_t __len,
		 align_val_t __align)
		{
			if(__len > this->capacity_)
			{
				return this->__reallocate_no_check(this->magnifier_(__len), __align);
			}
			if(__align != this->align_)
			{
				this->realign(__align);
			}
			return 0;
		}

		/// Resize
		pf_decl_constexpr size_t
		resize(
		 u8char_t __val,
		 size_t __count)
		{
			// TODO: resize __val, __count
			return -1;
		}
		pf_decl_constexpr size_t
		resize(
		 u8code_t __val,
		 size_t __count)
		{
			// TODO: resize __val, __count
			return -1;
		}
		pf_decl_constexpr size_t
		resize_with_magnifier(
		 u8char_t __val,
		 size_t __count)
		{
			return this->resize(__val, this->magnifier_(__count));
		}
		pf_decl_constexpr size_t
		resize_with_magnifier(
		 u8code_t __val,
		 size_t __count)
		{
			return this->resize(__val, this->magnifier_(__count));
		}

		/// Realign
		pf_decl_constexpr bool
		realign(
		 align_val_t __align)
		{
			const u8char_t *p = this->data_;
			if(this->align_ != __align)
				this->__reallocate_no_check(this->capacity_, __align);
			return (this->data_ != p);
		}

		/// Clear
		pf_decl_constexpr size_t
		clear() pf_attr_noexcept
		{
			const size_t c = this->len_;
			if(this->data_)
			{
				destroy(this->begin(), this->end());
				deallocate(this->allocator_, this->data_);
				this->data_			= nullptr;
				this->capacity_ = 0;
				this->len_			= 0;
			}
			return c;
		}

		/// Front
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr u8char_t &
		front() pf_attr_noexcept
		{
			return *this->begin();
		}
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const u8char_t &
		front() const pf_attr_noexcept
		{
			return *this->begin();
		}

		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr auto
		code_front() pf_attr_noexcept
		{
			return *this->code_begin();
		}
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr auto
		code_front() const pf_attr_noexcept
		{
			return *this->code_begin();
		}

		/// Back
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr u8char_t &
		back() pf_attr_noexcept
		{
			return *this->rbegin();
		}
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const u8char_t &
		back() const pf_attr_noexcept
		{
			return *this->rbegin();
		}

		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr auto
		code_back() pf_attr_noexcept
		{
			return *this->code_rbegin();
		}
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr auto
		code_back() const pf_attr_noexcept
		{
			return *this->code_rbegin();
		}

		/// Begin
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr iterator_t
		begin() pf_attr_noexcept
		{
			return this->data_;
		}
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const_iterator_t
		begin() const pf_attr_noexcept
		{
			return this->data_;
		}
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const_iterator_t
		cbegin() const pf_attr_noexcept
		{
			return this->begin();
		}

		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr u8iterator_t
		code_begin() pf_attr_noexcept
		{
			return { this->data_, this };
		}
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const_u8iterator_t
		code_begin() const pf_attr_noexcept
		{
			return { this->data_, this };
		}
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const_u8iterator_t
		code_cbegin() const pf_attr_noexcept
		{
			return this->code_begin();
		}

		/// End
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr iterator_t
		end() pf_attr_noexcept
		{
			return this->data_ + this->len_;
		}
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const_iterator_t
		end() const pf_attr_noexcept
		{
			return this->data_ + this->len_;
		}
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const_iterator_t
		cend() const pf_attr_noexcept
		{
			return this->end();
		}

		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr u8iterator_t
		code_end() pf_attr_noexcept
		{
			return this->data_ + this->len_;
		}
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const_u8iterator_t
		code_end() const pf_attr_noexcept
		{
			return this->data_ + this->len_;
		}
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const_u8iterator_t
		code_cend() const pf_attr_noexcept
		{
			return this->code_end();
		}

		/// Reverse Begin
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr reverse_iterator_t
		rbegin() pf_attr_noexcept
		{
			return this->data_ + this->len_ - 1;
		}
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const_reverse_iterator_t
		rbegin() const pf_attr_noexcept
		{
			return this->data_ + this->len_ - 1;
		}
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const_reverse_iterator_t
		crbegin() const pf_attr_noexcept
		{
			return this->rbegin();
		}

		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr reverse_u8iterator_t
		code_rbegin() pf_attr_noexcept
		{
			u8char_t *p = this->data_ + this->len_;
			return u8iterator_t { p - utf8::prev(p), this };
		}
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const_reverse_u8iterator_t
		code_rbegin() const pf_attr_noexcept
		{
			const u8char_t *p = this->data_ + this->len_;
			return const_u8iterator_t { p - utf8::prev(p), this };
		}
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const_reverse_u8iterator_t
		code_crbegin() const pf_attr_noexcept
		{
			return this->code_rbegin();
		}

		/// Reverse End
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr reverse_iterator_t
		rend() pf_attr_noexcept
		{
			return this->data_ - 1;
		}
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const_reverse_iterator_t
		rend() const pf_attr_noexcept
		{
			return this->data_ - 1;
		}
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const_reverse_iterator_t
		crend() const pf_attr_noexcept
		{
			return this->rend();
		}

		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr reverse_u8iterator_t
		code_rend() pf_attr_noexcept
		{
			return this->data_ - 1;
		}
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const_reverse_u8iterator_t
		code_rend() const pf_attr_noexcept
		{
			return this->data_ - 1;
		}
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const_reverse_u8iterator_t
		code_crend() const pf_attr_noexcept
		{
			return this->code_rend();
		}

		/// Data
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr u8char_t *
		data() pf_attr_noexcept
		{
			return this->data_;
		}
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const u8char_t *
		data() const pf_attr_noexcept
		{
			return this->data_;
		}

		/// Length
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr size_t
		length() const pf_attr_noexcept
		{
			return this->len_;
		}

		/// Count
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr size_t
		count() const pf_attr_noexcept
		{
			return utf8::countof(this->data_, this->len_);
		}

		/// Storage Size
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr size_t
		storage_size() const pf_attr_noexcept
		{
			return this->capacity_;
		}

		/// Capacity
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr size_t
		capacity() const pf_attr_noexcept
		{
			return this->capacity_;
		}

		/// Align
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr align_val_t
		alignment() const pf_attr_noexcept
		{
			return this->align_;
		}

		/// View
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr view_t
		view() const pf_attr_noexcept
		{
			return view_t(this->begin(), this->end());
		}

		/// Magnifier
		pf_hint_nodiscard pf_decl_inline _Magnifier &
		magnifier() pf_attr_noexcept
		{
			return this->magnifier_;
		}
		pf_hint_nodiscard pf_decl_inline const _Magnifier &
		magnifier() const pf_attr_noexcept
		{
			return this->magnifier_;
		}

		/// Allocator
		pf_hint_nodiscard pf_decl_inline _Allocator &
		allocator() pf_attr_noexcept
		{
			return this->allocator_;
		}
		pf_hint_nodiscard pf_decl_inline const _Allocator &
		allocator() const pf_attr_noexcept
		{
			return this->allocator_;
		}

		/// Is Allocated
		pf_hint_nodiscard pf_decl_inline bool
		is_allocated() const pf_attr_noexcept
		{
			return this->capacity_ != 0;
		}

		/// Is Empty
		pf_hint_nodiscard pf_decl_inline bool
		is_empty() const pf_attr_noexcept
		{
			return this->len_ == 0;
		}

	private:
		u8char_t *data_;
		size_t capacity_;
		align_val_t align_;
		size_t len_;
		pf_hint_nounique_address _Magnifier magnifier_;
		pf_hint_nounique_address _Allocator allocator_;
	};


	/// CHAR: Tests
	pf_assert_static(is_iterator_v<u8iterator<u8string_view>>);
	pf_assert_static(is_iterator_v<const_u8iterator<u8string_view>>);
	pf_assert_static(is_iterator_v<reverse_iterator<u8iterator<u8string_view>>>);
	pf_assert_static(is_iterator_v<reverse_iterator<const_u8iterator<u8string_view>>>);
	pf_assert_static(is_view_v<u8string_view>);
	pf_assert_static(is_container_v<u8string<>>);


	/// CHAR: Format - Types
	template<typename... _Args>
	using u8format_string = fmt::format_string<_Args...>;
	using u8style_t				= fmt::text_style;

	/// CHAR: Format
	pf_hint_nodiscard pf_decl_inline pf_decl_constexpr u8style_t
	u8fg(
	 fmt::detail::color_type __fg) pf_attr_noexcept
	{
		return fmt::fg(__fg);
	}
	pf_hint_nodiscard pf_decl_inline pf_decl_constexpr u8style_t
	u8bg(
	 fmt::detail::color_type __bg) pf_attr_noexcept
	{
		return fmt::bg(__bg);
	}
	template<typename _Ty>
	pf_hint_nodiscard pf_decl_inline pf_decl_constexpr auto
	u8styled(
	 const _Ty &__val,
	 u8style_t __style) pf_attr_noexcept
	{
		return fmt::styled(__val, __style);
	}
	template<typename... _Args>
	pf_hint_nodiscard pf_decl_inline pf_decl_constexpr size_t
	u8formatted_size(
	 u8format_string<_Args...> __fmt,
	 _Args &&...__args)
	{
		return fmt::formatted_size(__fmt, std::forward<_Args>(__args)...);
	}
	template<typename... _Args>
	pf_hint_nodiscard pf_decl_inline pf_decl_constexpr u8string<>
	u8format(
	 u8format_string<_Args...> __fmt,
	 _Args &&...__args)
	{
		const size_t r = fmt::formatted_size(__fmt, std::forward<_Args>(__args)...);
		u8string<> str('\0', r);
		fmt::format_to(str.data(), __fmt, std::forward<_Args>(__args)...);
		return str;
	}
	template<typename _IteratorOut, typename... _Args>
	pf_decl_inline pf_decl_constexpr auto
	u8format_to(
	 _IteratorOut __out,
	 u8format_string<_Args...> __fmt,
	 _Args &&...__args)
	{
		return fmt::format_to(__out, __fmt, std::forward<_Args>(__args)...);
	}

}	 // namespace pul

#endif	// !PULSAR_CHAR_HPP
