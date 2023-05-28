/*! @file   filesystem.hpp
 *  @author Louis-Quentin Noé (noe.louis-quentin@hotmail.com)
 *  @brief
 *  @date   06-04-2023
 *
 *  @copyright Copyright (c) 2023 - Pulsar Software
 *
 *  @since 0.1.8
 */

#ifndef PULSAR_FILESYSTEM_HPP
#define PULSAR_FILESYSTEM_HPP 1

// Include: Pulsar
#include "pulsar/pulsar.hpp"
#include "pulsar/utf8.hpp"

// Pulsar
namespace pul
{
	/// FILESYSTEM:
	template<typename _Magnifier, typename _Allocator>
	class fspath;

	/// FILESYSTEM: Path
	template<typename _Magnifier, typename _Allocator>
	class fspath : public u8string<_Magnifier, _Allocator>
	{
		template<typename _MagnifierR, typename _AllocatorR>
		class fspath;

	public:
		using value_t										 = u8char_t;
		using code_value_t							 = u8code_t;
		using iterator_t								 = iterator<u8char_t>;
		using const_iterator_t					 = const_iterator<u8char_t>;
		using reverse_iterator_t				 = reverse_iterator<iterator_t>;
		using const_reverse_iterator_t	 = reverse_iterator<const_iterator_t>;
		using u8iterator_t							 = u8iterator<u8string<_Magnifier, _Allocator>>;
		using const_u8iterator_t				 = const_u8iterator<u8string<_Magnifier, _Allocator>>;
		using reverse_u8iterator_t			 = reverse_iterator<u8iterator_t>;
		using const_reverse_u8iterator_t = reverse_iterator<const_u8iterator_t>;

		/// Constructors
		pf_decl_inline pf_decl_constexpr
		fspath(
		 align_val_t __align			= ALIGN_DEFAULT,
		 _Magnifier &&__magnifier = _Magnifier(),
		 _Allocator &&__allocator = _Allocator()) pf_attr_noexcept
			: u8string<_Magnifier, _Allocator>(__align, std::move(__magnifier), std::move(__allocator))
		{}
		pf_decl_constexpr
		fspath(
		 u8char_t __val,
		 size_t __count,
		 align_val_t __align			= ALIGN_DEFAULT,
		 _Magnifier &&__magnifier = _Magnifier(),
		 _Allocator &&__allocator = _Allocator())
			: u8string<_Magnifier, _Allocator>(__val, __count, __align, std::move(__magnifier), std::move(__allocator))
		{}
		pf_decl_constexpr
		fspath(
		 u8code_t __code,
		 size_t __count,
		 align_val_t __align			= ALIGN_DEFAULT,
		 _Magnifier &&__magnifier = _Magnifier(),
		 _Allocator &&__allocator = _Allocator())
			: u8string<_Magnifier, _Allocator>(__code, __count, __align, std::move(__magnifier), std::move(__allocator))
		{}
		template<typename _IteratorIn>
		pf_decl_constexpr
		fspath(
		 _IteratorIn __beg,
		 _IteratorIn __end,
		 align_val_t __align			= ALIGN_DEFAULT,
		 _Magnifier &&__magnifier = _Magnifier(),
		 _Allocator &&__allocator = _Allocator())
			requires(is_iterator_v<_IteratorIn>)
			: u8string<_Magnifier, _Allocator>(__beg, __end, __align, std::move(__magnifier), std::move(__allocator))
		{}
		template<size_t _Num>
		pf_decl_constexpr
		fspath(
		 const u8char_t (&__arr)[_Num],
		 align_val_t __align			= ALIGN_DEFAULT,
		 _Magnifier &&__magnifier = _Magnifier(),
		 _Allocator &&__allocator = _Allocator())
			: fspath(pul::begin(__arr), pul::end(__arr), __align, std::move(__magnifier), std::move(__allocator))
		{}
		template<size_t _Num>
		pf_decl_constexpr
		fspath(
		 const u8code_t (&__arr)[_Num],
		 align_val_t __align			= ALIGN_DEFAULT,
		 _Magnifier &&__magnifier = _Magnifier(),
		 _Allocator &&__allocator = _Allocator())
			: fspath(pul::begin(__arr), pul::end(__arr), __align, std::move(__magnifier), std::move(__allocator))
		{}
		template<size_t _Num>
		pf_decl_constexpr
		fspath(
		 array<u8char_t, _Num> const &__arr,
		 align_val_t __align			= ALIGN_DEFAULT,
		 _Magnifier &&__magnifier = _Magnifier(),
		 _Allocator &&__allocator = _Allocator())
			: fspath(pul::begin(__arr), pul::end(__arr), __align, std::move(__magnifier), std::move(__allocator))
		{}
		template<size_t _Num>
		pf_decl_constexpr
		fspath(
		 array<u8code_t, _Num> const &__arr,
		 align_val_t __align			= ALIGN_DEFAULT,
		 _Magnifier &&__magnifier = _Magnifier(),
		 _Allocator &&__allocator = _Allocator())
			: fspath(pul::begin(__arr), pul::end(__arr), __align, std::move(__magnifier), std::move(__allocator))
		{}
		template<typename _View>
		pf_decl_constexpr
		fspath(
		 _View __view,
		 align_val_t __align			= ALIGN_DEFAULT,
		 _Magnifier &&__magnifier = _Magnifier(),
		 _Allocator &&__allocator = _Allocator())
			requires(is_view_v<_View>)
			: fspath(__view.begin(), __view.end(), __align, std::move(__magnifier), std::move(__allocator))
		{}
		pf_decl_constexpr
		fspath(
		 initializer_list<u8char_t> __list,
		 align_val_t __align			= ALIGN_DEFAULT,
		 _Magnifier &&__magnifier = _Magnifier(),
		 _Allocator &&__allocator = _Allocator())
			: fspath(iterator(__list.begin()), iterator(__list.end()), __align, std::move(__magnifier), std::move(__allocator))
		{}
		pf_decl_constexpr
		fspath(
		 initializer_list<u8code_t> __list,
		 align_val_t __align			= ALIGN_DEFAULT,
		 _Magnifier &&__magnifier = _Magnifier(),
		 _Allocator &&__allocator = _Allocator())
			: fspath(iterator(__list.begin()), iterator(__list.end()), __align, std::move(__magnifier), std::move(__allocator))
		{}
		pf_decl_constexpr
		fspath(
		 const u8char_t *__str,
		 size_t __len,
		 align_val_t __align			= ALIGN_DEFAULT,
		 _Magnifier &&__magnifier = _Magnifier(),
		 _Allocator &&__allocator = _Allocator())
			: fspath(iterator(__str), iterator(__str + __len), __align, std::move(__magnifier), std::move(__allocator))
		{}
		pf_decl_constexpr
		fspath(
		 const u8char_t *__str,
		 align_val_t __align			= ALIGN_DEFAULT,
		 _Magnifier &&__magnifier = _Magnifier(),
		 _Allocator &&__allocator = _Allocator())
			: fspath(__str, utf8::lenof(__str) - 1, __align, std::move(__magnifier), std::move(__allocator))
		{}
		template<typename _MagnifierR, typename _AllocatorR>
		pf_decl_constexpr
		fspath(
		 const fspath<_MagnifierR, _AllocatorR> &__r,
		 align_val_t __align,
		 _Magnifier &&__magnifier,
		 _Allocator &&__allocator) pf_attr_noexcept
			: fspath(__r.begin(), __r.end(), __align, std::move(__magnifier))
		{}
		pf_decl_constexpr
		fspath(
		 const fspath<_Magnifier, _Allocator> &__r,
		 align_val_t __align) pf_attr_noexcept
			: fspath(__r.begin(), __r.end(), __align, _Magnifier(), _Allocator())
		{}
		pf_decl_constexpr
		fspath(
		 const fspath<_Magnifier, _Allocator> &__r) pf_attr_noexcept
			: fspath(__r.begin(), __r.end(), __r.align_, _Magnifier(), _Allocator())
		{}
		pf_decl_constexpr
		fspath(
		 fspath<_Magnifier, _Allocator> &&__r) pf_attr_noexcept
			: data_(__r.data_)
			, capacity_(__r.capacity_)
			, align_(__r.align_)
			, len_(__r.len_)
			, magnifier_(std::move(__r.magnifier_))
			, allocator_(std::move(__r.allocator_))
		{
			__r.data_			= nullptr;
			__r.capacity_ = 0;
			__r.len_			= 0;
		}

		/// Destructor
		pf_decl_inline pf_decl_constexpr ~u8string() pf_attr_noexcept
		{
			this->clear();
		}

		/// Operator =
		pf_decl_inline pf_decl_constexpr fspath<_Magnifier, _Allocator> &
		operator=(
		 const u8char_t *__str)
		{
			this->assign(__str);
			return *this;
		}
		template<typename _View>
		pf_decl_inline pf_decl_constexpr fspath<_Magnifier, _Allocator> &
		operator=(
		 _View __v)
			requires(is_view_v<_View>)
		{
			this->assign(__v);
			return *this;
		}
		template<typename _MagnifierR, typename _AllocatorR>
		pf_decl_inline pf_decl_constexpr fspath<_Magnifier, _Allocator> &
		operator=(
		 fspath<_MagnifierR, _AllocatorR> const &__r)
		{
			this->assign(__r);
			return *this;
		}
		template<size_t _Num>
		pf_decl_inline pf_decl_constexpr fspath<_Magnifier, _Allocator> &
		operator=(
		 const u8char_t (&__arr)[_Num])
		{
			this->assign(__arr);
			return *this;
		}
		template<size_t _Num>
		pf_decl_inline pf_decl_constexpr fspath<_Magnifier, _Allocator> &
		operator=(
		 const u8code_t (&__arr)[_Num])
		{
			this->assign(__arr);
			return *this;
		}
		template<size_t _Num>
		pf_decl_inline pf_decl_constexpr fspath<_Magnifier, _Allocator> &
		operator=(
		 array<u8char_t, _Num> const &__arr)
		{
			this->assign(__arr);
			return *this;
		}
		template<size_t _Num>
		pf_decl_inline pf_decl_constexpr fspath<_Magnifier, _Allocator> &
		operator=(
		 array<u8code_t, _Num> const &__arr)
		{
			this->assign(__arr);
			return *this;
		}
		pf_decl_inline pf_decl_constexpr fspath<_Magnifier, _Allocator> &
		operator=(
		 fspath<_Magnifier, _Allocator> &&__r)
		{
			this->assign(std::move(__r));
			return *this;
		}

		/// Operator (View)
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr
		operator u8string_view() const pf_attr_noexcept
		{
			return this->store_;
		}

		/// Push
		template<typename _IteratorIn>
		pf_decl_inline pf_decl_constexpr size_t
		path_push_back(
		 _IteratorIn __beg,
		 _IteratorIn __end)
			requires(is_iterator_v<_IteratorIn>)
		{
			this->__path_push_insert_back(__beg, __end);
		}
		template<typename _View>
		pf_decl_inline pf_decl_constexpr size_t
		path_push_back(
		 _View __v)
			requires(is_view_v<_View>)
		{
			return this->path_push_back(__v.begin(), __v.end());
		}
		pf_decl_inline pf_decl_constexpr size_t
		path_push_back(
		 initializer_list<u8char_t> __il)
		{
			return this->path_push_back(__il.begin(), __il.end());
		}
		pf_decl_inline pf_decl_constexpr size_t
		path_push_back(
		 initializer_list<u8code_t> __il)
		{
			return this->path_push_back(__il.begin(), __il.end());
		}
		pf_decl_inline pf_decl_constexpr size_t
		path_push_back(
		 const u8char_t *__str,
		 size_t __len)
		{
			return this->path_push_back(iterator(__str), iterator(__str + __len));
		}
		pf_decl_inline pf_decl_constexpr size_t
		path_push_back(
		 const u8char_t *__str)
		{
			return this->path_push_back(iterator(__str), iterator(__str + utf8::lenof(__str) - 1));
		}
		template<size_t _Num>
		pf_decl_inline pf_decl_constexpr size_t
		path_push_back(
		 const u8char_t (&__arr)[_Num])
		{
			return this->path_push_back(pul::begin(__arr), pul::end(__arr));
		}
		template<size_t _Num>
		pf_decl_inline pf_decl_constexpr size_t
		path_push_back(
		 const u8code_t (&__arr)[_Num])
		{
			return this->path_push_back(pul::begin(__arr), pul::end(__arr));
		}
		template<size_t _Num>
		pf_decl_inline pf_decl_constexpr size_t
		path_push_back(
		 array<u8char_t, _Num> const &__arr)
		{
			return this->path_push_back(pul::begin(__arr), pul::end(__arr));
		}
		template<size_t _Num>
		pf_decl_inline pf_decl_constexpr size_t
		path_push_back(
		 array<u8code_t, _Num> const &__arr)
		{
			return this->path_push_back(pul::begin(__arr), pul::end(__arr));
		}

		template<typename _IteratorIn>
		pf_decl_inline pf_decl_constexpr size_t
		path_push(
		 size_t __w,
		 _IteratorIn __beg,
		 _IteratorIn __end)
			requires(is_iterator_v<_IteratorIn>)
		{
			this->__path_push_insert(__w, __beg, __end);
		}
		template<typename _View>
		pf_decl_inline pf_decl_constexpr size_t
		path_push(
		 size_t __w,
		 _View __v)
			requires(is_view_v<_View>)
		{
			return this->path_push(__w, __v.begin(), __v.end());
		}
		pf_decl_inline pf_decl_constexpr size_t
		path_push(
		 size_t __w,
		 initializer_list<u8char_t> __il)
		{
			return this->path_push(__w, __il.begin(), __il.end());
		}
		pf_decl_inline pf_decl_constexpr size_t
		path_push(
		 size_t __w,
		 initializer_list<u8code_t> __il)
		{
			return this->path_push(__w, __il.begin(), __il.end());
		}
		pf_decl_inline pf_decl_constexpr size_t
		path_push(
		 size_t __w,
		 const u8char_t *__str,
		 size_t __len)
		{
			return this->path_push(__w, iterator(__str), iterator(__str + utf8::lenof(__str) - 1));
		}
		pf_decl_inline pf_decl_constexpr size_t
		path_push(
		 size_t __w,
		 const u8char_t *__str)
		{
			return this->path_push(__w, iterator(__str));
		}
		template<size_t _Num>
		pf_decl_inline pf_decl_constexpr size_t
		path_push(
		 size_t __w,
		 const u8char_t (&__arr)[_Num])
		{
			return this->path_push(__w, pul::begin(__arr), pul::end(__arr));
		}
		template<size_t _Num>
		pf_decl_inline pf_decl_constexpr size_t
		path_push(
		 size_t __w,
		 const u8code_t (&__arr)[_Num])
		{
			return this->path_push(__w, pul::begin(__arr), pul::end(__arr));
		}
		template<size_t _Num>
		pf_decl_inline pf_decl_constexpr size_t
		path_push(
		 size_t __w,
		 array<u8char_t, _Num> const &__arr)
		{
			return this->path_push(__w, pul::begin(__arr), pul::end(__arr));
		}
		template<size_t _Num>
		pf_decl_inline pf_decl_constexpr size_t
		path_push(
		 size_t __w,
		 array<u8code_t, _Num> const &__arr)
		{
			return this->path_push(__w, pul::begin(__arr), pul::end(__arr));
		}

		/// Insert
		pf_decl_inline pf_decl_constexpr iterator_t
		path_insert_back(
		 _IteratorIn __beg,
		 _IteratorIn __end)
		{
			return this->begin() + this->__path_push_insert_back(__beg, __end);
		}
		template<typename _View>
		pf_decl_inline pf_decl_constexpr iterator_t
		path_insert_back(
		 _View __v)
			requires(is_view_v<_View>)
		{
			return this->path_insert_back(__v.begin(), __v.end());
		}
		pf_decl_inline pf_decl_constexpr iterator_t
		path_insert_back(
		 initializer_list<u8char_t> __il)
		{
			return this->path_insert_back(__il.begin(), __il.end());
		}
		pf_decl_inline pf_decl_constexpr iterator_t
		path_insert_back(
		 initializer_list<u8code_t> __il)
		{
			return this->path_insert_back(__il.begin(), __il.end());
		}
		pf_decl_inline pf_decl_constexpr iterator_t
		path_insert_back(
		 const u8char_t *__str,
		 size_t __len)
		{
			return this->path_insert_back(iterator(__str), iterator(__str + __len));
		}
		pf_decl_inline pf_decl_constexpr iterator_t
		path_insert_back(
		 const u8char_t *__str)
		{
			return this->path_insert_back(iterator(__str), iterator(__str + utf8::lenof(__str) - 1));
		}
		template<size_t _Num>
		pf_decl_inline pf_decl_constexpr iterator_t
		path_insert_back(
		 const u8char_t (&__arr)[_Num])
		{
			return this->path_insert_back(pul::begin(__arr), pul::end(__arr));
		}
		template<size_t _Num>
		pf_decl_inline pf_decl_constexpr iterator_t
		path_insert_back(
		 const u8code_t (&__arr)[_Num])
		{
			return this->path_insert_back(pul::begin(__arr), pul::end(__arr));
		}
		template<size_t _Num>
		pf_decl_inline pf_decl_constexpr iterator_t
		path_insert_back(
		 array<u8char_t, _Num> const &__arr)
		{
			return this->path_insert_back(pul::begin(__arr), pul::end(__arr));
		}
		template<size_t _Num>
		pf_decl_inline pf_decl_constexpr iterator_t
		path_insert_back(
		 array<u8code_t, _Num> const &__arr)
		{
			return this->path_insert_back(pul::begin(__arr), pul::end(__arr));
		}

		template<typename _IteratorIn>
		pf_decl_inline pf_decl_constexpr iterator_t
		path_insert(
		 iterator_t __w,
		 _IteratorIn __beg,
		 _IteratorIn __end)
			requires(is_iterator_v<_IteratorIn>)
		{
			return this->begin() + this->__path_push_insert(this->__iterator_to_index(__w), __beg, __end);
		}
		template<typename _View>
		pf_decl_inline pf_decl_constexpr iterator_t
		path_insert(
		 iterator_t __w,
		 _View __v)
			requires(is_view_v<_View>)
		{
			return this->path_insert(__w, __v.begin(), __v.end());
		}
		pf_decl_inline pf_decl_constexpr iterator_t
		path_insert(
		 iterator_t __w,
		 initializer_list<u8char_t> __il)
		{
			return this->path_insert(__w, __il.begin(), __il.end());
		}
		pf_decl_inline pf_decl_constexpr iterator_t
		path_insert(
		 iterator_t __w,
		 initializer_list<u8code_t> __il)
		{
			return this->path_insert(__w, __il.begin(), __il.end());
		}
		pf_decl_inline pf_decl_constexpr iterator_t
		path_insert(
		 iterator_t __w,
		 const u8char_t *__str,
		 size_t __len)
		{
			return this->path_insert(__w, iterator(__str), iterator(__str + utf8::lenof(__str) - 1));
		}
		pf_decl_inline pf_decl_constexpr iterator_t
		path_insert(
		 iterator_t __w,
		 const u8char_t *__str)
		{
			return this->path_insert(__w, iterator(__str));
		}
		template<size_t _Num>
		pf_decl_inline pf_decl_constexpr iterator_t
		path_insert(
		 iterator_t __w,
		 const u8char_t (&__arr)[_Num])
		{
			return this->path_insert(__w, pul::begin(__arr), pul::end(__arr));
		}
		template<size_t _Num>
		pf_decl_inline pf_decl_constexpr iterator_t
		path_insert(
		 iterator_t __w,
		 const u8code_t (&__arr)[_Num])
		{
			return this->path_insert(__w, pul::begin(__arr), pul::end(__arr));
		}
		template<size_t _Num>
		pf_decl_inline pf_decl_constexpr iterator_t
		path_insert(
		 iterator_t __w,
		 array<u8char_t, _Num> const &__arr)
		{
			return this->path_insert(__w, pul::begin(__arr), pul::end(__arr));
		}
		template<size_t _Num>
		pf_decl_inline pf_decl_constexpr iterator_t
		path_insert(
		 iterator_t __w,
		 array<u8code_t, _Num> const &__arr)
		{
			return this->path_insert(__w, pul::begin(__arr), pul::end(__arr));
		}

		/// Pop
		pf_decl_inline pf_decl_constexpr size_t
		pop_extension();
		pf_decl_inline pf_decl_constexpr size_t
		pop_filename();
		pf_decl_inline pf_decl_constexpr size_t
		pop_component();

		/// Remove
		pf_decl_inline pf_decl_constexpr iterator_t
		remove_extension();
		pf_decl_inline pf_decl_constexpr iterator_t
		remove_filename();
		pf_decl_inline pf_decl_constexpr iterator_t
		remove_component();

		/// Replace
		pf_decl_inline pf_decl_constexpr size_t
		replace_extension();
		pf_decl_inline pf_decl_constexpr size_t
		replace_filename();
		pf_decl_inline pf_decl_constexpr size_t
		replace_component();

		/// Reinsert
	};

	/// FILESYSTEM: Iterator
	class fsiterator
	{
	};

	/// FILESYSTEM: Iterator - Recursive
	class fsiterator_recursive
	{
	};

	/// FILESYSTEM: Utility

	/// Get Root Name

	/// Get Root Directory

	/// Get Root Path

	/// Get Known Path

	/// Get Relative Path

	/// Get Parent Path

	/// Get Filename

	/// Get Stem

	/// Get Extension

	/// Has Root Path

	/// Has Root Name

	/// Has Root Directory

	/// Has Relative Path

	/// Has Parent Path

	/// Has Filename

	/// Has Stem

	/// Has Extension

	/// To Absolute

	/// To Relative

	/// To Canonical

	/// Create File

	/// Create Directory

	/// Create Directories

	/// Exist

	/// Remove Path

	/// Remove All Path

	/// Change File Permissions

	/// FILESYSTEM: Handle
	class fshandle
	{
	};

	/// FILESYSTEM: Package
	class fspackage	 // libzip, .ppkg
	{
	};
	class fspackage_iterator
	{
	};

	/// FILESYSTEM: Signal
	class fssignal
	{
	};

}	 // namespace pul

#endif	// PULSAR_FILESYSTEM_HPP
