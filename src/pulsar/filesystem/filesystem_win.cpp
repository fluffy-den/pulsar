/*! @file   filesystem_win.cpp
 *  @author Louis-Quentin Noé (noe.louis-quentin@hotmail.fr)
 *  @brief
 *  @date   30-05-2023
 *
 *  @copyright Copyright (c) 2023 - Pulsar Software
 *
 *  @since 0.1.8
 */

// Include: Pulsar
#include "pulsar/internal.hpp"

// Windows
#ifdef PF_OS_WINDOWS
 #include <windows.h>
 #include <shlwapi.h>
 #include <fileapi.h>
 #include <winioctl.h>
 #include <sddl.h>

 // Fmt
 #include <fmt/xchar.h>

// Pulsar
namespace pul
{
	// Filesystem
	namespace fs
	{
		// NOTE: https://googleprojectzero.blogspot.com/2016/02/the-definitive-guide-on-win32-to-nt.html

		/// Constants
		pf_decl_constexpr size_t __MAX_UNICODE_PATH_LEN	 = 32'767;
		pf_decl_constexpr size_t __BUFFER_SIZE					 = __MAX_UNICODE_PATH_LEN * 2;
		pf_decl_constexpr size_t __ITERATION_BUFFER_SIZE = 262'136;

		/// FILESYSTEM: Utility -> Throw
		pf_decl_inline void
		__throw_error_utf16le_to_utf8(
		 char_error_t err)
		{
			pf_throw(
			 dbg_category_char(),
			 err.code,
			 0,
			 "[WIN] Cast from utf16-le to utf8 failed! At position={}",
			 err.position);
		}
		pf_decl_inline void
		__throw_error_utf8_to_utf16le(
		 char_error_t err)
		{
			pf_throw(
			 dbg_category_char(),
			 err.code,
			 0,
			 "[WIN] Cast from utf8 to utf16-le failed! At position={}",
			 err.position);
		}

		/// FILESYSTEM: Utility -> Convert
		void
		__convert_by_equality_u16_win(
		 u16char_t *__str,
		 size_t __len,
		 u16char_t __from,
		 u16char_t __to) pf_attr_noexcept
		{
			pf_assert(uint16_t(__from) <= 127 || uint16_t(__to) <= 127, "__convert_by_equality work only with ascii values! __from={}, __to={}", uint16_t(__from), uint16_t(__to));

			size_t p = 0;
			if(instruction_set::simd & instruction_set::AVX2_BIT)
			{
				// NOTE: AVX512

				for(; p + 16 <= __len; p += 16)
				{
					m256i64_t *w				 = union_cast<m256i64_t *>(&__str[p]);
					const m256i64_t seq	 = _mm256_lddqu_si256(w);
					const m256i64_t mask = _mm256_cmpeq_epi8(seq, _mm256_set1_epi8(union_cast<u8char_t>(__from)));
					const m256i64_t out	 = _mm256_blendv_epi8(seq, _mm256_set1_epi8(union_cast<u8char_t>(__to)), mask);
					_mm256_storeu_si256(w, out);
				}
				if(p < __len)
				{
					m256i64_t *w	 = union_cast<m256i64_t *>(&__str[p]);
					const size_t r = (__len - p) * sizeof(u16char_t);
					m256i64_t seq	 = _mm256_setzero_si256();
					memcpy(&seq, w, r);
					const m256i64_t mask = _mm256_cmpeq_epi8(seq, _mm256_set1_epi8(union_cast<u8char_t>(__from)));
					const m256i64_t out	 = _mm256_blendv_epi8(seq, _mm256_set1_epi8(union_cast<u8char_t>(__to)), mask);
					memcpy(w, &out, r);
				}
				return;
			}
			if(instruction_set::simd & instruction_set::SSE2_BIT)
			{
				for(; p + 8 <= __len; p += 8)
				{
					m128i64_t *w				 = union_cast<m128i64_t *>(&__str[p]);
					const m128i64_t seq	 = _mm_lddqu_si128(w);
					const m128i64_t mask = _mm_cmpeq_epi8(seq, _mm_set1_epi8(union_cast<u8char_t>(__from)));
					const m128i64_t out	 = _mm_blendv_epi8(seq, _mm_set1_epi8(union_cast<u8char_t>(__to)), mask);
					_mm_storeu_si128(w, out);
				}
				if(p < __len)
				{
					m128i64_t *w	 = union_cast<m128i64_t *>(&__str[p]);
					const size_t r = (__len - p) * sizeof(u16char_t);
					m128i64_t seq	 = _mm_setzero_si128();
					memcpy(&seq, w, r);
					const m128i64_t mask = _mm_cmpeq_epi8(seq, _mm_set1_epi8(union_cast<u8char_t>(__from)));
					const m128i64_t out	 = _mm_blendv_epi8(seq, _mm_set1_epi8(union_cast<u8char_t>(__to)), mask);
					memcpy(w, &out, r);
				}
				return;
			}
			for(; p < __len; ++p)
			{
				if(__str[p] == __from) __str[p] = __to;
			}
		}
		pf_decl_always_inline void
		__convert_forwardslash_to_backslash_u16_win(
		 WCHAR *__str,
		 size_t __len) pf_attr_noexcept
		{
			return __convert_by_equality_u16_win(union_cast<u16char_t *>(__str), __len, u'/', u'\\');
		}
		void
		__convert_by_equality_u8_win(
		 u8char_t *__str,
		 size_t __len,
		 u8char_t __from,
		 u8char_t __to) pf_attr_noexcept
		{
			pf_assert(uint8_t(__from) <= 127 || uint8_t(__to) <= 127, "__convert_by_equality work only with ascii values! __from={}, __to={}", uint8_t(__from), uint8_t(__to));

			size_t p = 0;
			if(instruction_set::simd & instruction_set::AVX2_BIT)
			{
				// NOTE: AVX512

				for(; p + 32 <= __len; p += 32)
				{
					m256i64_t *w				 = union_cast<m256i64_t *>(&__str[p]);
					const m256i64_t seq	 = _mm256_lddqu_si256(w);
					const m256i64_t mask = _mm256_cmpeq_epi8(seq, _mm256_set1_epi8(__from));
					const m256i64_t out	 = _mm256_blendv_epi8(seq, _mm256_set1_epi8(__to), mask);
					_mm256_storeu_si256(w, out);
				}
				if(p < __len)
				{
					const size_t r = __len - p;
					m256i64_t *w	 = union_cast<m256i64_t *>(&__str[p]);
					m256i64_t seq	 = _mm256_setzero_si256();
					memcpy(&seq, w, r);
					const m256i64_t mask = _mm256_cmpeq_epi8(seq, _mm256_set1_epi8(__from));
					const m256i64_t out	 = _mm256_blendv_epi8(seq, _mm256_set1_epi8(__to), mask);
					memcpy(w, &out, r);
				}
				return;
			}
			if(instruction_set::simd & instruction_set::SSE2_BIT)
			{
				for(; p + 16 <= __len; p += 16)
				{
					m128i64_t *w				 = union_cast<m128i64_t *>(&__str[p]);
					const m128i64_t seq	 = _mm_lddqu_si128(w);
					const m128i64_t mask = _mm_cmpeq_epi8(seq, _mm_set1_epi8(__from));
					const m128i64_t out	 = _mm_blendv_epi8(seq, _mm_set1_epi8(__to), mask);
					_mm_storeu_si128(w, out);
				}
				if(p < __len)
				{
					const size_t r = __len - p;
					m128i64_t *w	 = union_cast<m128i64_t *>(&__str[p]);
					m128i64_t seq	 = _mm_setzero_si128();
					memcpy(&seq, w, r);
					const m128i64_t mask = _mm_cmpeq_epi8(seq, _mm_set1_epi8(__from));
					const m128i64_t out	 = _mm_blendv_epi8(seq, _mm_set1_epi8(__to), mask);
					memcpy(w, &out, r);
				}
				return;
			}
			for(; p < __len; ++p)
			{
				if(__str[p] == __from) __str[p] = __to;
			}
		}
		pf_decl_always_inline void
		__convert_backslash_to_forwardslash_u8_win(
		 u8char_t *__str,
		 size_t __len) pf_attr_noexcept
		{
			return __convert_by_equality_u8_win(__str, __len, '\\', '/');
		}

		/// FILESYSTEM: Utility -> Cast
		pf_hint_nodiscard unique_ptr_s<WCHAR[]>
		__cast_utf8_to_wchar_buffer_ex_win(
		 path_view __v,
		 size_t &__len,
		 size_t __exlen)
		{
			// Utf8 -> Utf16-le
			__len		 = utf8_to_utf16le::required_count(__v.data(), __v.length()) + 5 + __exlen;
			auto buf = make_unique_s<WCHAR[]>(__len);
			buf[0u]	 = L'\\';
			buf[1u]	 = L'\\';
			buf[2u]	 = L'?';
			buf[3u]	 = L'\\';
			utf8_to_utf16le::convert(__v.data(), __v.length(), union_cast<u16char_t *>(&buf[0u] + 4));
			__convert_forwardslash_to_backslash_u16_win(&buf[0u] + 4, __len - 5);
			buf[__len - 1u] = L'\0';
			return buf;
		}
		pf_hint_nodiscard pf_decl_inline unique_ptr_s<WCHAR[]>
		__cast_utf8_to_wchar_buffer_win(
		 path_view __v,
		 size_t &__len)
		{
			return __cast_utf8_to_wchar_buffer_ex_win(__v, __len, 0);
		}
		pf_hint_nodiscard path<>
		__cast_utf16le_to_utf8_buffer_win(
		 WCHAR *__wbuf,
		 size_t __wlen,
		 size_t __offset = 4)
		{
			const size_t len = utf16le_to_utf8::required_count(union_cast<u16char_t *>(&__wbuf[0] + __offset), __wlen);
			path p('\0', len);
			utf16le_to_utf8::convert(union_cast<u16char_t *>(&__wbuf[0]) + __offset, len, p.data());
			__convert_backslash_to_forwardslash_u8_win(p.data(), len);
			return p;
		}


		/// FILESYSTEM: Pathing
		pulsar_api path<>
		absolute(
		 path_view __v)
		{
			size_t len = 0;
			auto buf	 = __cast_utf8_to_wchar_buffer_win(__v, len);

			// GetFullPathNameW
			const DWORD castlen = GetLongPathNameW(&buf[0u], nullptr, 0) - 1;
			auto castbuf				= make_unique_s<WCHAR[]>(castlen);
			DWORD castres				= GetLongPathNameW(&buf[0u], &castbuf[0u], castlen);
			pf_throw_if(
			 !castres,
			 dbg_category_system(),
			 GetLastError(),
			 dbg_flags::dump_with_handle_data,
			 "[WIN] GetFullPathNameW failed!");

			// Utf16-le - Utf8
			return __cast_utf16le_to_utf8_buffer_win(castbuf, castlen);
		}
		pulsar_api path<>
		relative(
		 path_view __v)
		{
			// Utf8 - Utf16le
			size_t len = 0;
			auto buf	 = __cast_utf8_to_wchar_buffer_win(__v, len);

			// GetShortPathNameW
			const DWORD castlen = GetShortPathNameW(&buf[0u], nullptr, 0);
			auto castbuf				= make_unique_s<WCHAR[]>(castlen);
			DWORD castres				= GetShortPathNameW(&buf[0u], &castbuf[0u], castlen);
			pf_throw_if(
			 !castres,
			 dbg_category_system(),
			 GetLastError(),
			 dbg_flags::dump_with_handle_data,
			 "[WIN] GetShortPathNameW failed!");

			// Utf16-le - Utf8
			return __cast_utf16le_to_utf8_buffer_win(castbuf, castlen);
		}

		// Attributes
		pf_hint_nodiscard attribute_t
		__to_pul_attributes_win(
		 DWORD __attrs) pf_attr_noexcept
		{
			if(__attrs == INVALID_FILE_ATTRIBUTES) return attribute::dont_exist;
			attribute_t flags = attribute::none;
			if(__attrs & FILE_ATTRIBUTE_READONLY) flags |= attribute::readonly;
			if(__attrs & FILE_ATTRIBUTE_HIDDEN) flags |= attribute::hidden;
			if(__attrs & FILE_ATTRIBUTE_SYSTEM) flags |= attribute::system;
			if(__attrs & FILE_ATTRIBUTE_DIRECTORY) flags |= attribute::directory;
			if(__attrs & FILE_ATTRIBUTE_ARCHIVE) flags |= attribute::archive;
			if(__attrs & FILE_ATTRIBUTE_TEMPORARY) flags |= attribute::temporary;
			return flags;
		}
		pulsar_api attribute_t
		attributes(
		 path_view __v)
		{
			// Utf8 - Utf16le
			pf_hint_maybe_unused size_t len = 0;
			auto buf												= __cast_utf8_to_wchar_buffer_win(__v, len);

			// GetFileAttributesW
			DWORD attrs = GetFileAttributesW(&buf[0u]);
			return __to_pul_attributes_win(attrs);
		}
		pf_hint_nodiscard pf_decl_inline int32_t
		__calculate_day_of_year(
		 SYSTEMTIME const &__s)
		{
			int32_t d		 = __s.wDay;
			int32_t md[] = { 31, 31, 31, 30, 30, 28, 30, 30, 31, 31, 31 };
			for(int32_t i = 1; i < __s.wMonth - 1; ++i)
			{
				d += md[i];
			}
			if(__s.wMonth >= 7 && ((__s.wYear & 3) == 0) && (((__s.wYear % 100) != 0) || ((__s.wYear % 400) == 0))) ++d;
			return d;
		}
		pf_hint_nodiscard pf_decl_inline tm_t
		__system_time_to_tm_win(
		 FILETIME const &__t)
		{
			SYSTEMTIME s;
			pf_throw_if(
			 !FileTimeToSystemTime(&__t, &s) && GetLastError(),
			 dbg_category_system(),
			 GetLastError(),
			 dbg_flags::dump_with_handle_data,
			 "[WIN] FileTimeToSystemTime failed!");
			pf_throw_if(
			 !SystemTimeToTzSpecificLocalTime(nullptr, &s, &s),
			 dbg_category_system(),
			 GetLastError(),
			 dbg_flags::dump_with_handle_data,
			 "[WIN] SystemTimeToTzSpecificLocalTime failed!");
			return {
				s.wSecond,
				s.wMinute,
				s.wHour,
				s.wDay,
				s.wMonth,
				s.wYear,
				s.wDayOfWeek,
				__calculate_day_of_year(s),
				-1
			};
		}
		pulsar_api file_info_t
		information(
		 path_view __v)
		{
			// Utf8 - Utf16le
			pf_hint_maybe_unused size_t len = 0;
			WCHAR *buf											= __cast_utf8_to_wchar_buffer_win(__v, len);

			// GetFileAttributesExW
			WIN32_FILE_ATTRIBUTE_DATA data;
			const DWORD res = GetFileAttributesExW(&buf[0], GetFileExInfoStandard, &data);
			destroy_delete_s<WCHAR[]>(buf);
			pf_throw_if(
			 res == 0,
			 dbg_category_system(),
			 GetLastError(),
			 dbg_flags::dump_with_handle_data,
			 "[WIN] GetFileAttributesExW failed!");
			const uint64_t size = (union_cast<uint64_t>(data.nFileSizeHigh) << 32) + data.nFileSizeLow;
			return file_info_t {
				__system_time_to_tm_win(data.ftCreationTime),
				__system_time_to_tm_win(data.ftLastAccessTime),
				__system_time_to_tm_win(data.ftLastWriteTime),
				size,
				__to_pul_attributes_win(data.dwFileAttributes)
			};
		}

		/// FILESYSTEM: Utility -> Symlink
		pf_hint_nodiscard HANDLE
		__get_symlink_len(
		 const WCHAR *__path,
		 size_t &__len,
		 DWORD __attribs)
		{
			// File
			HANDLE file = CreateFileW(
			 __path,
			 GENERIC_READ,
			 FILE_SHARE_READ,
			 nullptr,
			 OPEN_EXISTING,
			 __attribs | (__attribs & FILE_ATTRIBUTE_DIRECTORY ? FILE_FLAG_BACKUP_SEMANTICS : 0),
			 nullptr);
			pf_throw_if(
			 file == INVALID_HANDLE_VALUE,
			 dbg_category_system(),
			 GetLastError(),
			 0,
			 "[WIN] CreateFileW failed!");

			// IO
			__len = union_cast<size_t>(GetFinalPathNameByHandleW(
			 file,
			 nullptr,
			 0,
			 VOLUME_NAME_DOS | FILE_NAME_NORMALIZED));

			// Ends
			return file;
		}
		void
		__write_symlink_real_path(
		 HANDLE __handle,
		 WCHAR *__buf,
		 size_t __buflen)
		{
			// Retrieve real symlink path
			pf_throw_if(
			 !GetFinalPathNameByHandleW(
				__handle,
				&__buf[0u],
				union_cast<DWORD>(__buflen),
				VOLUME_NAME_DOS | FILE_NAME_NORMALIZED),
			 dbg_category_system(),
			 GetLastError(),
			 0,
			 "[WIN] GetFinalPathNameByHandleW failed!");

			// Close
			pf_throw_if(
			 !CloseHandle(__handle),
			 dbg_category_system(),
			 GetLastError(),
			 0,
			 "[WIN] CloseHandle failed!");
		}
		pf_hint_nodiscard unique_ptr_s<WCHAR[]>
		__make_symlink_path_buffer_s_win(
		 const WCHAR *__path,
		 size_t &__len,
		 DWORD __attribs)
		{
			// Retrive real symlink path into unique_ptr_s
			HANDLE handle = __get_symlink_len(__path, __len, __attribs);
			auto realpath = make_unique_s<WCHAR[]>(__len);
			__write_symlink_real_path(handle, realpath.get(), __len);

			// End
			return realpath;
		}


		/// FILESYSTEM: Iterate -> Controller
		struct __iteration_controller_win_t
		{
			/// Constructors
			pf_decl_inline
			__iteration_controller_win_t(
			 size_t __wlen,
			 size_t __ulen) pf_attr_noexcept
				: handle(nullptr)
				, wlen(__wlen)
				, ulen(__ulen)
			{}
			__iteration_controller_win_t(__iteration_controller_win_t const &) = delete;
			__iteration_controller_win_t(__iteration_controller_win_t &&)			 = delete;

			/// Destructor
			pf_decl_inline ~__iteration_controller_win_t() pf_attr_noexcept = default;

			/// Operator =
			__iteration_controller_win_t &
			operator=(__iteration_controller_win_t const &) = delete;
			__iteration_controller_win_t &
			operator=(__iteration_controller_win_t &&) = delete;

			/// Buffers
			pf_hint_nodiscard pf_decl_inline WCHAR *
			get_wroot() pf_attr_noexcept
			{
				return union_cast<WCHAR *>(&this->data[0]);
			}
			pf_hint_nodiscard pf_decl_inline u8char_t *
			get_uroot() pf_attr_noexcept
			{
				return union_cast<u8char_t *>(&this->data[0] + this->wlen * sizeof(u16char_t));
			}
			pf_hint_nodiscard pf_decl_inline u8char_t *
			get_ubuf() pf_attr_noexcept
			{
				return union_cast<u8char_t *>(&this->data[0] + this->wlen * sizeof(u16char_t) + this->ulen);
			}

			/// Data
			void *handle;
			size_t wlen;
			size_t ulen;
			bool needupdate;
			byte_t data[];
		};


		find_info_t
		__iterate_write_entry(
		 __iteration_controller_win_t *__ctrl,
		 WIN32_FIND_DATAW &__data)
		{
			// Entry
			find_info_t entry;
			try
			{
				entry.creationTime	 = __system_time_to_tm_win(__data.ftCreationTime);
				entry.lastAccessTime = __system_time_to_tm_win(__data.ftLastAccessTime);
				entry.lastWriteTime	 = __system_time_to_tm_win(__data.ftLastWriteTime);
				entry.fileSize			 = (union_cast<uint64_t>(__data.nFileSizeHigh) << 32) + __data.nFileSizeLow;
				entry.attributes		 = __to_pul_attributes_win(__data.dwFileAttributes);

				// Path
				const size_t wnamelen = utf16le::lenof(union_cast<u16char_t *>(&__data.cFileName[0u]));
				auto ubuf							= __ctrl->get_ubuf();

				// Reparse Point
				if(__data.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT)
				{
					auto wroot					 = __ctrl->get_wroot();
					const size_t currlen = __ctrl->wlen + wnamelen - 2;
					auto currpath				 = make_unique_s<WCHAR[]>(currlen);
					memcpy(&currpath[0u], &wroot[0u], __ctrl->wlen * sizeof(u16char_t));
					memcpy(&currpath[__ctrl->wlen - 2u], &__data.cFileName[0u], wnamelen * sizeof(u16char_t));
					size_t fulllen;
					auto fullpath = __make_symlink_path_buffer_s_win(currpath, fulllen, __data.dwFileAttributes);
					utf16le_to_utf8::convert(union_cast<u16char_t *>(&fullpath[0u] + 4), fulllen - 4, &ubuf[0u]);
					const size_t ulen = utf8::lenof(&ubuf[0u]);
					__convert_backslash_to_forwardslash_u8_win(&ubuf[0u], ulen);
					entry.path				 = path_view(&ubuf[0u], ulen - 1);
					__ctrl->needupdate = true;
				}

				// Common
				else
				{
					const size_t unamelen = utf16le_to_utf8::required_count(union_cast<u16char_t *>(&__data.cFileName[0u]), wnamelen);
					if(__ctrl->needupdate)
					{
						auto uroot = __ctrl->get_uroot();
						memcpy(&ubuf[0u], &uroot[0u], __ctrl->ulen - 1);
						__ctrl->needupdate = false;
					}
					utf16le_to_utf8::convert(union_cast<u16char_t *>(&__data.cFileName[0u]), wnamelen, &ubuf[0u] + __ctrl->ulen - 1);
					entry.path = path_view(&ubuf[0u], __ctrl->ulen + unamelen - 1);
				}
			} catch(std::exception const &)
			{
				FindClose(__ctrl->handle);
				destroy_delete_s(__ctrl);
				throw;
			}
			return entry;
		}
		pulsar_api void *
		__iterate_initialize(
		 path_view __v,
		 entry_t &__entry)
		{
			// Controller
			const size_t wlen = utf8_to_utf16le::required_count(__v.data(), __v.length()) + 7;
			const size_t ulen = __v.length() + 2;
			auto ctrl					= new_construct_ex_s<__iteration_controller_win_t>(wlen + ulen + __MAX_UNICODE_PATH_LEN, wlen, ulen);
			auto wroot				= ctrl->get_wroot();
			auto uroot				= ctrl->get_uroot();

			// Initialize root paths
			try
			{
				// Wide
				wroot[0u] = L'\\';
				wroot[1u] = L'\\';
				wroot[2u] = L'?';
				wroot[3u] = L'\\';
				utf8_to_utf16le::convert(__v.data(), __v.length(), union_cast<u16char_t *>(&wroot[0u] + 4));
				__convert_forwardslash_to_backslash_u16_win(&wroot[0u] + 4, ctrl->wlen - 7);

				// End
				wroot[ctrl->wlen - 3u] = L'\\';
				wroot[ctrl->wlen - 2u] = L'*';
				wroot[ctrl->wlen - 1u] = L'\0';

				// Utf-8
				memcpy(&uroot[0u], __v.data(), __v.length());
				uroot[ctrl->ulen - 2u] = '/';
				uroot[ctrl->ulen - 1u] = '\0';
			} catch(std::exception const &)
			{
				destroy_delete_s(ctrl);
				throw;
			}

			// Initialize handle
			WIN32_FIND_DATAW data;
			ctrl->handle = FindFirstFileExW(&wroot[0u], FindExInfoBasic, &data, FindExSearchNameMatch, nullptr, FIND_FIRST_EX_CASE_SENSITIVE | FIND_FIRST_EX_LARGE_FETCH);
			if(ctrl->handle == INVALID_HANDLE_VALUE)
			{
				destroy_delete_s(ctrl);
				pf_throw(
				 dbg_category_system(),
				 GetLastError(),
				 0,
				 "FindFirstFileExW failed!");
			}

			// Skip '.', '..' and invalid files.
			if(data.cFileName[0] == L'.' || ((data.dwFileAttributes & FILE_ATTRIBUTE_NORMAL) && (data.dwFileAttributes & ~FILE_ATTRIBUTE_NORMAL)))
			{
				return __iterate_next(ctrl, __entry);
			}
			else
			{
				__entry = __iterate_write_entry(ctrl, data);
				return ctrl;
			}
		}
		pulsar_api void *
		__iterate_next(
		 void *__ctrl,
		 entry_t &__entry)
		{
			// Controller
			auto ctrl = union_cast<__iteration_controller_win_t *>(__ctrl);

			// Next
			WIN32_FIND_DATAW data;
			while(true)
			{
				if(!FindNextFileW(ctrl->handle, &data))
				{
					FindClose(ctrl->handle);
					destroy_delete_s(ctrl);
					return nullptr;
				}
				if(data.cFileName[0] == L'.' || ((data.dwFileAttributes & FILE_ATTRIBUTE_NORMAL) && (data.dwFileAttributes & ~FILE_ATTRIBUTE_NORMAL))) continue;

				// Entry
				__entry = __iterate_write_entry(ctrl, data);
				return ctrl;
			}
		}
		pulsar_api void
		__iterate_close(
		 void *__ctrl) pf_attr_noexcept
		{
			// Controller
			auto ctrl = union_cast<__iteration_controller_win_t *>(__ctrl);

			// Close
			FindClose(ctrl->handle);
			destroy_delete_s(ctrl);
		}

		// Iterate -> Recursive
		struct __iteration_rec_folder_win_t
		{
			/// Constructors
			pf_decl_inline
			__iteration_rec_folder_win_t(
			 size_t __wlen,
			 size_t __ulen)
				: next(nullptr)
				, wlen(__wlen)
				, ulen(__ulen)
			{}
			__iteration_rec_folder_win_t(__iteration_rec_folder_win_t const &) = delete;
			__iteration_rec_folder_win_t(__iteration_rec_folder_win_t &&)			 = delete;

			/// Operator =
			__iteration_rec_folder_win_t &
			operator=(__iteration_rec_folder_win_t const &) = delete;
			__iteration_rec_folder_win_t &
			operator=(__iteration_rec_folder_win_t &&) = delete;

			/// Destructor
			pf_decl_inline ~__iteration_rec_folder_win_t() pf_attr_noexcept = default;

			/// Buffers
			pf_hint_nodiscard pf_decl_inline WCHAR *
			get_wroot() pf_attr_noexcept
			{
				return union_cast<WCHAR *>(&this->data[0]);
			}
			pf_hint_nodiscard pf_decl_inline u8char_t *
			get_uroot() pf_attr_noexcept
			{
				return union_cast<u8char_t *>(&this->data[0] + sizeof(WCHAR) * this->wlen);
			}

			/// Data
			__iteration_rec_folder_win_t *next;
			size_t wlen;
			size_t ulen;
			byte_t data[];
		};
		struct __iteration_rec_controller_win_t
		{
			/// Constructors
			pf_decl_inline
			__iteration_rec_controller_win_t()
				: all(__ITERATION_BUFFER_SIZE, ALIGN_DEFAULT, magnifier_linear(__ITERATION_BUFFER_SIZE))
				, handle(nullptr)
				, head(nullptr)
				, tail(nullptr)
				, ulen(0)
				, needupdate(true)
			{}
			__iteration_rec_controller_win_t(__iteration_rec_controller_win_t const &) = delete;
			__iteration_rec_controller_win_t(__iteration_rec_controller_win_t &&)			 = delete;

			/// Operator =
			__iteration_rec_controller_win_t &
			operator=(__iteration_rec_controller_win_t const &) = delete;
			__iteration_rec_controller_win_t &
			operator=(__iteration_rec_controller_win_t &&) = delete;

			/// Destructor
			pf_decl_inline ~__iteration_rec_controller_win_t() pf_attr_noexcept = default;

			/// Data
			allocator_ring_buffer<magnifier_linear> all;
			HANDLE handle;
			__iteration_rec_folder_win_t *head;
			__iteration_rec_folder_win_t *tail;
			size_t ulen;
			bool needupdate;
			byte_t ubuf[__MAX_UNICODE_PATH_LEN];
		};

		pf_hint_nodiscard find_info_t
		__iterate_rec_write_entry(
		 __iteration_rec_controller_win_t *__ctrl,
		 WIN32_FIND_DATAW const &__data)
		{
			// Entry
			try
			{
				// Write entry time & attributes
				find_info_t entry;
				entry.creationTime	 = __system_time_to_tm_win(__data.ftCreationTime);
				entry.lastAccessTime = __system_time_to_tm_win(__data.ftLastAccessTime);
				entry.lastWriteTime	 = __system_time_to_tm_win(__data.ftLastWriteTime);
				entry.fileSize			 = (union_cast<uint64_t>(__data.nFileSizeHigh) << 32) + __data.nFileSizeLow;
				entry.attributes		 = __to_pul_attributes_win(__data.dwFileAttributes);

				// Initialize
				auto fold							= __ctrl->head;
				auto wroot						= fold->get_wroot();
				auto uroot						= fold->get_uroot();
				const size_t wfilelen = utf16le::lenof(union_cast<u16char_t *>(&__data.cFileName[0u]));

				// Reparse Point File
				if(__data.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT)
				{
					// Current path
					const size_t wcurlen = fold->wlen + wfilelen - 2;
					auto wcurpath				 = make_unique_s<WCHAR[]>(wcurlen);
					memcpy(&wcurpath[0u], &wroot[0u], (fold->wlen - 2) * sizeof(u16char_t));
					memcpy(&wcurpath[0u] + fold->wlen - 2, &__data.cFileName[0u], wfilelen * sizeof(u16char_t));

					// Real path
					size_t wreallen;
					auto wrealpath = __make_symlink_path_buffer_s_win(&wcurpath[0u], wreallen, __data.dwFileAttributes);

					// Write
					__ctrl->ulen = utf16le_to_utf8::required_count(union_cast<u16char_t *>(&wcurpath[0u]) + 4, wcurlen - 4);
					utf16le_to_utf8::convert(union_cast<u16char_t *>(&wcurpath[0u] + 4), wcurlen - 4, &__ctrl->ubuf[0u]);
					entry.path = path_view(&__ctrl->ubuf[0u], __ctrl->ulen - 1);

					// Need update
					__ctrl->needupdate = true;
				}

				// File
				else
				{
					if(__ctrl->needupdate)
					{
						memcpy(&__ctrl->ubuf[0u], &uroot[0u], fold->ulen - 1);
						__ctrl->needupdate = false;
					}
					__ctrl->ulen = fold->ulen + utf16le_to_utf8::required_count(union_cast<u16char_t *>(&__data.cFileName[0u]), wfilelen);
					utf16le_to_utf8::convert(union_cast<u16char_t *>(&__data.cFileName[0u]), wfilelen, &__ctrl->ubuf[0u] + fold->ulen - 1);
					entry.path = path_view(&__ctrl->ubuf[0u], __ctrl->ulen - 1);
				}

				// Finished
				return entry;
			} catch(std::exception const &__e)
			{
				destroy_delete_s(__ctrl);
				throw;
			}
		}
		pf_hint_nodiscard find_info_t
		__iterate_rec_build_folder(
		 __iteration_rec_controller_win_t *__ctrl,
		 WIN32_FIND_DATAW const &__data)
		{
			try
			{
				// Write entry time & attributes
				find_info_t entry;
				entry.creationTime	 = __system_time_to_tm_win(__data.ftCreationTime);
				entry.lastAccessTime = __system_time_to_tm_win(__data.ftLastAccessTime);
				entry.lastWriteTime	 = __system_time_to_tm_win(__data.ftLastWriteTime);
				entry.fileSize			 = 0;
				entry.attributes		 = __to_pul_attributes_win(__data.dwFileAttributes);

				// Initialize
				auto fold							= __ctrl->head;
				auto wroot						= fold->get_wroot();
				auto uroot						= fold->get_uroot();
				const size_t wfilelen = utf16le::lenof(union_cast<u16char_t *>(&__data.cFileName[0]));
				const size_t wcurlen	= fold->wlen + wfilelen;

				// Reparse Point directory
				if(__data.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT)
				{
					// Current Path
					auto wcurpath = make_unique_s<WCHAR[]>(wcurlen);
					memcpy(&wcurpath[0u], &wroot[0], (fold->wlen - 2) * sizeof(u16char_t));
					memcpy(&wcurpath[0u] + fold->wlen - 2, &__data.cFileName[0], wfilelen * sizeof(u16char_t));

					// Real Path
					size_t wrellen;
					auto wrelpath				 = __make_symlink_path_buffer_s_win(&wcurpath[0u], wrellen, __data.dwFileAttributes);
					const size_t urellen = utf16le_to_utf8::required_count(union_cast<u16char_t *>(&wrelpath[0u]), wrellen);

					// New folder
					const size_t wnewlen = wrellen + 2;
					const size_t unewlen = urellen + 1;
					auto newfold				 = new_construct_ex<__iteration_rec_folder_win_t>(__ctrl->all, wnewlen * sizeof(u16char_t) + unewlen, wrellen, unewlen);
					auto wnewroot				 = newfold->get_wroot();
					auto unewroot				 = newfold->get_uroot();

					// wroot
					memcpy(&wnewroot[0u], &wrelpath[0u], (wrellen - 1) * sizeof(u16char_t));
					wnewroot[wnewlen - 3u] = L'\\';
					wnewroot[wnewlen - 2u] = L'*';
					wnewroot[wnewlen - 1u] = L'\0';

					// uroot
					utf16le_to_utf8::convert(union_cast<u16char_t *>(&wnewroot[0u]) + 4, wrellen - 6, &unewroot[0u]);
					__convert_backslash_to_forwardslash_u8_win(&unewroot[0], unewlen - 2);
					unewroot[unewlen - 2u] = '/';
					unewroot[unewlen - 1u] = '\0';

					// link
					__ctrl->tail->next = newfold;
					__ctrl->tail			 = newfold;

					// Path
					entry.path = path_view(&unewroot[0u], unewlen);
				}

				// Standard directory
				else
				{
					// New folder
					const size_t ufilelen = utf16le_to_utf8::required_count(union_cast<u16char_t *>(&__data.cFileName[0u]), wfilelen) + 2;
					const size_t unewlen	= fold->ulen + ufilelen + 1;
					auto newfold					= new_construct_ex<__iteration_rec_folder_win_t>(__ctrl->all, wcurlen * sizeof(u16char_t) + unewlen, wcurlen, unewlen);
					auto wnewroot					= newfold->get_wroot();
					auto unewroot					= newfold->get_uroot();

					// wroot
					memcpy(&wnewroot[0u], &wroot[0u], (fold->wlen - 2) * sizeof(u16char_t));
					memcpy(&wnewroot[0u] + fold->wlen - 2, &__data.cFileName[0u], wfilelen * sizeof(u16char_t));
					wnewroot[wcurlen - 3u] = L'\\';
					wnewroot[wcurlen - 2u] = L'*';
					wnewroot[wcurlen - 1u] = L'\0';

					// uroot
					memcpy(&unewroot[0u], &uroot[0u], fold->ulen - 1);
					utf16le_to_utf8::convert(union_cast<u16char_t *>(&__data.cFileName[0u]), wfilelen, &unewroot[0u] + fold->ulen - 1);
					unewroot[unewlen - 2] = '/';
					unewroot[unewlen - 1] = '\0';

					// link
					__ctrl->tail->next = newfold;
					__ctrl->tail			 = newfold;

					// Path
					entry.path = path_view(&unewroot[0u], unewlen - 2);
				}

				// Finished
				return entry;
			} catch(std::exception const &)
			{
				destroy_delete_s(__ctrl);
				throw;
			}
		}
		pulsar_api void *
		__iterate_rec_initialize(
		 path_view __v,
		 entry_t &__entry)
		{
			// Controller
			auto ctrl = new_construct_s<__iteration_rec_controller_win_t>();

			// Folder
			__iteration_rec_folder_win_t *fold;
			const size_t wlen = utf8_to_utf16le::required_count(__v.data(), __v.length()) + 7;
			const size_t ulen = __v.length() + 2;

			// Initialize pathing
			try
			{
				// Folder
				fold			 = new_construct_ex<__iteration_rec_folder_win_t>(ctrl->all, wlen * sizeof(u16char_t) + ulen, wlen, ulen);
				auto wroot = fold->get_wroot();
				auto uroot = fold->get_uroot();

				// Wide
				wroot[0u] = L'\\';
				wroot[1u] = L'\\';
				wroot[2u] = L'?';
				wroot[3u] = L'\\';
				utf8_to_utf16le::convert(__v.data(), __v.length(), union_cast<u16char_t *>(&wroot[0u] + 4));
				__convert_forwardslash_to_backslash_u16_win(&wroot[0u] + 4, wlen - 7);
				wroot[wlen - 3u] = L'\\';
				wroot[wlen - 2u] = L'*';
				wroot[wlen - 1u] = L'\0';

				// Utf-8
				memcpy(&uroot[0u], __v.data(), __v.length());
				uroot[ulen - 2u] = '/';
				uroot[ulen - 1u] = '\0';

				// Initialize
				WIN32_FIND_DATAW data;
				ctrl->handle = FindFirstFileExW(&wroot[0], FindExInfoBasic, &data, FindExSearchNameMatch, nullptr, FIND_FIRST_EX_CASE_SENSITIVE | FIND_FIRST_EX_LARGE_FETCH);

				// Link
				ctrl->head = fold;
				ctrl->tail = fold;

				// Skip invalid / special .. or .
				if(data.cFileName[0] == L'.' || ((data.dwFileAttributes & FILE_ATTRIBUTE_NORMAL) && (data.dwFileAttributes & ~FILE_ATTRIBUTE_NORMAL)))
				{
					return __iterate_rec_next(ctrl, __entry);
				}
				else if(data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				{
					// Make folder
					__entry = __iterate_rec_build_folder(ctrl, data);
				}
				else	// File
				{
					__entry = __iterate_rec_write_entry(ctrl, data);
				}
				return ctrl;
			} catch(std::exception const &)
			{
				destroy_delete_s(ctrl);
				throw;
			}
		}
		pulsar_api void *
		__iterate_rec_next(
		 void *__ctrl,
		 entry_t &__entry)
		{
			// Control
			auto ctrl = union_cast<__iteration_rec_controller_win_t *>(__ctrl);

			// Next
			WIN32_FIND_DATAW data;
			while(true)
			{
				if(!FindNextFileW(ctrl->handle, &data))
				{
					// Find last handle
					FindClose(ctrl->handle);
					auto p		 = ctrl->head;
					ctrl->head = ctrl->head->next;
					if(!ctrl->head)
					{
						destroy_delete_s(ctrl);
						return nullptr;
					}
					ctrl->needupdate = true;
					destroy_delete(ctrl->all, p);

					// Folder
					auto fold	 = ctrl->head;
					auto wroot = fold->get_wroot();

					// Find initialize next
					ctrl->handle = FindFirstFileExW(&wroot[0u], FindExInfoBasic, &data, FindExSearchNameMatch, nullptr, FIND_FIRST_EX_CASE_SENSITIVE | FIND_FIRST_EX_LARGE_FETCH);
				}

				// Skip invalid / special .. or .
				if(data.cFileName[0] == L'.' || ((data.dwFileAttributes & FILE_ATTRIBUTE_NORMAL) && (data.dwFileAttributes & ~FILE_ATTRIBUTE_NORMAL))) continue;

				// Directory?
				if(data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				{
					__entry = __iterate_rec_build_folder(ctrl, data);
					return ctrl;
				}

				// File?
				else
				{
					__entry = __iterate_rec_write_entry(ctrl, data);
					return ctrl;
				}
			}
		}
		pulsar_api void
		__iterate_rec_close(
		 void *__ctrl) pf_attr_noexcept
		{
			// Controller
			auto ctrl = union_cast<__iteration_rec_controller_win_t *>(__ctrl);

			// Close
			FindClose(ctrl->handle);
			destroy_delete_s(ctrl);
		}



		/// FILESYSTEM: Drives
		pulsar_api drive_enumeration
		drive_enumerate()
		{
			const size_t wlen = GetLogicalDriveStringsW(0, nullptr);
			auto wbuf					= make_unique_s<WCHAR[]>(wlen);
			pf_throw_if(
			 !GetLogicalDriveStringsW(union_cast<DWORD>(wlen), &wbuf[0u]),
			 dbg_category_system(),
			 GetLastError(),
			 0,
			 "[WIN] GetLogicalDriveStrings failed!");
			const size_t ulen = utf16le_to_utf8::required_count(union_cast<const u16char_t *>(wbuf.get()), wlen) - 1;
			auto ubuf					= new_construct_s<u8char_t[]>(ulen);
			utf16le_to_utf8::convert(union_cast<u16char_t *>(wbuf.get()), wlen, &ubuf[0]);
			__convert_backslash_to_forwardslash_u8_win(&ubuf[0u], ulen);
			return drive_enumeration { ubuf, ulen };
		}
		pulsar_api drive_type
		drive_get_type(
		 path_view __path)
		{
			const size_t wlen = utf8_to_utf16le::required_count(__path.data(), __path.length()) + 1;
			auto wbuf					= make_unique_s<WCHAR[]>(wlen);
			utf8_to_utf16le::convert(__path.data(), __path.length(), union_cast<u16char_t *>(&wbuf[0u]));
			__convert_forwardslash_to_backslash_u16_win(&wbuf[0u], wlen - 1);
			wbuf[wlen - 1] = L'\0';
			UINT type			 = GetDriveTypeW(&wbuf[0u]);
			switch(type)
			{
				case DRIVE_NO_ROOT_DIR: return drive_type::invalid;
				case DRIVE_REMOVABLE: return drive_type::removable;
				case DRIVE_FIXED: return drive_type::fixed;
				case DRIVE_REMOTE: return drive_type::remote;
				case DRIVE_CDROM: return drive_type::cdrom;
				case DRIVE_RAMDISK: return drive_type::ramdisk;
				default: return drive_type::unknown;
			};
		}
		pulsar_api path<>
		drive_by_path(
		 path_view __path)
		{
			size_t wlen;
			auto wbuf = __cast_utf8_to_wchar_buffer_ex_win(__path, wlen, 8);
			pf_throw_if(
			 !GetVolumePathNameW(&wbuf[0u], &wbuf[wlen], 8),
			 dbg_category_system(),
			 GetLastError(),
			 0,
			 "[WIN] GetVolumePathNameW failed!");
			path<> str('\0', 3);
			utf16le_to_utf8::convert(union_cast<u16char_t *>(&wbuf[0u] + wlen + 4), 1, str.data());
			str[1] = ':';
			str[2] = '/';
			str[3] = '\0';
			return str;
		}


		/// FILESYSTEM: Security
		LPWSTR
		__generate_user_sid_string_win(
		 size_t &__len)
		{
			HANDLE token;
			pf_throw_if(
			 !OpenProcessToken(GetCurrentProcess(), TOKEN_READ, &token),
			 dbg_category_system(),
			 GetLastError(),
			 0,
			 "[WIN] OpenProcessToken failed!");
			DWORD len;
			GetTokenInformation(
			 token,
			 TokenUser,
			 nullptr,
			 0,
			 &len);
			auto tknuser = make_unique_ex_s<TOKEN_USER>(len - sizeof(TOKEN_USER));
			pf_throw_if(
			 !GetTokenInformation(token, TokenUser, tknuser, len, &len),
			 dbg_category_system(),
			 GetLastError(),
			 0,
			 "[WIN] GetTokenInformation failed!");
			LPWSTR siduserw = nullptr;
			pf_throw_if(
			 !ConvertSidToStringSidW(tknuser->User.Sid, &siduserw),
			 dbg_category_system(),
			 GetLastError(),
			 0,
			 "[WIN] ConvertSidToStringSidA failed!");
			__len = utf16le::lenof(union_cast<u16char_t *>(&siduserw[0u])) - 1;
			return siduserw;
		}
		void
		__generate_security_descriptor_win(
		 perm_t __perms,
		 SECURITY_ATTRIBUTES &__sa)
		{
			// Current User SID
			size_t siduserlen;
			auto siduser = __generate_user_sid_string_win(siduserlen);

			// Initialize SID
			size_t totallen = 1;	// '\0'
			uint32_t a			= 0;
			uint32_t d			= 0;
			WCHAR ownerA[7] = { L'\0' };
			WCHAR ownerD[7] = { L'\0' };
			if(__perms & perm::owner_read)
			{
				ownerA[a++] = L'F';
				ownerA[a++] = L'R';
			}
			else
			{
				ownerD[d++] = L'F';
				ownerD[d++] = L'R';
			}
			if(__perms & perm::owner_write)
			{
				ownerA[a++] = L'F';
				ownerA[a++] = L'W';
			}
			else
			{
				ownerD[d++] = L'F';
				ownerD[d++] = L'W';
			}
			if(__perms & perm::owner_exec)
			{
				ownerA[a++] = L'F';
				ownerA[a++] = L'X';
			}
			else
			{
				ownerD[d++] = L'F';
				ownerD[d++] = L'X';
			}
			unique_ptr_s<WCHAR[]> ownerstr;
			if(a && d)
			{
				const size_t size = 2 * 12 + 6 + 2 * siduserlen + 1;
				ownerstr					= make_unique_s<WCHAR[]>(size);
				totallen				 += size - 1;
				fmt::format_to(
				 &ownerstr[0u],
				 L"(A;OICI;{};;;{})(D;OICI;{};;;{})\0",
				 ownerA,
				 siduser,
				 ownerD,
				 siduser);
			}
			else
			{
				const size_t size = 18 + siduserlen + 1;
				ownerstr					= make_unique_s<WCHAR[]>(size);
				totallen				 += size - 1;
				fmt::format_to(
				 &ownerstr[0u],
				 L"({};OICI;{};;;{})\0",
				 a ? L'A' : L'D',
				 a ? ownerA : ownerD,
				 siduser);
			}
			LocalFree(siduser);

			a								= 0;
			d								= 0;
			WCHAR usersA[7] = { L'\0' };
			WCHAR usersD[7] = { L'\0' };
			if(__perms & perm::users_read)
			{
				usersA[a++] = L'F';
				usersA[a++] = L'R';
			}
			else
			{
				usersD[d++] = L'F';
				usersD[d++] = L'R';
			}
			if(__perms & perm::users_write)
			{
				usersA[a++] = L'F';
				usersA[a++] = L'W';
			}
			else
			{
				usersD[d++] = L'F';
				usersD[d++] = L'W';
			}
			if(__perms & perm::users_exec)
			{
				usersA[a++] = L'F';
				usersA[a++] = L'X';
			}
			else
			{
				usersD[d++] = L'F';
				usersD[d++] = L'X';
			}
			unique_ptr_s<WCHAR[]> userstr;
			if(a && d)
			{
				const size_t size = 14 * 2 + 6 * 3 + 1;
				userstr						= make_unique_s<WCHAR[]>(size);
				totallen				 += size - 1;
				fmt::format_to(
				 &userstr[0u],
				 L"(A;OICI;{};;;BU)(D;OICI;{};;;BU)\0",
				 usersA,
				 usersD);
			}
			else
			{
				const size_t size = 14 + 6 + 1;
				userstr						= make_unique_s<WCHAR[]>(size);
				totallen				 += size - 1;
				fmt::format_to(
				 &userstr[0u],
				 L"({};OICI;{};;;BU)\0",
				 a ? L'A' : L'D',
				 a ? usersA : usersD);
			}

			a								= 0;
			d								= 0;
			WCHAR guestA[7] = { L'\0' };
			WCHAR guestD[7] = { L'\0' };
			if(__perms & perm::guest_read)
			{
				guestA[a++] = L'F';
				guestA[a++] = L'R';
			}
			else
			{
				guestD[d++] = L'F';
				guestD[d++] = L'R';
			}
			if(__perms & perm::guest_write)
			{
				guestA[a++] = L'F';
				guestA[a++] = L'W';
			}
			else
			{
				guestD[d++] = L'F';
				guestD[d++] = L'W';
			}
			if(__perms & perm::guest_exec)
			{
				guestA[a++] = L'F';
				guestA[a++] = L'X';
			}
			else
			{
				guestD[d++] = L'F';
				guestD[d++] = L'X';
			}
			unique_ptr_s<WCHAR[]> gueststr;
			if(a && d)
			{
				const size_t size = 14 * 2 + 6 * 3 + 1;
				gueststr					= make_unique_s<WCHAR[]>(size);
				totallen				 += size - 1;
				fmt::format_to(
				 &gueststr[0u],
				 L"(A;OICI;{};;;BG)(D;OICI;{};;;BG)\0",
				 guestA,
				 guestD);
			}
			else
			{
				const size_t size = 14 + 6 + 1;
				gueststr					= make_unique_s<WCHAR[]>(size);
				totallen				 += size - 1;
				fmt::format_to(
				 &gueststr[0u],
				 L"({};OICI;{};;;BG)\0",
				 a ? L'A' : L'D',
				 a ? guestA : guestD);
			}

			// Format all
			unique_ptr_s<WCHAR[]> str = make_unique_s<WCHAR[]>(totallen);
			fmt::format_to(
			 &str[0u],
			 L"D:"
			 L"(A;OICI;FA;;;BA)"	// Local System
			 L"(A;OICI;FA;;;BA)"	// Administrators
			 L"{}"								// Owner
			 L"{}"								// Users
			 L"{}\0",							// Guests
			 &ownerstr[0u],
			 &userstr[0u],
			 &gueststr[0u]);
			pf_throw_if(
			 !ConvertStringSecurityDescriptorToSecurityDescriptorW(&str[0u], SDDL_REVISION_1, &__sa.lpSecurityDescriptor, nullptr),
			 dbg_category_system(),
			 GetLastError(),
			 0,
			 "[WIN] ConvertStringSecurityDescriptorToSecurityDescriptor failed!");
		}


		/// FILESYSTEM: Iteration -> Rec -> Size Controller
		struct __iteration_rec_size_folder_win_t
		{
			/// Constructors
			pf_decl_inline
			__iteration_rec_size_folder_win_t(
			 size_t __wlen) pf_attr_noexcept
				: next(nullptr)
				, wlen(__wlen)
			{}
			__iteration_rec_size_folder_win_t(__iteration_rec_size_folder_win_t const &) = delete;
			__iteration_rec_size_folder_win_t(__iteration_rec_size_folder_win_t &&)			 = delete;

			/// Operator =
			__iteration_rec_size_folder_win_t &
			operator=(__iteration_rec_size_folder_win_t const &) = delete;
			__iteration_rec_size_folder_win_t &
			operator=(__iteration_rec_size_folder_win_t &&) = delete;

			/// Data
			__iteration_rec_size_folder_win_t *next;
			size_t wlen;
			WCHAR buf[];
		};
		struct __iteration_rec_size_controller_t
		{
			/// Constructors
			pf_decl_inline
			__iteration_rec_size_controller_t() pf_attr_noexcept
				: handle(nullptr)
				, all(__ITERATION_BUFFER_SIZE, ALIGN_DEFAULT, magnifier_linear(__ITERATION_BUFFER_SIZE))
				, head(nullptr)
				, tail(nullptr)
			{}
			__iteration_rec_size_controller_t(__iteration_rec_size_controller_t const &) = delete;
			__iteration_rec_size_controller_t(__iteration_rec_size_controller_t &&)			 = delete;

			/// Operator =
			__iteration_rec_size_controller_t &
			operator=(__iteration_rec_size_controller_t const &) = delete;
			__iteration_rec_size_controller_t &
			operator=(__iteration_rec_size_controller_t &&) = delete;

			/// Data
			HANDLE handle;
			allocator_ring_buffer<magnifier_linear> all;
			__iteration_rec_size_folder_win_t *head;
			__iteration_rec_size_folder_win_t *tail;
		};


		/// FILESYSTEM: Directory
		pf_hint_nodiscard uint64_t
		__directory_calculate_size(
		 __iteration_rec_size_controller_t *__ctrl,
		 WIN32_FIND_DATAW const &__data)
		{
			// Skip?
			if(__data.cFileName[0] == L'.' || ((__data.dwFileAttributes & FILE_ATTRIBUTE_NORMAL) && (__data.dwFileAttributes & ~FILE_ATTRIBUTE_NORMAL))) return 0;

			// Directory?
			if(__data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				auto fold							= __ctrl->head;
				const size_t wfilelen = utf16le::lenof(union_cast<u16char_t *>(&__data.cFileName[0]));
				const size_t wlen			= fold->wlen + wfilelen;
				auto newfold					= new_construct_ex<__iteration_rec_size_folder_win_t>(wlen * sizeof(u16char_t), wlen);
				memcpy(&newfold->buf[0u], &fold->buf[0u], (fold->wlen - 2) * sizeof(u16char_t));
				memcpy(&newfold->buf[0u] + fold->wlen - 2, &__data.cFileName[0u], wfilelen * sizeof(u16char_t));
				newfold->buf[wlen - 3u] = L'\\';
				newfold->buf[wlen - 2u] = L'*';
				newfold->buf[wlen - 1u] = L'\0';
				__ctrl->tail->next			= newfold;
				__ctrl->tail						= newfold;
				return 0;
			}

			// File?
			else
			{
				return (union_cast<uint64_t>(__data.nFileSizeHigh) << 32) + __data.nFileSizeLow;
			}
		}
		pulsar_api uint64_t
		directory_size(
		 path_view __p)
		{
			// Initialize
			auto ctrl = make_unique_s<__iteration_rec_size_controller_t>();

			// Folder
			const size_t wlen = utf8_to_utf16le::required_count(__p.data(), __p.length()) + 2;
			auto fold					= new_construct_ex<__iteration_rec_size_folder_win_t>(ctrl->all, wlen * sizeof(u16char_t), wlen);
			utf8_to_utf16le::convert(__p.data(), __p.length(), union_cast<u16char_t *>(&fold->buf[0u]));
			__convert_forwardslash_to_backslash_u16_win(&fold->buf[0u], wlen - 3);
			fold->buf[wlen - 3u] = L'\\';
			fold->buf[wlen - 2u] = L'*';
			fold->buf[wlen - 1u] = L'\0';
			ctrl->head					 = fold;
			ctrl->tail					 = fold;

			// Handle
			WIN32_FIND_DATAW data;
			ctrl->handle = FindFirstFileExW(&fold->buf[0u], FindExInfoBasic, &data, FindExSearchNameMatch, nullptr, FIND_FIRST_EX_CASE_SENSITIVE | FIND_FIRST_EX_LARGE_FETCH);
			pf_throw_if(
			 ctrl->handle == INVALID_HANDLE_VALUE,
			 dbg_category_system(),
			 GetLastError(),
			 0,
			 "FindFirstFileExW failed!");

			// First
			uint64_t totalsize = __directory_calculate_size(ctrl, data);

			// Iterate
			while(true)
			{
				// Find next
				if(!FindNextFileW(ctrl->handle, &data))
				{
					// Find last handle
					FindClose(ctrl->handle);
					auto p		 = ctrl->head;
					ctrl->head = ctrl->head->next;
					if(!ctrl->head) return totalsize;
					destroy_delete(ctrl->all, p);

					// Next
					ctrl->handle = FindFirstFileExW(&ctrl->head->buf[0u], FindExInfoBasic, &data, FindExSearchNameMatch, nullptr, FIND_FIRST_EX_CASE_SENSITIVE | FIND_FIRST_EX_LARGE_FETCH);
				}

				// Calculate
				totalsize += __directory_calculate_size(ctrl, data);
			}
		}
		pf_hint_nodiscard bool
		__directory_is_empty_win(
		 WCHAR *__wpath,
		 size_t __wlen)
		{
			// Prepare iteration
			__wpath[__wlen - 3] = L'\\';
			__wpath[__wlen - 2] = L'*';
			__wpath[__wlen - 1] = L'\0';

			// Data
			WIN32_FIND_DATAW data;

			// Handle
			HANDLE handle = FindFirstFileExW(__wpath, FindExInfoBasic, &data, FindExSearchNameMatch, nullptr, FIND_FIRST_EX_CASE_SENSITIVE | FIND_FIRST_EX_LARGE_FETCH);
			pf_throw_if(handle == INVALID_HANDLE_VALUE, dbg_category_system(), GetLastError(), 0, "[WIN] FindFirstFileExW failed!");

			// Iterate
			do
			{
				// Skip
				if(data.cFileName[0] == L'.' || ((data.dwFileAttributes & FILE_ATTRIBUTE_NORMAL) && (data.dwFileAttributes & ~FILE_ATTRIBUTE_NORMAL))) continue;

				// Else
				pf_throw_if(!FindClose(handle), dbg_category_system(), GetLastError(), 0, "[WIN] CloseHandle failed!");
				return false;
			} while(FindNextFileW(handle, &data));
			pf_throw_if(!FindClose(handle), dbg_category_system(), GetLastError(), 0, "[WIN] CloseHandle failed!");
			return true;
		}
		pulsar_api bool
		directory_is_empty(
		 path_view __p)
		{
			size_t wlen;
			auto wpath = __cast_utf8_to_wchar_buffer_ex_win(__p, wlen, 2);
			if(pf_unlikely(GetFileAttributesW(&wpath[0u]) == INVALID_FILE_ATTRIBUTES)) return false;
			return __directory_is_empty_win(wpath, wlen);
		}
		pulsar_api void
		directory_create(
		 path_view __v,
		 perm_t __perms)
		{
			// Security
			SECURITY_ATTRIBUTES sa;
			__generate_security_descriptor_win(__perms, sa);

			// Cast
			size_t wlen;
			unique_ptr_s<WCHAR[]> wbuf;
			try
			{
				wbuf = __cast_utf8_to_wchar_buffer_win(__v, wlen);
			} catch(std::exception const &__e)
			{
				LocalFree(sa.lpSecurityDescriptor);
			}

			// Create Directory
			try
			{
				pf_throw_if(
				 !CreateDirectoryW(&wbuf[0u], &sa),
				 dbg_category_system(),
				 GetLastError(),
				 0,
				 "[WIN] CreateDirectoryW failed!");
			} catch(std::exception const &)
			{
				LocalFree(sa.lpSecurityDescriptor);
				throw;
			}

			// Free-up
			LocalFree(sa.lpSecurityDescriptor);
		}
		void
		__directory_create_rec_win(
		 WCHAR *__wbuf,
		 size_t __wlen,
		 SECURITY_ATTRIBUTES &__sa)
		{
			// Create Directories Recursively
			try
			{
				// Locate unknow directories
				size_t i		= __wlen - 2;
				size_t last = __wlen - 1;
				while(GetFileAttributesW(&__wbuf[0]) == INVALID_FILE_ATTRIBUTES)
				{
					if(__wbuf[i] == L'\\')
					{
						__wbuf[i] = '\0';
						last			= i;
						break;
					}
					--i;
					pf_throw_if(
					 i == 3,
					 dbg_category_system(),
					 ERROR_PATH_NOT_FOUND,
					 0,
					 "Cannot find path!");
				}
				while(GetFileAttributesW(&__wbuf[0]) == INVALID_FILE_ATTRIBUTES)
				{
					if(__wbuf[i] == L'\\') __wbuf[i] = '\0';
					--i;
					pf_throw_if(
					 i == 3,
					 dbg_category_system(),
					 ERROR_PATH_NOT_FOUND,
					 0,
					 "Cannot find path!");
				}

				// Local unbuilt directories
				while(i != last)
				{
					if(__wbuf[i] == L'\0')
					{
						__wbuf[i] = L'\\';
						// Create Directory
						pf_throw_if(
						 !CreateDirectoryW(&__wbuf[0], &__sa),
						 dbg_category_system(),
						 GetLastError(),
						 0,
						 "[WIN] CreateDirectoryW failed!");
					}
					++i;
				}

				// Create Last Directory
				if(pf_likely(last != __wlen - 1)) __wbuf[last] = L'\\';
				pf_throw_if(
				 !CreateDirectoryW(&__wbuf[0], &__sa),
				 dbg_category_system(),
				 GetLastError(),
				 0,
				 "[WIN] CreateDirectoryW failed!");
			} catch(std::exception const &)
			{
				LocalFree(__sa.lpSecurityDescriptor);
				throw;
			}
			LocalFree(__sa.lpSecurityDescriptor);
		}
		pulsar_api void
		directory_create_rec(
		 path_view __path,
		 perm __perms)
		{
			// Security
			SECURITY_ATTRIBUTES sa;
			__generate_security_descriptor_win(__perms, sa);

			// Cast
			size_t wlen;
			unique_ptr_s<WCHAR[]> wbuf;
			try
			{
				wbuf = __cast_utf8_to_wchar_buffer_win(__path, wlen);
			} catch(std::exception const &)
			{
				LocalFree(sa.lpSecurityDescriptor);
				throw;
			}

			// Create Recursively
			__directory_create_rec_win(wbuf, wlen, sa);
		}


		/// Copying
		struct __iteration_rec_copy_folder_win_t
		{
			/// Constructors
			pf_decl_inline
			__iteration_rec_copy_folder_win_t(
			 size_t __wfromlen,
			 size_t __wdestlen)
				: next(nullptr)
				, wfromlen(__wfromlen)
				, wdestlen(__wdestlen)
			{}

			/// Operator =
			__iteration_rec_copy_folder_win_t &
			operator=(__iteration_rec_copy_folder_win_t const &) = delete;
			__iteration_rec_copy_folder_win_t &
			operator=(__iteration_rec_copy_folder_win_t &&) = delete;

			/// Destructor
			pf_decl_inline ~__iteration_rec_copy_folder_win_t() pf_attr_noexcept = default;

			/// Buffers
			pf_hint_nodiscard pf_decl_inline WCHAR *
			get_wfrom() pf_attr_noexcept
			{
				return &this->wbuf[0];
			}
			pf_hint_nodiscard pf_decl_inline WCHAR *
			get_wdest() pf_attr_noexcept
			{
				return &this->wbuf[0] + this->wfromlen;
			}

			/// Data
			__iteration_rec_copy_folder_win_t *next;
			size_t wfromlen;
			size_t wdestlen;
			WCHAR wbuf[];
		};
		struct __iteration_rec_copy_controller_win_t
		{
			/// Constructors
			pf_decl_inline
			__iteration_rec_copy_controller_win_t()
				: all(__ITERATION_BUFFER_SIZE, ALIGN_DEFAULT, magnifier_linear(__ITERATION_BUFFER_SIZE))
				, head(nullptr)
				, tail(nullptr)
			{}
			__iteration_rec_copy_controller_win_t(__iteration_rec_copy_controller_win_t const &) = delete;
			__iteration_rec_copy_controller_win_t(__iteration_rec_copy_controller_win_t &&)			 = delete;

			/// Operator =
			__iteration_rec_copy_controller_win_t &
			operator=(__iteration_rec_copy_controller_win_t const &) = delete;
			__iteration_rec_copy_controller_win_t &
			operator=(__iteration_rec_copy_controller_win_t &&) = delete;

			/// Destructor
			pf_decl_inline ~__iteration_rec_copy_controller_win_t() pf_attr_noexcept = default;

			/// Data
			HANDLE handle;
			allocator_ring_buffer<magnifier_linear> all;
			__iteration_rec_copy_folder_win_t *head;
			__iteration_rec_copy_folder_win_t *tail;
		};

		void
		__directory_copy(
		 __iteration_rec_copy_controller_win_t *__ctrl,
		 WIN32_FIND_DATAW const &__data,
		 SECURITY_ATTRIBUTES &__sa,
		 copy_option_t __options)
		{
			// Skip?
			if(__data.cFileName[0] == L'.' || ((__data.dwFileAttributes & FILE_ATTRIBUTE_NORMAL) && (__data.dwFileAttributes & ~FILE_ATTRIBUTE_NORMAL))) return;

			// Initialize
			auto fold							= __ctrl->head;
			auto wfrom						= fold->get_wfrom();
			auto wdest						= fold->get_wdest();
			const size_t wfilelen = utf16le::lenof(union_cast<u16char_t *>(&__data.cFileName[0]));
			size_t wfromlen				= fold->wfromlen + wfilelen;
			const size_t wdestlen = fold->wdestlen + wfilelen;

			// From
			unique_ptr_s<WCHAR[]> wnewfrom = make_unique_s<WCHAR[]>(wfromlen);
			memcpy(&wnewfrom[0u], &wfrom[0u], (fold->wfromlen - 2u) * sizeof(u16char_t));
			memcpy(&wnewfrom[0u] + fold->wfromlen - 2u, &__data.cFileName[0u], wfilelen * sizeof(u16char_t));

			// From -> Reparse Point? (Get real path)
			if(__data.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT)
			{
				if(__options & copy_option::skip_symlinks) return;

				// Real Path
				wnewfrom	= __make_symlink_path_buffer_s_win(&wnewfrom[0u], wfromlen, GetFileAttributesW(&wnewfrom[0u]));
				wfromlen += 2;
			}

			// Dest
			unique_ptr_s<WCHAR[]> wnewdest = make_unique_s<WCHAR[]>(wdestlen);
			memcpy(&wnewdest[0u], &wdest[0u], (fold->wdestlen - 1u) * sizeof(u16char_t));
			memcpy(&wnewdest[0u] + fold->wdestlen - 1u, &__data.cFileName[0u], wfilelen * sizeof(u16char_t));

			// Directory
			if(__data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				// Dest -> Exist?
				DWORD wnewdestattrs = GetFileAttributesW(&wnewdest[0u]);
				if(wnewdestattrs != INVALID_FILE_ATTRIBUTES)
				{
					if(__options & copy_option::skip_existing)
					{
						return;
					}
					if(__options & copy_option::update_existing)
					{
						WIN32_FILE_ATTRIBUTE_DATA destfileinfo;
						pf_throw_if(!GetFileAttributesExW(&wnewdest[0u], GetFileExInfoStandard, &destfileinfo), dbg_category_system(), GetLastError(), 0, "[WIN] GetFileAttributesExW failed!");
						if(*union_cast<size_t *>(&__data.ftLastWriteTime.dwHighDateTime) <= *union_cast<size_t *>(&destfileinfo.ftLastWriteTime.dwHighDateTime))
						{
							return;
						}
					}
				}
				else
				{
					if(__options & copy_option::create_symlinks)	// <=> __options & copy_option::copy_symlinks
					{
						pf_throw_if(!CreateSymbolicLinkW(&wnewdest[0u], &wnewfrom[0u], SYMBOLIC_LINK_FLAG_DIRECTORY), dbg_category_system(), GetLastError(), 0, "[WIN] CreateSymbolicLinkW failed!");
						return;
					}
					else if(__options & copy_option::create_hard_links)
					{
						// Create directory
						pf_throw_if(
						 !CreateDirectoryW(&wnewdest[0u], &__sa),
						 dbg_category_system(),
						 GetLastError(),
						 0,
						 "[WIN] CreateDirectoryW failed!");

						// Open directory
						HANDLE newdir = CreateFileW(&wnewdest[0u], FILE_GENERIC_WRITE | FILE_GENERIC_READ, 0, &__sa, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, nullptr);
						pf_throw_if(
						 newdir == INVALID_HANDLE_VALUE,
						 dbg_category_system(),
						 GetLastError(),
						 0,
						 "[WIN] CreateFileW failed!");

						// Set Reparse Point
						try
						{
							pf_throw_if(
							 !DeviceIoControl(newdir, FSCTL_SET_REPARSE_POINT, &wnewfrom[0u], union_cast<DWORD>(wfromlen - 2), nullptr, 0, nullptr, 0),
							 dbg_category_system(),
							 GetLastError(),
							 0,
							 "[WIN] DeviceIoControl failed!");
						} catch(std::exception const &)
						{
							CloseHandle(newdir);
							throw;
						}

						// Close
						pf_throw_if(
						 !CloseHandle(newdir),
						 dbg_category_system(),
						 GetLastError(),
						 0,
						 "[WIN] CloseHandle failed!");
						return;
					}
					else
					{
						pf_throw_if(!CreateDirectoryW(&wnewdest[0u], &__sa), dbg_category_system(), GetLastError(), 0, "[WIN] CreateDirectoryW failed!");
					}
				}

				// Link?
				if(__options & copy_option::recursive)
				{
					// From as root
					wnewfrom[wfromlen - 3u] = L'\\';
					wnewfrom[wfromlen - 2u] = L'*';
					wnewfrom[wfromlen - 1u] = L'\0';

					// Dest as root
					wnewdest[wdestlen - 2u] = L'\\';
					wnewdest[wdestlen - 1u] = L'\0';

					// Create folder
					auto newfold			= new_construct_ex<__iteration_rec_copy_folder_win_t>(__ctrl->all, (wfromlen + wdestlen) * sizeof(u16char_t), wfromlen, wdestlen);
					auto wnewfoldfrom = newfold->get_wfrom();
					memcpy(&wnewfoldfrom[0u], &wnewfrom[0u], wfromlen * sizeof(u16char_t));
					auto wnewfolddest = newfold->get_wdest();
					memcpy(&wnewfolddest[0u], &wnewdest[0u], wdestlen * sizeof(u16char_t));

					// Link
					__ctrl->tail->next = newfold;
					__ctrl->tail			 = newfold;
				}
			}

			// File
			else if(!(__options & copy_option::directories_only))
			{
				// Dest -> Exist?
				DWORD wnewdestattrs = GetFileAttributesW(&wnewdest[0u]);
				if(wnewdestattrs != INVALID_FILE_ATTRIBUTES)
				{
					if(__options & copy_option::skip_existing)
					{
						return;
					}
					if(__options & copy_option::update_existing)
					{
						WIN32_FILE_ATTRIBUTE_DATA destfileinfo;
						pf_throw_if(!GetFileAttributesExW(&wnewdest[0u], GetFileExInfoStandard, &destfileinfo), dbg_category_system(), GetLastError(), 0, "[WIN] GetFileAttributesExW failed!");
						if(*union_cast<size_t *>(&__data.ftLastWriteTime.dwHighDateTime) <= *union_cast<size_t *>(&destfileinfo.ftLastWriteTime.dwHighDateTime))
						{
							return;
						}
					}
					if(__options & copy_option::overwrite_existing)
					{
						pf_throw_if(
						 !CopyFileW(&wnewfrom[0u], &wnewdest[0u], false),
						 dbg_category_system(),
						 GetLastError(),
						 0,
						 "[WIN] CopyFileW failed!");
					}
				}
				else
				{
					if(__options & copy_option::create_symlinks)
					{
						pf_throw_if(
						 !CreateSymbolicLinkW(&wnewdest[0u], &wnewfrom[0u], 0),
						 dbg_category_system(),
						 GetLastError(),
						 0,
						 "[WIN] CreateSymbolicLinkW failed!");
					}
					else if(__options & copy_option::create_hard_links)
					{
						pf_throw_if(
						 !CreateHardLinkW(&wnewdest[0u], &wnewfrom[0u], &__sa),
						 dbg_category_system(),
						 GetLastError(),
						 0,
						 "[WIN] CreateHardLinkW failed!");
					}
					else
					{
						pf_throw_if(
						 !CopyFileW(&wnewfrom[0u], &wnewdest[0u], true),
						 dbg_category_system(),
						 GetLastError(),
						 0,
						 "[WIN] CopyFileW failed!");
					}
				}
			}
		}
		pulsar_api void
		directory_copy(
		 path_view __from,
		 path_view __dest,
		 copy_option_t __options,
		 perm_t __perms)
		{
			// Cast
			size_t fromlen;
			unique_ptr_s<WCHAR[]> frombuf = __cast_utf8_to_wchar_buffer_win(__from, fromlen);

			// Is __in a directory?
			DWORD attrs = GetFileAttributesW(&frombuf[0u]);
			if(pf_unlikely(!(attrs & FILE_ATTRIBUTE_DIRECTORY)))
			{
				pf_throw(
				 dbg_category_system(),
				 ERROR_BAD_PATHNAME,
				 0,
				 "[WIN] __from isn't a path to a directory! path={}",
				 __from.data());
			}

			// Controller initialisation
			auto ctrl = make_unique_s<__iteration_rec_copy_controller_win_t>();

			// Data
			WIN32_FIND_DATAW data;

			// Security
			SECURITY_ATTRIBUTES sa;
			__generate_security_descriptor_win(__perms, sa);

			// Folder
			try
			{
				// Make
				const size_t wfromlen = utf8_to_utf16le::required_count(__from.data(), __from.length()) + 7;
				const size_t wdestlen = utf8_to_utf16le::required_count(__dest.data(), __dest.length()) + 6;
				auto fold							= new_construct_ex<__iteration_rec_copy_folder_win_t>((wfromlen + wdestlen) * sizeof(u16char_t), wfromlen, wdestlen);
				auto wfrom						= fold->get_wfrom();
				auto wdest						= fold->get_wdest();

				// From
				wfrom[0u] = L'\\';
				wfrom[1u] = L'\\';
				wfrom[2u] = L'?';
				wfrom[3u] = L'\\';
				utf8_to_utf16le::convert(__from.data(), __from.length(), union_cast<u16char_t *>(&wfrom[0u]) + 4u);
				__convert_forwardslash_to_backslash_u16_win(&wfrom[0u] + 4, wfromlen - 7);
				wfrom[wfromlen - 3u] = L'\\';
				wfrom[wfromlen - 2u] = L'*';
				wfrom[wfromlen - 1u] = L'\0';

				// Dest
				wdest[0u] = L'\\';
				wdest[1u] = L'\\';
				wdest[2u] = L'?';
				wdest[3u] = L'\\';
				utf8_to_utf16le::convert(__dest.data(), __dest.length(), union_cast<u16char_t *>(&wdest[0u]) + 4u);
				__convert_forwardslash_to_backslash_u16_win(&wdest[0u] + 4, wdestlen - 6);
				wdest[wdestlen - 2u] = '\0';
				attrs								 = GetFileAttributesW(&wdest[0u]);

				// Dest -> Valid?
				if(attrs == INVALID_FILE_ATTRIBUTES)
				{
					__directory_create_rec_win(wdest, wdestlen, sa);
				}
				else if(pf_unlikely(!(attrs & FILE_ATTRIBUTE_DIRECTORY)))
				{
					pf_throw(
					 dbg_category_system(),
					 ERROR_BAD_PATHNAME,
					 0,
					 "[WIN] __dest isn't a path to a directory! path={}",
					 __dest.data());
				}

				// Dest -> Root end
				wdest[wdestlen - 2u] = '\\';
				wdest[wdestlen - 1u] = '\0';

				// Link
				ctrl->head = fold;
				ctrl->tail = fold;

				// Handle
				ctrl->handle = FindFirstFileExW(
				 &wfrom[0u],
				 FindExInfoBasic,
				 &data,
				 FindExSearchNameMatch,
				 nullptr,
				 FIND_FIRST_EX_CASE_SENSITIVE | FIND_FIRST_EX_LARGE_FETCH);
				if(ctrl->handle == INVALID_HANDLE_VALUE)
				{
					pf_throw_if(
					 !ctrl->handle,
					 dbg_category_system(),
					 GetLastError(),
					 0,
					 "FindFirstFileExW failed!");
				}
			} catch(std::exception const &)
			{
				LocalFree(sa.lpSecurityDescriptor);
				throw;
			}

			// Copy recursively
			try
			{
				// Copy
				__directory_copy(ctrl, data, sa, __options);

				// Iterate
				while(true)
				{
					if(!FindNextFileW(ctrl->handle, &data))
					{
						// Find last handle
						FindClose(ctrl->handle);
						auto p		 = ctrl->head;
						ctrl->head = ctrl->head->next;
						if(!ctrl->head) return;
						destroy_delete(ctrl->all, p);

						// Folder
						auto wfrom = ctrl->head->get_wfrom();

						// Find initialize next
						ctrl->handle = FindFirstFileExW(&wfrom[0u], FindExInfoBasic, &data, FindExSearchNameMatch, nullptr, FIND_FIRST_EX_CASE_SENSITIVE | FIND_FIRST_EX_LARGE_FETCH);
					}

					// Copy
					__directory_copy(ctrl, data, sa, __options);
				}
			} catch(std::exception const &)
			{
				FindClose(ctrl->handle);
				LocalFree(sa.lpSecurityDescriptor);
				throw;
			}

			// Free-up
			LocalFree(sa.lpSecurityDescriptor);
		}

		struct __iteration_rec_remove_folder_win_t
		{
			/// Constructors
			pf_decl_inline
			__iteration_rec_remove_folder_win_t(
			 size_t __wlen) pf_attr_noexcept
				: next(nullptr)
				, prev(nullptr)
				, wlen(__wlen)
			{}
			__iteration_rec_remove_folder_win_t(__iteration_rec_remove_folder_win_t const &) = delete;
			__iteration_rec_remove_folder_win_t(__iteration_rec_remove_folder_win_t &&)			 = delete;

			/// Operator =
			__iteration_rec_remove_folder_win_t &
			operator=(__iteration_rec_remove_folder_win_t const &) = delete;
			__iteration_rec_remove_folder_win_t &
			operator=(__iteration_rec_remove_folder_win_t &&) = delete;

			/// Data
			__iteration_rec_remove_folder_win_t *next;
			__iteration_rec_remove_folder_win_t *prev;
			size_t wlen;
			WCHAR buf[];
		};
		struct __iteration_rec_remove_controller_win_t
		{
			/// Constructors
			pf_decl_inline
			__iteration_rec_remove_controller_win_t()
				: handle(nullptr)
				, all(__ITERATION_BUFFER_SIZE, ALIGN_DEFAULT, magnifier_linear(__ITERATION_BUFFER_SIZE))
				, head(nullptr)
				, tail(nullptr)
				, curr(nullptr)
			{}
			__iteration_rec_remove_controller_win_t(__iteration_rec_remove_controller_win_t const &) = delete;
			__iteration_rec_remove_controller_win_t(__iteration_rec_remove_controller_win_t &&)			 = delete;

			/// Operator =
			__iteration_rec_remove_controller_win_t &
			operator=(__iteration_rec_remove_controller_win_t const &) = delete;
			__iteration_rec_remove_controller_win_t &
			operator=(__iteration_rec_remove_controller_win_t &&) = delete;

			/// Data
			HANDLE handle;
			allocator_ring_buffer<magnifier_linear> all;
			__iteration_rec_remove_folder_win_t *head;
			__iteration_rec_remove_folder_win_t *tail;
			__iteration_rec_remove_folder_win_t *curr;
		};

		void
		__directory_remove(
		 __iteration_rec_remove_controller_win_t *__ctrl,
		 WIN32_FIND_DATAW const &__data)
		{
			// Skip?
			if(__data.cFileName[0] == L'.' || ((__data.dwFileAttributes & FILE_ATTRIBUTE_NORMAL) && (__data.dwFileAttributes & ~FILE_ATTRIBUTE_NORMAL))) return;

			// Common
			const size_t wfilelen = utf16le::lenof(union_cast<u16char_t *>(&__data.cFileName[0u]));
			auto fold							= __ctrl->curr;

			// Directory?
			if(__data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				const size_t wlen = fold->wlen + wfilelen;
				auto newfold			= new_construct_ex<__iteration_rec_remove_folder_win_t>(wlen * sizeof(u16char_t), wlen);
				memcpy(&newfold->buf[0u], &fold->buf[0u], (fold->wlen - 2) * sizeof(u16char_t));
				memcpy(&newfold->buf[0u] + fold->wlen - 2, &__data.cFileName[0u], wfilelen * sizeof(u16char_t));
				newfold->buf[wlen - 3u] = L'\\';
				newfold->buf[wlen - 2u] = L'*';
				newfold->buf[wlen - 1u] = L'\0';
				newfold->prev						= __ctrl->tail;
				__ctrl->tail->next			= newfold;
				__ctrl->tail						= newfold;
			}

			// File?
			else
			{
				const size_t wlen = fold->wlen + wfilelen - 2;
				auto tmp					= make_unique_s<WCHAR[]>(wlen);
				memcpy(&tmp[0u], &fold->buf[0u], (fold->wlen - 2) * sizeof(u16char_t));
				memcpy(&tmp[0u] + fold->wlen - 2, &__data.cFileName[0u], wfilelen * sizeof(u16char_t));
				pf_throw_if(
				 !DeleteFileW(&tmp[0u]),
				 dbg_category_system(),
				 GetLastError(),
				 0,
				 "[WIN] DeleteFileW failed!");
			}
		}
		pulsar_api void
		directory_remove(
		 path_view __p)
		{
			// Initialize
			auto ctrl = make_unique_s<__iteration_rec_remove_controller_win_t>();

			// Folder
			const size_t wlen = utf8_to_utf16le::required_count(__p.data(), __p.length()) + 7;
			auto fold					= new_construct_ex<__iteration_rec_remove_folder_win_t>(ctrl->all, wlen * sizeof(u16char_t), wlen);
			fold->buf[0u]			= L'\\';
			fold->buf[1u]			= L'\\';
			fold->buf[2u]			= L'?';
			fold->buf[3u]			= L'\\';
			utf8_to_utf16le::convert(__p.data(), __p.length(), union_cast<u16char_t *>(&fold->buf[0u]) + 4);
			__convert_forwardslash_to_backslash_u16_win(&fold->buf[0u] + 4, wlen - 7);
			fold->buf[wlen - 3u] = L'\\';
			fold->buf[wlen - 2u] = L'*';
			fold->buf[wlen - 1u] = L'\0';
			ctrl->head					 = fold;
			ctrl->tail					 = fold;
			ctrl->curr					 = fold;

			// Handle
			WIN32_FIND_DATAW data;
			ctrl->handle = FindFirstFileExW(&fold->buf[0u], FindExInfoBasic, &data, FindExSearchNameMatch, nullptr, FIND_FIRST_EX_CASE_SENSITIVE | FIND_FIRST_EX_LARGE_FETCH);
			pf_throw_if(
			 ctrl->handle == INVALID_HANDLE_VALUE,
			 dbg_category_system(),
			 GetLastError(),
			 0,
			 "FindFirstFileExW failed!");

			// Remove -> First
			__directory_remove(ctrl, data);

			// Iterate
			while(true)
			{
				// Find next
				if(!FindNextFileW(ctrl->handle, &data))
				{
					// Find last handle
					FindClose(ctrl->handle);
					ctrl->curr = ctrl->curr->next;
					if(!ctrl->curr)
					{
						while(ctrl->tail)
						{
							ctrl->tail->buf[ctrl->tail->wlen - 3u] = '\0';
							pf_throw_if(
							 !RemoveDirectoryW(&ctrl->tail->buf[0u]),
							 dbg_category_system(),
							 GetLastError(),
							 0,
							 "[WIN] RemoveDirectoryW failed!");
							ctrl->tail = ctrl->tail->prev;
						}
						return;
					}

					// Next
					ctrl->handle = FindFirstFileExW(&ctrl->curr->buf[0u], FindExInfoBasic, &data, FindExSearchNameMatch, nullptr, FIND_FIRST_EX_CASE_SENSITIVE | FIND_FIRST_EX_LARGE_FETCH);
				}

				// Remove -> next
				__directory_remove(ctrl, data);
			}
		}
		pulsar_api void
		directory_create_symlink(
		 path_view __link,
		 path_view __target)
		{
			// wlink
			size_t wlinklen;
			auto wlink = __cast_utf8_to_wchar_buffer_win(__link, wlinklen);

			// wtarget
			size_t wtargetlen;
			auto wtarget = __cast_utf8_to_wchar_buffer_win(__target, wtargetlen);

			// Link
			pf_throw_if(
			 !CreateSymbolicLinkW(&wlink[0u], &wtarget[0u], SYMBOLIC_LINK_FLAG_DIRECTORY),
			 dbg_category_system(),
			 GetLastError(),
			 0,
			 "[WIN] CreateSymbolicLinkW failed!");
		}
		pulsar_api void
		directory_copy_symlink(
		 path_view __from,
		 path_view __dest,
		 bool __overwrite)
		{
			// wfrom
			size_t wfromlen;
			auto wfrom = __cast_utf8_to_wchar_buffer_win(__from, wfromlen);
			wfrom			 = __make_symlink_path_buffer_s_win(wfrom, wfromlen, GetFileAttributesW(&wfrom[0u]));

			// wdest
			size_t wdestlen;
			auto wdest = __cast_utf8_to_wchar_buffer_win(__dest, wdestlen);
			if(__overwrite && GetFileAttributesW(&wdest[0u]) != INVALID_FILE_ATTRIBUTES)
			{
				directory_remove(__dest);
			}

			// Create
			pf_throw_if(
			 !CreateSymbolicLinkW(&wdest[0u], &wfrom[0u], SYMBOLIC_LINK_FLAG_DIRECTORY),
			 dbg_category_system(),
			 GetLastError(),
			 0,
			 "[WIN] CreateSymbolicLinkW failed!");
		}
		pulsar_api void
		directory_create_hardlink(
		 path_view __link,
		 path_view __target)
		{
			// wlink
			size_t wlinklen;
			auto wlink = __cast_utf8_to_wchar_buffer_ex_win(__link, wlinklen, 2);

			// wtarget
			size_t wtargetlen;
			auto wtarget = __cast_utf8_to_wchar_buffer_win(__target, wtargetlen);

			// Security
			SECURITY_ATTRIBUTES sa;
			__generate_security_descriptor_win(perm::owner_all, sa);

			// Exist?
			try
			{
				if(GetFileAttributesW(&wlink[0u]) != INVALID_FILE_ATTRIBUTES)
				{
					pf_throw_if(!__directory_is_empty_win(wlink, wlinklen), dbg_category_system(), ERROR_BAD_PATHNAME, 0, "[WIN] __link isn't an empty or inexisting directory! __path={}", __link.data());
					wlink[wlinklen - 3] = L'\0';
				}
				else
				{
					__directory_create_rec_win(wlink, wlinklen, sa);
				}

				// File Handle
				HANDLE dirhandle = CreateFileW(
				 &wlink[0u],
				 FILE_GENERIC_WRITE | FILE_GENERIC_READ,
				 0,
				 &sa,
				 OPEN_EXISTING,
				 FILE_ATTRIBUTE_NORMAL,
				 nullptr);
				pf_throw_if(
				 dirhandle == INVALID_HANDLE_VALUE,
				 dbg_category_system(),
				 GetLastError(),
				 0,
				 "[WIN] CreateFileW failed!");

				// DeviceIO
				try
				{
					pf_throw_if(
					 !DeviceIoControl(
						dirhandle,
						FSCTL_SET_REPARSE_POINT,
						&wtarget[0u],
						union_cast<DWORD>(wtargetlen),
						nullptr,
						0,
						nullptr,
						nullptr),
					 dbg_category_system(),
					 GetLastError(),
					 0,
					 "[WIN] DeviceIoControl failed!");
				} catch(std::exception const &)
				{
					pf_throw_if(!CloseHandle(dirhandle), dbg_category_system(), GetLastError(), 0, "[WIN] CloseHandle failed!");
				}

				// Close
				pf_throw_if(
				 !CloseHandle(dirhandle),
				 dbg_category_system(),
				 GetLastError(),
				 0,
				 "[WIN] CloseHandle failed!");
			} catch(std::exception const &)
			{
				LocalFree(sa.lpSecurityDescriptor);
				throw;
			}

			// Free-up
			LocalFree(sa.lpSecurityDescriptor);
		}
		pulsar_api path<>
		directory_known(
		 known __known)
		{
			// Using SHObj -> Known
			KNOWNFOLDERID ID;
			switch(__known)
			{
				case known::process_root:
				{
					DWORD wlen = GetCurrentDirectoryW(0, nullptr);
					auto wbuf	 = make_unique_s<WCHAR[]>(wlen);
					pf_throw_if(!GetCurrentDirectoryW(wlen, &wbuf[0u]), dbg_category_system(), GetLastError(), 0, "[WIN] GetCurrentDirectory failed!");
					return __cast_utf16le_to_utf8_buffer_win(&wbuf[0u], wlen, 0);
				};
				case known::shared:
				{
					ID = FOLDERID_RoamingAppData;
					break;
				};
				case known::local:
				{
					ID = FOLDERID_LocalAppData;
					break;
				};
				case known::user:
				{
					size_t wlen = MAX_PATH;
					auto wbuf		= make_unique_s<WCHAR[]>(wlen);
					pf_throw_if(!SHGetSpecialFolderPathW(nullptr, &wbuf[0u], CSIDL_PROFILE, true), dbg_category_system(), GetLastError(), 0, "[WIN] SHGetSpecialFolderPathW failed!");
					return __cast_utf16le_to_utf8_buffer_win(&wbuf[0u], utf16le::lenof(union_cast<u16char_t *>(&wbuf[0u])), 0);
				};
				case known::temporary:
				{
					size_t wlen = MAX_PATH + 1;
					auto wbuf		= make_unique_s<WCHAR[]>(wlen);
					DWORD wret	= GetTempPathW(union_cast<DWORD>(wlen), &wbuf[0u]);
					pf_throw_if(!wret, dbg_category_system(), GetLastError(), 0, "[WIN] GetTempPathW failed!");
					return __cast_utf16le_to_utf8_buffer_win(&wbuf[0u], wret + 1, 0);
				};
			}

			// Known
			PWSTR known;

			// Convert
			try
			{
				pf_throw_if(SHGetKnownFolderPath(ID, KF_FLAG_DEFAULT, nullptr, &known) != S_OK, dbg_category_system(), GetLastError(), 0, "[WIN] SHGetKnownFolderPath failed!");
				auto str = __cast_utf16le_to_utf8_buffer_win(&known[0u], utf16le::lenof(union_cast<u16char_t *>(&known[0u])), 0);
				CoTaskMemFree(known);
				return str;
			} catch(std::exception const &)
			{
				CoTaskMemFree(known);
				throw;
			}
		}
		pulsar_api void
		set_current_directory(
		 path_view __p)
		{
			size_t wlen;
			auto wbuf = __cast_utf8_to_wchar_buffer_win(__p, wlen);
			pf_throw_if(
			 !SetCurrentDirectoryW(&wbuf[0u]),
			 dbg_category_system(),
			 GetLastError(),
			 0,
			 "[WIN] SetCurrentDirectory failed!");
		}

		/// Pathing
		pulsar_api void
		file_create(
		 path_view __p,
		 perm_t __perms)
		{
			// Initialisation
			size_t wlen;
			auto wpath = __cast_utf8_to_wchar_buffer_win(__p, wlen);

			// Security
			SECURITY_ATTRIBUTES sa;
			__generate_security_descriptor_win(__perms, sa);

			// File
			HANDLE file = CreateFileW(
			 &wpath[0u],
			 FILE_GENERIC_WRITE,
			 0,
			 &sa,
			 OPEN_ALWAYS,
			 FILE_ATTRIBUTE_NORMAL,
			 nullptr);
			pf_throw_if(
			 file == INVALID_HANDLE_VALUE,
			 dbg_category_system(),
			 GetLastError(),
			 0,
			 "[WIN] File failed!");
			pf_throw_if(
			 !CloseHandle(file),
			 dbg_category_system(),
			 GetLastError(),
			 0,
			 "[WIN] CloseHandle failed!");
		}
		pulsar_api void
		file_remove(
		 path_view __p)
		{
			// Wide
			size_t wlen;
			auto wpath = __cast_utf8_to_wchar_buffer_win(__p, wlen);

			// Delete
			pf_throw_if(
			 !DeleteFileW(&wpath[0u]),
			 dbg_category_system(),
			 GetLastError(),
			 0,
			 "[WIN] DeleteFileW failed!");
		}
		pf_decl_inline void
		__file_copy_win(
		 path_view __from,
		 path_view __dest,
		 DWORD __flags)
		{
			// Wide
			size_t wfromlen, wdestlen;
			auto wfrom = __cast_utf8_to_wchar_buffer_win(__from, wfromlen);
			auto wdest = __cast_utf8_to_wchar_buffer_win(__dest, wdestlen);

			// Copy
			pf_throw_if(
			 !CopyFileExW(&wfrom[0u], &wdest[0u], nullptr, nullptr, nullptr, __flags),
			 dbg_category_system(),
			 GetLastError(),
			 0,
			 "[WIN] CopyFileW failed!");
		}
		pulsar_api void
		file_copy(
		 path_view __from,
		 path_view __dest,
		 bool __overwrite)
		{
			// Copy
			__file_copy_win(__from, __dest, __overwrite ? 0 : COPY_FILE_FAIL_IF_EXISTS);
		}
		pulsar_api void
		file_create_symlink(
		 path_view __link,
		 path_view __target)
		{
			// Wide
			size_t wlinklen, wtargetlen;
			auto wlink	 = __cast_utf8_to_wchar_buffer_win(__link, wlinklen);
			auto wtarget = __cast_utf8_to_wchar_buffer_win(__target, wtargetlen);

			// Create Symlink
			pf_throw_if(
			 !CreateSymbolicLinkW(&wlink[0u], &wtarget[0u], 0),
			 dbg_category_system(),
			 GetLastError(),
			 0,
			 "[WIN] CreateSymbolicLinkW failed!");
		}
		pulsar_api void
		file_copy_symlink(
		 path_view __from,
		 path_view __dest,
		 bool __overwrite)
		{
			// Options
			DWORD options = COPY_FILE_COPY_SYMLINK;
			options			 |= __overwrite ? 0 : COPY_FILE_FAIL_IF_EXISTS;

			// Copy
			__file_copy_win(__from, __dest, options);
		}
		pulsar_api void
		file_create_hardlink(
		 path_view __link,
		 path_view __target)
		{
			// Wide
			size_t wlinklen, wtargetlen;
			auto wlink	 = __cast_utf8_to_wchar_buffer_win(__link, wlinklen);
			auto wtarget = __cast_utf8_to_wchar_buffer_win(__target, wtargetlen);

			// Create Hardlink
			pf_throw_if(
			 !CreateHardLinkW(&wlink[0u], &wtarget[0u], nullptr),
			 dbg_category_system(),
			 GetLastError(),
			 0,
			 "[WIN] CreateHardLinkW failed!");
		}


		/// FILESYSTEM: Common
		pf_hint_nodiscard perm_t
		__get_perms_of_win(
		 WCHAR *__wpath)
		{
			// Get Security
			DWORD strlen;
			const DWORD security = DACL_SECURITY_INFORMATION;
			GetFileSecurityW(&__wpath[0u], security, nullptr, 0, &strlen);
			auto sd = make_unique_ex_s<SECURITY_DESCRIPTOR>(strlen - sizeof(SECURITY_DESCRIPTOR), 0u, 0u, 0u, nullptr, nullptr, nullptr, nullptr);
			pf_throw_if(
			 !GetFileSecurityW(&__wpath[0u], security, sd.get(), strlen, &strlen),
			 dbg_category_system(),
			 GetLastError(),
			 0,
			 "[WIN] GetFileSecurityW failed!");

			// Convert
			strlen		 = 0;
			LPWSTR str = nullptr;
			pf_throw_if(
			 !ConvertSecurityDescriptorToStringSecurityDescriptorW(sd.get(), SDDL_REVISION_1, security, &str, &strlen),
			 dbg_category_system(),
			 GetLastError(),
			 0,
			 "[WIN] ConvertSecurityDescriptorToStringSecurityDescriptorW failed!");
			strlen = union_cast<DWORD>(utf16le::lenof(union_cast<u16char_t *>(&str[0u])) - 1);

			// Get current user SID
			size_t usersidlen;
			auto usersid = __generate_user_sid_string_win(usersidlen);

			// Iterate through str
			perm_t perms;
			size_t p = 2;
			while(p != strlen)
			{
				// A or D?
				++p;
				if(str[p] != L'A' || str[p + 1] != L';')
				{
					while(str[p] != L')') ++p;
					++p;
					continue;
				}

				// Access
				uint32_t access = 0;

				// Look for 2 ;
				size_t c = 0;
				p				+= 2;
				while(c != 1)
				{
					if(str[p] == L';') ++c;
					++p;
				}

				// Accesses
				while(true)
				{
					if(str[p] == L'F')
					{
						++p;
						if(str[p] == L'R')
						{
							access |= 0x01;
						}
						else if(str[p] == L'W')
						{
							access |= 0x02;
						}
						else if(str[p] == L'X')
						{
							access |= 0x04;
						}
						else if(str[p] == L'A')
						{
							access |= 0x01 | 0x02 | 0x04;
						}
					}
					else if(str[p] == L';')
					{
						++p;
						break;
					}
					++p;
				}

				// Skip
				c = 0;
				while(c != 2)
				{
					if(str[p] == L';') ++c;
					++p;
				}

				// SID
				if(str[p] == 'B')
				{
					++p;
					if(str[p] == 'U')
					{
						++p;
						if(access & 0x01) perms |= perm::users_read;
						if(access & 0x02) perms |= perm::users_write;
						if(access & 0x04) perms |= perm::users_exec;
					}
					else if(str[p] == 'G')
					{
						++p;
						if(access & 0x01) perms |= perm::guest_read;
						if(access & 0x02) perms |= perm::guest_write;
						if(access & 0x04) perms |= perm::guest_exec;
					}
					else
					{
						// Skip
						while(str[p] != L')') ++p;
						++p;
						continue;
					}
				}
				else
				{
					size_t len = p;
					while(str[len] != ')') ++len;
					len -= p;
					if(len == usersidlen && equal(iterator(&str[0u] + p), iterator(&str[0u] + p + len), iterator(&usersid[0u])))
					{
						if(access & 0x01) perms |= perm::owner_read;
						if(access & 0x02) perms |= perm::owner_write;
						if(access & 0x04) perms |= perm::owner_exec;
					}
					else
					{
						// Skip
						p += len + 1;
						continue;
					}
					p += len + 1;
				}
			}

			// Free
			LocalFree(str);

			// Perms
			return perms;
		}
		void
		__set_perms_of_win(
		 WCHAR *__wpath,
		 perm_t __perms)
		{
			/// Security
			SECURITY_ATTRIBUTES sa;
			__generate_security_descriptor_win(__perms, sa);

			/// Set
			try
			{
				pf_throw_if(
				 !SetFileSecurityW(&__wpath[0u], DACL_SECURITY_INFORMATION, sa.lpSecurityDescriptor),
				 dbg_category_system(),
				 GetLastError(),
				 0,
				 "[WIN] SetFileSecurityW failed!");
				LocalFree(sa.lpSecurityDescriptor);
			} catch(std::exception const &)
			{
				LocalFree(sa.lpSecurityDescriptor);
				throw;
			}
		}
		pulsar_api perm_t
		perms(
		 path_view __p)
		{
			// Wide
			size_t wlen;
			auto wpath = __cast_utf8_to_wchar_buffer_win(__p, wlen);

			// Perms
			return __get_perms_of_win(&wpath[0u]);
		}
		pulsar_api perm_t
		symlink_perms(
		 path_view __link)
		{
			// Wide
			size_t wlen;
			auto wlink = __cast_utf8_to_wchar_buffer_win(__link, wlen);
			wlink			 = __make_symlink_path_buffer_s_win(&wlink[0u], wlen, GetFileAttributesW(&wlink[0u]));

			// Perms
			return __get_perms_of_win(&wlink[0u]);
		}
		pf_hint_nodiscard pf_decl_inline perm_t
		__transform_perm_win(
		 perm_t __old,
		 perm_t __new,
		 perm_opt_t __opt)
		{
			switch(__opt)
			{
				case perm_opt::add: return __old |= __new;
				case perm_opt::replace: return __new;
				case perm_opt::remove: return __old &= ~__new;
			};
			return perm::none;
		}
		pulsar_api perm_t
		set_perms(
		 path_view __p,
		 perm_t __perms,
		 perm_opt_t __opt)
		{
			// Wide
			size_t wlen;
			auto wpath = __cast_utf8_to_wchar_buffer_win(__p, wlen);

			// Options
			perm_t old = __get_perms_of_win(&wpath[0u]);
			perm_t set = __transform_perm_win(old, __perms, __opt);

			// Set
			__set_perms_of_win(&wpath[0u], set);

			// Old
			return old;
		}
		pulsar_api perm_t
		set_symlink_perms(
		 path_view __link,
		 perm_t __perms,
		 perm_opt_t __opt)
		{
			// Wide
			size_t wlen;
			auto wlink = __cast_utf8_to_wchar_buffer_win(__link, wlen);
			wlink			 = __make_symlink_path_buffer_s_win(&wlink[0u], wlen, GetFileAttributesW(&wlink[0u]));

			// Options
			perm_t old = __get_perms_of_win(&wlink[0u]);
			perm_t set = __transform_perm_win(old, __perms, __opt);

			// Set
			__set_perms_of_win(&wlink[0u], set);

			// Old
			return old;
		}
		pulsar_api void
		move(
		 path_view __from,
		 path_view __dest,
		 bool __overwrite)
		{
			// Wide
			size_t wfromlen, wdestlen;
			auto wfrom = __cast_utf8_to_wchar_buffer_win(__from, wfromlen);
			auto wdest = __cast_utf8_to_wchar_buffer_win(__dest, wdestlen);

			// Move
			pf_throw_if(
			 !MoveFileExW(&wfrom[0u], &wdest[0u], MOVEFILE_COPY_ALLOWED | (__overwrite ? MOVEFILE_REPLACE_EXISTING : 0)),
			 dbg_category_system(),
			 GetLastError(),
			 0,
			 "[WIN] MoveFileExW failed!");
		}
		pulsar_api void
		copy(
		 path_view __from,
		 path_view __dest,
		 bool __overwrite)
		{
			// Directory?
			if(attributes(__from) & attribute::directory)
			{
				copy_option_t opt = copy_option::recursive | copy_option::copy_symlinks;
				directory_copy(
				 __from,
				 __dest,
				 __overwrite
					? copy_option::overwrite_existing | opt
					: opt,
				 perms(__from));
			}
			else
			{
				file_copy(__from, __dest, __overwrite);
			}
		}
		pulsar_api void
		remove(
		 path_view __p)
		{
			// Directory?
			if(attributes(__p) & attribute::directory)
			{
				directory_remove(__p);
			}
			else
			{
				file_remove(__p);
			}
		}
		pulsar_api uint64_t
		size(
		 path_view __p)
		{
			// Information
			auto inf = information(__p);

			// Directory?
			if(inf.attributes & attribute::directory)
			{
				return directory_size(__p);
			}
			else
			{
				return inf.fileSize;
			}
		}
		pulsar_api void
		create_symlink(
		 path_view __link,
		 path_view __target)
		{
			// Directory?
			if(attributes(__link) & attribute::directory)
			{
				directory_create_symlink(__link, __target);
			}
			else
			{
				file_create_symlink(__link, __target);
			}
		}
		pulsar_api void
		copy_symlink(
		 path_view __from,
		 path_view __dest,
		 bool __overwrite)
		{
			// Directory?
			if(attributes(__from) & attribute::directory)
			{
				directory_copy_symlink(__from, __dest, __overwrite);
			}
			else
			{
				file_copy_symlink(__from, __dest, __overwrite);
			}
		}
		pulsar_api void
		create_hardlink(
		 path_view __link,
		 path_view __target)
		{
			// Directory?
			if(attributes(__link) & attribute::directory)
			{
				directory_create_hardlink(__link, __target);
			}
			else
			{
				file_create_hardlink(__link, __target);
			}
		}

		/// FILESYSTEM: File
		/// Internal
		pulsar_api void
		__file_close(
		 void *__handle) pf_attr_noexcept
		{
			CloseHandle(__handle);
		}
		pulsar_api path<>
		__file_get_path(
		 void *__handle)
		{
			DWORD wlen = GetFinalPathNameByHandleW(__handle, nullptr, 0, FILE_NAME_NORMALIZED | VOLUME_NAME_DOS) + 1;
			auto p		 = make_unique_s<WCHAR[]>(wlen);
			pf_throw_if(!GetFinalPathNameByHandleW(__handle, &p[0u], wlen, FILE_NAME_NORMALIZED | VOLUME_NAME_DOS), dbg_category_system(), GetLastError(), 0, "[WIN] GetFilePathNameByHandleW failed!");
			return __cast_utf16le_to_utf8_buffer_win(p, wlen);
		}
		pulsar_api void
		__file_flush_buffers(
		 void *__handle)
		{
			pf_throw_if(
			 !FlushFileBuffers(__handle),
			 dbg_category_system(),
			 GetLastError(),
			 0,
			 "[WIN] FlushFileBuffers failed!");
		}

		/// Lock
		pulsar_api void
		__file_do_lock(
		 void *__handle,
		 lock_mode __mode,
		 uint64_t __where,
		 uint64_t __size)
		{
			OVERLAPPED ov;
			memset(&ov, 0, sizeof(ov));
			ov.OffsetHigh = union_cast<uint32_t>(__where >> 32) & 0xFF'FF'FF'FF;
			ov.Offset			= union_cast<uint32_t>(__where) & 0xFF'FF'FF'FF;
			pf_throw_if(
			 !LockFileEx(__handle, __mode == lock_mode::exclusive ? LOCKFILE_EXCLUSIVE_LOCK : 0, 0, union_cast<uint32_t>(__size) & 0xFF'FF'FF'FF, union_cast<uint32_t>(__size >> 32) & 0xFF'FF'FF'FF, &ov),
			 dbg_category_system(),
			 GetLastError(),
			 0,
			 "[WIN] LockFileEx failed!");
		}
		pulsar_api void
		__file_do_unlock(
		 void *__handle,
		 uint64_t __where,
		 uint64_t __size)
		{
			OVERLAPPED ov;
			memset(&ov, 0, sizeof(ov));
			ov.OffsetHigh = union_cast<uint32_t>(__where >> 32) & 0xFF'FF'FF'FF;
			ov.Offset			= union_cast<uint32_t>(__where) & 0xFF'FF'FF'FF;
			pf_throw_if(
			 !UnlockFileEx(__handle, 0, union_cast<uint32_t>(__size) & 0xFF'FF'FF'FF, union_cast<uint32_t>(__size >> 32) & 0xFF'FF'FF'FF, &ov),
			 dbg_category_system(),
			 GetLastError(),
			 0,
			 "[WIN] UnlockFileEx failed!");
		}

		// File
		pf_hint_nodiscard pf_decl_inline DWORD
		__file_generic_hints_win(
		 io_access_t __access) pf_attr_noexcept
		{
			DWORD access = 0;
			if(__access & io_access::read) access |= FILE_GENERIC_READ;
			if(__access & io_access::write) access |= FILE_GENERIC_WRITE;
			if(__access & io_access::execute) access |= FILE_GENERIC_EXECUTE;
			return access;
		}
		pf_hint_nodiscard pf_decl_inline DWORD
		__file_shared_hints_win(
		 io_access_t __access) pf_attr_noexcept
		{
			DWORD access = 0;
			if(__access & io_access::shared_write) access |= FILE_SHARE_WRITE;
			if(__access & io_access::shared_read) access |= FILE_SHARE_READ;
			return access;
		}
		pf_hint_nodiscard pf_decl_inline DWORD
		__file_additionnal_hints_win(
		 io_access_t __access,
		 bool __async) pf_attr_noexcept
		{
			if(__async) return FILE_FLAG_OVERLAPPED;
			return (__access & io_access::random_access_cache) ? FILE_FLAG_RANDOM_ACCESS : FILE_FLAG_SEQUENTIAL_SCAN;
		}
		pulsar_api void *
		__file_do_open(
		 path_view __p,
		 io_access_t __access,
		 io_mode_t __modes,
		 bool __async)
		{
			/// Utf8
			size_t wlen;
			auto wpath = __cast_utf8_to_wchar_buffer_win(__p, wlen);

			/// Access
			DWORD access			= __file_generic_hints_win(__access);
			DWORD shared			= __file_shared_hints_win(__access);
			DWORD additionnal = __file_additionnal_hints_win(__access, __async);

			// Exist?
			pf_throw_if(
			 GetFileAttributesW(&wpath[0u]) == INVALID_FILE_ATTRIBUTES,
			 dbg_category_system(),
			 ERROR_PATH_NOT_FOUND,
			 0,
			 "[WIN] Trying to open unexisting path={}",
			 __p.data());

			/// Mode
			DWORD mode;
			switch(__modes)
			{
				case io_mode::overwrite:
				{
					mode = CREATE_ALWAYS;
					break;
				}
				case io_mode::truncate:
				{
					mode = TRUNCATE_EXISTING;
					break;
				}
				default:
				{
					mode = OPEN_EXISTING;
					break;
				}
			}

			/// Create File
			HANDLE hdl = CreateFileW(&wpath[0u], access, shared, nullptr, mode, additionnal, nullptr);
			pf_throw_if(
			 hdl == INVALID_HANDLE_VALUE,
			 dbg_category_system(),
			 GetLastError(),
			 0,
			 "[WIN] CreateFileW failed!");

			/// Handle
			return hdl;
		}
		pulsar_api void *
		file_do_reopen(
		 void *__handle,
		 io_access_t __access,
		 bool __async)
		{
			/// Access
			DWORD access			= __file_generic_hints_win(__access);
			DWORD shared			= __file_shared_hints_win(__access);
			DWORD additionnal = __file_additionnal_hints_win(__access, __async);

			/// Reopen
			__handle = ReOpenFile(__handle, access, shared, additionnal);
			pf_throw_if(!__handle, dbg_category_system(), GetLastError(), 0, "[WIN] ReOpenFile failed!");

			/// Handle
			return __handle;
		}

		// Sync
		pulsar_api uint32_t
		__file_sync_do_write(
		 void *__handle,
		 const void *__data,
		 uint32_t __size)
		{
			DWORD written;
			pf_throw_if(
			 !WriteFile(__handle, __data, __size, &written, nullptr),
			 dbg_category_system(),
			 GetLastError(),
			 0,
			 "[WIN] WriteFile failed!");
			return written;
		}
		pulsar_api uint32_t
		__file_sync_do_read(
		 void *__handle,
		 void *__data,
		 uint32_t __size)
		{
			DWORD written;
			pf_throw_if(
			 !ReadFile(__handle, __data, __size, &written, nullptr),
			 dbg_category_system(),
			 GetLastError(),
			 0,
			 "[WIN] ReadFile failed!");
			return written;
		}
		pf_hint_nodiscard pf_decl_inline DWORD
		__file_move_method_win(
		 ptr_mode_t __mode)
		{
			switch(__mode)
			{
				case ptr_mode::begin: return FILE_BEGIN;
				case ptr_mode::current: return FILE_CURRENT;
				case ptr_mode::end: return FILE_END;
			}
			return 3;
		}
		pulsar_api int64_t
		__file_sync_seek(
		 void *__handle,
		 ptr_mode_t __mode,
		 int64_t __where)
		{
			LARGE_INTEGER move;
			LARGE_INTEGER newpos;
			move.QuadPart = __where;
			pf_throw_if(
			 !SetFilePointerEx(__handle, move, &newpos, __file_move_method_win(__mode)),
			 dbg_category_system(),
			 GetLastError(),
			 0,
			 "[WIN] SetFilePointerEx failed!");
			return newpos.QuadPart;
		}
		pulsar_api void
		__file_set_pointer_end_of_file(
		 void *__handle)
		{
			pf_throw_if(
			 !SetEndOfFile(__handle),
			 dbg_category_system(),
			 GetLastError(),
			 0,
			 "[WIN] SetEndOfFile failed!");
		}

		// Async -> Internal Types
		struct __async_data_win_t
		{
			/// Constructors
			pf_decl_inline
			__async_data_win_t(
			 HANDLE __file,
			 uint64_t __where,
			 file_completion_callback_t __callback,
			 void *__userdata) pf_attr_noexcept
				: ov { 0, 0, union_cast<uint32_t>(__where), union_cast<uint32_t>(__where >> 32), nullptr }
				, file(__file)
				, callback(__callback)
				, userdata(__userdata)
			{}
			__async_data_win_t(__async_data_win_t const &) = delete;
			__async_data_win_t(__async_data_win_t &&)			 = delete;

			/// Destructor
			pf_decl_inline ~__async_data_win_t() pf_attr_noexcept = default;

			/// Operator =
			__async_data_win_t &
			operator=(__async_data_win_t const &) = delete;
			__async_data_win_t &
			operator=(__async_data_win_t &&) = delete;

			/// Store
			OVERLAPPED ov;
			HANDLE file;
			file_completion_callback_t callback;
			void *userdata;
		};
		struct __async_data_with_future_win_t
		{
		public:
			/// Constructors
			pf_decl_inline
			__async_data_with_future_win_t(
			 HANDLE __file,
			 uint64_t __where,
			 file_completion_callback_t __callback,
			 void *__userdata) pf_attr_noexcept
				: ov { 0, 0, union_cast<uint32_t>(__where), union_cast<uint32_t>(__where >> 32), nullptr }
				, finished(false)
				, file(__file)
				, callback(__callback)
				, userdata(__userdata)
				, written(0)
			{}
			__async_data_with_future_win_t(__async_data_with_future_win_t const &) = delete;
			__async_data_with_future_win_t(__async_data_with_future_win_t &&)			 = delete;

			/// Destructor
			pf_decl_inline ~__async_data_with_future_win_t() pf_attr_noexcept = default;

			/// Operator =
			__async_data_with_future_win_t &
			operator=(__async_data_with_future_win_t const &) = delete;
			__async_data_with_future_win_t &
			operator=(__async_data_with_future_win_t &&) = delete;

			/// Store
			OVERLAPPED ov;
			pf_alignas(CCY_ALIGN) atomic<bool> finished;
			HANDLE file;
			file_completion_callback_t callback;
			void *userdata;
			uint32_t written;
		};

		// Async -> Future
		void
		__file_future_data_wait(
		 __async_data_with_future_win_t *data) pf_attr_noexcept
		{
			if(!data->finished.load(atomic_order::acquire))
			{
				while(!data->finished.load(atomic_order::relaxed)) this_thread::yield();
			}
		}
		pulsar_api void
		__file_future_wait_and_close(
		 void *__handle) pf_attr_noexcept
		{
			__async_data_with_future_win_t *data = union_cast<__async_data_with_future_win_t *>(__handle);
			__file_future_data_wait(data);
			destroy_delete(data);
		}
		pulsar_api bool
		__file_future_is_finished(
		 void *__handle)
		{
			__async_data_with_future_win_t *data = union_cast<__async_data_with_future_win_t *>(__handle);
			return data->finished.load(atomic_order::relaxed);
		}
		pulsar_api bool
		__file_future_wait(
		 void *__handle)
		{
			__async_data_with_future_win_t *data = union_cast<__async_data_with_future_win_t *>(__handle);
			if(data->finished.load(atomic_order::acquire)) return false;
			while(!data->finished.load(atomic_order::relaxed)) this_thread::yield();
			return true;
		}
		pulsar_api uint32_t
		__file_future_retrieve_written(
		 void *__handle)
		{
			__async_data_with_future_win_t *data = union_cast<__async_data_with_future_win_t *>(__handle);
			__file_future_data_wait(data);
			return data->written;
		}

		// Async -> Internal
		pulsar_api pf_decl_static void
		__file_overlapped_completion_win(
		 __async_data_win_t *__ov)
		{
			// Result
			DWORD transfered;
			BOOL res = GetOverlappedResult(__ov->file, &__ov->ov, &transfered, false);

			// Unprocessed
			if(!res)
			{
				DWORD err = GetLastError();
				pf_throw_if(
				 err != ERROR_IO_INCOMPLETE,
				 dbg_category_system(),
				 err,
				 0,
				 "[WIN] GetOverlappedResult failed!");
				__internal.io_async_controller.submit_task(__file_overlapped_completion_win, __ov);
				return;
			}

			// Process
			auto callback	 = __ov->callback;
			uint64_t where = union_cast<uint64_t>(__ov->ov.Offset) + (union_cast<uint64_t>(__ov->ov.OffsetHigh) << 32);
			void *userdata = __ov->userdata;

			// Free
			destroy_delete_c(__ov);

			// Callback
			submit_task(callback.get(), where, transfered, userdata);
		}
		pulsar_api pf_decl_static void
		__file_overlapped_completion_with_future_win(
		 __async_data_with_future_win_t *__ov)
		{
			// Result
			DWORD transfered;
			BOOL res = GetOverlappedResult(__ov->file, &__ov->ov, &transfered, false);

			// Unprocessed
			if(!res)
			{
				DWORD err = GetLastError();
				pf_throw_if(
				 err != ERROR_IO_INCOMPLETE,
				 dbg_category_system(),
				 err,
				 0,
				 "[WIN] GetOverlappedResult failed!");
				__internal.io_async_controller.submit_task(__file_overlapped_completion_with_future_win, __ov);
				return;
			}

			// Process
			auto callback	 = __ov->callback;
			uint64_t where = union_cast<uint64_t>(__ov->ov.Offset) + (union_cast<uint64_t>(__ov->ov.OffsetHigh) << 32);
			void *userdata = __ov->userdata;

			// Finished
			__ov->written = transfered;
			__ov->finished.store(true, atomic_order::release);

			// Callback
			submit_task(callback.get(), where, transfered, userdata);
		}
		pulsar_api file_future_io_t
		__file_async_do_write_with_future(
		 void *__handle,
		 uint64_t __where,
		 const void *__data,
		 uint32_t __size,
		 file_completion_callback_t __callback,
		 void *__userdata)
		{
			// Initialisation
			__async_data_with_future_win_t *data = new_construct<__async_data_with_future_win_t>(__handle, __where, __callback, __userdata);

			// Write
			try
			{
				pf_throw_if(
				 !WriteFileEx(__handle, __data, __size, &data->ov, nullptr),
				 dbg_category_system(),
				 GetLastError(),
				 0,
				 "[WIN] WriteFileEx failed!");

				// Submit
				__internal.io_async_controller.submit_task(__file_overlapped_completion_with_future_win, data);
			} catch(dbg_exception_base const &)
			{
				destroy_delete(data);
				throw;
			}

			// Future
			return data;
		}
		pulsar_api file_future_io_t
		__file_async_do_read_with_future(
		 void *__handle,
		 uint64_t __where,
		 void *__data,
		 uint32_t __size,
		 file_completion_callback_t __callback,
		 void *__userdata)
		{
			// Initialisation
			__async_data_with_future_win_t *data = new_construct<__async_data_with_future_win_t>(__handle, __where, __callback, __userdata);

			// Read
			try
			{
				pf_throw_if(
				 !ReadFileEx(__handle, __data, __size, &data->ov, nullptr),
				 dbg_category_system(),
				 GetLastError(),
				 0,
				 "[WIN] ReadFileEx failed!");

				// Submit
				__internal.io_async_controller.submit_task(__file_overlapped_completion_with_future_win, data);
			} catch(dbg_exception_base const &)
			{
				destroy_delete(data);
				throw;
			}

			// Future
			return data;
		}
		pulsar_api void
		__file_async_do_write(
		 void *__handle,
		 uint64_t __where,
		 const void *__data,
		 uint32_t __size,
		 file_completion_callback_t __callback,
		 void *__userdata)
		{
			// Initialisation
			__async_data_win_t *data = new_construct_c<__async_data_win_t>(__handle, __where, __callback, __userdata);

			// Write
			try
			{
				pf_throw_if(
				 !WriteFileEx(__handle, __data, __size, &data->ov, nullptr),
				 dbg_category_system(),
				 GetLastError(),
				 0,
				 "[WIN] WriteFileEx failed!");

				// Submit
				__internal.io_async_controller.submit_task(__file_overlapped_completion_win, data);
			} catch(dbg_exception_base const &)
			{
				destroy_delete_c(data);
				throw;
			}
		}
		pulsar_api void
		__file_async_do_read(
		 void *__handle,
		 uint64_t __where,
		 void *__data,
		 uint32_t __size,
		 file_completion_callback_t __callback,
		 void *__userdata)
		{
			// Initialisation
			__async_data_win_t *data = new_construct_c<__async_data_win_t>(__handle, __where, __callback, __userdata);

			// Read
			try
			{
				pf_throw_if(
				 !ReadFileEx(__handle, __data, __size, &data->ov, nullptr),
				 dbg_category_system(),
				 GetLastError(),
				 0,
				 "[WIN] ReadFileEx failed!");

				// Submit
				__internal.io_async_controller.submit_task(__file_overlapped_completion_win, data);
			} catch(dbg_exception_base const &)
			{
				destroy_delete_c(data);
				throw;
			}
		}

	}	 // namespace fs
}	 // namespace pul

#endif	// !PF_OS_WINDOWS
