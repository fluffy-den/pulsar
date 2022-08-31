/*! @file   allocator_cds_linear.hpp
 *  @author Fluffy (noe.louis-quentin@hotmail.fr)
 *  @brief
 *  @date   11-06-2022
 *
 *  @copyright Copyright (c) 2022 - Pulsar Software
 *
 *  @since 0.1.1
 */

#ifndef PULSAR_MEMORY_ALLOCATOR_CDS_LINEAR_HPP
#define PULSAR_MEMORY_ALLOCATOR_CDS_LINEAR_HPP 1

// Include: Pulsar
#include "pulsar/memory.hpp"

// Include: C++
#include <tuple>

// Pulsar
namespace pul
{
	/// MEMORY: CDS Allocator Linear
	/*! @brief Concurrent linear memory allocator.
	 */
	class allocator_cds_linear
	{
		pf_decl_friend class allocator_cds_stack;

	public:
		using propagate_on_container_copy_assignment = std::true_type;
		using propagate_on_container_move_assignment = std::false_type;
		using propagate_on_container_swap						 = std::true_type;

		/// Constructors
		/*! @brief Default constructor.
		 */
		pf_decl_inline pf_decl_constexpr allocator_cds_linear() pf_attr_noexcept
				: off_(0)
		{}
		/*! @brief Constructor.
		 *
		 *  @param[in] __size 		Size of the memory buffer.
		 *  @param[in] __bufalign Alignment of the memory buffer.
		 */
		pf_decl_inline pf_decl_constexpr allocator_cds_linear(
				size_t __size,
				align_val_t __bufalign = MAX_ALIGN) pf_attr_noexcept
				: buf_(__size, __bufalign)
				, off_(this->buf_.begin())
		{}
		/*! @brief Copy constructor.
		 *
		 *  @param[in] __r Other linear allocator to copy from.
		 */
		pf_decl_inline allocator_cds_linear(
				allocator_cds_linear const &__r) pf_attr_noexcept
				: buf_(__r.buf_)
				, off_(__r.off_.load(std::memory_order::acquire))
		{}
		/*! @brief Copy constructor.
		 *
		 *  @param[in] __r Other linear allocator to copy from.
		 *  @param[in] __bufalign Alignment of the buffer.
		 */
		pf_decl_inline allocator_cds_linear(
				allocator_cds_linear const &__r,
				align_val_t __bufalign) pf_attr_noexcept
				: buf_(__r.buf_, __bufalign)
				, off_(__r.off_.load(std::memory_order::acquire))
		{}
		/*! @brief Move constructor.
		 *
		 *	@param[in] __r Other linear allocator to copy from.
		 */
		pf_decl_inline allocator_cds_linear(
				allocator_cds_linear &&__r) pf_attr_noexcept
				: buf_(std::move(__r.buf_))
				, off_(__r.off_.exchange(nullptr, std::memory_order::acq_rel))
		{}

		/// Operators=
		/*! @brief Copy assignment operator.
		 *
		 *  @param[in] __r Other linear allocator to copy from.
		 *  @return Reference on this allocator.
		 */
		pf_decl_inline allocator_cds_linear &operator=(
				allocator_cds_linear const &__r) pf_attr_noexcept
		{
			if (&__r == this) return *this;
			this->buf_ = __r.buf_;
			this->off_.store(this->buf_.begin(), std::memory_order::release);
			return *this;
		}
		/*! @brief Move assignment operator.
		 *
		 *  @param[in] __r Other linear allocator to move from.
		 *  @return Reference on this allocator.
		 */
		pf_decl_inline allocator_cds_linear &operator=(
				allocator_cds_linear &&__r) pf_attr_noexcept
		{
			if (&__r == this) return *this;
			this->buf_ = __r.buf_;
			this->off_.store(__r.off_.exchange(nullptr, std::memory_order::acq_rel), std::memory_order::release);
			return *this;
		}

		/// Operator==
		/*! @brief Equality operator.
		 *
		 *  @param[in] __r Other Allocator to compare with.
		 *  @return False, can't be equal with any allocator (unique memory).
		 */
		template <typename _Allocator>
		pf_decl_inline pf_decl_constexpr bool operator==(
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
		pf_hint_nodiscard void *allocate(
				size_t __size,
				align_val_t __align = MAX_ALIGN,
				size_t __offset			= 0) pf_attr_noexcept
		{
			__size += paddingof(__size + __offset, __align);
			union
			{
				byte_t *as_byte;
				size_t as_addr;
			};
			as_byte		= this->off_.load(std::memory_order::relaxed);
			byte_t *e = nullptr;
			do
			{
				as_addr += paddingof(as_addr + __offset, __align);
				e = as_byte + __size;
				if (e > this->buf_.end())
					return nullptr;
			} while (!this->off_.compare_exchange_weak(
					as_byte,
					e,
					std::memory_order::acquire,
					std::memory_order::release));
			return as_byte;
		}

		/// Deallocate
		/*! @brief Deallocates a memory pointed by @a __ptr.
		 *
		 *  @param[in] __ptr Pointer referring to a memory to be deallocated.
		 */
		pf_decl_inline void deallocate(
				void *__ptr) pf_attr_noexcept
		{
			std::ignore = __ptr;
		}

		/// Clear
		/*! @brief Resets the allocator to zero.
		 *
		 *  @warning Must not be used with types using this memory that use dynamic allocations
		 *  				 themselves!
		 */
		pf_decl_inline void clear(
				std::memory_order __order = std::memory_order::relaxed) pf_attr_noexcept
		{
			this->off_.exchange(this->buf_.begin(), __order);
		}

	private:
		buffer buf_;
		std::atomic<byte_t *> off_;
	};
}

#endif // !PULSAR_MEMORY_ALLOCATOR_CDS_LINEAR_HPP