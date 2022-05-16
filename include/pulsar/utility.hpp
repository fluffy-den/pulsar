/*! @file   utility.hpp
 *  @author Fluffy (noe.louis-quentin@hotmail.fr)
 *  @brief	Utility file.
 *  @date   09-02-2022
 *
 *  @copyright Copyright (c) 2022 - Pulsar Software
 *
 *  @since 0.1.0
 */

#ifndef PULSAR_UTILITY_HPP
#define PULSAR_UTILITY_HPP 1

// Include: Pulsar
#include "pulsar/debug.hpp"
#include "pulsar/pulsar.hpp"
#include "pulsar/system.hpp"

// Include: C++
#include <algorithm>
#include <atomic>
#include <string>
#include <string_view>
#include <utility>

// Include: C
#include <cstdarg>
#include <cstring>
#include <ctime>

// Pulsar
namespace pul
{

	/// Trim
	/*! @brief Deletes the spaces to the right of the last character of the string.
	 *
	 *  @param[in, out] __s String to modify.
	 *  @return Iterator referencing location of the first removal.
	 */
	pf_decl_inline std::string::iterator strtriml(
			std::string &__s)
	{
		return __s.erase(__s.begin(),
										 std::find_if(__s.begin(), __s.end(), [](char c)
																	{ return !std::isspace(c); }));
	}
	/*! @brief Deletes the spaces to the left of the first character of the string.
	 *
	 *  @param[in, out] __s String to modify.
	 *  @return Iterator referencing location of the first removal.
	 */
	pf_decl_inline std::string::iterator strtrimr(
			std::string &__s)
	{
		return __s.erase(std::find_if(__s.rbegin(), __s.rend(), [](char c)
																	{ return !std::isspace(c); })
												 .base(),
										 __s.end());
	}

	/// ToLower / Upper
	/*! @brief Converts all characters in a string to lower case.
	 *
	 *  @param[in, out] __s String to modify.
	 *  @return Output iterator equal to the last modified element.
	 */
	pf_decl_inline std::string::iterator strtolower(
			std::string &__s)
	{
		return std::transform(__s.cbegin(), __s.cend(), __s.begin(), [](char c)
													{ return std::tolower(c); });
	}
	/*! @brief Converts all characters in a string to upper case.
	 *
	 *  @param[in, out] __s String to modify.
	 *  @return Output iterator equal to the last modified element.
	 */
	pf_decl_inline std::string::iterator strtoupper(
			std::string &__s)
	{
		return std::transform(__s.cbegin(), __s.cend(), __s.begin(), [](char c)
													{ return std::toupper(c); });
	}

	/// Format
	/*! @brief Creates a string from  formatting specification @a __format by the variadic arguments
	 *				 based on std::vsnprintf function.
	 *
	 *  @ref See https://en.cppreference.com/w/cpp/io/c/fprintf to know how to use this function
	 * 			 (For @a __format and variadic arguments).
	 *
	 *  @param[in] __format Formatting specification.
	 *  @param[in] ... 		  Variadic arguments sent to @a __format.
	 *
	 *  @return Formatted string.
	 */
	pf_decl_inline std::string strfmt(
			std::string_view __format,
			...)
	{
		std::va_list va;
		va_start(va, __format);
		const size_t n = std::vsnprintf(nullptr, 0, __format.data(), va) + 1;
		std::string s(n, ' ');
		if (std::vsnprintf(s.data(), s.length(), __format.data(), va) < 0)
			return "";
		va_end(va);
		s.pop_back(); // remove null-terminating
		return s;
	}
	/*! @brief Inserts into a string @a __str, a formatted string at position @a __w from a
	 * 				 formatting specification @a __format.
	 *
	 *  @param[in] __format   Formatting specification.
	 *  @param[in, out] __str String to modify.
	 *  @param[in] __w				Location of the insertion.
	 *  @param[in] ...				Variadic arguments sent to @a __format.
	 *  @return Reference to the input string.
	 */
	pf_decl_inline std::string &strfmt(
			std::string_view __format,
			std::string &__str,
			std::string::iterator __w,
			...)
	{
		std::va_list va;
		va_start(va, __w);
		const size_t n = std::vsnprintf(nullptr, 0, __format.data(), va) + 1;
		__str.resize(__str.size() + n);
		char *w = __w.base();
		char *d = w + n;
		std::memcpy(d, w, n);
		if (std::vsnprintf(w, n, __format.data(), va) < 0)
		{
			std::memcpy(w, d, n);
			__str.resize(__str.size() - n);
		}
		else
		{
			__str.erase(__w + n - 1);
		}
		va_end(va);
		return __str;
	}

	/// Time format
	/*! @brief Formats from a specification @a __format, a date @a __tm.
	 *
	 *  @param[in] __format Format of the string.
	 *  @param[in] __tm  		Calendar time.
	 *  @return Character string containing a formatted time.
	 */
	pf_decl_inline std::string strfmt(
			std::string_view __format,
			const std::tm &__tm)
	{
		std::array<char, 128> buf{ ' ' };
		size_t n = std::strftime(buf.data(), buf.size(), __format.data(), &__tm) + 1;
		if (n == 1)
			return "";
		if (n <= buf.size())
			--n;
		return std::string(buf.begin(), buf.begin() + n);
	}
	/*! @brief Inserts into a string @a __str, a string formatted at the position @a __w, a
	 *				 formatting specification @a __format, a date @a __tm.
	 *
	 *	@param[in] __format 	Formatting specification.
	 * 	@param[in, out] __str String to modify.
	 * 	@param[in] __w				Location of the string.
	 * 	@param[in] __tm				Calendar time.
	 * 	@return Reference to the input string.
	 */
	pf_decl_inline std::string &strfmt(
			std::string_view __format,
			std::string &__str,
			std::string::iterator __w,
			const std::tm &__tm)
	{
		std::array<char, 128> buf{ ' ' };
		size_t n = std::strftime(buf.data(), buf.size(), __format.data(), &__tm) + 1;
		if (n == 1)
			return __str;
		if (n <= buf.size())
			--n;
		__str.insert(__w, buf.data(), buf.data() + n);
		return __str;
	}

	/// ITERATOR: Sequence
	/*! @brief Sequence iterator.
	 *
	 *  @tparam _Ty Type encapsulated by the iterator.
	 */
	template <typename _Ty>
	class iterator_sequence
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
		pf_decl_constexpr iterator_sequence(
				_Ty *__ptr,
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
		pf_decl_constexpr iterator_sequence(
				iterator_sequence<_Ty> const &__r) pf_attr_noexcept
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
		pf_decl_constexpr iterator_sequence(
				iterator_sequence<_Ty> const &__r,
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
		pf_decl_constexpr iterator_sequence<_Ty> &operator=(
				iterator_sequence<_Ty> const &__r) pf_attr_noexcept
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
		pf_decl_constexpr iterator_sequence<_Ty> &operator++() pf_attr_noexcept
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
		pf_decl_constexpr iterator_sequence<_Ty> operator++(
				int32_t __i) pf_attr_noexcept
		{
			iterator_sequence<_Ty> i = *this;
			++i;
			return i;
		}

		/// Operator+=
		/*! @brief Addition operator.
		 *
		 *  @param[in] __i Number of offset to apply.
		 *  @return Iterator that satisfy this->ptr_ + this->off_ * __i.
		 */
		pf_decl_constexpr iterator_sequence<_Ty> &operator+=(
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
		pf_decl_constexpr iterator_sequence<_Ty> &operator--() pf_attr_noexcept
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
		pf_decl_constexpr iterator_sequence<_Ty> operator--(
				int32_t __i) pf_attr_noexcept
		{
			iterator_sequence<_Ty> i = *this;
			--i;
			return i;
		}

		/// Operator-=
		/*! @brief Substraction operator.
		 *
		 *  @param[in] __i Number of offset to apply.
		 *  @return Iterator that satisfy this->ptr_ - this->off_ * __i.
		 */
		pf_decl_constexpr iterator_sequence<_Ty> &operator-=(
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
		pf_decl_constexpr _Ty &operator*() const pf_attr_noexcept
		{
			return *this->ptr_;
		}

		/// Operator->
		/*! @brief Arrow operator.
		 *
		 *  @return Encapsulated pointer.
		 */
		pf_decl_constexpr _Ty *operator->() const pf_attr_noexcept
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
		pf_decl_constexpr bool operator==(
				iterator_sequence<_OtherTy> const &__r) const pf_attr_noexcept
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
		pf_decl_constexpr std::strong_ordering operator<=>(
				iterator_sequence<_OtherTy> const &__r) const pf_attr_noexcept
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
	class iterator_sequence<const _Ty>
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
		pf_decl_constexpr iterator_sequence(
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
		pf_decl_constexpr iterator_sequence(
				iterator_sequence<const _Ty> const &__r) pf_attr_noexcept
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
		pf_decl_constexpr iterator_sequence(
				iterator_sequence<const _Ty> const &__r,
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
		pf_decl_constexpr iterator_sequence<const _Ty> &operator=(
				iterator_sequence<const _Ty> const &__r) pf_attr_noexcept
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
		pf_decl_constexpr iterator_sequence<const _Ty> &operator++() pf_attr_noexcept
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
		pf_decl_constexpr iterator_sequence<const _Ty> operator++(
				int32_t __i) pf_attr_noexcept
		{
			iterator_sequence<const _Ty> i = *this;
			++i;
			return i;
		}

		/// Operator+=
		/*! @brief Addition operator.
		 *
		 *  @param[in] __i Number of offset to apply.
		 *  @return Iterator that satisfy this->ptr_ + this->off_ * __i.
		 */
		pf_decl_constexpr iterator_sequence<const _Ty> &operator+=(
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
		pf_decl_constexpr iterator_sequence<const _Ty> &operator--() pf_attr_noexcept
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
		pf_decl_constexpr iterator_sequence<const _Ty> operator--(
				int32_t __i) pf_attr_noexcept
		{
			iterator_sequence<const _Ty> i = *this;
			--i;
			return i;
		}

		/// Operator-=
		/*! @brief Subtraction operator.
		 *
		 *  @param[in] __i Number of offset to apply.
		 *  @return Iterator that satisfy this->ptr_ - this->off_ * __i.
		 */
		pf_decl_constexpr iterator_sequence<const _Ty> &operator-=(
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
		pf_decl_constexpr const _Ty &operator*() const pf_attr_noexcept
		{
			return *this->ptr_;
		}

		/// Operator->
		/*! @brief Arrow operator.
		 *
		 *  @return Encapsulated pointer.
		 */
		pf_decl_constexpr const _Ty *operator->() const pf_attr_noexcept
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
		pf_decl_constexpr bool operator==(
				iterator_sequence<_OtherTy> const &__r) const pf_attr_noexcept
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
		pf_decl_constexpr bool operator==(
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
		pf_decl_constexpr std::strong_ordering operator<=>(
				iterator_sequence<const _Ty> const &__r) const pf_attr_noexcept
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
	pf_decl_constexpr iterator_sequence<_Ty> operator+(
			iterator_sequence<_Ty> __it,
			diff_t __i) pf_attr_noexcept
	{
		iterator_sequence<_Ty> i = __it;
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
	pf_decl_constexpr iterator_sequence<_Ty> operator-(
			iterator_sequence<_Ty> __it,
			diff_t __i) pf_attr_noexcept
	{
		iterator_sequence<_Ty> i = __it;
		return i -= __i;
	}

	/// ITERATOR: Const Iterator -> Deduction Guide
	/*! @brief Deduction guide for constant sequence iterators.
	 *
	 *  @tparam _Ty Type of encapsulated pointer.
	 */
	template <typename _Ty>
	iterator_sequence(const _Ty *)
			-> iterator_sequence<const _Ty>;

	/*\
	 * Important: All of the above lists are unsecured. For performance reasons, arguments passed to
	 * methods are all assumed to be correct (except in debug), but are noexcept (advantage in
	 * release, no need to check if arguments are valid). Test your data structures well before
	 * porting your application in release mode.
	\*/

	/// SINGLY: Types
	template <typename _Ty>
		requires(!std::is_void_v<_Ty>)
	class singly_node;
	template <typename _Ty>
	class singly_iterator;
	template <typename _Ty>
	class singly_linked_list;
	template <typename _Ty>
	class singly_rotative_linked_list;

	/// SINGLY: CDS Types
	template <typename _Ty>
	class singly_cds_linked_list;
	template <typename _Ty>
	class singly_cds_linked_lifo;
	template <typename _Ty>
	class singly_cds_linked_fifo;

	/// SINGLY: Node
	/*! @brief Singly linked node.
	 *
	 *  @tparam _Ty Type of contained object.
	 */
	template <typename _Ty>
		requires(!std::is_void_v<_Ty>)
	class singly_node
	{
		/// Linked-Lists
		pf_decl_friend class singly_iterator<std::remove_const_t<_Ty>>;
		pf_decl_friend class singly_iterator<const std::remove_const_t<_Ty>>;
		pf_decl_friend class singly_linked_list<_Ty>;
		pf_decl_friend class singly_rotative_linked_list<_Ty>;
		/// CDS
		pf_decl_friend class singly_cds_linked_list<_Ty>;
		pf_decl_friend class singly_cds_linked_lifo<_Ty>;
		pf_decl_friend class singly_cds_linked_fifo<_Ty>;

		/// Unlink
		/*! @brief Removes the links to other nodes.
		 */
		pf_decl_constexpr void __unlink() pf_attr_noexcept
		{
			this->next_ = nullptr;
		}

	public:
		/// Constructors
		/*! @brief Default constructor.
		 */
		pf_decl_constexpr singly_node() requires(std::is_default_constructible_v<std::remove_all_extents_t<_Ty>>)
				: next_(nullptr)
		{}
		/*! @brief Constructor. Builds the @a _Ty object and initializes the node to zero.
		 *
		 *  @tparam _Args Types of arguments.
		 *  @param[in] __args Arguments to pass to the constructor of @a _Ty .
		 */
		template <typename... _Args>
		pf_decl_constexpr singly_node(
				_Args &&...__args) requires(std::is_constructible_v<_Ty, _Args...>)
				: next_(nullptr)
				, store_(std::forward<_Args>(__args)...)
		{}
		/*! @brief Constructor. Construct from an @a _Ty move.
		 *
		 *  @param[in] __val Instance to be moved.
		 */
		pf_decl_constexpr singly_node(
				_Ty &&__val) requires(std::is_move_constructible_v<_Ty>)
				: next_(nullptr)
				, store_(std::move(__val))
		{}
		/*! @brief Constructor. Construct from an @a _Ty copy.
		 *
		 *  @param[in] __val Instance to be copied.
		 */
		pf_decl_constexpr singly_node(
				_Ty const &__val) requires(std::is_copy_constructible_v<_Ty>)
				: next_(nullptr)
				, store_(__val)
		{}
		/*! @brief Copy constructor.
		 *
		 *  @param[in] __r Other instance to copy from.
		 */
		singly_node(singly_node<_Ty> const &__r) = delete;

		/// Destructor
		/*! @brief Destructor.
		 */
		pf_decl_constexpr ~singly_node() pf_attr_noexcept
		{
			pf_assert(!this->is_linked(), "Destroying a linked node!");
		}

		/// Operator=
		/*! @brief Assignment operator. Move an @a __Ty into this node.
		 *
		 *  @param[in] __val Instance to be moved.
		 *  @return Reference on this instance.
		 */
		pf_decl_constexpr singly_node<_Ty> &operator=(
				_Ty &&__val) requires(std::is_move_assignable_v<_Ty>)
		{
			this->store_ = std::move(__val);
			return *this;
		}
		/*! @brief Assignment operator. Copy an @a __Ty into this node.
		 *
		 *  @param[in] __val Instance to be copied.
		 *  @return Reference on this instance.
		 */
		pf_decl_constexpr singly_node<_Ty> &operator=(
				_Ty const &__val) requires(std::is_copy_assignable_v<_Ty>)
		{
			this->store_ = __val;
			return *this;
		}
		/*! @brief Copy assignment operator.
		 *
		 *  @param[in] __r Other instance to copy from.
		 *  @return Reference on this instance.
		 */
		singly_node<_Ty> &operator=(singly_node<_Ty> const &__r) = delete;

		/// Operator*
		/*! @brief Deference operator.
		 *
		 *  @return Reference to the contained @a _Ty instance.
		 */
		pf_hint_nodiscard pf_decl_constexpr _Ty &operator*() pf_attr_noexcept
		{
			return this->store_;
		}
		/*! @brief Const deference operator.
		 *
		 *  @return Const reference to the contained @a _Ty instance.
		 */
		pf_hint_nodiscard pf_decl_constexpr const _Ty &operator*() const pf_attr_noexcept
		{
			return this->store;
		}

		/// Operator->
		/*! @brief Arrow operator.
		 *
		 *  @return Pointer to the contained @a _Ty instance.
		 */
		pf_hint_nodiscard pf_decl_constexpr _Ty *operator->() pf_attr_noexcept
		{
			return &this->store_;
		}
		/*! @brief Const arrow operator.
		 *
		 *  @return Const pointer to the contained @a _Ty instance.
		 */
		pf_hint_nodiscard pf_decl_constexpr const _Ty *operator->() const pf_attr_noexcept
		{
			return &this->store_;
		}

		/// Operator==
		/*! @brief Equality operator.
		 *
		 *  @param[in] __r node from which to compare the instances of @a _Ty contained.
		 *  @return True if equal.
		 *  @return False if not equal.
		 */
		pf_hint_nodiscard pf_decl_constexpr bool operator==(
				singly_node<_Ty> const &__r) const pf_attr_noexcept
		{
			return this->store_ == __r.store_;
		}

		/// Operator<=>
		/*! @brief Spaceship operator.
		 *
		 *  @param[in] __r Another node from which to compare the instances of @a _Ty contained.
		 *  @return Operator (<, <=, >, >=) equality.
		 */
		pf_hint_nodiscard pf_decl_constexpr bool operator<=>(
				singly_node<_Ty> const &__r) const pf_attr_noexcept
		{
			return this->store_ <=> __r->store_;
		}

		/// Next
		/*! @brief Get a pointer to the next node.
		 *
		 *  @return Pointer to the next node.
		 */
		pf_hint_nodiscard pf_decl_constexpr singly_node<_Ty> *next() const pf_attr_noexcept
		{
			return this->next_;
		}

		/// Is Linked
		/*! @brief Checks if the next contained node exists.
		 *
		 *  @return True if this->next_ isn't equal to nullptr.
		 *  @return False if this->next_ is equal to nullptr.
		 */
		pf_hint_nodiscard pf_decl_constexpr bool is_linked() const pf_attr_noexcept
		{
			return this->next_;
		}

	private:
		singly_node<_Ty> *next_;
		_Ty store_;
	};

	/// SINGLY: Iterator
	/*! @brief Singly iterator for singly linked nodes uses.
	 *
	 *  @tparam _Ty Type of working object.
	 */
	template <typename _Ty>
	class singly_iterator
	{
		pf_decl_friend singly_iterator<const _Ty>;

	public:
		using iterator_category = std::forward_iterator_tag;
		using value_type				= _Ty;
		using node							= singly_node<_Ty>;
		using difference_type		= diff_t;

		/// Constructors
		/*! @brief Constructor.
		 *
		 *  @param[in] __n Pointer to a singly linked node.
		 */
		pf_decl_constexpr singly_iterator(
				node *__n) pf_attr_noexcept
				: ptr_(__n)
		{}
		/*! @brief Copy constructor.
		 *
		 *  @param[in] __i Another iterator from which to copy the pointer.
		 */
		pf_decl_constexpr singly_iterator(
				singly_iterator<_Ty> const &__i) pf_attr_noexcept
				: ptr_(__i.ptr_)
		{}

		/// Operator=
		/*! @brief Copy assignment operator.
		 *
		 *  @param[in] __i Another iterator from which to copy the pointer.
		 *  @return Reference on this instance.
		 */
		pf_decl_constexpr singly_iterator<_Ty> &operator=(
				singly_iterator<_Ty> const &__i) pf_attr_noexcept
		{
			this->ptr_ = __i.ptr_;
			return *this;
		}

		/// Operator++
		/*! @brief Pre-incremental operator.
		 *
		 *  @return Reference on this instance after incrementation.
		 */
		pf_decl_constexpr singly_iterator<_Ty> &operator++() pf_attr_noexcept
		{
			this->ptr_ = this->ptr_->next_;
			return *this;
		}
		/*! @brief Post-incremental operator.
		 *
		 *  @param[in] __i Identifier for pre-incremental operator.
		 *  @return Copy of this instance before incrementation.
		 */
		pf_decl_constexpr singly_iterator<_Ty> operator++(
				int32_t __i) pf_attr_noexcept
		{
			singly_iterator<_Ty> c = this->ptr_;
			this->ptr_						 = this->ptr_->next_;
			return c;
		}

		/// Operator+=
		/*! @brief Addition operator.
		 *
		 *  @param[in] __i Difference to be applied to the iterator as is this->ptr_ = this->ptr_->next_
		 *  					 by __i times.
		 *  @return Reference on this instance after difference.
		 */
		pf_decl_constexpr singly_iterator<_Ty> &operator+=(
				difference_type __i) pf_attr_noexcept
		{
			for (; __i > 0; --__i)
			{
				this->ptr_ = this->ptr_->next_;
			}
			return *this;
		}

		/// Base
		/*! @brief Get the pointer associated with this iterator.
		 *
		 *  @return Pointer on a singly_node of this iterator.
		 */
		pf_hint_nodiscard pf_decl_constexpr node *base() const pf_attr_noexcept
		{
			return this->ptr_;
		}

		/// Operator*
		/*! @brief Deference operator.
		 *
		 *  @return Reference to the contained pointed node @a _Ty instance.
		 */
		pf_hint_nodiscard pf_decl_constexpr _Ty &operator*() const pf_attr_noexcept
		{
			return this->base()->store_;
		}

		/// Operator->
		/*! @brief Arrow operator.
		 *
		 *  @return Pointer to the contained pointed node @a _Ty instance.
		 */
		pf_hint_nodiscard pf_decl_constexpr _Ty *operator->() const pf_attr_noexcept
		{
			return this->base()->store_;
		}

		/// Operator cast: node*
		/*! @brief Cast operator to node *. Convert this instance type to a node object with the stored
		 *				 iterator.
		 *
		 *  @return Converted pointer of type node*.
		 */
		pf_hint_nodiscard pf_decl_constexpr operator node *() const pf_attr_noexcept
		{
			return this->base();
		}

		/// Operator==
		/*! @brief Equality operator. Checks if two iterators have the same pointer.
		 *
		 *  @param[in] __r Another iterator to compare with.
		 *  @return True if is equal.
		 *  @return False if isn't equal.
		 */
		pf_hint_nodiscard pf_decl_constexpr bool operator==(
				singly_iterator<_Ty> const &__r) const pf_attr_noexcept
		{
			return this->ptr_ == __r.ptr_;
		}
		/*! @brief Equality operator. Checks if contained iterator pointer is the same as @a __p .
		 *
		 *  @param[in] __p Pointer on a node to compare with.
		 *  @return True if is equal.
		 *  @return False if isn't equal.
		 */
		pf_hint_nodiscard pf_decl_constexpr bool operator==(
				node *__p) const pf_attr_noexcept
		{
			return this->ptr_ == __p;
		}

	private:
		node *ptr_;
	};

	/// SINGLY: Const Iterator
	/*! @brief Specialization of a singly iterator with the type @a _Ty = const _Ty (const iterator).
	 *
	 *  @tparam @a _Ty = const _Ty
	 */
	template <typename _Ty>
	class singly_iterator<const _Ty>
	{
	public:
		using iterator_category = std::forward_iterator_tag;
		using value_type				= const _Ty;
		using node							= const singly_node<_Ty>;
		using difference_type		= diff_t;

		/// Constructors
		/*! @brief Constructor.
		 *
		 *  @param[in] __n Const pointer to a singly linked node.
		 */
		pf_decl_constexpr singly_iterator(
				node *__n) pf_attr_noexcept
				: ptr_(__n)
		{}
		/*! @brief Copy constructor.
		 *
		 *  @param[in] __i Another const iterator from which to copy the pointer.
		 */
		pf_decl_constexpr singly_iterator(
				singly_iterator<const _Ty> const &__i) pf_attr_noexcept
				: ptr_(__i.ptr_)
		{}

		/// Operator=
		/*! @brief Copy assignment operator.
		 *
		 *  @param[in] __i Another const iterator from which to copy the pointer.
		 *  @return Reference on this instance.
		 */
		pf_decl_constexpr singly_iterator<const _Ty> &operator=(
				singly_iterator<const _Ty> const &__i) pf_attr_noexcept
		{
			this->ptr_ = __i.ptr_;
			return *this;
		}

		/// Operator++
		/*! @brief Pre-incremental operator.
		 *
		 *  @return Reference on this instance after incrementation.
		 */
		pf_decl_constexpr singly_iterator<const _Ty> &operator++() pf_attr_noexcept
		{
			this->ptr_ = this->ptr_->next_;
			return *this;
		}
		/*! @brief Post-incremental operator.
		 *
		 *  @param[in] __i Identifier for pre incremental operator.
		 *  @return Copy of this instance before incrementation.
		 */
		pf_decl_constexpr singly_iterator<const _Ty> operator++(
				int32_t __i) pf_attr_noexcept
		{
			singly_iterator<const _Ty> c = this->ptr_;
			this->ptr_									 = this->ptr_->next_;
			return c;
		}

		/// Operator+=
		/*! @brief Addition operator.
		 *
		 *  @param[in] __i Difference to be applied to the iterator as is this->ptr_ = this->ptr_->next_
		 *  					 by __i times.
		 *  @return Reference on this instance after difference.
		 */
		pf_decl_constexpr singly_iterator<const _Ty> &operator+=(
				difference_type __i) pf_attr_noexcept
		{
			for (; __i > 0; --__i)
			{
				this->ptr_ = this->ptr_->next_;
			}
			return *this;
		}

		/// Base
		/*! @brief Get the pointer associated with this iterator.
		 *
		 *  @return Const pointer on a singly_node of this iterator.
		 */
		pf_hint_nodiscard pf_decl_constexpr node *base() const pf_attr_noexcept
		{
			return this->ptr_;
		}

		/// Operator*
		/*! @brief Deference operator.
		 *
		 *  @return Const reference to the contained pointed node @a _Ty instance.
		 */
		pf_hint_nodiscard pf_decl_constexpr const _Ty &operator*() const pf_attr_noexcept
		{
			return this->base()->store_;
		}

		/// Operator->
		/*! @brief Arrow operator.
		 *
		 *  @return Const pointer to the contained pointed node @a _Ty instance.
		 */
		pf_hint_nodiscard pf_decl_constexpr const _Ty *operator->() const pf_attr_noexcept
		{
			return &this->base()->store_;
		}

		/// Operator cast: node*
		/*! @brief Cast operator to node *. Convert this instance type to a node object with the stored
		 *				 iterator.
		 *
		 *  @return Converted pointer of type node*.
		 */
		pf_hint_nodiscard pf_decl_constexpr operator node *() const pf_attr_noexcept
		{
			return this->base();
		}

		/// Operator==
		/*! @brief Equality operator. Checks if two iterators have the same pointer.
		 *
		 *  @param[in] __r Another iterator to compare with.
		 *  @return True if is equal.
		 *  @return False if isn't equal.
		 */
		pf_hint_nodiscard pf_decl_constexpr bool operator==(
				singly_iterator<const _Ty> const &__r) const pf_attr_noexcept
		{
			return this->ptr_ == __r.ptr_;
		}
		/*! @brief Equality operator. Checks if contained iterator pointer is the same as @a __p .
		 *
		 *  @param[in] __p Pointer on a node to compare with.
		 *  @return True if is equal.
		 *  @return False if isn't equal.
		 */
		pf_hint_nodiscard pf_decl_constexpr bool operator==(
				node *__p) const pf_attr_noexcept
		{
			return this->ptr_ == __p;
		}

	private:
		node *ptr_;
	};

	/// SINGLY: Iterator Operator+
	/*! @brief Addition operator.
	 *
	 *  @tparam _Ty Type of the encapsulated object.
	 *  @param[in] __it Iterator from which to apply the difference.
	 *  @param[in] __i Difference.
	 *  @return Copy of the iterator checking __it += __i.
	 */
	template <typename _Ty>
	pf_hint_nodiscard pf_decl_constexpr singly_iterator<_Ty> operator+(
			singly_iterator<_Ty> __it,
			diff_t __i) pf_attr_noexcept
	{
		return __it += __i;
	}

	/*! @brief Alias of a constant singly iterator.
	 *
	 *  @tparam _Ty Type of the encapsulated object.
	 */
	template <typename _Ty>
	using singly_const_iterator = singly_iterator<const std::remove_const_t<_Ty>>;

	/// SINGLY: Const Iterator -> Deduction Guide
	template <typename _Ty>
	singly_iterator(const singly_node<_Ty> *)
			-> singly_iterator<const _Ty>;

	/// SINGLY: Linked-List
	/*! @brief Singly linked-list of nodes.
	 *
	 *  @tparam _Ty Type of the encapsulated object.
	 */
	template <typename _Ty>
	class singly_linked_list pf_attr_final
	{
	public:
		using value_type		 = _Ty;
		using node					 = singly_node<_Ty>;
		using iterator			 = singly_iterator<_Ty>;
		using const_iterator = singly_const_iterator<_Ty>;

		/// Constructors
		/*! @brief Default constructor. Initializes the control variables to nullptr.
		 */
		pf_decl_constexpr singly_linked_list() pf_attr_noexcept
				: head_(nullptr)
				, tail_(nullptr)
		{}
		/*! @brief Move constructor.
		 *
		 *  @param[in] __r Another instance to move from.
		 */
		pf_decl_constexpr singly_linked_list(
				singly_linked_list<_Ty> &&__r) pf_attr_noexcept
				: head_(__r.head_)
				, tail_(__r.tail_)
		{
			__r.head_ = nullptr;
			__r.tail_ = nullptr;
		}

		/// Destructor
		/*! @brief Destructor. Clear the linked-list.
		 */
		pf_decl_constexpr ~singly_linked_list() pf_attr_noexcept
		{
			this->clear();
		}

		/// Operator=
		/*! @brief Move assignment operator.
		 *
		 *  @param[in] __r Another instance to move from. Sets to nullptr the control variables of the
		 * 								 other @a __r list.
		 *  @return Reference on this list.
		 */
		pf_decl_constexpr singly_linked_list<_Ty> &operator=(
				singly_linked_list<_Ty> &&__r) pf_attr_noexcept
		{
			if (!this->empty()) this->clear();
			this->head_ = __r.head_;
			__r.head_		= nullptr;
			this->tail_ = __r.tail_;
			__r.tail_		= nullptr;
			return *this;
		}

		/// Head
		/*! @brief Get a pointer to the head of the list.
		 *
		 *  @return Pointer to the head of the list.
		 */
		pf_hint_nodiscard pf_decl_constexpr node *head() pf_attr_noexcept
		{
			return this->head_;
		}
		/*! @brief Get a constant pointer to the head of the list.
		 *
		 *  @return Constant pointer to the head of the list.
		 */
		pf_hint_nodiscard pf_decl_constexpr const node *head() const pf_attr_noexcept
		{
			return this->head_;
		}

		/// Begin
		/*! @brief
		 *
		 *  @return pf_hint_nodiscard
		 */
		pf_hint_nodiscard pf_decl_constexpr iterator begin() pf_attr_noexcept
		{
			return this->head_;
		}
		/*! @brief
		 *
		 *  @return pf_hint_nodiscard
		 */
		pf_hint_nodiscard pf_decl_constexpr const_iterator cbegin() const pf_attr_noexcept
		{
			return this->head_;
		}
		/*! @brief
		 *
		 *  @return pf_hint_nodiscard
		 */
		pf_hint_nodiscard pf_decl_constexpr const_iterator begin() const pf_attr_noexcept
		{
			return this->cbegin();
		}

		/// Tail
		/*! @brief Get a pointer to the tail of the list.
		 *
		 *  @return Pointer to the tail of the list.
		 */
		pf_hint_nodiscard pf_decl_constexpr node *tail() pf_attr_noexcept
		{
			return this->tail_;
		}
		/*! @brief Get a constant pointer to the tail of the list.
		 *
		 *  @return Constant pointer to the tail of the list.
		 */
		pf_hint_nodiscard pf_decl_constexpr const node *tail() const pf_attr_noexcept
		{
			return this->tail_;
		}

		/// End
		/*! @brief
		 *
		 *  @return pf_hint_nodiscard
		 */
		pf_hint_nodiscard pf_decl_constexpr iterator end() pf_attr_noexcept
		{
			return nullptr;
		}
		/*! @brief
		 *
		 *  @return pf_hint_nodiscard
		 */
		pf_hint_nodiscard pf_decl_constexpr const_iterator cend() const pf_attr_noexcept
		{
			return nullptr;
		}
		/*! @brief
		 *
		 *  @return pf_hint_nodiscard
		 */
		pf_hint_nodiscard pf_decl_constexpr const_iterator end() const pf_attr_noexcept
		{
			return this->cend();
		}

		/// Insert
		/*! @brief Link @a __n in the place of the head of the list.
		 *
		 *  @param[in] __n Pointer on a node to link.
		 *  @return Pointer on the linked node.
		 */
		pf_decl_constexpr node *insert_head(
				node *__n) pf_attr_noexcept
		{
			pf_assert(__n != nullptr, "__n is nullptr!");
			pf_assert(!this->contains(__n), "__n is already contained in this list!");
			pf_assert(!__n->is_linked(), "__n is already linked!");
			if (!this->head_)
			{
				this->head_ = __n;
				this->tail_ = __n;
			}
			else
			{
				__n->next_	= this->head_;
				this->head_ = __n;
			}
			return this->head_;
		}

		/*! @brief Link @a __n in the place of the tail of the list.
		 *
		 *  @param[in] __n Pointer on a node to link.
		 *  @return Pointer on the linked node.
		 */
		pf_decl_constexpr node *insert_tail(
				node *__n) pf_attr_noexcept
		{
			pf_assert(__n != nullptr, "__n is nullptr!");
			pf_assert(!this->contains(__n), "__n is already contained in this list!");
			pf_assert(!__n->is_linked(), "__n is already linked!");
			if (!this->tail_)
			{
				this->head_ = __n;
				this->tail_ = __n;
			}
			else
			{
				this->tail_->next_ = __n;
				this->tail_				 = __n;
			}
			return this->tail_;
		}

		/*! @brief Link @a __n after @a __w.
		 *
		 *  @param[in] __w Pointer on a node after which to link @a __n.
		 *  @param[in] __n Pointer on a node to link.
		 *  @return Pointer on the linked node.
		 */
		pf_decl_constexpr node *insert_next(
				node *__w,
				node *__n) pf_attr_noexcept
		{
			if (!__w) return this->insert_head(__n);
			if (__w == this->tail_) return this->insert_tail(__n);
			pf_assert(__n != nullptr, "__n is nullptr!");
			pf_assert(!this->contains(__n), "__n is already contained in this list!");
			pf_assert(!__n->is_linked(), "__n is already linked!");
			__n->next_ = __w->next_;
			__w->next_ = __n;
			return __w;
		}

		/*! @brief Link @a __n before @a __w.
		 *
		 *  @param[in] __w Pointer on a node before which to link @a __n.
		 *  @param[in] __n Pointer on a node to link.
		 *  @return Pointer on the linked node.
		 */
		pf_decl_constexpr node *insert_prev(
				node *__w,
				node *__n) pf_attr_noexcept
		{
			if (!__w) return this->insert_tail(__n);
			if (__w == this->head_) return this->insert_head(__n);
			pf_assert(__n != nullptr, "__n is nullptr!");
			pf_assert(!this->contains(__n), "__n is already containted in this list!");
			pf_assert(!__n->is_linked(), "__n is already linked!");
			node *p		 = this->parent_of(__w);
			p->next_	 = __n;
			__n->next_ = __w;
			return __n;
		}

		/// Remove
		/*! @brief Unlink the head of the list.
		 *
		 *  @return Pointer on the unlinked node.
		 */
		pf_decl_constexpr node *remove_head() pf_attr_noexcept
		{
			if (this->empty()) return nullptr;
			node *c			= this->head_;
			this->head_ = this->head_->next_;
			if (!this->head_) this->tail_ = nullptr;
			c->__unlink();
			return c;
		}

		/*! @brief Unlink the tail of the list.
		 *
		 *  @return Pointer on the unlinked node.
		 */
		pf_decl_constexpr node *remove_tail() pf_attr_noexcept
		{
			if (this->empty()) return nullptr;
			node *c = this->tail_, *p = this->parent_of(this->tail_);
			if (p)
			{
				p->next_ = nullptr;
			}
			this->tail_ = p;
			if (!this->tail_) this->head_ = nullptr;
			c->__unlink();
			return c;
		}

		/*! @brief Unlink a specific @a __n node.
		 *
		 *  @param[in] __n Pointer on a node to unlink.
		 *  @return Pointer on the unlinked node.
		 */
		pf_decl_constexpr node *remove(
				node *__n) pf_attr_noexcept
		{
			pf_assert(__n != nullptr, "__n is nullptr!");
			pf_assert(this->contains(__n), "__n isn't contained in the list!");
			if (__n == head_) return this->remove_head();
			if (__n == tail_) return this->remove_tail();
			node *p	 = this->parent_of(__n);
			p->next_ = __n->next_;
			__n->__unlink();
			return __n;
		}

		/*! @brief Unlink the node after @a __w.
		 *
		 *  @param[in] __w Pointer on a node after which to unlink a node.
		 *  @return Pointer on the unlinked node.
		 */
		pf_decl_constexpr node *remove_next(
				node *__w) pf_attr_noexcept
		{
			pf_assert(this->contains(__w), "__w isn't contained in the list!");
			if (!__w) return this->remove_head();
			node *c = __w->next_;
			if (c == this->tail_) return this->remove_tail();
			__w->next_ = c->next_;
			c->__unlink();
			return c;
		}

		/*! @brief Unlink the node before @a __w.
		 *
		 *  @param[in] __n Pointer of a node before wich to unlink a node.
		 *  @return Pointer on the unlinked node.
		 */
		pf_decl_constexpr node *remove_prev(
				node *__w) pf_attr_noexcept
		{
			pf_assert(this->contains(__w), "__n isn't contained in the list!");
			if (!__w) return this->remove_tail();
			if (__w == this->head_) return nullptr;
			// double parent needed
			node *c = this->head_, *p = nullptr;
			while (c && c->next_ != this->head_ && c->next_ != __w)
			{
				p = c;
				c = c->next_;
			}
			if (c == this->head_) return this->remove_head();
			p->next_ = __w;
			c->__unlink();
			return c;
		}

		/// Parent
		/*! @brief Get the parent of a node from this list.
		 *
		 *  @param[in] __n Constant pointer to a node from which to retrieve its parent.
		 *  @return Pointer on the parent node of @a __n.
		 */
		pf_hint_nodiscard pf_decl_constexpr node *parent_of(
				const node *__n) pf_attr_noexcept
		{
			pf_assert(__n != nullptr, "__n is nullptr!");
			pf_assert(this->contains(__n), "__n isn't contained in the list!");
			node *p = nullptr, *c = this->head_;
			while (c && c != __n)
			{
				p = c;
				c = c->next_;
			}
			return p;
		}
		/*! @brief Get the parent of a node from this list.
		 *
		 *  @param[in] __n Constant pointer to a node from which to retrieve its parent.
		 *  @return Constant pointer on the parent node of @a __n.
		 */
		pf_hint_nodiscard pf_decl_constexpr const node *parent_of(
				const node *__n) const pf_attr_noexcept
		{
			this->parent_of(__n);
		}

		/// Empty
		/*! @brief Checks if this list is empty.
		 *
		 *  @return True if this list is empty.
		 *  @return False if this list isn't empty.
		 */
		pf_hint_nodiscard pf_decl_constexpr bool empty() const pf_attr_noexcept
		{
			return !this->head_;
		}

		/// Contains
		/*! @brief Check if @a __n is contained in this list.
		 *
		 *  @param[in] __n Pointer on a node.
		 *  @return True if @a __n is contained in this list.
		 *  @return False otherwise.
		 */
		pf_hint_nodiscard pf_decl_constexpr bool contains(
				const node *__n) const pf_attr_noexcept
		{
			const node *c = this->head_;
			while (c)
			{
				if (c == __n) return true;
				c = c->next_;
			}
			return false;
		}

		/// Size
		/*! @brief Retrieves the size of the list.
		 *
		 *  @return The size of the list. 0 if empty.
		 */
		pf_hint_nodiscard pf_decl_constexpr size_t size() const pf_attr_noexcept
		{
			size_t i			= 0;
			const node *c = this->head_;
			while (c)
			{
				c = c->next_;
				++i;
			}
			return i;
		}

		/// Clear
		/*! @brief Unlink all the nodes associated with this list.
		 */
		pf_decl_constexpr void clear() pf_attr_noexcept
		{
			while (this->remove_head())
				;
		}

	private:
		node *head_;
		node *tail_;
	};

	/// SINGLY: Rotative Linked-List
	/*! @brief Singly linked-list of nodes. Rotative version wich means, the tail is always linked to
	 *				 the head except if nullptr.
	 *
	 *  @tparam _Ty Type of the encapsulated object.
	 */
	template <typename _Ty>
	class singly_rotative_linked_list pf_attr_final
	{
	public:
		using value_type		 = _Ty;
		using node					 = singly_node<_Ty>;
		using iterator			 = singly_iterator<_Ty>;
		using const_iterator = singly_const_iterator<_Ty>;

		/// Constructors
		/*! @brief Default constructor. Initializes the control variables to nullptr.
		 */
		pf_decl_constexpr singly_rotative_linked_list() pf_attr_noexcept
				: head_(nullptr)
				, tail_(nullptr)
		{}
		/*! @brief Move constructor.
		 *
		 *  @param[in] __r Another instance to move from.
		 */
		pf_decl_constexpr singly_rotative_linked_list(
				singly_rotative_linked_list<_Ty> &&__r) pf_attr_noexcept
				: head_(__r.head_)
				, tail_(__r.tail_)
		{
			__r.head_ = nullptr;
			__r.tail_ = nullptr;
		}

		/// Destructor
		/*! @brief Destructor. Clear the linked-list.
		 */
		pf_decl_constexpr ~singly_rotative_linked_list() pf_attr_noexcept
		{
			this->clear();
		}

		/// Operator=
		/*! @brief Move assignment operator.
		 *
		 *  @param[in] __r Another instance to move from. Sets to nullptr the control variables of the
		 * 								 other @a __r list.
		 *  @return Reference on this list.
		 */
		pf_decl_constexpr singly_rotative_linked_list<_Ty> &operator=(
				singly_rotative_linked_list<_Ty> &&__r) pf_attr_noexcept
		{
			if (!this->empty()) this->clear();
			this->head_ = __r.head_;
			__r.head_		= nullptr;
			this->tail_ = __r.tail_;
			__r.tail_		= nullptr;
			return *this;
		}

		/// Head
		/*! @brief Get a constant pointer on the head of the list.
		 *
		 *  @return Constant pointer on the head of the list.
		 */
		pf_hint_nodiscard pf_decl_constexpr node *head() pf_attr_noexcept
		{
			return this->head_;
		}
		/*! @brief Get a pointer on the head of the list.
		 *
		 *  @return Pointer on the head of the list.
		 */
		pf_hint_nodiscard pf_decl_constexpr const node *head() const pf_attr_noexcept
		{
			return this->head_;
		}

		/// Tail
		/*! @brief Get a pointer on the tail of the list.
		 *
		 *  @return Pointer on the tail of the list.
		 */
		pf_hint_nodiscard pf_decl_constexpr node *tail() pf_attr_noexcept
		{
			return this->tail_;
		}
		/*! @brief Get a constant pointer on the tail of the list.
		 *
		 *  @return Constant pointer on the tail of the list.
		 */
		pf_hint_nodiscard pf_decl_constexpr const node *tail() const pf_attr_noexcept
		{
			return this->tail_;
		}

		/// Begin
		/*! @brief
		 *
		 *  @return pf_hint_nodiscard
		 */
		pf_hint_nodiscard pf_decl_constexpr iterator begin() pf_attr_noexcept
		{
			return this->head_;
		}
		/*! @brief
		 *
		 *  @return pf_hint_nodiscard
		 */
		pf_hint_nodiscard pf_decl_constexpr const_iterator cbegin() const pf_attr_noexcept
		{
			return this->head_;
		}
		/*! @brief
		 *
		 *  @return pf_hint_nodiscard
		 */
		pf_hint_nodiscard pf_decl_constexpr const_iterator begin() const pf_attr_noexcept
		{
			return this->cbegin();
		}

		/// Insert
		/*! @brief Link @a __n in the place of the head of the list.
		 *
		 *  @param[in] __n Pointer on a node to link.
		 *  @return Pointer on the linked node.
		 */
		pf_decl_constexpr node *insert_head(
				node *__n) pf_attr_noexcept
		{
			pf_assert(__n != nullptr, "__n is nullptr!");
			pf_assert(!this->contains(__n), "__n is already contained in this list!");
			pf_assert(!__n->is_linked(), "__n is already linked!");
			if (!this->head_)
			{
				this->head_ = __n;
				this->tail_ = __n;
				__n->next_	= __n;
			}
			else
			{
				__n->next_				 = this->head_;
				this->head_				 = __n;
				this->tail_->next_ = __n;
			}
			return this->head_;
		}

		/*! @brief Link @a __n in the place of the tail of this list.
		 *
		 *  @param[in] __n Pointer on a node to link.
		 *  @return Pointer on the linked node.
		 */
		pf_decl_constexpr node *insert_tail(
				node *__n) pf_attr_noexcept
		{
			pf_assert(__n != nullptr, "__n is nullptr!");
			pf_assert(!this->contains(__n), "__n is already contained in this list!");
			pf_assert(!__n->is_linked(), "__n is already linked!");
			if (!this->tail_)
			{
				this->head_ = __n;
				this->tail_ = __n;
				__n->next_	= __n;
			}
			else
			{
				this->tail_->next_ = __n;
				this->tail_				 = __n;
				__n->next_				 = this->head_;
			}
			return this->tail_;
		}

		/*! @brief Link @a __n after @a __w.
		 *
		 *  @param[in] __w Pointer on a node after which to link @a __n.
		 *  @param[in] __n Pointer on a node to link.
		 *  @return Pointer on the linked node.
		 */
		pf_decl_constexpr node *insert_next(
				node *__w,
				node *__n) pf_attr_noexcept
		{
			if (!__w) return this->insert_head(__n);
			if (__w == this->tail_) return this->insert_tail(__n);
			pf_assert(__n != nullptr, "__n is nullptr!");
			pf_assert(!this->contains(__n), "__n is already contained in this list!");
			pf_assert(!__n->is_linked(), "__n is already linked!");
			__n->next_ = __w->next_;
			__w->next_ = __n;
			return __w;
		}

		/*! @brief Link @a __n before @a __w.
		 *
		 *  @param[in] __w Pointer on a node before which to link @a __n.
		 *  @param[in] __n Pointer on a node to link.
		 *  @return Pointer on the linked node.
		 */
		pf_decl_constexpr node *insert_prev(
				node *__w,
				node *__n) pf_attr_noexcept
		{
			if (!__w) return this->insert_tail(__n);
			if (__w == this->head_) return this->insert_head(__n);
			pf_assert(__n != nullptr, "__n is nullptr!");
			pf_assert(!this->contains(__n), "__n is already contained in this list!");
			pf_assert(!__n->is_linked(), "__n is already linked!");
			node *p		 = this->parent_of(__w);
			p->next_	 = __n;
			__n->next_ = __w;
			return __n;
		}

		/// Remove
		/*! @brief Unlink the head of this list.
		 *
		 *  @return Pointer on the unlinked node.
		 */
		pf_decl_constexpr node *remove_head() pf_attr_noexcept
		{
			if (this->empty()) return nullptr;
			node *c = this->head_;
			if (this->head_->next_ == this->head_)
			{
				this->head_ = nullptr;
				this->tail_ = nullptr;
			}
			else
			{
				this->head_				 = this->head_->next_;
				this->tail_->next_ = this->head_;
			}
			c->__unlink();
			return c;
		}

		/*! @brief Unlink the tail of this list.
		 *
		 *  @return Pointer on the unlinked node.
		 */
		pf_decl_constexpr node *remove_tail() pf_attr_noexcept
		{
			if (this->empty()) return nullptr;
			node *c = this->tail_;
			if (this->tail_->next_ == this->tail_)
			{
				this->head_ = nullptr;
				this->tail_ = nullptr;
			}
			else
			{
				this->tail_				 = this->parent_of(this->tail_);
				this->tail_->next_ = this->head_;
			}
			c->__unlink();
			return c;
		}

		/*! @brief Unlink a specific @a __n node.
		 *
		 *  @param[in] __n Pointer on a node to unlink.
		 *  @return Pointer on the unlinked node.
		 */
		pf_decl_constexpr node *remove(
				node *__n) pf_attr_noexcept
		{
			pf_assert(__n != nullptr, "__n is nullptr!");
			pf_assert(this->contains(__n), "__n isn't contained in this list!");
			if (__n == head_) return this->remove_head();
			if (__n == tail_) return this->remove_tail();
			node *p	 = this->parent_of(__n);
			p->next_ = __n->next_;
			__n->__unlink();
			return __n;
		}

		/*! @brief Unlink the node after @a __w.
		 *
		 *  @param[in] __w Pointer on a node after which to unlink a node.
		 *  @return Pointer on the unlinked node.
		 */
		pf_decl_constexpr node *remove_next(
				node *__w) pf_attr_noexcept
		{
			pf_assert(this->contains(__w), "__w isn't contained in this list!");
			if (!__w) return this->remove_head();
			node *c = __w->next_;
			if (c == this->tail_) return this->remove_tail();
			__w->next_ = c->next_;
			c->__unlink();
			return c;
		}

		/*! @brief Unlink the node before @a __w.
		 *
		 *  @param[in] __n Pointer of a node before wich to unlink a node.
		 *  @return Pointer on the unlinked node.
		 */
		pf_decl_constexpr node *remove_prev(
				node *__w) pf_attr_noexcept
		{
			pf_assert(this->contains(__w), "__n isn't contained to this list!");
			if (!__w) return this->remove_tail();
			if (__w == this->head_) return nullptr;
			// double parent needed
			node *c = this->head_, *p = nullptr;
			while (c && c->next_ != this->head_ && c->next_ != __w)
			{
				p = c;
				c = c->next_;
			}
			if (c == this->head_) return this->remove_head();
			p->next_ = __w;
			c->__unlink();
			return c;
		}

		/// Parent
		/*! @brief Get the parent of a node from this list.
		 *
		 *  @param[in] __n Constant pointer to a node from which to retrieve its parent.
		 *  @return Pointer on the parent node of @a __n.
		 */
		pf_hint_nodiscard pf_decl_constexpr node *parent_of(
				const node *__n) pf_attr_noexcept
		{
			pf_assert(__n != nullptr, "__n is nullptr");
			pf_assert(this->contains(__n), "__n isn't contained in this list");
			node *c = this->head_;
			do
			{
				c = c->next_;
			} while (c != this->head_ && c->next_ != __n);
			return c;
		}
		/*! @brief Get the parent of a node from this list.
		 *
		 *  @param[in] __n Constant pointer to a node from which to retrieve its parent.
		 *  @return Constant pointer on the parent node of @a __n.
		 */
		pf_hint_nodiscard pf_decl_constexpr const node *parent_of(
				const node *__n) const pf_attr_noexcept
		{
			this->parent_of(__n);
		}

		/// Empty
		/*! @brief Checks if this list is empty.
		 *
		 *  @return True if this list is empty.
		 *  @return False if this list isn't empty.
		 */
		pf_hint_nodiscard pf_decl_constexpr bool empty() const pf_attr_noexcept
		{
			return !this->head_;
		}

		/// Contains
		/*! @brief Check if @a __n is contained in this list.
		 *
		 *  @param[in] __n Pointer on a node.
		 *  @return True if @a __n is contained in this list.
		 *  @return False otherwise.
		 */
		pf_hint_nodiscard pf_decl_constexpr bool contains(
				const node *__n) pf_attr_noexcept
		{
			pf_assert(__n != nullptr, "__n is nullptr");
			node *c = this->head_;
			do
			{
				if (c == __n) return true;
				c = c->next_;
			} while (c != this->head_);
			return false;
		}

		/// Size
		/*! @brief Retrieves the size of this list.
		 *
		 *  @return The size of this list. 0 if empty.
		 */
		pf_hint_nodiscard pf_decl_constexpr size_t size() const pf_attr_noexcept
		{
			if (this->empty()) return 0;
			size_t i = 0;
			node *c	 = this->head_;
			do
			{
				c = c->next_;
				++i;
			} while (c != this->head_);
			return i;
		}

		/// Clear
		/*! @brief Unlink all the nodes associated with this list.
		 */
		pf_decl_constexpr void clear() pf_attr_noexcept
		{
			while (!this->empty()) this->remove_head();
		}

	private:
		node *head_;
		node *tail_;
	};

	/// SINGLY: CDS List
	/*! @brief Base of a thread-safe linked list. Can't do anything on its own.
	 *
	 *  @tparam _Ty Type of the object encapsulated in the list nodes.
	 *
	 *  @remark Thread-safe
	 */
	template <typename _Ty>
	class alignas(2 * sizeof(std::atomic<void *>)) singly_cds_linked_list
	{
	public:
		using node					 = singly_node<_Ty>;
		using iterator			 = singly_iterator<_Ty>;
		using const_iterator = singly_const_iterator<_Ty>;

		/// Constructors
		/*! @brief Default constructor.
		 *
		 *  @remark Thread-safe.
		 */
		pf_decl_explicit singly_cds_linked_list() pf_attr_noexcept
				: head_(nullptr)
				, tail_(nullptr)
		{}
		/*! @brief Move constructor.
		 *
		 *  @param[in] __r Another list instance to copy from.
		 *
		 *  @remark Thread-safe.
		 */
		singly_cds_linked_list(
				singly_cds_linked_list<_Ty> &&__r) pf_attr_noexcept
				: head_(__r.head_.exchange(nullptr, std::memory_order::acq_rel))
				, tail_(__r.tail_.exchange(nullptr, std::memory_order::acq_rel))
		{}

		/// Operator=
		/*! @brief Move assignment operator.
		 *
		 *  @param[in] __r Another list instance to copy from.
		 *  @return Reference on this list.
		 *
		 *  @remark Thread_safe.
		 */
		singly_cds_linked_list<_Ty> &operator=(
				singly_cds_linked_list<_Ty> &&__r) pf_attr_noexcept
		{
			if (!this->empty()) this->clear();
			this->tail_.store(__r.tail_.exchange(nullptr, std::memory_order::acq_rel), std::memory_order::release);
			this->head_.store(__r.head_.exchange(nullptr, std::memory_order::acq_rel), std::memory_order::release);
			return *this;
		}

		/// Destructor
		/*! @brief Destructor.
		 *
		 *  @remark Thread_safe.
		 */
		~singly_cds_linked_list() pf_attr_noexcept
		{
			this->clear();
		}

		/// Head
		/*! @brief Get a pointer on the head of the list.
		 *
		 *  @param[in] __order Atomic memory barrier to be applied.
		 *  @return Pointer to the head of the list.
		 *
		 *  @remark Thread-safe.
		 */
		pf_hint_nodiscard node *head(
				std::memory_order __order = std::memory_order::relaxed) pf_attr_noexcept
		{
			return this->head_.load(__order);
		}
		/*! @brief Get a constant pointer on the head of the list.
		 *
		 *  @param[in] __order Atomic memory barrier to be applied.
		 *  @return Constant pointer on the head of the list.
		 *
		 *  @remark Thread-safe.
		 */
		pf_hint_nodiscard const node *head(
				std::memory_order __order = std::memory_order::relaxed) const pf_attr_noexcept
		{
			return this->head_.load(__order);
		}

		/// Begin
		/*! @brief
		 *
		 *  @return pf_hint_nodiscard
		 */
		pf_hint_nodiscard pf_decl_constexpr iterator begin(
				std::memory_order __order = std::memory_order::relaxed) pf_attr_noexcept
		{
			return this->head(__order);
		}
		/*! @brief
		 *
		 *  @return pf_hint_nodiscard
		 */
		pf_hint_nodiscard pf_decl_constexpr const_iterator cbegin(
				std::memory_order __order = std::memory_order::relaxed) const pf_attr_noexcept
		{
			return this->head(__order);
		}
		/*! @brief
		 *
		 *  @return pf_hint_nodiscard
		 */
		pf_hint_nodiscard pf_decl_constexpr const_iterator begin(
				std::memory_order __order = std::memory_order::relaxed) const pf_attr_noexcept
		{
			return this->cbegin(__order);
		}

		/// Tail
		/*! @brief Get a pointer on the tail of the list.
		 *
		 *  @param[in] __order Atomic memory barrier to be applied.
		 *  @return Pointer on the tail of the list.
		 *
		 *  @remark Thread-safe.
		 */
		pf_hint_nodiscard node *tail(
				std::memory_order __order = std::memory_order::relaxed) pf_attr_noexcept
		{
			return this->tail_.load(__order);
		}
		/*! @brief Get a constant pointer on the head of the list.
		 *
		 *  @param[in] __order Atomic memory barrier to be applied.
		 *  @return Constant pointer on the tail of the list.
		 *
		 *  @remark Thread-safe.
		 */
		pf_hint_nodiscard const node *tail(
				std::memory_order __order = std::memory_order::relaxed) const pf_attr_noexcept
		{
			return this->tail_.load(__order);
		}

		/// End
		/*! @brief
		 *
		 *  @return pf_hint_nodiscard
		 */
		pf_hint_nodiscard pf_decl_constexpr iterator end() pf_attr_noexcept
		{
			return nullptr;
		}
		/*! @brief
		 *
		 *  @return pf_hint_nodiscard
		 */
		pf_hint_nodiscard pf_decl_constexpr const_iterator cend() const pf_attr_noexcept
		{
			return nullptr;
		}
		/*! @brief
		 *
		 *  @return pf_hint_nodiscard
		 */
		pf_hint_nodiscard pf_decl_constexpr const_iterator end() const pf_attr_noexcept
		{
			return nullptr;
		}

		/// Empty
		/*! @brief Checks if this list is empty.
		 *
		 *  @param[in] __order Atomic memory barrier to be applied.
		 *  @return True if this list is empty.
		 *  @return False if this list isn't empty.
		 *
		 *  @remark Thread-safe.
		 */
		pf_hint_nodiscard bool empty(
				std::memory_order __order = std::memory_order::relaxed) const pf_attr_noexcept
		{
			return !this->head(__order);
		}

		/// Contains
		/*! @brief Check if @a __n is contained in this list.
		 *
		 *  @param[in] __n Pointer on a node.
		 *  @param[in] __order Atomic memory barrier to be applied.
		 *  @return True if @a __n is contained in this list.
		 *  @return False otherwise.
		 *
		 *  @remark Thread-safe.
		 */
		pf_hint_nodiscard bool contains(
				node *__n,
				std::memory_order __order = std::memory_order::relaxed) pf_attr_noexcept
		{
			for (node *b = this->head(__order), *e = nullptr; b != e; ++b)
			{
				if (b == __n) return true;
			}
			return false;
		}

		/// Size
		/*! @brief Retrieves the size of the list.
		 *
		 *  @param[in] __order Atomic memory barrier to be applied.
		 *  @return The size of this list. 0 if empty.
		 *
		 *  @remark Thread-safe.
		 */
		pf_hint_nodiscard size_t size(
				std::memory_order __order = std::memory_order::relaxed) const pf_attr_noexcept
		{
			iterator b = this->head(__order), e = nullptr;
			return std::distance(b, e);
		}

		/// Clear
		/*! @brief Unlink all the nodes associated with this list.
		 *
		 *  @remark Thread-safe.
		 */
		void clear() pf_attr_noexcept
		{
			this->tail_.exchange(nullptr, std::memory_order::release);
			node *h = this->head_.exchange(nullptr, std::memory_order::acq_rel);
			std::atomic_thread_fence(std::memory_order::acq_rel);
			while (h)
			{
				node *n = h->next_;
				h->__unlink();
				h = n;
			}
			std::atomic_thread_fence(std::memory_order::relaxed);
		}

	protected:
		std::atomic<node *> head_;
		std::atomic<node *> tail_;
	};

	/// SINGLY: CDS LIFO
	/*! @brief LIFO style linked-list. Thread-safe guarantee.
	 *
	 *  @tparam _Ty Type of the object encapsulated in nodes.
	 *
	 *  @remark Thread-safe
	 */
	template <typename _Ty>
	class singly_cds_linked_lifo: public singly_cds_linked_list<_Ty>
	{
	public:
		using node					 = singly_node<_Ty>;
		using iterator			 = singly_iterator<_Ty>;
		using const_iterator = singly_const_iterator<_Ty>;

		/// Constructor
		/*! @brief Default constructor.
		 */
		pf_decl_constexpr singly_cds_linked_lifo() pf_attr_noexcept
				: singly_cds_linked_list<_Ty>()
		{}
		/*! @brief Constructor.
		 *
		 *  @tparam _InIterator Input iterator.
		 *  @param[in] __beg Start of the initialization list.
		 *  @param[in] __end End of the initialization list.
		 *
		 *  @remark Thread-safe.
		 */
		template <typename _InIterator>
		singly_cds_linked_lifo(
				_InIterator __beg,
				_InIterator __end) pf_attr_noexcept
				: singly_cds_linked_list<_Ty>()
		{
			this->insert_tail_bulk(__beg, __end);
		}

		/// Insert
		/*! @brief Link @a __n in the place of the tail of this list.
		 *
		 *  @param[in] __n Pointer on a node to link.
		 *  @return Pointer on the linked node.
		 *
		 *  @remark Thread-safe
		 */
		node *insert_tail(
				node *__n) pf_attr_noexcept
		{
			pf_assert(__n != nullptr, "__n is nullptr!");
			pf_assert(!this->contains(__n), "__n is already contained in this list!");
			pf_assert(!__n->is_linked(), "__n is already linked!");
			// Tail
			node *t = this->tail_.load(std::memory_order::relaxed);
			while (!this->tail_.compare_exchange_weak(
					t,
					__n,
					std::memory_order::acquire,
					std::memory_order::release))
				;
			if (t) t->next_ = __n;
			// Head
			node *h = nullptr;
			if (!t) this->head_.compare_exchange_strong(
					h,
					__n,
					std::memory_order::acquire,
					std::memory_order::release);
			return __n;
		}
		/*! @brief Link a list of node in the place of the tail of this list.
		 *
		 *  @tparam _InIterator Input iterator.
		 *  @param[in] __beg Start of the initialization list.
		 *  @param[in] __end End of the initialization list.
		 *  @return Number of inserted nodes of the initialization list.
		 *
		 *  @remark Thread-safe.
		 */
		template <typename _InIterator>
		size_t insert_tail_bulk(
				_InIterator __beg,
				_InIterator __end) pf_attr_noexcept
		{
			pf_assert(__beg != nullptr, "__beg is nullptr!");
			pf_assert(__end != nullptr, "__end is nullptr!");
			// Init List
			node *b	 = &*__beg;
			node *e	 = b;
			size_t c = 1;
			++__beg;
			while (__beg != __end)
			{
				e->next_ = &*__beg;
				e				 = e->next_;
				++__beg;
				++c;
			}
			// Tail
			node *t = this->tail_.load(std::memory_order::relaxed);
			while (!this->tail_.compare_exchange_weak(
					t,
					e,
					std::memory_order::acquire,
					std::memory_order::release))
				;
			if (!t)
			{
				node *h = nullptr;
				this->head_.compare_exchange_strong(
						h,
						b,
						std::memory_order::acquire,
						std::memory_order::release);
			}
			// Head
			else
			{
				t->next_ = b;
			}
			return c;
		}

		/// Remove
		/*! @brief Unlink the head of this list.
		 *
		 *  @return Pointer on the unlinked node.
		 *
		 *  @remark Thread-safe.
		 */
		node *remove_head() pf_attr_noexcept
		{
			// Head
			node *h = this->head_.load(std::memory_order::relaxed);
			do
			{
				if (!h) return nullptr;
			} while (!this->head_.compare_exchange_weak(
					h,
					h->next_,
					std::memory_order::acquire,
					std::memory_order::release));
			// Tail
			if (!h->next_) this->tail_.compare_exchange_strong(
					h,
					nullptr,
					std::memory_order::acquire,
					std::memory_order::release);
			if (h) h->__unlink();
			return h;
		}
		/*! @brief Unlink a range of element from the head of this list and insert them to an output
		 *				 list.
		 *
		 *  @tparam _OutIterator Output iterator.
		 *  @param[in] __beg Start of the initialization list.
		 *  @param[in] __end End of the initialization list.
		 *  @return size_t Number of removed elements.
		 */
		template <typename _OutIterator>
		size_t remove_head_bulk(
				_OutIterator __beg,
				_OutIterator __end) pf_attr_noexcept
		{
			pf_assert(__beg != nullptr, "__beg is nullptr!");
			pf_assert(__end != nullptr, "__end is nullptr!");
			const size_t num = std::distance(__beg, __end);
			// Head
			node *h = this->head_.load(std::memory_order::relaxed), *b = nullptr, *e = nullptr;
			size_t rnum = 0;
			do
			{
				if (!h) return 0;
				rnum = 0;
				b		 = h;
				e		 = h;
				while (e && rnum < num)
				{
					node *c = b->next_;
					if (!c) break;
					e = c;
					++rnum;
				}
			} while (!this->head_.compare_exchange_weak(
					h,
					e->next_,
					std::memory_order::acquire,
					std::memory_order::release));
			// Tail
			this->tail_.compare_exchange_strong(
					e->next_,
					nullptr,
					std::memory_order::acquire,
					std::memory_order::release);
			// Copy
			while (__beg != __end && b != e)
			{
				*__beg	= b;
				node *c = b;
				b				= b->next_;
				c->__unlink();
				++__beg;
			}
			return rnum;
		}
	};

	/// SINGLY: CDS FIFO
	/*! @brief FIFO style linked-list. Thread-safe guarantee.
	 *
	 *  @tparam _Ty Type of the object encapsulated in the nodes.
	 *
	 *  @remark Thread-safe
	 */
	template <typename _Ty>
	class singly_cds_linked_fifo: public singly_cds_linked_list<_Ty>
	{
	public:
		using node					 = singly_node<_Ty>;
		using iterator			 = singly_iterator<_Ty>;
		using const_iterator = singly_const_iterator<_Ty>;

		/// Constructor
		/*! @brief Default constructor.
		 */
		singly_cds_linked_fifo() pf_attr_noexcept
				: singly_cds_linked_list<_Ty>()
		{}
		/*! @brief Constructor.
		 *
		 *  @tparam _InIterator Input iterator.
		 *  @param __beg Start of the initialization list.
		 *  @param __end End of the initialization list.
		 *
		 *  @remark Thread-safe.
		 */
		template <typename _InIterator>
		singly_cds_linked_fifo(
				_InIterator __beg,
				_InIterator __end) pf_attr_noexcept
		{
			this->insert_tail_bulk(__beg, __end);
		}

		/// Insert
		/*! @brief Link @a __n in the place of the head of this list.
		 *
		 *  @param[in] __n Pointer on a node to link.
		 *  @return Pointer on the linked node.
		 *
		 *  @remark Thread-safe
		 */
		node *insert_head(
				node *__n) pf_attr_noexcept
		{
			pf_assert(__n != nullptr, "__n is nullptr!");
			pf_assert(!this->contains(__n), "__n is already contained in this list!");
			pf_assert(!__n->is_linked(), "__n is already linked!");
			// Head
			node *h = this->head_.load(std::memory_order::relaxed);
			while (!this->head_.compare_exchange_weak(
					h,
					__n,
					std::memory_order::consume,
					std::memory_order::release))
				;
			__n->next_ = h;
			// Tail
			node *t		 = nullptr;
			if (!__n->next_) this->head_.compare_exchange_strong(
					t,
					__n,
					std::memory_order::consume,
					std::memory_order::release);
			return __n;
		}
		/*! @brief Link a list of node in the place of the head of this list.
		 *
		 *  @tparam _InIterator Input iterator.
		 *  @param[in] __beg Start of the initialization list.
		 *  @param[in] __end End of the initialization list.
		 *  @return Number of inserted nodes of the initialization list.
		 *
		 *  @remark Thread-safe.
		 */
		template <typename _InIterator>
		size_t insert_head_bulk(
				_InIterator __beg,
				_InIterator __end) pf_attr_noexcept
		{
			pf_assert(__beg != nullptr, "__beg is nullptr!");
			pf_assert(__end != nullptr, "__end is nullptr!");
			// Init List
			const node *b = *__beg;
			node *e				= b;
			size_t c			= 1;
			++__beg;
			while (__beg != __end)
			{
				e->next_ = *__beg;
				e				 = e->next_;
				++__beg;
				++c;
			}
			// Head
			node *h = this->head_.load(std::memory_order::relaxed);
			while (!this->head_.compare_exchange_weak(
					h,
					e,
					std::memory_order::consume,
					std::memory_order::release))
				;
			// Tail
			if (!h)
			{
				node *t = nullptr;
				this->tail_.compare_exchange_strong(
						t,
						e,
						std::memory_order::consume,
						std::memory_order::release);
			}
			else
			{
				e->next_ = h;
			}
			return c;
		}

		/// Remove
		/*! @brief Unlink the head of this list.
		 *
		 *  @return Pointer on the unlinked node.
		 *
		 *  @remark Thread-safe.
		 */
		node *remove_head() pf_attr_noexcept
		{
			// Head
			node *h = this->head_.load(std::memory_order::relaxed);
			do
			{
				if (!h) return nullptr;
			} while (!this->head_.compare_exchange_weak(
					h,
					h->next_,
					std::memory_order::consume,
					std::memory_order::release));
			// Tail
			if (!h->next_) this->tail_.compare_exchange_strong(
					h,
					nullptr,
					std::memory_order::consume,
					std::memory_order::release);
			if (h) h->__unlink();
			return h;
		}
		/*! @brief Unlink a range of element from the head of this list and insert them to an output
		 *				 list.
		 *
		 *  @tparam _OutIterator Output iterator.
		 *  @param[in] __beg Start of the initialization list.
		 *  @param[in] __end End of the initialization list.
		 *  @return size_t Number of removed elements.
		 */
		template <typename _OutIterator>
		size_t remove_head_bulk(
				_OutIterator __beg,
				_OutIterator __end) pf_attr_noexcept
		{
			pf_assert(__beg != nullptr, "__beg is nullptr!");
			pf_assert(__end != nullptr, "__end is nullptr!");
			const size_t num = std::distance(__beg, __end);
			// Head
			node *h = this->head_.load(std::memory_order::relaxed), *b = nullptr, *e = nullptr;
			size_t rnum = 0;
			do
			{
				if (!h) return 0;
				rnum = 0;
				b		 = h;
				e		 = h;
				while (e && rnum < num)
				{
					node *c = b->next_;
					if (!c) break;
					e = c;
					++rnum;
				}
			} while (!this->head_.compare_exchange_weak(
					h,
					e->next_,
					std::memory_order::consume,
					std::memory_order::release));
			// Tail
			this->tail_.compare_exchange_strong(
					e->next_,
					nullptr,
					std::memory_order::consume,
					std::memory_order::release);
			// Copy
			while (__beg != __end && b != e)
			{
				*__beg	= b;
				node *c = b;
				b				= b->next_;
				c->__unlink();
				++__beg;
			}
			return rnum;
		}
	};

	/// DOUBLY: Types
	template <typename _Ty>
		requires(!std::is_void_v<_Ty>)
	class doubly_node;
	template <typename _Ty>
	class doubly_iterator;
	template <typename _Ty>
	class doubly_reverse_iterator;
	template <typename _Ty>
	class doubly_linked_list;
	template <typename _Ty>
	class doubly_rotative_linked_list;

	/*! @brief Doubly linked node.
	 *
	 *  @tparam _Ty Type of contained object.
	 */
	template <typename _Ty>
		requires(!std::is_void_v<_Ty>)
	class doubly_node
	{
		pf_decl_friend doubly_iterator<std::remove_const_t<_Ty>>;
		pf_decl_friend doubly_iterator<const std::remove_const_t<_Ty>>;
		pf_decl_friend doubly_reverse_iterator<std::remove_const_t<_Ty>>;
		pf_decl_friend doubly_reverse_iterator<const std::remove_const_t<_Ty>>;
		pf_decl_friend doubly_linked_list<_Ty>;
		pf_decl_friend doubly_rotative_linked_list<_Ty>;

		/// Unlink
		/*! @brief Removes the links to other nodes.
		 */
		pf_decl_constexpr void __unlink() pf_attr_noexcept
		{
			this->next_ = nullptr;
			this->prev_ = nullptr;
		}

	public:
		/// Constructors
		/*! @brief  Default constructor.
		 */
		pf_decl_constexpr doubly_node() pf_attr_noexcept requires(std::is_default_constructible_v<std::remove_all_extents_t<_Ty>>)
				: next_(nullptr)
				, prev_(nullptr)
		{}
		/*! @brief Constructor. Builds the @a _Ty object and initializes the node to zero.
		 *
		 *  @tparam _Args Types of arguments.
		 *  @param[in] __args Arguments to pass to the constructor of @a _Ty .
		 */
		template <typename... _Args>
		pf_decl_constexpr doubly_node(
				_Args &&...__args) requires(std::is_constructible_v<_Ty, _Args...>)
				: next_(nullptr)
				, prev_(nullptr)
				, store_(std::forward<_Args>(__args)...)
		{}
		/*! @brief Constructor. Construct from an @a _Ty move.
		 *
		 *  @param[in] __val Instance to be moved.
		 */
		pf_decl_constexpr doubly_node(
				_Ty &&__val) requires(std::is_move_constructible_v<_Ty>)
				: next_(nullptr)
				, prev_(nullptr)
				, store_(std::move(__val))
		{}
		/*! @brief Constructor. Construct from an @a _Ty copy.
		 *
		 *  @param[in] __val Instance to be copied.
		 */
		pf_decl_constexpr doubly_node(
				_Ty const &__val) requires(std::is_copy_constructible_v<_Ty>)
				: next_(nullptr)
				, prev_(nullptr)
				, store_(__val)
		{}
		/*! @brief Copy constructor.
		 *
		 *  @param[in] __r Other instance to copy from.
		 */
		doubly_node(doubly_node<_Ty> const &) = delete;

		/// Destructor
		/*! @brief Destructor.
		 */
		pf_decl_constexpr ~doubly_node() pf_attr_noexcept
		{
			pf_assert(!this->is_linked(), "Destroying a linked node!");
		}

		/// Operator=
		/*! @brief Assignment operator. Move an @a __Ty into this node.
		 *
		 *  @param[in] __val Instance to be moved.
		 *  @return Reference on this instance.
		 */
		pf_decl_constexpr doubly_node<_Ty> &operator=(
				_Ty &&__val) requires(std::is_move_assignable_v<_Ty>)
		{
			this->store_ = std::move(__val);
			return *this;
		}
		/*! @brief Assignment operator. Copy an @a __Ty into this node.
		 *
		 *  @param[in] __val Instance to be copied.
		 *  @return Reference on this instance.
		 */
		pf_decl_constexpr doubly_node<_Ty> &operator=(
				_Ty const &__val) requires(std::is_copy_assignable_v<_Ty>)
		{
			this->store_ = std::move(__val);
			return *this;
		}
		/*! @brief Copy assignment operator.
		 *
		 *  @param[in] __r Other instance to copy from.
		 *  @return Reference on this instance.
		 */
		doubly_node<_Ty> &operator=(doubly_node<_Ty> const &) = delete;

		/// Operator*
		/*! @brief Deference operator.
		 *
		 *  @return Reference to the contained @a _Ty instance.
		 */
		pf_hint_nodiscard pf_decl_constexpr _Ty &operator*() pf_attr_noexcept
		{
			return this->store_;
		}
		/*! @brief Const deference operator.
		 *
		 *  @return Const reference to the contained @a _Ty instance.
		 */
		pf_hint_nodiscard pf_decl_constexpr const _Ty &operator*() const pf_attr_noexcept
		{
			return this->store;
		}

		/// Operator->
		/*! @brief Arrow operator.
		 *
		 *  @return Pointer to the contained @a _Ty instance.
		 */
		pf_hint_nodiscard pf_decl_constexpr _Ty *operator->() pf_attr_noexcept
		{
			return &this->store_;
		}
		/*! @brief Const arrow operator.
		 *
		 *  @return Const pointer to the contained @a _Ty instance.
		 */
		pf_hint_nodiscard pf_decl_constexpr const _Ty *operator->() const pf_attr_noexcept
		{
			return &this->store_;
		}

		/// Operator==
		/*! @brief Equality operator.
		 *
		 *  @param[in] __r node from which to compare the instances of @a _Ty contained.
		 *  @return True if equal.
		 *  @return False if not equal.
		 */
		pf_hint_nodiscard pf_decl_constexpr bool operator==(
				doubly_node<_Ty> const &__r) const pf_attr_noexcept
		{
			return this->store_ == __r.store_;
		}

		/// Operator<=>
		/*! @brief Spaceship operator.
		 *
		 *  @param[in] __r Another node from which to compare the instances of @a _Ty contained.
		 *  @return Operator (<, <=, >, >=) equality.
		 */
		pf_hint_nodiscard pf_decl_constexpr bool operator<=>(
				doubly_node<_Ty> const &__r) const pf_attr_noexcept
		{
			return this->store_ <=> __r.store_;
		}

		/// Next / Prev
		/*! @brief Get a pointer to the next node.
		 *
		 *  @return Pointer to the next node.
		 */
		pf_hint_nodiscard pf_decl_constexpr doubly_node<_Ty> *next() const pf_attr_noexcept
		{
			return this->next_;
		}
		/*! @brief Get a pointer to the previous node.
		 *
		 *  @return Pointer to the previous node.
		 */
		pf_hint_nodiscard pf_decl_constexpr doubly_node<_Ty> *prev() const pf_attr_noexcept
		{
			return this->prev_;
		}

		/// Is Linked
		/*! @brief Checks if the next contained node exists.
		 *
		 *  @return True if this->next_ isn't equal to nullptr.
		 *  @return False if this->next_ is equal to nullptr.
		 */
		pf_hint_nodiscard pf_decl_constexpr bool is_linked() const pf_attr_noexcept
		{
			return !this->next_ || !this->prev_;
		}

	private:
		doubly_node<_Ty> *next_;
		doubly_node<_Ty> *prev_;
		_Ty store_;
	};

	/*! @brief Doubly iterator for doubly linked nodes uses.
	 *
	 *  @tparam _Ty Type of working object.
	 */
	template <typename _Ty>
	class doubly_iterator
	{
		pf_decl_friend doubly_iterator<const _Ty>;

	public:
		using iterator_category = std::bidirectional_iterator_tag;
		using value_type				= _Ty;
		using node							= doubly_node<_Ty>;
		using difference_type		= diff_t;

		/// Constructors
		/*! @brief Constructor.
		 *
		 *  @param[in] __n Pointer to a doubly linked node.
		 */
		pf_decl_constexpr doubly_iterator(
				node *__n) pf_attr_noexcept
				: ptr_(__n)
		{}
		/*! @brief Copy constructor.
		 *
		 *  @param[in] __i Another iterator from which to copy the pointer.
		 */
		pf_decl_constexpr doubly_iterator(
				doubly_iterator<_Ty> const &__i) pf_attr_noexcept
				: ptr_(__i.ptr_)
		{}

		/// Operator=
		/*! @brief Copy assignment operator.
		 *
		 *  @param[in] __i Another iterator from which to copy the pointer.
		 *  @return Reference on this instance.
		 */
		pf_decl_constexpr doubly_iterator<_Ty> &operator=(
				doubly_iterator<_Ty> const &__i) pf_attr_noexcept
		{
			this->ptr_ = __i.ptr_;
			return *this;
		}

		/// Operator++
		/*! @brief Pre-incremental operator.
		 *
		 *  @return Reference on this instance after incrementation.
		 */
		pf_decl_constexpr doubly_iterator<_Ty> &operator++() pf_attr_noexcept
		{
			this->ptr_ = this->ptr_->next_;
			return *this;
		}
		/*! @brief Post-incremental operator.
		 *
		 *  @param[in] __i Identifier for pre-incremental operator.
		 *  @return Copy of this instance before incrementation.
		 */
		pf_decl_constexpr doubly_iterator<_Ty> operator++(
				int32_t __i) pf_attr_noexcept
		{
			doubly_iterator<_Ty> c = this->ptr_;
			this->ptr_						 = this->ptr_->next_;
			return c;
		}

		/// Operator+=
		/*! @brief Addition operator.
		 *
		 *  @param[in] __i Difference to be applied to the iterator as is this->ptr_ = this->ptr_->next_
		 *  					 	   by __i times.
		 *  @return Reference on this instance after difference.
		 */
		pf_decl_constexpr doubly_iterator<_Ty> &operator+=(
				difference_type __i) pf_attr_noexcept
		{
			for (; __i > 0; --__i)
			{
				this->ptr_ = this->ptr_->next_;
			}
			for (; __i < 0; ++__i)
			{
				this->ptr_ = this->ptr_->prev_;
			}
			return *this;
		}

		/// Operator--
		/*! @brief Pre-decremental operator.
		 *
		 *  @return Reference on this instance after decrementation.
		 */
		pf_decl_constexpr doubly_iterator<_Ty> &operator--() pf_attr_noexcept
		{
			this->ptr_ = this->ptr_->prev_;
			return *this;
		}
		/*! @brief Post-decremental operator.
		 *
		 *  @param[in] __i Identifier for pre-decremental operator.
		 *  @return Copy of this instance before decrementation.
		 */
		pf_decl_constexpr doubly_iterator<_Ty> operator--(
				int32_t __i) pf_attr_noexcept
		{
			doubly_iterator<_Ty> c = this->ptr_;
			this->ptr_						 = this->ptr_->prev_;
			return c;
		}

		/// Operator-=
		/*! @brief Addition operator.
		 *
		 *  @param[in] __i Difference to be applied to the iterator as is this->ptr_ = this->ptr_->prev_
		 *  					 by __i times.
		 *  @return Reference on this instance after difference.
		 */
		pf_decl_constexpr doubly_iterator &operator-=(
				difference_type __i) pf_attr_noexcept
		{
			for (; __i < 0; ++__i)
			{
				this->ptr_ = this->ptr_->prev_;
			}
			for (; __i > 0; --__i)
			{
				this->ptr_ = this->ptr_->next_;
			}
			return *this;
		}

		/// Base
		/*! @brief Get the pointer associated with this iterator.
		 *
		 *  @return Pointer on a doubly_node of this iterator.
		 */
		pf_hint_nodiscard pf_decl_constexpr node *base() const pf_attr_noexcept
		{
			return this->ptr_;
		}

		/// Operator*
		/*! @brief Deference operator.
		 *
		 *  @return Reference to the contained pointed node @a _Ty instance.
		 */
		pf_hint_nodiscard pf_decl_constexpr _Ty &operator*() const pf_attr_noexcept
		{
			return this->base()->store_;
		}

		/// Operator->
		/*! @brief Arrow operator.
		 *
		 *  @return Pointer to the contained pointed node @a _Ty instance.
		 */
		pf_hint_nodiscard pf_decl_constexpr _Ty *operator->() const pf_attr_noexcept
		{
			return this->base()->store_;
		}

		/// Operator Cast: node *
		/*! @brief Cast operator to node *. Convert this instance type to a node object with the stored
		 *				 iterator.
		 *
		 *  @return Converted pointer of type node*.
		 */
		pf_hint_nodiscard pf_decl_constexpr operator node *() const pf_attr_noexcept
		{
			return this->base();
		}

		/// Operator==
		/*! @brief Equality operator. Checks if two iterators have the same pointer.
		 *
		 *  @param[in] __r Another iterator to compare with.
		 *  @return True if is equal.
		 *  @return False if isn't equal.
		 */
		pf_hint_nodiscard pf_decl_constexpr bool operator==(
				doubly_iterator<_Ty> const &__r) const pf_attr_noexcept
		{
			return this->ptr_ == __r.ptr_;
		}
		/*! @brief Equality operator. Checks if contained iterator pointer is the same as @a __p .
		 *
		 *  @param[in] __p Pointer on a node to compare with.
		 *  @return True if is equal.
		 *  @return False if isn't equal.
		 */
		pf_hint_nodiscard pf_decl_constexpr bool operator==(
				node *__p) const pf_attr_noexcept
		{
			return this->ptr_ == __p;
		}

	private:
		node *ptr_;
	};

	/*! @brief Specialization of a doubly iterator with the type @a _Ty = const _Ty (const iterator).
	 *
	 *  @tparam @a _Ty = const _Ty
	 */
	template <typename _Ty>
	class doubly_iterator<const _Ty>
	{
	public:
		using iterator_category = std::bidirectional_iterator_tag;
		using value_type				= const _Ty;
		using node							= const doubly_node<_Ty>;
		using difference_type		= diff_t;

		/// Constructors
		/*! @brief Constructor.
		 *
		 *  @param[in] __n Const pointer to a doubly linked node.
		 */
		pf_decl_constexpr doubly_iterator(
				node *__n) pf_attr_noexcept
				: ptr_(__n)
		{}
		/*! @brief Copy constructor.
		 *
		 *  @param[in] __i Another const iterator from which to copy the pointer.
		 */
		pf_decl_constexpr doubly_iterator(
				doubly_iterator<const _Ty> const &__i) pf_attr_noexcept
				: ptr_(__i.ptr_)
		{}

		/// Operator=
		/*! @brief Copy assignment operator.
		 *
		 *  @param[in] __i Another const iterator from which to copy the pointer.
		 *  @return Reference on this instance.
		 */
		pf_decl_constexpr doubly_iterator<const _Ty> &operator=(
				doubly_iterator<const _Ty> const &__i) pf_attr_noexcept
		{}

		/// Operator++
		/*! @brief Pre-incremental operator.
		 *
		 *  @return Reference on this instance after incrementation.
		 */
		pf_decl_constexpr doubly_iterator<const _Ty> &operator++() pf_attr_noexcept
		{
			this->ptr_ = this->ptr_->next_;
			return *this;
		}
		/*! @brief Post-incremental operator.
		 *
		 *  @param[in] __i Identifier for pre incremental operator.
		 *  @return Copy of this instance before incrementation.
		 */
		pf_decl_constexpr doubly_iterator<const _Ty> operator++(
				int32_t __i) pf_attr_noexcept
		{
			doubly_iterator<const _Ty> c = this->ptr_;
			this->ptr_									 = this->ptr_->prev_;
			return c;
		}

		/// Operator+=
		/*! @brief Addition operator.
		 *
		 *  @param[in] __i Difference to be applied to the iterator as is this->ptr_ = this->ptr_->next_
		 *  					 		 by __i times.
		 *  @return Reference on this instance after difference.
		 */
		pf_decl_constexpr doubly_iterator<const _Ty> &operator+=(
				difference_type __i) pf_attr_noexcept
		{
			for (; __i > 0; --__i)
			{
				this->ptr_ = this->ptr_->next_;
			}
			for (; __i < 0; ++__i)
			{
				this->ptr_ = this->ptr_->prev_;
			}
			return *this;
		}

		/// Operator--
		/*! @brief Pre-decremental operator.
		 *
		 *  @return Reference on this instance after decrementation.
		 */
		pf_decl_constexpr doubly_iterator<const _Ty> &operator--() pf_attr_noexcept
		{
			this->ptr_ = this->ptr_->prev_;
			return *this;
		}
		/*! @brief Post-decremental operator.
		 *
		 *  @param[in] __i Identifier for pre-decremental operator.
		 *  @return Copy of this instance before decrementation.
		 */
		pf_decl_constexpr doubly_iterator<const _Ty> operator--(
				int32_t __i) pf_attr_noexcept
		{
			doubly_iterator<const _Ty> c = this->ptr_;
			this->ptr_									 = this->ptr_->prev_;
			return c;
		}

		/// Operator-=
		/*! @brief Addition operator.
		 *
		 *  @param[in] __i Difference to be applied to the iterator as is this->ptr_ = this->ptr_->prev_
		 *  					 by __i times.
		 *  @return Reference on this instance after difference.
		 */
		pf_decl_constexpr doubly_iterator<const _Ty> &operator-=(
				difference_type __i) pf_attr_noexcept
		{
			for (; __i < 0; ++__i)
			{
				this->ptr_ = this->ptr_->prev_;
			}
			for (; __i > 0; --__i)
			{
				this->ptr_ = this->ptr_->next_;
			}
			return *this;
		}

		/// Base
		/*! @brief Get the pointer associated with this iterator.
		 *
		 *  @return Const pointer on a singly_node of this iterator.
		 */
		pf_hint_nodiscard pf_decl_constexpr node *base() const pf_attr_noexcept
		{
			return this->ptr_;
		}

		/// Operator*
		/*! @brief Deference operator.
		 *
		 *  @return Const reference to the contained pointed node @a _Ty instance.
		 */
		pf_hint_nodiscard pf_decl_constexpr const _Ty &operator*() const pf_attr_noexcept
		{
			return this->base()->store_;
		}

		/// Operator->
		/*! @brief Arrow operator.
		 *
		 *  @return Const pointer to the contained pointed node @a _Ty instance.
		 */
		pf_hint_nodiscard pf_decl_constexpr const _Ty *operator->() const pf_attr_noexcept
		{
			return &this->base()->store_;
		}

		/// Operator Cast: node *
		/*! @brief Cast operator to node *. Convert this instance type to a node object with the stored
		 *				 iterator.
		 *
		 *  @return Converted pointer of type node*.
		 */
		pf_hint_nodiscard pf_decl_constexpr operator node *() const pf_attr_noexcept
		{
			return this->base();
		}

		/// Operator==
		/*! @brief Equality operator. Checks if two iterators have the same pointer.
		 *
		 *  @param[in] __r Another iterator to compare with.
		 *  @return True if is equal.
		 *  @return False if isn't equal.
		 */
		pf_hint_nodiscard pf_decl_constexpr bool operator==(
				doubly_iterator<const _Ty> const &__r) const pf_attr_noexcept
		{
			return this->ptr_ == __r.ptr_;
		}
		/*! @brief Equality operator. Checks if contained iterator pointer is the same as @a __p .
		 *
		 *  @param[in] __p Pointer on a node to compare with.
		 *  @return True if is equal.
		 *  @return False if isn't equal.
		 */
		pf_hint_nodiscard pf_decl_constexpr bool operator==(
				node *__p) const pf_attr_noexcept
		{
			return this->ptr_ == __p;
		}

	private:
		node *ptr_;
	};

	/// DOUBLY: Iterator Operator+
	/*! @brief Addition operator.
	 *
	 *  @tparam _Ty Type of the encapsulated object.
	 *  @param[in] __it Iterator from which to apply the difference.
	 *  @param[in] __i Difference.
	 *  @return Copy of the iterator checking __it += __i.
	 */
	template <typename _Ty>
	pf_hint_nodiscard pf_decl_constexpr doubly_iterator<_Ty> operator+(
			doubly_iterator<_Ty> __it,
			diff_t __i) pf_attr_noexcept
	{
		return __it += __i;
	}

	/// DOUBLY: Iterator Operator-
	/*! @brief Addition operator.
	 *
	 *  @tparam _Ty Type of the encapsulated object.
	 *  @param[in] __it Iterator from which to apply the difference.
	 *  @param[in] __i Difference.
	 *  @return Copy of the iterator checking __it -= __i.
	 */
	template <typename _Ty>
	pf_hint_nodiscard pf_decl_constexpr doubly_iterator<_Ty> operator-(
			doubly_iterator<_Ty> __it,
			diff_t __i) pf_attr_noexcept
	{
		return __it -= __i;
	}

	/*! @brief Alias of a constant iterator.
	 *
	 *  @tparam _Ty Type of the encapsulated object.
	 */
	template <typename _Ty>
	using doubly_const_iterator = doubly_iterator<const std::remove_const_t<_Ty>>;

	/// DOUBLY: Const Iterator -> Deduction Guide
	template <typename _Ty>
	doubly_iterator(const doubly_node<_Ty> *)
			-> doubly_iterator<const _Ty>;

	/*! @brief Doubly reverse iterator for doubly linked nodes uses.
	 *
	 *  @tparam _Ty Type of working object.
	 */
	template <typename _Ty>
	class doubly_reverse_iterator
	{
		pf_decl_friend doubly_reverse_iterator<const _Ty>;

	public:
		using iterator_category = std::bidirectional_iterator_tag;
		using value_type				= _Ty;
		using node							= doubly_node<_Ty>;
		using difference_type		= diff_t;

		/// Constructors
		/*! @brief Constructor.
		 *
		 *  @param[in] __n Pointer to a doubly linked node.
		 */
		pf_decl_constexpr doubly_reverse_iterator(
				node *__n) pf_attr_noexcept
				: ptr_(__n)
		{}
		/*! @brief Copy constructor.
		 *
		 *  @param[in] __i Another iterator from which to copy the pointer.
		 */
		pf_decl_constexpr doubly_reverse_iterator(
				doubly_reverse_iterator<_Ty> const &__r) pf_attr_noexcept
				: ptr_(__r.ptr_)
		{}

		/// Operator=
		/*! @brief Copy assignment operator.
		 *
		 *  @param[in] __i Another iterator from which to copy the pointer.
		 *  @return Reference on this instance.
		 */
		pf_decl_constexpr doubly_reverse_iterator<_Ty> &operator=(
				doubly_reverse_iterator<_Ty> const &__r) pf_attr_noexcept
		{
			this->ptr_ = __r.ptr_;
			return *this;
		}

		/// Operator++
		/*! @brief Pre-incremental operator.
		 *
		 *  @return Reference on this instance after incrementation.
		 */
		pf_decl_constexpr doubly_reverse_iterator<_Ty> &operator++() pf_attr_noexcept
		{
			this->ptr_ = this->ptr_->prev_;
		}
		/*! @brief Post-incremental operator.
		 *
		 *  @param[in] __i Identifier for pre-incremental operator.
		 *  @return Copy of this instance before incrementation.
		 */
		pf_decl_constexpr doubly_reverse_iterator<_Ty> operator++(
				int32_t __i) pf_attr_noexcept
		{
			doubly_reverse_iterator<_Ty> c = this->ptr_;
			this->ptr_										 = this->ptr_->prev_;
			return c;
		}

		/// Operator+=
		/*! @brief Addition operator.
		 *
		 *  @param[in] __i Difference to be applied to the iterator as is this->ptr_ = this->ptr_->prev_
		 *  					 		 by __i times.
		 *  @return Reference on this instance after difference.
		 */
		pf_decl_constexpr doubly_reverse_iterator<_Ty> &operator+=(
				difference_type __i) pf_attr_noexcept
		{
			for (; __i > 0; --__i)
			{
				this->ptr_ = this->ptr_->prev_;
			}
			for (; __i < 0; ++__i)
			{
				this->ptr_ = this->ptr_->next_;
			}
			return *this;
		}

		/// Operator--
		/*! @brief Pre-incremental operator.
		 *
		 *  @return Reference on this instance after incrementation.
		 */
		pf_decl_constexpr doubly_reverse_iterator<_Ty> &operator--() pf_attr_noexcept
		{
			this->ptr_ = this->ptr_->next_;
			return *this;
		}
		/*! @brief Post-incremental operator.
		 *
		 *  @param[in] __i Identifier for pre-incremental operator.
		 *  @return Copy of this instance before incrementation.
		 */
		pf_decl_constexpr doubly_reverse_iterator<_Ty> operator--(
				int32_t __i) pf_attr_noexcept
		{
			doubly_reverse_iterator<_Ty> c = this->ptr_;
			this->ptr_										 = this->ptr_->next_;
			return c;
		}

		/// Operator-=
		/*! @brief Addition operator.
		 *
		 *  @param[in] __i Difference to be applied to the iterator as is this->ptr_ = this->ptr_->next_
		 *  					 		 by __i times.
		 *  @return Reference on this instance after difference.
		 */
		pf_decl_constexpr doubly_reverse_iterator &operator-=(
				difference_type __i) pf_attr_noexcept
		{
			for (; __i < 0; ++__i)
			{
				this->ptr_ = this->ptr_->next_;
			}
			for (; __i > 0; --__i)
			{
				this->ptr_ = this->ptr_->prev_;
			}
			return *this;
		}

		/// Base
		/*! @brief Get the pointer associated with this iterator.
		 *
		 *  @return Pointer on a singly_node of this iterator.
		 */
		pf_hint_nodiscard pf_decl_constexpr node *base() const pf_attr_noexcept
		{
			return this->ptr_;
		}

		/// Operator*
		/*! @brief Deference operator.
		 *
		 *  @return Reference to the contained pointed node @a _Ty instance.
		 */
		pf_hint_nodiscard pf_decl_constexpr _Ty &operator*() const pf_attr_noexcept
		{
			return this->base()->store_;
		}

		/// Operator->
		/*! @brief Arrow operator.
		 *
		 *  @return Pointer to the contained pointed node @a _Ty instance.
		 */
		pf_hint_nodiscard pf_decl_constexpr _Ty *operator->() const pf_attr_noexcept
		{
			return this->base()->store_;
		}

		/// Operator Cast : node *
		/*! @brief Cast operator to node *. Convert this instance type to a node object with the stored
		 *				 iterator.
		 *
		 *  @return Converted pointer of type node*.
		 */
		pf_hint_nodiscard pf_decl_constexpr operator node *() const pf_attr_noexcept
		{
			return this->base();
		}

		/// Operator==
		/*! @brief Equality operator. Checks if two iterators have the same pointer.
		 *
		 *  @param[in] __r Another iterator to compare with.
		 *  @return True if is equal.
		 *  @return False if isn't equal.
		 */
		pf_hint_nodiscard pf_decl_constexpr bool operator==(
				doubly_reverse_iterator<_Ty> const &__r) const pf_attr_noexcept
		{
			return this->ptr_ == __r.ptr_;
		}
		/*! @brief Equality operator. Checks if contained iterator pointer is the same as @a __p .
		 *
		 *  @param[in] __p Pointer on a node to compare with.
		 *  @return True if is equal.
		 *  @return False if isn't equal.
		 */
		pf_hint_nodiscard pf_decl_constexpr bool operator==(
				node *__p) const pf_attr_noexcept
		{
			return this->ptr_ == __p;
		}

	private:
		node *ptr_;
	};

	/*! @brief Specialization of a reverse doubly iterator with the type @a _Ty = const _Ty (const
	 *				 iterator).
	 *
	 *  @tparam @a _Ty = const _Ty
	 */
	template <typename _Ty>
	class doubly_reverse_iterator<const _Ty>
	{
	public:
		using iterator_category = std::bidirectional_iterator_tag;
		using value_type				= const _Ty;
		using node							= const doubly_node<_Ty>;
		using difference_type		= diff_t;

		/// Constructors
		/*! @brief Constructor.
		 *
		 *  @param[in] __n Const pointer to a doubly linked node.
		 */
		pf_decl_constexpr doubly_reverse_iterator(
				node *__n) pf_attr_noexcept
				: ptr_(__n)
		{}
		/*! @brief Copy constructor.
		 *
		 *  @param[in] __i Another const iterator from which to copy the pointer.
		 */
		pf_decl_constexpr doubly_reverse_iterator(
				doubly_reverse_iterator<const _Ty> const &__r) pf_attr_noexcept
				: ptr_(__r.ptr_)
		{}

		/// Operator=
		/*! @brief Copy assignment operator.
		 *
		 *  @param[in] __i Another const iterator from which to copy the pointer.
		 *  @return Reference on this instance.
		 */
		pf_decl_constexpr doubly_reverse_iterator<_Ty> &operator=(
				doubly_reverse_iterator<const _Ty> const &__r) pf_attr_noexcept
		{
			this->ptr_ = __r.ptr_;
			return *this;
		}

		/// Operator++
		/*! @brief Pre-incremental operator.
		 *
		 *  @return Reference on this instance after incrementation.
		 */
		pf_decl_constexpr doubly_reverse_iterator<const _Ty> &operator++() pf_attr_noexcept
		{
			this->ptr_ = this->ptr_->prev_;
		}
		/*! @brief Post-incremental operator.
		 *
		 *  @param[in] __i Identifier for pre incremental operator.
		 *  @return Copy of this instance before incrementation.
		 */
		pf_decl_constexpr doubly_reverse_iterator<const _Ty> operator++(
				int32_t __i) pf_attr_noexcept
		{
			doubly_reverse_iterator<const _Ty> c = this->ptr_;
			this->ptr_													 = this->ptr_->prev_;
			return c;
		}

		/// Operator+=
		/*! @brief Addition operator.
		 *
		 *  @param[in] __i Difference to be applied to the iterator as is this->ptr_ = this->ptr_->prev_
		 *  					 		 by __i times.
		 *  @return Reference on this instance after difference.
		 */
		pf_decl_constexpr doubly_reverse_iterator<const _Ty> &operator+=(
				difference_type __i) pf_attr_noexcept
		{
			for (; __i > 0; --__i)
			{
				this->ptr_ = this->ptr_->prev_;
			}
			for (; __i < 0; ++__i)
			{
				this->ptr_ = this->ptr_->next_;
			}
			return *this;
		}

		/// Operator--
		/*! @brief Pre-incremental operator.
		 *
		 *  @return Reference on this instance after incrementation.
		 */
		pf_decl_constexpr doubly_reverse_iterator<const _Ty> &operator--() pf_attr_noexcept
		{
			this->ptr_ = this->ptr_->next_;
			return *this;
		}
		/*! @brief Post-incremental operator.
		 *
		 *  @param[in] __i Identifier for pre incremental operator.
		 *  @return Copy of this instance before incrementation.
		 */
		pf_decl_constexpr doubly_reverse_iterator<const _Ty> operator--(
				int32_t __i) pf_attr_noexcept
		{
			doubly_reverse_iterator<const _Ty> c = this->ptr_;
			this->ptr_													 = this->ptr_->next_;
			return c;
		}

		/// Operator-=
		/*! @brief Addition operator.
		 *
		 *  @param[in] __i Difference to be applied to the iterator as is this->ptr_ = this->ptr_->next_
		 *  					 		 by __i times.
		 *  @return Reference on this instance after difference.
		 */
		pf_decl_constexpr doubly_reverse_iterator &operator-=(
				difference_type __i) pf_attr_noexcept
		{
			for (; __i < 0; ++__i)
			{
				this->ptr_ = this->ptr_->next_;
			}
			for (; __i > 0; --__i)
			{
				this->ptr_ = this->ptr_->prev_;
			}
			return *this;
		}

		/// Base
		/*! @brief Get the pointer associated with this iterator.
		 *
		 *  @return Const pointer on a singly_node of this iterator.
		 */
		pf_hint_nodiscard pf_decl_constexpr node *base() const pf_attr_noexcept
		{
			return this->ptr_;
		}

		/// Operator*
		/*! @brief Deference operator.
		 *
		 *  @return Const reference to the contained pointed node @a _Ty instance.
		 */
		pf_hint_nodiscard pf_decl_constexpr const _Ty &operator*() const pf_attr_noexcept
		{
			return this->base()->store_;
		}

		/// Operator->
		/*! @brief Arrow operator.
		 *
		 *  @return Const pointer to the contained pointed node @a _Ty instance.
		 */
		pf_hint_nodiscard pf_decl_constexpr const _Ty *operator->() const pf_attr_noexcept
		{
			return &this->base()->store_;
		}

		/// Operator Cast : node *
		/*! @brief Cast operator to node *. Convert this instance type to a node object with the stored
		 *				 iterator.
		 *
		 *  @return Converted pointer of type node*.
		 */
		pf_hint_nodiscard pf_decl_constexpr operator node *() const pf_attr_noexcept
		{
			return this->base();
		}

		/// Operator==
		/*! @brief Equality operator. Checks if two iterators have the same pointer.
		 *
		 *  @param[in] __r Another iterator to compare with.
		 *  @return True if is equal.
		 *  @return False if isn't equal.
		 */
		pf_hint_nodiscard pf_decl_constexpr bool operator==(
				doubly_reverse_iterator<const _Ty> const &__r) const pf_attr_noexcept
		{
			return this->ptr_ == __r.ptr_;
		}
		/*! @brief Equality operator. Checks if contained iterator pointer is the same as @a __p .
		 *
		 *  @param[in] __p Pointer on a node to compare with.
		 *  @return True if is equal.
		 *  @return False if isn't equal.
		 */
		pf_hint_nodiscard pf_decl_constexpr bool operator==(
				node *__p) const pf_attr_noexcept
		{
			return this->ptr_ == __p;
		}

	private:
		node *ptr_;
	};

	/// DOUBLY: Reverse-Iterator Operator+
	/*! @brief Addition operator.
	 *
	 *  @tparam _Ty Type of the encapsulated object.
	 *  @param[in] __it Iterator from which to apply the difference.
	 *  @param[in] __i Difference.
	 *  @return Copy of the iterator checking __it += __i.
	 */
	template <typename _Ty>
	pf_hint_nodiscard pf_decl_constexpr doubly_reverse_iterator<_Ty> operator+(
			doubly_reverse_iterator<_Ty> __it,
			diff_t __i) pf_attr_noexcept
	{
		return __it += __i;
	}

	/// DOUBLY: Reverse-Iterator Operator-
	/*! @brief Addition operator.
	 *
	 *  @tparam _Ty Type of the encapsulated object.
	 *  @param[in] __it Iterator from which to apply the difference.
	 *  @param[in] __i Difference.
	 *  @return Copy of the iterator checking __it -= __i.
	 */
	template <typename _Ty>
	pf_hint_nodiscard pf_decl_constexpr doubly_reverse_iterator<_Ty> operator-(
			doubly_reverse_iterator<_Ty> __it,
			diff_t __i) pf_attr_noexcept
	{
		return __it -= __i;
	}

	/*! @brief Alias of a constant doubly iterator.
	 *
	 *  @tparam _Ty Type of the encapsulated object.
	 */
	template <typename _Ty>
	using doubly_const_reverse_iterator = doubly_iterator<const std::remove_const_t<_Ty>>;

	/// DOUBLY: Const Reverse Iterator -> Deduction Guide
	template <typename _Ty>
	doubly_reverse_iterator(const doubly_node<_Ty> *)
			-> doubly_reverse_iterator<const _Ty>;

	/*! @brief Doubly linked-list.
	 *
	 *  @tparam _Ty Type of the encapsulated object.
	 */
	template <typename _Ty>
	class doubly_linked_list
	{
	public:
		using value_type						 = _Ty;
		using node									 = doubly_node<_Ty>;
		using iterator							 = doubly_iterator<_Ty>;
		using const_iterator				 = doubly_const_iterator<_Ty>;
		using reverse_iterator			 = doubly_reverse_iterator<_Ty>;
		using const_reverse_iterator = doubly_const_reverse_iterator<_Ty>;

		/// Constructors
		/*! @brief Default constructor. Initializes the control variables to nullptr.
		 */
		pf_decl_constexpr doubly_linked_list() pf_attr_noexcept
				: head_(nullptr)
				, tail_(nullptr)
		{}
		/*! @brief Move constructor.
		 *
		 *  @param[in] __r Another instance to move from.
		 */
		pf_decl_constexpr doubly_linked_list(
				doubly_linked_list<_Ty> &&__r) pf_attr_noexcept
				: head_(__r.head_)
				, tail_(__r.tail_)
		{
			__r.head_ = nullptr;
			__r.tail_ = nullptr;
		}

		/// Destructor
		/*! @brief Destructor. Clear the linked-list.
		 */
		pf_decl_constexpr ~doubly_linked_list() pf_attr_noexcept
		{
			this->clear();
		}

		/// Operator=
		/*! @brief Move assignment operator.
		 *
		 *  @param[in] __r Another instance to move from. Sets to nullptr the control variables of the
		 * 								 other @a __r list.
		 *  @return Reference on this list.
		 */
		pf_decl_constexpr doubly_linked_list<_Ty> &operator=(
				doubly_linked_list<_Ty> &&__r) pf_attr_noexcept
		{
			if (!this->empty()) this->clear();
			this->head_ = __r.head_;
			__r.head_		= nullptr;
			this->tail_ = __r.tail_;
			__r.tail_		= nullptr;
			return *this;
		}

		/// Head
		/*! @brief Get a pointer to the head of the list.
		 *
		 *  @return Pointer to the head of the list.
		 */
		pf_hint_nodiscard pf_decl_constexpr node *head() pf_attr_noexcept
		{
			return this->head_;
		}
		/*! @brief Get a constant pointer to the head of the list.
		 *
		 *  @return Constant pointer to the head of the list.
		 */
		pf_hint_nodiscard pf_decl_constexpr const node *head() const pf_attr_noexcept
		{
			return this->head_;
		}

		/// Begin
		/*! @brief
		 *
		 *  @return pf_hint_nodiscard
		 */
		pf_hint_nodiscard pf_decl_constexpr iterator begin() pf_attr_noexcept
		{
			return this->head_;
		}
		/*! @brief
		 *
		 *  @return pf_hint_nodiscard
		 */
		pf_hint_nodiscard pf_decl_constexpr const_iterator cbegin() const pf_attr_noexcept
		{
			return this->head_;
		}
		/*! @brief
		 *
		 *  @return pf_hint_nodiscard
		 */
		pf_hint_nodiscard pf_decl_constexpr const_iterator begin() const pf_attr_noexcept
		{
			return this->cbegin();
		}

		/// Reverse End
		/*! @brief
		 *
		 *  @return pf_hint_nodiscard
		 */
		pf_hint_nodiscard pf_decl_constexpr iterator rend() pf_attr_noexcept
		{
			return nullptr;
		}
		/*! @brief
		 *
		 *  @return pf_hint_nodiscard
		 */
		pf_hint_nodiscard pf_decl_constexpr const_iterator crend() const pf_attr_noexcept
		{
			return nullptr;
		}
		/*! @brief
		 *
		 *  @return pf_hint_nodiscard
		 */
		pf_hint_nodiscard pf_decl_constexpr const_iterator rend() const pf_attr_noexcept
		{
			return this->crend();
		}

		/// Tail
		/*! @brief Get a pointer to the tail of the list.
		 *
		 *  @return Pointer to the tail of the list.
		 */
		pf_hint_nodiscard pf_decl_constexpr node *tail() pf_attr_noexcept
		{
			return this->tail_;
		}
		/*! @brief Get a constant pointer to the tail of the list.
		 *
		 *  @return Constant pointer to the tail of the list.
		 */
		pf_hint_nodiscard pf_decl_constexpr const node *tail() const pf_attr_noexcept
		{
			return this->tail_;
		}

		/// End
		/*! @brief
		 *
		 *  @return pf_hint_nodiscard
		 */
		pf_hint_nodiscard pf_decl_constexpr iterator end() pf_attr_noexcept
		{
			return nullptr;
		}
		/*! @brief
		 *
		 *  @return pf_hint_nodiscard
		 */
		pf_hint_nodiscard pf_decl_constexpr const_iterator cend() const pf_attr_noexcept
		{
			return nullptr;
		}
		/*! @brief
		 *
		 *  @return pf_hint_nodiscard
		 */
		pf_hint_nodiscard pf_decl_constexpr const_iterator end() const pf_attr_noexcept
		{
			return this->cend();
		}

		/// Reverse Begin
		/*! @brief
		 *
		 *  @return pf_hint_nodiscard
		 */
		pf_hint_nodiscard pf_decl_constexpr iterator rbegin() pf_attr_noexcept
		{
			return this->tail_;
		}
		/*! @brief
		 *
		 *  @return pf_hint_nodiscard
		 */
		pf_hint_nodiscard pf_decl_constexpr const_iterator crbegin() const pf_attr_noexcept
		{
			return this->tail_;
		}
		/*! @brief
		 *
		 *  @return pf_hint_nodiscard
		 */
		pf_hint_nodiscard pf_decl_constexpr const_iterator rbegin() const pf_attr_noexcept
		{
			return this->cbegin();
		}

		/// Insert
		/*! @brief Link @a __n in the place of the head of the list.
		 *
		 *  @param[in] __n Pointer on a node to link.
		 *  @return Pointer on the linked node.
		 */
		pf_decl_constexpr node *insert_head(
				node *__n) pf_attr_noexcept
		{
			pf_assert(__n != nullptr, "__n is nullptr!");
			pf_assert(!this->contains(__n), "__n is already linked to this list!");
			pf_assert(!__n->is_linked(), "__n is already linked!");
			if (!this->head_)
			{
				this->head_ = __n;
				this->tail_ = __n;
			}
			else
			{
				__n->next_				 = this->head_;
				this->head_->prev_ = __n;
				this->head_				 = __n;
			}
			return this->head_;
		}

		/*! @brief Link @a __n in the place of the tail of the list.
		 *
		 *  @param[in] __n Pointer on a node to link.
		 *  @return Pointer on the linked node.
		 */
		pf_decl_constexpr node *insert_tail(
				node *__n) pf_attr_noexcept
		{
			pf_assert(__n != nullptr, "__n is nullptr!");
			pf_assert(!this->contains(__n), "__n is already linked to this list!");
			pf_assert(!__n->is_linked(), "__n is already linked!");
			if (!this->tail_)
			{
				this->head_ = __n;
				this->tail_ = __n;
			}
			else
			{
				__n->prev_				 = this->tail_;
				this->tail_->next_ = __n;
				this->tail_				 = __n;
			}
			return this->tail_;
		}

		/*! @brief Link @a __n after @a __w.
		 *
		 *  @param[in] __w Pointer on a node after which to link @a __n.
		 *  @param[in] __n Pointer on a node to link.
		 *  @return Pointer on the linked node.
		 */
		pf_decl_constexpr node *insert_next(
				node *__w,
				node *__n) pf_attr_noexcept
		{
			if (!__w) return this->insert_head(__n);
			if (__w == this->tail_) return this->insert_tail(__n);
			pf_assert(__n != nullptr, "__n is nullptr!");
			pf_assert(!this->contains(__n), "__n is already linked to this list!");
			pf_assert(!__n->is_linked(), "__n is already linked!");
			__n->next_				= __w->next_;
			__n->next_->prev_ = __n;
			__w->next_				= __n;
			__n->prev_				= __w;
			return __n;
		}

		/*! @brief Link @a __n before @a __w.
		 *
		 *  @param[in] __w Pointer on a node before which to link @a __n.
		 *  @param[in] __n Pointer on a node to link.
		 *  @return Pointer on the linked node.
		 */
		pf_decl_constexpr node *insert_prev(
				node *__w,
				node *__n) pf_attr_noexcept
		{
			if (!__w) return this->insert_tail(__n);
			if (__w == this->head_) return this->insert_head(__n);
			pf_assert(__n != nullptr, "__n is nullptr!");
			pf_assert(this->contains(__n), "__n is already linked on this list!");
			pf_assert(__n->is_linked(), "__n is already linked!");
			__n->prev_				= __w->prev_;
			__n->prev_->next_ = __n;
			__w->prev_				= __n;
			__n->next_				= __w;
			return __n;
		}

		/// Remove
		/*! @brief Unlink the tail of the list.
		 *
		 *  @return Pointer on the unlinked node.
		 */
		pf_decl_constexpr node *remove_head() pf_attr_noexcept
		{
			if (this->empty()) return nullptr;
			node *c			= this->head_;
			this->head_ = this->head_->next_;
			if (this->head_)
			{
				this->head_->prev_ = nullptr;
			}
			else
			{
				this->tail_ = nullptr;
			}
			c->__unlink();
			return c;
		}

		/*! @brief Unlink the tail of the list.
		 *
		 *  @return Pointer on the unlinked node.
		 */
		pf_decl_constexpr node *remove_tail() pf_attr_noexcept
		{
			if (this->empty()) return nullptr;
			node *c			= this->tail_;
			this->tail_ = this->tail_->prev_;
			if (this->tail_)
			{
				this->tail_->next_ = nullptr;
			}
			else
			{
				this->head_ = nullptr;
			}
			c->__unlink();
			return c;
		}

		/*! @brief Unlink a specific @a __n node.
		 *
		 *  @param[in] __n Pointer on a node to unlink.
		 *  @return Pointer on the unlinked node.
		 */
		pf_decl_constexpr node *remove(
				node *__n) pf_attr_noexcept
		{
			if (!__n) return nullptr;
			if (__n == this->head_) return this->remove_head();
			if (__n == this->tail_) return this->remove_tail();
			node *n = __n->next_, *p = __n->prev_;
			n->prev_ = p;
			p->next_ = n;
			__n->__unlink();
			return __n;
		}

		/*! @brief Unlink the node after @a __w.
		 *
		 *  @param[in] __w Pointer on a node after which to unlink a node.
		 *  @return Pointer on the unlinked node.
		 */
		pf_decl_constexpr node *remove_next(
				node *__n) pf_attr_noexcept
		{
			return this->remove(__n->next_);
		}

		/*! @brief Unlink the node before @a __w.
		 *
		 *  @param[in] __n Pointer of a node before wich to unlink a node.
		 *  @return Pointer on the unlinked node.
		 */
		pf_decl_constexpr node *remove_prev(
				node *__n) pf_attr_noexcept
		{
			return this->remove(__n->prev_);
		}

		/// Empty
		/*! @brief Checks if this list is empty.
		 *
		 *  @return True if this list is empty.
		 *  @return False if this list isn't empty.
		 */
		pf_hint_nodiscard pf_decl_constexpr bool empty() const pf_attr_noexcept
		{
			return !this->head_;
		}

		/// Contains
		/*! @brief Check if @a __n is contained in this list.
		 *
		 *  @param[in] __n Pointer on a node.
		 *  @return True if @a __n is contained in this list.
		 *  @return False otherwise.
		 */
		pf_hint_nodiscard pf_decl_constexpr bool contains(
				const node *__n) const pf_attr_noexcept
		{
			const node *c = this->head_;
			while (c)
			{
				if (c == __n) return true;
				c = c->next_;
			}
			return false;
		}

		/// Size
		/*! @brief Retrieves the size of the list.
		 *
		 *  @return The size of the list. 0 if empty.
		 */
		pf_hint_nodiscard pf_decl_constexpr size_t size() const pf_attr_noexcept
		{
			size_t i			= 0;
			const node *c = this->head_;
			while (c)
			{
				c = c->next_;
				++i;
			}
			return i;
		}

		/// Clear
		/*! @brief Unlink all the nodes associated with this list.
		 */
		pf_decl_constexpr void clear() pf_attr_noexcept
		{
			while (!this->empty()) this->remove_head();
		}

	private:
		node *head_;
		node *tail_;
	};

	/*! @brief Doubly linked-list of nodes. Rotative version wich means, the tail is always linked to
	 *				 the head except if nullptr.
	 *
	 *  @tparam _Ty Type of the encapsulated object.
	 */
	template <typename _Ty>
	class doubly_rotative_linked_list
	{
	public:
		using value_type						 = _Ty;
		using node									 = doubly_node<_Ty>;
		using iterator							 = doubly_iterator<_Ty>;
		using const_iterator				 = doubly_const_iterator<std::remove_const_t<_Ty>>;
		using reverse_iterator			 = doubly_reverse_iterator<_Ty>;
		using const_reverse_iterator = doubly_const_reverse_iterator<std::remove_const_t<_Ty>>;

		/// Constructors
		/*! @brief Default constructor. Initializes the control variables to nullptr.
		 */
		pf_decl_constexpr doubly_rotative_linked_list() pf_attr_noexcept
				: head_(nullptr)
				, tail_(nullptr)
		{}
		/*! @brief Move constructor.
		 *
		 *  @param[in] __r Another instance to move from.
		 */
		pf_decl_constexpr doubly_rotative_linked_list(
				doubly_rotative_linked_list<_Ty> &&__r) pf_attr_noexcept
				: head_(__r.head_)
				, tail_(__r.tail_)
		{
			__r.head_ = nullptr;
			__r.tail_ = nullptr;
		}

		/// Destructor
		/*! @brief Destructor. Clear the linked-list.
		 */
		pf_decl_constexpr ~doubly_rotative_linked_list() pf_attr_noexcept
		{
			this->clear();
		}

		/// Operator=
		/*! @brief Move assignment operator.
		 *
		 *  @param[in] __r Another instance to move from. Sets to nullptr the control variables of the
		 * 								 other @a __r list.
		 *  @return Reference on this list.
		 */
		pf_decl_constexpr doubly_rotative_linked_list<_Ty> &operator=(
				doubly_rotative_linked_list<_Ty> &&__r) pf_attr_noexcept
		{
			if (!this->empty()) this->clear();
			this->head_ = __r.head_;
			__r.head_		= nullptr;
			this->tail_ = __r.tail_;
			__r.tail_		= nullptr;
			return *this;
		}

		/// Head
		/*! @brief Get a pointer on the head of the list.
		 *
		 *  @return Pointer on the head of the list.
		 */
		pf_hint_nodiscard pf_decl_constexpr node *head() pf_attr_noexcept
		{
			return this->head_;
		}
		/*! @brief Get a constant pointer on the head of the list.
		 *
		 *  @return Constant pointer on the head of the list.
		 */
		pf_hint_nodiscard pf_decl_constexpr const node *head() const pf_attr_noexcept
		{
			return this->head_;
		}

		/// Begin
		/*! @brief
		 *
		 *  @return pf_hint_nodiscard
		 */
		pf_hint_nodiscard pf_decl_constexpr iterator begin() pf_attr_noexcept
		{
			return this->head_;
		}
		/*! @brief
		 *
		 *  @return pf_hint_nodiscard
		 */
		pf_hint_nodiscard pf_decl_constexpr const_iterator cbegin() const pf_attr_noexcept
		{
			return this->head_;
		}
		/*! @brief
		 *
		 *  @return pf_hint_nodiscard
		 */
		pf_hint_nodiscard pf_decl_constexpr const_iterator begin() const pf_attr_noexcept
		{
			return this->cbegin();
		}

		/// Reverse End
		/*! @brief
		 *
		 *  @return pf_hint_nodiscard
		 */
		pf_hint_nodiscard pf_decl_constexpr iterator rend() pf_attr_noexcept
		{
			return nullptr;
		}
		/*! @brief
		 *
		 *  @return pf_hint_nodiscard
		 */
		pf_hint_nodiscard pf_decl_constexpr const_iterator crend() const pf_attr_noexcept
		{
			return nullptr;
		}
		/*! @brief
		 *
		 *  @return pf_hint_nodiscard
		 */
		pf_hint_nodiscard pf_decl_constexpr const_iterator rend() const pf_attr_noexcept
		{
			return this->crend();
		}

		/// Tail
		/*! @brief Get a pointer on the tail of the list.
		 *
		 *  @return Pointer on the tail of the list.
		 */
		pf_hint_nodiscard pf_decl_constexpr node *tail() pf_attr_noexcept
		{
			return this->tail_;
		}
		/*! @brief Get a constant pointer on the tail of the list.
		 *
		 *  @return Constant pointer on the tail of the list.
		 */
		pf_hint_nodiscard pf_decl_constexpr const node *tail() const pf_attr_noexcept
		{
			return this->tail_;
		}

		/// End
		/*! @brief
		 *
		 *  @return pf_hint_nodiscard
		 */
		pf_hint_nodiscard pf_decl_constexpr iterator end() pf_attr_noexcept
		{
			return nullptr;
		}
		/*! @brief
		 *
		 *  @return pf_hint_nodiscard
		 */
		pf_hint_nodiscard pf_decl_constexpr const_iterator cend() const pf_attr_noexcept
		{
			return nullptr;
		}
		/*! @brief
		 *
		 *  @return pf_hint_nodiscard
		 */
		pf_hint_nodiscard pf_decl_constexpr const_iterator end() const pf_attr_noexcept
		{
			return this->cend();
		}

		/// Reverse Begin
		/*! @brief
		 *
		 *  @return pf_hint_nodiscard
		 */
		pf_hint_nodiscard pf_decl_constexpr iterator rbegin() pf_attr_noexcept
		{
			return this->tail_;
		}
		/*! @brief
		 *
		 *  @return pf_hint_nodiscard
		 */
		pf_hint_nodiscard pf_decl_constexpr const_iterator crbegin() const pf_attr_noexcept
		{
			return this->tail_;
		}
		/*! @brief
		 *
		 *  @return pf_hint_nodiscard
		 */
		pf_hint_nodiscard pf_decl_constexpr const_iterator rbegin() const pf_attr_noexcept
		{
			return this->crend();
		}

		/// Insert
		/*! @brief Link @a __n in the place of the head of the list.
		 *
		 *  @param[in] __n Pointer on a node to link.
		 *  @return Pointer on the linked node.
		 */
		pf_decl_constexpr node *insert_head(
				node *__n) pf_attr_noexcept
		{
			pf_assert(__n != nullptr, "__n is nullptr!");
			pf_assert(!this->contains(__n), "__n is already linked to this list!");
			pf_assert(!__n->is_linked(), "__n is already linked!");
			if (!this->head_)
			{
				this->head_ = __n;
				this->tail_ = __n;
				__n->next_	= __n;
				__n->prev_	= __n;
			}
			else
			{
				__n->next_				 = this->head_;
				__n->prev_				 = this->tail_;
				this->head_->prev_ = __n;
				this->head_				 = __n;
				this->tail_->next_ = __n;
			}
			return this->head_;
		}

		/*! @brief Link @a __n in the place of the tail of this list.
		 *
		 *  @param[in] __n Pointer on a node to link.
		 *  @return Pointer on the linked node.
		 */
		pf_decl_constexpr node *insert_tail(
				node *__n) pf_attr_noexcept
		{
			pf_assert(__n != nullptr, "__n is nullptr!");
			pf_assert(!this->contains(__n), "__n is already linked to this list!");
			pf_assert(!__n->is_linked(), "__n is already linked!");
			if (!this->tail_)
			{
				this->head_ = __n;
				this->tail_ = __n;
				__n->next_	= __n;
				__n->prev_	= __n;
			}
			else
			{
				__n->next_				 = this->head_;
				__n->prev_				 = this->tail_;
				this->tail_->next_ = __n;
				this->tail_				 = __n;
				this->head_->prev_ = __n;
			}
			return this->tail_;
		}

		/*! @brief Link @a __n after @a __w.
		 *
		 *  @param[in] __w Pointer on a node after which to link @a __n.
		 *  @param[in] __n Pointer on a node to link.
		 *  @return Pointer on the linked node.
		 */
		pf_decl_constexpr node *insert_next(
				node *__w,
				node *__n) pf_attr_noexcept
		{
			if (!__w) return this->insert_head(__n);
			if (__w == this->tail_) return this->insert_tail(__n);
			pf_assert(__n != nullptr, "__n is nullptr!");
			pf_assert(!this->contains(__n), "__n is already linked to this list!");
			pf_assert(!__n->is_linked(), "__n is already linked!");
			__n->next_				= __w->next_;
			__n->next_->prev_ = __n;
			__w->next_				= __n;
			__n->prev_				= __w;
			return __n;
		}

		/*! @brief Link @a __n before @a __w.
		 *
		 *  @param[in] __w Pointer on a node before which to link @a __n.
		 *  @param[in] __n Pointer on a node to link.
		 *  @return Pointer on the linked node.
		 */
		pf_decl_constexpr node *insert_prev(
				node *__w,
				node *__n) pf_attr_noexcept
		{
			if (!__w) return this->insert_tail(__n);
			if (__w == this->head_) return this->insert_head(__n);
			pf_assert(__n != nullptr, "__n is nullptr!");
			pf_assert(this->contains(__n), "__n is already linked on this list!");
			pf_assert(__n->is_linked(), "__n is already linked!");
			__n->prev_				= __w->prev_;
			__n->prev_->next_ = __n;
			__w->prev_				= __n;
			__n->next_				= __w;
			return __n;
		}

		/// Remove
		/*! @brief Unlink the head of this list.
		 *
		 *  @return Pointer on the unlinked node.
		 */
		pf_decl_constexpr node *remove_head() pf_attr_noexcept
		{
			if (this->empty()) return nullptr;
			node *c = this->head_;
			if (this->head_->next_ == this->head_)
			{
				this->head_ = nullptr;
				this->tail_ = nullptr;
			}
			else
			{
				this->head_				 = this->head_->next_;
				this->head_->prev_ = this->tail_;
				this->tail_->next_ = this->head_;
			}
			c->__unlink();
			return c;
		}

		/*! @brief Unlink the tail of this list.
		 *
		 *  @return Pointer on the unlinked node.
		 */
		pf_decl_constexpr node *remove_tail() pf_attr_noexcept
		{
			if (this->empty()) return nullptr;
			node *c = this->tail_;
			if (this->tail_->next_ == this->tail_)
			{
				this->head_ = nullptr;
				this->tail_ = nullptr;
			}
			else
			{
				this->tail_				 = this->tail_->prev_;
				this->tail_->next_ = this->head_;
				this->head_->prev_ = this->tail_;
			}
			c->__unlink();
			return c;
		}

		/*! @brief Unlink a specific @a __n node.
		 *
		 *  @param[in] __n Pointer on a node to unlink.
		 *  @return Pointer on the unlinked node.
		 */
		pf_decl_constexpr node *remove(
				node *__n) pf_attr_noexcept
		{
			pf_assert(__n != nullptr, "__n is nullptr!");
			pf_assert(this->contains(__n), "__n isn't contained in the list!");
			if (__n == this->head_) return this->remove_head();
			if (__n == this->tail_) return this->remove_tail();
			node *n = __n->next_, *p = __n->prev_;
			n->prev_ = p;
			p->next_ = n;
			__n->__unlink();
			return __n;
		}

		/*! @brief Unlink the node after @a __w.
		 *
		 *  @param[in] __w Pointer on a node after which to unlink a node.
		 *  @return Pointer on the unlinked node.
		 */
		pf_decl_constexpr node *remove_next(
				node *__n) pf_attr_noexcept
		{
			return this->remove(__n->next_);
		}

		/*! @brief Unlink the node before @a __w.
		 *
		 *  @param[in] __n Pointer of a node before wich to unlink a node.
		 *  @return Pointer on the unlinked node.
		 */
		pf_decl_constexpr node *remove_prev(
				node *__n) pf_attr_noexcept
		{
			return this->remove(__n->prev_);
		}

		/// Empty
		/*! @brief Checks if this list is empty.
		 *
		 *  @return True if this list is empty.
		 *  @return False if this list isn't empty.
		 */
		pf_hint_nodiscard pf_decl_constexpr bool empty() const pf_attr_noexcept
		{
			return !this->head_;
		}

		/// Contains
		/*! @brief Check if @a __n is contained in this list.
		 *
		 *  @param[in] __n Pointer on a node.
		 *  @return True if @a __n is contained in this list.
		 *  @return False otherwise.
		 */
		pf_hint_nodiscard pf_decl_constexpr bool contains(
				const node *__n) pf_attr_noexcept
		{
			pf_assert(__n != nullptr, "__n is nullptr");
			node *c = this->head_;
			do
			{
				if (c == __n) return true;
				c = c->next_;
			} while (c != this->head_);
			return false;
		}

		/// Size
		/*! @brief Retrieves the size of this list.
		 *
		 *  @return The size of this list. 0 if empty.
		 */
		pf_hint_nodiscard pf_decl_constexpr size_t size() const pf_attr_noexcept
		{
			if (this->empty()) return 0;
			size_t i = 0;
			node *c	 = this->head_;
			do
			{
				c = c->next_;
				++i;
			} while (c != this->head_);
			return i;
		}

		/// Clear
		/*! @brief Unlink all the nodes associated with this list.
		 */
		pf_decl_constexpr void clear() pf_attr_noexcept
		{
			while (!this->empty()) this->remove_head();
		}

	private:
		node *head_;
		node *tail_;
	};

	/// FUNCTION: Pointer
	/*! @brief Function pointer.
	 *
	 *  @tparam _Fun Encapsulated function type.
	 */
	template <typename _Fun>
	class fun_ptr;

	/*! @brief Function pointer specialization.
	 *
	 *  @tparam _Ret  Type of return
	 *  @tparam _Args Type of arguments.
	 */
	template <typename _Ret, typename... _Args>
	class fun_ptr<_Ret(_Args...)>
	{
	public:
		using return_t = _Ret;

		/// Constructors
		/*! @brief Default constructor.
		 */
		pf_decl_constexpr fun_ptr() pf_attr_noexcept
				: ptr_(nullptr)
		{}

		/*! @brief Constructor.
		 *
		 *  @param[in] __ptr Pointer of function.
		 */
		pf_decl_constexpr fun_ptr(
				_Ret (*__ptr)(_Args...)) pf_attr_noexcept
				: ptr_(__ptr)
		{}
		/*! @brief Constructor for functors.
		 *
		 *  @tparam _Fun Functor type.
		 *  @param[in] __ptr Function to copy.
		 */
		template <typename _Fun>
		pf_decl_constexpr fun_ptr(
				_Fun &&__ptr) pf_attr_noexcept
				: ptr_(std::move(__ptr))
		{}
		/*! @brief Copy constructor.
		 *
		 *  @param[in] __r Other function pointer.
		 */
		pf_decl_constexpr fun_ptr(
				fun_ptr<_Ret(_Args...)> const &__r) pf_attr_noexcept
				: ptr_(__r.ptr_)
		{}

		/// Operator=
		/*! @brief Assignment operator for functors.
		 *
		 *  @tparam _Fun Functor type.
		 *  @param[in] __ptr Function to copy.
		 *  @return This function pointer.
		 */
		template <typename _Fun>
		pf_decl_constexpr fun_ptr<_Ret(_Args...)> &operator=(
				_Fun &&__ptr) pf_attr_noexcept
		{
			this->ptr_ = __ptr;
		}
		/*! @brief Copy assignment operator.
		 *
		 *  @param[in] __r Other function pointer.
		 *  @return This function pointer.
		 */
		pf_decl_constexpr fun_ptr<_Ret(_Args...)> &operator=(
				fun_ptr<_Ret(_Args...)> const &__r) pf_attr_noexcept
		{
			this->ptr_ = __r.ptr_;
			return *this;
		}

		/// Operator()
		/*! @brief Call operator.
		 *
		 *  @param[in] __args Parameters to be sent to the encapsulated function pointer.
		 *  @return Value returned by the encapsulated function pointer.
		 */
		pf_decl_constexpr _Ret operator()(
				_Args &&...__args) const pf_attr_noexcept
		{
			return this->ptr_(std::forward<_Args>(__args)...);
		}

		/// Operator==
		/*! @brief Equality operator.
		 *
		 *  @param[in] __r Other function pointer.
		 *  @return True If both pointer are equals.
		 *  @return False Otherwise.
		 */
		pf_decl_constexpr bool operator==(
				fun_ptr<_Ret(_Args...)> const &__r) const pf_attr_noexcept
		{
			return this->ptr_ = __r.ptr_;
		}

	private:
		_Ret (*ptr_)(_Args...);
	};

	/// FUNCTION: Pointer Deduction Guide (Based on std::function)
	/*! @brief Structure to help deducing a function signature type.
	 */
	template <typename>
	struct fun_ptr_helper
	{};

	/*! @brief Deduction specialization for non-constant non-referred methods.
	 *
	 *  @tparam _Res  Return type.
	 *  @tparam _Tp   Type of class containing the method.
	 *  @tparam _Nx	  Is noexcept.
	 *  @tparam _Args Type of function's arguments.
	 */
	template <typename _Res, typename _Tp, bool _Nx, typename... _Args>
	struct fun_ptr_helper<
			_Res (_Tp::*)(_Args...) noexcept(_Nx)>
	{
		using type = _Res(_Args...);
	};

	/*! @brief Deduction specialization for non-constant referred methods.
	 *
	 *  @tparam _Res  Return type.
	 *  @tparam _Tp   Type of class containing the method.
	 *  @tparam _Nx	  Is noexcept.
	 *  @tparam _Args Type of function's arguments.
	 */
	template <typename _Res, typename _Tp, bool _Nx, typename... _Args>
	struct fun_ptr_helper<
			_Res (_Tp::*)(_Args...) &noexcept(_Nx)>
	{
		using type = _Res(_Args...);
	};

	/*! @brief Deduction specialization for constant non-referred methods.
	 *
	 *  @tparam _Res  Return type.
	 *  @tparam _Tp   Type of class containing the method.
	 *  @tparam _Nx	  Is noexcept.
	 *  @tparam _Args Type of function's arguments.
	 */
	template <typename _Res, typename _Tp, bool _Nx, typename... _Args>
	struct fun_ptr_helper<
			_Res (_Tp::*)(_Args...) const noexcept(_Nx)>
	{
		using type = _Res(_Args...);
	};

	/*! @brief Deduction specialization for constant referred methods.
	 *
	 *  @tparam _Res  Return type.
	 *  @tparam _Tp   Type of class containing the method.
	 *  @tparam _Nx	  Is noexcept.
	 *  @tparam _Args Type of function's arguments.
	 */
	template <typename _Res, typename _Tp, bool _Nx, typename... _Args>
	struct fun_ptr_helper<
			_Res (_Tp::*)(_Args...) const &noexcept(_Nx)>
	{
		using type = _Res(_Args...);
	};

	/*! @brief Deduction guide for function pointers.
	 *
	 *  @tparam _Res 		  Return type.
	 *  @tparam _ArgTypes Type of function's arguments.
	 */
	template <typename _Res, typename... _ArgTypes>
	fun_ptr(_Res (*)(_ArgTypes...)) -> fun_ptr<_Res(_ArgTypes...)>;

	/*! @brief Deduction guide for fonctors / methods.
	 *
	 *  @tparam _Functor 	 Functor type.
	 *  @tparam _Signature Encapsulated function type.
	 */
	template <typename _Functor, typename _Signature = typename fun_ptr_helper<decltype(&_Functor::operator())>::type>
	fun_ptr(_Functor) -> fun_ptr<_Signature>;
}

#endif // PULSAR_UTILITY_HPP