/*! @file   raii.hpp
 *  @author Louis-Quentin Noé (noe.louis-quentin@hotmail.fr)
 *  @brief
 *  @date   05-03-2023
 *
 *  @copyright Copyright (c) 2023 - Pulsar Software
 *
 *  @since 0.1.4
 */

#ifndef PULSAR_RAII_HPP
#define PULSAR_RAII_HPP 1

// Include: Pulsar
#include "pulsar/pulsar.hpp"
#include "pulsar/allocator.hpp"

// Pulsar
namespace pul
{
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
			destroy_delete(__ptr);
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
			destroy_delete_array(__ptr);
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
			destroy(__ptr);
			this->allocator_->deallocate(__ptr);
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
			_Ty *__ptr) const pf_attr_noexcept
		{
			(__ptr);
			this->allocator_->deallocate(__ptr, 1);
		}

	private:
		_Allocator *allocator_;
	};
}

#endif // !PULSAR_RAII_HPP