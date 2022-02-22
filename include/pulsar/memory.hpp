/*! @file    memory.hpp
 *  @author  Fluffy (noe.louis-quentin@hotmail.fr)
 *  @brief
 *  @date    09-01-2022
 *
 *  @copyright Copyright (c) 2022 - Pulsar Software
 *
 *  @since 0.1.0
 */

#ifndef PULSAR_MEMORY_HPP
#define PULSAR_MEMORY_HPP 1

// Include: Pulsar
#include "pulsar/pulsar.hpp"

// Pulsar
namespace pul
{
	// Memory
	namespace memory
	{

		/// Alignment
		pf_decl_constexpr size_t max_align = alignof(void *);

		/// Utils
		/*! @brief Calculates the offset of a memory sequence starting at address @a __addr needed to
		 * 				 align the sequence to the value of @a __align.
		 *
		 *  @param __addr	 Memory address of the beginning of the sequence.
		 *  @param __align Desired alignment of the memory.
		 *
		 *  @return 0 if @a __align is equal to 0.
		 * 				  The offset required to align the memory sequence otherwise.
		 */
		pf_hint_nodiscard pf_decl_constexpr size_t padding_of(
				size_t __addr,
				size_t __align) pf_attr_noexcept
		{
			if (__align == 0)
				return 0;
			return (__align - (__addr & (__align - 1))) & (__align - 1);
		}

		/*! @brief Computes from the base @a __ptr a new pointer aligned to the value of @a __align by
		 *  			 an	offset of @a __offset.
		 *
		 *  @param __ptr		Pointer to a memory sequence.
		 *  @param __align	Desired alignment of the memory.
		 *  @param __offset Head offset of the new pointer (optional).
		 *
		 *  @return Aligned pointer.
		 */
		pf_hint_nodiscard pf_decl_constexpr void *align_top(
				void *__ptr,
				size_t __align,
				size_t __offset = 0) pf_attr_noexcept
		{
			union
			{
				void *as_voidptr;
				size_t as_addr;
			};

			as_voidptr = __ptr;
			as_addr += padding_of(as_addr + __offset, __align);
			return as_voidptr;
		}

		/*! @brief Checks if a block of memory pointed to by @a __ptr whose head is aligned by
		 *				 @a __align by an @a __offset.
		 *
		 *  @param __ptr		Pointer to a memory sequence.
		 *  @param __align	Alignment of this memory sequence.
		 *  @param __offset Offset of the alignment.
		 *  @return True if the block is aligned.
		 * 					False otherwise.
		 */
		pf_hint_nodiscard pf_decl_constexpr bool is_aligned(
				const void *__ptr,
				size_t __align,
				size_t __offset = 0) pf_attr_noexcept
		{
			union
			{
				const void *as_void;
				size_t as_addr;
			};

			as_void = __ptr;

			return padding_of(as_addr + __offset, __align) == 0;
		}

		/*! @brief Calls the constructor of the @a _Ty object parameterized by @a _Args explicitly on a
		 * 			 	 memory sequence starting with the @a __ptr pointer of @a __args parameters.
		 * 				 This function isn't defined when the constructor of @a _Ty does not exist or is not
		 * 				 visible in the context of this call.
		 *
		 *  @tparam _Ty   A class with a constructor parameterized by the argument(s) of type(s)
		 * 								@a _Args.
		 *  @tparam _Args The arguments of a @a _Ty constructor.
		 *
		 *  @param __ptr	Pointer to a memory sequence.
		 *  @param __args Parameters sent to a constructor of @a _Ty.
		 */
		template <typename _Ty,
							typename... _Args>
			requires(std::is_constructible_v<_Ty, _Args...>)
		pf_decl_constexpr void construct_at(
				_Ty *__ptr,
				_Args &&...__args)
		{
			new (__ptr) _Ty(std::forward<_Args>(__args)...);
		}

		/*! @brief Calls the destructor of the object @a Ty explicitly on the memory sequence pointed
		 *				 by @a __ptr.
		 * 				 This function isn't defined when the constructor of @a _Ty does not exist or is not
		 * 				 visible in the context of this call.
		 *
		 *  @tparam _Ty  The class from which the destructor is called.
		 *
		 *  @param __ptr Pointer to a memory sequence.
		 */
		template <typename _Ty>
			requires(std::is_destructible_v<_Ty>)
		pf_decl_constexpr void destruct_at(_Ty *__ptr)
		{
			__ptr->~_Ty();
		}


		// TODO: Allocators (linear, stack, pool, slab, freelist, general)
	}
}

// TODO: Operators new/delete with allocators

#endif // !PULSAR_MEMORY_HPP