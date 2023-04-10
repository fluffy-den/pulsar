/*! @file   memory.hpp
 *  @author Louis-Quentin Noé (noe.louis-quentin@hotmail.fr)
 *  @brief
 *  @date   19-02-2023
 *
 *  @copyright Copyright (c) 2023 - Pulsar Software
 *
 *  @since 0.1.4
 */

#ifndef PULSAR_MEMORY_HPP
#define PULSAR_MEMORY_HPP 1

// Include: C
#include <cstring>	// memcpy

// Include: C++
#include <concepts>

// Include: Pulsar
#include "pulsar/pulsar.hpp"
#include "pulsar/malloc.hpp"
#include "pulsar/iterator.hpp"
#include "pulsar/utility.hpp"
#include "pulsar/debug.hpp"


// Pulsar
namespace pul
{
	/// MEMORY: Allocator -> Concept
	template<typename _Allocator>
	concept __allocator_c =
	 requires(	// clang-format off
	 _Allocator &__all) 
	{
		{ __all.allocate(std::declval<size_t>(), std::declval<align_val_t>(), std::declval<size_t>()) } -> std::convertible_to<void *>;
		{ __all.reallocate(std::declval<void *>(), std::declval<size_t>(), std::declval<align_val_t>(), std::declval<size_t>()) } -> std::convertible_to<void *>;
		__all.deallocate(std::declval<void *>());
	};	 // clang-format on
	template<typename _Allocator>
	struct is_allocator : std::false_type
	{};
	template<__allocator_c _Allocator>
	struct is_allocator<_Allocator> : std::true_type
	{};
	template<typename _Allocator>
	pf_decl_static pf_decl_constexpr bool is_allocator_v = is_allocator<_Allocator>::value;

	/// MEMORY: Allocator -> Allocate
	template<typename _Allocator>
	pf_hint_nodiscard pf_decl_inline pf_decl_constexpr void *
	allocate(
	 _Allocator &__all,
	 size_t __size,
	 align_val_t __align = ALIGN_DEFAULT,
	 size_t __offset		 = 0) pf_attr_noexcept
		requires(is_allocator_v<_Allocator>)
	{
		if(std::is_constant_evaluated())
		{
			return cevalloc(__size, __align, __offset);
		}
		else
		{
			return __all.allocate(__size, union_cast<size_t>(__align), __offset);
		}
	}

	/// MEMORY: Allocator -> Reallocate
	template<typename _Allocator>
	pf_decl_inline pf_decl_constexpr void *
	reallocate(
	 _Allocator &__all,
	 void *__ptr,
	 size_t __nsize,
	 align_val_t __align = ALIGN_DEFAULT,
	 size_t __offset		 = 0) pf_attr_noexcept
	{
		if(std::is_constant_evaluated())
		{
			return cevrealloc(__ptr, __nsize, __align, __offset);
		}
		else
		{
			return __all.reallocate(__ptr, __nsize, __align, __offset);
		}
	}

	/// MEMORY: Allocator -> Deallocate
	template<typename _Allocator>
	pf_decl_inline pf_decl_constexpr void
	deallocate(
	 _Allocator &__all,
	 void *__ptr) pf_attr_noexcept
	{
		if(std::is_constant_evaluated())
		{
			return cevfree(__ptr);
		}
		else
		{
			__all.deallocate(__ptr);
		}
	}

	/// MEMORY: Utility
	pf_hint_nodiscard pf_decl_inline pf_decl_constexpr bool
	is_power_of_two(
	 size_t __val) pf_attr_noexcept
	{
		return (__val & (__val - 1)) == 0;
	}
	pf_hint_nodiscard pf_decl_inline pf_decl_constexpr size_t
	paddingof(
	 size_t __addr,
	 align_val_t __align) pf_attr_noexcept
	{
		const size_t a = union_cast<size_t>(__align);
		return (a - (__addr & (a - 1))) & (a - 1);
	}
	pf_hint_nodiscard pf_decl_inline pf_decl_constexpr void *
	align_top(
	 void *__ptr,
	 align_val_t __align,
	 size_t __offset = 0) pf_attr_noexcept
	{
		byte_t *p = union_cast<byte_t *>(__ptr);
		p				 += paddingof(addressof(p + __offset), __align);
		return p;
	}
	pf_hint_nodiscard pf_decl_inline pf_decl_constexpr bool
	is_aligned(
	 const void *__ptr,
	 align_val_t __align,
	 size_t __offset = 0)
	{
		return paddingof(addressof(__ptr) + __offset, __align) == 0;
	}

	/// MEMORY: Utility -> Construct
	template<
	 typename _Ty,
	 typename... _Args>
	pf_decl_inline pf_decl_constexpr void
	construct(
	 _Ty *__ptr,
	 _Args &&...__args)
		requires(std::is_constructible_v<_Ty, _Args...>)
	{
		pf_assert(__ptr, "__ptr is nullptr!");
		new(__ptr) _Ty(std::forward<_Args>(__args)...);
	}
	template<
	 typename _IteratorIn,
	 typename... _Args>
	pf_decl_inline pf_decl_constexpr void
	construct(
	 _IteratorIn __it,
	 _Args &&...__args)
		requires(is_iterator_v<_IteratorIn> && std::is_constructible_v<typename _IteratorIn::value_t, _Args...>)
	{
		return construct(__it.get(), std::forward<_Args>(__args)...);
	}
	template<
	 typename _IteratorIn>
	pf_decl_inline pf_decl_constexpr void
	construct(
	 _IteratorIn __beg,
	 _IteratorIn __end)
		requires(is_iterator_v<_IteratorIn> && std::is_default_constructible_v<typename _IteratorIn::value_t>)
	{
		for(; __beg != __end; ++__beg) construct(__beg.get());
	}
	template<
	 typename _IteratorIn>
	pf_decl_inline pf_decl_constexpr void
	construct(
	 _IteratorIn __beg,
	 _IteratorIn __end,
	 typename _IteratorIn::value_t const &__val)
		requires(is_iterator_v<_IteratorIn> && std::is_copy_constructible_v<typename _IteratorIn::value_t>)
	{
		for(; __beg != __end; ++__beg) construct(__beg.get(), __val);
	}
	template<
	 typename _IteratorOut,
	 typename _IteratorIn>
	pf_decl_inline pf_decl_constexpr void
	construct(
	 _IteratorOut __ab,
	 _IteratorOut __ae,
	 _IteratorIn __bb)
		requires(is_iterator_v<_IteratorOut> && is_iterator_v<_IteratorIn>)
	{
		for(; __ab != __ae; ++__ab, ++__bb) construct(__ab.get(), *__bb);
	}
	template<typename _Ty>
	pf_decl_inline pf_decl_constexpr void
	construct(
	 __marray<_Ty> *__arr,
	 size_t __count)
		requires(std::is_default_constructible_v<_Ty>)
	{
		__arr->count = __count;
		construct(iterator(&__arr->data[0]), iterator(&__arr->data[0] + __count));
	}
	template<
	 typename _Ty,
	 typename _IteratorIn>
	pf_decl_inline pf_decl_constexpr void
	construct(
	 __marray<_Ty> *__arr,
	 _IteratorIn __beg,
	 _IteratorIn __end)
		requires(
		 is_iterator_v<_IteratorIn>
		 && std::is_convertible_v<typename _IteratorIn::value_t, _Ty>)
	{
		const size_t c = countof(__beg, __end);
		__arr->count	 = c;
		construct(iterator(&__arr->data[0]), iterator(&__arr->data[0] + c), __beg);
	}
	template<typename _Ty>
	pf_decl_inline pf_decl_constexpr void
	construct(
	 __marray<_Ty> *__arr,
	 initializer_list<_Ty> __il)
		requires(std::is_copy_constructible_v<_Ty>)
	{
		const size_t c = __il.size();
		__arr->count	 = c;
		construct(iterator(&__arr->data[0]), iterator(&__arr->data[0] + c), iterator(__il.begin()));
	}

	/// MEMORY: Utility -> Destroy
	template<typename _Ty>
	pf_decl_inline pf_decl_constexpr void
	destroy(
	 _Ty *__ptr) pf_attr_noexcept
	{
		__ptr->~_Ty();
	}
	template<
	 typename _IteratorIn>
	pf_decl_inline pf_decl_constexpr void
	destroy(
	 _IteratorIn __it) pf_attr_noexcept
		requires(is_iterator_v<_IteratorIn>)
	{
		destroy(__it.get());
	}
	template<
	 typename _IteratorIn>
	pf_decl_inline pf_decl_constexpr void
	destroy(
	 _IteratorIn __beg,
	 _IteratorIn __end) pf_attr_noexcept
		requires(is_iterator_v<_IteratorIn>)
	{
		for(; __beg != __end; ++__beg) destroy(__beg.get());
	}
	template<
	 typename _Ty>
	pf_decl_inline pf_decl_constexpr void
	destroy(
	 __marray<_Ty> *__ptr) pf_attr_noexcept
	{
		destroy(iterator(&__ptr->data[0]), iterator(&__ptr->data[0] + __ptr->count));
	}

	/// MEMORY: Utility -> Assign
	template<
	 typename _Ty,
	 typename _Uy>
	pf_decl_inline pf_decl_constexpr void
	assign(
	 _Ty *__ptr,
	 _Uy const &__val)
		requires(std::is_assignable_v<_Ty, _Uy const &>)
	{
		*__ptr = __val;
	}
	template<
	 typename _Ty,
	 typename _Uy>
	pf_decl_inline pf_decl_constexpr void
	assign(
	 _Ty *__ptr,
	 _Uy const &__val)
		requires(!std::is_assignable_v<_Ty, _Uy const &> && std::is_constructible_v<_Ty, _Uy const &>)
	{
		destroy(__ptr);
		construct(__ptr, __val);
	}
	template<
	 typename _IteratorIn,
	 typename _Uy>
	pf_decl_inline pf_decl_constexpr void
	assign(
	 _IteratorIn __it,
	 _Uy const &__val)
		requires(is_iterator_v<_IteratorIn>)
	{
		return assign(__it.get(), __val);
	}
	template<
	 typename _Ty,
	 typename _Uy>
	pf_decl_inline pf_decl_constexpr void
	assign(
	 _Ty *__ptr,
	 _Uy &&__val)
		requires(std::is_assignable_v<_Ty, _Uy &&>)
	{
		*__ptr = std::move(__val);
	}
	template<
	 typename _Ty,
	 typename _Uy>
	pf_decl_inline pf_decl_constexpr void
	assign(
	 _Ty *__ptr,
	 _Uy &&__val)
		requires(!std::is_assignable_v<_Ty, _Uy &&> && std::is_constructible_v<_Ty, _Uy &&>)
	{
		destroy(__ptr);
		construct(__ptr, std::move(__val));
	}
	template<
	 typename _IteratorIn,
	 typename _Uy>
	pf_decl_inline pf_decl_constexpr void
	assign(
	 _IteratorIn __it,
	 _Uy &&__val)
		requires(is_iterator_v<_IteratorIn>)
	{
		return assign(__it.get(), std::move(__val));
	}
	template<
	 typename _IteratorOut,
	 typename _IteratorIn>
	pf_decl_inline pf_decl_constexpr void
	assign(
	 _IteratorOut __beg,
	 _IteratorOut __end,
	 _IteratorIn __in)
		requires(is_iterator_v<_IteratorOut> && is_iterator_v<_IteratorIn>)
	{
		for(; __beg != __end; ++__beg, ++__in)
		{
			assign(__beg, *__in);
		}
	}
	template<
	 typename _IteratorOut,
	 typename _Uy>
	pf_decl_inline pf_decl_constexpr void
	assign(
	 _IteratorOut __beg,
	 _IteratorOut __end,
	 _Uy const &__val)
		requires(is_iterator_v<_IteratorOut> && !is_iterator_v<_Uy>)
	{
		for(; __beg != __end; ++__beg)
		{
			assign(__beg, __val);
		}
	}
	// NOTE: May have to be updated later

	/// MEMORY: Cache -> New Construct
	template<
	 typename _Ty,
	 typename... _Args>
	pf_hint_nodiscard _Ty *
	cnew_construct_ex(
	 size_t __exBytes,
	 _Args &&...__args)
	{
		_Ty *ptr = union_cast<_Ty *>(calloc(sizeof(_Ty) + __exBytes));
		new(ptr) _Ty(std::forward<_Args>(__args)...);
		return ptr;
	}
	template<
	 typename _Ty,
	 typename... _Args>
	pf_hint_nodiscard _Ty *
	cnew_construct(
	 _Args &&...__args)
	{
		return cnew_construct_ex<_Ty>(0, std::forward<_Args>(__args)...);
	}
	// TODO: Thread Pool Cache + Logger Message Cache
	//		OR Global Cache ? => 1. May have less cache misses?

	template<
	 typename _Ty>
	void
	cdestroy_delete(
	 _Ty *__p) pf_attr_noexcept
	{
		__p->~_Ty();
		cfree(__p);
	}

	/// MEMORY: New Construct
	template<typename _Ty, typename... _Args>
	pf_decl_inline pf_decl_constexpr _Ty *
	new_construct_aligned_at_ex(
	 size_t __exBytes,
	 align_val_t __align,
	 size_t __offset,
	 _Args &&...__args)
		requires(
		 !std::is_array_v<_Ty>
		 && std::is_constructible_v<_Ty, _Args...>)
	{
		_Ty *p = union_cast<_Ty *>(
		 halloc(sizeof(_Ty) + __exBytes, __align, __offset));
		if(pf_unlikely(!p)) pf_throw(
		 dbg_category_generic(), dbg_code::bad_alloc, dbg_flags::dump_with_handle_data, "Failed to create new object of type={}, size={}, align={}, offset={} with heap memory.", typeid(_Ty).name(), sizeof(_Ty) + __exBytes, union_cast<size_t>(__align), __offset);
		construct(p, std::forward<_Args>(__args)...);
		return p;
	}
	template<typename _Ty, typename... _Args>
	pf_decl_inline pf_decl_constexpr _Ty *
	new_construct_aligned_ex(
	 size_t __exBytes,
	 align_val_t __align,
	 _Args &&...__args)
		requires(
		 !std::is_array_v<_Ty>
		 && std::is_constructible_v<_Ty, _Args...>)
	{
		return new_construct_aligned_at_ex<_Ty>(
		 __exBytes,
		 __align,
		 0,
		 std::forward<_Args>(__args)...);
	}
	template<typename _Ty, typename... _Args>
	pf_decl_inline pf_decl_constexpr _Ty *
	new_construct_ex(
	 size_t __exBytes,
	 _Args &&...__args)
		requires(
		 !std::is_array_v<_Ty>
		 && std::is_constructible_v<_Ty, _Args...>)
	{
		return new_construct_aligned_at_ex<_Ty>(
		 __exBytes,
		 align_val_t(alignof(_Ty)),
		 0,
		 std::forward<_Args>(__args)...);
	}
	template<typename _Ty, typename _Allocator, typename... _Args>
	pf_decl_inline pf_decl_constexpr _Ty *
	new_construct_aligned_at_ex(
	 _Allocator &__all,
	 align_val_t __align,
	 size_t __offset,
	 size_t __exBytes,
	 _Args &&...__args)
		requires(
		 !std::is_array_v<_Ty>
		 && is_allocator_v<_Allocator>
		 && std::is_constructible_v<_Ty, _Args...>)
	{
		_Ty *p = union_cast<_Ty *>(
		 allocate<_Ty>(__all, sizeof(_Ty) + __exBytes, __align, __offset));
		if(pf_unlikely(!p)) pf_throw(
		 dbg_category_generic(), dbg_code::bad_alloc, dbg_flags::dump_with_handle_data, "Failed to create new object of type={}, size={}, align={}, offset={} with allocator={}.", typeid(_Ty).name(), sizeof(_Ty) + __exBytes, union_cast<size_t>(__align), __offset, union_cast<void *>(&__all));
		construct(p, std::forward<_Args>(__args)...);
		return p;
	}
	template<typename _Ty, typename _Allocator, typename... _Args>
	pf_decl_inline pf_decl_constexpr _Ty *
	new_construct_aligned_ex(
	 _Allocator &__all,
	 align_val_t __align,
	 size_t __exBytes,
	 _Args &&...__args)
		requires(
		 !std::is_array_v<_Ty>
		 && is_allocator_v<_Allocator>
		 && std::is_constructible_v<_Ty, _Args...>)
	{
		return new_construct_aligned_at_ex<_Ty>(
		 __all,
		 0,
		 __align,
		 0,
		 std::forward<_Args>(__args)...);
	}
	template<typename _Ty, typename _Allocator, typename... _Args>
	pf_decl_inline pf_decl_constexpr _Ty *
	new_construct_ex(
	 _Allocator &__all,
	 size_t __exBytes,
	 _Args &&...__args)
		requires(
		 !std::is_array_v<_Ty>
		 && is_allocator_v<_Allocator>
		 && std::is_constructible_v<_Ty, _Args...>)
	{
		return new_construct_aligned_at_ex<_Ty>(
		 __all,
		 0,
		 align_val_t(alignof(_Ty)),
		 0,
		 std::forward<_Args>(__args)...);
	}
	template<typename _Ty, typename... _Args>
	pf_decl_inline pf_decl_constexpr _Ty *
	new_construct_aligned_at(
	 align_val_t __align,
	 size_t __offset,
	 _Args &&...__args)
		requires(
		 !std::is_array_v<_Ty>
		 && std::is_constructible_v<_Ty, _Args...>)
	{
		return new_construct_aligned_at_ex<_Ty>(0, __align, __offset, std::forward<_Args>(__args)...);
	}
	template<typename _Ty, typename... _Args>
	pf_decl_inline pf_decl_constexpr _Ty *
	new_construct_aligned(
	 align_val_t __align,
	 _Args &&...__args)
		requires(
		 !std::is_array_v<_Ty>
		 && std::is_constructible_v<_Ty, _Args...>)
	{
		return new_construct_aligned_ex<_Ty>(0, __align, std::forward<_Args>(__args)...);
	}
	template<typename _Ty, typename... _Args>
	pf_decl_inline pf_decl_constexpr _Ty *
	new_construct(
	 _Args &&...__args)
		requires(
		 !std::is_array_v<_Ty>
		 && std::is_constructible_v<_Ty, _Args...>)
	{
		return new_construct_ex<_Ty>(0, std::forward<_Args>(__args)...);
	}

	template<typename _Ty, typename _Allocator, typename... _Args>
	pf_decl_inline pf_decl_constexpr _Ty *
	new_construct_aligned_at(
	 _Allocator &__all,
	 align_val_t __align,
	 size_t __offset,
	 _Args &&...__args)
		requires(
		 !std::is_array_v<_Ty>
		 && is_allocator_v<_Allocator>
		 && std::is_constructible_v<_Ty, _Args...>)
	{
		return new_construct_aligned_at_ex<_Ty>(__all, 0, __align, __offset, std::forward<_Args>(__args)...);
	}
	template<typename _Ty, typename _Allocator, typename... _Args>
	pf_decl_inline pf_decl_constexpr _Ty *
	new_construct_aligned(
	 _Allocator &__all,
	 align_val_t __align,
	 _Args &&...__args)
		requires(
		 !std::is_array_v<_Ty>
		 && is_allocator_v<_Allocator>
		 && std::is_constructible_v<_Ty, _Args...>)
	{
		return new_construct_aligned_ex<_Ty>(__all, 0, __align, std::forward<_Args>(__args)...);
	}
	template<typename _Ty, typename _Allocator, typename... _Args>
	pf_decl_inline pf_decl_constexpr _Ty *
	new_construct(
	 _Allocator &__all,
	 _Args &&...__args)
		requires(
		 !std::is_array_v<_Ty>
		 && is_allocator_v<_Allocator>
		 && std::is_constructible_v<_Ty, _Args...>)
	{
		return new_construct_ex<_Ty>(__all, 0, std::forward<_Args>(__args)...);
	}

	/// MEMORY: Utility -> Destroy Delete
	template<typename _Ty>
	pf_decl_inline pf_decl_constexpr void
	destroy_delete(
	 _Ty *__ptr) pf_attr_noexcept
		requires(
		 !std::is_array_v<_Ty>)
	{
		destroy(__ptr);
		hfree(__ptr);
	}
	template<typename _Ty, typename _Allocator>
	pf_decl_inline pf_decl_constexpr void
	destroy_delete(
	 _Allocator &__all,
	 _Ty *__ptr) pf_attr_noexcept
		requires(
		 is_allocator_v<_Allocator>
		 && !std::is_array_v<_Ty>)
	{
		destroy(__ptr);
		deallocate(__all, __ptr);
	}


	/// MEMORY: Utility -> New Construct -> Array
	template<typename _Ty>
	pf_decl_inline pf_decl_constexpr std::remove_all_extents_t<_Ty> *
	new_construct_aligned_at(
	 size_t __count,
	 align_val_t __align,
	 size_t __offset)
		requires(
		 std::is_array_v<_Ty>
		 && std::is_default_constructible_v<std::remove_all_extents_t<_Ty>>)
	{
		union
		{
			__marray<std::remove_all_extents_t<_Ty>> *as_array;
			size_t *as_size;
		};
		as_array = union_cast<__marray<std::remove_all_extents_t<_Ty>> *>(
		 halloc(sizeof(size_t) + sizeof(std::remove_all_extents_t<_Ty>) * __count, __align, sizeof(size_t) + __offset));
		as_array->count = __count;
		construct(iterator(&as_array->data[0]), iterator(&as_array->data[0] + __count));
		return &as_array->data[0];
	}
	template<typename _Ty>
	pf_decl_inline pf_decl_constexpr std::remove_all_extents_t<_Ty> *
	new_construct_aligned(
	 size_t __count,
	 align_val_t __align)
		requires(
		 std::is_array_v<_Ty>
		 && std::is_default_constructible_v<std::remove_all_extents_t<_Ty>>)
	{
		return new_construct_aligned_at<_Ty>(__count, __align, 0);
	}
	template<typename _Ty>
	pf_decl_inline pf_decl_constexpr std::remove_all_extents_t<_Ty> *
	new_construct(
	 size_t __count)
		requires(
		 std::is_array_v<_Ty>
		 && std::is_default_constructible_v<std::remove_all_extents_t<_Ty>>)
	{
		return new_construct_aligned_at<_Ty>(__count, align_val_t(alignof(std::remove_all_extents_t<_Ty>)), 0);
	}
	template<typename _Ty, typename _Allocator>
	pf_decl_inline pf_decl_constexpr std::remove_all_extents_t<_Ty> *
	new_construct_aligned_at(
	 _Allocator &__all,
	 size_t __count,
	 align_val_t __align,
	 size_t __offset)
		requires(
		 std::is_array_v<_Ty>
		 && is_allocator_v<_Allocator>
		 && std::is_default_constructible_v<std::remove_all_extents_t<_Ty>>)
	{
		union
		{
			__marray<std::remove_all_extents_t<_Ty>> *as_array;
			size_t *as_size;
		};
		as_array = union_cast<__marray<std::remove_all_extents_t<_Ty>> *>(
		 halloc(__all, sizeof(size_t) + sizeof(std::remove_all_extents_t<_Ty>) * __count, __align, sizeof(size_t) + __offset));
		as_array->count = __count;
		construct(iterator(&as_array->data[0]), iterator(&as_array->data[0] + __count));
		return &as_array->data[0];
	}
	template<typename _Ty, typename _Allocator>
	pf_decl_inline pf_decl_constexpr std::remove_all_extents_t<_Ty> *
	new_construct_aligned(
	 _Allocator &__all,
	 size_t __count,
	 align_val_t __align)
		requires(
		 std::is_array_v<_Ty>
		 && is_allocator_v<_Allocator>
		 && std::is_default_constructible_v<std::remove_all_extents_t<_Ty>>)
	{
		return new_construct_aligned_at<_Ty>(__all, __count, __align, 0);
	}
	template<typename _Ty, typename _Allocator>
	pf_decl_inline pf_decl_constexpr std::remove_all_extents_t<_Ty> *
	new_construct(
	 _Allocator &__all,
	 size_t __count)
		requires(
		 std::is_array_v<_Ty>
		 && is_allocator_v<_Allocator>
		 && std::is_default_constructible_v<std::remove_all_extents_t<_Ty>>)
	{
		return new_construct_aligned_at<_Ty>(__all, __count, align_val_t(alignof(std::remove_all_extents_t<_Ty>)), 0);
	}

	template<typename _Ty>
	pf_decl_inline pf_decl_constexpr std::remove_all_extents_t<_Ty> *
	new_construct_aligned_at(
	 size_t __count,
	 align_val_t __align,
	 size_t __offset,
	 std::remove_all_extents_t<_Ty> const &__val)
		requires(
		 std::is_array_v<_Ty>
		 && std::is_copy_constructible_v<std::remove_all_extents_t<_Ty>>)
	{
		union
		{
			__marray<std::remove_all_extents_t<_Ty>> *as_array;
			size_t *as_size;
		};
		as_array = union_cast<__marray<std::remove_all_extents_t<_Ty>> *>(
		 halloc(sizeof(size_t) + sizeof(std::remove_all_extents_t<_Ty>) * __count, __align, sizeof(size_t) + __offset));
		as_array->count = __count;
		construct(iterator(&as_array->data[0]), iterator(&as_array->data[0] + __count), __val);
		return &as_array->data[0];
	}
	template<typename _Ty>
	pf_decl_inline pf_decl_constexpr std::remove_all_extents_t<_Ty> *
	new_construct_aligned(
	 size_t __count,
	 align_val_t __align,
	 std::remove_all_extents_t<_Ty> const &__val)
		requires(
		 std::is_array_v<_Ty>
		 && std::is_copy_constructible_v<std::remove_all_extents_t<_Ty>>)
	{
		return new_construct_aligned_at<_Ty>(__count, __align, 0, __val);
	}
	template<typename _Ty>
	pf_decl_inline pf_decl_constexpr std::remove_all_extents_t<_Ty> *
	new_construct(
	 size_t __count,
	 std::remove_all_extents_t<_Ty> const &__val)
		requires(
		 std::is_array_v<_Ty>
		 && std::is_copy_constructible_v<std::remove_all_extents_t<_Ty>>)
	{
		return new_construct_aligned_at<_Ty>(__count, align_val_t(alignof(std::remove_all_extents_t<_Ty>)), 0, __val);
	}

	template<typename _Ty, typename _Allocator>
	pf_decl_inline pf_decl_constexpr std::remove_all_extents_t<_Ty> *
	new_construct_aligned_at(
	 _Allocator &__all,
	 size_t __count,
	 align_val_t __align,
	 size_t __offset,
	 std::remove_all_extents_t<_Ty> const &__val)
		requires(
		 std::is_array_v<_Ty>
		 && is_allocator_v<_Allocator>
		 && std::is_copy_constructible_v<std::remove_all_extents_t<_Ty>>)
	{
		union
		{
			__marray<std::remove_all_extents_t<_Ty>> *as_array;
			size_t *as_size;
		};
		as_array = union_cast<__marray<std::remove_all_extents_t<_Ty>> *>(
		 halloc(__all, sizeof(std::remove_all_extents_t<_Ty>) * __count, __align, sizeof(size_t) + __offset));
		as_array->count = __count;
		construct(iterator(&as_array->data[0]), iterator(&as_array->data[0] + __count), __val);
		return &as_array->data[0];
	}
	template<typename _Ty, typename _Allocator>
	pf_decl_inline pf_decl_constexpr std::remove_all_extents_t<_Ty> *
	new_construct_aligned(
	 _Allocator &__all,
	 size_t __count,
	 align_val_t __align,
	 std::remove_all_extents_t<_Ty> const &__val)
		requires(
		 std::is_array_v<_Ty>
		 && is_allocator_v<_Allocator>
		 && std::is_copy_constructible_v<std::remove_all_extents_t<_Ty>>)
	{
		return new_construct_aligned_at<_Ty>(__all, __count, __align, 0, __val);
	}
	template<typename _Ty, typename _Allocator>
	pf_decl_inline pf_decl_constexpr std::remove_all_extents_t<_Ty> *
	new_construct(
	 _Allocator &__all,
	 size_t __count,
	 std::remove_all_extents_t<_Ty> const &__val)
		requires(
		 std::is_array_v<_Ty>
		 && is_allocator_v<_Allocator>
		 && std::is_copy_constructible_v<std::remove_all_extents_t<_Ty>>)
	{
		return new_construct_aligned_at<_Ty>(__all, __count, align_val_t(alignof(std::remove_all_extents_t<_Ty>)), 0, __val);
	}

	template<typename _IteratorIn>
	pf_decl_inline pf_decl_constexpr typename _IteratorIn::value_t *
	new_construct_aligned_at(
	 _IteratorIn __beg,
	 _IteratorIn __end,
	 align_val_t __align,
	 size_t __offset)
		requires(
		 std::is_copy_constructible_v<typename _IteratorIn::value_t>)
	{
		const size_t c = countof(__beg, __end);
		union
		{
			__marray<typename _IteratorIn::value_t> *as_array;
			size_t *as_size;
		};
		as_array = union_cast<__marray<typename _IteratorIn::value_t> *>(
		 halloc(sizeof(typename _IteratorIn::value_t) * c, __align, sizeof(size_t) + __offset));
		as_array->count = c;
		construct(iterator(&as_array->data[0]), iterator(&as_array->data[0] + c), __beg);
		return &as_array->data[0];
	}
	template<typename _IteratorIn>
	pf_decl_inline pf_decl_constexpr typename _IteratorIn::value_t *
	new_construct_aligned(
	 _IteratorIn __beg,
	 _IteratorIn __end,
	 align_val_t __align)
		requires(
		 std::is_copy_constructible_v<typename _IteratorIn::value_t>)
	{
		return new_construct_aligned_at(
		 __beg,
		 __end,
		 __align,
		 0);
	}
	template<typename _IteratorIn>
	pf_decl_inline pf_decl_constexpr typename _IteratorIn::value_t *
	new_construct(
	 _IteratorIn __beg,
	 _IteratorIn __end)
		requires(
		 std::is_copy_constructible_v<typename _IteratorIn::value_t>)
	{
		return new_construct_aligned_at(
		 __beg,
		 __end,
		 align_val_t(alignof(typename _IteratorIn::value_t)),
		 0);
	}
	template<typename _IteratorIn, typename _Allocator>
	pf_decl_inline pf_decl_constexpr typename _IteratorIn::value_t *
	new_construct_aligned_at(
	 _Allocator &__all,
	 _IteratorIn __beg,
	 _IteratorIn __end,
	 align_val_t __align,
	 size_t __offset)
		requires(
		 is_allocator_v<_Allocator>
		 && std::is_copy_constructible_v<typename _IteratorIn::value_t>)
	{
		const size_t c = countof(__beg, __end);
		union
		{
			__marray<typename _IteratorIn::value_t> *as_array;
			size_t *as_size;
		};
		as_array = union_cast<__marray<typename _IteratorIn::value_t> *>(
		 halloc(__all, sizeof(typename _IteratorIn::value_t) * c, __align, sizeof(size_t) + __offset));
		as_array->count = c;
		construct(iterator(&as_array->data[0]), iterator(&as_array->data[0] + c), __beg);
		return &as_array->data[0];
	}
	template<typename _IteratorIn, typename _Allocator>
	pf_decl_inline pf_decl_constexpr typename _IteratorIn::value_t *
	new_construct_aligned(
	 _Allocator &__all,
	 _IteratorIn __beg,
	 _IteratorIn __end,
	 align_val_t __align)
		requires(
		 is_allocator_v<_Allocator>
		 && std::is_copy_constructible_v<typename _IteratorIn::value_t>)
	{
		return new_construct_aligned_at(
		 __all,
		 __beg,
		 __end,
		 __align,
		 0);
	}
	template<typename _IteratorIn, typename _Allocator>
	pf_decl_inline pf_decl_constexpr typename _IteratorIn::value_t *
	new_construct(
	 _Allocator &__all,
	 _IteratorIn __beg,
	 _IteratorIn __end)
		requires(
		 is_allocator_v<_Allocator>
		 && std::is_copy_constructible_v<typename _IteratorIn::value_t>)
	{
		return new_construct_aligned_at(
		 __all,
		 __beg,
		 __end,
		 std::align_val_t(alignof(typename _IteratorIn::value_t)),
		 0);
	}


	/// MEMORY: Utility -> Destroy Delete
	template<typename _Ty>
	pf_decl_inline pf_decl_constexpr void
	destroy_delete(
	 std::remove_all_extents_t<_Ty> *__ptr) pf_attr_noexcept
		requires(std::is_array_v<_Ty>)
	{
		__marray<std::remove_all_extents_t<_Ty>> *as_array = __mem_get_marray_typeless<std::remove_all_extents_t<_Ty>>(__ptr);
		destroy(as_array);
		hfree(as_array);
	}
	template<typename _Ty, typename _Allocator>
	pf_decl_inline pf_decl_constexpr void
	destroy_delete(
	 _Allocator &__all,
	 std::remove_all_extents_t<_Ty> *__ptr) pf_attr_noexcept
		requires(
		 is_allocator_v<_Allocator>
		 && std::is_array_v<_Ty>)
	{
		__marray<std::remove_all_extents_t<_Ty>> *as_array = __mem_get_marray_typeless<std::remove_all_extents_t<_Ty>>(__ptr);
		destroy(as_array);
		deallocate(__all, as_array);
	}


	/// RAII: Deleter -> Default
	template<typename _Ty>
	class deleter_default
	{
	public:
		/// Operator()
		pf_decl_inline pf_decl_constexpr void
		operator()(
		 _Ty *__ptr) pf_attr_noexcept
		{
			destroy_delete<_Ty>(__ptr);
		}
	};
	template<typename _Ty>
	class deleter_default<_Ty[]>
	{
	public:
		/// Operator()
		pf_decl_inline pf_decl_constexpr void
		operator()(
		 _Ty *__ptr) pf_attr_noexcept
		{
			destroy_delete<_Ty[]>(__ptr);
		}
	};

	/// MEMORY: Deleter -> Allocator
	template<typename _Ty, typename _Allocator>
	class deleter_allocator
	{
	public:
		/// Constructor
		pf_decl_inline pf_decl_constexpr
		deleter_allocator(
		 _Allocator *__all) pf_attr_noexcept
			: allocator_(__all)
		{}

		/// Operator()
		pf_decl_inline pf_decl_constexpr void
		operator()(
		 _Ty *__ptr) const pf_attr_noexcept
		{
			destroy_delete<_Ty>(__ptr);
		}

	private:
		_Allocator *allocator_;
	};
	template<typename _Ty, typename _Allocator>
	class deleter_allocator<_Ty[], _Allocator>
	{
	public:
		/// Constructor
		pf_decl_inline pf_decl_constexpr
		deleter_allocator(
		 _Allocator *__all) pf_attr_noexcept
			: allocator_(__all)
		{}

		/// Operator()
		pf_decl_inline pf_decl_constexpr void
		operator()(
		 std::remove_all_extents_t<_Ty> *__ptr) const pf_attr_noexcept
		{
			destroy_delete<_Ty[]>(__ptr);
		}

	private:
		_Allocator *allocator_;
	};




	/*
	 * !            !
	 *   Unique Ptr
	 * !            !
	 */

	/// MEMORY: Unique -> Store
	template<typename _Ty>
	struct unique_store
	{
		/// Constructors
		template<typename... _Args>
		pf_decl_constexpr
		unique_store(
		 _Args &&...__args) pf_attr_noexcept
			requires(std::is_constructible_v<_Ty, _Args...>)
			: store(std::forward<_Args>(__args)...)
		{}
		unique_store(unique_store<_Ty> const &) = delete;
		unique_store(unique_store<_Ty> &&)			= delete;

		/// Destructor
		pf_decl_constexpr ~unique_store() pf_attr_noexcept = default;

		/// Store
		_Ty store;
	};

	/// MEMORY: Unique -> Array Store
	template<typename _Ty>
	struct unique_store<_Ty[]>
	{
		/// Constructors
		pf_decl_constexpr
		unique_store(
		 size_t __count) pf_attr_noexcept
			requires(std::is_default_constructible_v<_Ty>)
		{
			construct(
			 &this->store,
			 __count);
		}
		template<typename _IteratorIn>
		unique_store(
		 size_t __count,
		 _IteratorIn __beg,
		 _IteratorIn __end) pf_attr_noexcept
			requires(
			 is_iterator_v<_IteratorIn>
			 && std::is_convertible_v<typename _IteratorIn::value_t, _Ty>)
		{
			ignore						= __end;
			// Optimized construct, since we already know __count
			this->store.count = __count;
			construct(
			 iterator(&this->store.data[0]),
			 iterator(&this->store.data[0] + __count),
			 __beg);
		}
		pf_decl_constexpr
		unique_store(
		 initializer_list<_Ty> __il) pf_attr_noexcept
			requires(std::is_copy_constructible_v<_Ty>)
		{
			construct(
			 &this->store,
			 __il);
		}
		unique_store(unique_store<_Ty[]> const &) = delete;
		unique_store(unique_store<_Ty[]> &&)			= delete;

		/// Destructor
		pf_decl_constexpr ~unique_store() pf_attr_noexcept
		{
			destroy(&this->store);
		}

		/// Store
		__marray<_Ty> store;
	};

	/// MEMORY: Unique -> Pointer
	template<typename _Ty, typename _Deleter>
	class unique_ptr
	{
		/// Delete
		pf_decl_constexpr void
		__delete_no_check() pf_attr_noexcept
		{
			this->del_(this->ptr_);
		}

	public:
		/// Constructors
		pf_decl_constexpr
		unique_ptr(
		 unique_store<_Ty> *__ptr,
		 _Deleter &&__deleter) pf_attr_noexcept
			: ptr_(__ptr)
			, del_(std::move(__deleter))
		{}
		pf_decl_constexpr
		unique_ptr(
		 nullptr_t,
		 _Deleter &&__deleter) pf_attr_noexcept
			: unique_ptr(nullptr, std::move(__deleter))
		{}
		pf_decl_constexpr
		unique_ptr(
		 _Deleter &&__deleter) pf_attr_noexcept
			: unique_ptr(nullptr, std::move(__deleter))
		{}
		pf_decl_constexpr
		unique_ptr(
		 unique_ptr<_Ty, _Deleter> const &__r) pf_attr_noexcept = delete;
		pf_decl_constexpr
		unique_ptr(
		 unique_ptr<_Ty, _Deleter> &&__r) pf_attr_noexcept
			: unique_ptr(__r.ptr_, std::move(__r.del_))
		{
			__r.ptr_ = nullptr;
		}

		/// Destructor
		pf_decl_constexpr ~unique_ptr() pf_attr_noexcept
		{
			this->reset();
		}

		/// Operator=
		pf_decl_constexpr unique_ptr<_Ty, _Deleter> &
		operator=(
		 unique_ptr<_Ty, _Deleter> const &) pf_attr_noexcept = delete;
		pf_decl_constexpr unique_ptr<_Ty, _Deleter> &
		operator=(
		 unique_ptr<_Ty, _Deleter> &&__r) pf_attr_noexcept
		{
			if(this != &__r)
			{
				pf_assert(this->ptr_ != __r.ptr_, "Both unique ptr are equals!");
				this->set(__r.ptr_);
				__r.ptr_ = nullptr;
				__r.del_ = std::move(__r.del_);
			}
			return *this;
		}
		pf_decl_constexpr unique_ptr<_Ty, _Deleter> &
		operator=(
		 unique_store<_Ty> *__r) pf_attr_noexcept
		{
			if(this->ptr_ != __r)
			{
				this->set(__r);
			}
			return *this;
		}

		/// Operator<=> / ==
		pf_decl_constexpr bool
		operator==(
		 const _Ty *__r) pf_attr_noexcept
		{
			return this->ptr_ == __r;
		}
		pf_decl_constexpr std::strong_ordering
		operator<=>(
		 const _Ty *__r) pf_attr_noexcept
		{
			return this->ptr_ <=> __r;
		}

		/// Operator*
		pf_decl_constexpr _Ty &
		operator*() pf_attr_noexcept
		{
			return *this->get();
		}
		pf_decl_constexpr const _Ty &
		operator*() const pf_attr_noexcept
		{
			return *this->get();
		}

		/// Operator->
		pf_decl_constexpr _Ty *
		operator->() pf_attr_noexcept
		{
			return this->get();
		}
		pf_decl_constexpr const _Ty *
		operator->() const pf_attr_noexcept
		{
			return this->get();
		}

		/// Operator _Ty*
		pf_decl_constexpr
		operator _Ty *() pf_attr_noexcept
		{
			return this->get();
		}
		pf_decl_constexpr
		operator const _Ty *() const pf_attr_noexcept
		{
			return this->get();
		}

		/// Operator bool
		pf_decl_explicit pf_decl_constexpr
		operator bool() const pf_attr_noexcept
		{
			return this->get();
		}

		/// Get
		pf_decl_constexpr _Ty *
		get() pf_attr_noexcept
		{
			return &this->ptr_->store;
		}
		pf_decl_constexpr const _Ty *
		get() const pf_attr_noexcept
		{
			return &this->ptr_->store;
		}

		/// Set
		pf_decl_constexpr void
		set(
		 unique_store<_Ty> *__r,
		 _Deleter &&__deleter = _Deleter()) pf_attr_noexcept
		{
			if(this->ptr_) this->__delete_no_check();
			this->ptr_ = __r;
			this->del_ = std::move(__deleter);
		}

		/// Reset
		pf_decl_constexpr void
		reset() pf_attr_noexcept
		{
			if(this->ptr_)
			{
				this->__delete_no_check();
				this->ptr_ = nullptr;
			}
		}

	private:
		/// Store
		unique_store<_Ty> *ptr_;
		pf_hint_nounique_address _Deleter del_;
	};

	/// MEMORY: Unique -> Pointer Array
	template<typename _Ty, typename _Deleter>
	class unique_ptr<_Ty[], _Deleter>
	{
		/// Delete
		pf_decl_constexpr void
		__delete_no_check() pf_attr_noexcept
		{
			this->del_(this->ptr_);
		}

	public:
		using value_t									 = _Ty;
		using iterator_t							 = iterator<_Ty>;
		using const_iterator_t				 = const_iterator<_Ty>;
		using reverse_iterator_t			 = reverse_iterator<iterator_t>;
		using const_reverse_iterator_t = reverse_iterator<const_iterator_t>;

		/// Constructors
		pf_decl_constexpr
		unique_ptr(
		 unique_store<_Ty[]> *__ptr,
		 _Deleter &&__deleter) pf_attr_noexcept
			: ptr_(__ptr)
			, del_(std::move(__deleter))
		{}
		pf_decl_constexpr
		unique_ptr(
		 nullptr_t,
		 _Deleter &&__deleter) pf_attr_noexcept
			: unique_ptr(nullptr, std::move(__deleter))
		{}
		pf_decl_constexpr
		unique_ptr(
		 _Deleter &&__deleter) pf_attr_noexcept
			: unique_ptr(nullptr, std::move(__deleter))
		{}
		pf_decl_constexpr
		unique_ptr(
		 unique_ptr<_Ty[], _Deleter> const &__r) pf_attr_noexcept = delete;
		pf_decl_constexpr
		unique_ptr(
		 unique_ptr<_Ty[], _Deleter> &&__r) pf_attr_noexcept
			: unique_ptr(__r.ptr_, std::move(__r.del_))
		{
			__r.ptr_ = nullptr;
		}

		/// Destructor
		pf_decl_constexpr ~unique_ptr() pf_attr_noexcept
		{
			this->reset();
		}

		/// Operator=
		pf_decl_constexpr unique_ptr<_Ty[], _Deleter> &
		operator=(
		 unique_ptr<_Ty[], _Deleter> const &) pf_attr_noexcept = delete;
		pf_decl_constexpr unique_ptr<_Ty[], _Deleter> &
		operator=(
		 unique_ptr<_Ty[], _Deleter> &&__r) pf_attr_noexcept
		{
			if(this != &__r)
			{
				pf_assert(this->ptr_ != __r.ptr_, "Both unique ptr are equals!");
				this->set(__r.ptr_);
				this->del_ = std::move(__r.del_);
				__r.ptr_	 = nullptr;
			}
			return *this;
		}
		pf_decl_constexpr unique_ptr<_Ty[], _Deleter> &
		operator=(
		 unique_store<_Ty[]> *__s) pf_attr_noexcept
		{
			if(this->ptr_ != __s)
			{
				this->set(__s);
			}
			return *this;
		}

		/// Operator<=> / ==
		pf_decl_constexpr bool
		operator==(
		 const _Ty *__ptr) const pf_attr_noexcept
		{
			return this->get() == __ptr;
		}
		pf_decl_constexpr std::strong_ordering
		operator<=>(
		 const _Ty *__ptr) const pf_attr_noexcept
		{
			return this->get() <=> __ptr;
		}

		/// Operator[]
		pf_decl_constexpr _Ty &
		operator[](
		 size_t __index) pf_attr_noexcept
		{
			return this->get()[__index];
		}
		pf_decl_constexpr const _Ty &
		operator[](
		 size_t __index) const pf_attr_noexcept
		{
			return this->get()[__index];
		}

		/// Operator _Ty*
		pf_decl_constexpr
		operator _Ty *() pf_attr_noexcept
		{
			return this->get();
		}
		pf_decl_constexpr
		operator const _Ty *() const pf_attr_noexcept
		{
			return this->get();
		}

		/// Operator bool
		pf_decl_explicit pf_decl_constexpr
		operator bool() const pf_attr_noexcept
		{
			return this->get();
		}

		/// Get
		pf_decl_constexpr _Ty *
		get() pf_attr_noexcept
		{
			return &this->ptr_->store.data[0];
		}
		pf_decl_constexpr const _Ty *
		get() const pf_attr_noexcept
		{
			return &this->ptr_->store.data[0];
		}

		/// Set
		pf_decl_constexpr void
		set(
		 unique_store<_Ty[]> *__r,
		 _Deleter &&__deleter = _Deleter()) pf_attr_noexcept
		{
			if(this->ptr_) this->__delete_no_check();
			this->ptr_ = __r;
			this->del_ = std::move(__deleter);
		}

		/// Begin
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr iterator_t
		begin() pf_attr_noexcept
		{
			return &this->ptr_->store.data[0];
		}
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const_iterator_t
		begin() const pf_attr_noexcept
		{
			return &this->ptr_->store.data[0];
		}
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const_iterator_t
		cbegin() const pf_attr_noexcept
		{
			return this->begin();
		}

		/// Reverse Begin
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr reverse_iterator_t
		rbegin() pf_attr_noexcept
		{
			return &this->ptr_->store.data[0] + (this->ptr_->store.count - 1);
		}
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const_reverse_iterator_t
		rbegin() const pf_attr_noexcept
		{
			return &this->ptr_->store.data[0] + (this->ptr_->store.count - 1);
		}
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const_reverse_iterator_t
		crbegin() const pf_attr_noexcept
		{
			return &this->rbegin();
		}

		/// End
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr iterator_t
		end() pf_attr_noexcept
		{
			return &this->ptr_->store.data[0] + this->ptr_->store.count;
		}
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const_iterator_t
		end() const pf_attr_noexcept
		{
			return &this->ptr_->store.data[0] + this->ptr_->store.count;
		}
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const_iterator_t
		cend() const pf_attr_noexcept
		{
			return this->end();
		}

		/// Reverse End
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr reverse_iterator_t
		rend() pf_attr_noexcept
		{
			return &this->ptr_->store.data[0] - 1;
		}
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const_reverse_iterator_t
		rend() const pf_attr_noexcept
		{
			return &this->ptr_->store.data[0] - 1;
		}
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const_reverse_iterator_t
		crend() const pf_attr_noexcept
		{
			return this->rend();
		}

		/// Count
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr size_t
		count() pf_attr_noexcept
		{
			return this->ptr_->store.count;
		}

		/// Size
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr size_t
		size() pf_attr_noexcept
		{
			return this->count * sizeof(std::remove_all_extents_t<_Ty>);
		}

		/// Reset
		pf_decl_constexpr void
		reset() pf_attr_noexcept
		{
			if(this->ptr_)
			{
				this->__delete_no_check();
				this->ptr_ = nullptr;
			}
		}

	private:
		/// Store
		unique_store<_Ty[]> *ptr_;
		pf_hint_nounique_address _Deleter del_;
	};


	/// MEMORY: Unique -> Maker
	template<typename _Ty, typename... _Args>
	pf_hint_nodiscard pf_decl_constexpr unique_ptr<_Ty, deleter_default<unique_store<_Ty>>>
	make_unique(
	 _Args &&...__args)
		requires(!std::is_array_v<_Ty>)
	{
		return unique_ptr(
		 new_construct<unique_store<_Ty>>(
			std::forward<_Args>(__args)...),
		 deleter_default<unique_store<_Ty>>());
	}
	template<typename _Ty, typename... _Args>
	pf_hint_nodiscard pf_decl_constexpr unique_ptr<_Ty, deleter_default<unique_store<_Ty>>>
	make_unique_aligned(
	 align_val_t __align,
	 _Args &&...__args)
		requires(!std::is_array_v<_Ty>)
	{
		return unique_ptr(
		 new_construct_aligned<unique_store<_Ty>>(
			__align,
			std::forward<_Args>(__args)...),
		 deleter_default<unique_store<_Ty>>());
	}
	template<typename _Ty, typename... _Args>
	pf_hint_nodiscard pf_decl_constexpr unique_ptr<_Ty, deleter_default<unique_store<_Ty>>>
	make_unique_aligned_at(
	 align_val_t __align,
	 size_t __offset,
	 _Args &&...__args)
		requires(!std::is_array_v<_Ty>)
	{
		return unique_ptr(
		 new_construct_aligned_at<unique_store<_Ty>>(
			__align,
			__offset,
			std::forward<_Args>(__args)...),
		 deleter_default<unique_store<_Ty>>());
	}

	/// MEMORY: Unique -> Maker -> Array
	template<typename _Ty>
	pf_hint_nodiscard pf_decl_constexpr unique_ptr<_Ty, deleter_default<unique_store<_Ty>>>
	make_unique(
	 size_t __count)
		requires(
		 std::is_array_v<_Ty>
		 && std::is_default_constructible_v<std::remove_all_extents_t<_Ty>>)
	{
		pf_assert(__count != 0, "Trying to create empty memory array!");
		return unique_ptr(
		 new_construct_aligned_at_ex<unique_store<_Ty>>(
			__count * sizeof(std::remove_all_extents_t<_Ty>),
			pf_alignof(std::remove_all_extents_t<_Ty>),
			sizeof(__marray<std::remove_all_extents_t<_Ty>>),
			__count),
		 deleter_default<unique_store<_Ty>>());
	}
	template<
	 typename _Ty,
	 typename _IteratorIn>
	pf_hint_nodiscard pf_decl_constexpr unique_ptr<_Ty, deleter_default<unique_store<_Ty>>>
	make_unique(
	 _IteratorIn __beg,
	 _IteratorIn __end)
		requires(
		 std::is_array_v<_Ty>
		 && is_iterator_v<_IteratorIn>
		 && std::is_convertible_v<typename _IteratorIn::value_t, std::remove_all_extents_t<_Ty>>)
	{
		const size_t c = countof(__beg, __end);
		pf_assert(c != 0, "Trying to create empty memory array!");
		return unique_ptr(
		 new_construct_aligned_at_ex<unique_store<_Ty>>(
			c * sizeof(std::remove_all_extents_t<_Ty>),
			pf_alignof(std::remove_all_extents_t<_Ty>),
			sizeof(__marray<std::remove_all_extents_t<_Ty>>),
			c,
			__beg,
			__end),
		 deleter_default<unique_store<_Ty>>());
	}
	template<typename _Ty>
	pf_hint_nodiscard pf_decl_constexpr unique_ptr<_Ty, deleter_default<unique_store<_Ty>>>
	make_unique(
	 initializer_list<std::remove_all_extents_t<_Ty>> __il)
		requires(
		 std::is_array_v<_Ty>
		 && std::is_copy_constructible_v<std::remove_all_extents_t<_Ty>>)
	{
		const size_t c = __il.size();
		pf_assert(c != 0, "Trying to create empty memory array!");
		return unique_ptr(
		 new_construct_aligned_at_ex<unique_store<_Ty>>(
			c * sizeof(std::remove_all_extents_t<_Ty>),
			pf_alignof(std::remove_all_extents_t<_Ty>),
			sizeof(__marray<std::remove_all_extents_t<_Ty>>),
			__il),
		 deleter_default<unique_store<_Ty>>());
	}
	template<typename _Ty>
	pf_hint_nodiscard pf_decl_constexpr unique_ptr<_Ty, deleter_default<unique_store<_Ty>>>
	make_unique_aligned(
	 size_t __count,
	 align_val_t __align)
		requires(
		 std::is_array_v<_Ty>
		 && std::is_default_constructible_v<std::remove_all_extents_t<_Ty>>)
	{
		pf_assert(__count != 0, "Trying to create empty memory array!");
		return unique_ptr(
		 new_construct_aligned_at_ex<unique_store<_Ty>>(
			__count * sizeof(std::remove_all_extents_t<_Ty>),
			__align,
			sizeof(__marray<std::remove_all_extents_t<_Ty>>),
			__count),
		 deleter_default<unique_store<_Ty>>());
	}
	template<
	 typename _Ty,
	 typename _IteratorIn>
	pf_hint_nodiscard pf_decl_constexpr unique_ptr<_Ty, deleter_default<unique_store<_Ty>>>
	make_unique_aligned(
	 _IteratorIn __beg,
	 _IteratorIn __end,
	 align_val_t __align)
		requires(
		 std::is_array_v<_Ty>
		 && is_iterator_v<_IteratorIn>
		 && std::is_convertible_v<typename _IteratorIn::value_t, std::remove_all_extents_t<_Ty>>)
	{
		const size_t c = countof(__beg, __end);
		pf_assert(c != 0, "Trying to create empty memory array!");
		return unique_ptr(
		 new_construct_aligned_at_ex<unique_store<_Ty>>(
			c * sizeof(std::remove_all_extents_t<_Ty>),
			__align,
			sizeof(__marray<std::remove_all_extents_t<_Ty>>),
			c,
			__beg,
			__end),
		 deleter_default<unique_store<_Ty>>());
	}
	template<typename _Ty>
	pf_hint_nodiscard pf_decl_constexpr unique_ptr<_Ty, deleter_default<unique_store<_Ty>>>
	make_unique_aligned(
	 initializer_list<std::remove_all_extents_t<_Ty>> __il,
	 align_val_t __align)
		requires(
		 std::is_array_v<_Ty>
		 && std::is_copy_constructible_v<std::remove_all_extents_t<_Ty>>)
	{
		const size_t c = __il.size();
		pf_assert(c != 0, "Trying to create empty memory array!");
		return unique_ptr(
		 new_construct_aligned_at_ex<unique_store<_Ty>>(
			c * sizeof(std::remove_all_extents_t<_Ty>),
			__align,
			sizeof(__marray<std::remove_all_extents_t<_Ty>>),
			__il),
		 deleter_default<unique_store<_Ty>>());
	}

	/// MEMORY: Unique -> Maker -> Allocator
	template<typename _Ty, typename _Allocator, typename... _Args>
	pf_hint_nodiscard pf_decl_constexpr unique_ptr<_Ty, deleter_allocator<unique_store<_Ty>, _Allocator>>
	make_unique(
	 _Allocator &__all,
	 _Args &&...__args)
		requires(
		 is_allocator_v<_Allocator>
		 && !std::is_array_v<_Ty>)
	{
		return unique_ptr(
		 new_construct<unique_store<_Ty>>(
			__all,
			std::forward<_Args>(__args)...),
		 deleter_allocator<unique_store<_Ty>, _Allocator>(&__all));
	}
	template<typename _Ty, typename _Allocator, typename... _Args>
	pf_hint_nodiscard pf_decl_constexpr unique_ptr<_Ty, deleter_allocator<unique_store<_Ty>, _Allocator>>
	make_unique_aligned(
	 _Allocator &__all,
	 align_val_t __align,
	 _Args &&...__args)
		requires(
		 is_allocator_v<_Allocator>
		 && !std::is_array_v<_Ty>)
	{
		return unique_ptr(
		 new_construct_aligned<unique_store<_Ty>>(
			__all,
			__align,
			std::forward<_Args>(__args)...),
		 deleter_allocator<unique_store<_Ty>, _Allocator>(&__all));
	}
	template<typename _Ty, typename _Allocator, typename... _Args>
	pf_hint_nodiscard pf_decl_constexpr unique_ptr<_Ty, deleter_allocator<unique_store<_Ty>, _Allocator>>
	make_unique_aligned_at(
	 _Allocator &__all,
	 align_val_t __align,
	 size_t __offset,
	 _Args &&...__args)
		requires(
		 is_allocator_v<_Allocator>
		 && !std::is_array_v<_Ty>)
	{
		return unique_ptr(
		 new_construct_aligned_at<unique_store<_Ty>>(
			__all,
			__align,
			__offset,
			std::forward<_Args>(__args)...),
		 deleter_allocator<unique_store<_Ty>, _Allocator>(&__all));
	}

	/// MEMORY: Unique -> Maker -> Allocator -> Array
	template<typename _Ty, typename _Allocator>
	pf_hint_nodiscard pf_decl_constexpr unique_ptr<_Ty, deleter_allocator<unique_store<_Ty>, _Allocator>>
	make_unique(
	 _Allocator &__all,
	 size_t __count)
		requires(
		 is_allocator_v<_Allocator>
		 && std::is_array_v<_Ty>)
	{
		pf_assert(__count != 0, "Trying to create empty memory array!");
		return unique_ptr(
		 new_construct_aligned_at_ex<unique_store<_Ty>>(
			__all,
			__count * sizeof(std::remove_all_extents_t<_Ty>),
			pf_alignof(std::remove_all_extents_t<_Ty>),
			sizeof(__marray<std::remove_all_extents_t<_Ty>>),
			__count),
		 deleter_allocator<unique_store<_Ty>, _Allocator>(&__all));
	}
	template<typename _Ty, typename _Allocator, typename _IteratorIn>
	pf_hint_nodiscard pf_decl_constexpr unique_ptr<_Ty, deleter_allocator<unique_store<_Ty>, _Allocator>>
	make_unique(
	 _Allocator &__all,
	 _IteratorIn __beg,
	 _IteratorIn __end)
		requires(
		 is_allocator_v<_Allocator>
		 && std::is_array_v<_Ty>
		 && is_iterator_v<_IteratorIn>
		 && std::is_convertible_v<typename _IteratorIn::value_t, std::remove_all_extents_t<_Ty>>)
	{
		const size_t c = countof(__beg, __end);
		pf_assert(c != 0, "Trying to create empty memory array!");
		return unique_ptr(
		 new_construct_aligned_at_ex<unique_store<_Ty>>(
			__all,
			c * sizeof(std::remove_all_extents_t<_Ty>),
			pf_alignof(std::remove_all_extents_t<_Ty>),
			sizeof(__marray<std::remove_all_extents_t<_Ty>>),
			c,
			__beg,
			__end),
		 deleter_allocator<unique_store<_Ty>, _Allocator>(&__all));
	}
	template<typename _Ty, typename _Allocator>
	pf_hint_nodiscard pf_decl_constexpr unique_ptr<_Ty, deleter_allocator<unique_store<_Ty>, _Allocator>>
	make_unique(
	 _Allocator &__all,
	 initializer_list<std::remove_all_extents_t<_Ty>> __il)
		requires(
		 is_allocator_v<_Allocator>
		 && std::is_array_v<_Ty>)
	{
		const size_t c = __il.size();
		pf_assert(c != 0, "Trying to create empty memory array!");
		return unique_ptr(
		 new_construct_aligned_at_ex<unique_store<_Ty>>(
			__all,
			c * sizeof(std::remove_all_extents_t<_Ty>),
			pf_alignof(std::remove_all_extents_t<_Ty>),
			sizeof(__marray<std::remove_all_extents_t<_Ty>>),
			__il),
		 deleter_allocator<unique_store<_Ty>, _Allocator>(&__all));
	}
	template<typename _Ty, typename _Allocator>
	pf_hint_nodiscard pf_decl_constexpr unique_ptr<_Ty, deleter_allocator<unique_store<_Ty>, _Allocator>>
	make_unique_aligned(
	 _Allocator &__all,
	 size_t __count,
	 align_val_t __align)
		requires(
		 is_allocator_v<_Allocator>
		 && std::is_array_v<_Ty>)
	{
		pf_assert(__count != 0, "Trying to create empty memory array!");
		return unique_ptr(
		 new_construct_aligned_at_ex<unique_store<_Ty>>(
			__all,
			__count * sizeof(std::remove_all_extents_t<_Ty>),
			__align,
			sizeof(__marray<std::remove_all_extents_t<_Ty>>),
			__count),
		 deleter_allocator<unique_store<_Ty>, _Allocator>(&__all));
	}
	template<typename _Ty, typename _Allocator, typename _IteratorIn>
	pf_hint_nodiscard pf_decl_constexpr unique_ptr<_Ty, deleter_allocator<unique_store<_Ty>, _Allocator>>
	make_unique_aligned(
	 _Allocator &__all,
	 _IteratorIn __beg,
	 _IteratorIn __end,
	 align_val_t __align)
		requires(
		 is_allocator_v<_Allocator>
		 && std::is_array_v<_Ty>
		 && is_iterator_v<_IteratorIn>
		 && std::is_convertible_v<typename _IteratorIn::value_t, std::remove_all_extents_t<_Ty>>)
	{
		const size_t c = countof(__beg, __end);
		pf_assert(c != 0, "Trying to create empty memory array!");
		return unique_ptr(
		 new_construct_aligned_at_ex<unique_store<_Ty>>(
			__all,
			c * sizeof(std::remove_all_extents_t<_Ty>),
			__align,
			sizeof(__marray<std::remove_all_extents_t<_Ty>>),
			c,
			__beg,
			__end),
		 deleter_allocator<unique_store<_Ty>, _Allocator>(&__all));
	}
	template<typename _Ty, typename _Allocator>
	pf_hint_nodiscard pf_decl_constexpr unique_ptr<_Ty, deleter_allocator<unique_store<_Ty>, _Allocator>>
	make_unique_aligned(
	 _Allocator &__all,
	 initializer_list<std::remove_all_extents_t<_Ty>> __il,
	 align_val_t __align)
		requires(
		 is_allocator_v<_Allocator>
		 && std::is_array_v<_Ty>)
	{
		const size_t c = __il.size();
		pf_assert(c != 0, "Trying to create empty memory array!");
		return unique_ptr(
		 new_construct_aligned_at_ex<unique_store<_Ty>>(
			__all,
			c * sizeof(std::remove_all_extents_t<_Ty>),
			__align,
			sizeof(__marray<std::remove_all_extents_t<_Ty>>),
			__il),
		 deleter_allocator<unique_store<_Ty>, _Allocator>(&__all));
	}




	/*
	 * !            !
	 *   Shared Ptr
	 * !            !
	 */

	/// MEMORY: Shared -> Store
	template<typename _Ty>
	struct shared_store
	{
		/// Constructor
		template<typename... _Args>
		pf_decl_constexpr
		shared_store(
		 _Args &&...__args) pf_attr_noexcept
			requires(std::is_constructible_v<_Ty, _Args...>)
			: shared(1)
			, store(std::forward<_Args>(__args)...)
		{}
		shared_store(shared_store<_Ty> const &) = delete;
		shared_store(shared_store<_Ty> &&)			= delete;

		/// Destructor
		pf_decl_constexpr ~shared_store() pf_attr_noexcept
		{
			pf_assert(shared == 0, "Shared count isn't equal to 0!");
		}

		/// Store
		size_t shared;
		_Ty store;
	};

	/// MEMORY: Shared -> Array Store
	template<typename _Ty>
	struct shared_store<_Ty[]>
	{
		/// Constructors
		pf_decl_constexpr
		shared_store(
		 size_t __count) pf_attr_noexcept
			requires(std::is_default_constructible_v<_Ty>)
		{
			construct(
			 &this->store,
			 __count);
		}
		template<typename _IteratorIn>
		shared_store(
		 size_t __count,
		 _IteratorIn __beg,
		 _IteratorIn __end) pf_attr_noexcept
			requires(
			 is_iterator_v<_IteratorIn>
			 && std::is_convertible_v<typename _IteratorIn::value_t, _Ty>)
		{
			ignore						= __end;
			// Optimized construct, since we already know __count
			this->store.count = __count;
			construct(
			 iterator(&this->store.data[0]),
			 iterator(&this->store.data[0] + __count),
			 __beg);
		}
		pf_decl_constexpr
		shared_store(
		 initializer_list<_Ty> __il) pf_attr_noexcept
			requires(std::is_copy_constructible_v<_Ty>)
		{
			construct(
			 &this->store,
			 __il);
		}
		shared_store(unique_store<_Ty[]> const &) = delete;
		shared_store(unique_store<_Ty[]> &&)			= delete;

		/// Destructor
		pf_decl_constexpr ~shared_store() pf_attr_noexcept
		{
			destroy(&this->store);
		}

		/// Store
		size_t shared;
		__marray<_Ty> store;
	};

	/// MEMORY: Shared -> Pointer
	template<typename _Ty, typename _Deleter>
	class shared_ptr
	{
		/// Delete
		pf_decl_constexpr void
		__increase_share_count() pf_attr_noexcept
		{
			if(this->ptr_) ++this->ptr_->shared;
		}
		pf_decl_constexpr void
		__delete_no_check() pf_attr_noexcept
		{
			if(--this->ptr_->shared == 0) this->del_(this->ptr_);
		}

	public:
		/// Constructors
		pf_decl_constexpr
		shared_ptr(
		 shared_store<_Ty> *__ptr,
		 _Deleter &&__deleter) pf_attr_noexcept
			: ptr_(__ptr)
			, del_(std::move(__deleter))
		{}
		pf_decl_constexpr
		shared_ptr(
		 nullptr_t,
		 _Deleter &&__deleter) pf_attr_noexcept
			: shared_ptr(nullptr, std::move(__deleter))
		{}
		pf_decl_constexpr
		shared_ptr(
		 _Deleter &&__deleter) pf_attr_noexcept
			: shared_ptr(nullptr, std::move(__deleter))
		{}
		pf_decl_constexpr
		shared_ptr(
		 shared_ptr<_Ty, _Deleter> const &__r) pf_attr_noexcept
			: shared_ptr(__r.ptr_, __r.del_)
		{
			this->__increase_share_count();
		}
		pf_decl_constexpr
		shared_ptr(
		 shared_ptr<_Ty, _Deleter> &&__r) pf_attr_noexcept
			: shared_ptr(__r.ptr_, std::move(__r.del_))
		{
			this->ptr_ = __r;
			__r.ptr_	 = nullptr;
		}

		/// Destructor
		pf_decl_constexpr ~shared_ptr() pf_attr_noexcept
		{
			this->reset();
		}

		/// Operator=
		pf_decl_constexpr shared_ptr<_Ty, _Deleter> &
		operator=(
		 shared_ptr<_Ty, _Deleter> const &__r) pf_attr_noexcept
		{
			if(this != &__r)
			{
				this->set(__r.ptr, __r.del_);
			}
			return *this;
		}
		pf_decl_constexpr shared_ptr<_Ty, _Deleter> &
		operator=(
		 shared_ptr<_Ty, _Deleter> &&__r) pf_attr_noexcept
		{
			if(this != &__r)
			{
				if(this->ptr_) this->__delete_no_check();
				this->ptr_ = __r;
				this->del_ = std::move(__r.del_);
				__r.ptr_	 = nullptr;
			}
			return *this;
		}
		pf_decl_constexpr shared_ptr<_Ty, _Deleter> &
		operator=(
		 shared_store<_Ty> *__s) pf_attr_noexcept
		{
			if(this != &__s)
			{
				this->set(__s);
			}
			return *this;
		}

		/// Operator <=> / ==
		pf_decl_constexpr bool
		operator==(
		 const _Ty *__ptr) const pf_attr_noexcept
		{
			return this->ptr_ == __ptr;
		}
		pf_decl_constexpr std::strong_ordering
		operator<=>(
		 const _Ty *__ptr) const pf_attr_noexcept
		{
			return this->ptr_ <=> __ptr;
		}

		/// Operator*
		pf_decl_constexpr _Ty &
		operator*() pf_attr_noexcept
		{
			return *this->get();
		}
		pf_decl_constexpr const _Ty &
		operator*() const pf_attr_noexcept
		{
			return *this->get();
		}

		/// Operator->
		pf_decl_constexpr _Ty *
		operator->() pf_attr_noexcept
		{
			return this->get();
		}
		pf_decl_constexpr const _Ty *
		operator->() const pf_attr_noexcept
		{
			return this->get();
		}

		/// Operator _Ty *
		pf_decl_constexpr
		operator _Ty *() pf_attr_noexcept
		{
			return this->get();
		}
		pf_decl_constexpr operator const _Ty *() const pf_attr_noexcept
		{
			return this->get();
		}

		/// Operator bool
		pf_decl_explicit pf_decl_constexpr operator bool() const pf_attr_noexcept
		{
			return this->get();
		}

		/// Get
		pf_decl_constexpr _Ty *
		get() pf_attr_noexcept
		{
			return &this->ptr_->store;
		}
		pf_decl_constexpr const _Ty *
		get() const pf_attr_noexcept
		{
			return &this->ptr_->store;
		}

		/// Set
		pf_decl_constexpr void
		set(shared_store<_Ty> *__r, _Deleter &&__del = _Deleter()) pf_attr_noexcept
		{
			if(this->ptr_ && (this->ptr_ != __r))
			{
				this->__delete_no_check();
				this->ptr_ = __r;
				this->del_ = std::move(__del);
				this->__increase_share_count();
			}
		}

		/// Reset
		pf_decl_constexpr void
		reset() pf_attr_noexcept
		{
			if(this->ptr_)
			{
				this->__delete_no_check();
				this->ptr_ = nullptr;
			}
		}

	private:
		/// Store
		shared_store<_Ty> *ptr_;
		pf_hint_nounique_address _Deleter del_;
	};

	/// MEMORY: Shared -> Array Pointer
	template<typename _Ty, typename _Deleter>
	class shared_ptr<_Ty[], _Deleter>
	{
		/// Delete
		pf_decl_constexpr void
		__increase_share_count() pf_attr_noexcept
		{
			if(this->ptr_) ++this->ptr_->shared;
		}
		pf_decl_constexpr void
		__delete_no_check() pf_attr_noexcept
		{
			if(--this->ptr_->shared == 0) this->del_(this->ptr_);
		}

	public:
		using value_t									 = _Ty;
		using iterator_t							 = iterator<_Ty>;
		using const_iterator_t				 = const_iterator<_Ty>;
		using reverse_iterator_t			 = reverse_iterator<iterator_t>;
		using const_reverse_iterator_t = reverse_iterator<const_iterator_t>;

		/// Constructors
		pf_decl_constexpr
		shared_ptr(
		 shared_store<_Ty[]> *__ptr,
		 _Deleter &&__deleter) pf_attr_noexcept
			: ptr_(__ptr)
			, del_(std::move(__deleter))
		{}
		pf_decl_constexpr
		shared_ptr(
		 nullptr_t,
		 _Deleter &&__deleter) pf_attr_noexcept
			: shared_ptr(nullptr, std::move(__deleter))
		{}
		pf_decl_constexpr
		shared_ptr(
		 _Deleter &&__deleter) pf_attr_noexcept
			: shared_ptr(nullptr, std::move(__deleter))
		{}
		pf_decl_constexpr
		shared_ptr(
		 shared_ptr<_Ty[], _Deleter> const &__r) pf_attr_noexcept
			: shared_ptr(__r.ptr_, __r.del_)
		{
			this->__increase_share_count();
		}
		pf_decl_constexpr
		shared_ptr(
		 shared_ptr<_Ty[], _Deleter> &&__r) pf_attr_noexcept
			: shared_ptr(__r.ptr_, std::move(__r.del_))
		{
			this->ptr_ = __r;
			__r.ptr_	 = nullptr;
		}

		/// Destructor
		pf_decl_constexpr ~shared_ptr() pf_attr_noexcept
		{
			this->reset();
		}

		/// Operator=
		pf_decl_constexpr shared_ptr<_Ty[], _Deleter> &
		operator=(
		 shared_ptr<_Ty[], _Deleter> const &__r) pf_attr_noexcept
		{
			if(this != &__r)
			{
				this->set(__r.ptr_, __r.del_);
			}
			return *this;
		}
		pf_decl_constexpr shared_ptr<_Ty[], _Deleter> &
		operator=(
		 shared_ptr<_Ty[], _Deleter> &&__r) pf_attr_noexcept
		{
			if(this != &__r)
			{
				if(this->ptr_) this->__delete_no_check();
				this->ptr_ = __r;
				this->del_ = std::move(__r.del_);
				__r.ptr_	 = nullptr;
			}
			return *this;
		}
		pf_decl_constexpr shared_ptr<_Ty[], _Deleter> &
		operator=(
		 shared_store<_Ty[]> *__s) pf_attr_noexcept
		{
			if(this != &__s)
			{
				this->set(__s);
			}
			return *this;
		}

		/// Operator <=> / ==
		pf_decl_constexpr bool
		operator==(
		 const _Ty *__ptr) const pf_attr_noexcept
		{
			return this->ptr_ == __ptr;
		}
		pf_decl_constexpr std::strong_ordering
		operator<=>(
		 const _Ty *__ptr) const pf_attr_noexcept
		{
			return this->ptr_ <=> __ptr;
		}

		/// Operator[]
		pf_decl_constexpr _Ty &
		operator[](
		 size_t __index) pf_attr_noexcept
		{
			return this->get()[__index];
		}
		pf_decl_constexpr const _Ty &
		operator[](
		 size_t __index) const pf_attr_noexcept
		{
			return this->get()[__index];
		}

		/// Operator _Ty *
		pf_decl_constexpr
		operator _Ty *() pf_attr_noexcept
		{
			return this->get();
		}
		pf_decl_constexpr
		operator const _Ty *() const pf_attr_noexcept
		{
			return this->get();
		}

		/// Operator bool
		pf_decl_explicit pf_decl_constexpr
		operator bool() const pf_attr_noexcept
		{
			return this->get();
		}

		/// Get
		pf_decl_constexpr _Ty *
		get() pf_attr_noexcept
		{
			return &this->ptr_->store.data[0];
		}
		pf_decl_constexpr const _Ty *
		get() const pf_attr_noexcept
		{
			return &this->ptr_->store.data[0];
		}

		/// Set
		pf_decl_constexpr void
		set(
		 shared_store<_Ty[]> *__r, _Deleter &&__del = _Deleter()) pf_attr_noexcept
		{
			if(this->ptr_ && (this->ptr_ != __r))
			{
				this->__delete_no_check();
				this->ptr_ = __r;
				this->del_ = std::move(__del);
				this->__increase_share_count();
			}
		}

		/// Begin
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr iterator_t
		begin() pf_attr_noexcept
		{
			return &this->ptr_->store.data[0];
		}
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const_iterator_t
		begin() const pf_attr_noexcept
		{
			return &this->ptr_->store.data[0];
		}
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const_iterator_t
		cbegin() const pf_attr_noexcept
		{
			return this->begin();
		}

		/// Reverse Begin
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr reverse_iterator_t
		rbegin() pf_attr_noexcept
		{
			return &this->ptr_->store.data[0] + (this->ptr_->store.count - 1);
		}
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const_reverse_iterator_t
		rbegin() const pf_attr_noexcept
		{
			return &this->ptr_->store.data[0] + (this->ptr_->store.count - 1);
		}
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const_reverse_iterator_t
		crbegin() const pf_attr_noexcept
		{
			return &this->rbegin();
		}

		/// End
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr iterator_t
		end() pf_attr_noexcept
		{
			return &this->ptr_->store.data[0] + this->ptr_->store.count;
		}
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const_iterator_t
		end() const pf_attr_noexcept
		{
			return &this->ptr_->store.data[0] + this->ptr_->store.count;
		}
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const_iterator_t
		cend() const pf_attr_noexcept
		{
			return this->end();
		}

		/// Reverse End
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr reverse_iterator_t
		rend() pf_attr_noexcept
		{
			return &this->ptr_->store.data[0] - 1;
		}
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const_reverse_iterator_t
		rend() const pf_attr_noexcept
		{
			return &this->ptr_->store.data[0] - 1;
		}
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const_reverse_iterator_t
		crend() const pf_attr_noexcept
		{
			return this->rend();
		}

		/// Count
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr size_t
		count() pf_attr_noexcept
		{
			return this->ptr_->store.count;
		}

		/// Size
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr size_t
		size() pf_attr_noexcept
		{
			return this->count * sizeof(std::remove_all_extents_t<_Ty>);
		}

		/// Reset
		pf_decl_constexpr void
		reset() pf_attr_noexcept
		{
			if(this->ptr_)
			{
				this->__delete_no_check();
				this->ptr_ = nullptr;
			}
		}

	private:
		/// Store
		shared_store<_Ty[]> *ptr_;
		pf_hint_nounique_address _Deleter del_;
	};

	/// MEMORY: Shared -> Maker
	template<typename _Ty, typename... _Args>
	pf_hint_nodiscard pf_decl_constexpr shared_ptr<_Ty, deleter_default<shared_store<_Ty>>>
	make_shared(
	 _Args &&...__args)
		requires(!std::is_array_v<_Ty>)
	{
		return shared_ptr(
		 new_construct_aligned_at<shared_store<_Ty>>(
			pf_alignof(_Ty),
			sizeof(size_t),
			std::forward<_Args>(__args)...),
		 deleter_default<shared_store<_Ty>>());
	}
	template<typename _Ty, typename... _Args>
	pf_hint_nodiscard pf_decl_constexpr shared_ptr<_Ty, deleter_default<shared_store<_Ty>>>
	make_shared_aligned(
	 align_val_t __align,
	 _Args &&...__args)
		requires(!std::is_array_v<_Ty>)
	{
		return shared_ptr(
		 new_construct_aligned_at<shared_store<_Ty>>(
			__align,
			sizeof(size_t),
			std::forward<_Args>(__args)...),
		 deleter_default<shared_store<_Ty>>());
	}
	template<typename _Ty, typename... _Args>
	pf_hint_nodiscard pf_decl_constexpr shared_ptr<_Ty, deleter_default<shared_store<_Ty>>>
	make_shared_aligned_at(
	 align_val_t __align,
	 size_t __offset,
	 _Args &&...__args)
		requires(!std::is_array_v<_Ty>)
	{
		return shared_ptr(
		 new_construct_aligned_at<shared_store<_Ty>>(
			__align,
			sizeof(size_t) + __offset,
			std::forward<_Args>(__args)...),
		 deleter_default<shared_store<_Ty>>());
	}

	/// MEMORY: Shared -> Maker -> Array
	template<typename _Ty>
	pf_hint_nodiscard pf_decl_constexpr shared_ptr<_Ty, deleter_default<shared_store<_Ty>>>
	make_shared(
	 size_t __count)
		requires(
		 std::is_array_v<_Ty>
		 && std::is_default_constructible_v<std::remove_all_extents_t<_Ty>>)
	{
		pf_assert(__count != 0, "Trying to create empty memory array!");
		return shared_ptr(
		 new_construct_aligned_at_ex<shared_store<_Ty>>(
			__count * sizeof(std::remove_all_extents_t<_Ty>),
			pf_alignof(std::remove_all_extents_t<_Ty>),
			sizeof(size_t) + sizeof(__marray<std::remove_all_extents_t<_Ty>>),
			__count),
		 deleter_default<shared_store<_Ty>>());
	}
	template<
	 typename _Ty,
	 typename _IteratorIn>
	pf_hint_nodiscard pf_decl_constexpr shared_ptr<_Ty, deleter_default<shared_store<_Ty>>>
	make_shared(
	 _IteratorIn __beg,
	 _IteratorIn __end)
		requires(
		 std::is_array_v<_Ty>
		 && is_iterator_v<_IteratorIn>
		 && std::is_convertible_v<typename _IteratorIn::value_t, std::remove_all_extents_t<_Ty>>)
	{
		const size_t c = countof(__beg, __end);
		pf_assert(c != 0, "Trying to create empty memory array!");
		return shared_ptr(
		 new_construct_aligned_at_ex<shared_store<_Ty>>(
			c * sizeof(std::remove_all_extents_t<_Ty>),
			pf_alignof(std::remove_all_extents_t<_Ty>),
			sizeof(size_t) + sizeof(__marray<std::remove_all_extents_t<_Ty>>),
			c,
			__beg,
			__end),
		 deleter_default<shared_store<_Ty>>());
	}
	template<typename _Ty>
	pf_hint_nodiscard pf_decl_constexpr shared_ptr<_Ty, deleter_default<shared_store<_Ty>>>
	make_shared(
	 initializer_list<std::remove_all_extents_t<_Ty>> __il)
		requires(
		 std::is_array_v<_Ty>
		 && std::is_copy_constructible_v<std::remove_all_extents_t<_Ty>>)
	{
		const size_t c = __il.size();
		pf_assert(c != 0, "Trying to create empty memory array!");
		return shared_ptr(
		 new_construct_aligned_at_ex<shared_store<_Ty>>(
			c * sizeof(std::remove_all_extents_t<_Ty>),
			pf_alignof(std::remove_all_extents_t<_Ty>),
			sizeof(size_t) + sizeof(__marray<std::remove_all_extents_t<_Ty>>),
			__il),
		 deleter_default<shared_store<_Ty>>());
	}
	template<typename _Ty>
	pf_hint_nodiscard pf_decl_constexpr shared_ptr<_Ty, deleter_default<shared_store<_Ty>>>
	make_shared_aligned(
	 size_t __count,
	 align_val_t __align)
		requires(
		 std::is_array_v<_Ty>
		 && std::is_default_constructible_v<std::remove_all_extents_t<_Ty>>)
	{
		pf_assert(__count != 0, "Trying to create empty memory array!");
		return shared_ptr(
		 new_construct_aligned_at_ex<shared_store<_Ty>>(
			__count * sizeof(std::remove_all_extents_t<_Ty>),
			__align,
			sizeof(size_t) + sizeof(__marray<std::remove_all_extents_t<_Ty>>),
			__count),
		 deleter_default<shared_store<_Ty>>());
	}
	template<
	 typename _Ty,
	 typename _IteratorIn>
	pf_hint_nodiscard pf_decl_constexpr shared_ptr<_Ty, deleter_default<shared_store<_Ty>>>
	make_shared_aligned(
	 _IteratorIn __beg,
	 _IteratorIn __end,
	 align_val_t __align)
		requires(
		 std::is_array_v<_Ty>
		 && is_iterator_v<_IteratorIn>
		 && std::is_convertible_v<typename _IteratorIn::value_t, std::remove_all_extents_t<_Ty>>)
	{
		const size_t c = countof(__beg, __end);
		pf_assert(c != 0, "Trying to create empty memory array!");
		return shared_ptr(
		 new_construct_aligned_at_ex<shared_store<_Ty>>(
			c * sizeof(std::remove_all_extents_t<_Ty>),
			__align,
			sizeof(size_t) + sizeof(__marray<std::remove_all_extents_t<_Ty>>),
			c,
			__beg,
			__end),
		 deleter_default<shared_store<_Ty>>());
	}
	template<typename _Ty>
	pf_hint_nodiscard pf_decl_constexpr shared_ptr<_Ty, deleter_default<shared_store<_Ty>>>
	make_shared_aligned(
	 initializer_list<std::remove_all_extents_t<_Ty>> __il,
	 align_val_t __align)
		requires(
		 std::is_array_v<_Ty>
		 && std::is_copy_constructible_v<std::remove_all_extents_t<_Ty>>)
	{
		const size_t c = __il.size();
		pf_assert(c != 0, "Trying to create empty memory array!");
		return shared_ptr(
		 new_construct_aligned_at_ex<shared_store<_Ty>>(
			c * sizeof(std::remove_all_extents_t<_Ty>),
			__align,
			sizeof(size_t) + sizeof(__marray<std::remove_all_extents_t<_Ty>>),
			__il),
		 deleter_default<shared_store<_Ty>>());
	}

	/// MEMORY: Shared -> Maker -> Allocator
	template<typename _Ty, typename _Allocator, typename... _Args>
	pf_hint_nodiscard pf_decl_constexpr shared_ptr<_Ty, deleter_allocator<shared_store<_Ty>, _Allocator>>
	make_shared(
	 _Allocator &__all,
	 _Args &&...__args)
		requires(
		 is_allocator_v<_Allocator>
		 && !std::is_array_v<_Ty>)
	{
		return shared_ptr(
		 new_construct_aligned_at<shared_store<_Ty>>(
			__all,
			alignof(_Ty),
			sizeof(size_t),
			std::forward<_Args>(__args)...),
		 deleter_allocator<shared_store<_Ty>, _Allocator>(&__all));
	}
	template<typename _Ty, typename _Allocator, typename... _Args>
	pf_hint_nodiscard pf_decl_constexpr shared_ptr<_Ty, deleter_allocator<shared_store<_Ty>, _Allocator>>
	make_shared_aligned(
	 _Allocator &__all,
	 align_val_t __align,
	 _Args &&...__args)
		requires(
		 is_allocator_v<_Allocator>
		 && !std::is_array_v<_Ty>)
	{
		return shared_ptr(
		 new_construct_aligned_at<shared_store<_Ty>>(
			__all,
			__align,
			sizeof(size_t),
			std::forward<_Args>(__args)...),
		 deleter_allocator<shared_store<_Ty>, _Allocator>(&__all));
	}
	template<typename _Ty, typename _Allocator, typename... _Args>
	pf_hint_nodiscard pf_decl_constexpr shared_ptr<_Ty, deleter_allocator<shared_store<_Ty>, _Allocator>>
	make_shared_aligned_at(
	 _Allocator &__all,
	 align_val_t __align,
	 size_t __offset,
	 _Args &&...__args)
		requires(
		 is_allocator_v<_Allocator>
		 && !std::is_array_v<_Ty>)
	{
		return shared_ptr(
		 new_construct_aligned_at<shared_store<_Ty>>(
			__all,
			__align,
			sizeof(size_t) + __offset,
			std::forward<_Args>(__args)...),
		 deleter_allocator<shared_store<_Ty>, _Allocator>(&__all));
	}

	/// MEMORY: Shared -> Maker -> Allocator -> Array
	template<typename _Ty, typename _Allocator>
	pf_hint_nodiscard pf_decl_constexpr shared_ptr<_Ty, deleter_allocator<shared_store<_Ty>, _Allocator>>
	make_shared(
	 _Allocator &__all,
	 size_t __count)
		requires(
		 is_allocator_v<_Allocator>
		 && std::is_array_v<_Ty>)
	{
		pf_assert(__count != 0, "Trying to create empty memory array!");
		return shared_ptr(
		 new_construct_aligned_at_ex<shared_store<_Ty>>(
			__all,
			__count * sizeof(std::remove_all_extents_t<_Ty>),
			pf_alignof(std::remove_all_extents_t<_Ty>),
			sizeof(size_t) + sizeof(__marray<std::remove_all_extents_t<_Ty>>),
			__count),
		 deleter_allocator<shared_store<_Ty>, _Allocator>(&__all));
	}
	template<typename _Ty, typename _Allocator, typename _IteratorIn>
	pf_hint_nodiscard pf_decl_constexpr shared_ptr<_Ty, deleter_allocator<shared_store<_Ty>, _Allocator>>
	make_shared(
	 _Allocator &__all,
	 _IteratorIn __beg,
	 _IteratorIn __end)
		requires(
		 is_allocator_v<_Allocator>
		 && std::is_array_v<_Ty>
		 && is_iterator_v<_IteratorIn>
		 && std::is_convertible_v<typename _IteratorIn::value_t, std::remove_all_extents_t<_Ty>>)
	{
		const size_t c = countof(__beg, __end);
		pf_assert(c != 0, "Trying to create empty memory array!");
		return shared_ptr(
		 new_construct_aligned_at_ex<shared_store<_Ty>>(
			__all,
			c * sizeof(std::remove_all_extents_t<_Ty>),
			pf_alignof(std::remove_all_extents_t<_Ty>),
			sizeof(size_t) + sizeof(__marray<std::remove_all_extents_t<_Ty>>),
			c,
			__beg,
			__end),
		 deleter_allocator<shared_store<_Ty>, _Allocator>(&__all));
	}
	template<typename _Ty, typename _Allocator>
	pf_hint_nodiscard pf_decl_constexpr shared_ptr<_Ty, deleter_allocator<shared_store<_Ty>, _Allocator>>
	make_shared(
	 _Allocator &__all,
	 initializer_list<std::remove_all_extents_t<_Ty>> __il)
		requires(
		 is_allocator_v<_Allocator>
		 && std::is_array_v<_Ty>)
	{
		const size_t c = __il.size();
		pf_assert(c != 0, "Trying to create empty memory array!");
		return shared_ptr(
		 new_construct_aligned_at_ex<shared_store<_Ty>>(
			__all,
			c * sizeof(std::remove_all_extents_t<_Ty>),
			pf_alignof(std::remove_all_extents_t<_Ty>),
			sizeof(size_t) + sizeof(__marray<std::remove_all_extents_t<_Ty>>),
			__il),
		 deleter_allocator<shared_store<_Ty>, _Allocator>(&__all));
	}
	template<typename _Ty, typename _Allocator>
	pf_hint_nodiscard pf_decl_constexpr shared_ptr<_Ty, deleter_allocator<shared_store<_Ty>, _Allocator>>
	make_shared_aligned(
	 _Allocator &__all,
	 size_t __count,
	 align_val_t __align)
		requires(
		 is_allocator_v<_Allocator>
		 && std::is_array_v<_Ty>)
	{
		pf_assert(__count != 0, "Trying to create empty memory array!");
		return shared_ptr(
		 new_construct_aligned_at_ex<shared_store<_Ty>>(
			__all,
			__count * sizeof(std::remove_all_extents_t<_Ty>),
			__align,
			sizeof(size_t) + sizeof(__marray<std::remove_all_extents_t<_Ty>>),
			__count),
		 deleter_allocator<shared_store<_Ty>, _Allocator>(&__all));
	}
	template<typename _Ty, typename _Allocator, typename _IteratorIn>
	pf_hint_nodiscard pf_decl_constexpr shared_ptr<_Ty, deleter_allocator<shared_store<_Ty>, _Allocator>>
	make_shared_aligned(
	 _Allocator &__all,
	 _IteratorIn __beg,
	 _IteratorIn __end,
	 align_val_t __align)
		requires(
		 is_allocator_v<_Allocator>
		 && std::is_array_v<_Ty>
		 && is_iterator_v<_IteratorIn>
		 && std::is_convertible_v<typename _IteratorIn::value_t, std::remove_all_extents_t<_Ty>>)
	{
		const size_t c = countof(__beg, __end);
		pf_assert(c != 0, "Trying to create empty memory array!");
		return shared_ptr(
		 new_construct_aligned_at_ex<shared_store<_Ty>>(
			__all,
			c * sizeof(std::remove_all_extents_t<_Ty>),
			__align,
			sizeof(size_t) + sizeof(__marray<std::remove_all_extents_t<_Ty>>),
			c,
			__beg,
			__end),
		 deleter_allocator<shared_store<_Ty>, _Allocator>(&__all));
	}
	template<typename _Ty, typename _Allocator>
	pf_hint_nodiscard pf_decl_constexpr shared_ptr<_Ty, deleter_allocator<shared_store<_Ty>, _Allocator>>
	make_shared_aligned(
	 _Allocator &__all,
	 initializer_list<std::remove_all_extents_t<_Ty>> __il,
	 align_val_t __align)
		requires(
		 is_allocator_v<_Allocator>
		 && std::is_array_v<_Ty>)
	{
		const size_t c = __il.size();
		pf_assert(c != 0, "Trying to create empty memory array!");
		return shared_ptr(
		 new_construct_aligned_at_ex<shared_store<_Ty>>(
			__all,
			c * sizeof(std::remove_all_extents_t<_Ty>),
			__align,
			sizeof(size_t) + sizeof(__marray<std::remove_all_extents_t<_Ty>>),
			__il),
		 deleter_allocator<shared_store<_Ty>, _Allocator>(&__all));
	}
}	 // namespace pul

#endif	// !PULSAR_MEMORY_HPP
