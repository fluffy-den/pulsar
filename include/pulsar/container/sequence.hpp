/*! @file   sequence.hpp
 *  @author Louis-Quentin Noé (noe.louis-quentin@hotmail.fr)
 *  @brief
 *  @date   02-01-2023
 *
 *  @copyright Copyright (c) 2023 - Pulsar Software
 *
 *  @since 0.1.3
 */

#ifndef PULSAR_SEQUENCE_HPP
#define PULSAR_SEQUENCE_HPP 1

// Include: Pulsar
#include "pulsar/container.hpp"

// Pulsar
namespace pul
{
	/// SEQUENCE: Types
	template <
		typename _Ty>
	class sequence_view;
	template <
		typename _Ty,
		typename _Magnifier = magnifier_default,
		typename _Allocator = allocator_default>
	class sequence;

	/// SEQUENCE: View
	template <
		typename _Ty>
	class sequence_view
	{
	pf_static_assert(!std::is_const_v<_Ty>, "_Ty mustn't be a const type!");

	public:
		using value_t									 = const _Ty;
		using const_iterator_t				 = const_iterator<_Ty>;
		using const_reverse_iterator_t = reverse_iterator<const_iterator_t>;

		/// Constructors
		pf_decl_inline pf_decl_constexpr sequence_view() pf_attr_noexcept
			: begin_(nullptr)
			, count_(0)
		{}
		pf_decl_inline pf_decl_constexpr sequence_view(nullptr_t) pf_attr_noexcept
			: sequence_view()
		{}
		pf_decl_inline pf_decl_constexpr sequence_view(
			const _Ty *__begin,
			size_t __count) pf_attr_noexcept
			: begin_(__begin)
			, count_(__count)
		{}
		template <typename _Iterable>
		pf_decl_inline pf_decl_constexpr sequence_view(
			_Iterable const &__ia) pf_attr_noexcept
		requires(is_iterable_v<_Iterable>
						 && std::is_same_v<typename _Iterable::value_t, value_t>
						 && is_sequence_iterator_v<typename _Iterable::const_iterator_t>)
			: begin_(__ia.begin())
			, count_(__ia.count())
		{}
		sequence_view(sequence_view<_Ty> const &) = default;
		sequence_view(sequence_view<_Ty> &&)			= default;

		/// Destructor
		~sequence_view() default;

		/// Begin
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const_iterator_t
		begin() const pf_attr_noexcept
		{
			return this->begin_;
		}
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const_iterator_t
		cbegin() const pf_attr_noexcept
		{
			return this->begin_;
		}

		/// End
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const_iterator_t
		end() const pf_attr_noexcept
		{
			return this->begin_ + this->count_;
		}
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const_iterator_t
		cend() const pf_attr_noexcept
		{
			return this->begin_ + this->count_;
		}

		/// Reverse Begin
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const_reverse_iterator_t
		rbegin() const pf_attr_noexcept
		{
			return this->begin_ + (this->count_ - 1);
		}
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const_reverse_iterator_t
		crbegin() const pf_attr_noexcept
		{
			return this->begin_ + (this->count_ - 1);
		}

		/// Reverse End
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const_reverse_iterator_t
		rend() const pf_attr_noexcept
		{
			return this->end_ - 1;
		}
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const_reverse_iterator_t
		crend() const pf_attr_noexcept
		{
			return this->end_ - 1;
		}

		/// Data
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr value_t*
		data() const pf_attr_noexcept
		{
			return this->begin_;
		}

		/// Size
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr size_t
		size() const pf_attr_noexcept
		{
			return this->count_ * sizeof(_Ty);
		}

		/// Count
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr size_t
		count() const pf_attr_noexcept
		{
			return this->count_;
		}

		/// Operator =
		sequence_view<_Ty> &
		operator=(
			sequence_view<_Ty> const &) = default;
		sequence_view<_Ty> &
		operator=(
			sequence_view<_Ty> &&) = default;
		template <typename _Iterable>
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr sequence_view<_Ty> &
		operator=(
			_Iterable const &__ia) pf_attr_noexcept
		requires(is_iterable_v<_Iterable>
						 && std::is_same_v<typename _Iterable::value_t, value_t>
						 && is_sequence_iterator_v<typename _Iterable::const_iterator_t>)
		{
			this->begin_ = __ia.begin();
			this->count_ = __ia.count();
			return *this;
		}

		/// Operator ==
		template <typename _Iterable>
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr bool
		operator ==(
			_Iterable const &__ia) const pf_attr_noexcept
		requires(is_iterable_v<_Iterable>
						 && std::is_same_v<typename _Iterable::value_t, value_t>
						 && is_sequence_iterator_v<typename _Iterable::const_iterator_t>)
		{
			if (this->count_ != __ia.count()) return false;
			for (auto b = this->begin(), e = this->end(), ib = __ia.begin(); b != e; ++b, ++ib)
			{
				if(*b != *ib) return false;
			}
			return true;
		}
		template <typename _Iterable>
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr bool
		operator !=(
			_Iterable const &__ia) const pf_attr_noexcept
		requires(is_iterable_v<_Iterable>
						 && std::is_same_v<typename _Iterable::value_t, value_t>
						 && is_sequence_iterator_v<typename _Iterable::const_iterator_t>)
		{
			return !(*this == __ia);
		}

		/// Operator (value_t*)
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr
		operator value_t*() pf_attr_noexcept
		{
			return this->begin_;
		}

		/// Operator (Bool)
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr
		operator bool() pf_attr_noexcept
		{
			return this->begin_;
		}

	private:
		value_t *begin_;
		size_t count_;
	};

	/// SEQUENCE:
	template <
		typename _Ty,
		typename _Magnifier,
		typename _Allocator>
	class sequence// TODO: Modify!
	{
	/// Reallocate
	pf_decl_constexpr bool __reallocate_no_check(
		size_t __s)
	{
		const size_t cs = this->count();
		const size_t cm = __s / sizeof(_Ty);
		bool r					= false;
		if(std::is_constant_evaluated())
		{
			if(!this->store_)
			{
				this->store_ = union_cast<_Ty*>(new _Ty[cm]);
			}
			else
			{
				_Ty *ptr = union_cast<_Ty*>(new _Ty[cm]);
				std::memmove(ptr, this->store_, std::min(cs, cm) * sizeof(_Ty));
				delete[] this->store_;
				this->store_ = ptr;
				r						 = true;
			}
		}
		else
		{
			if(!this->store_)
			{
				this->store_ = union_cast<_Ty*>(this->allocator_.allocate(__s, this->memAlign_, 0));
			}
			else
			{
				_Ty *p = this->store_;
				this->store_ = union_cast<_Ty*>(this->allocator_.reallocate(this->store_, __s, this->memAlign_, 0));
				r						 = (p == this->store_);
			}
		}
		this->count_		= cs;
		this->memCount_ = cm;
		return r;
	}
	pf_decl_constexpr bool __reallocate(
		size_t __s)
	{
		__s = this->magnifier_(__s);
		if(__s > this->capacity())
		{
			return this->__reallocate_no_check(__s);
		}
		return false;
	}
	pf_decl_constexpr bool __reallocate(
		size_t __s,
		align_val_t __na)
	{
		if(__na != this->memAlign_)
		{
			this->memAlign_ = __na;
			return this->__reallocate_no_check(__s);
		}
		else
		{
			return this->__reallocate(__s);
		}
		return false;
	}

	/// Resize
	pf_decl_constexpr void __resize_default_construct(
		size_t __c)
	{
		const size_t c = this->count_;
		if(c > __c) pf_hint_unlikely return;
		__c -= c;
		_Ty *se = this->store_ + this->count_;
		_Ty *e	= se + __c;
		this->count_ += countof(se, e);
		while(se != e)
		{
			construct_at(se);
			++se;
		}
	}
	pf_decl_constexpr void __resize_copy_construct(
		size_t __c,
		_Ty const &__val)
	{
		const size_t c = this->count_;
		if(c > __c) pf_hint_unlikely return;
		__c -= c;
		_Ty *se = this->store_ + this->count_;
		_Ty *e	= se + __c;
		this->count_ += countof(se, e);
		while(se != e)
		{
			construct_at(se, __val);
			++se;
		}
	}

	public:
		using value_t									 = _Ty;
		using iterator_t							 = iterator<_Ty>;
		using const_iterator_t				 = iterator<const _Ty>;
		using reverse_iterator_t			 = reverse_iterator<_Ty>;
		using const_reverse_iterator_t = reverse_iterator<const _Ty>;
		using magnifier_t							 = _Magnifier;
		using allocator_t							 = _Allocator;

		/// Constructors
		pf_decl_constexpr
		sequence(
			align_val_t __align			 = union_cast<align_val_t>(alignof(_Ty)),
			_Magnifier &&__magnifier = magnifier_default(),
			_Allocator &&__allocator = allocator_malloc()) pf_attr_noexcept
			: store_(nullptr)
			, count_(0)
			, memCount_(0)
			, memAlign_(__align)
			, magnifier_(std::move(__magnifier))
			, allocator_(std::move(__allocator))
		{}
		pf_decl_constexpr pf_decl_explicit
		sequence(
			size_t __n,
			_Ty const &__value,
			align_val_t __align			 = union_cast<align_val_t>(alignof(_Ty)),
			_Magnifier &&__magnifier = magnifier_default(),
			_Allocator &&__allocator = allocator_malloc()) pf_attr_noexcept
		requires(std::is_copy_constructible_v<_Ty>)
			: sequence(
				__align,
				std::move(__magnifier),
				std::move(__allocator))
		{
			this->resize(__n, __align, __value);
		}
		template <typename _InIteratorTy>
		pf_decl_constexpr
		sequence(
			_InIteratorTy __beg,
			_InIteratorTy __end,
			align_val_t __align			 = union_cast<align_val_t>(alignof(_Ty)),
			_Magnifier &&__magnifier = magnifier_default(),
			_Allocator &&__allocator = allocator_malloc())
		requires(
			std::is_copy_constructible_v<_Ty> &&
			std::is_same_v<std::remove_cvref_t<decltype(*std::declval<_InIteratorTy>())>, _Ty>)
			: sequence(
				__align,
				std::move(__magnifier),
				std::move(__allocator))
		{
			this->insert_back(__beg, __end);
		}
		pf_decl_constexpr
		sequence(
			initializer_list<_Ty> __il,
			align_val_t __align			 = union_cast<align_val_t>(alignof(_Ty)),
			_Magnifier &&__magnifier = magnifier_default(),
			_Allocator &&__allocator = allocator_malloc())
			: sequence(
				__il.begin(),
				__il.end(),
				__align,
				std::move(__magnifier),
				std::move(__allocator))
		{}
		pf_decl_constexpr
		sequence(
			sequence<_Ty, _Magnifier, _Allocator> const &__r)
		requires(std::is_copy_constructible_v<_Ty>)
			: store_(nullptr)
			, count_(0)
			, memCount_(0)
			, memAlign_(__r.memAlign_)
			, magnifier_(__r.magnifier_)
			, allocator_(__r.allocator_)
		{
			this->insert_back(__r.begin(), __r.end());
		}
		pf_decl_constexpr
		sequence(
			sequence<_Ty, _Magnifier, _Allocator> const &__r,
			align_val_t __align)
		requires(std::is_copy_constructible_v<_Ty>)
			: store_(nullptr)
			, count_(0)
			, memCount_(0)
			, memAlign_(__align)
			, magnifier_(__r.magnifier_)
			, allocator_(__r.allocator_)
		{
			this->insert_back(__r.begin(), __r.end());
		}
		pf_decl_constexpr
		sequence(
			sequence<_Ty, _Magnifier, _Allocator> const &__r,
			align_val_t __align,
			_Magnifier &&__magnifier,
			_Allocator &&__allocator = allocator_malloc())
		requires(std::is_copy_constructible_v<_Ty>)
			: store_(nullptr)
			, count_(0)
			, memCount_(0)
			, memAlign_(__align)
			, magnifier_(std::move(__magnifier))
			, allocator_(std::move(__allocator))
		{
			this->insert_back(__r.begin(), __r.end());
		}
		pf_decl_constexpr
		sequence(
			sequence<_Ty, _Magnifier, _Allocator> &&__r) pf_attr_noexcept
			: store_(__r.store_)
			, count_(__r.count_)
			, memCount_(__r.memCount_)
			, memAlign_(__r.memAlign_)
			, magnifier_(std::move(__r.magnifier_))
			, allocator_(std::move(__r.allocator_))
		{
			__r.store_		= nullptr;
			__r.count_		= 0;
			__r.memCount_ = 0;
		}
		pf_decl_constexpr
		sequence(
			sequence<_Ty, _Magnifier, _Allocator> &&__r,
			align_val_t __align) pf_attr_noexcept
			: store_(__r.store_)
			, count_(__r.count_)
			, memCount_(__r.memCount_)
			, memAlign_(__r.memAlign_)
			, magnifier_(std::move(__r.magnifier_))
			, allocator_(std::move(__r.allocator_))
		{
			if(__align != this->memAlign_) this->realign(__align);
			__r.store_		= nullptr;
			__r.count_		= 0;
			__r.memCount_ = 0;
		}
		template <typename _MagnifierR, typename _AllocatorR>
		pf_decl_constexpr
		sequence(
			sequence<_Ty, _MagnifierR, _AllocatorR> &&__r,
			align_val_t __align,
			_MagnifierR &&__magnifier,
			_AllocatorR &&__allocator) pf_attr_noexcept
			: store_(__r.store_)
			, count_(__r.count_)
			, memCount_(__r.memCount_)
			, memAlign_(__r.memAlign_)
			, magnifier_(std::move(__magnifier))
			, allocator_(std::move(__allocator))
		{
			if(__align != this->memAlign_) this->realign(__align);
			__r.store_		= nullptr;
			__r.count_		= 0;
			__r.memCount_ = 0;
		}

		/// Destructor
		pf_decl_constexpr ~sequence() pf_attr_noexcept
		{
			this->clear();
		}

		/// Operator =
		template <typename _MagnifierR, typename _AllocatorR>
		pf_decl_constexpr sequence<_Ty, _Magnifier, _AllocatorR> &
		operator =(
			sequence<_Ty, _MagnifierR, _AllocatorR> const &__r)
		requires(std::is_copy_constructible_v<_Ty>)
		{
			if(this == &__r) pf_hint_unlikely return *this;
			this->reserve(__r.count());
			this->insert(this->begin(), __r.begin(), __r.end());
			this->resize(__r.count());
			return *this;
		}
		template <typename _MagnifierR, typename _AllocatorR>
		pf_decl_constexpr sequence<_Ty, _Magnifier, _AllocatorR> &
		operator =(
			sequence<_Ty, _MagnifierR, _AllocatorR> &&__r)
		{
			if(this == &__r) pf_hint_unlikely return *this;
			this->clear();
			this->store_		= __r.store_;
			__r.store_			= nullptr;
			this->count_		= __r.count_;
			__r.count_			= 0;
			this->memCount_ = __r.memCount_;
			__r.memCount_		= 0;
			return *this;
		}
		pf_decl_constexpr sequence<_Ty, _Magnifier, _Allocator> &
		operator =(
			initializer_list<_Ty> const &__il)
		requires(std::is_copy_constructible_v<_Ty>)
		{
			this->reserve(__il.size());
			this->insert(this->begin(), __il.begin(), __il.end());
			this->resize(__il.size());
			return *this;
		}

		/// Operator ==
		template <typename _MagnifierR, typename _AllocatorR>
		pf_decl_constexpr bool operator ==(
			sequence<_Ty, _MagnifierR, _AllocatorR> const &__r)
		{
			if(this->count() != __r.count())
				return false;
			for(auto b = this->begin(), e = this->end(), rb = __r.begin();
					b != e;)
			{
				if(*b != *rb)
					return false;
				++b;
				++rb;
			}
			return true;
		}
		template <typename _MagnifierR, typename _AllocatorR>
		pf_decl_constexpr bool operator !=(
			sequence<_Ty, _MagnifierR, _AllocatorR> const &__r)
		{
			return !(*this == __r);
		}


		/// Operator []
		pf_hint_nodiscard pf_decl_constexpr _Ty &
		operator [](
			size_t __index) pf_attr_noexcept
		{
			pf_assert(__index < this->count_, "__index is out of sequence!");
			return *(this->store_ + __index);
		}
		pf_hint_nodiscard pf_decl_constexpr const _Ty &
		operator [](
			size_t __index) const pf_attr_noexcept
		{
			pf_assert(__index < this->count_, "__index is out of sequence!");
			return *(this->store_ + __index);
		}

		/// Accessors
		pf_hint_nodiscard pf_decl_constexpr _Ty &
		front() pf_attr_noexcept
		{
			return *this->store_;
		}
		pf_hint_nodiscard pf_decl_constexpr const _Ty &
		front() const pf_attr_noexcept
		{
			return *this->store_;
		}
		pf_hint_nodiscard pf_decl_constexpr _Ty &
		back() pf_attr_noexcept
		{
			return *(this->store_ + this->count_ - 1);
		}
		pf_hint_nodiscard pf_decl_constexpr const _Ty &
		back() const pf_attr_noexcept
		{
			return *(this->store_ + this->count_ - 1);
		}
		pf_hint_nodiscard pf_decl_constexpr _Ty*
		data() pf_attr_noexcept
		{
			return this->store_;
		}
		pf_hint_nodiscard pf_decl_constexpr const _Ty*
		data() const pf_attr_noexcept
		{
			return this->store_;
		}

		/// Iterator -> Begin
		pf_hint_nodiscard pf_decl_constexpr iterator_t
		begin() pf_attr_noexcept
		{
			return this->store_;
		}
		pf_hint_nodiscard pf_decl_constexpr const_iterator_t
		begin() const pf_attr_noexcept
		{
			return this->store_;
		}
		pf_hint_nodiscard pf_decl_constexpr const_iterator_t
		cbegin() const pf_attr_noexcept
		{
			return this->store_;
		}

		/// Iterator -> End
		pf_hint_nodiscard pf_decl_constexpr iterator_t
		end() pf_attr_noexcept
		{
			return this->store_ + this->count_;
		}
		pf_hint_nodiscard pf_decl_constexpr const_iterator_t
		end() const pf_attr_noexcept
		{
			return this->store_ + this->count_;
		}
		pf_hint_nodiscard pf_decl_constexpr const_iterator_t
		cend() const pf_attr_noexcept
		{
			return this->store_ + this->count_;
		}

		/// Iterator -> Reverse Begin
		pf_hint_nodiscard pf_decl_constexpr iterator_t
		rbegin() pf_attr_noexcept
		{
			return this->store_ + this->count_ - 1;
		}
		pf_hint_nodiscard pf_decl_constexpr const_iterator_t
		rbegin() const pf_attr_noexcept
		{
			return this->store_ + this->count_ - 1;
		}
		pf_hint_nodiscard pf_decl_constexpr const_iterator_t
		rcbegin() const pf_attr_noexcept
		{
			return this->store_ + this->count_ - 1;
		}

		/// Iterator -> Reverse End
		pf_hint_nodiscard pf_decl_constexpr iterator_t
		rend() pf_attr_noexcept
		{
			return this->store_ - 1;
		}
		pf_hint_nodiscard pf_decl_constexpr const_iterator_t
		rend() const pf_attr_noexcept
		{
			return this->store_ - 1;
		}
		pf_hint_nodiscard pf_decl_constexpr const_iterator_t
		rcend() const pf_attr_noexcept
		{
			return this->store_ - 1;
		}

		/// Memory
		pf_hint_nodiscard pf_decl_constexpr size_t
		size() const pf_attr_noexcept
		{
			return this->count_ * sizeof(_Ty);
		}
		pf_hint_nodiscard pf_decl_constexpr size_t
		count() const pf_attr_noexcept
		{
			return this->count_;
		}
		pf_hint_nodiscard pf_decl_constexpr size_t
		capacity() const pf_attr_noexcept
		{
			return this->memCount_ * sizeof(_Ty);
		}
		pf_hint_nodiscard pf_decl_constexpr size_t
		shrink_to_magnifier() const pf_attr_noexcept
		{
			const size_t cs = this->capacity();
			const size_t ss = this->magnifier_(this->size());
			this->__reallocate(ss);
			return cs - ss;
		}
		pf_hint_nodiscard pf_decl_constexpr size_t
		shrink_to_fit() const pf_attr_noexcept
		{
			const size_t cs = this->capacity();
			const size_t ss = this->size();
			this->__reallocate(ss);
			return cs - ss;
		}
		pf_hint_nodiscard pf_decl_constexpr align_val_t
		alignment() const pf_attr_noexcept
		{
			return this->memAlign_;
		}

		/// Magnifier
		pf_decl_constexpr _Magnifier &
		magnifier() pf_attr_noexcept
		{
			return this->magnifier_;
		}
		pf_decl_constexpr const _Magnifier &
		magnifier() const pf_attr_noexcept
		{
			return this->magnifier_;
		}

		/// Allocator
		pf_decl_constexpr _Allocator &
		allocator() pf_attr_noexcept
		{
			return this->allocator_;
		}
		pf_decl_constexpr const _Allocator &
		allocator() const pf_attr_noexcept
		{
			return this->allocator_;
		}

		/// Reserve
		pf_decl_constexpr bool
		reserve(
			size_t __c)
		{
			return this->__reallocate(__c * sizeof(_Ty));
		}
		pf_decl_constexpr bool
		reserve(
			size_t __c,
			align_val_t __na)
		{
			return this->__reallocate(__c * sizeof(_Ty), __na);
		}

		/// Realign
		pf_decl_constexpr bool
		realign(
			align_val_t __na)
		{
			return this->reserve(this->count(), __na);
		}

		/// Resize
		pf_decl_constexpr bool
		resize(
			size_t __c)
		{
			bool r = this->reserve(__c);
			this->__resize_default_construct(__c);
			return r;
		}
		pf_decl_constexpr bool
		resize(
			size_t __c,
			align_val_t __na)
		{
			bool r = this->reserve(__c, __na);
			this->__resize_default_construct(__c);
			return r;
		}
		pf_decl_constexpr bool
		resize(
			size_t __c,
			_Ty const &__val)
		{
			bool r = this->reserve(__c);
			this->__resize_copy_construct(__c, __val);
			return r;
		}
		pf_decl_constexpr bool
		resize(
			size_t __c,
			align_val_t __na,
			_Ty const &__val)
		{
			bool r = this->reserve(__c, __na);
			this->__resize_copy_construct(__c, __val);
			return r;
		}

		/// Insert Back
		template <typename ..._Args>
		pf_decl_constexpr iterator_t
		insert_back(
			_Args && ... __args)
		requires(std::is_constructible_v<_Ty, _Args ...>)
		{
			if(this->count_ + 1 > this->memCount_)
			{
				this->reserve(this->count_ + 1);
			}
			_Ty *p = this->store_ + this->count_;
			construct_at(p, std::forward<_Args>(__args)...);
			++this->count_;
			return p;
		}
		pf_decl_constexpr iterator_t
		insert_back(
			size_t __num,
			const _Ty &__val)
		requires(std::is_copy_constructible_v<_Ty>)
		{
			if(this->count_ + __num > this->memCount_)
			{
				this->reserve(this->count_ + __num);
			}
			_Ty *p = this->store_ + this->count_;
			_Ty *e = p + __num;
			for(_Ty *c = p; c != e; ++c)
			{
				construct_at(c, __val);
			}
			this->count_ += __num;
			return p;
		}
		pf_decl_constexpr iterator_t
		insert_back(
			_Ty &&__val)
		requires(std::is_move_constructible_v<_Ty>)
		{
			if(this->count_ + 1 > this->memCount_)
			{
				this->reserve(this->count() + 1);
			}
			_Ty *p = this->store_ + this->count_;
			construct_at(p, std::move(__val));
			++this->count_;
			return p;
		}
		template <typename _InIteratorTy>
		pf_decl_constexpr iterator_t
		insert_back(
			_InIteratorTy __beg,
			_InIteratorTy __end)
		requires(
			std::is_copy_constructible_v<_Ty>&&
			std::is_same_v<std::remove_cvref_t<decltype(*std::declval<_InIteratorTy>())>, _Ty>)
		{
			const size_t d = countof(__beg, __end);
			if(this->count_ + d > this->memCount_)
			{
				this->reserve(this->count_ + d);
			}
			_Ty *p = this->store_ + this->count_;
			while(__beg != __end)
			{
				construct_at(p, *__beg);
				++p;
				++__beg;
			}
			this->count_ += d;
			return p;
		}
		pf_decl_constexpr iterator_t
		insert_back(
			initializer_list<_Ty> const &__il)
		requires(std::is_copy_constructible_v<_Ty>)
		{
			return this->insert_back(__il.begin(), __il.end());
		}

		/// Insert
		template <typename ..._Args>
		pf_decl_constexpr iterator_t
		insert(
			iterator_t __it,
			_Args && ... __args)
		requires(std::is_constructible_v<_Ty, _Args ...>)
		{
			if(this->count_ + 1 > this->memCount_)
			{
				size_t c = countof(this->begin(), __it);
				if(this->reserve(this->count_ + 1))
				{
					__it = this->begin() + c;
				}
			}
			std::memmove(__it + 1, __it, sizeof_s(__it, this->store_ + this->count_));
			construct_at(__it, std::forward<_Args>(__args)...);
			++this->count_;
			return __it;
		}
		pf_decl_constexpr iterator_t
		insert(
			iterator_t __it,
			size_t __num,
			const _Ty &__val)
		requires(std::is_copy_constructible_v<_Ty>)
		{
			const size_t c = this->count_ + __num;
			if(c > this->memCount_)
			{
				size_t o = countof(this->begin(), __it);
				if(this->reserve(c))
				{
					__it = this->begin() + o;
				}
			}
			std::memmove(__it + __num, __it, sizeof_s(__it.get(), this->store_ + this->count_));
			for(iterator_t b = __it, e = __it + __num; b != e; ++b)
			{
				construct_at(b.get(), __val);
			}
			this->count_ = c;
			return __it;
		}
		pf_decl_constexpr iterator_t
		insert(
			iterator_t __it,
			_Ty &&__val)
		requires(std::is_copy_constructible_v<_Ty>)
		{
			if(this->count_ + 1 > this->memCount_)
			{
				size_t c = countof(this->begin(), __it);
				if(this->reserve(this->count_ + 1))
				{
					__it = this->begin() + c;
				}
			}
			std::memmove(__it + 1, __it, sizeof_s(__it.get(), this->store_ + this->count_));
			construct_at(__it.get(), std::move(__val));
			++this->count_;
			return __it;
		}
		template <typename _InIteratorTy>
		pf_decl_constexpr iterator_t
		insert(
			iterator_t __it,
			_InIteratorTy __beg,
			_InIteratorTy __end)
		requires(
			std::is_copy_constructible_v<_Ty>&&
			std::is_same_v<std::remove_cvref_t<decltype(*std::declval<_InIteratorTy>())>, _Ty>)
		{
			const size_t s = sizeof_s(__beg, __end);
			const size_t c = s / sizeof(value_typeof_t<_InIteratorTy>);
			if(this->count_ + c > this->memCount_)
			{
				size_t o = countof(this->begin(), __it);
				if(this->reserve(this->count_ + c))
				{
					__it = this->begin() + o;
				}
			}
			std::memmove(__it + c, __it, sizeof_s(__it.get(), this->store_ + this->count_));
			for(iterator_t b = __it; __beg != __end;)
			{
				construct_at(b.get(), *__beg);
				++b;
				++__beg;
			}
			this->count_ += c;
			return __it;
		}
		pf_decl_constexpr iterator_t
		insert(
			iterator_t __it,
			initializer_list<_Ty> const &__il)
		requires(std::is_copy_constructible_v<_Ty>)
		{
			return this->insert(__it, __il.begin(), __il.end());
		}

		/// Erase
		pf_decl_constexpr void
		erase_back() pf_attr_noexcept
		{
			--this->count_;
			destroy_at(this->store_ + this->count_);
		}
		pf_decl_constexpr void
		erase(
			iterator_t __it) pf_attr_noexcept
		{
			destroy_at(__it.get());
			std::memmove(__it, __it + 1, sizeof_s((__it + 1).get(), this->store_ + this->count_));
			--this->count_;
		}
		pf_decl_constexpr void
		erase(
			iterator_t __beg,
			iterator_t __end) pf_attr_noexcept
		{
			iterator_t b = __beg;
			while(b != __end)
			{
				destroy_at(b.get());
				++b;
			}
			std::memmove(__beg, __end, sizeof_s(__end, this->store_ + this->count_));
			this->count_ -= countof(__beg, __end);
		}

		/// Swap
		pf_decl_constexpr void
		swap(
			sequence<_Ty, _Magnifier, _Allocator> &__r) pf_attr_noexcept
		{
			swap(this->store_, __r.store_);
			swap(this->count_, __r.count_);
			swap(this->memCount_, __r.memCount_);
			swap(this->growCount_, __r.growCount_);
			swap(this->memAlign_, __r.memAlign_);
			swap(this->magnifier_, __r.magnifier_);
			swap(this->allocator_, __r.allocator_);
		}

		/// Clear
		pf_decl_constexpr void
		clear() pf_attr_noexcept
		{
			if(!this->store_) return;
			if(std::is_constant_evaluated())
			{
				delete[] this->store_;
			}
			else
			{
				_Ty *it	 = this->store_;
				_Ty *end = this->store_ + this->count_;
				while(it != end)
				{
					destroy_at(it);
					++it;
				}
				this->allocator_.deallocate(this->store_);
			}
			this->store_		= nullptr;
			this->count_		= 0;
			this->memCount_ = 0;
		}

		/// Is Empty
		pf_hint_nodiscard pf_decl_constexpr bool
		is_empty() const pf_attr_noexcept
		{
			return this->count_ == 0;
		}

		/// View
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr sequence_view<_Ty>
		view() pf_attr_noexcept
		{
			return sequence_view<_Ty>(this->begin(), this->end());
		}

		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr sequence_view<_Ty>
		view() const pf_attr_noexcept
		{
			return sequence_view<const _Ty>(this->begin(), this->end());
		}

		/// Operator (sequence_view<_Ty>)
		pf_hint_nodiscard pf_decl_constexpr
		operator sequence_view<_Ty>() pf_attr_noexcept
		{
			return this->view();
		}
		pf_hint_nodiscard pf_decl_constexpr
		operator sequence_view<const _Ty>() const pf_attr_noexcept
		{
			return this->view();
		}

	private:
		_Ty *store_;
		size_t count_;
		size_t memCount_;
		align_val_t memAlign_;
		pf_hint_nounique_address _Magnifier magnifier_;
		pf_hint_nounique_address _Allocator allocator_;
	};
}

#endif // !PULSAR_SEQUENCE_HPP