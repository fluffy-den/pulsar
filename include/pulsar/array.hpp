/*! @file   array.hpp
 *  @author Fluffy (noe.louis-quentin@hotmail.fr)
 *  @brief
 *  @date   11-09-2022
 *
 *  @copyright Copyright (c) 2022 - Pulsar Software
 *
 *  @since 0.1.2
 */

#ifndef PULSAR_ARRAY_HPP
#define PULSAR_ARRAY_HPP 1

// Include: Pulsar
#include "pulsar/memory.hpp"
#include "pulsar/pulsar.hpp"
#include "pulsar/iterator.hpp"
#include "pulsar/assert.hpp"
#include "pulsar/utility.hpp"

// Pulsar
namespace pul
{
	/// ARRAY:
	template <typename _Ty, size_t _Num>
	class array
	{
	public:
		using value_t									 = _Ty;
		using iterator_t							 = iterator<_Ty>;
		using const_iterator_t				 = iterator<const _Ty>;
		using reverse_iterator_t			 = reverse_iterator<_Ty>;
		using const_reverse_iterator_t = reverse_iterator<const _Ty>;

		/// Constructors
		pf_decl_constexpr
		array() pf_attr_noexcept
		requires(std::is_default_constructible_v<_Ty>)
		{}
		pf_decl_constexpr
		array(
			_Ty const &__val) pf_attr_noexcept
		requires(std::is_copy_constructible_v<_Ty>)
			: store_{__val}
		{}
		template <size_t _InNum>
		pf_decl_constexpr
		array(
			const _Ty (&__il)[_InNum])
		requires(_InNum == _Num)
			: store_(__il)
		{}
		pf_decl_constexpr
		array(
			array<_Ty, _Num> const &__r) pf_attr_noexcept
		requires(std::is_copy_constructible_v<_Ty>)
			: store_(__r.store_)
		{}
		pf_decl_constexpr
		array(
			array<_Ty, _Num> &&__r) pf_attr_noexcept
		requires(std::is_move_constructible_v<_Ty>)
			: store_(std::move(__r.store_))
		{}

		/// Operator=
		template <size_t _InNum>
		pf_decl_constexpr array<_Ty, _Num> &
		operator =(
			const _Ty (&__il)[_InNum])
		requires(
			std::is_copy_assignable_v<_Ty>
			&& _InNum == _Num)
		{
			for(auto sb = this->begin(), rb = iterator(__il[0]), se = this->end();
					sb != se;
					++sb, ++rb)
			{
				*sb = *this->rb;
			}
		}
		template <size_t _InNum>
		pf_decl_constexpr array<_Ty, _Num> &
		operator =(
			_Ty(&& __il)[_InNum])
		requires(
			std::is_move_assignable_v<_Ty>
			&& _InNum == _Num)
		{
			for(auto sb = this->begin(), rb = iterator(__il[0]), se = this->end();
					sb != se;
					++sb, ++rb)
			{
				*sb = std::move(*this->rb);
			}
		}
		pf_decl_constexpr array<_Ty, _Num> &
		operator =(
			array<_Ty, _Num> const &__r) pf_attr_noexcept
		requires(std::is_copy_assignable_v<_Ty>)
		{
			if(this == &__r) pf_hint_unlikely return *this;
			for(auto sb = this->begin(), rb = __r.begin(), se = this->end();
					sb != se;
					++sb, ++rb)
			{
				*sb = *this->rb;
			}
		}
		pf_decl_constexpr array<_Ty, _Num> &
		operator =(
			array<_Ty, _Num> &&__r) pf_attr_noexcept
		requires(std::is_move_assignable_v<_Ty>)
		{
			if(this == &__r) pf_hint_unlikely return *this;
			for(auto sb = this->begin(), rb = __r.begin(), se = this->end();
					sb != se;
					++sb, ++rb)
			{
				*sb = std::move(*this->rb);
			}
		}

		/// Operator ==
		pf_hint_nodiscard pf_decl_constexpr bool
		operator ==(
			array<_Ty, _Num> const &__r) pf_attr_noexcept
		{
			for(auto sb = this->begin(), rb = __r.begin(), se = this->end();
					sb != se;
					++sb, ++rb)
			{
				if(*sb != *rb)
					return false;
			}
			return true;
		}
		pf_hint_nodiscard pf_decl_constexpr bool
		operator !=(
			array<_Ty, _Num> const &__r) pf_attr_noexcept
		{
			return !(this == __r);
		}

		/// Operator[]
		pf_hint_nodiscard pf_decl_constexpr _Ty &
		operator [](
			size_t __index) pf_attr_noexcept
		{
			pf_assert(__index < _Num, u8"__index is out of sequence!");
			return this->store_[__index];
		}
		pf_hint_nodiscard pf_decl_constexpr const _Ty &
		operator [](
			size_t __index) const pf_attr_noexcept
		{
			pf_assert(__index < _Num, u8"__index is out of sequence!");
			return this->store_[__index];
		}

		/// Swap
		pf_decl_constexpr void swap(
			array<_Ty, _Num> &__r) pf_attr_noexcept
		{
			for(auto l = this->begin(), r = __r.begin(), e = this->end();
					l != e;
					++l)
			{
				swap(*l, *r);
			}
		}

		/// Accessors
		pf_hint_nodiscard pf_decl_constexpr _Ty *
		data() pf_attr_noexcept
		{
			return &this->store_[0];
		}
		pf_hint_nodiscard pf_decl_constexpr const _Ty *
		data() const pf_attr_noexcept
		{
			return &this->store_[0];
		}

		/// Iterator -> Begin
		pf_hint_nodiscard pf_decl_constexpr iterator_t
		begin() pf_attr_noexcept
		{
			return &this->store_[0];
		}
		pf_hint_nodiscard pf_decl_constexpr const_iterator_t
		begin() const pf_attr_noexcept
		{
			return &this->store_[0];
		}
		pf_hint_nodiscard pf_decl_constexpr const_iterator_t
		cbegin() const pf_attr_noexcept
		{
			return &this->store_[0];
		}

		/// Iterator -> End
		pf_hint_nodiscard pf_decl_constexpr iterator_t
		end() pf_attr_noexcept
		{
			return &this->store_[0] + _Num;
		}
		pf_hint_nodiscard pf_decl_constexpr const_iterator_t
		end() const pf_attr_noexcept
		{
			return &this->store_[0] + _Num;
		}
		pf_hint_nodiscard pf_decl_constexpr const_iterator_t
		cend() const pf_attr_noexcept
		{
			return &this->store_[0] + _Num;
		}

		/// Iterator -> Reverse Begin
		pf_hint_nodiscard pf_decl_constexpr iterator_t
		rbegin() pf_attr_noexcept
		{
			return &this->store_[0] + (_Num - 1);
		}
		pf_hint_nodiscard pf_decl_constexpr const_iterator_t
		rbegin() const pf_attr_noexcept
		{

			return &this->store_[0] + (_Num - 1);
		}
		pf_hint_nodiscard pf_decl_constexpr const_iterator_t
		crbegin() const pf_attr_noexcept
		{
			return &this->store_[0] + (_Num - 1);
		}

		/// Iterator -> Reverse End
		pf_hint_nodiscard pf_decl_constexpr iterator_t
		rend() pf_attr_noexcept
		{
			return &this->store_[0] - 1;
		}
		pf_hint_nodiscard pf_decl_constexpr const_iterator_t
		rend() const pf_attr_noexcept
		{
			return &this->store_[0] - 1;
		}
		pf_hint_nodiscard pf_decl_constexpr const_iterator_t
		crend() const pf_attr_noexcept
		{
			return &this->store_[0] - 1;
		}

		/// Memory
		pf_hint_nodiscard pf_decl_constexpr size_t
		size() const pf_attr_noexcept
		{
			return _Num * sizeof(_Ty);
		}
		pf_hint_nodiscard pf_decl_constexpr size_t
		count() const pf_attr_noexcept
		{
			return _Num;
		}

	private:
		_Ty store_[_Num];
	};

	/// ARRAY: Swap
	template <typename _Ty, size_t _Num>
	pf_decl_constexpr void
	swap(
		array<_Ty, _Num> &&__a1,
		array<_Ty, _Num> &&__a2) pf_attr_noexcept
	{
		__a1.swap(__a2);
	}

	/// ARRAY: Begin
	template <typename _Ty, size_t _Num>
	pf_hint_nodiscard pf_decl_constexpr auto
	begin(
		array<_Ty, _Num> &__a) pf_attr_noexcept
	{
		return __a.begin();
	}
	template <typename _Ty, size_t _Num>
	pf_hint_nodiscard pf_decl_constexpr auto
	begin(
		array<_Ty, _Num> const &__a) pf_attr_noexcept
	{
		return __a.begin();
	}

	/// ARRAY: End
	template <typename _Ty, size_t _Num>
	pf_hint_nodiscard pf_decl_constexpr auto
	end(
		array<_Ty, _Num> &__a) pf_attr_noexcept
	{
		return __a.end();
	}
	template <typename _Ty, size_t _Num>
	pf_hint_nodiscard pf_decl_constexpr auto
	end(
		array<_Ty, _Num> const &__a) pf_attr_noexcept
	{
		return __a.end();
	}

	/// ARRAY: Const Begin
	template <typename _Ty, size_t _Num>
	pf_hint_nodiscard pf_decl_constexpr auto
	cbegin(
		array<_Ty, _Num> const &__a) pf_attr_noexcept
	{
		return __a.cbegin();
	}

	/// ARRAY: Const End
	template <typename _Ty, size_t _Num>
	pf_hint_nodiscard pf_decl_constexpr auto
	cend(
		array<_Ty, _Num> const &__a) pf_attr_noexcept
	{
		return __a.cend();
	}

	/// ARRAY: Reverse Begin
	template <typename _Ty, size_t _Num>
	pf_hint_nodiscard pf_decl_constexpr auto
	rbegin(
		array<_Ty, _Num> &__a) pf_attr_noexcept
	{
		return __a.rbegin();
	}
	template <typename _Ty, size_t _Num>
	pf_hint_nodiscard pf_decl_constexpr auto
	rbegin(
		array<_Ty, _Num> const &__a) pf_attr_noexcept
	{
		return __a.rbegin();
	}

	/// ARRAY: Reverse End
	template <typename _Ty, size_t _Num>
	pf_hint_nodiscard pf_decl_constexpr auto
	crend(
		array<_Ty, _Num> const &__a) pf_attr_noexcept
	{
		return __a.crend();
	}

	/// ARRAY: Const Reverse Begin
	template <typename _Ty, size_t _Num>
	pf_hint_nodiscard pf_decl_constexpr auto
	crbegin(
		array<_Ty, _Num> const &__a) pf_attr_noexcept
	{
		return __a.crbegin();
	}

	/// ARRAY: Const Reverse End
	template <typename _Ty, size_t _Num>
	pf_hint_nodiscard pf_decl_constexpr auto
	crend(
		array<_Ty, _Num> const &__a) pf_attr_noexcept
	{
		return __a.crend();
	}

	/// ARRAY: Data
	template <typename _Ty, size_t _Num>
	pf_hint_nodiscard pf_decl_constexpr auto
	data(
		array<_Ty, _Num> &__a) pf_attr_noexcept
	{
		return __a.data();
	}
	template <typename _Ty, size_t _Num>
	pf_hint_nodiscard pf_decl_constexpr auto
	data(
		array<_Ty, _Num> const &__a) pf_attr_noexcept
	{
		return __a.data();
	}
}

#endif // !PULSAR_ARRAY_HPP