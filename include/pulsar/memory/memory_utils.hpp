/**
 * @file    memory_utils.hpp
 * @author  Fluffy (noe.louis-quentin@hotmail.fr)
 * @brief
 * @date    15-12-2021
 *
 * @copyright Copyright (c) 2021 - Pulsar Software
 *
 */

#ifndef PULSAR_MEMORY_UTILS_HPP
#define PULSAR_MEMORY_UTILS_HPP 1

// Include: Pulsar
#include "pulsar/pulsar.hpp"

// Include: C++
#include <utility>

// namespace pulsar
namespace pulsar
{
	// namespace memory
	namespace memory
	{

		/**
		 * @brief
		 *
		 * @param addr
		 * @param align
		 *
		 * @return pf_hint_nodiscard
		 */
		pf_hint_nodiscard pf_decl_constexpr ptrsize_t padding_of(ptrsize_t addr,
																														 ptrsize_t align) pf_attr_noexcept
		{
			if (align <= 1) return addr;
			return (align - (addr & (align - 1))) & (align - 1);
		}

		/**
		 * @brief
		 *
		 * @param ptr
		 * @param align
		 * @param offset
		 *
		 * @return pf_hint_nodiscard*
		 */
		pf_hint_nodiscard pf_decl_constexpr void *align_top(void *ptr,
																												ptrsize_t align,
																												ptrsize_t offset = 0)
		{
			union
			{
				void *as_voidptr;
				ptrsize_t as_addr;
			};

			as_voidptr = ptr;
			as_addr += padding_of(as_addr + offset, align);
			return as_voidptr;
		}

		/**
		 * @brief
		 *
		 * @tparam _Ty
		 * @tparam _Args
		 *
		 * @param ptr
		 * @param args
		 *
		 * @return pf_decl_constexpr
		 */
		template <typename _Ty, typename... _Args>
		pf_decl_constexpr void construct_at(_Ty *ptr, _Args &&...args)
		{
			new (ptr) _Ty(std::forward<_Args>(args)...);
		}

		/**
		 * @brief
		 *
		 * @tparam _Ty
		 *
		 * @param ptr
		 *
		 * @return pf_decl_constexpr
		 */
		template <typename _Ty>
		pf_decl_constexpr void deconstruct_at(_Ty *ptr)
		{
			ptr->~_Ty();
		}

	} // namespace memory

} // namespace pulsar

#endif // !PULSAR_MEMORY_UTILS_HPP