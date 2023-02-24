/*! @file   char_traits_string.hpp
 *  @author Louis-Quentin Noé (noe.louis-quentin@hotmail.fr)
 *  @brief
 *  @date   02-01-2023
 *
 *  @copyright Copyright (c) 2023 - Pulsar Software
 *
 *  @since 0.1.3
 */

#ifndef PULSAR_CHAR_TRAITS_STRING_HPP
#define PULSAR_CHAR_TRAITS_STRING_HPP 1

// Include: Pulsar
#include "pulsar/encoding.hpp"

// Pulsar
namespace pul
{
	/// TRAITS: String
	template<
		typename _Traits,
		typename _Magnifier = magnifier_default,
		typename _Allocator = allocator_default>
	class char_traits_string
	{
	using __value_iterator_t			 = iterator<typename _Traits::value_t>;
	using __value_const_iterator_t = const_iterator<typename _Traits::value_t>;

	public:
		using traits									 = _Traits;
		using value_t									 = typename _Traits::value_t;
		using code_value_t						 = typename _Traits::code_value_t;
		using iterator_t							 = char_traits_iterator<char_traits_string<_Traits, _Magnifier, _Allocator>>;
		using const_iterator_t				 = char_traits_const_iterator<_Traits>;
		using reverse_iterator_t			 = char_traits_reverse_iterator<char_traits_string<_Traits, _Magnifier, _Allocator>>;
		using const_reverse_iterator_t = char_traits_const_reverse_iterator<_Traits>;

		/// Constructors
		pf_decl_constexpr
		char_traits_string(
			align_val_t __av	= align_val_t(16),
			_Magnifier  &&__m = _Magnifier(),
			_Allocator  &&__a = _Allocator())
			: store_(nullptr)
			, count_(0)
			, memCount_(0)
			, memAlign_(__av)
			, magnifier_(std::move(__m))
			, allocator_(std::move(__a))
		{}
		template<typename _IteratorIn>
		pf_decl_constexpr
		char_traits_string(
			_IteratorIn __beg,
			_IteratorIn __end,
			align_val_t __av	= align_val_t(16),
			_Magnifier  &&__m = _Magnifier(),
			_Allocator  &&__a = _Allocator())
		requires(is_sequence_iterator_v<_IteratorIn>
						 && std::is_convertible_v<typename _IteratorIn::value_t, value_t>)
			: store_(nullptr)
			, count_(0)
			, memCount_(0)
			, memAlign_(__av)
			, magnifier_(std::move(__m))
			, allocator_(std::move(__a))
		{
			this->__assign(__beg, __end);
			// TODO: Exception !traits::is_valid(this->store_)
		}
		template <size_t _Num>
		pf_decl_constexpr
		char_traits_string(
			const value_t (&__str)[_Num],
			align_val_t __av	= align_val_t(16),
			_Magnifier  &&__m = _Magnifier(),
			_Allocator  &&__a = _Allocator())
			: store_(nullptr)
			, count_(0)
			, memCount_(0)
			, memAlign_(__av)
			, magnifier_(std::move(__m))
			, allocator_(std::move(__a))
		{
			this->__assign(begin(__str), end(__str));
			// TODO: Exception !traits::is_valid(this->store_)
		}
		pf_decl_constexpr
		char_traits_string(
			char_traits_string_view<_Traits> __view,
			align_val_t __av = align_val_t(16),
			_Magnifier &&__m = _Magnifier(),
			_Allocator &&__a = _Allocator())
			: store_(nullptr)
			, count_(0)
			, memCount_(0)
			, memAlign_(__av)
			, magnifier_(std::move(__m))
			, allocator_(std::move(__a))
		{
			this->__assign(__view.begin(), __view.end());
		}
		template<typename _MagnifierR, typename _AllocatorR>
		pf_decl_constexpr
		char_traits_string(
			char_traits_string<_Traits, _MagnifierR, _AllocatorR> const &__r,
			align_val_t __av = align_val_t(16),
			_Magnifier &&__m = _Magnifier(),
			_Allocator &&__a = _Allocator())
			: store_(nullptr)
			, count_(0)
			, memCount_(0)
			, memAlign_(__av)
			, magnifier_(std::move(__m))
			, allocator_(std::move(__a))
		{
			this->__assign(__r.begin(), __r.end());
		}
		template<typename _MagnifierR>
		pf_decl_constexpr
		char_traits_string(
			char_traits_string<_Traits, _MagnifierR, _Allocator> &&__r,
			_Magnifier &&__m = _Magnifier()) pf_attr_noexcept
			: store_(__r.store_)
			, count_(__r.count_)
			, memCount_(__r.memCount_)
			, memAlign_(__r.memAlign_)
			, magnifier_(std::move(__m))
			, allocator_(std::move(__r.allocator_))
		{
			__r.store_		= nullptr;
			__r.count_		= 0;
			__r.memCount_ = 0;
		}
		pf_decl_constexpr
		char_traits_string(
			char_traits_string<_Traits, _Magnifier, _Allocator> &&__r) pf_attr_noexcept
			: store_(__r.store_)
			, count_(__r.count_)
			, memCount_(__r.memCount_)
			, memAlign_(__r.memAlign_)
			, magnifier_(std::move(__r.allocator_))
			, allocator_(std::move(__r.allocator_))
		{
			__r.store_		= nullptr;
			__r.count_		= 0;
			__r.memCount_ = 0;
		}

		/// Destructor
		pf_decl_constexpr ~char_traits_string() pf_attr_noexcept
		{
			this->clear();
		}

		/// Operator =
		pf_decl_constexpr char_traits_string<_Traits, _Magnifier, _Allocator> &
		operator=(
			char_traits_string_view<_Traits> __view)
		{
			this->__assign(__view.begin(), __view.end());
			return *this;
		}
		template<typename _MagnifierR, typename _AllocatorR>
		pf_decl_constexpr char_traits_string<_Traits, _Magnifier, _Allocator> &
		operator=(
			char_traits_string<_Traits, _MagnifierR, _AllocatorR> const &__r)
		{
			if(pf_likely(this != &__r))
			{
				this->__assign(__r.begin(), __r.end());
			}
			return *this;
		}
		template<typename _MagnifierR>
		pf_decl_constexpr char_traits_string<_Traits, _MagnifierR, _Allocator> &
		operator=(
			char_traits_string<_Traits, _MagnifierR, _Allocator> &&__r) pf_attr_noexcept
		{
			if(pf_likely(this != &__r))
			{
				this->store_		 = __r.store_;    __r.store_ = nullptr;
				this->count_		 = __r.count_;    __r.count_ = 0;
				this->memCount_	 = __r.memCount_; __r.memCount_ = 0;
				this->memAlign_	 = __r.memAlign_;
				this->magnifier_ = std::move(__r.magnifier_);
				this->allocator_ = std::move(__r.allocator_);
			}
			return *this;
		}

		/// Operator ==
		pf_decl_constexpr bool
		operator==(char_traits_string_view<_Traits> __view) pf_attr_noexcept
		{
			return this->to_span() == __view;
		}
		pf_decl_constexpr bool
		operator!=(char_traits_string_view<_Traits> __view) pf_attr_noexcept
		{
			return this->to_span() != __view;
		}

		/// Operator (char_traits_string_view<_Traits>)
		pf_decl_constexpr operator char_traits_string_view<_Traits>() const pf_attr_noexcept
		{
			return this->to_span();
		}

		/// Operator (_Ty *)
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

		/// Insert
		pf_decl_constexpr iterator_t
		insert_back(
			size_t __c,
			code_value_t __cv)
		{
			size_t t = __c;
			if (this->is_empty()) ++t;
			this->reserve(this->count_ + t);
			const size_t len = traits::codelenof(__cv);
			auto b					 = this->__end_str_as_value();
			auto k					 = b;
			auto e					 = b + __c * len;
			for (; b != e;) b += this->__write_char_code_at(b, __cv, len);// TODO: Check if it memset?
			this->__write_char_code_at(e, traits::null_code());
			this->count_ += __c;
			return k;
		}
		template<typename _IteratorIn>
		pf_decl_constexpr iterator_t
		insert_back(
			_IteratorIn __beg,
			_IteratorIn __end)
		{
			size_t t = distof(__beg, __end);
			size_t h = 0;
			if (this->is_empty()) ++h;
			this->reserve(this->count_ + t + h);
			auto b = this->__end_str_as_value();
			auto k = b;
			auto e = b + t;
			copy_forward(__beg, __end, b);
			this->__write_char_code_at(e, traits::null_code());
			this->count_ += t;
			return k;
		}
		pf_decl_constexpr iterator_t
		insert_back(
			char_traits_string_view<_Traits> __view)
		{
			return this->insert_back(__view.begin(), __view.end());
		}

		pf_decl_constexpr iterator_t
		insert(
			iterator_t __w,
			size_t __c,
			code_value_t __cv)
		{
			if (__w.get() > this->__end_str_as_value()) this->insert_back(__c, __cv);
			// TODO: insert
		}
		template<typename _IteratorIn>
		pf_decl_constexpr iterator_t
		insert(
			iterator_t __w,
			_IteratorIn __beg,
			_IteratorIn __end)
		{
			if (__w.get() > this->__end_str_as_value()) this->insert_back(__w, __beg, __end);
			// TODO: insert
		}
		pf_decl_constexpr iterator_t
		insert(iterator_t __w,
					 char_traits_string_view<_Traits> const &__r)
		{
			return this->insert_back(__r.begin(), __r.end());
		}

		/// Replace
		pf_decl_constexpr iterator_t
		replace(
			iterator_t __wb,
			iterator_t __we,
			size_t __c,
			code_value_t __cv)
		{
			// TODO: replace
		}
		template<typename _Ty>
		pf_decl_constexpr reverse_iterator_t
		replace(
			reverse_iterator_t __wb,
			reverse_iterator_t __we,
			size_t __c,
			code_value_t __cv)
		{
			return this->replace(++__we, ++__wb, __c, __cv);
		}
		pf_decl_constexpr iterator_t
		replace(
			iterator_t __w,
			size_t __c,
			code_value_t __cv)
		{
			return this->replace(__w, __w + 1, __c, __cv);
		}
		template<typename _IteratorIn>
		pf_decl_constexpr iterator_t
		replace(
			iterator_t __wb,
			iterator_t __we,
			_IteratorIn __beg,
			_IteratorIn __end)
		{
			// TODO: replace
		}
		template<typename _IteratorIn>
		pf_decl_constexpr reverse_iterator_t
		replace(
			reverse_iterator_t __wb,
			reverse_iterator_t __we,
			_IteratorIn __beg,
			_IteratorIn __end)
		{
			return this->replace(++__we, ++__wb, __beg, __end);
		}
		template<typename _IteratorIn>
		pf_decl_constexpr iterator_t
		replace(
			iterator_t __wb,
			iterator_t __we,
			char_traits_string_view<_Traits> __view)
		{
			return this->replace(__wb, __we, __view.begin(), __view.end());
		}
		pf_decl_constexpr reverse_iterator_t
		replace(
			reverse_iterator_t __wb,
			reverse_iterator_t __we,
			char_traits_string_view<_Traits> __view)
		{
			return this->replace(++__we, ++__wb, __view.begin(), __view.end());
		}
		template<typename _IteratorIn>
		pf_decl_constexpr iterator_t
		replace(
			iterator_t __w,
			_IteratorIn __beg,
			_IteratorIn __end)
		{
			// TODO
		}
		pf_decl_constexpr iterator_t
		replace(
			iterator_t __w,
			char_traits_string_view<_Traits> __il)
		{
			return this->replace(__w, __il.begin(), __il.end());
		}

		/// Erase
		pf_decl_constexpr iterator_t
		erase(iterator_t __wb,
					iterator_t __we)
		{
			if (__we = this->end()) --__we;	// NOTE: this->end() == traits::null_code();
			copy_forward(__we, this->end(), __wb);
			this->count_ -= distof(__wb, __we) / sizeof(value_t);
			return __wb;
		}
		pf_decl_constexpr iterator_t
		erase(iterator_t __w)
		{
			return this->erase(__w, __w + 1);
		}
		pf_decl_constexpr reverse_iterator_t
		erase(reverse_iterator_t __wb,
					reverse_iterator_t __we)
		{
			return this->erase(++__we, ++__wb);
		}

		/// Reserve
		pf_decl_constexpr size_t
		reserve_no_magnifier(
			size_t __c,
			align_val_t __av)
		{
			const size_t c = this->memCount_;
			if (c < __c || __av != this->memAlign_)
			{
				this->__reallocate_no_check(__c, __av);
			}
			return __c - c;
		}
		pf_decl_constexpr size_t
		reserve_no_magnifier(
			size_t __c)
		{
			const size_t c = this->memCount_;
			if (c < __c)
			{
				this->__reallocate_no_check(__c, this->memAlign_);
			}
			return __c - c;
		}
		pf_decl_constexpr size_t
		reserve(
			size_t __c,
			align_val_t __av)
		{
			const size_t c = this->memCount_;
			__c = this->magnifier_(__c);
			if (c < __c || __av != this->memAlign_)
			{
				this->__reallocate_no_check(__c, __av);
			}
			return __c - c;
		}
		pf_decl_constexpr size_t
		reserve(
			size_t __c)
		{
			const size_t c = this->memCount_;
			__c = this->magnifier_(__c);
			if (c < __c)
			{
				this->__reallocate_no_check(__c, this->memAlign_);
			}
			return __c - c;
		}

		/// Realign
		pf_decl_constexpr void
		realign(
			align_val_t __av)
		{
			if (this->memAlign_ != __av)
			{
				this->__reallocate_no_check(this->count_ + 1, __av);
			}
		}

		/// Resize
		pf_decl_constexpr size_t
		resize_no_magnifier(
			size_t __c,
			code_value_t __val)
		{
			return this->__resize(__c, this->memAlign_, __val);
		}
		pf_decl_constexpr size_t
		resize_no_magnifier(
			size_t __c,
			align_val_t __av,
			code_value_t __val)
		{
			return this->__resize(__c, __av, __val);
		}
		pf_decl_constexpr size_t
		resize(
			size_t __c,
			code_value_t __val)
		{
			return this->__resize(this->magnifier_(__c), this->memAlign_, __val);
		}
		pf_decl_constexpr size_t
		resize(
			size_t __c,
			align_val_t __av,
			code_value_t __val)
		{
			return this->__resize(this->magnifier_(__c), __av, __val);
		}

		/// Begin
		pf_hint_nodiscard pf_decl_constexpr iterator_t
		begin() pf_attr_noexcept
		{
			return { &this->store_[0], this };
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

		/// End
		pf_hint_nodiscard pf_decl_constexpr iterator_t
		end() pf_attr_noexcept
		{
			return { &this->store_[0] + (this->count_), this };
		}
		pf_hint_nodiscard pf_decl_constexpr const_iterator_t
		end() const pf_attr_noexcept
		{
			return &this->store_[0] + (this->count_);
		}
		pf_hint_nodiscard pf_decl_constexpr const_iterator_t
		cend() const pf_attr_noexcept
		{
			return &this->store_[0] + (this->count_);
		}

		/// Reverse Begin
		pf_hint_nodiscard pf_decl_constexpr reverse_iterator_t
		rbegin() pf_attr_noexcept
		{
			return {&this->store_[0] + (this->count_ - 1), this};
		}
		pf_hint_nodiscard pf_decl_constexpr const_reverse_iterator_t
		rbegin() const pf_attr_noexcept
		{
			return &this->store_[0] + (this->count_ - 1);
		}
		pf_hint_nodiscard pf_decl_constexpr const_reverse_iterator_t
		rcbegin() const pf_attr_noexcept
		{
			return &this->store_[0] + (this->count_ - 1);
		}

		/// Reverse End
		pf_hint_nodiscard pf_decl_constexpr reverse_iterator_t
		rend() pf_attr_noexcept
		{
			return {&this->store_[0] - 1, this};
		}
		pf_hint_nodiscard pf_decl_constexpr const_reverse_iterator_t
		rend() const pf_attr_noexcept
		{
			return &this->store_[0] - 1;
		}
		pf_hint_nodiscard pf_decl_constexpr const_reverse_iterator_t
		rcend() const pf_attr_noexcept
		{
			return &this->store_[0] - 1;
		}

		/// Size
		pf_hint_nodiscard pf_decl_constexpr size_t
		size() const pf_attr_noexcept
		{
			return distof(this->begin(), this->end());
		}

		/// Code Count
		pf_hint_nodiscard pf_decl_constexpr size_t
		code_count() const pf_attr_noexcept
		{
			return countof(this->begin(), this->end());
		}

		/// Count
		pf_hint_nodiscard pf_decl_constexpr size_t
		count() const pf_attr_noexcept
		{
			return this->size() / sizeof(value_t);
		}

		/// Capacity
		pf_hint_nodiscard pf_decl_constexpr size_t
		capacity() const pf_attr_noexcept
		{
			return this->memCount_;
		}

		/// Alignment
		pf_hint_nodiscard pf_decl_constexpr align_val_t
		alignment() const pf_attr_noexcept
		{
			return this->memAlign_;
		}

		/// Magnifier
		pf_hint_nodiscard pf_decl_constexpr _Magnifier const &
		magnifier() const pf_attr_noexcept
		{
			return this->magnifier_;
		}

		/// Allocator
		pf_hint_nodiscard pf_decl_constexpr _Allocator const &
		allocator() const pf_attr_noexcept
		{
			return this->allocator_;
		}

		/// To Span
		pf_hint_nodiscard pf_decl_constexpr char_traits_string_view<_Traits>
		to_span() const pf_attr_noexcept
		{
			return char_traits_string_view<_Traits>(this->begin(), this->end());
		}

		/// Data
		pf_hint_nodiscard pf_decl_constexpr value_t*
		data() pf_attr_noexcept
		{
			return &this->store_[0];
		}
		pf_hint_nodiscard pf_decl_constexpr const value_t*
		data() const pf_attr_noexcept
		{
			return &this->store_[0];
		}

		/// Shrink
		pf_decl_constexpr size_t
		shrink_to_fit()
		{
			return this->__shrink(this->count_);
		}
		pf_decl_constexpr size_t
		shrink_to_magnifier()
		{
			return this->__shrink(this->magnifier_(this->count_));
		}

		/// Clear
		pf_decl_constexpr void
		clear()
		{
			if (this->store_)
			{
				deallocate(this->store_);
				this->store_		= nullptr;
				this->count_		= 0;
				this->memCount_ = 0;
			}
		}

		/// Is Empty
		pf_hint_nodiscard pf_decl_constexpr bool
		is_empty() const pf_attr_noexcept
		{
			return this->count_ == 0;
		}
		pf_hint_nodiscard pf_decl_constexpr bool
		is_valid() const pf_attr_noexcept
		{
			return traits::is_valid(this->data(), this->size());
		}

	private:
		/// __begin / __end as value
		pf_decl_constexpr pf_decl_inline __value_iterator_t
		__begin_as_value() pf_attr_noexcept
		{
			return &this->store_[0];
		}
		// NOTE: We DON'T exclude the last '\0' for memory operations purposes
		pf_decl_constexpr pf_decl_inline __value_iterator_t
		__end_as_value() pf_attr_noexcept
		{
			return &this->store_[0] + this->count_;
		}
		pf_decl_constexpr pf_decl_inline __value_iterator_t
		__end_str_as_value() pf_attr_noexcept
		{
			return &this->store_[0] + (this->count_ + traits::codelenof(traits::null_code()));
		}

		/// __fill_at
		pf_decl_constexpr pf_decl_inline __value_iterator_t
		__fill_at(
			__value_iterator_t __w,
			size_t __c,
			code_value_t __val,
			size_t __len) pf_attr_noexcept
		{
			// TODO: __fill_at
		}
		pf_decl_constexpr pf_decl_inline __value_iterator_t
		__fill_at(
			__value_iterator_t __w,
			size_t __c,
			code_value_t __val) pf_attr_noexcept
		{
			return this->__fill_at(__w, __c, __val, traits::codelenof(__val));
		}

		/// __write_at
		template <typename _IteratorIn>
		pf_decl_constexpr pf_decl_inline __value_iterator_t
		__write_at(
			__value_iterator_t __w,
			_IteratorIn __beg,
			_IteratorIn __end) pf_attr_noexcept
		{
			// TODO: __write_at
		}

		/// __write_char_code at
		pf_decl_constexpr pf_decl_inline __value_iterator_t
		__write_char_code_at(
			__value_iterator_t __w,
			code_value_t __val,
			size_t __len) pf_attr_noexcept
		{
			pf_assert(traits::codelenof(__val) == __len, "__len isn't equal to traits::codelenof(__val)!");
			__value_iterator_t b = union_cast<value_t*>(&__val);
			__value_iterator_t e = b + __len;
			copy_forward(b, e, __w);
			return e;
		}
		pf_decl_constexpr pf_decl_inline __value_iterator_t
		__write_char_code_at(
			__value_iterator_t __w,
			code_value_t __val) pf_attr_noexcept
		{
			return this->__write_char_code_at(__w, __val, traits::codelenof(__val));
		}

		/// __reallocate_no_check
		pf_decl_constexpr void
		__reallocate_no_check(
			size_t __count,
			align_val_t __align)
		{
			this->store_ = union_cast<value_t*>(
				reallocate(
					this->allocator_,
					this->store_,
					this->memCount_,
					__count * sizeof(value_t),
					__align));
			this->memCount_ = __count;
			this->memAlign_ = __align;
		}

		/// __assign
		template<typename _IteratorIn>
		pf_decl_constexpr void
		__assign(
			_IteratorIn __beg,
			_IteratorIn __end)
		requires(is_sequence_iterator_v<_IteratorIn> &&
						 (std::is_same_v<typename _IteratorIn::value_t, typename traits::value_t>
							|| std::is_same_v<typename _IteratorIn::value_t, typename traits::code_value_t>))
		{
			size_t c			= distof(__beg, __end);
			auto last			= traits::to_code_point((__end - 1));
			auto null			= traits::null_code();
			auto nullen		= traits::codelenof(null);
			auto havenull = traits::is_null_code(last);
			this->reserve(c + (havenull ? 0 : 1));
			// TODO: this->__write_at
			copy_forward(
				__value_iterator_t(union_cast<value_t*>(__beg.get())),
				__value_iterator_t(union_cast<value_t*>(__end.get())),
				this->__begin_as_value());
			this->__write_char_code_at(this->__begin_as_value() + c, null, nullen);
			this->count_ += c;
		}

		/// resize
		pf_decl_constexpr size_t
		__resize(
			size_t __c,
			align_val_t __align,
			code_value_t __val) pf_attr_noexcept
		{
			if (__c > this->count_)
			{
				this->__reallocate_no_check(__c, __align);
				const size_t len = traits::codelenof(__val);
				__c -= this->count_;
				__c *= __c / len + __c % len == 0 ? 0 : 1;// TODO: Optimize
				auto b = this->__end_str_as_value();
				auto e = b + __c;
				for (; b != e;) b += this->__write_char_code_at(b, __val, len);
				if (this->is_empty()) ++__c;
				this->__write_char_code_at(b, traits::null_code());
				this->count_ += __c;
			}
			else if (__c < this->count_)
			{
				auto b = this->__begin_as_value();
				auto k = b;
				auto e = this->__begin_as_value() + __c;
				for (; b < e; ++b);
				this->__write_char_code_at(b, traits::null_code());
				this->count_ = distof(k, b);
			}
			return 0;
		}

		/// shrink
		pf_decl_constexpr size_t
		__shrink(
			size_t __c) pf_attr_noexcept
		{
			if (__c < this->memCount_)
			{
				this->__reallocate_no_check(__c, this->memAlign_);
				return this->memCount_ - __c;
			}
			return 0;
		}

		value_t *store_;
		size_t count_;
		size_t memCount_;
		align_val_t memAlign_;
		pf_hint_nounique_address _Magnifier magnifier_;
		pf_hint_nounique_address _Allocator allocator_;
	};

	/// TRAITS: String -> Alias
	template<typename _Magnifier, typename _Allocator>
	using a7string	 = char_traits_string<char_traits::ascii, _Magnifier, _Allocator>;
	using a7string_t = char_traits_string<char_traits::ascii>;
	// TODO: template<typename _Magnifier, typename _Allocator>
	// TODO: using wsstring	 = char_traits_string<char_traits::wide, _Magnifier, _Allocator>;
	// TODO: using wsstring_t = char_traits_string<char_traits::wide>;
	// TODO: template<typename _Magnifier, typename _Allocator>
	// TODO: using u8string	 = char_traits_string<char_traits::utf8, _Magnifier, _Allocator>;
	// TODO: using u8string_t = char_traits_string<char_traits::utf8>;


	/// TRAITS: Iterator -> Valid?
	pf_static_assert(is_sequence_iterator_v<char_traits_iterator<char_traits_string<char_traits::ascii, magnifier_default, allocator_default>>>);
	pf_static_assert(is_sequence_iterator_v<char_traits_const_iterator<char_traits::ascii>>);
	pf_static_assert(is_sequence_iterator_v<char_traits_reverse_iterator<char_traits_string<char_traits::ascii, magnifier_default, allocator_default>>>);
	pf_static_assert(is_sequence_iterator_v<char_traits_const_reverse_iterator<char_traits::ascii>>);
}

#endif // !PULSAR_CHAR_TRAITS_STRING_HPP