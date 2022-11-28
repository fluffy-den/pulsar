/*! @file   shared_ptr.hpp
 *  @author Fluffy (noe.louis-quentin@hotmail.fr)
 *  @brief
 *  @date   11-09-2022
 *
 *  @copyright Copyright (c) 2022 - Pulsar Software
 *
 *  @since 0.1.1
 */

#ifndef PULSAR_SHARED_PTR_HPP
#define PULSAR_SHARED_PTR_HPP 1

// Include: Pulsar
#include "pulsar/memory.hpp"
#include "pulsar/utility.hpp"

// Pulsar
namespace pul
{

	/// SHARED: Store
	template <typename _Ty>
	struct shared_store
	{
		/// Constructor
		template <typename ..._Args>
		pf_decl_constexpr
		shared_store(
			_Args && ... __args) pf_attr_noexcept
		requires(std::is_constructible_v<_Ty, _Args ...>)
			: shared(1)
			, store(std::forward<_Args>(__args)...)
		{}
		shared_store(shared_store<_Ty> const &) = delete;
		shared_store(shared_store<_Ty> &&)			= delete;

		/// Destructor
		pf_decl_constexpr
		~shared_store() pf_attr_noexcept
		{
			pf_assert(shared == 0, u8"Shared count isn't equal to 0!");
		}

		/// Store
		size_t shared;
		_Ty store;
	};

	/// SHARED: Store -> Array
	template <typename _Ty>
	struct shared_store<_Ty[]>
	{
		/// Constructor
		template <typename ..._Args>
		pf_decl_constexpr
		shared_store(
			size_t __count,
			_Args && ... __args) pf_attr_noexcept
		requires((sizeof...(_Args) == 0 && std::is_default_constructible_v<_Ty>)
						 || std::is_copy_constructible_v<_Ty>)
			: shared(1)
		{
			construct_array_at(
				&this->array,
				__count,
				std::forward<_Args>(__args)...);
		}
		shared_store(shared_store<_Ty[]> const &) = delete;
		shared_store(shared_store<_Ty[]> &&)			= delete;

		/// Destructor
		pf_decl_constexpr
		~shared_store() pf_attr_noexcept
		{
			pf_assert(shared == 0, u8"Shared count isn't equal to 0!");
			destroy_array_at(&this->array);
		}

		/// Store
		size_t shared;
		memory_array_t<_Ty> array;
	};

	/// SHARED: Ptr
	template <typename _Ty, typename _Deleter>
	class shared_ptr
	{
		/// Delete
		pf_decl_constexpr void
		__increase_share_count() pf_attr_noexcept
		{
			if(this->ptr_) ++this->ptr_->shared;
		}
		pf_decl_constexpr void
		__delete_no_check() pf_attr_noexcept
		{
			if(--this->ptr_->shared == 0) this->del_(this->ptr_);
		}

	public:
		/// Constructors
		pf_decl_constexpr
		shared_ptr(
			shared_store<_Ty> *__ptr,
			_Deleter &&__deleter) pf_attr_noexcept
			: ptr_(__ptr)
			, del_(std::move(__deleter))
		{}
		pf_decl_constexpr
		shared_ptr(
			nullptr_t,
			_Deleter &&__deleter) pf_attr_noexcept
			: shared_ptr(nullptr, std::move(__deleter))
		{}
		pf_decl_constexpr
		shared_ptr(
			_Deleter &&__deleter) pf_attr_noexcept
			: shared_ptr(nullptr, std::move(__deleter))
		{}
		pf_decl_constexpr
		shared_ptr(
			shared_ptr<_Ty, _Deleter> const &__r) pf_attr_noexcept
			: shared_ptr(__r.ptr_, __r.del_)
		{
			this->__increase_share_count();
		}
		pf_decl_constexpr
		shared_ptr(
			shared_ptr<_Ty, _Deleter> &&__r) pf_attr_noexcept
			: shared_ptr(__r.ptr_, std::move(__r.del_))
		{
			this->ptr_ = __r;
			__r.ptr_	 = nullptr;
		}

		/// Destructor
		pf_decl_constexpr
		~shared_ptr() pf_attr_noexcept
		{
			this->reset();
		}

		/// Operator=
		pf_decl_constexpr shared_ptr<_Ty, _Deleter> &
		operator =(
			shared_ptr<_Ty, _Deleter> const &__r) pf_attr_noexcept
		{
			if(this != &__r)
			{
				this->set(__r.ptr, __r.del_);
			}
			return *this;
		}
		pf_decl_constexpr shared_ptr<_Ty, _Deleter> &
		operator =(
			shared_ptr<_Ty, _Deleter> &&__r) pf_attr_noexcept
		{
			if(this != &__r)
			{
				if(this->ptr_) this->__delete_no_check();
				this->ptr_ = __r;
				this->del_ = std::move(__r.del_);
				__r.ptr_	 = nullptr;
			}
			return *this;
		}
		pf_decl_constexpr shared_ptr<_Ty, _Deleter> &
		operator =(
			shared_store<_Ty> *__s) pf_attr_noexcept
		{
			if(this != &__s)
			{
				this->set(__s);
			}
			return *this;
		}

		/// Operator <=> / ==
		pf_decl_constexpr bool
		operator ==(
			const _Ty *__ptr) const pf_attr_noexcept
		{
			return this->ptr_ == __ptr;
		}
		pf_decl_constexpr std::strong_ordering
		operator <=>(
			const _Ty *__ptr) const pf_attr_noexcept
		{
			return this->ptr_ <=> __ptr;
		}

		/// Operator*
		pf_decl_constexpr _Ty &
		operator *() pf_attr_noexcept
		{
			return *this->get();
		}
		pf_decl_constexpr const _Ty &
		operator *() const pf_attr_noexcept
		{
			return *this->get();
		}

		/// Operator->
		pf_decl_constexpr _Ty *
		operator ->() pf_attr_noexcept
		{
			return this->get();
		}
		pf_decl_constexpr const _Ty *
		operator ->() const pf_attr_noexcept
		{
			return this->get();
		}

		/// Operator _Ty *
		pf_decl_constexpr
		operator _Ty *() pf_attr_noexcept
		{
			return this->get();
		}
		pf_decl_constexpr
		operator const _Ty *() const pf_attr_noexcept
		{
			return this->get();
		}

		/// Operator bool
		pf_decl_explicit pf_decl_constexpr
		operator bool() const pf_attr_noexcept
		{
			return this->get();
		}

		/// Get
		pf_decl_constexpr _Ty *
		get() pf_attr_noexcept
		{
			return &this->ptr_->first;
		}
		pf_decl_constexpr const _Ty *
		get() const pf_attr_noexcept
		{
			return &this->ptr_->first;
		}

		/// Set
		pf_decl_constexpr void
		set(
			shared_store<_Ty> *__r,
			_Deleter &&__del = _Deleter()) pf_attr_noexcept
		{
			if(this->ptr_ && (this->ptr_ != __r))
			{
				this->__delete_no_check();
				this->ptr_ = __r;
				this->del_ = std::move(__del);
				this->__increase_share_count();
			}
		}

		/// Reset
		pf_decl_constexpr void
		reset() pf_attr_noexcept
		{
			if(this->ptr_)
			{
				this->__delete_no_check();
				this->ptr_ = nullptr;
			}
		}

	private:
		/// Store
		shared_store<_Ty> *ptr_;
		_Deleter del_;
	};

	/// SHARED: Ptr -> Array
	template <typename _Ty, typename _Deleter>
	class shared_ptr<_Ty[], _Deleter>
	{
		/// Delete
		pf_decl_constexpr void
		__increase_share_count() pf_attr_noexcept
		{
			if(this->ptr_) ++this->ptr_->shared;
		}
		pf_decl_constexpr void
		__delete_no_check() pf_attr_noexcept
		{
			if(--this->ptr_->shared == 0) this->del_(this->ptr_);
		}

	public:
		/// Constructors
		pf_decl_constexpr
		shared_ptr(
			shared_store<_Ty[]> *__ptr,
			_Deleter &&__deleter) pf_attr_noexcept
			: ptr_(__ptr)
			, del_(std::move(__deleter))
		{}
		pf_decl_constexpr
		shared_ptr(
			nullptr_t,
			_Deleter &&__deleter) pf_attr_noexcept
			: shared_ptr(nullptr, std::move(__deleter))
		{}
		pf_decl_constexpr
		shared_ptr(
			_Deleter &&__deleter) pf_attr_noexcept
			: shared_ptr(nullptr, std::move(__deleter))
		{}
		pf_decl_constexpr
		shared_ptr(
			shared_ptr<_Ty[], _Deleter> const &__r) pf_attr_noexcept
			: shared_ptr(__r.ptr_, __r.del_)
		{
			this->__increase_share_count();
		}
		pf_decl_constexpr
		shared_ptr(
			shared_ptr<_Ty[], _Deleter> &&__r) pf_attr_noexcept
			: shared_ptr(__r.ptr_, std::move(__r.del_))
		{
			this->ptr_ = __r;
			__r.ptr_	 = nullptr;
		}

		/// Destructor
		pf_decl_constexpr
		~shared_ptr() pf_attr_noexcept
		{
			this->reset();
		}

		/// Operator=
		pf_decl_constexpr shared_ptr<_Ty[], _Deleter> &
		operator =(
			shared_ptr<_Ty[], _Deleter> const &__r) pf_attr_noexcept
		{
			if(this != &__r)
			{
				this->set(__r.ptr_, __r.del_);
			}
			return *this;
		}
		pf_decl_constexpr shared_ptr<_Ty[], _Deleter> &
		operator =(
			shared_ptr<_Ty[], _Deleter> &&__r) pf_attr_noexcept
		{
			if(this != &__r)
			{
				if(this->ptr_) this->__delete_no_check();
				this->ptr_ = __r;
				this->del_ = std::move(__r.del_);
				__r.ptr_	 = nullptr;
			}
			return *this;
		}
		pf_decl_constexpr shared_ptr<_Ty[], _Deleter> &
		operator =(
			shared_store<_Ty[]> *__s) pf_attr_noexcept
		{
			if(this != &__s)
			{
				this->set(__s);
			}
			return *this;
		}

		/// Operator <=> / ==
		pf_decl_constexpr bool
		operator ==(
			const _Ty *__ptr) const pf_attr_noexcept
		{
			return this->ptr_ == __ptr;
		}
		pf_decl_constexpr std::strong_ordering
		operator <=>(
			const _Ty *__ptr) const pf_attr_noexcept
		{
			return this->ptr_ <=> __ptr;
		}

		/// Operator[]
		pf_decl_constexpr _Ty &
		operator [](
			size_t __index) pf_attr_noexcept
		{
			return this->get()[__index];
		}
		pf_decl_constexpr const _Ty &
		operator [](
			size_t __index) const pf_attr_noexcept
		{
			return this->get()[__index];
		}

		/// Operator _Ty *
		pf_decl_constexpr
		operator _Ty *() pf_attr_noexcept
		{
			return this->get();
		}
		pf_decl_constexpr
		operator const _Ty *() const pf_attr_noexcept
		{
			return this->get();
		}

		/// Operator bool
		pf_decl_explicit pf_decl_constexpr
		operator bool() const pf_attr_noexcept
		{
			return this->get();
		}

		/// Get
		pf_decl_constexpr _Ty *
		get() pf_attr_noexcept
		{
			return &this->ptr_->first;
		}
		pf_decl_constexpr const _Ty *
		get() const pf_attr_noexcept
		{
			return &this->ptr_->first;
		}

		/// Set
		pf_decl_constexpr void
		set(
			shared_store<_Ty[]> *__r,
			_Deleter &&__del = _Deleter()) pf_attr_noexcept
		{
			if(this->ptr_ && (this->ptr_ != __r))
			{
				this->__delete_no_check();
				this->ptr_ = __r;
				this->del_ = std::move(__del);
				this->__increase_share_count();
			}
		}

		/// Reset
		pf_decl_constexpr void
		reset() pf_attr_noexcept
		{
			if(this->ptr_)
			{
				this->__delete_no_check();
				this->ptr_ = nullptr;
			}
		}

	private:
		/// Store
		shared_store<_Ty[]> *ptr_;
		_Deleter del_;
	};

	/// SHARED: Ptr -> Empty Deleter
	template <typename _Ty, typename _Deleter>
	requires(std::is_empty_v<_Deleter>)
	class shared_ptr<_Ty, _Deleter>
	{
		/// Delete
		pf_decl_constexpr void
		__increase_share_count() pf_attr_noexcept
		{
			if(this->ptr_) ++this->ptr_->shared;
		}
		pf_decl_constexpr void
		__delete_no_check() pf_attr_noexcept
		{
			if(--this->ptr_->shared == 0)
			{
				_Deleter del;
				del(this->ptr_);
			}
		}

	public:
		/// Constructors
		pf_decl_constexpr
		shared_ptr(
			shared_store<_Ty> *__ptr,
			_Deleter &&__deleter) pf_attr_noexcept
			: ptr_(__ptr)
		{
			ignore = __deleter;
		}
		pf_decl_constexpr
		shared_ptr(
			nullptr_t,
			_Deleter &&__deleter) pf_attr_noexcept
			: shared_ptr(nullptr, std::move(__deleter))
		{}
		pf_decl_constexpr
		shared_ptr(
			_Deleter &&__deleter) pf_attr_noexcept
			: shared_ptr(nullptr, std::move(__deleter))
		{}
		pf_decl_constexpr
		shared_ptr(
			shared_ptr<_Ty, _Deleter> const &__r) pf_attr_noexcept
			: shared_ptr(__r.ptr_, _Deleter())
		{
			this->__increase_share_count();
		}
		pf_decl_constexpr
		shared_ptr(
			shared_ptr<_Ty, _Deleter> &&__r) pf_attr_noexcept
			: shared_ptr(__r.ptr_, _Deleter())
		{
			this->ptr_ = __r;
			__r.ptr_	 = nullptr;
		}

		/// Destructor
		pf_decl_constexpr
		~shared_ptr() pf_attr_noexcept
		{
			this->reset();
		}

		/// Operator=
		pf_decl_constexpr shared_ptr<_Ty, _Deleter> &
		operator =(
			shared_ptr<_Ty, _Deleter> const &__r) pf_attr_noexcept
		{
			if(this != &__r)
			{
				this->set(__r.ptr_);
			}
			return *this;
		}
		pf_decl_constexpr shared_ptr<_Ty, _Deleter> &
		operator =(
			shared_ptr<_Ty, _Deleter> &&__r) pf_attr_noexcept
		{
			if(this != &__r)
			{
				if(this->ptr_) this->__delete_no_check();
				this->ptr_ = __r;
				__r.ptr_	 = nullptr;
			}
			return *this;
		}
		pf_decl_constexpr shared_ptr<_Ty, _Deleter> &
		operator =(
			shared_store<_Ty> *__s) pf_attr_noexcept
		{
			if(this != &__s)
			{
				this->set(__s);
			}
			return *this;
		}

		/// Operator <=> / ==
		pf_decl_constexpr bool
		operator ==(
			const _Ty *__ptr) const pf_attr_noexcept
		{
			return this->ptr_ == __ptr;
		}
		pf_decl_constexpr std::strong_ordering
		operator <=>(
			const _Ty *__ptr) const pf_attr_noexcept
		{
			return this->ptr_ <=> __ptr;
		}

		/// Operator*
		pf_decl_constexpr _Ty &
		operator *() pf_attr_noexcept
		{
			return *this->get();
		}
		pf_decl_constexpr const _Ty &
		operator *() const pf_attr_noexcept
		{
			return *this->get();
		}

		/// Operator->
		pf_decl_constexpr _Ty *
		operator ->() pf_attr_noexcept
		{
			return this->get();
		}
		pf_decl_constexpr const _Ty *
		operator ->() const pf_attr_noexcept
		{
			return this->get();
		}

		/// Operator _Ty *
		pf_decl_constexpr
		operator _Ty *() pf_attr_noexcept
		{
			return this->get();
		}
		pf_decl_constexpr
		operator const _Ty *() const pf_attr_noexcept
		{
			return this->get();
		}

		/// Operator bool
		pf_decl_explicit pf_decl_constexpr
		operator bool() const pf_attr_noexcept
		{
			return this->get();
		}

		/// Get
		pf_decl_constexpr _Ty *
		get() pf_attr_noexcept
		{
			return &this->ptr_->first;
		}
		pf_decl_constexpr const _Ty *
		get() const pf_attr_noexcept
		{
			return &this->ptr_->first;
		}

		/// Set
		pf_decl_constexpr void
		set(
			shared_store<_Ty> *__r,
			_Deleter &&__deleter = _Deleter()) pf_attr_noexcept
		{
			if(this->ptr_ && (this->ptr_ != __r))
			{
				this->__delete_no_check();
				this->ptr_ = __r;
				ignore		 = __deleter;
				this->__increase_share_count();
			}
		}

		/// Reset
		pf_decl_constexpr void
		reset() pf_attr_noexcept
		{
			if(this->ptr_)
			{
				this->__delete_no_check();
				this->ptr_ = nullptr;
			}
		}

	private:
		/// Store
		shared_store<_Ty> *ptr_;
	};

	/// SHARED: Ptr -> Array with Empty Deleter
	template <typename _Ty, typename _Deleter>
	requires(std::is_empty_v<_Deleter>)
	class shared_ptr<_Ty[], _Deleter>
	{
		/// Delete
		pf_decl_constexpr void
		__increase_share_count() pf_attr_noexcept
		{
			if(this->ptr_) ++this->ptr_->shared;
		}
		pf_decl_constexpr void
		__delete_no_check() pf_attr_noexcept
		{
			if(--this->ptr_->shared == 0)
			{
				_Deleter del;
				del(this->ptr_);
			}
		}

	public:
		/// Constructors
		pf_decl_constexpr
		shared_ptr(
			shared_store<_Ty[]> *__ptr,
			_Deleter &&__deleter) pf_attr_noexcept
			: ptr_(__ptr)
		{
			ignore = __deleter;
		}
		pf_decl_constexpr
		shared_ptr(
			nullptr_t,
			_Deleter &&__deleter) pf_attr_noexcept
			: shared_ptr(nullptr, std::move(__deleter))
		{}
		pf_decl_constexpr
		shared_ptr(
			_Deleter &&__deleter) pf_attr_noexcept
			: shared_ptr(nullptr, std::move(__deleter))
		{}
		pf_decl_constexpr
		shared_ptr(
			shared_ptr<_Ty[], _Deleter> const &__r) pf_attr_noexcept
			: shared_ptr(__r.ptr_, _Deleter())
		{
			this->__increase_share_count();
		}
		pf_decl_constexpr
		shared_ptr(
			shared_ptr<_Ty[], _Deleter> &&__r) pf_attr_noexcept
			: shared_ptr(__r.ptr_, _Deleter())
		{
			this->ptr_ = __r;
			__r.ptr_	 = nullptr;
		}

		/// Destructor
		pf_decl_constexpr
		~shared_ptr() pf_attr_noexcept
		{
			this->reset();
		}

		/// Operator=
		pf_decl_constexpr shared_ptr<_Ty[], _Deleter> &
		operator =(
			shared_ptr<_Ty[], _Deleter> const &__r) pf_attr_noexcept
		{
			if(this != &__r)
			{
				this->set(__r.ptr_);
			}
			return *this;
		}
		pf_decl_constexpr shared_ptr<_Ty[], _Deleter> &
		operator =(
			shared_ptr<_Ty[], _Deleter> &&__r) pf_attr_noexcept
		{
			if(this != &__r)
			{
				if(this->ptr_) this->__delete_no_check();
				this->ptr_ = __r;
				this->del_ = std::move(__r.del_);
				__r.ptr_	 = nullptr;
			}
			return *this;
		}
		pf_decl_constexpr shared_ptr<_Ty[], _Deleter> &
		operator =(
			shared_store<_Ty[]> *__s) pf_attr_noexcept
		{
			if(this != &__s)
			{
				this->set(__s);
			}
			return *this;
		}

		/// Operator <=> / ==
		pf_decl_constexpr bool
		operator ==(
			const _Ty *__ptr) const pf_attr_noexcept
		{
			return this->ptr_ == __ptr;
		}
		pf_decl_constexpr std::strong_ordering
		operator <=>(
			const _Ty *__ptr) const pf_attr_noexcept
		{
			return this->ptr_ <=> __ptr;
		}

		/// Operator[]
		pf_decl_constexpr _Ty &
		operator [](
			size_t __index) pf_attr_noexcept
		{
			return this->get()[__index];
		}
		pf_decl_constexpr const _Ty &
		operator [](
			size_t __index) const pf_attr_noexcept
		{
			return this->get()[__index];
		}

		/// Operator _Ty *
		pf_decl_constexpr
		operator _Ty *() pf_attr_noexcept
		{
			return this->get();
		}
		pf_decl_constexpr
		operator const _Ty *() const pf_attr_noexcept
		{
			return this->get();
		}

		/// Operator bool
		pf_decl_explicit pf_decl_constexpr
		operator bool() const pf_attr_noexcept
		{
			return this->get();
		}

		/// Get
		pf_decl_constexpr _Ty *
		get() pf_attr_noexcept
		{
			return &this->ptr_->first;
		}
		pf_decl_constexpr const _Ty *
		get() const pf_attr_noexcept
		{
			return &this->ptr_->first;
		}

		/// Set
		pf_decl_constexpr void
		set(
			shared_store<_Ty[]> *__r,
			_Deleter &&__del = _Deleter()) pf_attr_noexcept
		{
			if(this->ptr_ && (this->ptr_ != __r))
			{
				this->__delete_no_check();
				this->ptr_ = __r;
				ignore		 = __del;
				this->__increase_share_count();
			}
		}

		/// Reset
		pf_decl_constexpr void
		reset() pf_attr_noexcept
		{
			if(this->ptr_)
			{
				this->__delete_no_check();
				this->ptr_ = nullptr;
			}
		}
	private:
		/// Store
		shared_store<_Ty[]> *ptr_;
	};

	/// SHARED: Maker
	template <typename _Ty, typename ..._Args>
	pf_decl_constexpr shared_ptr<_Ty, deleter_default<shared_store<_Ty>>>
	make_shared(
		_Args && ... __args) pf_attr_noexcept
	requires(!std::is_array_v<_Ty>)
	{
		return shared_ptr(
			construct_new<shared_store<_Ty>>(
				std::forward<_Args>(__args)...),
			deleter_default<shared_store<_Ty>>());
	}
	template <typename _Ty, typename ..._Args>
	pf_decl_constexpr shared_ptr<_Ty, deleter_default<shared_store<_Ty>>>
	make_shared_aligned(
		align_val_t __align,
		_Args && ... __args) pf_attr_noexcept
	requires(!std::is_array_v<_Ty>)
	{
		return shared_ptr(
			construct_new_aligned<shared_store<_Ty>>(
				__align,
				std::forward<_Args>(__args)...),
			deleter_default<shared_store<_Ty>>());
	}
	template <typename _Ty, typename ..._Args>
	pf_decl_constexpr shared_ptr<_Ty, deleter_default<shared_store<_Ty>>>
	make_shared_aligned_at(
		align_val_t __align,
		size_t __offset,
		_Args && ... __args) pf_attr_noexcept
	requires(!std::is_array_v<_Ty>)
	{
		return shared_ptr(
			construct_new_aligned_at<shared_store<_Ty>>(
				__align,
				__offset,
				std::forward<_Args>(__args)...),
			deleter_default<shared_store<_Ty>>());
	}

	/// SHARED: Maker -> Array
	template <typename _Ty, typename ..._Args>
	pf_decl_constexpr shared_ptr<_Ty, deleter_default<shared_store<_Ty>>>
	make_shared(
		size_t __count,
		_Args && ... __args) pf_attr_noexcept
	requires(std::is_array_v<_Ty>)
	{
		return shared_ptr(
			construct_new_ex<shared_store<_Ty>>(
				(__count - 1) * sizeof(std::remove_all_extents_t<_Ty>),
				__count,
				std::forward<_Args>(__args)...),
			deleter_default<shared_store<_Ty>>());
	}
	template <typename _Ty, typename ..._Args>
	pf_decl_constexpr shared_ptr<_Ty, deleter_default<shared_store<_Ty>>>
	make_shared_aligned(
		size_t __count,
		align_val_t __align,
		_Args && ... __args) pf_attr_noexcept
	requires(std::is_array_v<_Ty>)
	{
		return shared_ptr(
			construct_new_aligned_ex<shared_store<_Ty>>(
				(__count - 1) * sizeof(std::remove_all_extents_t<_Ty>),
				__align,
				__count,
				std::forward<_Args>(__args)...),
			deleter_default<shared_store<_Ty>>());
	}

	/// SHARED: Maker -> Allocator
	template <typename _Ty, typename _Allocator, typename ..._Args>
	pf_decl_constexpr shared_ptr<_Ty, deleter_allocator<shared_store<_Ty>, _Allocator>>
	make_shared(
		_Allocator &__all,
		_Args && ... __args) pf_attr_noexcept
	requires(is_allocator_v<_Allocator> &&
					 !std::is_array_v<_Ty>)
	{
		return shared_ptr(
			construct_new<shared_store<_Ty>>(
				std::forward<_Args>(__args)...),
			deleter_allocator<shared_store<_Ty>, _Allocator>(&__all));
	}
	template <typename _Ty, typename _Allocator, typename ..._Args>
	pf_decl_constexpr shared_ptr<_Ty, deleter_allocator<shared_store<_Ty>, _Allocator>>
	make_shared_aligned(
		_Allocator &__all,
		align_val_t __align,
		_Args && ... __args) pf_attr_noexcept
	requires(is_allocator_v<_Allocator> &&
					 !std::is_array_v<_Ty>)
	{
		return shared_ptr(
			construct_new_aligned<shared_store<_Ty>>(
				__align,
				std::forward<_Args>(__args)...),
			deleter_allocator<shared_store<_Ty>, _Allocator>(&__all));
	}
	template <typename _Ty, typename _Allocator, typename ..._Args>
	pf_decl_constexpr shared_ptr<_Ty, deleter_allocator<shared_store<_Ty>, _Allocator>>
	make_shared_aligned_at(
		_Allocator &__all,
		align_val_t __align,
		size_t __offset,
		_Args && ... __args) pf_attr_noexcept
	requires(is_allocator_v<_Allocator> &&
					 !std::is_array_v<_Ty>)
	{
		return shared_ptr(
			construct_new_aligned_at<shared_store<_Ty>>(
				__align,
				__offset,
				std::forward<_Args>(__args)...),
			deleter_allocator<shared_store<_Ty>, _Allocator>(&__all));
	}

	/// SHARED: Maker -> Allocator -> Array
	template <typename _Ty, typename _Allocator, typename ..._Args>
	pf_decl_constexpr shared_ptr<_Ty, deleter_allocator<shared_store<_Ty>, _Allocator>>
	make_shared(
		_Allocator &__all,
		size_t __count,
		_Args && ... __args) pf_attr_noexcept
	requires(is_allocator_v<_Allocator> &&
					 std::is_array_v<_Ty>)
	{
		return shared_ptr(
			construct_new_ex<shared_store<_Ty>>(
				(__count - 1) * sizeof(std::remove_all_extents_t<_Ty>),
				__count,
				std::forward<_Args>(__args)...),
			deleter_allocator<shared_store<_Ty>, _Allocator>(&__all));
	}
	template <typename _Ty, typename _Allocator, typename ..._Args>
	pf_decl_constexpr shared_ptr<_Ty, deleter_allocator<shared_store<_Ty>, _Allocator>>
	make_shared_aligned(
		_Allocator &__all,
		size_t __count,
		align_val_t __align,
		_Args && ... __args) pf_attr_noexcept
	requires(is_allocator_v<_Allocator> &&
					 std::is_array_v<_Ty>)
	{
		return shared_ptr(
			construct_new_aligned_ex<shared_store<_Ty>>(
				(__count - 1) * sizeof(std::remove_all_extents_t<_Ty>),
				__align,
				__count,
				std::forward<_Args>(__args)...),
			deleter_allocator<shared_store<_Ty>, _Allocator>(&__all));
	}
}

#endif // !PULSAR_SHARED_PTR_HPP