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
#include "pulsar/iterable.hpp"

// Pulsar
namespace pul
{
	/// SEQUENCE: Types
	template<typename _Ty>
		requires(!std::is_const_v<_Ty>)
	class sequence_view;
	template<
	 typename _Ty,
	 typename _Magnifier = magnifier_default,
	 typename _Allocator = allocator_default>
		requires(
		 !std::is_const_v<_Ty>
		 && is_magnifier_v<_Magnifier>
		 && is_allocator_v<_Allocator>)
	class sequence;

	/// SEQUENCE: View
	template<typename _Ty>
		requires(!std::is_const_v<_Ty>)
	class sequence_view
	{
	public:
		using value_t									 = _Ty;
		using const_iterator_t				 = const_iterator<_Ty>;
		using const_reverse_iterator_t = reverse_iterator<const_iterator_t>;

		/// Constructors
		pf_decl_inline pf_decl_constexpr
		sequence_view() pf_attr_noexcept
			: data_(nullptr)
			, count_(0)
		{}
		pf_decl_inline pf_decl_constexpr
		sequence_view(
		 const _Ty *__data,
		 size_t __count) pf_attr_noexcept
			: data_(__data)
			, count_(__count)
		{}
		template<
		 size_t _Num>
		pf_decl_inline pf_decl_constexpr
		sequence_view(
		 const _Ty (&__arr)[_Num]) pf_attr_noexcept
			: data_(&__arr[0])
			, count_(_Num)
		{}
		template<
		 size_t _Num>
		pf_decl_inline pf_decl_constexpr
		sequence_view(
		 const array<_Ty, _Num> &__arr) pf_attr_noexcept
			: data_(&__arr[0])
			, count_(_Num)
		{}
		template<
		 typename _Magnifier,
		 typename _Allocator>
		pf_decl_inline pf_decl_constexpr
		sequence_view(
		 const sequence<_Ty, _Magnifier, _Allocator> &__s)
		 pf_attr_noexcept : data_(__s.data())
			, count_(__s.count())
		{}
		pf_decl_inline pf_decl_constexpr
		sequence_view(
		 const_iterator_t __beg,
		 const_iterator_t __end) pf_attr_noexcept
			: data_(__beg.get())
			, count_(countof(__beg, __end))
		{}
		pf_decl_inline pf_decl_constexpr
		sequence_view(const sequence_view<_Ty> &__r) pf_attr_noexcept
			: data_(__r.data_)
			, count_(__r.count_)
		{}

		/// Destructor
		pf_decl_inline pf_decl_constexpr ~sequence_view() pf_attr_noexcept = default;

		/// Operator =
		pf_decl_inline pf_decl_constexpr sequence_view<_Ty> &
		operator=(
		 const sequence_view<_Ty> &__r) pf_attr_noexcept
		{
			if(pf_likely(this != &__r))
			{
				this->data_	 = __r.data_;
				this->count_ = __r.count_;
			}
			return *this;
		}

		/// Operator []
		pf_decl_inline pf_decl_constexpr const _Ty &
		operator[](
		 size_t __index) const
		{
			pf_throw_if(
			 __index >= this->count_,
			 dbg_category_generic(),
			 dbg_code::invalid_argument,
			 dbg_flags::none,
			 "index is out of sequence view! count={}, index={}",
			 this->count_,
			 __index);
			return *(this->data_ + __index);
		}

		/// Begin
		pf_hint_nodiscard pf_decl_inline const_iterator_t
		begin() const pf_attr_noexcept
		{
			return this->data_;
		}
		pf_hint_nodiscard pf_decl_inline const_iterator_t
		cbegin() const pf_attr_noexcept
		{
			return this->begin();
		}

		/// End
		pf_hint_nodiscard pf_decl_inline const_iterator_t
		end() const pf_attr_noexcept
		{
			return (this->data_ + this->count_);
		}
		pf_hint_nodiscard pf_decl_inline const_iterator_t
		cend() const pf_attr_noexcept
		{
			return this->end();
		}

		/// Reverse Begin
		pf_hint_nodiscard pf_decl_inline const_iterator_t
		rbegin() const pf_attr_noexcept
		{
			return (this->data_ - 1 + this->count_);
		}
		pf_hint_nodiscard pf_decl_inline const_iterator_t
		crbegin() const pf_attr_noexcept
		{
			return this->rbegin();
		}

		/// Reverse End
		pf_hint_nodiscard pf_decl_inline const_iterator_t
		rend() const pf_attr_noexcept
		{
			return (this->data_ - 1);
		}
		pf_hint_nodiscard pf_decl_inline const_iterator_t
		crend() const pf_attr_noexcept
		{
			return this->rend();
		}

		/// Data
		pf_hint_nodiscard pf_decl_inline const _Ty *
		data() const pf_attr_noexcept
		{
			return this->data_;
		}

		/// Count
		pf_hint_nodiscard pf_decl_inline size_t
		count() const pf_attr_noexcept
		{
			return this->count_;
		}

		/// Size
		pf_hint_nodiscard pf_decl_inline size_t
		size() const pf_attr_noexcept
		{
			return this->count_ * sizeof(_Ty);
		}

		/// Is Empty
		pf_hint_nodiscard pf_decl_inline bool
		is_empty() const pf_attr_noexcept
		{
			return !this->data_;
		}

	private:
		const _Ty *data_;
		size_t count_;
	};

	/// SEQUENCE: View -> CTAD
	template<
	 typename _IteratorIn>
		requires(is_iterator_v<_IteratorIn>)
	sequence_view(_IteratorIn, _IteratorIn)
	 -> sequence_view<std::remove_const_t<typename _IteratorIn::value_t>>;

	/// SEQUENCE:
	template<
	 typename _Ty,
	 typename _Magnifier,
	 typename _Allocator>
		requires(
		 !std::is_const_v<_Ty>
		 && is_magnifier_v<_Magnifier>
		 && is_allocator_v<_Allocator>)
	class sequence
	{
		template<typename _Uy, typename _MagnifierR, typename _AllocatorR>
		pf_decl_friend class sequence;

		// Reallocate Storage
		/*!
		 * @brief Reallocate Storage
		 *
		 * @param[in] __count The size of the new storage
		 * @param[in] __align The alignment of the new storage
		 *
		 * @return diff_t The difference between the old and new storage size
		 */
		pf_decl_constexpr diff_t
		__reallocate_no_check(
		 size_t __count,
		 align_val_t __align)
		{
			this->data_ = union_cast<_Ty *>(reallocate(
			 this->allocator_,
			 this->data_,
			 __count * sizeof(_Ty),
			 __align));
			pf_throw_if(
			 !this->data_,
			 dbg_category_generic(),
			 dbg_code::bad_alloc,
			 dbg_flags::dump_with_data_segs | dbg_flags::dump_with_handle_data,
			 "Cannot reallocate sequence storage!");
			const diff_t d	= union_cast<diff_t>(__count) - this->capacity_;
			this->capacity_ = __count;
			this->align_		= __align;
			return d;
		}

		// Index to iterator
		pf_decl_constexpr pf_decl_inline size_t
		__iterator_to_index(iterator<int32_t> __it) const pf_attr_noexcept
		{
			return countof(this->data(), __it.get());
		}

	public:
		using value_t									 = _Ty;
		using iterator_t							 = iterator<_Ty>;
		using const_iterator_t				 = const_iterator<_Ty>;
		using reverse_iterator_t			 = reverse_iterator<iterator_t>;
		using const_reverse_iterator_t = reverse_iterator<const_iterator_t>;
		using view_t									 = sequence_view<_Ty>;

		/// Constructors
		pf_decl_constexpr
		sequence(
		 align_val_t __align			= ALIGN_DEFAULT,
		 _Magnifier &&__magnifier = _Magnifier(),
		 _Allocator &&__allocator = _Allocator()) pf_attr_noexcept
			: data_(nullptr)
			, capacity_(0)
			, align_(__align)
			, count_(0)
			, magnifier_(std::move(__magnifier))
			, allocator_(std::move(__allocator))
		{
		}
		pf_decl_constexpr
		sequence(
		 const _Ty &__val,
		 size_t __count,
		 align_val_t __align			= ALIGN_DEFAULT,
		 _Magnifier &&__magnifier = _Magnifier(),
		 _Allocator &&__allocator = _Allocator())
			: sequence(__align, std::move(__magnifier), std::move(__allocator))
		{
			__count = this->magnifier_(__count);
			this->__reallocate_no_check(__count, __align);
			construct(this->begin(), this->begin() + __count, __val);
			this->count_ = __count;
		}
		template<typename _IteratorIn>
		pf_decl_constexpr
		sequence(
		 _IteratorIn __beg,
		 _IteratorIn __end,
		 align_val_t __align			= ALIGN_DEFAULT,
		 _Magnifier &&__magnifier = _Magnifier(),
		 _Allocator &&__allocator = _Allocator())
			requires(is_iterator_v<_IteratorIn>)
			: sequence(__align, std::move(__magnifier), std::move(__allocator))
		{
			const size_t c = this->magnifier_(countof(__beg, __end));
			this->__reallocate_no_check(c, __align);
			construct(this->begin(), this->begin() + c, __beg);
			this->count_ = c;
		}
		template<size_t _Num>
		pf_decl_constexpr
		sequence(
		 const _Ty (&__arr)[_Num],
		 align_val_t __align			= ALIGN_DEFAULT,
		 _Magnifier &&__magnifier = _Magnifier(),
		 _Allocator &&__allocator = _Allocator())
			: sequence(pul::begin(__arr), pul::end(__arr), __align, std::move(__magnifier), std::move(__allocator))
		{}
		template<size_t _Num>
		pf_decl_constexpr
		sequence(
		 const array<_Ty, _Num> &__arr,
		 align_val_t __align			= ALIGN_DEFAULT,
		 _Magnifier &&__magnifier = _Magnifier(),
		 _Allocator &&__allocator = _Allocator())
			: sequence(__arr.begin(), __arr.end(), __align, std::move(__magnifier), std::move(__allocator))
		{}
		pf_decl_constexpr
		sequence(
		 sequence_view<_Ty> __view,
		 align_val_t __align			= ALIGN_DEFAULT,
		 _Magnifier &&__magnifier = _Magnifier(),
		 _Allocator &&__allocator = _Allocator())
			: sequence(__view.begin(), __view.end(), __align, std::move(__magnifier), std::move(__allocator))
		{}
		pf_decl_constexpr
		sequence(
		 initializer_list<_Ty> __list,
		 align_val_t __align			= ALIGN_DEFAULT,
		 _Magnifier &&__magnifier = _Magnifier(),
		 _Allocator &&__allocator = _Allocator())
			: sequence(const_iterator_t(__list.begin()), const_iterator_t(__list.end()), __align, std::move(__magnifier), std::move(__allocator))
		{}
		template<typename _MagnifierR, typename _AllocatorR>
		pf_decl_constexpr
		sequence(
		 const sequence<_Ty, _MagnifierR, _AllocatorR> &__r,
		 align_val_t __align,
		 _Magnifier &&__magnifier,
		 _Allocator &&__allocator) pf_attr_noexcept
			: sequence(__r.begin(), __r.end(), __align, std::move(__magnifier), std::move(__allocator))
		{}
		pf_decl_constexpr
		sequence(
		 const sequence<_Ty, _Magnifier, _Allocator> &__r,
		 align_val_t __align) pf_attr_noexcept
			: sequence(__r.begin(), __r.end(), __align, _Magnifier(), _Allocator())
		{}
		pf_decl_constexpr
		sequence(
		 const sequence<_Ty, _Magnifier, _Allocator> &__r) pf_attr_noexcept
			: sequence(__r.begin(), __r.end(), __r.align_, _Magnifier(), _Allocator())
		{}
		pf_decl_constexpr
		sequence(sequence<_Ty, _Magnifier, _Allocator> &&__r) pf_attr_noexcept
			: data_(__r.data_)
			, capacity_(__r.capacity_)
			, align_(__r.align_)
			, count_(__r.count_)
			, magnifier_(std::move(__r.magnifier_))
			, allocator_(std::move(__r.allocator_))
		{
			__r.data_			= nullptr;
			__r.capacity_ = 0;
			__r.count_		= 0;
		}
		template<typename _MagnifierR>
		pf_decl_constexpr
		sequence(
		 sequence<_Ty, _MagnifierR, _Allocator> &&__r,
		 _Magnifier &&__magnifier) pf_attr_noexcept
			: data_(__r.data_)
			, capacity_(__r.capacity_)
			, align_(__r.align_)
			, count_(__r.count_)
			, magnifier_(std::move(__magnifier))
			, allocator_(__r.allocator_)
		{
			__r.data_			= nullptr;
			__r.capacity_ = 0;
			__r.count_		= 0;
		}

		/// Destructor
		pf_decl_constexpr ~sequence() pf_attr_noexcept
		{
			this->clear();
		}

		/// Operator =
		pf_decl_constexpr sequence<_Ty, _Magnifier, _Allocator> &
		operator=(
		 sequence_view<_Ty> __v)
		{
			this->assign(__v.begin(), __v.end());
			return *this;
		}
		pf_decl_constexpr sequence<_Ty, _Magnifier, _Allocator> &
		operator=(
		 const sequence<_Ty, _Magnifier, _Allocator> &__r)
		{
			if(this != &__r)
			{
				this->assign(__r.begin(), __r.end());
			}
			return *this;
		}
		template<typename _MagnifierR, typename _AllocatorR>
		pf_decl_constexpr sequence<_Ty, _Magnifier, _Allocator> &
		operator=(
		 const sequence<_Ty, _MagnifierR, _AllocatorR> &__r)
			requires(is_magnifier_v<_MagnifierR> && is_allocator_v<_AllocatorR>)
		{
			this->assign(__r.begin(), __r.end());
			return *this;
		}
		pf_decl_inline pf_decl_constexpr sequence<_Ty, _Magnifier, _Allocator> &
		operator=(
		 sequence<_Ty, _Magnifier, _Allocator> &&__r)
		{
			if(pf_likely(this->data_ != __r.data_))
			{
				this->clear();
				this->data_			 = __r.data_;
				this->capacity_	 = __r.capacity_;
				this->align_		 = __r.align_;
				this->count_		 = __r.count_;
				this->magnifier_ = std::move(__r.magnifier_);
				this->allocator_ = std::move(__r.allocator_);
				__r.data_				 = nullptr;
				__r.capacity_		 = 0;
				__r.count_			 = 0;
			}
			return *this;
		}

		/// Operator (View)
		pf_hint_nodiscard pf_decl_explicit pf_decl_inline pf_decl_constexpr
		operator sequence_view<_Ty>() const pf_attr_noexcept
		{
			return this->view();
		}

		/// Operator []
		pf_decl_inline pf_decl_constexpr _Ty &
		operator[](size_t __index)
		{
			pf_throw_if(
			 __index >= this->count_,
			 dbg_category_generic(),
			 dbg_code::invalid_argument,
			 dbg_flags::none,
			 "index is out of sequence range! count={}, index={}",
			 this->count_,
			 __index);
			return *(this->data_ + __index);
		}
		pf_decl_inline pf_decl_constexpr const _Ty &
		operator[](size_t __index) const
		{
			pf_throw_if(
			 __index >= this->count_,
			 dbg_category_generic(),
			 dbg_code::invalid_argument,
			 dbg_flags::none,
			 "index is out of sequence range! count={}, index={}",
			 this->count_,
			 __index);
			return *(this->data_ + __index);
		}

		/// Assign
		pf_decl_constexpr void
		assign(
		 const _Ty &__val,
		 size_t __count)
		{
			if(this->count_ < __count)
			{
				this->reserve_with_magnifier(__count);
				pul::assign(this->begin(), this->begin() + this->count_, __val);
				construct(this->begin() + this->count_, this->begin() + __count, __val);
			}
			else
			{
				pul::assign(this->begin(), this->begin() + __count, __val);
				destroy(this->begin() + __count, this->end());
			}
			this->count_ = __count;
		}
		template<typename _IteratorIn>
		pf_decl_constexpr void
		assign(
		 _IteratorIn __beg,
		 _IteratorIn __end)
			requires(is_iterator_v<_IteratorIn>)
		{
			const size_t c = countof(__beg, __end);
			if(this->count_ < c)
			{
				this->reserve_with_magnifier(c);
				pul::assign(this->begin(), this->begin() + this->count_, __beg);
				construct(this->begin() + this->count_, this->begin() + c, __beg + this->count_);
			}
			else
			{
				pul::assign(this->begin(), this->begin() + c, __beg);
				destroy(this->begin() + c, this->end());
			}
			this->count_ = c;
		}
		template<typename _View>
		pf_decl_constexpr void
		assign(
		 _View __v)
			requires(is_view_v<_View>)
		{
			this->assign(__v.begin(), __v.end());
		}
		pf_decl_constexpr void
		assign(
		 initializer_list<_Ty> __il)
		{
			this->assign(iterator(__il.begin()), iterator(__il.end()));
		}

		/// Insert
		template<typename... _Args>
		pf_decl_constexpr size_t
		push_back(
		 _Args &&...__args)
			requires(std::is_constructible_v<_Ty, _Args...>)
		{
			this->reserve_with_magnifier(this->count_ + 1);
			iterator_t w = this->begin() + this->count_;
			construct(w, std::forward<_Args>(__args)...);
			++this->count_;
			return this->count_;
		}
		pf_decl_constexpr size_t
		push_back(
		 const _Ty &__val,
		 size_t __count)
		// requires(is_copy_makable<_Ty>)
		{
			this->reserve_with_magnifier(this->count_ + __count);
			iterator_t b = this->begin() + this->count_;
			iterator_t e = this->begin() + this->count_ + __count;
			construct(b, e, __val);
			this->count_ += __count;
			return this->count_;
		}
		template<typename _IteratorIn>
		pf_decl_constexpr size_t
		push_back(
		 _IteratorIn __beg,
		 _IteratorIn __end)
			requires(is_iterator_v<_IteratorIn>)
		{
			const size_t c = countof(__beg, __end);
			this->reserve_with_magnifier(this->count_ + c);
			iterator_t b = this->begin() + this->count_;
			iterator_t e = this->begin() + this->count_ + c;
			construct(b, e, __beg);
			this->count_ += c;
			return this->count_;
		}
		template<typename _View>
		pf_decl_constexpr size_t
		push_back(
		 _View __v)
			requires(
			 is_view_v<_View>
			 && std::is_same_v<_Ty, value_type_t<_View>>)
		{
			return this->push(__v.begin(), __v.end());
		}
		pf_decl_constexpr size_t
		push_back(
		 initializer_list<_Ty> __il)
		{
			return this->push(iterator_t(__il.begin()), iterator_t(__il.end()));
		}

		template<typename... _Args>
		pf_decl_constexpr size_t
		push(
		 size_t __w,
		 _Args &&...__args)
			requires(std::is_constructible_v<_Ty, _Args...>)
		{
			pf_throw_if(
			 __w > this->count_,
			 dbg_category_generic(),
			 dbg_code::invalid_argument,
			 dbg_flags::none,
			 "Trying to insert outside of sequence range! count={}, where={}",
			 this->count_,
			 __w);
			this->reserve_with_magnifier(this->count_ + 1);
			iterator_t w = this->begin() + __w;
			memmove(w + 1, w, distof(w, this->end()));
			construct(w, std::forward<_Args>(__args)...);
			++this->count_;
			return (__w + 1);
		}
		pf_decl_constexpr size_t
		push(
		 size_t __w,
		 const _Ty &__val,
		 size_t __count)
		// requires(is_copy_makable_v<_Ty>)
		{
			pf_throw_if(
			 __w > this->count_,
			 dbg_category_generic(),
			 dbg_code::invalid_argument,
			 dbg_flags::none,
			 "Trying to insert outside of sequence range! count={}, where={}",
			 this->count_,
			 __w);
			this->reserve_with_magnifier(this->count_ + __count);
			iterator_t w = this->begin() + __w;
			memmove(w + __count, w, distof(w, this->end()));
			construct(w, w + __count, __val);
			this->count_ += __count;
			return (__w + __count);
		}
		template<typename _IteratorIn>
		pf_decl_constexpr size_t
		push(
		 size_t __w,
		 _IteratorIn __beg,
		 _IteratorIn __end)
			requires(is_iterator_v<_IteratorIn>)
		{
			pf_throw_if(
			 __w > this->count_,
			 dbg_category_generic(),
			 dbg_code::invalid_argument,
			 dbg_flags::none,
			 "Trying to insert outside of sequence range! count={}, where={}",
			 this->count_,
			 __w);
			const size_t c = countof(__beg, __end);
			this->reserve_with_magnifier(this->count_ + c);
			iterator_t w = this->begin() + __w;
			memmove(w + c, w, distof(w, this->end()));
			construct(w, w + c, __beg);
			this->count_ += c;
			return __w + c;
		}
		template<typename _View>
		pf_decl_constexpr size_t
		push(
		 size_t __w,
		 _View __v)
			requires(
			 is_view_v<_View>
			 && std::is_same_v<_Ty, value_type_t<_View>>)
		{
			return this->push(__w, __v.begin(), __v.end());
		}
		pf_decl_constexpr size_t
		push(
		 size_t __w,
		 initializer_list<_Ty> __il)
		{
			return this->push(__w, iterator_t(__il.begin()), iterator_t(__il.end()));
		}

		/// Insert
		template<typename... _Args>
		pf_decl_constexpr iterator_t
		insert_back(
		 _Args &&...__args)
			requires(std::is_constructible_v<_Ty, _Args...>)
		{
			return this->begin() + this->push_back(std::forward<_Args>(__args)...);
		}
		pf_decl_constexpr iterator_t
		insert_back(
		 const _Ty &__val,
		 size_t __count)
		{
			return this->begin() + this->push_back(__val, __count);
		}
		template<typename _IteratorIn>
		pf_decl_constexpr iterator_t
		insert_back(
		 _IteratorIn __beg,
		 _IteratorIn __end)
			requires(is_iterator_v<_IteratorIn>)
		{
			return this->begin() + this->push_back(__beg, __end);
		}
		template<typename _View>
		pf_decl_constexpr iterator_t
		insert_back(
		 _View __v)
			requires(
			 is_view_v<_View>
			 && std::is_same_v<_Ty, value_type_t<_View>>)
		{
			return this->insert_back(__v.begin(), __v.end());
		}
		pf_decl_constexpr iterator_t
		insert_back(
		 initializer_list<_Ty> __il)
		{
			return this->insert_back(iterator_t(__il.begin()), iterator_t(__il.end()));
		}

		template<typename... _Args>
		pf_decl_constexpr iterator_t
		insert(
		 iterator_t __w,
		 _Args &&...__args)
			requires(std::is_constructible_v<_Ty, _Args...>)
		{
			return this->begin()
					 + this->push(this->__iterator_to_index(__w), std::forward<_Args>(__args)...);
		}
		pf_decl_constexpr iterator_t
		insert(
		 iterator_t __w,
		 const _Ty &__val,
		 size_t __count)
		{
			return this->begin()
					 + this->push(this->__iterator_to_index(__w), __val, __count);
		}
		template<typename _IteratorIn>
		pf_decl_constexpr iterator_t
		insert(
		 iterator_t __w,
		 _IteratorIn __beg,
		 _IteratorIn __end)
			requires(is_iterator_v<_IteratorIn>)
		{
			return this->begin()
					 + this->push(this->__iterator_to_index(__w), __beg, __end);
		}
		pf_decl_constexpr iterator_t
		insert(
		 iterator_t __w,
		 sequence_view<_Ty> __v)
		{
			return this->begin()
					 + this->push(this->__iterator_to_index(__w), __v.begin(), __v.end());
		}
		template<typename _View>
		pf_decl_constexpr iterator_t
		insert(
		 iterator_t __w,
		 _View __v)
			requires(
			 is_view_v<_View>
			 && std::is_same_v<_Ty, value_type_t<_View>>)
		{
			return this->insert(__w, __v.begin(), __v.end());
		}
		pf_decl_constexpr iterator_t
		insert(
		 iterator_t __w,
		 initializer_list<_Ty> __il)
		{
			return this->insert(__w, iterator_t(__il.begin()), iterator_t(__il.end()));
		}

		/// Remove
		pf_decl_constexpr size_t
		pop_back() pf_attr_noexcept
		{
			pf_throw_if(
			 this->is_empty(),
			 dbg_category_generic(),
			 dbg_code::invalid_argument,
			 dbg_flags::none,
			 "Trying to remove elements from empty sequence!");
			--this->count_;
			destroy(this->end());
			return this->count_;
		}

		pf_decl_constexpr size_t
		pop(
		 size_t __w) pf_attr_noexcept
		{
			pf_throw_if(
			 __w > this->count_,
			 dbg_category_generic(),
			 dbg_code::invalid_argument,
			 dbg_flags::none,
			 "Trying to insert outside of sequence range! count={}, where={}",
			 this->count_,
			 __w);
			iterator_t w = this->begin() + __w;
			destroy(w);
			memmove(w, w + 1, distof(w + 1, this->end()));
			--this->count_;
			return __w;
		}
		pf_decl_constexpr size_t
		pop(
		 size_t __beg,
		 size_t __end) pf_attr_noexcept
		{
			pf_throw_if(
			 __beg > __end,
			 dbg_category_generic(),
			 dbg_code::invalid_argument,
			 dbg_flags::none,
			 "Range of elements to remove is ill-formed! begin={}, end={}",
			 __beg,
			 __end);
			pf_throw_if(
			 __end > this->count_,
			 dbg_category_generic(),
			 dbg_code::invalid_argument,
			 dbg_flags::none,
			 "Trying to insert outside of sequence range! count={}, end={}",
			 this->count_,
			 __end);
			iterator_t b = this->begin() + __beg;
			iterator_t e = this->begin() + __end;
			destroy(b, e);
			memcpy(b, e, distof(e, this->end()));
			this->count_ -= countof(b, e);
			return __beg;
		}

		/// Remove
		pf_decl_constexpr iterator_t
		remove_back() pf_attr_noexcept
		{
			return this->begin() + this->pop_back();
		}

		pf_decl_constexpr iterator_t
		remove(
		 iterator_t __w) pf_attr_noexcept
		{
			return this->begin() + this->pop(this->__iterator_to_index(__w));
		}
		pf_decl_constexpr iterator_t
		remove(
		 iterator_t __wb,
		 iterator_t __we) pf_attr_noexcept
		{
			return this->begin()
					 + this->pop(this->__iterator_to_index(__wb), this->__iterator_to_index(__we));
		}

		/// Replace
		template<typename... _Args>
		pf_decl_constexpr size_t
		replace(
		 size_t __w,
		 _Args &&...__args)
			requires(std::is_constructible_v<_Ty, _Args...>)
		{
			pf_throw_if(
			 __w >= this->count_,
			 dbg_category_generic(),
			 dbg_code::invalid_argument,
			 dbg_flags::none,
			 "Trying to insert outside of sequence range! count={}, where={}",
			 this->count_,
			 __w);
			iterator_t w = this->begin() + __w;
			pul::assign(w, std::forward<_Args>(__args)...);
			return __w + 1;
		}
		pf_decl_constexpr size_t
		replace(
		 size_t __w,
		 const _Ty &__val,
		 size_t __count)
		// requires(is_copy_makable_v<_Ty>)
		{
			pf_throw_if(
			 __w >= this->count_,
			 dbg_category_generic(),
			 dbg_code::invalid_argument,
			 dbg_flags::none,
			 "Trying to insert outside of sequence range! count={}, where={}",
			 this->count_,
			 __w);
			iterator_t w = this->begin() + __w;
			pul::assign(w, __val);
			return this->push(__w + 1, __val, __count - 1);
		}
		template<typename _IteratorIn>
		pf_decl_constexpr size_t
		replace(
		 size_t __w,
		 _IteratorIn __beg,
		 _IteratorIn __end)
			requires(is_iterator_v<_IteratorIn>)
		{
			pf_throw_if(
			 __w >= this->count_,
			 dbg_category_generic(),
			 dbg_code::invalid_argument,
			 dbg_flags::none,
			 "Trying to insert outside of sequence range! count={}, where={}",
			 this->count_,
			 __w);
			iterator_t w = this->begin() + __w;
			pul::assign(w, *__beg);
			if(__beg == __end)
				return __w + 1;
			return this->push(__w + 1, __beg + 1, __end);
		}
		template<typename _View>
		pf_decl_constexpr size_t
		replace(
		 size_t __w,
		 _View __v)
			requires(
			 is_view_v<_View>
			 && std::is_same_v<_Ty, value_type_t<_View>>)
		{
			return this->replace(__w, __v.begin(), __v.end());
		}
		pf_decl_constexpr size_t
		replace(
		 size_t __w,
		 initializer_list<_Ty> __il)
		{
			return this->replace(__w, iterator_t(__il.begin()), iterator_t(__il.end()));
		}


		pf_decl_constexpr size_t
		replace(
		 size_t __wbeg,
		 size_t __wend,
		 const _Ty &__val,
		 size_t __count)
		{
			pf_throw_if(
			 __wbeg >= __wend,
			 dbg_category_generic(),
			 dbg_code::invalid_argument,
			 dbg_flags::none,
			 "Range of elements to replace is ill-formed! begin={}, end={}",
			 __wbeg,
			 __wend);
			pf_throw_if(
			 __wend > this->count_,
			 dbg_category_generic(),
			 dbg_code::invalid_argument,
			 dbg_flags::none,
			 "Trying to replace range of elements outside of sequence "
			 "range! count={}, end={}",
			 this->count_,
			 __wend);
			const size_t c = __wend - __wbeg;
			if(c < __count)
			{
				const size_t n = __count - c;
				this->reserve_with_magnifier(this->count_ + n);
				iterator_t wb = this->begin() + __wbeg;
				iterator_t we = this->begin() + __wend;
				iterator_t l	= wb + __count;
				pul::assign(wb, we, __val);
				memmove(l, we, distof(we, this->end()));
				construct(wb + c, l, __val);
				this->count_ += n;
			}
			else
			{
				iterator_t wb = this->begin() + __wbeg;
				iterator_t we = this->begin() + __wend;
				iterator_t l	= wb + __count;
				pul::assign(wb, we, __val);
				if(c > __count)
				{
					destroy(l, we);
					memcpy(l, we, distof(we, this->end()));
					this->count_ -= (c - __count);
				}
			}
			return __wbeg + __count;
		}
		template<typename _IteratorIn>
		pf_decl_constexpr size_t
		replace(
		 size_t __wbeg,
		 size_t __wend,
		 _IteratorIn __ibeg,
		 _IteratorIn __iend)
			requires(is_iterator_v<_IteratorIn>)
		{
			pf_throw_if(
			 __wbeg >= __wend,
			 dbg_category_generic(),
			 dbg_code::invalid_argument,
			 dbg_flags::none,
			 "Range of elements to replace is ill-formed! begin={}, end={}",
			 __wbeg,
			 __wend);
			pf_throw_if(
			 __wend > this->count_,
			 dbg_category_generic(),
			 dbg_code::invalid_argument,
			 dbg_flags::none,
			 "Trying to replace range of elements outside of sequence "
			 "range! count={}, end={}",
			 this->count_,
			 __wend);
			const size_t wc = __wend - __wbeg;
			const size_t ic = countof(__ibeg, __iend);
			if(wc < ic)
			{
				const size_t n = ic - wc;
				this->reserve_with_magnifier(this->count_ + n);
				iterator_t wb = this->begin() + __wbeg;
				iterator_t we = this->begin() + __wend;
				iterator_t l	= wb + ic;
				pul::assign(wb, we, __ibeg);
				memmove(l, we, distof(we, this->end()));
				construct(wb + wc, l, __ibeg + wc);
				this->count_ += n;
			}
			else
			{
				iterator_t wb = this->begin() + __wbeg;
				iterator_t we = this->begin() + __wend;
				iterator_t l	= wb + ic;
				pul::assign(wb, we, __ibeg);
				if(wc > ic)
				{
					destroy(l, we);
					memcpy(l, we, distof(we, this->end()));
					this->count_ -= (wc - ic);
				}
			}
			return __wbeg + ic;
		}
		template<typename _View>
		pf_decl_constexpr size_t
		replace(
		 size_t __wbeg,
		 size_t __wend,
		 _View __v)
			requires(
			 is_view_v<_View>
			 && std::is_same_v<_Ty, value_type_t<_View>>)
		{
			return this->replace(__wbeg, __wend, __v.begin(), __v.end());
		}
		pf_decl_constexpr size_t
		replace(
		 size_t __wbeg,
		 size_t __wend,
		 initializer_list<_Ty> __il)
		{
			return this->replace(__wbeg, __wend, iterator_t(__il.begin()), iterator_t(__il.end()));
		}


		/// Replace (Iterator)
		template<typename... _Args>
		pf_decl_constexpr iterator_t
		replace(
		 iterator_t __w,
		 _Args &&...__args)
			requires(std::is_constructible_v<_Ty, _Args...>)
		{
			return this->begin()
					 + this->replace(this->__iterator_to_index(__w), std::forward<_Args>(__args)...);
		}
		pf_decl_constexpr iterator_t
		replace(
		 iterator_t __w,
		 const _Ty &__val,
		 size_t __count)
		{
			return this->begin()
					 + this->replace(this->__iterator_to_index(__w), __val, __count);
		}
		template<typename _IteratorIn>
		pf_decl_constexpr iterator_t
		replace(
		 iterator_t __w,
		 _IteratorIn __beg,
		 _IteratorIn __end)
			requires(is_iterator_v<_IteratorIn>)
		{
			return this->begin()
					 + this->replace(this->__iterator_to_index(__w), __beg, __end);
		}
		template<typename _View>
		pf_decl_constexpr iterator_t
		replace(
		 iterator_t __w,
		 _View __v)
			requires(
			 is_view_v<_View>
			 && std::is_same_v<_Ty, value_type_t<_View>>)
		{
			return this->replace(__w, __v.begin(), __v.end());
		}
		pf_decl_constexpr iterator_t
		replace(
		 iterator_t __w,
		 initializer_list<_Ty> __il)
		{
			return this->replace(__w, iterator(__il.begin()), iterator(__il.end()));
		}

		pf_decl_constexpr iterator_t
		replace(
		 iterator_t __wbeg,
		 iterator_t __wend,
		 const _Ty &__val,
		 size_t __count)
		{
			return this->begin()
					 + this->replace(this->__iterator_to_index(__wbeg), this->__iterator_to_index(__wend), __val, __count);
		}
		template<typename _IteratorIn>
		pf_decl_constexpr iterator_t
		replace(
		 iterator_t __wbeg,
		 iterator_t __wend,
		 _IteratorIn __ibeg,
		 _IteratorIn __iend)
			requires(is_iterator_v<_IteratorIn>)
		{
			return this->begin()
					 + this->replace(this->__iterator_to_index(__wbeg), this->__iterator_to_index(__wend), __ibeg, __iend);
		}
		template<typename _View>
		pf_decl_constexpr iterator_t
		replace(
		 iterator_t __wbeg,
		 iterator_t __wend,
		 _View __v)
			requires(
			 is_view_v<_View>
			 && std::is_same_v<_Ty, value_type_t<_View>>)
		{
			return this->replace(__wbeg, __wend, __v.begin(), __v.end());
		}
		pf_decl_constexpr iterator_t
		replace(
		 iterator_t __wbeg,
		 iterator_t __wend,
		 initializer_list<_Ty> __il)
		{
			return this->replace(__wbeg, __wend, iterator(__il.begin()), iterator(__il.end()));
		}

		/// Shrink
		pf_decl_constexpr size_t
		shrink(
		 size_t __nc)
		{
			if(pf_unlikely(__nc < this->count_))
				__nc = this->count_;
			if(__nc < this->capacity_)
				return -this->__reallocate_no_check(__nc, this->align_);
			return 0;
		}
		pf_decl_constexpr size_t
		shrink_to_fit()
		{
			return this->shrink(this->count_);
		}
		pf_decl_constexpr size_t
		shrink_to_magnifier()
		{
			return this->shrink(this->magnifier_(this->count_));
		}

		/// Reserve
		pf_decl_constexpr size_t
		reserve(
		 size_t __count)
		{
			if(__count > this->capacity_)
			{
				return this->__reallocate_no_check(__count, this->align_);
			}
			return 0;
		}
		pf_decl_constexpr size_t
		reserve(
		 size_t __count,
		 align_val_t __align)
		{
			if(__count > this->capacity_)
			{
				return this->__reallocate_no_check(__count, __align);
			}
			if(__align != this->align_)
			{
				this->realign(__align);
			}
			return 0;
		}
		pf_decl_constexpr size_t
		reserve_with_magnifier(
		 size_t __count)
		{
			if(__count > this->capacity_)
			{
				return this->__reallocate_no_check(this->magnifier_(__count), this->align_);
			}
			return 0;
		}
		pf_decl_constexpr size_t
		reserve_with_magnifier(
		 size_t __count,
		 align_val_t __align)
		{
			if(__count > this->capacity_)
			{
				return this->__reallocate_no_check(this->magnifier_(__count), __align);
			}
			if(__align != this->align_)
			{
				this->realign(__align);
			}
			return 0;
		}

		/// Resize
		pf_decl_constexpr size_t
		resize(
		 const _Ty &__val,
		 size_t __count)
		{
			if(__count > this->count_)
			{
				this->reserve(__count);
				construct(this->begin() + this->count_, __val);
				this->count_ = __count;
			}
			return 0;
		}

		/// Realign
		pf_decl_constexpr bool
		realign(
		 align_val_t __align)
		{
			_Ty *p = this->data_;
			if(this->align_ != __align)
				this->__reallocate_no_check(this->capacity_, __align);
			return (this->data_ != p);
		}

		/// Clear
		pf_decl_constexpr size_t
		clear() pf_attr_noexcept
		{
			const size_t c = this->count_;
			if(this->data_)
			{
				destroy(this->begin(), this->end());
				deallocate(this->allocator_, this->data_);
				this->data_			= nullptr;
				this->capacity_ = 0;
				this->count_		= 0;
			}
			return c;
		}

		/// Front
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr _Ty &
		front() pf_attr_noexcept
		{
			return *this->begin();
		}
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const _Ty &
		front() const pf_attr_noexcept
		{
			return *this->begin();
		}

		/// Back
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr _Ty &
		back() pf_attr_noexcept
		{
			return *(this->begin() - 1 + this->count_);
		}
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const _Ty &
		back() const pf_attr_noexcept
		{
			return *(this->begin() - 1 + this->count_);
		}

		/// Begin
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr iterator_t
		begin() pf_attr_noexcept
		{
			return this->data_;
		}
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const_iterator_t
		begin() const pf_attr_noexcept
		{
			return this->data_;
		}
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const_iterator_t
		cbegin() const pf_attr_noexcept
		{
			return this->begin();
		}

		/// End
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr iterator_t
		end() pf_attr_noexcept
		{
			return (this->data_ + this->count_);
		}
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const_iterator_t
		end() const pf_attr_noexcept
		{
			return (this->data_ + this->count_);
		}
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const_iterator_t
		cend() const pf_attr_noexcept
		{
			return this->end();
		}

		/// Reverse Begin
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr reverse_iterator_t
		rbegin() pf_attr_noexcept
		{
			return (this->data_ - 1 + this->count_);
		}
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const_reverse_iterator_t
		rbegin() const pf_attr_noexcept
		{
			return (this->data_ - 1 + this->count_);
		}
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const_reverse_iterator_t
		crbegin() const pf_attr_noexcept
		{
			return this->rbegin();
		}

		/// Reverse End
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr reverse_iterator_t
		rend() pf_attr_noexcept
		{
			return (this->data_ - 1);
		}
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const_reverse_iterator_t
		rend() const pf_attr_noexcept
		{
			return (this->data_ - 1);
		}
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const_reverse_iterator_t
		crend() const pf_attr_noexcept
		{
			return this->rend();
		}

		/// Data
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr _Ty *
		data() pf_attr_noexcept
		{
			return this->data_;
		}
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const _Ty *
		data() const pf_attr_noexcept
		{
			return this->data_;
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

		/// Storage Size
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr size_t
		storage_size() const pf_attr_noexcept
		{
			return this->capacity_ * sizeof(_Ty);
		}

		/// Capacity
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr size_t
		capacity() const pf_attr_noexcept
		{
			return this->capacity_;
		}

		/// Align
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr align_val_t
		alignment() const pf_attr_noexcept
		{
			return this->align_;
		}

		/// View
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr view_t
		view() const pf_attr_noexcept
		{
			return view_t(this->begin(), this->end());
		}

		/// Magnifier
		pf_hint_nodiscard pf_decl_inline _Magnifier &
		magnifier() pf_attr_noexcept
		{
			return this->magnifier_;
		}
		pf_hint_nodiscard pf_decl_inline const _Magnifier &
		magnifier() const pf_attr_noexcept
		{
			return this->magnifier_;
		}

		/// Allocator
		pf_hint_nodiscard pf_decl_inline _Allocator &
		allocator() pf_attr_noexcept
		{
			return this->allocator_;
		}
		pf_hint_nodiscard pf_decl_inline const _Allocator &
		allocator() const pf_attr_noexcept
		{
			return this->allocator_;
		}

		/// Is Allocated
		pf_hint_nodiscard pf_decl_inline bool
		is_allocated() const pf_attr_noexcept
		{
			return this->capacity_ != 0;
		}

		/// Is Empty
		pf_hint_nodiscard pf_decl_inline bool
		is_empty() const pf_attr_noexcept
		{
			return this->count_ == 0;
		}

	private:
		_Ty *data_;
		size_t capacity_;
		align_val_t align_;
		size_t count_;
		pf_hint_nounique_address _Magnifier magnifier_;
		pf_hint_nounique_address _Allocator allocator_;
	};

	/// SEQUENCE: CTADs
	// iterator
	template<typename _IteratorIn>
		requires(is_iterator_v<_IteratorIn>)
	sequence(
	 _IteratorIn,
	 _IteratorIn)
	 -> sequence<
		std::remove_const_t<typename _IteratorIn::value_t>,
		magnifier_default,
		allocator_default>;
	template<typename _IteratorIn>
		requires(is_iterator_v<_IteratorIn>)
	sequence(
	 _IteratorIn,
	 _IteratorIn,
	 align_val_t)
	 -> sequence<
		std::remove_const_t<typename _IteratorIn::value_t>,
		magnifier_default,
		allocator_default>;
	template<typename _IteratorIn, typename _Magnifier>
		requires(is_iterator_v<_IteratorIn> && is_magnifier_v<_Magnifier>)
	sequence(
	 _IteratorIn,
	 _IteratorIn,
	 align_val_t,
	 _Magnifier &&)
	 -> sequence<
		std::remove_const_t<typename _IteratorIn::value_t>,
		_Magnifier,
		allocator_default>;
	template<typename _IteratorIn, typename _Magnifier, typename _Allocator>
		requires(is_iterator_v<_IteratorIn> && is_magnifier_v<_Magnifier> && is_allocator_v<_Allocator>)
	sequence(
	 _IteratorIn,
	 _IteratorIn,
	 align_val_t,
	 _Magnifier &&,
	 _Allocator &&)
	 -> sequence<
		std::remove_const_t<typename _IteratorIn::value_t>,
		_Magnifier,
		_Allocator>;

	// initializer_list
	template<typename _Ty>
	sequence(initializer_list<_Ty>) -> sequence<_Ty>;

	/// SEQUENCE: Concepts -> Tests
	pf_assert_static(is_view_v<sequence_view<int32_t>>);
	pf_assert_static(is_container_v<sequence<int32_t>>);
}	 // namespace pul

#endif	// !PULSAR_SEQUENCE_HPP
