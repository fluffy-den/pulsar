/*! @file   allocator.hpp
 *  @author Louis-Quentin Noé (noe.louis-quentin@hotmail.fr)
 *  @brief
 *  @date   05-03-2023
 *
 *  @copyright Copyright (c) 2023 - Pulsar Software
 *
 *  @since 0.1.4
 */

#ifndef PULSAR_ALLOCATOR_HPP
#define PULSAR_ALLOCATOR_HPP 1

// Include: Pulsar
#include "pulsar/pulsar.hpp"
#include "pulsar/malloc.hpp"
#include "pulsar/magnifier.hpp"
#include "pulsar/utility.hpp"
#include "pulsar/memory.hpp"

// Pulsar
namespace pul
{
	/// ALLOCATOR: halloc
	class allocator_halloc pf_attr_final
	{
	public:
		/// Constructors
		pf_decl_inline pf_decl_constexpr
		allocator_halloc() pf_attr_noexcept = default;
		pf_decl_inline pf_decl_constexpr
		allocator_halloc(allocator_halloc const &) = default;
		pf_decl_inline pf_decl_constexpr
		allocator_halloc(allocator_halloc &&) = default;

		/// Destructor
		pf_decl_inline pf_decl_constexpr ~allocator_halloc() pf_attr_noexcept = default;

		/// Operator =
		pf_decl_inline pf_decl_constexpr allocator_halloc &
		operator=(allocator_halloc const &) = default;
		pf_decl_inline pf_decl_constexpr allocator_halloc &
		operator=(allocator_halloc &&) = default;

		/// Allocate
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr void *
		allocate(
		 size_t __size,
		 align_val_t __align = ALIGN_DEFAULT,
		 size_t __offset		 = 0) pf_attr_noexcept
		{
			return halloc(__size, __align, __offset);
		}

		/// Reallocate
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr void *
		reallocate(
		 void *__ptr,
		 size_t __size,
		 align_val_t __align = ALIGN_DEFAULT,
		 size_t __offset		 = 0) pf_attr_noexcept
		{
			return hrealloc(__ptr, __size, __align, __offset);
		}

		/// Deallocate
		pf_decl_inline pf_decl_constexpr void
		deallocate(
		 void *__ptr) pf_attr_noexcept
		{
			hfree(__ptr);
		}

		/// Purge
		pf_decl_inline pf_decl_constexpr size_t
		purge() pf_attr_noexcept
		{
			return 0;
		}
	};

	/// ALLOCATOR: calloc
	class allocator_calloc pf_attr_final
	{
	public:
		/// Constructors
		pf_decl_inline pf_decl_constexpr
		allocator_calloc() pf_attr_noexcept = default;
		pf_decl_inline pf_decl_constexpr
		allocator_calloc(allocator_calloc const &) = default;
		pf_decl_inline pf_decl_constexpr
		allocator_calloc(allocator_calloc &&) = default;

		/// Destructor
		pf_decl_inline pf_decl_constexpr ~allocator_calloc() pf_attr_noexcept = default;

		/// Operator =
		pf_decl_inline pf_decl_constexpr allocator_calloc &
		operator=(allocator_calloc const &) = default;
		pf_decl_inline pf_decl_constexpr allocator_calloc &
		operator=(allocator_calloc &&) = default;

		/// Allocate
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr void *
		allocate(
		 size_t __size,
		 align_val_t __align = ALIGN_DEFAULT,
		 size_t __offset		 = 0) pf_attr_noexcept
		{
			return calloc(__size, __align, __offset);
		}

		/// Reallocate
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr void *
		reallocate(
		 void *__ptr,
		 size_t __size,
		 align_val_t __align = ALIGN_DEFAULT,
		 size_t __offset		 = 0) pf_attr_noexcept
		{
			return crealloc(__ptr, __size, __align, __offset);
		}

		/// Deallocate
		pf_decl_inline pf_decl_constexpr void
		deallocate(
		 void *__ptr) pf_attr_noexcept
		{
			cfree(__ptr);
		}

		/// Purge
		pf_decl_inline pf_decl_constexpr size_t
		purge() pf_attr_noexcept
		{
			return 0;
		}
	};

	/// ALLOCATOR: salloc
	class allocator_salloc pf_attr_final
	{
	public:
		/// Constructors
		pf_decl_inline pf_decl_constexpr
		allocator_salloc() pf_attr_noexcept = default;
		pf_decl_inline pf_decl_constexpr
		allocator_salloc(allocator_salloc const &) = default;
		pf_decl_inline pf_decl_constexpr
		allocator_salloc(allocator_salloc &&) = default;

		/// Destructor
		pf_decl_inline pf_decl_constexpr ~allocator_salloc() pf_attr_noexcept = default;

		/// Operator =
		pf_decl_inline pf_decl_constexpr allocator_salloc &
		operator=(allocator_salloc const &) = default;
		pf_decl_inline pf_decl_constexpr allocator_salloc &
		operator=(allocator_salloc &&) = default;

		/// Allocate
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr void *
		allocate(
		 size_t __size,
		 align_val_t __align = ALIGN_DEFAULT,
		 size_t __offset		 = 0) pf_attr_noexcept
		{
			return salloc(__size, __align, __offset);
		}

		/// Reallocate
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr void *
		reallocate(
		 void *__ptr,
		 size_t __size,
		 align_val_t __align = ALIGN_DEFAULT,
		 size_t __offset		 = 0) pf_attr_noexcept
		{
			return srealloc(__ptr, __size, __align, __offset);
		}

		/// Deallocate
		pf_decl_inline pf_decl_constexpr void
		deallocate(
		 void *__ptr) pf_attr_noexcept
		{
			sfree(__ptr);
		}

		/// Purge
		pf_decl_inline pf_decl_constexpr size_t
		purge() pf_attr_noexcept
		{
			return 0;
		}
	};

	/// ALLOCATOR: SFINAE -> Is Standard Allocator
	template<typename _Allocator>
	struct is_standard_allocator : std::false_type
	{};
	template<>
	struct is_standard_allocator<allocator_halloc> : std::true_type
	{};
	template<>
	struct is_standard_allocator<allocator_calloc> : std::true_type
	{};
	template<>
	struct is_standard_allocator<allocator_salloc> : std::true_type
	{};
	template<typename _Allocator>
	pf_decl_inline pf_decl_constexpr bool is_standard_allocator_v = is_standard_allocator<_Allocator>::value;

	/// ALLOCATOR: SFINAE -> Is Custom Allocator
	template<typename _Allocator>
		requires(is_allocator_v<_Allocator>)
	struct is_custom_allocator : std::integral_constant<bool, !is_standard_allocator_v<_Allocator>>
	{};
	template<typename _Allocator>
	pf_decl_inline pf_decl_constexpr bool is_custom_allocator_v = is_custom_allocator<_Allocator>::value;

	/// ALLOCATOR: Pointer
	template<typename _Allocator, typename _Deleter>
		requires(is_allocator_v<_Allocator>)
	class allocator_unique_ptr
	{
	public:
		/// Constructors
		template<typename... _Args>
		pf_decl_inline pf_decl_constexpr
		allocator_unique_ptr(
		 _Args &&...__args)
			requires(std::is_constructible_v<_Allocator, _Args...>)
			: ptr_(std::forward<_Args>(__args)...)
		{}
		pf_decl_inline pf_decl_constexpr
		allocator_unique_ptr(
		 unique_store<_Allocator> *__store) pf_attr_noexcept
			: ptr_(__store)
		{}
		allocator_unique_ptr(allocator_unique_ptr<_Allocator, _Deleter> const &) = delete;
		pf_decl_inline pf_decl_constexpr
		allocator_unique_ptr(
		 allocator_unique_ptr<_Allocator, _Deleter> &&__other) pf_attr_noexcept
		{
			this->ptr_ = std::move(__other.ptr_);
		}

		/// Destructor
		pf_decl_inline pf_decl_constexpr ~allocator_unique_ptr() pf_attr_noexcept = default;

		/// Operator =
		allocator_unique_ptr<_Allocator, _Deleter> &
		operator=(allocator_unique_ptr<_Allocator, _Deleter> const &) = delete;
		pf_decl_inline pf_decl_constexpr allocator_unique_ptr<_Allocator, _Deleter> &
		operator=(allocator_unique_ptr<_Allocator, _Deleter> &&) = default;

		/// Allocate
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr void *
		allocate(
		 size_t __size,
		 align_val_t __align,
		 size_t __offset) pf_attr_noexcept
		{
			return this->ptr_->allocate(__size, __align, __offset);
		}

		/// Reallocate
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr void *
		reallocate(
		 void *__ptr,
		 size_t __size,
		 align_val_t __align,
		 size_t __offset) pf_attr_noexcept
		{
			return this->ptr_->reallocate(__ptr, __size, __align, __offset);
		}

		/// Deallocate
		pf_decl_inline pf_decl_constexpr void
		deallocate(
		 void *__ptr) pf_attr_noexcept
		{
			return this->ptr_->deallocate(__ptr);
		}

		/// Allocator
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr _Allocator *
		get() pf_attr_noexcept
		{
			return this->ptr_.get();
		}
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const _Allocator *
		get() const pf_attr_noexcept
		{
			return this->ptr_.get();
		}

	private:
		unique_ptr<_Allocator, _Deleter> ptr_;
	};
	template<typename _Allocator, typename _Deleter>
		requires(is_allocator_v<_Allocator>)
	class allocator_shared_ptr
	{
	public:
		/// Constructors
		template<typename... _Args>
		pf_decl_inline pf_decl_constexpr
		allocator_shared_ptr(
		 _Args &&...__args)
			requires(std::is_constructible_v<_Allocator, _Args...>)
			: ptr_(std::forward<_Args>(__args)...)
		{}
		pf_decl_inline pf_decl_constexpr
		allocator_shared_ptr(
		 shared_store<_Allocator> *__store) pf_attr_noexcept
			: ptr_(__store)
		{}
		allocator_shared_ptr(allocator_shared_ptr<_Allocator, _Deleter> const &) = delete;
		pf_decl_inline pf_decl_constexpr
		allocator_shared_ptr(
		 allocator_shared_ptr<_Allocator, _Deleter> &&__other) pf_attr_noexcept
		{
			this->ptr_ = std::move(__other.ptr_);
		}

		/// Destructor
		pf_decl_inline pf_decl_constexpr ~allocator_shared_ptr() pf_attr_noexcept = default;

		/// Operator =
		allocator_shared_ptr<_Allocator, _Deleter> &
		operator=(allocator_shared_ptr<_Allocator, _Deleter> const &) = delete;
		pf_decl_inline pf_decl_constexpr allocator_shared_ptr<_Allocator, _Deleter> &
		operator=(allocator_shared_ptr<_Allocator, _Deleter> &&) = default;

		/// Allocate
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr void *
		allocate(
		 size_t __size,
		 align_val_t __align,
		 size_t __offset) pf_attr_noexcept
		{
			return this->ptr_->allocate(__size, __align, __offset);
		}

		/// Reallocate
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr void *
		reallocate(
		 void *__ptr,
		 size_t __size,
		 align_val_t __align,
		 size_t __offset) pf_attr_noexcept
		{
			return this->ptr_->reallocate(__ptr, __size, __align, __offset);
		}

		/// Deallocate
		pf_decl_inline pf_decl_constexpr void
		deallocate(
		 void *__ptr) pf_attr_noexcept
		{
			return this->ptr_->deallocate(__ptr);
		}

		/// Allocator
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr _Allocator *
		get() pf_attr_noexcept
		{
			return this->ptr_.get();
		}
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const _Allocator *
		get() const pf_attr_noexcept
		{
			return this->ptr_.get();
		}

	private:
		shared_ptr<_Allocator, _Deleter> ptr_;
	};


	/// ALLOCATOR: Linear
	template<
	 typename _Magnifier			= magnifier_default,
	 typename _MemoryProvider = allocator_halloc>
	class allocator_linear pf_attr_final
	{
		/// Type -> Header
		struct __header_t
		{
			size_t size;
		};

		/// Type -> Buffer
		struct __buffer_t
		{
			/// Constructors
			pf_decl_inline pf_decl_constexpr
			__buffer_t(
			 size_t __size) pf_attr_noexcept
				: size(__size)
				, offset(0)
				, next(nullptr)
			{}
			__buffer_t(__buffer_t const &) = delete;
			__buffer_t(__buffer_t &&)			 = delete;

			/// Destructor
			pf_decl_inline pf_decl_constexpr ~__buffer_t() pf_attr_noexcept = default;

			/// Operator =
			__buffer_t &
			operator=(__buffer_t const &) = delete;
			__buffer_t &
			operator=(__buffer_t &&) = delete;

			/// Allocate
			pf_hint_nodiscard pf_decl_constexpr void *
			__allocate(
			 size_t __size,
			 align_val_t __align,
			 size_t __offset) pf_attr_noexcept
			{
				__size	 += sizeof(__header_t);
				__offset += sizeof(__header_t);
				union
				{
					size_t as_size;
					byte_t *as_byte;
					__header_t *as_header;
				};
				as_byte							 = &this->buf[0] + this->offset;
				const size_t padding = paddingof(as_size + __offset, __align);
				as_byte							+= padding;
				if(pf_unlikely(this->offset + __size + padding > this->size)) return nullptr;
				as_header->size = __size;
				this->offset	 += __size + padding;
				return ++as_header;
			}

			/// Deallocate
			pf_decl_inline pf_decl_constexpr void
			__deallocate(
			 void *) pf_attr_noexcept
			{}

			/// Purge
			pf_decl_inline pf_decl_constexpr size_t
			__purge() pf_attr_noexcept
			{
				const size_t tmp = this->offset;
				this->offset		 = 0;
				return tmp;
			}

			/// Data
			const size_t size;
			size_t offset;
			__buffer_t *next;
			byte_t buf[];
		};

		/// Internal
		pf_decl_inline pf_decl_constexpr void
		__delete_all_buffers()
		{
			while(this->head_)
			{
				__buffer_t *next = this->head_->next;
				destroy_delete(this->provider_, this->head_);
				this->head_ = next;
			}
		}
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr void *
		__push_buffer_and_allocate(
		 size_t __size,
		 align_val_t __align,
		 size_t __offset)
		{
			const size_t ms = this->magnifier_(this->tail_->size);
			const size_t ss = __size > ms ? this->magnifier_(__size) : ms;
			auto buf				= new_construct_ex<__buffer_t>(this->provider_, ss, ss);
			if(pf_unlikely(!this->head_))
			{
				this->head_ = buf;
				this->tail_ = buf;
			}
			else
			{
				this->tail_->next = buf;
				this->tail_				= buf;
			}
			return this->tail_->__allocate(__size, __align, __offset);
		}

	public:
		/// Constructors
		pf_decl_constexpr
		allocator_linear(
		 size_t __startsize,
		 align_val_t __align,
		 _Magnifier &&__magnifier			= _Magnifier(),
		 _MemoryProvider &&__provider = _MemoryProvider())
			: head_(nullptr)
			, tail_(nullptr)
			, startsize_(__startsize)
			, align_(__align)
			, magnifier_(std::move(__magnifier))
			, provider_(std::move(__provider))
		{
			this->head_ = new_construct_ex<__buffer_t>(this->provider_, __startsize, __startsize);
			this->tail_ = this->head_;
		}
		template<typename _MagnifierR, typename _MemoryProviderR>
		pf_decl_inline pf_decl_constexpr
		allocator_linear(
		 allocator_linear<_MagnifierR, _MemoryProviderR> const &__other,
		 align_val_t __align,
		 _Magnifier &&__magnifier,
		 _MemoryProvider &&__provider = _MemoryProvider())
			: allocator_linear(__other.startsize_, __align, std::move(__magnifier), std::move(__provider))
		{}
		pf_decl_inline pf_decl_constexpr
		allocator_linear(
		 allocator_linear<_Magnifier, _MemoryProvider> const &__other,
		 align_val_t __align)
			: allocator_linear(__other.startsize_, __align, __other.magnifier_, __other.provider_)
		{}
		pf_decl_inline pf_decl_constexpr
		allocator_linear(
		 allocator_linear<_Magnifier, _MemoryProvider> const &__other)
			: allocator_linear(__other.startsize_, __other.align_, __other.magnifier_, __other.provider_)
		{}
		pf_decl_inline pf_decl_constexpr
		allocator_linear(
		 allocator_linear<_Magnifier, _MemoryProvider> &&__other) pf_attr_noexcept
			: head_(__other.head_)
			, tail_(__other.tail_)
			, startsize_(__other.startsize_)
			, align_(__other.align_)
			, magnifier_(std::move(__other.magnifier_))
			, provider_(std::move(__other.provider_))
		{
			__other.head_ = nullptr;
			__other.tail_ = nullptr;
		}

		/// Destructor
		pf_decl_inline pf_decl_constexpr ~allocator_linear() pf_attr_noexcept
		{
			this->__delete_all_buffers();
		}

		/// Operator =
		pf_decl_inline pf_decl_constexpr allocator_linear<_Magnifier, _MemoryProvider> &
		operator=(
		 allocator_linear<_Magnifier, _MemoryProvider> &&__other)
		{
			if(pf_likely(this != &__other))
			{
				this->__delete_all_buffers();
				this->head_			 = __other.head_;
				this->tail_			 = __other.tail_;
				this->startsize_ = __other.startsize_;
				this->align_		 = __other.align_;
				this->magnifier_ = std::move(__other.magnifier_);
				this->provider_	 = std::move(__other.provider_);
				__other.head_		 = nullptr;
				__other.tail_		 = nullptr;
			}
			return *this;
		}

		/// Allocate
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr void *
		allocate(
		 size_t __size,
		 align_val_t __align = ALIGN_DEFAULT,
		 size_t __offset		 = 0) pf_attr_noexcept
		{
			void *p;
			if(pf_unlikely(!this->tail_))
			{
				p = this->__push_buffer_and_allocate(__size, __align, __offset);
			}
			else
			{
				p = this->tail_->__allocate(__size, __align, __offset);
				if(pf_unlikely(!p))
				{
					p = this->__push_buffer_and_allocate(__size, __align, __offset);
				}
			}
			return p;
		}

		/// Deallocate
		pf_decl_inline pf_decl_constexpr void
		deallocate(void *) pf_attr_noexcept
		{}

		/// Reallocate
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr void *
		reallocate(
		 void *__ptr,
		 size_t __size,
		 align_val_t __align = ALIGN_DEFAULT,
		 size_t __offset		 = 0) pf_attr_noexcept
		{
			void *p = this->allocate(__size, __align, __offset);
			if(pf_unlikely(!p)) return nullptr;
			union
			{
				void *as_void;
				__header_t *as_header;
			};
			as_void = __ptr;
			--as_header;
			memcpy(p, __ptr, std::min(as_header->size, __size));
			return p;
		}

		/// Purge
		pf_decl_inline pf_decl_constexpr size_t
		purge()	 // <=> reset()
		{
			__buffer_t *c = this->head_->next;
			size_t s			= this->head_->__purge();
			while(c)
			{
				__buffer_t *n = c->next;
				s						 += c->__purge();
				destroy_delete(this->provider_, c);
				c = n;
			}
			return s;
		}

	private:
		/// Data
		__buffer_t *head_;
		__buffer_t *tail_;
		size_t startsize_;
		align_val_t align_;
		pf_hint_nounique_address _Magnifier magnifier_;
		pf_hint_nounique_address _MemoryProvider provider_;
	};

	/// ALLOCATOR: Stack
	template<
	 typename _Magnifier			= magnifier_default,
	 typename _MemoryProvider = allocator_halloc>
	class allocator_stack pf_attr_final
	{
		/// Type -> Header
		struct __header_t
		{
			size_t size;
		};

		/// Type -> Buffer
		struct __buffer_t
		{
			/// Constructors
			pf_decl_inline pf_decl_constexpr
			__buffer_t(
			 size_t __size) pf_attr_noexcept
				: size(__size)
				, offset(0)
				, next(nullptr)
			{}
			__buffer_t(__buffer_t const &) = delete;
			__buffer_t(__buffer_t &&)			 = delete;

			/// Destructor
			pf_decl_inline pf_decl_constexpr ~__buffer_t() pf_attr_noexcept = default;

			/// Operator =
			__buffer_t &
			operator=(__buffer_t const &) = delete;
			__buffer_t &
			operator=(__buffer_t &&) = delete;

			/// Allocate
			pf_hint_nodiscard pf_decl_constexpr void *
			__allocate(
			 size_t __size,
			 align_val_t __align,
			 size_t __offset) pf_attr_noexcept
			{
				__size	 += sizeof(__header_t);
				__offset += sizeof(__header_t);
				union
				{
					size_t as_size;
					byte_t *as_byte;
					__header_t *as_header;
				};
				as_byte							 = &this->buf[0] + this->offset;
				const size_t padding = paddingof(as_size + __offset, __align);
				as_byte							+= padding;
				if(pf_unlikely(this->offset + __size + padding > this->size)) return nullptr;
				as_header->size = __size;
				this->offset	 += __size + padding;
				return ++as_header;
			}

			/// Deallocate
			pf_decl_inline pf_decl_constexpr void
			__deallocate(
			 void *__ptr) pf_attr_noexcept
			{
				const size_t pos = distof(&this->buf[0u], --union_cast<__header_t *>(__ptr));
				if(pos < this->offset) this->offset = pos;
			}

			/// Purge
			pf_decl_inline pf_decl_constexpr size_t
			__purge()
			{
				const size_t tmp = this->offset;
				this->offset		 = 0;
				return tmp;
			}

			/// Data
			const size_t size;
			size_t offset;
			__buffer_t *next;
			byte_t buf[];
		};

		/// Internal
		pf_decl_inline pf_decl_constexpr void
		__delete_all_buffers()
		{
			while(this->head)
			{
				__buffer_t *next = this->head_->next;
				destroy_delete(this->provider_, this->head_);
				this->head_ = next;
			}
		}
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr void *
		__push_buffer_and_allocate(
		 size_t __size,
		 align_val_t __align,
		 size_t __offset)
		{
			const size_t ms = this->magnifier_(this->tail_->size);
			const size_t ss = __size > ms ? this->magnifier_(__size) : ms;
			auto buf				= new_construct_ex<__buffer_t>(this->provider_, ss, ss);
			if(pf_unlikely(!this->head_))
			{
				this->head_ = buf;
				this->tail_ = buf;
			}
			else
			{
				this->tail_->next = buf;
				this->tail_				= buf;
			}
			return this->tail_->__allocate(__size, __align, __offset);
		}

	public:
		/// Constructors
		pf_decl_constexpr
		allocator_stack(
		 size_t __startsize,
		 align_val_t __align,
		 _Magnifier &&__magnifier			= _Magnifier(),
		 _MemoryProvider &&__provider = _MemoryProvider())
			: head_(nullptr)
			, tail_(nullptr)
			, startsize_(__startsize)
			, align_(__align)
			, magnifier_(std::move(__magnifier))
			, provider_(std::move(__provider))
		{}
		template<typename _MagnifierR, typename _MemoryProviderR>
		pf_decl_inline pf_decl_constexpr
		allocator_stack(
		 allocator_stack<_MagnifierR, _MemoryProviderR> const &__other,
		 align_val_t __align,
		 _Magnifier &&__magnifier,
		 _MemoryProvider &&__provider = _MemoryProvider())
			: allocator_stack(__other.startsize_, __align, std::move(__magnifier), std::move(__provider))
		{}
		pf_decl_inline pf_decl_constexpr
		allocator_stack(
		 allocator_stack<_Magnifier, _MemoryProvider> const &__other,
		 align_val_t __align)
			: allocator_stack(__other.startsize_, __align, __other.magnifier_, __other.provider_)
		{}
		pf_decl_inline pf_decl_constexpr
		allocator_stack(
		 allocator_stack<_Magnifier, _MemoryProvider> const &__other)
			: allocator_stack(__other.startsize_, __other.align_, __other.magnifier_, __other.provider_)
		{}
		pf_decl_inline pf_decl_constexpr
		allocator_stack(
		 allocator_stack<_Magnifier, _MemoryProvider> &&__other)
			: head_(__other.head_)
			, tail_(__other.tail_)
			, startsize_(__other.startsize_)
			, align_(__other.align_)
			, magnifier_(std::move(__other.magnifier_))
			, provider_(std::move(__other.provider_))
		{
			__other.head_ = nullptr;
			__other.tail_ = nullptr;
		}

		/// Destructor
		pf_decl_inline pf_decl_constexpr ~allocator_stack() pf_attr_noexcept
		{
			this->__delete_all_buffers();
		}

		/// Operator =
		pf_decl_inline pf_decl_constexpr allocator_stack<_Magnifier, _MemoryProvider> &
		operator=(
		 allocator_stack<_Magnifier, _MemoryProvider> &&__other)
		{
			if(pf_likely(this != &__other))
			{
				this->__delete_all_buffers();
				this->head_			 = __other.head_;
				this->tail_			 = __other.tail_;
				this->startsize_ = __other.startsize_;
				this->align_		 = __other.align_;
				this->magnifier_ = std::move(__other.magnifier_);
				this->provider_	 = std::move(__other.provider_);
				__other.head_		 = nullptr;
				__other.tail_		 = nullptr;
			}
			return *this;
		}

		/// Allocate
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr void *
		allocate(
		 size_t __size,
		 align_val_t __align = ALIGN_DEFAULT,
		 size_t __offset		 = 0)
		{
			void *p;
			if(pf_unlikely(!this->tail_))
			{
				p = this->__push_buffer_and_allocate(__size, __align, __offset);
			}
			else
			{
				p = this->tail_->__allocate(__size, __align, __offset);
				if(pf_unlikely(!p))
				{
					p = this->__push_buffer_and_allocate(__size, __align, __offset);
				}
			}
			return p;
		}

		/// Deallocate
		pf_decl_inline pf_decl_constexpr void
		deallocate(
		 void *__ptr) pf_attr_noexcept
		{
			__header_t *h = this->__header_of(__ptr);
			__buffer_t *b = this->__buffer_of(__ptr);
			b->__deallocate(h);
		}
		pf_decl_inline pf_decl_constexpr size_t
		deallocate_and_purge(
		 void *__ptr)
		{
			this->deallocate(__ptr);
			return this->purge();
		}

		/// Reallocate
		pf_decl_inline pf_decl_constexpr void *
		reallocate(
		 void *__ptr,
		 size_t __size,
		 align_val_t __align = ALIGN_DEFAULT,
		 size_t __offset		 = 0)
		{
			__header_t *h = this->__header_of(__ptr);
			__buffer_t *b = this->__buffer_of(h);
			void *p;
			b->__deallocate(h);
			if(pf_unlikely(b != this->tail_))
			{
				p = this->tail_->__allocate(__size, __align, __offset);
			}
			else
			{
				p = b->__allocate(__size, __align, __offset);
			}
			if(pf_unlikely(!p))
			{
				p = this->__push_buffer_and_allocate(__size, __align, __offset);
			}
			memcpy(p, __ptr, std::min(h->size, __size));
			return p;
		}

		/// Purge
		pf_hint_nodiscard pf_decl_constexpr size_t
		purge() pf_attr_noexcept
		{
			if(pf_unlikely(this->head_ != this->tail_))
			{
				size_t size = 0;
				while(this->head_ != this->tail_ && !this->head_->head)
				{
					__buffer_t *next = this->head_->next;
					size						+= this->head_->size;
					destroy_delete(this->provider_, this->head_);
					this->head_ = next;
				}
				if(this->head_ == this->tail_)
				{
					if(this->head_->head == nullptr)
					{
						size			 += this->head_->size;
						this->head_ = union_cast<__buffer_t *>(this->provider_.reallocate(this->head_, this->startsize_, this->align_, sizeof(__buffer_t)));
						this->tail_ = this->head_;
						construct(this->head_);
					}
				}
				return size;
			}
			return 0;
		}

	private:
		/// Data
		__buffer_t *head_;
		__buffer_t *tail_;
		size_t startsize_;
		align_val_t align_;
		pf_hint_nounique_address _Magnifier magnifier_;
		pf_hint_nounique_address _MemoryProvider provider_;
	};

	/// ALLOCATOR: Ring Buffer
	template<
	 typename _Magnifier			= magnifier_default,
	 typename _MemoryProvider = allocator_halloc>
		requires(is_standard_allocator_v<_MemoryProvider>)
	class allocator_ring_buffer
	{
		/// Type -> Header
		struct __header_t
		{
			uint32_t position;
			uint32_t size;
			int32_t next;
			int32_t prev;
		};

		/// Type -> Buffer
		struct __buffer_t
		{
			pf_hint_nodiscard pf_decl_always_inline byte_t *
			__realign_allocation(
			 byte_t *__p,
			 size_t __size,
			 align_val_t __align,
			 size_t __offset) pf_attr_noexcept
			{
				__p = union_cast<byte_t *>(align_top(__p, __align, __offset));
				if(__p + __size > &this->seq[0] + this->size)
				{
					__p = union_cast<byte_t *>(align_top(&this->seq[0], __align, __offset));
				}
				return __p;
			}

			/// Constructors
			pf_decl_inline
			__buffer_t(
			 size_t __size) pf_attr_noexcept
				: size(__size)
				, next(nullptr)
				, head(nullptr)
				, tail(union_cast<__header_t *>(&this->seq[0]))
			{}
			__buffer_t(__buffer_t const &) = delete;
			__buffer_t(__buffer_t &&)			 = delete;

			/// Destructor
			pf_decl_inline ~__buffer_t() pf_attr_noexcept = default;

			/// Allocate
			pf_hint_nodiscard void *
			__allocate(
			 size_t __size,
			 align_val_t __align,
			 size_t __offset) pf_attr_noexcept
			{
				__size	 += sizeof(__header_t);
				__offset += sizeof(__header_t);
				union
				{
					__header_t *as_header;
					byte_t *as_byte;
				};
				// Allocate
				as_header = this->tail;
				if(pf_unlikely(!this->head))	// !head = empty list
																			//  head = first to dealloc
				{
					// Allocate
					as_byte							= this->__realign_allocation(as_byte, __size, __align, __offset);
					as_header->position = union_cast<uint32_t>(distof(this, as_byte));
					as_header->size			= union_cast<uint32_t>(__size);

					// Store
					this->tail = as_header;
					this->head = as_header;
				}
				else
				{
					// Allocate
					as_byte += as_header->size;
					as_byte	 = this->__realign_allocation(as_byte, __size, __align, __offset);

					// Check if good allocation
					byte_t *n = this->__realign_allocation(as_byte + __size, 0, __align, __offset);
					byte_t *h = union_cast<byte_t *>(this->head);
					byte_t *t = union_cast<byte_t *>(this->tail);
					if(pf_unlikely(h != t && (as_byte <= h && n > h))) return nullptr;

					// Construct
					as_header->position = union_cast<uint32_t>(distof(this, as_byte));
					as_header->size			= union_cast<uint32_t>(__size);
					as_header->prev			= union_cast<int32_t>(diffof(as_header, this->tail));

					// Store
					this->tail->next = union_cast<int32_t>(diffof(this->tail, as_header));
					this->tail			 = as_header;
				}
				return (++as_header);
			}

			/// Deallocate
			void
			__deallocate(
			 __header_t *__ptr) pf_attr_noexcept
			{
				union
				{
					__header_t *as_header;
					byte_t *as_byte;
				};
				as_header = __ptr;
				if(as_header == this->head)
				{
					as_byte		+= this->head->next;
					this->head = as_header;
					if(pf_unlikely(this->head == this->tail)) this->head = nullptr;
				}
				else
				{
					__header_t *prev = union_cast<__header_t *>(as_byte + as_header->prev);
					prev->next			+= as_header->next;
				}
			}

			/// Store
			const size_t size;
			__buffer_t *next;
			__header_t *head;
			__header_t *tail;
			byte_t seq[];
		};

		/// Header
		pf_hint_nodiscard pf_decl_always_inline pf_decl_constexpr __header_t *
		__header_of(
		 void *__ptr) pf_attr_noexcept
		{
			__header_t *h = union_cast<__header_t *>(__ptr);
			return --h;
		}
		pf_hint_nodiscard pf_decl_always_inline pf_decl_constexpr __buffer_t *
		__buffer_of(
		 __header_t *__ptr) pf_attr_noexcept
		{
			union
			{
				__buffer_t *as_buffer;
				__header_t *as_header;
				byte_t *as_byte;
			};
			as_header = __ptr;
			as_byte	 -= as_header->position;
			return as_buffer;
		}

		/// Buffer
		pf_decl_inline pf_decl_constexpr void *
		__push_buffer_and_allocate(
		 size_t __size,
		 align_val_t __align,
		 size_t __offset)
		{
			const size_t ms = this->magnifier_(this->tail_->size);
			const size_t ss = __size > ms ? this->magnifier_(__size) : ms;
			auto buf				= new_construct_ex<__buffer_t>(this->provider_, ss, ss);
			if(pf_unlikely(!this->head_))
			{
				this->head_ = buf;
				this->tail_ = buf;
			}
			else
			{
				this->tail_->next = buf;
				this->tail_				= buf;
			}
			return this->tail_->__allocate(__size, __align, __offset);
		}
		pf_decl_inline pf_decl_constexpr void
		__delete_all_buffers() pf_attr_noexcept
		{
			while(this->head_)
			{
				__buffer_t *next = this->head_->next;
				destroy_delete(this->provider_, this->head_);
				this->head_ = next;
			}
		}

	public:
		/// Constructors
		pf_decl_inline pf_decl_constexpr
		allocator_ring_buffer(
		 size_t __startsize,
		 align_val_t __align					= ALIGN_NO_ALIGN,
		 _Magnifier &&__magnifier			= _Magnifier(),
		 _MemoryProvider &&__provider = _MemoryProvider())
			: startsize_(__startsize)
			, align_(__align)
			, magnifier_(std::move(__magnifier))
			, provider_(std::move(__provider))
		{
			this->head_ = new_construct_ex<__buffer_t>(this->provider_, this->startsize_, this->startsize_);
			this->tail_ = this->head_;
		}
		template<typename _MagnifierR, typename _MemoryProviderR>
		pf_decl_inline pf_decl_constexpr
		allocator_ring_buffer(
		 allocator_ring_buffer<_MagnifierR, _MemoryProviderR> const &__other,
		 align_val_t __align,
		 _Magnifier &&__magnifier,
		 _MemoryProvider &&__provider = _MemoryProvider())
			: allocator_ring_buffer(__other.startsize_, __align, std::move(__magnifier), std::move(__provider))
		{}
		pf_decl_inline pf_decl_constexpr
		allocator_ring_buffer(
		 allocator_ring_buffer<_Magnifier, _MemoryProvider> const &__other,
		 align_val_t __align)
			: allocator_ring_buffer(__other.startsize_, __align, __other.magnifier_, __other.provider_)
		{}
		pf_decl_inline pf_decl_constexpr
		allocator_ring_buffer(
		 allocator_ring_buffer<_Magnifier, _MemoryProvider> const &__other)
			: allocator_ring_buffer(__other.startsize_, __other.align_, __other.magnifier_, __other.provider_)
		{}
		pf_decl_inline pf_decl_constexpr
		allocator_ring_buffer(
		 allocator_ring_buffer<_Magnifier, _MemoryProvider> &&__other) pf_attr_noexcept
			: head_(__other.head_)
			, tail_(__other.tail_)
			, startsize_(__other.startsize_)
			, align_(__other.align_)
			, magnifier_(std::move(__other.magnifier_))
			, provider_(std::move(__other.provider_))
		{
			__other.head_ = nullptr;
			__other.tail_ = nullptr;
		}

		/// Destructor
		pf_decl_inline pf_decl_constexpr ~allocator_ring_buffer() pf_attr_noexcept
		{
			this->__delete_all_buffers();
		}

		/// Operator =
		pf_decl_inline pf_decl_constexpr allocator_ring_buffer<_Magnifier, _MemoryProvider> &
		operator=(
		 allocator_ring_buffer<_Magnifier, _MemoryProvider> &&__other)
		{
			if(pf_likely(this != &__other))
			{
				this->__delete_all_buffers();
				this->head_			 = __other.head_;
				this->tail_			 = __other.tail_;
				this->startsize_ = __other.startsize_;
				this->align_		 = __other.align_;
				this->magnifier_ = std::move(__other.magnifier_);
				this->provider_	 = std::move(__other.provider_);
				__other.head_		 = nullptr;
				__other.tail_		 = nullptr;
			}
			return *this;
		}

		/// Allocate
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr void *
		allocate(
		 size_t __size,
		 align_val_t __align = ALIGN_DEFAULT,
		 size_t __offset		 = 0)
		{
			void *p;
			if(pf_unlikely(!this->tail_))
			{
				p = this->__push_buffer_and_allocate(__size, __align, __offset);
			}
			else
			{
				p = this->tail_->__allocate(__size, __align, __offset);
				if(pf_unlikely(!p))
				{
					p = this->__push_buffer_and_allocate(__size, __align, __offset);
				}
			}
			return p;
		}

		/// Deallocate
		pf_decl_inline pf_decl_constexpr void
		deallocate(
		 void *__ptr) pf_attr_noexcept
		{
			__header_t *h = this->__header_of(__ptr);
			__buffer_t *b = this->__buffer_of(h);
			b->__deallocate(h);
		}
		pf_decl_inline pf_decl_constexpr size_t
		deallocate_and_purge(
		 void *__ptr) pf_attr_noexcept
		{
			this->deallocate(__ptr);
			return this->purge();
		}

		/// Reallocate
		pf_decl_inline pf_decl_constexpr void *
		reallocate(
		 void *__ptr,
		 size_t __size,
		 align_val_t __align = ALIGN_DEFAULT,
		 size_t __offset		 = 0)
		{
			__header_t *h = this->__header_of(__ptr);
			__buffer_t *b = this->__buffer_of(h);
			void *p;
			b->__deallocate(h);
			if(pf_unlikely(b != this->tail_))
			{
				p = this->tail_->__allocate(__size, __align, __offset);
			}
			else
			{
				p = b->__allocate(__size, __align, __offset);
			}
			if(pf_unlikely(!p))
			{
				p = this->__push_buffer_and_allocate(__size, __align, __offset);
			}
			memcpy(p, __ptr, std::min(h->size, __size));
			return p;
		}

		/// Purge
		pf_hint_nodiscard pf_decl_constexpr size_t
		purge() pf_attr_noexcept
		{
			if(pf_unlikely(this->head_ != this->tail_))
			{
				size_t size = 0;
				while(this->head_ != this->tail_ && !this->head_->head)
				{
					__buffer_t *next = this->head_->next;
					size						+= this->head_->size;
					destroy_delete(this->provider_, this->head_);
					this->head_ = next;
				}
				if(this->head_ == this->tail_)
				{
					if(this->head_->head == nullptr)
					{
						size			 += this->head_->size;
						this->head_ = union_cast<__buffer_t *>(this->provider_.reallocate(this->head_, this->startsize_, this->align_, sizeof(__buffer_t)));
						this->tail_ = this->head_;
						construct(this->head_);
					}
				}
				return size;
			}
			return 0;
		}

	private:
		/// Data
		__buffer_t *head_;
		__buffer_t *tail_;
		size_t startsize_;
		align_val_t align_;
		pf_hint_nounique_address _Magnifier magnifier_;
		pf_hint_nounique_address _MemoryProvider provider_;
	};

	/// ALLOCATOR: Pool
	template<
	 typename _Magnifier			= magnifier_default,
	 typename _MemoryProvider = allocator_halloc>
	class allocator_pool pf_attr_final
	{
		/// Type -> Header
		struct __header_t
		{
			union
			{
				uint32_t prev;		// In deallocation state
				uint32_t offset;	// In allocation state
			};
			uint32_t next;
		};

		/// Type -> Buffer
		struct __buffer_t
		{
			/// Constructors
			pf_decl_inline pf_decl_constexpr
			__buffer_t(
			 size_t __maxElemSize,
			 size_t __elemCount,
			 align_val_t __maxAlign) pf_attr_noexcept
				: next(nullptr)
				, allocHead(nullptr)
				, allocTail(nullptr)
			{
				pf_assert(!__elemCount, "__elemCount is equal to 0!");
				union
				{
					byte_t *as_byte;
					__header_t *as_header;
				};
				as_byte					= &seq[0u];
				this->allocHead = as_header;
				--__elemCount;
				while(__elemCount > 0)
				{
					as_byte								 += __maxElemSize + sizeof(__header_t);
					this->allocHead->offset = distof(this, this->allocHead);
					this->allocHead->next		= diffof(&this->allocHead, &as_header);
					this->allocHead					= as_header;
					--__elemCount;
				}
			}
			__buffer_t(__buffer_t const &) = delete;
			__buffer_t(__buffer_t &&)			 = delete;

			/// Destructor
			pf_decl_inline pf_decl_constexpr ~__buffer_t() pf_attr_noexcept = default;

			/// Operator =
			__buffer_t &
			operator=(__buffer_t const &) = delete;
			__buffer_t &
			operator=(__buffer_t &&) = delete;

			/// Allocate
			pf_hint_nodiscard pf_decl_inline pf_decl_constexpr void *
			__allocate(
			 size_t __size,
			 align_val_t __align,
			 size_t __offset)
			{
				if(pf_unlikely(!this->allocHead)) return nullptr;
				__header_t *p = this->allocHead;
				union
				{
					__header_t *as_header;
					byte_t *as_byte;
				};
				as_header				= this->allocHead;
				as_byte				 += this->allocHead->next;
				this->allocHead = as_header;
				if(pf_unlikely(!this->allocHead)) this->allocTail = nullptr;
				p->next = 0;
				return ++p;
			}

			/// Deallocate
			pf_decl_inline pf_decl_constexpr void
			__deallocate(
			 diff_t __diff,
			 __header_t *__header) pf_attr_noexcept
			{
				union
				{
					__header_t *as_header;
					byte_t *as_byte;
				};
				as_header = __header;
				if(pf_unlikely(!this->allocHead))
				{
					this->allocHead = as_header;
					this->allocTail = as_header;
				}
				else
				{
					__header_t *current = as_header;
					if(current > this->allocTail)
					{
						this->allocTail->next = diffof(this->allocTail, current);
						this->allocTail				= current;
						current->next					= 0;
					}
					else if(current < this->allocHead)
					{
						current->next		= diffof(current, this->allocHead);
						this->allocHead = current;
					}
					else
					{
						as_header = current;
						while(true)
						{
							as_byte -= __diff;
							if(as_byte <= &this->seq[0u] || as_header->next == 0) break;
						}
						__header_t *p = as_header;
						as_header			= current;
						as_byte			 += as_header->next;
						__header_t *n = as_header;
						if(pf_unlikely(p != current)) p->next = distof(p, n);
					}
				}
			}

			/// Data
			__buffer_t *next;
			const size_t count;
			__header_t *allocHead;
			__header_t *allocTail;
			byte_t seq[];
		};

		/// Header
		pf_hint_nodiscard pf_decl_always_inline pf_decl_constexpr __header_t *
		__header_of(
		 void *__ptr) pf_attr_noexcept
		{
			__header_t *h = union_cast<__header_t *>(__ptr);
			return --h;
		}
		pf_hint_nodiscard pf_decl_always_inline pf_decl_constexpr __buffer_t *
		__buffer_of(
		 __header_t *__ptr) pf_attr_noexcept
		{
			union
			{
				__buffer_t *as_buffer;
				__header_t *as_header;
				byte_t *as_byte;
			};
			as_header = __ptr;
			as_byte	 -= as_header->position;
			return as_buffer;
		}

		/// Internal
		pf_decl_inline pf_decl_constexpr void
		__delete_all_buffers() pf_attr_noexcept
		{
			while(this->head_)
			{
				__buffer_t *next = this->head_->next;
				destroy_delete(this->provider_, this->head_);
				this->head_ = next;
			}
		}
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr void *
		__push_buffer_and_allocate(
		 size_t __size,
		 align_val_t __align,
		 size_t __offset)
		{
			const size_t ms = this->magnifier_(this->tail_->count);
			const size_t ss = __size > ms ? this->magnifier_(__size) : ms;
			auto buf				= new_construct_ex<__buffer_t>(this->provider_, ss, ss);
			if(pf_unlikely(!this->head_))
			{
				this->head_ = buf;
				this->tail_ = buf;
			}
			else
			{
				this->tail_->next = buf;
				this->tail_				= buf;
			}
			return this->tail_->__allocate(__size, __align, __offset);
		}

	public:
		/// Constructors
		pf_decl_inline pf_decl_constexpr
		allocator_pool(
		 size_t __maxElemSize,
		 size_t __startElemCount,
		 align_val_t __maxAlign,
		 _Magnifier &&__magnifier			= _Magnifier(),
		 _MemoryProvider &&__provider = _MemoryProvider())
			: head_(nullptr)
			, tail_(nullptr)
			, maxElemSize_(__maxElemSize + paddingof(__maxElemSize + sizeof(__header_t), __maxAlign))
			, startElemCount_(__startElemCount)
			, maxElemAlign_(__maxAlign)
			, magnifier_(std::move(__magnifier))
			, provider_(std::move(__provider))
		{
		}
		template<typename _MagnifierR, typename _MemoryProviderR>
		pf_decl_inline pf_decl_constexpr
		allocator_pool(
		 allocator_pool<_MagnifierR, _MemoryProviderR> const &__other,
		 align_val_t __align,
		 _Magnifier &&__magnifier,
		 _MemoryProvider &&__provider = _MemoryProvider())
			: allocator_pool(__other.maxElemSize_, __other.startElemCount_, __align, std::move(__magnifier), std::move(__provider))
		{}
		pf_decl_inline pf_decl_constexpr
		allocator_pool(
		 allocator_pool<_Magnifier, _MemoryProvider> const &__other,
		 align_val_t __align)
			: allocator_pool(__other.maxElemSize_, __other.startElemCount_, __align, __other.magnifier_, __other.provider_)
		{}
		pf_decl_inline pf_decl_constexpr
		allocator_pool(
		 allocator_pool<_Magnifier, _MemoryProvider> const &__other)
			: allocator_pool(__other.maxElemSize_, __other.startElemCount_, __other.maxElemAlign_, __other.magnifier_, __other.provider_)
		{}
		pf_decl_inline pf_decl_constexpr
		allocator_pool(
		 allocator_pool<_Magnifier, _MemoryProvider> &&__other)
			: head_(__other.head_)
			, tail_(__other.tail_)
			, maxElemSize_(__other.maxElemSize_)
			, startElemCount_(__other.startElemCount_)
			, maxElemAlign_(__other.maxElemAlign_)
			, magnifier_(std::move(__other.magnifier_))
			, provider_(std::move(__other.provider_))
		{
			__other.head_ = nullptr;
			__other.tail_ = nullptr;
		}

		/// Destructor
		pf_decl_inline pf_decl_constexpr ~allocator_pool() pf_attr_noexcept
		{
			this->__delete_all_buffers();
		}

		/// Operator =
		pf_decl_inline pf_decl_constexpr allocator_pool<_Magnifier, _MemoryProvider> &
		operator=(
		 allocator_pool<_Magnifier, _MemoryProvider> &&__other)
		{
			if(pf_likely(this != &__other))
			{
				this->__delete_all_buffers();
				this->head_						= __other.head_;
				this->tail_						= __other.tail_;
				this->maxElemSize_		= __other.maxElemSize_;
				this->startElemCount_ = __other.startElemCount_;
				this->maxElemAlign_		= __other.maxElemAlign_;
				this->magnifier_			= std::move(__other.magnifier_);
				this->provider_				= std::move(__other.provider_);
				__other.head_					= nullptr;
				__other.tail_					= nullptr;
			}
			return *this;
		}

		/// Allocate
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr void *
		allocate(
		 size_t __size,
		 align_val_t __align,
		 size_t __offset)
		{
			pf_assert(__size > this->maxElemSize_, "__size is greater than maximum element size!");
			pf_assert(__align > this->maxElemAlign_, "__align is greater than maximum element alignment!");
			void *p;
			if(pf_unlikely(!this->tail_))
			{
				p = this->__push_buffer_and_allocate(__size, __align, __offset);
			}
			else
			{
				p = this->tail_->__allocate(__size, __align, __offset);
				if(pf_unlikely(!p))
				{
					p = this->__push_buffer_and_allocate(__size, __align, __offset);
				}
			}
			return p;
		}

		/// Deallocate
		pf_decl_inline pf_decl_constexpr void
		deallocate(
		 void *__ptr) pf_attr_noexcept
		{
			union
			{
				__header_t *as_header;
				void *as_void;
			};
			as_void = __ptr;
			--as_header;
			__buffer_t *buf = __buffer_of(as_header);
			buf->__deallocate(this->maxElemSize_ + sizeof(__header_t), as_header);
		}

		/// Reallocate
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr void *
		reallocate(
		 void *__ptr,
		 size_t __size,
		 align_val_t __align,
		 size_t)
		{
			pf_assert(__size > this->maxElemSize_, "__size is greater than maximum element size!");
			pf_assert(__align > this->maxElemAlign_, "__align is greater than maximum element alignment!");
			return __ptr;	 // DOES NOTHING, AS SIZE CAN'T CHANGE!
		}

		/// Purge
		pf_decl_inline pf_decl_constexpr size_t
		purge(
		 void *__ptr)
		{
			__buffer_t *p = nullptr;
			__buffer_t *c = this->head_;
			size_t s			= 0;
			while(c)
			{
				__header_t *n = c->next;
				if(!c->allocHead && c->count != this->startElemCount_)
				{
					s += c->count * (this->maxElemSize_ + sizeof(__header_t));
					destroy_delete(this->provider_, c);
				}
				else
				{
					if(p) p->next = c;
					p = c;
				}
				c = n;
			}
			return c;
		}

	private:
		/// Data
		__buffer_t *head_;
		__buffer_t *tail_;
		size_t maxElemSize_;
		size_t startElemCount_;
		align_val_t maxElemAlign_;
		pf_hint_nounique_address _Magnifier magnifier_;
		pf_hint_nounique_address _MemoryProvider provider_;
	};

}	 // namespace pul

#endif	// !PULSAR_ALLOCATOR_HPP
