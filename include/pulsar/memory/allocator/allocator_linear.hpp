/*! @file   allocator_linear.hpp
 *  @author Fluffy (noe.louis-quentin@hotmail.fr)
 *  @brief
 *  @date   11-06-2022
 *
 *  @copyright Copyright (c) 2022 - Pulsar Software
 *
 *  @since 0.1.1
 */

#ifndef PULSAR_MEMORY_ALLOCATOR_LINEAR_HPP
#define PULSAR_MEMORY_ALLOCATOR_LINEAR_HPP 1

// Include: Pulsar
#include "pulsar/memory.hpp"

// Pulsar
namespace pul
{
	// Memory
	namespace memory
	{
		/// MEMORY: Allocator Linear
		/*! @brief Linear memory allocator.
		 */
		class allocator_linear
		{
			pf_decl_friend class allocator_stack;
			pf_decl_friend class allocator_ring;

		public:
			using propagate_on_container_copy_assignment = std::true_type;
			using propagate_on_container_move_assignment = std::false_type;
			using propagate_on_container_swap						 = std::true_type;

			/// Constructors
			/*! @brief Default constructor.
			 */
			pf_decl_constexpr allocator_linear() pf_attr_noexcept
					: off_(0)
			{}
			/*! @brief Constructor.
			 *
			 *  @param[in] __size 		Size of the memory buffer.
			 *  @param[in] __bufalign Alignment of the memory buffer.
			 */
			pf_decl_constexpr allocator_linear(
					size_t __size,
					align_val_t __bufalign = MAX_ALIGN) pf_attr_noexcept
					: buf_(__size, __bufalign)
					, off_(this->buf_.begin())
			{}
			/*! @brief Copy constructor.
			 *
			 *  @param[in] __r Other linear allocator to copy from.
			 */
			pf_decl_constexpr allocator_linear(
					allocator_linear const &__r) pf_attr_noexcept
					: buf_(__r.buf_)
					, off_(__r.off_)
			{}
			/*! @brief Copy constructor.
			 *
			 *  @param[in] __r 				Other linear allocator to copy from.
			 *  @param[in] __bufalign Alignment of the buffer.
			 */
			pf_decl_constexpr allocator_linear(
					allocator_linear const &__r,
					align_val_t __bufalign) pf_attr_noexcept
					: buf_(__r.buf_, __bufalign)
					, off_(__r.off_)
			{}
			/*! @brief Move constructor.
			 *
			 *	@param[in] __r Other linear allocator to copy from.
			 */
			pf_decl_constexpr allocator_linear(
					allocator_linear &&__r) pf_attr_noexcept
					: buf_(std::move(__r.buf_))
					, off_(__r.off_)
			{
				__r.off_ = nullptr;
			}

			/// Operators=
			/*! @brief Copy assignment operator.
			 *
			 *  @param[in] __r Other linear allocator to copy from.
			 *  @return Reference on this allocator.
			 */
			pf_decl_constexpr allocator_linear &operator=(
					allocator_linear const &__r) pf_attr_noexcept
			{
				if (&__r == this) return *this;
				this->buf_ = __r.buf_;
				this->off_ = this->buf_.begin();
				return *this;
			}
			/*! @brief Move assignment operator.
			 *
			 *  @param[in] __r Other linear allocator to move from.
			 *  @return Reference on this allocator.
			 */
			pf_decl_constexpr allocator_linear &operator=(
					allocator_linear &&__r) pf_attr_noexcept
			{
				if (&__r == this) return *this;
				this->buf_ = __r.buf_;
				this->off_ = __r.off_;
				return *this;
			}

			/// Operator==
			/*! @brief Equality operator.
			 *
			 *  @param[in] __r Other Allocator to compare with.
			 *  @return False, can't be equal with any allocator (unique memory).
			 */
			template <typename _Allocator>
			pf_decl_constexpr bool operator==(
					_Allocator const &__r) pf_attr_noexcept
			{
				return false;
			}

			/// Allocate
			/*! @brief Allocates a memory of size @a __size, aligned to @a __align at the head of
			 *				 @a __offset.
			 *
			 *  @param[in] __size 	Size of the allocation.
			 *  @param[in] __align	Alignment of the allocation.
			 *  @param[in] __offset	Offset to alignment.
			 *  @return Pointer on a allocated memory.
			 */
			pf_hint_nodiscard pf_decl_constexpr void *allocate(
					size_t __size,
					align_val_t __align = MAX_ALIGN,
					size_t __offset			= 0) pf_attr_noexcept
			{
				__size += padding_of(__size + __offset, __align);
				union
				{
					byte_t *as_byte;
					size_t as_addr;
				};
				as_byte = this->off_;
				as_addr += padding_of(as_addr + __offset, __align);
				byte_t *e = as_byte + __size;
				if (e > this->buf_.end())
					return nullptr;
				this->off_ = e;
				return as_byte;
			}

			/// Deallocate
			/*! @brief Deallocates a memory pointed by @a __ptr.
			 *
			 *  @param[in] __ptr Pointer referring to a memory to be deallocated.
			 */
			pf_decl_constexpr void deallocate(
					void *__ptr) pf_attr_noexcept
			{
				std::ignore = __ptr;
			}

			/// Clear
			/*! @brief Resets the allocator.
			 *
			 *  @warning Must not be used with types using this memory that use dynamic allocations
			 *  				 themselves!
			 */
			pf_decl_constexpr void clear() pf_attr_noexcept
			{
				this->off_ = this->buf_.begin();
			}

		private:
			buffer buf_;
			byte_t *off_;
		};
	}
}

#endif // !PULSAR_MEMORY_ALLOCATOR_LINEAR_HPP