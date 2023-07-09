/*! @file   malloc.hpp
 *  @author Louis-Quentin Noé (noe.louis-quentin@hotmail.fr)
 *  @brief
 *  @date   05-03-2023
 *
 *  @copyright Copyright (c) 2023 - Pulsar Software
 *
 *  @since 0.1.4
 */

#ifndef PULSAR_MALLOC_HPP
#define PULSAR_MALLOC_HPP 1

// Include: Pulsar
#include "pulsar/pulsar.hpp"
#include "pulsar/utility.hpp"

// Include: MiMalloc
#include <mimalloc.h>

// Include: C
#include <cstring>	// memcpy, memmove...

// Pulsar
namespace pul
{
	/// MALLOC: Constants
	pf_decl_constexpr align_val_t ALIGN_NO_ALIGN = align_val_t(1);
	pf_decl_constexpr align_val_t ALIGN_DEFAULT	 = align_val_t(alignof(void *));
	pf_decl_constexpr align_val_t ALIGN_MAX			 = align_val_t(1'024);

	/// MALLOC: Array
	template<typename _Ty>
	struct __marray
	{
		size_t count;
		_Ty data[];
	};
	using __marray_t = __marray<byte_t>;

	/// MALLOC: Constant Evaluated
	pf_hint_nodiscard pf_decl_inline pf_decl_constexpr __marray_t *
	__mem_get_marray(
	 void *__ptr) pf_attr_noexcept
	{
		union
		{
			void *as_void;
			byte_t *as_byte;
			__marray_t *as_array;
		};
		as_void	 = __ptr;
		as_byte -= sizeof(size_t);
		return as_array;
	}
	template<typename _Ty>
	pf_hint_nodiscard pf_decl_inline pf_decl_constexpr __marray<_Ty> *
	__mem_get_marray_typeless(
	 void *__ptr) pf_attr_noexcept
	{
		union
		{
			void *as_void;
			byte_t *as_byte;
			__marray<_Ty> *as_array;
		};
		as_void	 = __ptr;
		as_byte -= sizeof(size_t);
		return as_array;
	}
	pf_hint_nodiscard pf_decl_inline pf_decl_constexpr void *
	cevalloc(
	 size_t __size,
	 align_val_t __align = ALIGN_DEFAULT,
	 size_t __offset		 = 0) pf_attr_noexcept
	{
		ignore		= __align;
		ignore		= __offset;
		auto *ma	= union_cast<__marray_t *>(new byte_t[sizeof(__marray_t) + __size]);
		ma->count = __size;
		memset(&ma->data[0], 0, ma->count);
		return &ma->data[0];
	}
	pf_decl_inline pf_decl_constexpr void
	cevfree(
	 void *__ptr)
	{
		delete[] union_cast<byte_t *>(__mem_get_marray(__ptr));
	}
	pf_hint_nodiscard pf_decl_inline pf_decl_constexpr void *
	cevrealloc(
	 void *__ptr,
	 size_t __size,
	 align_val_t __align = ALIGN_DEFAULT,
	 size_t __offset		 = 0) pf_attr_noexcept
	{
		ignore		= __align;
		ignore		= __offset;
		auto *ma	= union_cast<__marray_t *>(new byte_t[sizeof(__marray_t) + __size]);
		ma->count = __size;
		if(pf_unlikely(!__ptr)) return &ma->data[0];
		auto *oa = __mem_get_marray(__ptr);
		memcpy(&ma->data[0], &oa->data[0], std::min(ma->count, oa->count));
		if(oa->count < ma->count) memset(&ma->data[0] + oa->count, 0, ma->count - oa->count);
		return &oa->data[0];
	}
	pf_hint_nodiscard pf_decl_inline pf_decl_constexpr size_t
	cevsize(
	 void *__ptr)
	{
		return __mem_get_marray(__ptr)->count;
	}
	pf_hint_nodiscard pf_decl_inline pf_decl_constexpr size_t
	cevgoodsize(
	 size_t __size)
	{
		return __size;
	}

	/// MALLOC: Heap
	pf_hint_nodiscard pulsar_api void *
	__halloc(
	 size_t __size,
	 align_val_t __align = ALIGN_DEFAULT,
	 size_t __offset		 = 0);
	pf_hint_nodiscard pulsar_api void *
	__hrealloc(
	 void *__ptr,
	 size_t __size,
	 align_val_t __align = ALIGN_DEFAULT,
	 size_t __offset		 = 0);
	pulsar_api void
	__hfree(
	 void *__ptr) pf_attr_noexcept;
	pf_hint_nodiscard pf_decl_inline pf_decl_constexpr void *
	halloc(
	 size_t __size,
	 align_val_t __align = ALIGN_DEFAULT,
	 size_t __offset		 = 0)
	{
		if(std::is_constant_evaluated())
		{
			return cevalloc(__size, __align, __offset);
		}
		else
		{
			return __halloc(__size, __align, __offset);
		}
	}
	pf_decl_inline pf_decl_constexpr void
	hfree(
	 void *__ptr) pf_attr_noexcept
	{
		if(std::is_constant_evaluated())
		{
			delete[] union_cast<byte_t *>(__mem_get_marray(__ptr));
		}
		else
		{
			mi_free(__ptr);
		}
	}
	pf_decl_inline pf_decl_constexpr void *
	hrealloc(
	 void *__ptr,
	 size_t __nsize,
	 align_val_t __nalign = ALIGN_DEFAULT,
	 size_t __noffset			= 0)
	{
		if(std::is_constant_evaluated())
		{
			return cevrealloc(__ptr, __nsize, __nalign, __noffset);
		}
		else
		{
			return __hrealloc(__ptr, __nsize, __nalign, __noffset);
		}
	}
	pf_decl_inline pf_decl_constexpr size_t
	hsize(
	 void *__ptr) pf_attr_noexcept
	{
		if(std::is_constant_evaluated())
		{
			return cevsize(__ptr);
		}
		else
		{
			return mi_usable_size(__ptr);
		}
	}
	pf_decl_inline pf_decl_constexpr size_t
	hgoodsize(
	 size_t __size)
	{
		if(std::is_constant_evaluated())
		{
			return cevgoodsize(__size);
		}
		else
		{
			return mi_good_size(__size);
		}
	}

	/// MALLOC: Cache
	pf_hint_nodiscard pulsar_api void *
	__calloc(
	 size_t __size,
	 align_val_t __align = ALIGN_DEFAULT,
	 size_t __offset		 = 0);
	pf_hint_nodiscard pulsar_api void *
	__crealloc(
	 void *__ptr,
	 size_t __size,
	 align_val_t __align = ALIGN_DEFAULT,
	 size_t __offset		 = 0);
	pulsar_api void
	__cfree(
	 void *__ptr) pf_attr_noexcept;

	pf_hint_nodiscard pf_decl_inline pf_decl_constexpr void *
	calloc(
	 size_t __size,
	 align_val_t __align = ALIGN_DEFAULT,
	 size_t __offset		 = 0)
	{
		if(std::is_constant_evaluated())
		{
			return cevalloc(__size, __align, __offset);
		}
		else
		{
			return __calloc(__size, __align, __offset);
		}
	}
	pf_hint_nodiscard pf_decl_inline pf_decl_constexpr void *
	crealloc(
	 void *__ptr,
	 size_t __size,
	 align_val_t __align = ALIGN_DEFAULT,
	 size_t __offset		 = 0)
	{
		if(std::is_constant_evaluated())
		{
			return cevrealloc(__ptr, __size, __align, __offset);
		}
		else
		{
			return __crealloc(__ptr, __size, __align, __offset);
		}
	}
	pf_decl_inline pf_decl_constexpr void
	cfree(
	 void *__ptr)
	{
		if(std::is_constant_evaluated())
		{
			cevfree(__ptr);
		}
		else
		{
			__cfree(__ptr);
		}
	}


	/// MALLOC: Stack
	pf_hint_nodiscard pulsar_api void *
	__salloc(
	 size_t __size,
	 align_val_t __align = ALIGN_DEFAULT,
	 size_t __offset		 = 0);
	pf_hint_nodiscard pulsar_api void *
	__srealloc(
	 void *__ptr,
	 size_t __size,
	 align_val_t __align = ALIGN_DEFAULT,
	 size_t __offset		 = 0);
	pulsar_api void
	__sfree(
	 void *__ptr) pf_attr_noexcept;

	pf_hint_nodiscard pf_decl_inline pf_decl_constexpr void *
	salloc(
	 size_t __size,
	 align_val_t __align = ALIGN_DEFAULT,
	 size_t __offset		 = 0)
	{
		if(std::is_constant_evaluated())
			return cevalloc(__size, __align, __offset);
		else
			return __salloc(__size, __align, __offset);
	}
	pf_hint_nodiscard pf_decl_inline pf_decl_constexpr void *
	srealloc(
	 void *__ptr,
	 size_t __size,
	 align_val_t __align = ALIGN_DEFAULT,
	 size_t __offset		 = 0)
	{
		if(std::is_constant_evaluated())
			return cevrealloc(__ptr, __size, __align, __offset);
		else
			return __srealloc(__ptr, __size, __align, __offset);
	}
	pf_decl_inline pf_decl_constexpr void
	sfree(
	 void *__ptr)
	{
		if(std::is_constant_evaluated())
			cevfree(__ptr);
		else
			__sfree(__ptr);
	}
}	 // namespace pul

#endif	// !PULSAR_MALLOC_HPP
