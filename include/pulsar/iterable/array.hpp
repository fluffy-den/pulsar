/*! @file   array.hpp
 *  @author Louis-Quentin Noé (noe.louis-quentin@hotmail.fr)
 *  @brief
 *  @date   17-02-2023
 *
 *  @copyright Copyright (c) 2023 - Pulsar Software
 *
 *  @since 0.1.4
 */

#ifndef PULSAR_ARRAY_HPP
#define PULSAR_ARRAY_HPP 1

// Include: Pulsar
#include "pulsar/iterable.hpp"

// Pulsar
namespace pul
{

	/// CONTAINER: Array
	template<typename _Ty, size_t _Num>
	class array
	{
	pf_assert_static(!std::is_const_v<_Ty>);

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
		array(const _Ty &__val) pf_attr_noexcept
		requires(std::is_copy_constructible_v<_Ty>)
			: store_{ __val }
		{}
		template<size_t _NumIn>
		pf_decl_constexpr
		array(const _Ty (&__arr)[_NumIn])
		requires(_NumIn <= _Num)
		{
			copy_forward(pul::begin(__arr), pul::end(__arr), this->begin());
		}
		template<size_t _NumIn>
		pf_decl_constexpr
		array(_Ty (&& __arr)[_NumIn])
		requires(_NumIn <= _Num)
		{
			copy_forward(make_move_iterator(pul::begin(__arr)), make_move_iterator(pul::end(__arr)), this->begin());
		}
		template<typename ... _Args>
		pf_decl_constexpr
		array(_Args &&... __args) pf_attr_noexcept
		requires(sizeof...(_Args) <= _Num && (std::is_constructible_v<_Ty, _Args> && ...))
			: store_{ std::forward<_Args>(__args)... }
		{}
		pf_decl_constexpr
		array(const array<_Ty, _Num> &__r) pf_attr_noexcept
		requires(std::is_copy_constructible_v<_Ty>)
			: store_(__r.store_)
		{}
		pf_decl_constexpr
		array(array<_Ty, _Num> &&__r) pf_attr_noexcept
		requires(std::is_move_constructible_v<_Ty>)
			: store_(std::move(__r.store_))
		{}

		/// Operator=
		pf_decl_constexpr array<_Ty, _Num> &
		operator=(const _Ty (&__arr)[_Num])
		requires(std::is_copy_assignable_v<_Ty>)
		{
			copy_forward(pul::begin(__arr), pul::end(__arr), this->begin());
			return *this;
		}
		pf_decl_constexpr array<_Ty, _Num> &
		operator=(_Ty (&& __arr)[_Num])
		requires(std::is_move_assignable_v<_Ty>)
		{
			copy_forward(make_move_iterator(pul::begin(__arr)), make_move_iterator(pul::end(__arr)), this->begin());
			return *this;
		}
		pf_decl_constexpr array<_Ty, _Num> &
		operator=(array<_Ty, _Num> const &__r) pf_attr_noexcept
		requires(std::is_copy_assignable_v<_Ty>)
		{
			if (pf_likely(this != &__r))
			{
				copy_forward(__r.begin(), __r.end(), this->begin());
			}
			return *this;
		}
		pf_decl_constexpr array<_Ty, _Num> &
		operator=(array<_Ty, _Num> &&__r) pf_attr_noexcept
		requires(std::is_move_assignable_v<_Ty>)
		{
			if (pf_likely(this != &__r))
			{
				copy_forward(make_move_iterator(__r.begin()), make_move_iterator(__r.end()), this->begin());
			}
			return *this;
		}
		template<typename ... _Args>
		pf_decl_constexpr array<_Ty, _Num> &
		operator=(_Args &&... __args) pf_attr_noexcept
		requires(sizeof...(_Args) == _Num && (std::is_constructible_v<_Ty, _Args> && ...))
		{
			this->store_ = { std::forward<_Args>(__args)... };
			return *this;
		}

		/// Operator ==
		pf_hint_nodiscard pf_decl_constexpr bool
		operator==(array<_Ty, _Num> const &__r) const pf_attr_noexcept
		{
			if(pf_likely(this != &__r))
			{
				return equal(this->begin(), this->end(), __r.begin());
			}
			return true;
		}
		pf_hint_nodiscard pf_decl_constexpr bool
		operator!=(const array<_Ty, _Num> &__r) const pf_attr_noexcept = default;

		/// Operator[]
		pf_hint_nodiscard pf_decl_constexpr _Ty &
		operator[](size_t __index) pf_attr_noexcept
		{
			return this->store_[__index];
		}
		pf_hint_nodiscard pf_decl_constexpr const _Ty &
		operator[](size_t __index) const pf_attr_noexcept
		{
			return this->store_[__index];
		}

		/// Swap
		pf_decl_constexpr void
		swap(array<_Ty, _Num> &__r) pf_attr_noexcept
		{
			for(auto l = this->begin(), r = __r.begin(), e = this->end(); l != e; ++l) { swap(*l, *r); }
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
		pf_hint_nodiscard pf_decl_constexpr reverse_iterator_t
		rbegin() pf_attr_noexcept
		{
			return &this->store_[0] + (_Num - 1);
		}
		pf_hint_nodiscard pf_decl_constexpr const_reverse_iterator_t
		rbegin() const pf_attr_noexcept
		{
			return &this->store_[0] + (_Num - 1);
		}
		pf_hint_nodiscard pf_decl_constexpr const_reverse_iterator_t
		crbegin() const pf_attr_noexcept
		{
			return &this->store_[0] + (_Num - 1);
		}

		/// Iterator -> Reverse End
		pf_hint_nodiscard pf_decl_constexpr reverse_iterator_t
		rend() pf_attr_noexcept
		{
			return &this->store_[0] - 1;
		}
		pf_hint_nodiscard pf_decl_constexpr const_reverse_iterator_t
		rend() const pf_attr_noexcept
		{
			return &this->store_[0] - 1;
		}
		pf_hint_nodiscard pf_decl_constexpr const_reverse_iterator_t
		crend() const pf_attr_noexcept
		{
			return &this->store_[0] - 1;
		}

		/// Accessors
		pf_hint_nodiscard pf_decl_constexpr _Ty*
		data() pf_attr_noexcept
		{
			return &this->store_[0];
		}
		pf_hint_nodiscard pf_decl_constexpr const _Ty*
		data() const pf_attr_noexcept
		{
			return &this->store_[0];
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

		/// Operator (value_t*)
		pf_hint_nodiscard pf_decl_explicit pf_decl_constexpr
		operator value_t* () pf_attr_noexcept
		{
			return this->data();
		}
		pf_hint_nodiscard pf_decl_explicit pf_decl_constexpr
		operator const value_t* () const pf_attr_noexcept
		{
			return this->data();
		}
	private:
		_Ty store_[_Num];
	};

	/// ARRAY: CTAD
	template<typename _Ty, size_t _Num>
	array(const _Ty (&__il)[_Num])->array<_Ty, _Num>;
	template<typename _Ty, size_t _Num>
	array(_Ty (&& __il)[_Num])->array<_Ty, _Num>;
	template<typename _Ty, typename ... _Args>
	array(const _Ty &__arg, _Args && ... __args)->array<_Ty, sizeof...(_Args)>;

	/// ARRAY: Test
	pf_assert_static(is_iterable_v<array<int32_t, 6>>);
}


#endif // !PULSAR_ARRAY_HPP