/*! @file   memory_utils.hpp
 *  @author Fluffy (noe.louis-quentin@hotmail.fr)
 *  @brief  Definition of memory utils functions.
 *  @date 12-08-2022
 *
 *  @copyright Copyright (c) 2022 - Pulsar Software
 *
 */

#ifndef PULSAR_MEMORY_UTILS_HPP
#define PULSAR_MEMORY_UTILS_HPP 1

// Include: Pulsar
#include "pulsar/memory.hpp"

// Pulsar
namespace pul
{
	// Memory
	namespace memory
	{

		/// MEMORY: Alignment
		pf_decl_constexpr align_val_t max_align = align_val_t(alignof(void *));

		/*! @brief Calculates the offset of a memory sequence starting at address @a __addr needed to
		 * 				 align the sequence to the value of @a __align.
		 *
		 *  @param[in] __addr	 Memory address of the beginning of the sequence.
		 *  @param[in] __align Desired alignment of the memory.
		 *
		 *  @return 0 if @a __align is equal to 0.
		 * 				  The offset required to align the memory sequence otherwise.
		 */
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr size_t paddingof(
				size_t __addr,
				align_val_t __align) pf_attr_noexcept
		{
			if (__align == align_val_t(0))
				return 0;
			return (static_cast<size_t>(__align) - (__addr & (static_cast<size_t>(__align) - 1))) & (static_cast<size_t>(__align) - 1);
		}
		/*! @brief Convert a pointer to a memory address.
		 *
		 *  @param[in] __ptr Pointer to cast.
		 *  @return Memory address of @a __ptr.
		 */
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr size_t addressof(
				const void *__ptr) pf_attr_noexcept
		{
			return reinterpret_cast<size_t>(__ptr);
		}
		/*! @brief Convert a memory address to a type _Ty pointer.
		 *
		 *  @tparam _Ty Type of pointer.
		 *
		 *  @param[in] __addr Memory address.
		 * 	@return _Ty pointer pointing to @a __addr.
		 */
		template <typename _Ty>
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr _Ty *addptr(
				size_t __addr) pf_attr_noexcept
		{
			return reinterpret_cast<_Ty *>(__addr);
		}
		/*! @brief Convert a memory address to a type const _Ty pointer.
		 *
		 *  @tparam _Ty Type of pointer.
		 *
		 *  @param[in] __addr Memory address.
		 *  @return const _Ty pointer pointing to @a __addr.
		 */
		template <typename _Ty>
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const _Ty *addcptr(
				size_t __addr) pf_attr_noexcept
		{
			return reinterpret_cast<const _Ty *>(__addr);
		}
		/*! @brief Computes from the base @a __ptr a new pointer aligned to the value of @a __align by
		 *  			 an	offset of @a __offset.
		 *
		 *  @param[in] __ptr		Pointer to a memory sequence.
		 *  @param[in] __align	Desired alignment of the memory.
		 *  @param[in] __offset Head offset of the new pointer (optional).
		 *
		 *  @return Aligned pointer.
		 */
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr void *align_top(
				void *__ptr,
				align_val_t __align = max_align,
				size_t __offset			= 0) pf_attr_noexcept
		{
			union
			{
				void *as_voidptr;
				size_t as_addr;
			};

			as_voidptr = __ptr;
			as_addr += paddingof(as_addr + __offset, __align);
			return as_voidptr;
		}
		/*! @brief Checks if a block of memory pointed to by @a __ptr whose head is aligned by
		 *				 @a __align by an @a __offset.
		 *
		 *  @param[in] __ptr		Pointer to a memory sequence.
		 *  @param[in] __align	Alignment of this memory sequence.
		 *  @param[in] __offset Offset of the alignment.
		 *  @return True if the block is aligned.
		 * 					False otherwise.
		 */
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr bool is_aligned(
				const void *__ptr,
				align_val_t __align,
				size_t __offset = 0) pf_attr_noexcept
		{
			union
			{
				const void *as_void;
				size_t as_addr;
			};

			as_void = __ptr;

			return paddingof(as_addr + __offset, __align) == 0;
		}

		/*!
		 *  @brief Converts a type to another using union.
		 *
		 *  @tparam _ToTy Output variable type.
		 *  @tparam _InTy Input variable type.
		 *  @param[in] __in Variable to convert.
		 *  @return _ToTy casted variable of @a __in.
		 */
		/*
		template <typename _ToTy, typename _InTy>
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr _ToTy union_cast(
				_InTy &&__in)
		{
			union
			{
				_ToTy &&to;
				_InTy &&in;
			};
			in = __in;
			return to;
		}
		 */
	}
}

#endif // !PULSAR_MEMORY_UTILS_HPP 1