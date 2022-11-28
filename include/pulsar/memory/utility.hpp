/*! @file   utility.hpp
 *  @author Louis-Quentin NOE (noe.louis-quentin@hotmail.fr)
 *  @brief
 *  @date   14-11-2022
 *
 *  @copyright Copyright (c) 2022 - Pulsar Software
 *
 *  @since 0.1.2
 */

#ifndef PULSAR_MEMORY_UTILITY_HPP
#define PULSAR_MEMORY_UTILITY_HPP 1

// Include: Pulsar
#include "pulsar/utility.hpp"
#include "pulsar/assert.hpp"

// Include: Pulsar -> Memory
#include "pulsar/memory/allocator.hpp"

// Pulsar
namespace pul
{
	/// UTILITY: Memory -> Constants
	pf_decl_constexpr align_val_t ALIGN_DEFAULT = align_val_t(alignof(void *));
	pf_decl_constexpr align_val_t ALIGN_MAX			= align_val_t(1024);

	/// UTILITY: Memory
	/*! @brief
	 *
	 *  @param[in] __val
	 *  @return pf_hint_nodiscard
	 */
	pf_hint_nodiscard pf_decl_constexpr bool
	is_power_of_two(
		size_t __val) pf_attr_noexcept
	{
		return (__val & (__val - 1)) == 0;
	}
	/*! @brief Calculates the offset of a memory sequence starting at address @a __addr needed to
	 * 				 align the sequence to the value of @a __align.
	 *
	 *  @param[in] __addr	 Memory address of the beginning of the sequence.
	 *  @param[in] __align Desired alignment of the memory.
	 *
	 *  @return 0 if @a __align is equal to 0.
	 * 				  The offset required to align the memory sequence otherwise.
	 */
	pf_hint_nodiscard pf_decl_constexpr size_t
	paddingof(
		size_t __addr,
		align_val_t __align) pf_attr_noexcept
	{
		// TODO: pf_assert(is_power_of_two(static_cast<size_t>(__align)), u8"__align isn't power of two!");
		if(__align == align_val_t(0))
			return 0;
		return (static_cast<size_t>(__align) - (__addr & (static_cast<size_t>(__align) - 1))) & (static_cast<size_t>(__align) - 1);
	}
	/*! @brief Convert a pointer to a memory address.
	 *
	 *  @param[in] __ptr Pointer to cast.
	 *  @return Memory address of @a __ptr.
	 */
	pf_hint_nodiscard pf_decl_constexpr size_t
	addressof(
		const void *__ptr) pf_attr_noexcept
	{
		return union_cast<size_t>(__ptr);
	}
	/*! @brief Convert a memory address to a type _Ty pointer.
	 *
	 *  @tparam _Ty Type of pointer.
	 *
	 *  @param[in] __addr Memory address.
	 * 	@return _Ty pointer pointing to @a __addr.
	 */
	template <typename _Ty>
	pf_hint_nodiscard pf_decl_constexpr _Ty *
	addrtoptr(
		size_t __addr) pf_attr_noexcept
	{
		return union_cast<_Ty *>(__addr);
	}
	/*! @brief Convert a memory address to a type const _Ty pointer.
	 *
	 *  @tparam _Ty Type of pointer.
	 *
	 *  @param[in] __addr Memory address.
	 *  @return const _Ty pointer pointing to @a __addr.
	 */
	template <typename _Ty>
	pf_hint_nodiscard pf_decl_constexpr const _Ty *
	addrtocptr(
		size_t __addr) pf_attr_noexcept
	{
		return union_cast<const _Ty *>(__addr);
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
	pf_hint_nodiscard pf_decl_constexpr void *
	align_top(
		void *__ptr,
		align_val_t __align = ALIGN_MAX,
		size_t __offset			= 0) pf_attr_noexcept
	{
		// TODO: pf_assert(__ptr, u8"__ptr is nullptr!");
		// TODO: pf_assert(is_power_of_two(static_cast<size_t>(__align)), u8"__align isn't power of two!");
		byte_t *p = union_cast<byte_t *>(__ptr);
		p += paddingof(union_cast<size_t>(p + __offset), __align);
		return p;
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
	pf_hint_nodiscard pf_decl_constexpr bool
	is_aligned(
		const void *__ptr,
		align_val_t __align,
		size_t __offset = 0) pf_attr_noexcept
	{
		// TODO: pf_assert(__ptr, u8"__ptr is nullptr!");
		return paddingof(union_cast<size_t>(__ptr) + __offset, __align) == 0;
	}

	/// UTILITY: Placement Construct
	/*! @brief
	 *
	 *  @tparam _Ty
	 *  @tparam _Args
	 */
	template <typename _Ty, typename ..._Args>
	pf_decl_constexpr void
	construct_at(
		_Ty *__ptr,
		_Args && ... __args)
	requires(std::is_constructible_v<_Ty, _Args ...>)
	{
		pf_assert(__ptr, u8"__ptr is nullptr!");
		new(__ptr )_Ty (std::forward<_Args>(__args)...);
	}

	template <typename _Ty>
	struct memory_array_t
	{
		size_t count;
		_Ty first;
	};

	template <typename _Ty, typename ..._Args>
	pf_decl_constexpr void
	construct_array_at(
		memory_array_t<_Ty> *__ptr,
		size_t __num)
	requires(std::is_copy_constructible_v<_Ty>)
	{
		pf_assert(__ptr, u8"__ptr is nullptr!");
		__ptr->count = __num;
		for(_Ty *b = &__ptr->first, *e = b + __num;
				b != e;
				++b)
		{
			construct_at(b);
		}
	}
	template <typename _Ty, typename ..._Args>
	pf_decl_constexpr void
	construct_array_at(
		memory_array_t<_Ty> *__ptr,
		size_t __num,
		_Ty const &__val)
	requires(std::is_copy_constructible_v<_Ty>)
	{
		pf_assert(__ptr, u8"__ptr is nullptr!");
		__ptr->count = __num;
		for(_Ty *b = &__ptr->first, *e = b + __num;
				b != e;
				++b)
		{
			construct_at(b, __val);
		}
	}
	template <typename _Ty, typename ..._Args>
	pf_decl_constexpr void
	construct_array_at(
		memory_array_t<_Ty> *__ptr,
		size_t __num,
		initializer_list<_Ty> const &__il)
	requires(std::is_copy_constructible_v<_Ty>)
	{
		pf_assert(__il.size() <= __num, u8"Array out of bound!");
		pf_assert(__ptr, u8"__ptr is nullptr!");
		__ptr->count = __num;
		const _Ty *i = __il.begin();
		for(_Ty *b = &__ptr->first, *e = b + __num;
				b != e;
				++b, ++i)
		{
			construct_at(b, *i);
		}
	}

	/// UTILITY: Placement Destroy
	/*! @brief
	 *
	 *  @tparam _Ty
	 *  @param[in] __ptr
	 *  @return pf_decl_inline
	 */
	template <typename _Ty>
	pf_decl_constexpr void
	destroy_at(
		_Ty *__ptr) pf_attr_noexcept
	{
		pf_assert(__ptr, u8"__ptr is nullptr!");
		__ptr->~_Ty();
	}
	/*! @brief
	 *
	 *  @tparam _Ty
	 *  @param[in] __ptr
	 *  @return pf_decl_inline
	 */
	template <typename _Ty>
	pf_decl_constexpr void
	destroy_array_at(
		memory_array_t<_Ty> *__ptr) pf_attr_noexcept
	{
		pf_assert(__ptr, u8"__ptr is nullptr!");
		_Ty *b = &__ptr->first;
		_Ty *e = b + __ptr->count;
		while(b != e)
		{
			destroy_at(b);
			++b;
		}
	}

	/// UTILITY: Construct -> Extended
	template <typename _Ty, typename ..._Args>
	pf_decl_constexpr _Ty *
	construct_new_aligned_ex(
		size_t __exBytes,
		align_val_t __align,
		_Args && ... __args) pf_attr_noexcept
	requires(std::is_constructible_v<_Ty, _Args ...>)
	{
		pf_assert(is_power_of_two(static_cast<size_t>(__align)), u8"__align isn't power of two!");
		_Ty *p;
		if(std::is_constant_evaluated())
		{
			p = union_cast<_Ty *>(::operator new(sizeof(_Ty) + __exBytes));
		}
		else
		{
			p = union_cast<_Ty *>(
				mi_malloc_aligned(
					sizeof(_Ty) + __exBytes,
					static_cast<size_t>(__align)));
		}
		construct_at(p, std::forward<_Args>(__args)...);
		return p;
	}
	template <typename _Ty, typename ..._Args>
	pf_decl_constexpr _Ty *
	construct_new_ex(
		size_t __exBytes,
		_Args && ... __args) pf_attr_noexcept
	requires(std::is_constructible_v<_Ty, _Args ...>)
	{
		return construct_new_aligned_ex<_Ty>(
			__exBytes,
			align_val_t(alignof(_Ty)),
			std::forward<_Args>(__args)...);
	}
	template <typename _Ty, typename ..._Args>
	pf_decl_constexpr _Ty *
	construct_new_aligned_at_ex(
		size_t __exBytes,
		align_val_t __align,
		size_t __offset,
		_Args && ... __args) pf_attr_noexcept
	requires(std::is_constructible_v<_Ty, _Args ...>)
	{
		pf_assert(is_power_of_two(static_cast<size_t>(__align)), u8"__align isn't power of two!");
		pf_assert(__offset <= sizeof(_Ty) + __exBytes, u8"__offset is greater than allocation size!");
		_Ty *p;
		if(std::is_constant_evaluated())
		{
			p = union_cast<_Ty *>(::operator new(sizeof(_Ty) + __exBytes));
		}
		else
		{
			p = union_cast<_Ty *>(
				mi_malloc_aligned_at(
					sizeof(_Ty) + __exBytes,
					static_cast<size_t>(__align),
					__offset));
		}
		construct_at(p, std::forward<_Args>(__args)...);
		return p;
	}

	/// UTILITY: Construct -> Allocator -> Extended
	template <typename _Ty, typename _Allocator, typename ..._Args>
	pf_decl_constexpr _Ty *
	construct_new_aligned_ex(
		_Allocator &__all,
		size_t __exBytes,
		align_val_t __align,
		_Args && ... __args) pf_attr_noexcept
	requires(is_allocator_v<_Allocator>)
	{
		pf_assert(is_power_of_two(static_cast<size_t>(__align)), u8"__align isn't power of two!");
		auto *p = union_cast<_Ty *>(
			__all.allocate(sizeof(_Ty) + __exBytes,
										 static_cast<size_t>(__align)));
		construct_at(p, std::forward<_Args>(__args)...);
		return p;
	}
	template <typename _Ty, typename _Allocator, typename ..._Args>
	pf_decl_constexpr _Ty *
	construct_new_ex(
		_Allocator &__all,
		size_t __exBytes,
		_Args && ... __args) pf_attr_noexcept
	requires(is_allocator_v<_Allocator>)
	{
		return construct_new_aligned_ex(
			__all,
			__exBytes,
			align_val_t(alignof(_Ty)),
			std::forward<_Args>(__args)...);
	}
	template <typename _Ty, typename _Allocator, typename ..._Args>
	pf_decl_constexpr _Ty *
	construct_new_aligned_at_ex(
		_Allocator &__all,
		size_t __exBytes,
		align_val_t __align,
		size_t __offset,
		_Args && ... __args) pf_attr_noexcept
	requires(is_allocator_v<_Allocator>)
	{
		pf_assert(is_power_of_two(static_cast<size_t>(__align)), u8"__align isn't power of two!");
		pf_assert(__offset <= sizeof(_Ty) + __exBytes, u8"__offset is greater than allocation size!");
		auto *p = union_cast<_Ty *>(
			__all.allocate(
				sizeof(_Ty) + __exBytes,
				static_cast<size_t>(__align),
				__offset));
		construct_at(p, std::forward<_Args>(__args)...);
		return p;
	}

	/// UTILITY: Construct
	template <typename _Ty, typename ..._Args>
	pf_decl_constexpr _Ty *
	construct_new(
		_Args && ... __args) pf_attr_noexcept
	requires(std::is_constructible_v<_Ty, _Args ...>)
	{
		return construct_new_ex<_Ty>(0, std::forward<_Args>(__args)...);
	}
	template <typename _Ty, typename ..._Args>
	pf_decl_constexpr _Ty *
	construct_new_aligned(
		align_val_t __align,
		_Args && ... __args) pf_attr_noexcept
	requires(std::is_constructible_v<_Ty, _Args ...>)
	{
		return construct_new_aligned_ex<_Ty>(0, __align, std::forward<_Args>(__args)...);
	}
	template <typename _Ty, typename ..._Args>
	pf_decl_constexpr _Ty *
	construct_new_aligned_at(
		align_val_t __align,
		size_t __offset,
		_Args && ... __args) pf_attr_noexcept
	requires(std::is_constructible_v<_Ty, _Args ...>)
	{
		return construct_new_aligned_at_ex<_Ty>(0, __align, __offset, std::forward<_Args>(__args)...);
	}

	/// UTILITY: Construct -> Allocator
	template <typename _Ty, typename _Allocator, typename ..._Args>
	pf_decl_constexpr _Ty *
	construct_new(
		_Allocator &__all,
		_Args && ... __args) pf_attr_noexcept
	requires(is_allocator_v<_Allocator>)
	{
		return construct_new_ex<_Ty>(__all, 0, std::forward<_Args>(__args)...);
	}
	template <typename _Ty, typename _Allocator, typename ..._Args>
	pf_decl_constexpr _Ty *
	construct_new_aligned(
		_Allocator &__all,
		align_val_t __align,
		_Args && ... __args) pf_attr_noexcept
	requires(is_allocator_v<_Allocator>)
	{
		return construct_new_aligned_ex<_Ty>(__all, 0, __align, std::forward<_Args>(__args)...);
	}
	template <typename _Ty, typename _Allocator, typename ..._Args>
	pf_decl_constexpr _Ty *
	construct_new_aligned_at(
		_Allocator &__all,
		align_val_t __align,
		size_t __offset,
		_Args && ... __args) pf_attr_noexcept
	requires(is_allocator_v<_Allocator>)
	{
		return construct_new_aligned_at_ex<_Ty>(__all, 0, __align, __offset, std::forward<_Args>(__args)...);
	}

	/// UTILITY: Construct -> Array
	template <typename _Ty, typename ..._Args>
	pf_decl_constexpr _Ty *
	construct_new_aligned_array(
		align_val_t __align,
		size_t __num,
		_Args && ... __args) pf_attr_noexcept
	requires(std::is_array_v<_Ty> &&
					 (std::is_invocable_v<construct_array_at<_Ty, _Args ...>, memory_array_t<_Ty> *, size_t, _Args ...>))
	{
		pf_assert(is_power_of_two(static_cast<size_t>(__align)), u8"__align isn't power of two!");
		pf_assert(__num != 0, u8"__num is null!");
		if(std::is_constant_evaluated())
		{
			ignore = __align;
			return new _Ty[__num];
		}
		else
		{
			auto *p = union_cast<memory_array_t<_Ty> *>(
				mi_malloc_aligned_at(
					sizeof(size_t) + sizeof(_Ty) * __num,
					static_cast<size_t>(__align),
					sizeof(size_t)));
			construct_array_at(p, __num, std::forward<_Args>(__args)...);
			return &p->first;
		}
	}
	template <typename _Ty, typename ..._Args>
	pf_decl_constexpr _Ty *
	construct_new_array(
		size_t __num,
		_Args && ... __args) pf_attr_noexcept
	requires(std::is_array_v<_Ty> &&
					 (std::is_invocable_v<construct_array_at<_Ty, _Args ...>, memory_array_t<_Ty> *, size_t, _Args ...>))
	{
		return construct_new_aligned_array<_Ty>(
			align_val_t(alignof(_Ty)),
			__num,
			std::forward<_Args>(__args)...);
	}

	/// UTILITY: Construct -> Allocator -> Array
	template <typename _Ty, typename _Allocator, typename ..._Args>
	pf_decl_constexpr _Ty *
	construct_new_aligned_array(
		_Allocator &&__all,
		align_val_t __align,
		size_t __num,
		_Args && ... __args) pf_attr_noexcept
	requires(is_allocator_v<_Allocator> &&
					 std::is_array_v<_Ty> &&
					 (std::is_invocable_v<construct_array_at<_Ty, _Args ...>, memory_array_t<_Ty> *, size_t, _Args ...>))
	{
		pf_assert(is_power_of_two(static_cast<size_t>(__align)), u8"__align isn't power of two!");
		pf_assert(__num != 0, u8"__num is null!");
		auto *p = union_cast<memory_array_t<_Ty> *>(
			__all.allocate(
				sizeof(size_t) + sizeof(_Ty) * __num,
				static_cast<size_t>(__align),
				sizeof(size_t)));
		construct_array_at(p, __num, std::forward<_Args>(__args)...);
		return &p->first;
	}
	template <typename _Ty, typename _Allocator, typename ..._Args>
	pf_decl_constexpr _Ty *
	construct_new_array(
		_Allocator &&__all,
		size_t __num,
		_Args && ... __args) pf_attr_noexcept
	requires(is_allocator_v<_Allocator> &&
					 std::is_array_v<_Ty> &&
					 (std::is_invocable_v<construct_array_at<_Ty, _Args ...>, memory_array_t<_Ty> *, size_t, _Args ...>))
	{
		return construct_new_aligned_array<_Ty>(
			__all,
			align_val_t(alignof(_Ty)),
			__num,
			std::forward<_Args>(__args)...);
	}


	/// UTILITY: Destroy
	template <typename _Ty>
	pf_decl_constexpr void
	destroy_delete(
		_Ty *__ptr) pf_attr_noexcept
	{
		pf_assert(__ptr, u8"__ptr is nullptr!");
		destroy_at(__ptr);
		if(std::is_constant_evaluated())
		{
			delete __ptr;
		}
		else
		{
			mi_free(__ptr);
		}
	}

	/// UTILITY: -> Destroy Array
	template <typename _Ty>
	pf_decl_constexpr void
	destroy_delete_array(
		_Ty *__ptr) pf_attr_noexcept
	{
		pf_assert(__ptr, u8"__ptr is nullptr!");
		if(std::is_constant_evaluated())
		{
			delete[] __ptr;
		}
		else
		{
			destroy_array_at(__ptr);
			union
			{
				_Ty *as_type;
				size_t *as_size;
			};
			as_type = __ptr;
			--as_size;
			mi_free(as_type);
		}
	}

	/// UTILITY: Deleter -> Default
	template <typename _Ty>
	struct deleter_default
	{
		/// Operator()
		pf_decl_constexpr void
		operator ()(
			_Ty *__ptr) pf_attr_noexcept
		{
			destroy_delete(__ptr);
		}
	};

	/// UTILITY: Deleter -> Default Array
	template <typename _Ty>
	struct deleter_default<_Ty[]>
	{
		/// Operator()
		pf_decl_constexpr void
		operator ()(
			_Ty *__ptr) pf_attr_noexcept
		{
			destroy_delete_array(__ptr);
		}
	};

	/// UTILITY: Deleter -> Allocator
	template <typename _Ty, typename _Allocator>
	struct deleter_allocator
	{
		/// Constructor
		pf_decl_constexpr
		deleter_allocator(
			_Allocator *__all) pf_attr_noexcept
			: allocator_(__all)
		{}

		/// Operator()
		pf_decl_constexpr void
		operator ()(
			_Ty *__ptr) const pf_attr_noexcept
		{
			destroy_at(__ptr);
			this->allocator_.deallocate(__ptr);
		}

	private:
		_Allocator *allocator_;
	};

	/// UTILITY: Magnifier
	struct magnifier_default
	{
		/// Operator()
		pf_hint_nodiscard pf_decl_constexpr size_t
		operator ()(
			size_t __rs) const pf_attr_noexcept
		{
			return __rs;
		}
	};
}

#endif // !PULSAR_MEMORY_UTILITY_HPP