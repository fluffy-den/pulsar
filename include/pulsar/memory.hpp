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

// Include: Pulsar
#include "pulsar/pulsar.hpp"
#include "pulsar/heap.hpp"
#include "pulsar/iterator.hpp"
#include "pulsar/utility.hpp"

// Include: C++
#include <concepts>

// Include: C
#include <cstring>// memcpy

// Pulsar
namespace pul
{
	/// MEMORY: Allocator
	/// ALLOCATOR: Concept
	template <typename _Allocator>
	concept __allocator_c = requires(
		_Allocator & __all)
	{
		{ __all.allocate(std::declval<size_t>(), std::declval<align_val_t>(), std::declval<size_t>()) }->std::convertible_to<void*>;
		{ __all.reallocate(std::declval<void*>(), std::declval<size_t>(), std::declval<align_val_t>(), std::declval<size_t>()) }->std::convertible_to<void*>;
		__all.deallocate(std::declval<void*>());
	};
	template <typename _Allocator>
	struct is_allocator : std::false_type
	{};
	template <__allocator_c _Allocator>
	struct is_allocator<_Allocator> : std::true_type
	{};
	template <typename _Allocator>
	pf_decl_static pf_decl_constexpr bool is_allocator_v = is_allocator<_Allocator>::value;

	/// ALLOCATOR: Allocate
	template <typename _Allocator>
	pf_hint_nodiscard pf_decl_inline pf_decl_constexpr void*
	allocate(
		_Allocator &__all,
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

	/// ALLOCATOR: Reallocate
	template <typename _Allocator>
	pf_decl_inline pf_decl_constexpr void*
	reallocate(
		_Allocator &__all,
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
			return __all.reallocate(__ptr, __nsize, __align, __offset);
		}
	}

	/// ALLOCATOR: Deallocate
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
	pf_hint_nodiscard pf_decl_inline pf_decl_constexpr void*
	align_top(
		void *__ptr,
		align_val_t __align,
		size_t __offset = 0) pf_attr_noexcept
	{
		byte_t *p = union_cast<byte_t*>(__ptr);
		p += paddingof(addressof(p + __offset), __align);
		return p;
	}
	pf_hint_nodiscard pf_decl_inline pf_decl_constexpr size_t
	__base2_mod(
		size_t __x,
		size_t __y) pf_attr_noexcept
	{
		return __y & (__y - 1) ? __x % __y : __x & (__y - 1);
	}
	pf_hint_nodiscard pf_decl_inline pf_decl_constexpr bool
	is_aligned(
		const void *__ptr,
		align_val_t __align,
		size_t __offset = 0)
	{
		return __base2_mod(addressof(__ptr) + __offset, union_cast<size_t>(__align)) == 0;
	}

	/// MEMORY: Utility -> Array Type
	template <typename _Ty>
	struct memory_array
	{
		// Flexible Arrays -> Disable warning
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"

		size_t count;
		_Ty data[];

		// Flexible Arrays
#pragma GCC diagnostic pop
	};
	using memory_array_t = memory_array<byte_t>;

	/// MEMORY: Utility -> Construct
	template<typename _Ty, typename ... _Args>
	pf_decl_inline pf_decl_constexpr void
	construct(
		_Ty *__ptr,
		_Args &&... __args)
	requires(std::is_constructible_v<_Ty, _Args...>)
	{
		// pf_assert(__ptr, "__ptr is nullptr!"); // TODO: Exception
		new(__ptr) _Ty(std::forward<_Args>(__args)...);
	}
	template <typename _IteratorIn>
	pf_decl_inline pf_decl_constexpr void
	construct(
		_IteratorIn __beg,
		_IteratorIn __end)
	requires(is_iterator_v<_IteratorIn>
					 && std::is_default_constructible_v<typename _IteratorIn::value_t>)
	{
		for (; __beg != __end; ++__beg) construct(__beg.get());
	}
	template <typename _IteratorIn>
	pf_decl_inline pf_decl_constexpr void
	construct(
		_IteratorIn __beg,
		_IteratorIn __end,
		typename _IteratorIn::value_t const &__val)
	requires(is_iterator_v<_IteratorIn>
					 && std::is_copy_constructible_v<typename _IteratorIn::value_t>)
	{
		for (; __beg != __end; ++__beg) construct(__beg.get(), __val);
	}
	template <typename _Iterator, typename _IteratorIn>
	pf_decl_inline pf_decl_constexpr void
	construct(
		_Iterator __ab,
		_Iterator __ae,
		_IteratorIn __bb)
	requires(is_iterator_v<_Iterator>
					 && is_iterator_v<_IteratorIn>
					 && std::is_convertible_v<typename _IteratorIn::value_t, typename _Iterator::value_t>)
	{
		for (; __ab != __ae; ++__ab, ++__bb) construct(__ab.get(), *__bb);
	}


	/// MEMORY: Utility -> Assign
	template <typename _Ty, typename ... _Args>
	pf_decl_inline pf_decl_constexpr void
	assign(
		_Ty *__ptr,
		_Args&&... __args) pf_attr_noexcept
	requires(std::is_assignable_v<_Ty, _Args...>)
	{
		*__ptr = { std::forward<_Args>(__args) ... };
	}
	template <typename _Ty, typename ... _Args>
	pf_decl_inline pf_decl_constexpr void
	assign(
		_Ty *__ptr,
		_Args&& ... __args) pf_attr_noexcept
	requires(!std::is_assignable_v<_Ty, _Args...>
					 && std::is_constructible_v<_Ty, _Args...>)
	{
		destroy(__ptr);
		*__ptr = { std::forward<_Args>(__args) ... };
	}


	/// MEMORY: Utility -> Destroy
	template<typename _Ty>
	pf_decl_inline pf_decl_constexpr void
	destroy(
		_Ty *__ptr) pf_attr_noexcept
	{
		__ptr->~_Ty();
	}
	template <typename _IteratorIn>
	pf_decl_inline pf_decl_constexpr void
	destroy(
		_IteratorIn __beg,
		_IteratorIn __end) pf_attr_noexcept
	requires(is_iterator_v<_IteratorIn>)
	{
		for (; __beg != __end; ++__beg) destroy(__beg.get());
	}
	template <typename _Ty>
	pf_decl_inline pf_decl_constexpr void
	destroy(
		memory_array<_Ty> *__ptr) pf_attr_noexcept
	{
		destroy(iterator(&__ptr->data[0]), iterator(&__ptr->data[0] + __ptr->count));
	}
	template <typename _Ty>
	pf_decl_inline pf_decl_constexpr void
	destroy_array(
		_Ty *__ptr) pf_attr_noexcept
	{
		union
		{
			memory_array<_Ty> *as_array;
			_Ty *as_type;
		};
		as_type = __ptr;
		--as_array;
		destroy(iterator(&as_array->data[0]),
						iterator(&as_array->data[0] + as_array->count));
	}


	/// MEMORY: New Construct
	template <typename _Ty, typename ... _Args>
	pf_decl_inline pf_decl_constexpr _Ty*
	new_construct_aligned_at_ex(
		size_t __exBytes,
		align_val_t __align,
		size_t __offset,
		_Args&& ... __args)
	requires(std::is_constructible_v<_Ty, _Args...>)
	{
		_Ty *p = union_cast<_Ty*>(
			halloc(sizeof(_Ty) + __exBytes, __align, __offset));
		if (pf_unlikely(!p)) pf_throw(
				dbg_category_generic(), errv::bad_alloc, dbg_flags::dump_with_handle_data,
				"Failed to create new object of type={}, size={}, align={}, offset={} with heap memory.",
				typeid(_Ty).name(), sizeof(_Ty) + __exBytes, union_cast<size_t>(__align), __offset);
		construct(p, std::forward<_Args>(__args)...);
		return p;
	}
	template <typename _Ty, typename ... _Args>
	pf_decl_inline pf_decl_constexpr _Ty*
	new_construct_aligned_ex(
		size_t __exBytes,
		align_val_t __align,
		_Args&& ... __args)
	requires(std::is_constructible_v<_Ty, _Args...>)
	{
		return new_construct_aligned_at_ex<_Ty>(
			__exBytes,
			__align,
			0,
			std::forward<_Args>(__args)...);
	}
	template <typename _Ty, typename ... _Args>
	pf_decl_inline pf_decl_constexpr _Ty*
	new_construct_ex(
		size_t __exBytes,
		_Args&& ... __args)
	requires(std::is_constructible_v<_Ty, _Args...>)
	{
		return new_construct_aligned_at_ex<_Ty>(
			__exBytes,
			align_val_t(alignof(_Ty)),
			0,
			std::forward<_Args>(__args)...);
	}
	template <typename _Ty, typename _Allocator, typename ... _Args>
	pf_decl_inline pf_decl_constexpr _Ty*
	new_construct_aligned_at_ex(
		_Allocator  &__all,
		align_val_t __align,
		size_t __offset,
		size_t __exBytes,
		_Args&& ... __args)
	requires(is_allocator_v<_Allocator>
					 && std::is_constructible_v<_Ty, _Args...>)
	{
		_Ty *p = union_cast<_Ty*>(
			allocate<_Ty>(__all, sizeof(_Ty) + __exBytes, __align, __offset));
		if (pf_unlikely(!p)) pf_throw(
				dbg_category_generic(), errv::bad_alloc, dbg_flags::dump_with_handle_data,
				"Failed to create new object of type={}, size={}, align={}, offset={} with allocator={}.",
				typeid(_Ty).name(), sizeof(_Ty) + __exBytes, union_cast<size_t>(__align), __offset, union_cast<void*>(&__all));
		construct(p, std::forward<_Args>(__args)...);
		return p;
	}
	template <typename _Ty, typename _Allocator, typename ... _Args>
	pf_decl_inline pf_decl_constexpr _Ty*
	new_construct_aligned_ex(
		_Allocator  &__all,
		align_val_t __align,
		size_t __exBytes,
		_Args&& ... __args)
	requires(is_allocator_v<_Allocator>
					 && std::is_constructible_v<_Ty, _Args...>)
	{
		return new_construct_aligned_at_ex<_Ty>(
			__all,
			0,
			__align,
			0,
			std::forward<_Args>(__args)...);
	}
	template <typename _Ty, typename _Allocator, typename ... _Args>
	pf_decl_inline pf_decl_constexpr _Ty*
	new_construct_ex(
		_Allocator  &__all,
		size_t __exBytes,
		_Args&& ... __args)
	requires(is_allocator_v<_Allocator>
					 && std::is_constructible_v<_Ty, _Args...>)
	{
		return new_construct_aligned_at_ex<_Ty>(
			__all,
			0,
			align_val_t(alignof(_Ty)),
			0,
			std::forward<_Args>(__args)...);
	}
	template <typename _Ty, typename ... _Args>
	pf_decl_inline pf_decl_constexpr _Ty*
	new_construct_aligned_at(
		align_val_t __align,
		size_t __offset,
		_Args&& ... __args)
	requires(std::is_constructible_v<_Ty, _Args...>)
	{
		return new_construct_aligned_at_ex<_Ty>(0, __align, __offset, std::forward<_Args>(__args)...);
	}
	template <typename _Ty, typename ... _Args>
	pf_decl_inline pf_decl_constexpr _Ty*
	new_construct_aligned(
		align_val_t __align,
		_Args&& ... __args)
	requires(std::is_constructible_v<_Ty, _Args...>)
	{
		return new_construct_aligned_ex<_Ty>(0, __align, std::forward<_Args>(__args)...);
	}
	template <typename _Ty, typename ... _Args>
	pf_decl_inline pf_decl_constexpr _Ty*
	new_construct(
		_Args&& ... __args)
	requires(std::is_constructible_v<_Ty, _Args...>)
	{
		return new_construct_ex<_Ty>(0, std::forward<_Args>(__args)...);
	}

	template <typename _Ty, typename _Allocator, typename ... _Args>
	pf_decl_inline pf_decl_constexpr _Ty*
	new_construct_aligned_at(
		_Allocator  &__all,
		align_val_t __align,
		size_t __offset,
		_Args&& ... __args)
	requires(is_allocator_v<_Allocator>
					 && std::is_constructible_v<_Ty, _Args...>)
	{
		return new_construct_aligned_at_ex<_Ty>(__all, 0, __align, __offset, std::forward<_Args>(__args)...);
	}
	template <typename _Ty, typename _Allocator, typename ... _Args>
	pf_decl_inline pf_decl_constexpr _Ty*
	new_construct_aligned(
		_Allocator  &__all,
		align_val_t __align,
		_Args&& ... __args)
	requires(is_allocator_v<_Allocator>
					 && std::is_constructible_v<_Ty, _Args...>)
	{
		return new_construct_aligned_ex<_Ty>(__all, 0, __align, std::forward<_Args>(__args)...);
	}
	template <typename _Ty, typename _Allocator, typename ... _Args>
	pf_decl_inline pf_decl_constexpr _Ty*
	new_construct(
		_Allocator  &__all,
		_Args&& ... __args)
	requires(is_allocator_v<_Allocator>
					 && std::is_constructible_v<_Ty, _Args...>)
	{
		return new_construct_ex<_Ty>(__all, 0, std::forward<_Args>(__args)...);
	}

	/// MEMORY: Utility -> Destroy Delete
	template<typename _Ty>
	pf_decl_inline pf_decl_constexpr void
	destroy_delete(
		_Ty *__ptr) pf_attr_noexcept
	{
		destroy(__ptr);
		hfree(__ptr);
	}
	template<typename _Ty, typename _Allocator>
	pf_decl_inline pf_decl_constexpr void
	destroy_delete(
		_Allocator &__all,
		_Ty *__ptr) pf_attr_noexcept
	requires(is_allocator_v<_Allocator>)
	{
		destroy(__ptr);
		deallocate(__all, __ptr);
	}


	/// MEMORY: Utility -> New Construct Array
	template <typename _Ty>
	pf_decl_inline pf_decl_constexpr _Ty*
	new_construct_array_aligned_at(
		size_t __count,
		align_val_t __align,
		size_t __offset)
	requires(std::is_default_constructible_v<_Ty>)
	{
		union
		{
			memory_array<_Ty> *as_array;
			size_t *as_size;
		};
		as_array = union_cast<memory_array<_Ty>*>(
			halloc(sizeof(size_t) + sizeof(_Ty) * __count, __align, sizeof(size_t) + __offset));
		as_array->count = __count;
		construct(iterator(&as_array->data[0]), iterator(&as_array->data[0] + __count));
		return &as_array->data[0];
	}
	template <typename _Ty>
	pf_decl_inline pf_decl_constexpr _Ty*
	new_construct_array_aligned(
		size_t __count,
		align_val_t __align)
	requires(std::is_default_constructible_v<_Ty>)
	{
		return new_construct_array_aligned_at<_Ty>(__count, __align, 0);
	}
	template <typename _Ty>
	pf_decl_inline pf_decl_constexpr _Ty*
	new_construct_array(
		size_t __count)
	requires(std::is_default_constructible_v<_Ty>)
	{
		return new_construct_array_aligned_at<_Ty>(__count, align_val_t(alignof(_Ty)), 0);
	}
	template <typename _Ty, typename _Allocator>
	pf_decl_inline pf_decl_constexpr _Ty*
	new_construct_array_aligned_at(
		_Allocator  &__all,
		size_t __count,
		align_val_t __align,
		size_t __offset)
	requires(is_allocator_v<_Allocator>
					 && std::is_default_constructible_v<_Ty>)
	{
		union
		{
			memory_array<_Ty> *as_array;
			size_t *as_size;
		};
		as_array = union_cast<memory_array<_Ty>*>(
			halloc(__all, sizeof(size_t) + sizeof(_Ty) * __count, __align, sizeof(size_t) + __offset));
		as_array->count = __count;
		construct(iterator(&as_array->data[0]), iterator(&as_array->data[0] + __count));
		return &as_array->data[0];
	}
	template <typename _Ty, typename _Allocator>
	pf_decl_inline pf_decl_constexpr _Ty*
	new_construct_array_aligned(
		_Allocator  &__all,
		size_t __count,
		align_val_t __align)
	requires(is_allocator_v<_Allocator>
					 && std::is_default_constructible_v<_Ty>)
	{
		return new_construct_array_aligned_at<_Ty>(__all, __count, __align, 0);
	}
	template <typename _Ty, typename _Allocator>
	pf_decl_inline pf_decl_constexpr _Ty*
	new_construct_array(
		_Allocator &__all,
		size_t __count)
	requires(is_allocator_v<_Allocator>
					 && std::is_default_constructible_v<_Ty>)
	{
		return new_construct_array_aligned_at<_Ty>(__all, __count, align_val_t(alignof(_Ty)), 0);
	}

	template <typename _Ty>
	pf_decl_inline pf_decl_constexpr _Ty*
	new_construct_array_aligned_at(
		size_t __count,
		align_val_t __align,
		size_t __offset,
		_Ty const   &__val)
	requires(std::is_copy_constructible_v<_Ty>)
	{
		union
		{
			memory_array<_Ty> *as_array;
			size_t *as_size;
		};
		as_array = union_cast<memory_array<_Ty>*>(
			halloc(sizeof(size_t) + sizeof(_Ty) * __count, __align, sizeof(size_t) + __offset));
		as_array->count = __count;
		construct(iterator(&as_array->data[0]), iterator(&as_array->data[0] + __count), __val);
		return &as_array->data[0];
	}
	template <typename _Ty>
	pf_decl_inline pf_decl_constexpr _Ty*
	new_construct_array_aligned(
		size_t __count,
		align_val_t __align,
		_Ty const   &__val)
	requires(std::is_copy_constructible_v<_Ty>)
	{
		return new_construct_array_aligned_at<_Ty>(__count, __align, 0, __val);
	}
	template <typename _Ty>
	pf_decl_inline pf_decl_constexpr _Ty*
	new_construct_array(
		size_t __count,
		_Ty const &__val)
	requires(std::is_copy_constructible_v<_Ty>)
	{
		return new_construct_array_aligned_at<_Ty>(__count, align_val_t(alignof(_Ty)), 0, __val);
	}

	template <typename _Ty, typename _Allocator>
	pf_decl_inline pf_decl_constexpr _Ty*
	new_construct_array_aligned_at(
		_Allocator  &__all,
		size_t __count,
		align_val_t __align,
		size_t __offset,
		_Ty const   &__val)
	requires(is_allocator_v<_Allocator>
					 && std::is_copy_constructible_v<_Ty>)
	{
		union
		{
			memory_array<_Ty> *as_array;
			size_t *as_size;
		};
		as_array = union_cast<memory_array<_Ty>*>(
			halloc(__all, sizeof(_Ty) * __count, __align, sizeof(size_t) + __offset));
		as_array->count = __count;
		construct(iterator(&as_array->data[0]), iterator(&as_array->data[0] + __count), __val);
		return &as_array->data[0];
	}
	template <typename _Ty, typename _Allocator>
	pf_decl_inline pf_decl_constexpr _Ty*
	new_construct_array_aligned(
		_Allocator  &__all,
		size_t __count,
		align_val_t __align,
		_Ty const   &__val)
	requires(is_allocator_v<_Allocator>
					 && std::is_copy_constructible_v<_Ty>)
	{
		return new_construct_array_aligned_at<_Ty>(__all, __count, __align, 0, __val);
	}
	template <typename _Ty, typename _Allocator>
	pf_decl_inline pf_decl_constexpr _Ty*
	new_construct_array(
		_Allocator &__all,
		size_t __count,
		_Ty const  &__val)
	requires(is_allocator_v<_Allocator>
					 && std::is_copy_constructible_v<_Ty>)
	{
		return new_construct_array_aligned_at<_Ty>(__all, __count, align_val_t(alignof(_Ty)), 0, __val);
	}

	template <typename _IteratorIn>
	pf_decl_inline pf_decl_constexpr typename _IteratorIn::value_t*
	new_construct_array_aligned_at(
		_IteratorIn __beg,
		_IteratorIn __end,
		align_val_t __align,
		size_t __offset)
	requires(std::is_copy_constructible_v<typename _IteratorIn::value_t>)
	{
		const size_t c = countof(__beg, __end);
		union
		{
			memory_array<typename _IteratorIn::value_t> *as_array;
			size_t *as_size;
		};
		as_array = union_cast<memory_array<typename _IteratorIn::value_t>*>(
			halloc(sizeof(typename _IteratorIn::value_t) * c, __align, sizeof(size_t) + __offset));
		as_array->count = c;
		construct(iterator(&as_array->data[0]), iterator(&as_array->data[0] + c), __beg);
		return &as_array->data[0];
	}
	template <typename _IteratorIn>
	pf_decl_inline pf_decl_constexpr typename _IteratorIn::value_t*
	new_construct_array_aligned(
		_IteratorIn __beg,
		_IteratorIn __end,
		align_val_t __align)
	requires(std::is_copy_constructible_v<typename _IteratorIn::value_t>)
	{
		return new_construct_array_aligned_at(
			__beg,
			__end,
			__align,
			0);
	}
	template <typename _IteratorIn>
	pf_decl_inline pf_decl_constexpr typename _IteratorIn::value_t*
	new_construct_array(
		_IteratorIn __beg,
		_IteratorIn __end)
	requires(std::is_copy_constructible_v<typename _IteratorIn::value_t>)
	{
		return new_construct_array_aligned_at(
			__beg,
			__end,
			align_val_t(alignof(typename _IteratorIn::value_t)),
			0);
	}
	template <typename _IteratorIn, typename _Allocator>
	pf_decl_inline pf_decl_constexpr typename _IteratorIn::value_t*
	new_construct_array_aligned_at(
		_Allocator  &__all,
		_IteratorIn __beg,
		_IteratorIn __end,
		align_val_t __align,
		size_t __offset)
	requires(is_allocator_v<_Allocator>
					 && std::is_copy_constructible_v<typename _IteratorIn::value_t>)
	{
		const size_t c = countof(__beg, __end);
		union
		{
			memory_array<typename _IteratorIn::value_t> *as_array;
			size_t *as_size;
		};
		as_array = union_cast<memory_array<typename _IteratorIn::value_t>*>(
			halloc(__all, sizeof(typename _IteratorIn::value_t) * c, __align, sizeof(size_t) + __offset));
		as_array->count = c;
		construct(iterator(&as_array->data[0]), iterator(&as_array->data[0] + c), __beg);
		return &as_array->data[0];
	}
	template <typename _IteratorIn, typename _Allocator>
	pf_decl_inline pf_decl_constexpr typename _IteratorIn::value_t*
	new_construct_array_aligned(
		_Allocator  &__all,
		_IteratorIn __beg,
		_IteratorIn __end,
		align_val_t __align)
	requires(is_allocator_v<_Allocator>
					 && std::is_copy_constructible_v<typename _IteratorIn::value_t>)
	{
		return new_construct_array_aligned_at(
			__all,
			__beg,
			__end,
			__align,
			0);
	}
	template <typename _IteratorIn, typename _Allocator>
	pf_decl_inline pf_decl_constexpr typename _IteratorIn::value_t*
	new_construct_array(
		_Allocator  &__all,
		_IteratorIn __beg,
		_IteratorIn __end)
	requires(is_allocator_v<_Allocator>
					 && std::is_copy_constructible_v<typename _IteratorIn::value_t>)
	{
		return new_construct_array_aligned_at(
			__all,
			__beg,
			__end,
			std::align_val_t(alignof(typename _IteratorIn::value_t)),
			0);
	}


	/// MEMORY: Utility -> Destroy Delete
	template<typename _Ty>
	pf_decl_inline pf_decl_constexpr void
	destroy_delete_array(
		_Ty *__ptr) pf_attr_noexcept
	{
		destroy_array(__ptr);
		memory_array<_Ty> *as_array = union_cast<memory_array<_Ty>*>(__ptr);
		hfree(--as_array);
	}
	template<typename _Ty, typename _Allocator>
	pf_decl_inline pf_decl_constexpr void
	destroy_delete_array(
		_Allocator &__all,
		_Ty *__ptr) pf_attr_noexcept
	requires(is_allocator_v<_Allocator>)
	{
		destroy_array(__ptr);
		memory_array<_Ty> *as_array = union_cast<memory_array<_Ty>*>(__ptr);
		deallocate(__all, --as_array);
	}

}

#endif // !PULSAR_MEMORY_HPP