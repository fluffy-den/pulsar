/*! @file   unique_ptr.hpp
 *  @author Fluffy (noe.louis-quentin@hotmail.fr)
 *  @brief
 *  @date   11-09-2022
 *
 *  @copyright Copyright (c) 2022 - Pulsar Software
 *
 *  @since 0.1.1
 */

#ifndef PULSAR_UNIQUE_PTR_HPP
#define PULSAR_UNIQUE_PTR_HPP 1

// Include: Pulsar
#include "pulsar/memory.hpp"
#include "pulsar/utility.hpp"

// Pulsar
namespace pul
{

	/// UNIQUE: Store
	template <typename _Ty>
	struct unique_store
	{
		/// Constructor
		template <typename ..._Args>
		 pf_decl_constexpr
		unique_store(
			_Args && ... __args) pf_attr_noexcept
		requires(std::is_constructible_v<_Ty, _Args ...>)
			: store(std::forward<_Args>(__args)...)
		{}
		unique_store(unique_store<_Ty> const &) = delete;
		unique_store(unique_store<_Ty> &&)			= delete;

		/// Destructor
		 pf_decl_constexpr
		~unique_store() pf_attr_noexcept = default;

		/// Store
		_Ty store;
	};

	/// UNIQUE: Store -> Array
	template <typename _Ty>
	struct unique_store<_Ty[]>
	{
		/// Constructor
		template <typename ..._Args>
		 pf_decl_constexpr
		unique_store(
			size_t __count,
			_Args && ... __args) pf_attr_noexcept
		requires((sizeof...(_Args) == 0 && std::is_default_constructible_v<_Ty>)
             || std::is_copy_constructible_v<_Ty>)
		{
			construct_array_at(
				&this->store,
				__count,
				std::forward<_Args>(__args)...);
		}
		unique_store(unique_store<_Ty[]> const &) = delete;
		unique_store(unique_store<_Ty[]> &&)			= delete;

		/// Destructor
		pf_decl_constexpr
		~unique_store() pf_attr_noexcept
		{
			destroy_array_at(&this->store);
		}

    /// Store
    memory_array_t<_Ty> store;
  };

	/// UNIQUE: Ptr
	template <typename _Ty, typename _Deleter>
	class unique_ptr
	{
    /// Delete
    pf_decl_constexpr void 
    __delete_no_check() pf_attr_noexcept
    {
      this->del_(this->ptr_);
    }

	public:
    /// Constructors
    pf_decl_constexpr  
    unique_ptr(
      unique_store<_Ty> *__ptr,
      _Deleter&& __deleter
    ) pf_attr_noexcept
      : ptr_(__ptr)
      , del_(std::move(__deleter))
    {}
    pf_decl_constexpr  
    unique_ptr(
      nullptr_t,
      _Deleter&& __deleter
    ) pf_attr_noexcept
      : unique_ptr(nullptr, std::move(__deleter))
    {}
    pf_decl_constexpr  
    unique_ptr(
      _Deleter&& __deleter 
    ) pf_attr_noexcept
      : unique_ptr(nullptr, std::move(__deleter))
    {}
    pf_decl_constexpr  
    unique_ptr(
      unique_ptr<_Ty, _Deleter> const &__r
    ) pf_attr_noexcept = delete;
    pf_decl_constexpr  
    unique_ptr(
      unique_ptr<_Ty, _Deleter> &&__r 
    ) pf_attr_noexcept
      : unique_ptr(__r.ptr_, std::move(__r.del_))
    {
      __r.ptr_ = nullptr;
    }

    /// Destructor
    pf_decl_constexpr  
    ~unique_ptr() pf_attr_noexcept
    {
      this->reset();
    }

    /// Operator=
    pf_decl_constexpr  unique_ptr<_Ty, _Deleter> &
    operator=(
      unique_ptr<_Ty, _Deleter> const& 
    ) pf_attr_noexcept = delete;
    pf_decl_constexpr  unique_ptr<_Ty, _Deleter> &
    operator=(
      unique_ptr<_Ty, _Deleter> &&__r 
    ) pf_attr_noexcept
    {
      if (this != &__r)
      {
        pf_assert(this->ptr_ != __r.ptr_, u8"Both unique ptr are equals!"); 
        this->set(__r.ptr_);
        __r.ptr_ = nullptr;
        __r.del_ = std::move(__r.del_);
      }
      return *this;
    }
    pf_decl_constexpr  unique_ptr<_Ty, _Deleter> &
    operator=(
      unique_store<_Ty> *__r 
    ) pf_attr_noexcept
    {
      if (this->ptr_ != __r)
      {
        this->set(__r);
      }
      return *this;
    }

    /// Operator<=> / ==
    pf_decl_constexpr  bool 
    operator==(
      const _Ty *__r 
    ) pf_attr_noexcept
    {
      return (this->ptr_ == __r);
    }
    pf_decl_constexpr  std::strong_ordering 
    operator<=>(
      const _Ty *__r 
    ) pf_attr_noexcept
    {
      return (this->ptr_ <=> __r);
    }

    /// Operator*
    pf_decl_constexpr  _Ty &
    operator*() pf_attr_noexcept
    {
      return *this->get();
    }
    pf_decl_constexpr  const _Ty &
    operator*() const pf_attr_noexcept
    {
      return *this->get();
    }

    /// Operator->
    pf_decl_constexpr  _Ty*
    operator->() pf_attr_noexcept
    {
      return this->get();
    }
    pf_decl_constexpr  const _Ty*
    operator->() const pf_attr_noexcept
    {
      return this->get();
    }

    /// Operator _Ty*
    pf_decl_constexpr 
    operator _Ty*() pf_attr_noexcept
    {
      return this->get();
    }
    pf_decl_constexpr 
    operator const _Ty*() const pf_attr_noexcept
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
    pf_decl_constexpr  _Ty *
    get() pf_attr_noexcept
    {
      return &this->ptr_->store;
    }
    pf_decl_constexpr  const _Ty *
    get() const pf_attr_noexcept
    {
      return &this->ptr_->store;
    }
  
    /// Set
    pf_decl_constexpr  void
    set(
      unique_store<_Ty> *__r,
      _Deleter &&__deleter = _Deleter()) pf_attr_noexcept
    {
      if (this->ptr_) this->__delete_no_check();
      this->ptr_ = __r;
      this->del_ = std::move(__deleter);
    }

    /// Reset
    pf_decl_constexpr  void 
    reset() pf_attr_noexcept
    {
      if (this->ptr_)
      {
        this->__delete_no_check();
        this->ptr_ = nullptr;
      }
    }

	private:
		/// Store
		unique_store<_Ty> *ptr_;
    _Deleter del_;
	};

	/// UNIQUE: Ptr -> Array
  template <typename _Ty, typename _Deleter>
  class unique_ptr<_Ty[], _Deleter>
  {
    /// Delete
    pf_decl_constexpr void __delete_no_check() pf_attr_noexcept
    {
      this->del_(this->ptr_);
    }

  public:
    /// Constructors
    pf_decl_constexpr  
    unique_ptr(
      unique_store<_Ty[]> *__ptr,
      _Deleter&& __deleter
    ) pf_attr_noexcept
      : ptr_(__ptr)
      , del_(std::move(__deleter))
    {}
    pf_decl_constexpr  
    unique_ptr(
      nullptr_t,
      _Deleter&& __deleter
    ) pf_attr_noexcept
      : unique_ptr(nullptr, std::move(__deleter))
    {}
    pf_decl_constexpr  
    unique_ptr(
      _Deleter&& __deleter 
    ) pf_attr_noexcept
      : unique_ptr(nullptr, std::move(__deleter))
    {}
    pf_decl_constexpr  
    unique_ptr(
      unique_ptr<_Ty[], _Deleter> const &__r
    ) pf_attr_noexcept = delete;
    pf_decl_constexpr  
    unique_ptr(
      unique_ptr<_Ty[], _Deleter> &&__r 
    ) pf_attr_noexcept
      : unique_ptr(__r.ptr_, std::move(__r.del_))
    {
      __r.ptr_ = nullptr;
    }

    /// Destructor
    pf_decl_constexpr  
    ~unique_ptr() pf_attr_noexcept
    {
      this->reset();
    }

    /// Operator=
    pf_decl_constexpr  unique_ptr<_Ty[], _Deleter> &
    operator=(
      unique_ptr<_Ty[], _Deleter> const& 
    ) pf_attr_noexcept = delete;
    pf_decl_constexpr  unique_ptr<_Ty[], _Deleter> &
    operator=(
      unique_ptr<_Ty[], _Deleter> &&__r 
    ) pf_attr_noexcept
    {
      if (this != &__r)
      {
        pf_assert(this->ptr_ != __r.ptr_, u8"Both unique ptr are equals!"); 
        this->set(__r.ptr_);
        this->del_ = std::move(__r.del_);
        __r.ptr_ = nullptr;
      }
      return *this;
    }
    pf_decl_constexpr  unique_ptr<_Ty[], _Deleter> &
    operator=(
      unique_store<_Ty[]> *__s
    ) pf_attr_noexcept
    {
      if (this->ptr_ != __s)
      {
        this->set(__s);
      }
      return *this;
    }

    /// Operator<=> / ==
    pf_decl_constexpr  bool 
    operator==(
      const _Ty *__ptr
    ) const pf_attr_noexcept
    {
      return (this->get() == __ptr);
    }
    pf_decl_constexpr  std::strong_ordering 
    operator<=>(
      const _Ty *__ptr 
    ) const pf_attr_noexcept
    {
      return (this->get() <=> __ptr);
    }

    /// Operator[]
    pf_decl_constexpr _Ty &operator[](
      size_t __index 
    ) pf_attr_noexcept
    {
      return this->get()[__index];
    }
    pf_decl_constexpr const _Ty &operator[](
      size_t __index 
    ) const pf_attr_noexcept
    {
      return this->get()[__index];
    }

    /// Operator _Ty*
    pf_decl_constexpr 
    operator _Ty*() pf_attr_noexcept
    {
      return this->get();
    }
    pf_decl_constexpr 
    operator const _Ty*() const pf_attr_noexcept
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
    pf_decl_constexpr  _Ty *
    get() pf_attr_noexcept
    {
      return &this->ptr_->first;
    }
    pf_decl_constexpr  const _Ty *
    get() const pf_attr_noexcept
    {
      return &this->ptr_->first;
    }
  
    /// Set
    pf_decl_constexpr  void
    set(
      unique_store<_Ty[]> *__r,
      _Deleter &&__deleter = _Deleter()) pf_attr_noexcept{
      if (this->ptr_) this->__delete_no_check();
      this->ptr_ = __r;
      this->del_ = std::move(__deleter);
    }

    /// Reset
    pf_decl_constexpr  void 
    reset() pf_attr_noexcept
    {
      if (this->ptr_)
      {
        this->__delete_no_check();
        this->ptr_ = nullptr;
      }
    }

  private:
    /// Store
    unique_store<_Ty[]> *ptr_;
    _Deleter del_;
  };

	/// UNIQUE: Ptr -> Empty Deleter
  template <typename _Ty, typename _Deleter>
  requires(std::is_empty_v<_Deleter>)
  class unique_ptr<_Ty, _Deleter>
  {
    /// Delete
    pf_decl_constexpr void __delete_no_check() pf_attr_noexcept
    {
      _Deleter del;
      del(this->ptr_);
    }

  public:
    /// Constructors
    pf_decl_constexpr  
    unique_ptr(
      unique_store<_Ty> *__ptr,
      _Deleter&& __deleter
    ) pf_attr_noexcept
      : ptr_(__ptr)
    {
      ignore = __deleter;
    }
    pf_decl_constexpr  
    unique_ptr(
      nullptr_t,
      _Deleter&& __deleter
    ) pf_attr_noexcept
      : unique_ptr(nullptr)
    {
      ignore = __deleter;
    }
    pf_decl_constexpr  
    unique_ptr(
      _Deleter&& __deleter 
    ) pf_attr_noexcept
      : unique_ptr(nullptr)
    {
      ignore = __deleter;
    }
    pf_decl_constexpr  
    unique_ptr(
      unique_ptr<_Ty, _Deleter> const &__r
    ) pf_attr_noexcept = delete;
    pf_decl_constexpr  
    unique_ptr(
      unique_ptr<_Ty, _Deleter> &&__r 
    ) pf_attr_noexcept
      : unique_ptr(__r.ptr_, _Deleter())
    {
      __r.ptr_ = nullptr;
    }

    /// Destructor
    pf_decl_constexpr  
    ~unique_ptr() pf_attr_noexcept
    {
      this->reset();
    }

    /// Operator=
    pf_decl_constexpr  unique_ptr<_Ty, _Deleter> &
    operator=(
      unique_ptr<_Ty, _Deleter> const& 
    ) pf_attr_noexcept = delete;
    pf_decl_constexpr  unique_ptr<_Ty, _Deleter> &
    operator=(
      unique_ptr<_Ty, _Deleter> &&__r 
    ) pf_attr_noexcept
    {
      if (this != &__r)
      {
        pf_assert(this->ptr_ != __r.ptr_, u8"Both unique ptr are equals!"); 
        this->set(__r.ptr_);
        __r.ptr_ = nullptr;
      }
      return *this;
    }
    pf_decl_constexpr  unique_ptr<_Ty, _Deleter> &
    operator=(
      unique_store<_Ty> *__r 
    ) pf_attr_noexcept
    {
      if (this->ptr_ != __r)
      {
        this->set(__r);
      }
      return *this;
    }

    /// Operator<=> / ==
    pf_decl_constexpr  bool 
    operator==(
      const _Ty *__r 
    ) pf_attr_noexcept
    {
      return (this->ptr_ == __r);
    }
    pf_decl_constexpr  std::strong_ordering 
    operator<=>(
      const _Ty *__r 
    ) pf_attr_noexcept
    {
      return (this->ptr_ <=> __r);
    }

    /// Operator*
    pf_decl_constexpr  _Ty &
    operator*() pf_attr_noexcept
    {
      return *this->get();
    }
    pf_decl_constexpr  const _Ty &
    operator*() const pf_attr_noexcept
    {
      return *this->get();
    }

    /// Operator->
    pf_decl_constexpr  _Ty*
    operator->() pf_attr_noexcept
    {
      return this->get();
    }
    pf_decl_constexpr  const _Ty*
    operator->() const pf_attr_noexcept
    {
      return this->get();
    }

    /// Operator _Ty*
    pf_decl_constexpr 
    operator _Ty*() pf_attr_noexcept
    {
      return this->get();
    }
    pf_decl_constexpr 
    operator const _Ty*() const pf_attr_noexcept
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
    pf_decl_constexpr  _Ty *
    get() pf_attr_noexcept
    {
      return &this->ptr_->store;
    }
    pf_decl_constexpr  const _Ty *
    get() const pf_attr_noexcept
    {
      return &this->ptr_->store;
    }
  
    /// Set
    pf_decl_constexpr  void
    set(
      unique_store<_Ty> *__r) pf_attr_noexcept
    {
      if (this->ptr_) this->__delete_no_check();
      this->ptr_ = __r;
    }

    /// Reset
    pf_decl_constexpr  void 
    reset() pf_attr_noexcept
    {
      if (this->ptr_)
      {
        this->__delete_no_check();
        this->ptr_ = nullptr;
      }
    }

  private:
    /// Store
    unique_store<_Ty> *ptr_;
  };

	/// UNIQUE: Ptr -> Array with Empty Deleter
  template <typename _Ty, typename _Deleter>
  requires(std::is_empty_v<_Deleter>)
  class unique_ptr<_Ty[], _Deleter>
  {
    /// Delete
    pf_decl_constexpr void __delete_no_check() pf_attr_noexcept
    {
      _Deleter del;
      del(this->ptr_);
    }

  public:
    /// Constructors
    pf_decl_constexpr  
    unique_ptr(
      unique_store<_Ty[]> *__ptr,
      _Deleter&& __deleter
    ) pf_attr_noexcept
      : ptr_(__ptr)
    {
      ignore = __deleter;
    }
    pf_decl_constexpr  
    unique_ptr(
      nullptr_t,
      _Deleter&& __deleter
    ) pf_attr_noexcept
      : unique_ptr(nullptr, std::move(__deleter))
    {
      ignore = __deleter;
    }
    pf_decl_constexpr  
    unique_ptr(
      _Deleter&& __deleter 
    ) pf_attr_noexcept
      : unique_ptr(nullptr, std::move(__deleter))
    {
      ignore = __deleter;
    }
    pf_decl_constexpr  
    unique_ptr(
      unique_ptr<_Ty[], _Deleter> const &__r
    ) pf_attr_noexcept = delete;
    pf_decl_constexpr  
    unique_ptr(
      unique_ptr<_Ty[], _Deleter> &&__r 
    ) pf_attr_noexcept
      : unique_ptr(__r.ptr_, _Deleter())
    {
      __r.ptr_ = nullptr;
    }

    /// Destructor
    pf_decl_constexpr  
    ~unique_ptr() pf_attr_noexcept
    {
      this->reset();
    }

    /// Operator=
    pf_decl_constexpr  unique_ptr<_Ty[], _Deleter> &
    operator=(
      unique_ptr<_Ty[], _Deleter> const& 
    ) pf_attr_noexcept = delete;
    pf_decl_constexpr  unique_ptr<_Ty[], _Deleter> &
    operator=(
      unique_ptr<_Ty[], _Deleter> &&__r 
    ) pf_attr_noexcept
    {
      if (this != &__r)
      {
        pf_assert(this->ptr_ != __r.ptr_, u8"Both unique ptr are equals!"); 
        this->set(__r.ptr_);
        __r.ptr_ = nullptr;
      }
      return *this;
    }
    pf_decl_constexpr  unique_ptr<_Ty[], _Deleter> &
    operator=(
      unique_store<_Ty[]> *__r 
    ) pf_attr_noexcept
    {
      if (this->ptr_ != __r)
      {
        this->set(__r);
      }
      return *this;
    }

    /// Operator<=> / ==
    pf_decl_constexpr  bool 
    operator==(
      const _Ty *__r 
    ) pf_attr_noexcept
    {
      return (this->ptr_ == __r);
    }
    pf_decl_constexpr  std::strong_ordering 
    operator<=>(
      const _Ty *__r 
    ) pf_attr_noexcept
    {
      return (this->ptr_ <=> __r);
    }

    /// Operator[]
    pf_decl_constexpr _Ty &operator[](
      size_t __index 
    ) pf_attr_noexcept
    {
      return this->get()[__index];
    }
    pf_decl_constexpr const _Ty &operator[](
      size_t __index 
    ) const pf_attr_noexcept
    {
      return this->get()[__index];
    }

    /// Operator _Ty*
    pf_decl_constexpr 
    operator _Ty*() pf_attr_noexcept
    {
      return this->get();
    }
    pf_decl_constexpr 
    operator const _Ty*() const pf_attr_noexcept
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
    pf_decl_constexpr  _Ty *
    get() pf_attr_noexcept
    {
      return &this->ptr_->first;
    }
    pf_decl_constexpr  const _Ty *
    get() const pf_attr_noexcept
    {
      return &this->ptr_->first;
    }
  
    /// Set
    pf_decl_constexpr  void
    set(
      unique_store<_Ty[]> *__r) pf_attr_noexcept
    {
      if (this->ptr_) this->__delete_no_check();
      this->ptr_ = __r;
    }

    /// Reset
    pf_decl_constexpr  void 
    reset() pf_attr_noexcept
    {
      if (this->ptr_)
      {
        this->__delete_no_check();
        this->ptr_ = nullptr;
      }
    }

  private:
    /// Store
    unique_store<_Ty[]> *ptr_;
  };

  /// UNIQUE: Maker
  template <typename _Ty, typename... _Args>
  pf_decl_constexpr unique_ptr<_Ty, deleter_default<unique_store<_Ty>>>
  make_unique(
    _Args&& ... __args 
  ) pf_attr_noexcept
  requires(!std::is_array_v<_Ty>)
  {
    return unique_ptr(
        construct_new<unique_store<_Ty>>(
            std::forward<_Args>(__args)...),
        deleter_default<unique_store<_Ty>>());
  }
  template <typename _Ty, typename... _Args>
  pf_decl_constexpr unique_ptr<_Ty, deleter_default<unique_store<_Ty>>>
  make_unique_aligned(
    align_val_t __align,
    _Args&& ... __args 
  ) pf_attr_noexcept
  requires(!std::is_array_v<_Ty>)
  {
    return unique_ptr(
      construct_new_aligned<unique_store<_Ty>>(
      __align,
      std::forward<_Args>(__args)...),
      deleter_default<unique_store<_Ty>>());
  }
  template <typename _Ty, typename... _Args>
  pf_decl_constexpr unique_ptr<_Ty, deleter_default<unique_store<_Ty>>>
  make_unique_aligned_at(
    align_val_t __align,
    size_t __offset,
    _Args&& ... __args 
  ) pf_attr_noexcept
  requires(!std::is_array_v<_Ty>)
  {
    return unique_ptr(
      construct_new_aligned_at<unique_store<_Ty>>(
      __align,
      __offset,
      std::forward<_Args>(__args)...),
      deleter_default<unique_store<_Ty>>());
  }

  /// UNIQUE: Maker -> Array
  template <typename _Ty, typename... _Args>
  pf_decl_constexpr unique_ptr<_Ty, deleter_default<unique_store<_Ty>>>
  make_unique(
    size_t __count,
    _Args&&... __args
  ) pf_attr_noexcept
  requires(std::is_array_v<_Ty>)
  {
    return unique_ptr(
      construct_new_ex<unique_store<_Ty>>(
      (__count - 1) * sizeof(std::remove_all_extents_t<_Ty>),
      __count,
      std::forward<_Args>(__args)...),
      deleter_default<unique_store<_Ty>>());
  }
  template <typename _Ty, typename... _Args>
  pf_decl_constexpr unique_ptr<_Ty, deleter_default<unique_store<_Ty>>>
  make_unique_aligned(
    size_t __count,
    align_val_t __align,
    _Args&&... __args
  ) pf_attr_noexcept
  requires(std::is_array_v<_Ty>)
  {
    return unique_ptr(
      construct_new_aligned_ex<unique_store<_Ty>>(
      (__count - 1) * sizeof(std::remove_all_extents_t<_Ty>),
      __align,
      __count,
      std::forward<_Args>(__args)...),
      deleter_default<unique_store<_Ty>>());
  }

  /// UNIQUE: Maker -> Allocator
  template <typename _Ty, typename _Allocator, typename... _Args>
  pf_decl_constexpr unique_ptr<_Ty, deleter_allocator<unique_store<_Ty>, _Allocator>>
  make_unique(
    _Allocator &__all,
    _Args&& ... __args
  ) pf_attr_noexcept
  requires(is_allocator_v<_Allocator>
           && !std::is_array_v<_Ty>)
  {
    return unique_ptr(
      construct_new<unique_store<_Ty>>(
      __all,
      std::forward<_Args>(__args)...),
      deleter_allocator<unique_store<_Ty>, _Allocator>(&__all));
  }
  template <typename _Ty, typename _Allocator, typename... _Args>
  pf_decl_constexpr unique_ptr<_Ty, deleter_allocator<unique_store<_Ty>, _Allocator>>
  make_unique_aligned(
    _Allocator &__all,
    align_val_t __align,
    _Args&& ... __args
  ) pf_attr_noexcept
  requires(is_allocator_v<_Allocator>
           && !std::is_array_v<_Ty>)
  {
    return unique_ptr(
      construct_new_aligned_ex<unique_store<_Ty>>(
      __all,
      __align,
      std::forward<_Args>(__args)...),
      deleter_allocator<unique_store<_Ty>, _Allocator>(&__all));
  }
  template <typename _Ty, typename _Allocator, typename... _Args>
  pf_decl_constexpr unique_ptr<_Ty, deleter_allocator<unique_store<_Ty>, _Allocator>>
  make_unique_aligned_at(
    _Allocator &__all,
    align_val_t __align,
    size_t __offset,
    _Args&& ... __args
  ) pf_attr_noexcept
  requires(is_allocator_v<_Allocator>
           && !std::is_array_v<_Ty>)
  {
    return unique_ptr(
      construct_new_aligned_at_ex<unique_store<_Ty>>(
      __all,
      __align,
      __offset,
      std::forward<_Args>(__args)...),
      deleter_allocator<unique_store<_Ty>, _Allocator>(&__all));
  }

  /// UNIQUE: Maker -> Allocator -> Array
  template <typename _Ty, typename _Allocator, typename... _Args>
  pf_decl_constexpr unique_ptr<_Ty, deleter_allocator<unique_store<_Ty>, _Allocator>>
  make_unique(
    _Allocator &__all,
    size_t __count,
    _Args&&... __args 
  ) pf_attr_noexcept
  requires(is_allocator_v<_Allocator>
           && std::is_array_v<_Ty>)
  {
    return unique_ptr(
      construct_new_ex<unique_store<_Ty>>(
      __all,
      (__count - 1) * sizeof(std::remove_all_extents_t<_Ty>),
      __count,
      std::forward<_Args>(__args)...),
      deleter_allocator<unique_store<_Ty>, _Allocator>(&__all));
  }
  template <typename _Ty, typename _Allocator, typename... _Args>
  pf_decl_constexpr unique_ptr<_Ty, deleter_allocator<unique_store<_Ty>, _Allocator>>
  make_unique_aligned(
    _Allocator &__all,
    size_t __count,
    align_val_t __align,
    _Args&&... __args 
  ) pf_attr_noexcept
  requires(is_allocator_v<_Allocator>
           && std::is_array_v<_Ty>)
  {
    return unique_ptr(
      construct_new_ex<unique_store<_Ty>>(
      __all,
      (__count - 1) * sizeof(std::remove_all_extents_t<_Ty>),
      __align,
      __count,
      std::forward<_Args>(__args)...),
      deleter_allocator<unique_store<_Ty>, _Allocator>(&__all));
  }
}

#endif // !PULSAR_UNIQUE_PTR_HPP