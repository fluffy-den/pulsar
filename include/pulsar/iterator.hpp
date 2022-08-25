/*! @file   iterator.hpp
 *  @author Fluffy (noe.louis-quentin@hotmail.fr)
 *  @brief	Adds the iterators.
 *  @date   24-05-2022
 *
 *  @copyright Copyright (c) 2022 - Pulsar Software
 *
 *  @since 0.1.1
 */

#ifndef PULSAR_ITERATOR_HPP
#define PULSAR_ITERATOR_HPP 1

// Include: Pulsar
#include "pulsar/pulsar.hpp"

// Pulsar
namespace pul
{
	/// ITERATOR: Standard
	/*! @brief Iterator.
	 *
	 *  @tparam _Ty Type encapsulated by the iterator.
	 */
	template <typename _Ty>
	class iterator
	{
	public:
		using iterator_category = std::random_access_iterator_tag;
		using value_type				= _Ty;
		using difference_type		= diff_t;

		/// Constructors
		/*! @brief Constructor.
		 *
		 *  @param[in] __ptr Pointer.
		 *  @param[in] __off (optional) Iterator's offset.
		 */
		pf_decl_constexpr iterator(
				_Ty *__ptr,
				diff_t __off = sizeof(_Ty)) pf_attr_noexcept
				: ptr_(__ptr)
				, off_(__off)
		{
			pf_assert(static_cast<size_t>(this->off_) >= sizeof(_Ty), "__off isn't greater or equal than sizeof(_Ty)");
		}
		/*! @brief Copy constructor.
		 *
		 *  @param[in] __r Other iterator.
		 */
		pf_decl_constexpr iterator(
				iterator<_Ty> const &__r) pf_attr_noexcept
				: ptr_(__r.ptr_)
				, off_(__r.off_)
		{
			pf_assert(static_cast<size_t>(this->off_) >= sizeof(_Ty), "__off isn't greater or equal than sizeof(_Ty)");
		}
		/*! @brief Copy constructor.
		 *
		 *  @param[in] __r 	 Other iterator.
		 *  @param[in] __off Iterator's offset.
		 */
		pf_decl_constexpr iterator(
				iterator<_Ty> const &__r,
				diff_t __off) pf_attr_noexcept
				: ptr_(__r.ptr_)
				, off_(__off)
		{
			pf_assert(static_cast<size_t>(this->off_) >= sizeof(_Ty), "__off isn't greater or equal than sizeof(_Ty)");
		}

		/// Operator=
		/*! @brief Copy assignment operator.
		 *
		 *  @param[in] __r	 Other iterator.
		 *  @return pf_decl_constexpr&
		 */
		pf_decl_constexpr iterator<_Ty> &operator=(
				iterator<_Ty> const &__r) pf_attr_noexcept
		{
			pf_assert(this->off_ >= sizeof(_Ty), "__off isn't greater or equal than sizeof(_Ty)");
			this->ptr_ = __r.ptr_;
			this->off_ = __r.off_;
			return *this;
		}

		/// Operator++
		/*! @brief Pre-incremental operator.
		 *
		 *  @return Pre-incremented iterator.
		 */
		pf_decl_constexpr iterator<_Ty> &operator++() pf_attr_noexcept
		{
			union
			{
				byte_t *as_byte;
				_Ty *as_type;
			};
			as_type = this->ptr_;
			as_byte += this->off_;
			this->ptr_ = as_type;
			return *this;
		}
		/*! @brief Post-incremental operator.
		 *
		 *  @param[in] __i Identifier.
		 *  @return Copy of iterator before incrementation.
		 */
		pf_decl_constexpr iterator<_Ty> operator++(
				int32_t __i) pf_attr_noexcept
		{
			iterator<_Ty> i = *this;
			++i;
			return i;
		}

		/// Operator+=
		/*! @brief Addition operator.
		 *
		 *  @param[in] __i Number of offset to apply.
		 *  @return Iterator that satisfy this->ptr_ + this->off_ * __i.
		 */
		pf_decl_constexpr iterator<_Ty> &operator+=(
				diff_t __i) pf_attr_noexcept
		{
			union
			{
				byte_t *as_byte;
				_Ty *as_type;
			};
			as_type = this->ptr_;
			as_byte += __i;
			return as_type;
		}

		/// Operator--
		/*! @brief Pre-decremental operator.
		 *
		 *  @return Pre-decremented iterator.
		 */
		pf_decl_constexpr iterator<_Ty> &operator--() pf_attr_noexcept
		{
			union
			{
				byte_t *as_byte;
				_Ty *as_type;
			};
			as_type = this->ptr_;
			as_byte -= this->off_;
			this->ptr_ = as_type;
			return *this;
		}
		/*! @brief Post-decremental operator.
		 *
		 *  @param[in] __i Identifier.
		 *  @return Copy of iterator before decrementation.
		 */
		pf_decl_constexpr iterator<_Ty> operator--(
				int32_t __i) pf_attr_noexcept
		{
			iterator<_Ty> i = *this;
			--i;
			return i;
		}

		/// Operator-=
		/*! @brief Substraction operator.
		 *
		 *  @param[in] __i Number of offset to apply.
		 *  @return Iterator that satisfy this->ptr_ - this->off_ * __i.
		 */
		pf_decl_constexpr iterator<_Ty> &operator-=(
				diff_t __i) pf_attr_noexcept
		{
			union
			{
				byte_t *as_byte;
				_Ty *as_type;
			};
			as_type = this->ptr_;
			as_byte -= __i;
			return as_type;
		}

		/// Operator*
		/*! @brief Deference operator.
		 *
		 *  @return Ref-value of encapsulated pointer.
		 */
		pf_hint_nodiscard pf_decl_constexpr _Ty &operator*() const pf_attr_noexcept
		{
			return *this->ptr_;
		}

		/// Operator->
		/*! @brief Arrow operator.
		 *
		 *  @return Encapsulated pointer.
		 */
		pf_hint_nodiscard pf_decl_constexpr _Ty *operator->() const pf_attr_noexcept
		{
			return this->ptr_;
		}

		/// Operator==
		/*! @brief Equality operator.
		 *
		 *  @tparam _OtherType Other iterator encapsulated pointer type.
		 *  @param[in] __r Other iterator.
		 *  @return True if equals.
		 *  @return False otherwise.
		 */
		template <typename _OtherTy>
		pf_hint_nodiscard pf_decl_constexpr bool operator==(
				iterator<_OtherTy> const &__r) const pf_attr_noexcept
		{
			union
			{
				_Ty *as_type;
				size_t as_addr;
			};
			as_type = this->ptr_;
			union
			{
				_OtherTy *as_other;
				size_t as_otherAddr;
			};
			as_other = __r.ptr_;
			return as_addr == as_otherAddr;
		}

		/// Operator<=>
		/*! @brief Spaceship operator.
		 *
		 *  @tparam _OtherType Other iterator encapsulated pointer type.
		 *  @param[in] __r Other iterator.
		 *  @return std::strong_ordering.
		 */
		template <typename _OtherTy>
		pf_hint_nodiscard pf_decl_constexpr std::strong_ordering operator<=>(
				iterator<_OtherTy> const &__r) const pf_attr_noexcept
		{
			union
			{
				_Ty *as_type;
				size_t as_addr;
			};
			as_type = this->ptr_;
			union
			{
				_OtherTy *as_other;
				size_t as_otherAddr;
			};
			as_other = __r.ptr_;
			return as_addr <=> as_otherAddr;
		}

		/// Set Offset
		/*! @brief Set the offset value of the iterator.
		 *
		 *  @param[in] __off New offset to set.
		 */
		pf_decl_constexpr void set_offset(
				diff_t __off)
		{
			this->off_ = __off;
		}

		/// Get Offset
		/*! @brief Gets the current offset value of the iterator.
		 *
		 *  @return Current offset value.
		 */
		pf_hint_nodiscard pf_decl_constexpr diff_t get_offset() const pf_attr_noexcept
		{
			return this->off_;
		}

		/// Base
		/*! @brief Gets the encapsulated pointer of the iterator.
		 *
		 *  @return Encapsulated pointer.
		 */
		pf_hint_nodiscard pf_decl_constexpr _Ty *base() const pf_attr_noexcept
		{
			return this->ptr_;
		}

	private:
		_Ty *ptr_;
		diff_t off_;
	};

	/// ITERATOR: Sequence Const
	/*! @brief Const sequence iterator specialization.
	 *
	 *  @tparam _Ty Const type encapsulated by the iterator.
	 */
	template <typename _Ty>
	class iterator<const _Ty>
	{
	public:
		using iterator_category = std::random_access_iterator_tag;
		using value_type				= const _Ty;
		using difference_type		= diff_t;

		/// Constructors
		/*! @brief Constructor.
		 *
		 *  @param[in] __ptr Pointer.
		 *  @param[in] __off (optional) Iterator's offset.
		 */
		pf_decl_constexpr iterator(
				const _Ty *__ptr,
				diff_t __off = sizeof(_Ty)) pf_attr_noexcept
				: ptr_(__ptr)
				, off_(__off)
		{
			pf_assert(this->off_ >= sizeof(_Ty), "__off isn't greater or equal than sizeof(_Ty)");
		}
		/*! @brief Copy constructor.
		 *
		 *  @param[in] __r Other iterator.
		 */
		pf_decl_constexpr iterator(
				iterator<const _Ty> const &__r) pf_attr_noexcept
				: ptr_(__r.ptr_)
				, off_(__r.off_)
		{
			pf_assert(this->off_ >= sizeof(_Ty), "__off isn't greater or equal than sizeof(_Ty)");
		}
		/*! @brief Copy constructor.
		 *
		 *  @param[in] __r 	 Other iterator.
		 *  @param[in] __off Iterator's offset.
		 */
		pf_decl_constexpr iterator(
				iterator<const _Ty> const &__r,
				diff_t __off) pf_attr_noexcept
				: ptr_(__r.ptr_)
				, off_(__off)
		{
			pf_assert(this->off_ >= sizeof(_Ty), "__off isn't greater or equal than sizeof(_Ty)");
		}

		/// Operator=
		/*! @brief Copy assignment operator.
		 *
		 *  @param[in] __r	 Other iterator.
		 *  @return pf_decl_constexpr&
		 */
		pf_decl_constexpr iterator<const _Ty> &operator=(
				iterator<const _Ty> const &__r) pf_attr_noexcept
		{
			this->ptr_ = __r.ptr_;
			this->off_ = __r.off_;
			pf_assert(this->off_ >= sizeof(_Ty), "__off isn't greater or equal than sizeof(_Ty)");
			return *this;
		}

		/// Operator++
		/*! @brief Pre-incremental operator.
		 *
		 *  @return Pre-incremented iterator.
		 */
		pf_decl_constexpr iterator<const _Ty> &operator++() pf_attr_noexcept
		{
			union
			{
				byte_t *as_byte;
				_Ty *as_type;
			};
			as_type = this->ptr_;
			as_byte += this->off_;
			this->ptr_ = as_type;
			return *this;
		}
		/*! @brief Post-incremental operator.
		 *
		 *  @param[in] __i Identifier.
		 *  @return Copy of iterator before incrementation.
		 */
		pf_decl_constexpr iterator<const _Ty> operator++(
				int32_t __i) pf_attr_noexcept
		{
			iterator<const _Ty> i = *this;
			++i;
			return i;
		}

		/// Operator+=
		/*! @brief Addition operator.
		 *
		 *  @param[in] __i Number of offset to apply.
		 *  @return Iterator that satisfy this->ptr_ + this->off_ * __i.
		 */
		pf_decl_constexpr iterator<const _Ty> &operator+=(
				diff_t __i) pf_attr_noexcept
		{
			union
			{
				byte_t *as_byte;
				_Ty *as_type;
			};
			as_type = this->ptr_;
			as_byte += __i;
			return as_type;
		}

		/// Operator--
		/*! @brief Pre-decremental operator.
		 *
		 *  @return Pre-decremented iterator.
		 */
		pf_decl_constexpr iterator<const _Ty> &operator--() pf_attr_noexcept
		{
			union
			{
				byte_t *as_byte;
				_Ty *as_type;
			};
			as_type = this->ptr_;
			as_byte -= this->off_;
			this->ptr_ = as_type;
			return *this;
		}
		/*! @brief Post-decremental operator.
		 *
		 *  @param[in] __i Identifier.
		 *  @return Copy of iterator before decrementation.
		 */
		pf_decl_constexpr iterator<const _Ty> operator--(
				int32_t __i) pf_attr_noexcept
		{
			iterator<const _Ty> i = *this;
			--i;
			return i;
		}

		/// Operator-=
		/*! @brief Subtraction operator.
		 *
		 *  @param[in] __i Number of offset to apply.
		 *  @return Iterator that satisfy this->ptr_ - this->off_ * __i.
		 */
		pf_decl_constexpr iterator<const _Ty> &operator-=(
				diff_t __i) pf_attr_noexcept
		{
			union
			{
				byte_t *as_byte;
				_Ty *as_type;
			};
			as_type = this->ptr_;
			as_byte -= __i;
			return as_type;
		}

		/// Operator*
		/*! @brief Deference operator.
		 *
		 *  @return Ref-value of encapsulated pointer.
		 */
		pf_hint_nodiscard pf_decl_constexpr const _Ty &operator*() const pf_attr_noexcept
		{
			return *this->ptr_;
		}

		/// Operator->
		/*! @brief Arrow operator.
		 *
		 *  @return Encapsulated pointer.
		 */
		pf_hint_nodiscard pf_decl_constexpr const _Ty *operator->() const pf_attr_noexcept
		{
			return this->ptr_;
		}

		/// Operator==
		/*! @brief Equality operator.
		 *
		 *  @tparam _OtherTy Other iterator encapsulated pointer type.
		 *  @param[in] __r Other iterator.
		 *  @return pf_decl_constexpr
		 */
		template <typename _OtherTy>
		pf_hint_nodiscard pf_decl_constexpr bool operator==(
				iterator<_OtherTy> const &__r) const pf_attr_noexcept
		{
			union
			{
				const _Ty *as_type;
				size_t as_addr;
			};
			as_type = this->ptr_;
			union
			{
				_OtherTy *as_other;
				size_t as_otherAddr;
			};
			as_other = __r.ptr_;
			return as_addr == as_otherAddr;
		}
		/*! @brief Equality operator.
		 *
		 *	@tparam _OtherTy Other iterator encapsulated pointer type.
		 *  @param[in] __r Other iterator.
		 *  @return True if equals.
		 *  @return False otherwise.
		 */
		template <typename _OtherTy>
		pf_hint_nodiscard pf_decl_constexpr bool operator==(
				const _Ty *__r) const pf_attr_noexcept
		{
			union
			{
				_Ty *as_type;
				size_t as_addr;
			};
			as_type = this->ptr_;
			union
			{
				_OtherTy *as_other;
				size_t as_otherAddr;
			};
			as_other = __r.ptr_;
			return as_addr == as_otherAddr;
		}

		/// Operator<=>
		/*! @brief Spaceship operator.
		 *
		 *  @tparam _OtherTy Other iterator encapsulated pointer type.
		 *  @param[in] __r Other iterator.
		 *  @return std::strong_ordering.
		 */
		template <typename _OtherTy>
		pf_hint_nodiscard pf_decl_constexpr std::strong_ordering operator<=>(
				iterator<const _Ty> const &__r) const pf_attr_noexcept
		{
			union
			{
				_Ty *as_type;
				size_t as_addr;
			};
			as_type = this->ptr_;
			union
			{
				_OtherTy *as_other;
				size_t as_otherAddr;
			};
			as_other = __r.ptr_;
			return as_addr <=> as_otherAddr;
		}

		/// Set Offset
		/*! @brief Set the offset value of the iterator.
		 *
		 *  @param[in] __off New offset to set.
		 */
		pf_decl_constexpr void set_offset(
				diff_t __off = sizeof(_Ty))
		{
			this->off_ = __off;
		}

		/// Get Offset
		/*! @brief Gets the current offset value of the iterator.
		 *
		 *  @return Current offset value.
		 */
		pf_hint_nodiscard pf_decl_constexpr diff_t get_offset() const pf_attr_noexcept
		{
			return this->off_;
		}

		/// Base
		/*! @brief Gets the encapsulated pointer of the iterator.
		 *
		 *  @return Encapsulated pointer.
		 */
		pf_hint_nodiscard pf_decl_constexpr _Ty *base() const pf_attr_noexcept
		{
			return this->ptr_;
		}

	private:
		const _Ty *ptr_;
		diff_t off_;
	};

	/// ITERATOR: Sequence Operator+
	/*! @brief Sequence Iterator Addition Operator
	 *
	 *  @tparam _Ty Type of encapsulated pointer.
	 *  @param[in] __it Iterator.
	 *  @param[in] __i  Number of offset to apply.
	 *  @return __it + __i as copy.
	 */
	template <typename _Ty>
	pf_hint_nodiscard pf_decl_constexpr iterator<_Ty> operator+(
			iterator<_Ty> __it,
			diff_t __i) pf_attr_noexcept
	{
		iterator<_Ty> i = __it;
		return i += __i;
	}

	/// ITERATOR: Sequence Operator-
	/*! @brief Sequence Iterator Subtraction Operator
	 *
	 *  @tparam _Ty Type of encapsulated pointer.
	 *  @param[in] __it Iterator.
	 *  @param[in] __i  Number of offset to apply.
	 *  @return __it - __i as copy.
	 */
	template <typename _Ty>
	pf_hint_nodiscard pf_decl_constexpr iterator<_Ty> operator-(
			iterator<_Ty> __it,
			diff_t __i) pf_attr_noexcept
	{
		iterator<_Ty> i = __it;
		return i -= __i;
	}

	/// ITERATOR: Const Iterator -> Deduction Guide
	/*! @brief Deduction guide for constant sequence iterators.
	 *
	 *  @tparam _Ty Type of encapsulated pointer.
	 */
	template <typename _Ty>
	iterator(const _Ty *)
			-> iterator<const _Ty>;
	template <typename _Ty>
	/*! @brief Deduction guide for constant sequence iterators with offset as second
	 *				 constructor's parameter.
	 *
	 *  @tparam _Ty Type of encapsulated pointer.
	 */
	iterator(const _Ty *, diff_t _off)
			->iterator<const _Ty>;
}

#endif // PULSAR_ITERATOR_HPP