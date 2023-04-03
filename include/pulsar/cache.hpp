/*! @file   cache.hpp
 *  @author Louis-Quentin Noé (noe.louis-quentin@hotmail.fr)
 *  @brief
 *  @date   31-03-2023
 *
 *  @copyright Copyright (c) 2023 - Pulsar Software
 *
 *  @since 0.1.5
 */

#ifndef PULSAR_CACHE_HPP
#define PULSAR_CACHE_HPP 1

// Include: Pulsar
#include "pulsar/pulsar.hpp"

// Pulsar
namespace pul
{
	/// CACHE: Alloc / Free
	pf_hint_nodiscard pulsar_api void*
	calloc(
		size_t __size,
		align_val_t __align = ALIGN_DEFAULT,
		size_t __offset			= 0) pf_attr_noexcept;
	pulsar_api void
	cfree(
		void *__ptr) pf_attr_noexcept;

	/// CACHE: Memory
	template <
		typename _Ty,
		typename ... _Args>
	pf_hint_nodiscard _Ty*
	cnew_construct_ex(
		size_t __exBytes,
		_Args && ... __args)
	{
		_Ty *ptr = union_cast<_Ty*>(calloc(sizeof(_Ty) + __exBytes));
		new(ptr) _Ty(std::forward<_Args>(__args)...);
		return ptr;
	}
	template <
		typename _Ty,
		typename ... _Args>
	pf_hint_nodiscard _Ty*
	cnew_construct(
		_Args && ... __args)
	{
		return cnew_construct_ex<_Ty>(0, std::forward<_Args>(__args)...);
	}

	template <
		typename _Ty>
	void cdestroy_delete(
		_Ty *__p) pf_attr_noexcept
	{
		__p->~_Ty();
		cfree(__p);
	}
}

#endif // !PULSAR_CACHE_HPP