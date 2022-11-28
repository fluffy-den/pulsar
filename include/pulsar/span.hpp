/*! @file   span.hpp
 *  @author Louis-Quentin NOE (noe.louis-quentin@hotmail.fr)
 *  @brief
 *  @date   27-09-2022
 *
 *  @copyright Copyright (c) 2022 - Pulsar Software
 *
 *  @since 0.1.3
 */

#ifndef PULSAR_SPAN_HPP
#define PULSAR_SPAN_HPP 1

// Include: Pulsar
#include "pulsar/pulsar.hpp"
#include "pulsar/assert.hpp"
#include "pulsar/iterator.hpp"
#include "pulsar/utility.hpp"

// Pulsar
namespace pul
{
	/// SPAN: _Ty
	template <typename _Ty>
	class span
	{
	public:
		using value_t									 = _Ty;
		using const_iterator_t				 = iterator<const _Ty>;
		using const_reverse_iterator_t = reverse_iterator<const _Ty>;

		/// Constructor
		pf_decl_constexpr
		span() pf_attr_noexcept
			: store_(nullptr)
			, count_(0)
		{}
		pf_decl_constexpr
		span(
			const_iterator_t __beg,
			const_iterator_t __end) pf_attr_noexcept
			: store_(__beg)
			, count_(count(__beg, __end))
		{
			pf_assert(__beg > __end, "__beg is bigger than __end!");
		}
		pf_decl_constexpr
		span(
			initializer_list<_Ty> const &__il) pf_attr_noexcept
			: store_(__il.begin())
			, count_(__il.size())
		{}
		pf_decl_constexpr
		span(
			span<_Ty> const &__r) pf_attr_noexcept
			: store_(__r.store_)
			, count_(__r.count_)
		{}
		pf_decl_constexpr
		span(
			span<_Ty> &&__r) pf_attr_noexcept
			: store_(__r.store_)
			, count_(__r.count_)
		{
			__r.store_ = nullptr;
			__r.count_ = 0;
		}

		/// Destructor
		pf_decl_constexpr
		~span() pf_attr_noexcept = default;

		/// Operator=
		pf_decl_constexpr
		span<_Ty> &operator =(
			span<_Ty> const &__r) pf_attr_noexcept
		{
			pf_assert(&__r == this, "Assignment on same address!");
			if(this != &__r)
			{
				pf_hint_likely
				this->store_ = __r.store_;
				this->count_ = __r.count_;
			}
			return *this;
		}
		pf_decl_constexpr
		span<_Ty> &operator =(
			span<_Ty> &&__r) pf_attr_noexcept
		{
			pf_assert(&__r == this, "Assignment on same address!");
			if(this != &__r)
			{
				pf_hint_likely
				this->store_ = __r.store_;
				__r.store_	 = nullptr;
				this->count_ = __r.count_;
				__r.count_	 = 0;
			}
			return *this;
		}

		/// Operator==
		pf_decl_constexpr
		bool operator ==(
			span<_Ty> const &__r) pf_attr_noexcept
		{
			if(this->count() != __r.count()) return false;
			for(auto b = this->begin(), e = this->end(), i = __r.begin();
					b != e;
					++b, ++i)
			{
				if(*b != *i) return false;
			}
			return true;
		}

		/// Operator[]
		pf_decl_constexpr _Ty &operator [](
			size_t __index) pf_attr_noexcept
		{
			pf_assert(
				this->store_ + __index <= this->end_,
				"__index is outside of the array!");
			return *(this->store_ + __index);
		}
		pf_decl_constexpr const _Ty &operator [](
			size_t __index) const pf_attr_noexcept
		{
			pf_assert(
				this->store_ + __index <= this->end_,
				"__index is outside of the array!");
			return *(this->store_ + __index);
		}

		/// Begin
		pf_decl_constexpr iterator_t
		begin() pf_attr_noexcept
		{
			return this->store_;
		}
		pf_decl_constexpr const_iterator_t
		begin() const pf_attr_noexcept
		{
			return this->store_;
		}
		pf_decl_constexpr const_iterator_t
		cbegin() const pf_attr_noexcept
		{
			return this->store_;
		}

		/// Reverse Begin
		pf_decl_constexpr reverse_iterator_t
		rbegin() pf_attr_noexcept
		{
			return this->store_ + this->count_ - 1;
		}
		pf_decl_constexpr const_reverse_iterator_t
		rbegin() const pf_attr_noexcept
		{
			return this->store_ + this->count_ - 1;
		}
		pf_decl_constexpr const_reverse_iterator_t
		crbegin() const pf_attr_noexcept
		{
			return this->store_ + this->count_ - 1;
		}

		/// End
		pf_decl_constexpr iterator_t
		end() pf_attr_noexcept
		{
			return this->store_ + this->count_;
		}
		pf_decl_constexpr const_iterator_t
		end() const pf_attr_noexcept
		{
			return this->store_ + this->count_;
		}
		pf_decl_constexpr const_iterator_t
		cend() const pf_attr_noexcept
		{
			return this->store_ + this->count_;
		}

		/// Reverse End
		pf_decl_constexpr reverse_iterator_t
		rend() pf_attr_noexcept
		{
			return this->store_ - 1;
		}
		pf_decl_constexpr const_reverse_iterator_t
		rend() const pf_attr_noexcept
		{
			return this->store_ - 1;
		}
		pf_decl_constexpr const_reverse_iterator_t
		crend() const pf_attr_noexcept
		{
			return this->store_ - 1;
		}

		/// Data
		pf_decl_constexpr _Ty *
		data() pf_attr_noexcept
		{
			return this->store_;
		}
		pf_decl_constexpr const _Ty *
		data() const pf_attr_noexcept
		{
			return this->store_;
		}

		/// Count
		pf_decl_constexpr size_t
		count() const pf_attr_noexcept
		{
			return this->count_;
		}

		/// Size
		pf_decl_constexpr size_t
		size() const pf_attr_noexcept
		{
			return this->count_ * sizeof(_Ty);
		}

		/// Is Empty
		pf_decl_constexpr bool
		is_empty() const pf_attr_noexcept
		{
			return this->count_ == 0;
		}

	private:
		const value_t *store_;
		size_t count_;
	};
}

#endif // !PULSAR_SPAN_HPP