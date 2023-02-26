/*! @file   memory_utility.hpp
 *  @author Louis-Quentin Noé (noe.louis-quentin@hotmail.fr)
 *  @brief
 *  @date   19-02-2023
 *
 *  @copyright Copyright (c) 2023 - Pulsar Software
 *
 *  @since 0.1.4
 */

#ifndef PULSAR_MEMORY_UTILITY_HPP
#define PULSAR_MEMORY_UTILITY_HPP 1

// Include: Pulsar
#include "pulsar/memory.hpp"

// Pulsar
namespace pul
{

	// TODO: Enable -> Large OS Page
	// TODO: Mimalloc options

	/// MEMORY: Allocate
	pf_hint_nodiscard pf_decl_inline pf_decl_constexpr void*
	allocate(
		size_t __size,
		align_val_t __align = ALIGN_DEFAULT,
		size_t __offset			= 0) pf_attr_noexcept
	{
		if(std::is_constant_evaluated())
		{
			return new byte_t[__size];
		}
		else
		{
			return mi_malloc_aligned_at(
				__size,
				union_cast<size_t>(__align),
				__offset);
		}
	}
	template <typename _Allocator>
	pf_hint_nodiscard pf_decl_inline pf_decl_constexpr void*
	allocate(
		_Allocator  &__all,
		size_t __size,
		align_val_t __align = ALIGN_DEFAULT,
		size_t __offset			= 0) pf_attr_noexcept
	requires(is_allocator_v<_Allocator>)
	{
		if(std::is_constant_evaluated())
		{
			return new byte_t[__size];
		}
		else
		{
			return __all.allocate(
				__size,
				union_cast<size_t>(__align),
				__offset);
		}
	}

	/// MEMORY: Deallocate
	pf_decl_inline pf_decl_constexpr void
	deallocate(
		void *__ptr) pf_attr_noexcept
	{
		if (std::is_constant_evaluated())
		{
			delete[] union_cast<byte_t*>(__ptr);
		}
		else
		{
			mi_free(__ptr);
		}
	}
	template <typename _Allocator>
	pf_decl_inline pf_decl_constexpr void
	deallocate(
		_Allocator &__all,
		void *__ptr) pf_attr_noexcept
	{
		if (std::is_constant_evaluated())
		{
			delete[] union_cast<byte_t*>(__ptr);
		}
		else
		{
			__all.deallocate(__ptr);
		}
	}

	/// MEMORY: Reallocate
	pf_decl_inline pf_decl_constexpr void*
	reallocate(
		void *__ptr,
		size_t __osize,
		size_t __nsize,
		align_val_t __nalign = ALIGN_DEFAULT,
		size_t __noffset		 = 0) pf_attr_noexcept
	{
		if (std::is_constant_evaluated())
		{
			byte_t *p = new byte_t[__nsize];
			if (!__ptr) return p;
			const size_t m = std::min(__osize, __nsize);
			std::memcpy(p, __ptr, std::min(__osize, __nsize));
			if (__nsize > __osize) std::memset(p + m, 0, __nsize - __osize);
			deallocate(__ptr);
			return p;
		}
		else
		{
			return mi_realloc_aligned_at(__ptr, __nsize, union_cast<size_t>(__nalign), __noffset);
		}
	}
	template <typename _Allocator>
	pf_decl_inline pf_decl_constexpr void*
	reallocate(
		_Allocator  &__all,
		void *__ptr,
		size_t __osize,
		size_t __nsize,
		align_val_t __align = ALIGN_DEFAULT,
		size_t __offset			= 0) pf_attr_noexcept
	{
		if (std::is_constant_evaluated())
		{
			byte_t *p = new byte_t[__nsize];
			if (!__ptr) return p;
			std::memcpy(p, __ptr, std::min(__osize, __nsize));
			deallocate(__all, __ptr);
			return p;
		}
		else
		{
			return __all.reallocate(__ptr, __nsize, __align, __offset);	// TODO: Note is it correct?
		}
	}

	/*
	 * !   !
	 *  RAM
	 * !   !
	 */

	/// MEMORY: RAM -> Struct
	struct ram_info_t
	{
		// TODO: ram_info_t
	};

	/// MEMORY: RAM -> Function
	pf_hint_nodiscard pulsar_api ram_info_t
	get_process_ram_info() pf_attr_noexcept;
	// pf_hint_nodiscard pulsar_api ram_info_t
	// get_module_ram_info(module const &__module) pf_attr_noexcept;
	pf_hint_nodiscard pulsar_api ram_info_t
	get_machine_ram_info() pf_attr_noexcept;
}


#endif // !PULSAR_MEMORY_UTILITY_HPP