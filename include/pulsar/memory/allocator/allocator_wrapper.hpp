/*! @file allocator_wrapper.hpp
 *  @author Fluffy (noe.louis-quentin@hotmail.fr)
 *  @brief
 *  @date 11-06-2022
 *
 *  @copyright Copyright (c) 2022 - Pulsar Software
 *
 *  @since 0.1.1
 */

#ifndef PULSAR_ALLOCATOR_WRAPPER_HPP
#define PULSAR_ALLOCATOR_WRAPPER_HPP 1

// Include: Pulsar
#include "pulsar/memory.hpp"

// Pulsar
namespace pul
{
	// Memory
	namespace memory
	{
		/// MEMORY: Allocator -> Concept
		template <typename _Allocator>
		concept allocator_concept = requires(
				_Allocator _all,
				size_t __size,
				align_val_t __align,
				size_t __offset,
				void *__ptr)
		{
			{
				_all.allocate(__size, __align, __offset)
				} -> std::same_as<void *>;
			{
				_all.deallocate(__ptr)
				} -> std::same_as<void>;
		};

		/// MEMORY: Allocator -> Wrapper
		/*! @brief Type of allocator acting as an interface to the standard library. Takes as parameter
		 *				 another allocator.
		 *
		 *  @tparam _Ty				 Type of object to allocate.
		 *  @tparam _Allocator Type of allocator.
		 */
		template <typename _Ty, allocator_concept _Allocator>
		class allocator_wrapper
		{
		public:
			using value_type														 = _Ty;
			using size_type															 = size_t;
			using difference_type												 = diff_t;
			using propagate_on_container_copy_assignment = _Allocator::propagate_on_container_copy_assignment;
			using propagate_on_container_move_assignment = _Allocator::propagate_on_container_move_assignment;
			using propagate_on_container_swap						 = _Allocator::propagate_on_container_swap;

			/// Constructors
			/*! @brief Constructor.
			 *
			 *  @param[in] __all Allocator to encapsulate.
			 */
			pf_decl_inline pf_decl_constexpr allocator_wrapper(
					_Allocator &&__all) pf_attr_noexcept
					: all_(std::move(__all))
			{}
			/*! @brief Copy constructor.
			 *
			 *  @param[in] __allwrp Wrapper to copy from.
			 */
			pf_decl_inline pf_decl_constexpr allocator_wrapper(
					allocator_wrapper<_Ty, _Allocator> const &__allwrp) pf_attr_noexcept requires(std::is_copy_constructible_v<_Allocator>)
					: all_(__allwrp.all_)
			{}
			/*! @brief Move constructor.
			 *
			 *  @param[in] __allwrp Wrapper to move from.
			 */
			pf_decl_inline pf_decl_constexpr allocator_wrapper(
					allocator_wrapper<_Ty, _Allocator> &&__allwrp) pf_attr_noexcept requires(std::is_move_constructible_v<_Allocator>)
					: all_(std::move(__allwrp.all_))
			{}

			/// Operators=
			/*! @brief Copy assignment operator.
			 *
			 *  @param[in] __allwrp Wrapper to copy from.
			 *  @return Reference on this.
			 */
			pf_decl_inline pf_decl_constexpr allocator_wrapper<_Ty, _Allocator> &operator=(
					allocator_wrapper<_Ty, _Allocator> const &__allwrp) pf_attr_noexcept requires(std::is_copy_assignable_v<_Allocator>)
			{
				if (&__allwrp == this) return *this;
				this->all_ = __allwrp.all_;
				return *this;
			}
			/*! @brief Move assignment operator.
			 *
			 *  @param[in] __allwrp Wrapper to move from.
			 *  @return Reference on this.
			 */
			pf_decl_inline pf_decl_constexpr allocator_wrapper<_Ty, _Allocator> &operator=(
					allocator_wrapper<_Ty, _Allocator> &&__allwrp) pf_attr_noexcept requires(std::is_move_assignable_v<_Allocator>)
			{
				if (&__allwrp == this) return *this;
				this->all_ = std::move(__allwrp.all_);
				return *this;
			}

			/// Operator==
			/*! @brief Equality operator.
			 *
			 *  @param[in] __r Wrapper to compare with.
			 *  @return True if equals.
			 *  @return False otherwise.
			 */
			pf_hint_nodiscard pf_decl_inline pf_decl_constexpr bool operator==(
					allocator_wrapper<_Ty, _Allocator> const &__r) const pf_attr_noexcept
			{
				return this->all_ == __r.all_;
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
			pf_hint_nodiscard pf_decl_inline pf_decl_constexpr void *allocate(
					size_t __count,
					align_val_t __align = align_val_t(alignof(_Ty)),
					size_t __offset			= 0)
			{
				return this->all_.allocate(__count + paddingof(__count + __offset, __align), __align);
			}

			/// Deallocate
			/*! @brief Deallocates a memory pointed by @a __ptr.
			 *
			 *  @param[in] __ptr Pointer referring to a memory to be deallocated.
			 */
			pf_decl_inline pf_decl_constexpr void deallocate(
					void *__ptr) pf_attr_noexcept
			{
				this->all_.deallocate(__ptr);
			}

		private:
			_Allocator all_;
		};

	}
}

/// NEW: Allocator Allocate
/*! @brief Operator new for allocators.
 *
 *  @tparam _Allocator Type of allocator.
 *  @param[in] __size Size of allocation.
 *  @param[in] __all  Allocator instance.
 *  @return @ref __all.allocate(__size).
 */
template <pul::memory::allocator_concept _Allocator>
pf_hint_nodiscard pf_decl_inline void *operator new(
		pul::size_t __size,
		_Allocator &__all) pf_attr_noexcept
{
	return __all.allocate(__size);
}
/*! @brief Operator new for allocators.
 *
 *  @tparam _Allocator Type of allocator.
 *  @param[in] __size  Size of allocation.
 *  @param[in] __align Alignment of the allocation.
 *  @param[in] __all   Allocator instance.
 *  @return @ref __all.allocate(__size, __align).
 */
template <pul::memory::allocator_concept _Allocator>
pf_hint_nodiscard pf_decl_inline void *operator new(
		pul::size_t __size,
		pul::align_val_t __align,
		_Allocator &__all) pf_attr_noexcept
{
	return __all.allocate(__size, __align);
}
/*! @brief Operator new for allocators.
 *
 *  @tparam _Allocator Type of allocator.
 *  @param[in] __size   Size of allocation.
 *  @param[in] __align  Alignment of the allocation.
 *  @param[in] __offset Alignment offset of the allocation.
 *  @param[in] __all    Allocator instance.
 *  @return @ref __all.allocate(__size, __align, __offset).
 */
template <pul::memory::allocator_concept _Allocator>
pf_hint_nodiscard pf_decl_inline void *operator new(
		pul::size_t __size,
		pul::align_val_t __align,
		pul::size_t __offset,
		_Allocator &__all) pf_attr_noexcept
{
	return __all.allocate(__size, __align, __offset);
}
/*! @brief Operator new[] for allocators.
 *
 *  @tparam _Allocator Type of allocator.
 *  @param[in] __size Size of allocation.
 *  @param[in] __all  Allocator instance.
 *  @return @ref __all.allocate(__size).
 */
template <pul::memory::allocator_concept _Allocator>
pf_hint_nodiscard pf_decl_inline void *operator new[](
		pul::size_t __size,
		_Allocator &__all) pf_attr_noexcept
{
	return __all.allocate(__size);
}
/*! @brief Operator new[] for allocators.
 *
 *  @tparam _Allocator Type of allocator.
 *  @param[in] __size  Size of allocation.
 *  @param[in] __align Alignment of the allocation.
 *  @param[in] __all   Allocator instance.
 *  @return @ref __all.allocate(__size, __align).
 */
template <pul::memory::allocator_concept _Allocator>
pf_hint_nodiscard pf_decl_inline void *operator new[](
		pul::size_t __size,
		pul::align_val_t __align,
		_Allocator &__all) pf_attr_noexcept
{
	return __all.allocate(__size, __align);
}
/*! @brief Operator new[] for allocators.
 *
 *  @tparam _Allocator Type of allocator.
 *  @param[in] __size   Size of allocation.
 *  @param[in] __align  Alignment of the allocation.
 *  @param[in] __offset Alignment offset of the allocation.
 *  @param[in] __all    Allocator instance.
 *  @return @ref __all.allocate(__size, __align, __offset).
 */
template <pul::memory::allocator_concept _Allocator>
pf_hint_nodiscard pf_decl_inline void *operator new[](
		pul::size_t __size,
		pul::align_val_t __align,
		pul::size_t __offset,
		_Allocator &__all) pf_attr_noexcept
{
	return __all.allocate(__size, __align, __offset);
}

/// DELETE: Allocator Deallocate
/*! @brief Operator delete for allocators.
 *
 *  @tparam _Allocator Type of allocator.
 *  @param[in] __ptr Pointer on memory ressource allocated by @a __all.
 *  @param[in] __all Allocator instance.
 *  @return @ref __all.deallocate(__ptr).
 */
template <pul::memory::allocator_concept _Allocator>
pf_decl_inline void operator delete(
		void *__ptr,
		_Allocator &__all) pf_attr_noexcept
{
	__all.deallocate(__ptr);
}
/*! @brief Operator delete[] for allocators.
 *
 *  @tparam _Allocator Type of allocator.
 *  @param[in] __ptr Pointer on memory ressource allocated by @a __all.
 *  @param[in] __all Allocator instance.
 *  @return @ref __all.deallocate(__ptr).
 */
template <pul::memory::allocator_concept _Allocator>
pf_decl_inline void operator delete[](
		void *__ptr,
		_Allocator &__all) pf_attr_noexcept
{
	__all.deallocate(__ptr);
}

#endif // !PULSAR_ALLOCATOR_WRAPPER_HPP