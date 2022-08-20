/*! @file   memory_buffer.hpp
 *  @author Fluffy (noe.louis-quentin@hotmail.fr)
 *  @brief	Definition of memory buffer.
 *  @date   11-06-2022
 *
 *  @copyright Copyright (c) 2022 - Pulsar Software
 *
 *  @since 0.1.1
 */

#ifndef PULSAR_MEMORY_BUFFER_HPP
#define PULSAR_MEMORY_BUFFER_HPP 1

// Include: Pulsar
#include "pulsar/pulsar.hpp"

// Include: Pulsar -> Memory
#include "pulsar/memory.hpp"
#include "pulsar/memory/memory_utils.hpp"

// Include: C++
#include <iterator>

// Include: C
#include <cstring>

// Pulsar
namespace pul
{
	// Memory
	namespace memory
	{
		/// MEMORY: Buffer
		/*! @brief Memory buffer.
		 */
		class buffer
		{
		public:
			/// Constructors
			/*! @brief Constructor. Does not build dynamic memory.
			 */
			pf_decl_inline pf_decl_constexpr buffer() pf_attr_noexcept
					: align_(std::align_val_t(0))
					, beg_(nullptr)
					, end_(nullptr)
			{}
			/*! @brief Constructor. Builds a memory of size @a __size and aligned to @a __bufalign.
			 *
			 *  @param[in] __size			Size of the memory to build.
			 *  @param[in] __bufalign Alignment of this memory.
			 */
			pf_decl_inline pf_decl_constexpr buffer(
					size_t __size,
					align_val_t __bufalign = max_align) pf_attr_noexcept
					: align_(std::align_val_t(__bufalign))
					, beg_(new (align_) byte_t[(__size += padding_of(__size, __bufalign))])
					, end_(this->beg_ + __size)
			{
				pf_assert(this->capacity() % static_cast<size_t>(__bufalign) == 0,
									"Buffer size isn't aligned to __bufalign!");
				pf_assert(is_aligned(this->beg_, __bufalign),
									"Buffer starting address isn't aligned to __bufalign!");
				this->fill();
			}
			/*! @brief Copy constructor. Builds a memory of the same size as the other buffer @a __r.
			 *				 Doesn't copy memory, as types may uses their own operator= or copy constructors.
			 *
			 *  @param[in] __r				Other memory buffer to copy from.
			 */
			pf_decl_inline pf_decl_constexpr buffer(
					buffer const &__r) pf_attr_noexcept
					: buffer(__r.capacity(), __r.align_)
			{}
			/*! @brief Copy constructor. Builds a memory of the same size as the other buffer @a __r.
			 *				 Doesn't copy memory, as types may uses their own operator= or copy constructors.
			 *				 The newly created buffer is aligned to @a __bufalign.
			 *
			 *  @param[in] __r				Other memory buffer to copy from.
			 *  @param[in] __bufalign New alignment of this buffer.
			 */
			pf_decl_inline pf_decl_constexpr buffer(
					buffer const &__r,
					align_val_t __bufalign) pf_attr_noexcept
					: buffer(__r.capacity(), __bufalign)
			{}
			/*! @brief Move constructor. Moves a memory.
			 *
			 *  @param[in] __r Other memory buffer to move from.
			 */
			pf_decl_inline pf_decl_constexpr buffer(
					buffer &&__r) pf_attr_noexcept
					: align_(__r.align_)
					, beg_(__r.beg_)
					, end_(__r.end_)
			{
				__r.align_ = std::align_val_t(0);
				__r.beg_	 = nullptr;
				__r.end_	 = nullptr;
			}

			/// Destructor
			/*! @brief Destructor. Free the memory.
			 */
			pf_decl_inline pf_decl_constexpr ~buffer() pf_attr_noexcept
			{
				this->__free();
			}

			/// Operators=
			/*! @brief Copy assignment operator. Resize the memory at the same size as the other buffer
			 *				 @a __r. Doesn't copy memory, as types may uses their own operator= or copy
			 *				 constructors. Cleans owned memory before resizing. See @ref resize.
			 *
			 *  @param[in] __r Other memory buffer to copy from.
			 *  @return Reference on this memory buffer.
			 */
			pf_decl_inline pf_decl_constexpr buffer &operator=(
					buffer const &__r) pf_attr_noexcept
			{
				if (&__r == this) return *this;
				this->resize(__r.capacity(), max_align);
				return *this;
			}
			/*! @brief Move assignment operator. Moves a buffer memory. Cleans owned memory before
			 *				 resizing. See @ref resize.
			 *
			 *  @param[in] __r Other memory buffer to move from.
			 *  @return Reference on this memory buffer.
			 */
			pf_decl_inline pf_decl_constexpr buffer &operator=(
					buffer &&__r) pf_attr_noexcept
			{
				if (&__r == this) return *this;
				this->__free();
				this->align_ = __r.align_;
				this->beg_	 = __r.beg_;
				this->end_	 = __r.end_;
				__r.align_	 = std::align_val_t(0);
				__r.beg_		 = nullptr;
				__r.end_		 = nullptr;
				return *this;
			}

			/// Capacity
			/*! @brief Returns the maximum capacity of this buffer.
			 *
			 *  @return Capacity in bytes of this buffer.
			 */
			pf_decl_inline pf_decl_constexpr size_t capacity() const pf_attr_noexcept
			{
				return std::distance(this->beg_, this->end_);
			}

			/// Resize
			/*! @brief Resizes the memory buffer to a new size @a __newsize, aligned to @a __newalign,
			 *				 and fill the new memory to @a __val.
			 *
			 * 	@warning All the previous content are discarded (no internal copy!).
			 *
			 *  @param[in] __newsize	The new size of this buffer.
			 *  @param[in] __newalign	The new alignment of this buffer.
			 *  @param[in] __val			The filled value of the new memory.
			 *  @return Difference of size between the last memory size and the new one.
			 */
			pf_decl_inline pf_decl_constexpr diff_t resize(
					size_t __newsize,
					align_val_t __newalign = max_align,
					int32_t __val					 = '\0') pf_attr_noexcept
			{
				size_t s = this->capacity();
				if (__newsize == s)
					return 0;
				this->__free();
				if (__newsize == 0)
				{
					return -s;
				}
				this->align_ = std::align_val_t(__newalign);
				this->beg_	 = new (this->align_) byte_t[__newsize];
				this->end_	 = this->beg_ + __newsize;
				this->fill(__val);
				pf_assert(this->capacity() % static_cast<size_t>(__newalign) == 0,
									"Buffer size isn't aligned to __newalign!");
				pf_assert(is_aligned(this->beg_, __newalign),
									"Buffer starting address isn't aligned to __newalign!");
				return static_cast<diff_t>(this->capacity()) - static_cast<diff_t>(s);
			}

			/// Alignment
			/*! @brief Returns the current alignment of this buffer.
			 *
			 *  @return Alignment in power of two of this buffer.
			 */
			pf_decl_inline pf_decl_constexpr align_val_t alignment() const pf_attr_noexcept
			{
				return this->align_;
			}

			/// Fill
			/*! @brief Fill the memory with @a __val.
			 *
			 *  @param[in] __val Value to fill with.
			 */
			pf_decl_inline pf_decl_constexpr void fill(
					int32_t __val = '\0') const pf_attr_noexcept
			{
				std::memset(this->beg_, __val, this->capacity());
			}

			/// Begin
			/*! @brief Returns the beginning of the memory sequence.
			 *
			 *  @return Pointer on the beginning of the memory sequence.
			 */
			pf_decl_inline pf_decl_constexpr byte_t *begin() pf_attr_noexcept
			{
				return this->beg_;
			}

			/// End
			/*! @brief Returns the end of the memory sequence.
			 *
			 *  @return Pointer on the end of the memory sequence.
			 */
			pf_decl_inline pf_decl_constexpr byte_t *end() pf_attr_noexcept
			{
				return this->end_;
			}

		private:
			/// Free
			/*! @brief Free the memory buffer.
			 */
			pf_decl_inline pf_decl_constexpr void __free() pf_attr_noexcept
			{
				if (this->beg_)
				{
					::operator delete[](this->beg_, this->align_);
					this->beg_ = nullptr;
					this->end_ = nullptr;
				}
			}

			std::align_val_t align_;
			byte_t *beg_;
			byte_t *end_;
		};
	}
}

#endif // !PULSAR_MEMORY_BUFFER_HPP