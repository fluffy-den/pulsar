/**
 * @file    memory_utils.hpp
 * @author  Fluffy (noe.louis-quentin@hotmail.fr)
 * @brief   Adds utility functions to memory usage
 * @date    15-12-2021
 *
 * @copyright Copyright (c) 2021 - Pulsar Software
 *
 */

#ifndef PULSAR_MEMORY_UTILS_HPP
#define PULSAR_MEMORY_UTILS_HPP 1

// Include: Pulsar
#include "pulsar/debug.hpp"

// Include: C++
#include <utility>

// Pulsar
namespace pulsar
{
	/**
	 * @brief Calculates the offset of a memory sequence starting at address @a __addr needed to
	 * 				align the sequence to the value of @a __align
	 *
	 * @param __addr	Memory address of the beginning of the sequence
	 * @param __align Desired alignment of the memory
	 *
	 * @return 0 if @a __align is equal to 0
	 * 				 The offset required to align the memory sequence otherwise
	 */
	pf_hint_nodiscard pf_decl_constexpr ptrsize_t padding_of(ptrsize_t __addr,
																													 ptrsize_t __align) pf_attr_noexcept
	{
		if (__align == 0)
			return 0;
		return (__align - (__addr & (__align - 1))) & (__align - 1);
	}

	/**
	 * @brief Computes from the base @a __ptr a new pointer aligned to the value of @a __align by an
	 * 			 	offset of @a __offset
	 *
	 * @param __ptr		 Pointer to a memory sequence
	 * @param __align	 Desired alignment of the memory
	 * @param __offset Head offset of the new pointer (optional)
	 *
	 * @return Aligned pointer
	 */
	pf_hint_nodiscard pf_decl_constexpr void *align_top(void *__ptr,
																											ptrsize_t __align,
																											ptrsize_t __offset = 0) pf_attr_noexcept
	{
		union
		{
			void *as_voidptr;
			ptrsize_t as_addr;
		};

		as_voidptr = __ptr;
		as_addr += padding_of(as_addr + __offset, __align);
		return as_voidptr;
	}

	/**
	 * @brief Calls the constructor of the @a _Ty object parameterized by @a _Args explicitly on a
	 * 				memory sequence starting with the @a __ptr pointer of @a __args parameters
	 * 				This function isn't defined when the constructor of @a _Ty does not exist or is not
	 * 				visible in the context of this call
	 *
	 * @tparam _Ty   A class with a constructor parameterized by the argument(s) of type(s) @a _Args
	 * @tparam _Args The arguments of a @a _Ty constructor
	 *
	 * @param __ptr	 Pointer to a memory sequence
	 * @param __args Parameters sent to a constructor of @a _Ty
	 */
	template <typename _Ty, typename... _Args>
	pf_decl_constexpr std::enable_if_t<std::is_constructible_v<_Ty, _Args>> construct_at(_Ty *__ptr,
																																											 _Args &&...__args)
	{
		new (__ptr) _Ty(std::forward<_Args>(__args)...);
	}

	/**
	 * @brief Calls the destructor of the object @a Ty explicitly on the memory sequence pointed by
	 * 				@a __ptr
	 * 				This function isn't defined when the constructor of @a _Ty does not exist or is not
	 * 				visible in the context of this call
	 *
	 * @tparam _Ty  The class from which the destructor is called
	 *
	 * @param __ptr Pointer to a memory sequence
	 */
	template <typename _Ty>
	pf_decl_constexpr std::enable_if_t<std::is_destructible_v<_Ty>> deconstruct_at(_Ty *__ptr)
	{
		__ptr->~_Ty();
	}

} // Pulsar

#endif // !PULSAR_MEMORY_UTILS_HPP