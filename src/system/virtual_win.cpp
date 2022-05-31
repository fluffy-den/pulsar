/*! @file   virtual_win.cpp
 *  @author Fluffy (noe.louis-quentin@hotmail.fr)
 *  @brief	Virtual memory implementation under windows.
 *  @date   29-03-2022
 *
 *  @copyright Copyright (c) 2022 - Pulsar Software
 *
 *  @since 0.1.1
 */

// Include: Pulsar
#include "pulsar/debug.hpp"
#include "pulsar/pulsar.hpp"
#include "pulsar/system.hpp"

// Include: Windows
#ifdef PF_OS_WINDOWS
#	include <windows.h>
#	include <memoryapi.h>

// Pulsar
namespace pul
{
	// Memory
	namespace memory
	{
		/// VIRTUAL: Memory
		/*! @brief Convert pulsar access flags (VIRTUAL_ACC_*) to windows access flags (PAGE_*).
		 *
		 *  @param[in] __access Pulsar access flags.
		 *  @return Windows access flags.
		 */
		pf_hint_nodiscard pf_decl_constexpr uint32_t __virtual_access_to_win_cast(
				uint32_t __access) pf_attr_noexcept
		{
			bool bexec	 = (__access & VIRTUAL_ACC_EXECUTE_BIT) == VIRTUAL_ACC_EXECUTE_BIT;
			bool bread	 = (__access & VIRTUAL_ACC_READ_BIT) == VIRTUAL_ACC_READ_BIT;
			bool bwrite	 = (__access & VIRTUAL_ACC_WRITE_BIT) == VIRTUAL_ACC_WRITE_BIT;
			// Additionnal
			uint32_t val = 0;
			if ((__access & VIRTUAL_ACC_GUARD_BIT) == VIRTUAL_ACC_GUARD_BIT)
				val |= PAGE_GUARD;
			if ((__access & VIRTUAL_ACC_NOCACHE_BIT) == VIRTUAL_ACC_NOCACHE_BIT)
				val |= PAGE_NOCACHE;
			// Access
			if (bexec)
			{
				if (bread && bwrite)
					return val | PAGE_EXECUTE_READWRITE;
				else if (bread)
					return val | PAGE_EXECUTE_READ;
				else if (bwrite)
					return val | PAGE_EXECUTE_WRITECOPY;
				else
					return val | PAGE_EXECUTE;
			}
			else
			{
				if (bread && bwrite)
					return val | PAGE_READWRITE;
				else if (bread)
					return val | PAGE_READONLY;
				else if (bwrite)
					return val | PAGE_WRITECOPY;
			}
			return val | PAGE_NOACCESS;
		}
		/*! @brief Convert windows access flags (PAGE_*) to pulsar access flags (VIRTUAL_ACC_*).
		 *
		 *  @param[in] __access Windows access flags.
		 *  @return Pulsar access flags.
		 */
		pf_hint_nodiscard pf_decl_constexpr uint32_t __virtual_access_to_pul_cast(
				uint32_t __access) pf_attr_noexcept
		{
			uint32_t a = 0;
			// Access
			switch (__access)
			{
				case PAGE_EXECUTE_READWRITE:
					a |= VIRTUAL_ACC_EXECUTE_BIT
						 | VIRTUAL_ACC_READ_BIT
						 | VIRTUAL_ACC_WRITE_BIT;
					break;
				case PAGE_EXECUTE_READ:
					a |= VIRTUAL_ACC_EXECUTE_BIT
						 | VIRTUAL_ACC_READ_BIT;
					break;
				case PAGE_EXECUTE_WRITECOPY:
					a |= VIRTUAL_ACC_EXECUTE_BIT
						 | VIRTUAL_ACC_WRITE_BIT;
					break;
				case PAGE_EXECUTE:
					a |= VIRTUAL_ACC_EXECUTE_BIT;
					break;
				case PAGE_READWRITE:
					a |= VIRTUAL_ACC_READ_BIT
						 | VIRTUAL_ACC_WRITE_BIT;
					break;
				case PAGE_READONLY:
					a |= VIRTUAL_ACC_READ_BIT;
					break;
				case PAGE_WRITECOPY:
					a |= VIRTUAL_ACC_WRITE_BIT;
					break;
				case PAGE_NOACCESS:
					a |= VIRTUAL_ACC_NOACCESS_BIT;
					break;
			};
			// Additionnal
			if ((__access & PAGE_GUARD) == PAGE_GUARD)
				a |= VIRTUAL_ACC_GUARD_BIT;
			if ((__access & PAGE_NOCACHE) == PAGE_NOCACHE)
				a |= VIRTUAL_ACC_NOCACHE_BIT;
			return a;
		}

		pulsar_api pf_hint_nodiscard void *virtual_reserve(
				size_t __size)
		{
			void *all = VirtualAllocEx(
					GetCurrentProcess(),
					nullptr,
					__size,
					MEM_RESERVE,
					PAGE_NOACCESS);
			if (!all)
				throw(exception(
						std::system_category(),
						GetLastError(),
						"",
						debug_flags::default_flags | debug_flags::dump_with_handle_data));
			return all;
		}
		pulsar_api void *virtual_alloc(
				void *__where,
				size_t __size,
				uint32_t __access)
		{
			void *all = VirtualAllocEx(
					GetCurrentProcess(),
					__where,
					__size,
					MEM_COMMIT,
					__virtual_access_to_win_cast(__access));
			if (!all)
				throw(exception(
						std::system_category(),
						GetLastError(),
						"",
						debug_flags::default_flags | debug_flags::dump_with_handle_data));
			return all;
		}
		pulsar_api void virtual_free(
				void *__where,
				size_t __size)
		{
			if (!VirtualFreeEx(GetCurrentProcess(), __where, __size, MEM_DECOMMIT))
				throw(exception(
						std::system_category(),
						GetLastError(),
						"",
						debug_flags::default_flags | debug_flags::dump_with_handle_data));
		}
		pulsar_api void virtual_release(
				void *__base)
		{
			if (!VirtualFreeEx(GetCurrentProcess(), __base, 0, MEM_RELEASE))
				throw(exception(
						std::system_category(),
						GetLastError(),
						"",
						debug_flags::default_flags | debug_flags::dump_with_handle_data));
		}
		pulsar_api pf_hint_nodiscard size_t virtual_access(
				void *__where,
				size_t __size,
				uint32_t __access)
		{
			DWORD a;
			if (!VirtualProtectEx(
							GetCurrentProcess(),
							__where,
							__size,
							__virtual_access_to_win_cast(__access),
							&a))
				throw(exception(
						std::system_category(),
						GetLastError(),
						"",
						debug_flags::default_flags | debug_flags::dump_with_handle_data));
			return __virtual_access_to_pul_cast(a);
		}

	}
}

#endif // PF_OS_WINDOWS