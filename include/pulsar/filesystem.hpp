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
#include "pulsar/char.hpp"

// Pulsar
namespace pul
{
	// Filesystem
	namespace fs
	{
		// NOTE: Every path will be wrote using forward slash '/'
		// NOTE: filename is stem.extension

		/// FILESYSTEM: Types
		template<typename _Magnifier = magnifier_default, typename _Allocator = allocator_halloc>
			requires(is_magnifier_v<_Magnifier> && is_allocator_v<_Allocator>)
		class path;
		using path_view = u8string_view;

		/// FILESYSTEM: Path
		template<typename _Magnifier, typename _Allocator>
			requires(is_magnifier_v<_Magnifier> && is_allocator_v<_Allocator>)
		class path : public u8string<_Magnifier, _Allocator>
		{
			template<typename _MagnifierR, typename _AllocatorR>
				requires(is_magnifier_v<_MagnifierR> && is_allocator_v<_AllocatorR>)
			pf_decl_friend class path;

			// __path_push_insert_back
			template<typename _IteratorIn>
			pf_hint_nodiscard pf_decl_constexpr size_t
			__path_push_insert_back(
			 _IteratorIn __beg,
			 _IteratorIn __end)
			{
				while(__end - 1 != __beg && *(__end - 1) == '/') --__end;
				size_t len = this->__input_required_length(__beg, __end);
				if(pf_unlikely(!len)) return -1;
				bool ss;
				if(pf_unlikely(!this->len_))
				{
					++len;
					ss = false;
				}
				else
				{
					ss = (this->data_[this->len_ - 2] == '/');
				}
				bool is = (__beg == '/');
				if(!is && !ss)
				{
					++len;
					this->reserve_with_magnifier(this->len_ + len);
					auto b = this->begin() + this->len_ - 1;
					auto e = this->begin() + this->len_ + len - 1;
					*b		 = '/';
					++b;
					this->__input_copy(b, __beg, __end);
					*e = '\0';
				}
				else
				{
					if(is && ss)
					{
						++__beg;
						--len;
					}
					this->reserve_with_magnifier(this->len_ + len);
					auto e = this->begin() + this->len_ + len - 1;
					this->__input_copy(this->begin() + this->len_ - 1, __beg, __end);
					*e = '\0';
				}
				this->len_ += len;
				return this->len_ - 1;
			}

			// __path_pop_remove_filename
			pf_hint_nodiscard pf_decl_constexpr size_t
			__path_pop_remove_filename()
			{
				size_t s = (this->len_ ? this->len_ - 2 : 0);
				bool f	 = false;
				for(; s != size_t(-1); --s)
				{
					if(this->data_[s] == '/')
					{
						f = true;
						break;
					}
				}
				if(f)
				{
					this->data_[s] = '\0';
					this->len_		-= (this->len_ - s - 1);
					if(pf_unlikely(this->len_ == 1))
					{
						this->len_ = 0;
						return -1;
					}
					return s;
				}
				return -1;
			}

			// __path_pop_remove_extension
			pf_hint_nodiscard pf_decl_constexpr size_t
			__path_pop_remove_extension()
			{
				size_t s = (this->len_ ? this->len_ - 2 : 0);
				bool f	 = false;
				for(; s != size_t(-1) && this->data_[s] != '/'; --s)
				{
					if(this->data_[s] == '.')
					{
						f = true;
						break;
					}
				}
				if(f)
				{
					this->data_[s] = '\0';
					if(s > 0 && this->data_[s - 1] == '/') --s;
					this->len_ -= (this->len_ - s - 1);
					if(pf_unlikely(this->len_ == 1))
					{
						this->len_ = 0;
						return -1;
					}
					return s;
				}
				return -1;
			}

			// __path_replace_reinsert_filename
			template<typename _IteratorIn>
			pf_hint_nodiscard pf_decl_constexpr size_t
			__path_replace_reinsert_filename(
			 _IteratorIn __beg,
			 _IteratorIn __end)
			{
				while(__end - 1 != __beg && *(__end - 1) == '/') --__end;
				size_t len = this->__input_required_length(__beg, __end);
				if(pf_unlikely(!len)) return this->__path_pop_remove_filename();
				size_t s;
				if(pf_unlikely(!this->len_))
				{
					++len;
					s = 0;
				}
				else
				{
					s = this->len_ - 2;
				}
				for(; s != size_t(-1); --s)
				{
					if(this->data_[s] == '/') break;
				}
				++s;
				const diff_t diff = len - (this->len_ - s - 1);
				this->reserve_with_magnifier(this->len_ + diff);
				this->__input_copy(this->data_ + s, __beg, __end);
				this->len_								 += diff;
				this->data_[this->len_ - 1] = '\0';
				return s;
			}

			// __path_replace_reinsert_component
			template<typename _IteratorIn>
			pf_hint_nodiscard pf_decl_constexpr size_t
			__path_replace_reinsert_stem(
			 _IteratorIn __beg,
			 _IteratorIn __end)
			{
				size_t len = this->__input_required_length(__beg, __end);
				if(pf_unlikely(!len)) return -1;
				size_t e;
				if(pf_unlikely(!this->len_))
				{
					++len;
					e = 0;
				}
				else
				{
					e = this->len_ - 2;
				}
				bool fe = false;
				for(; e != size_t(-1) && this->data_[e] != '/'; --e)
				{
					if(this->data_[e] == '.')
					{
						fe = true;
						break;
					}
				}
				const diff_t diff = len - (this->len_ - e - 1);
				this->reserve_with_magnifier(this->len_ + diff);
				size_t s = e;
				for(; s != size_t(-1); --s)
				{
					if(this->data_[s] == '/') break;
				}
				++s;
				if(fe)
				{
					memmove(this->data_ + e + diff, this->data_ + e, this->len_ - e);
				}
				this->__input_copy(this->data_ + s, __beg, __end);
				this->len_ += diff;
				return s;
			}

			// __path_replace_reinsert_extension
			template<typename _IteratorIn>
			pf_hint_nodiscard pf_decl_constexpr size_t
			__path_replace_reinsert_extension(
			 _IteratorIn __beg,
			 _IteratorIn __end)
			{
				size_t len = this->__input_required_length(__beg, __end);
				if(pf_unlikely(!len)) return this->__path_pop_remove_extension();
				size_t s;
				if(pf_unlikely(!this->len_))
				{
					++len;
					s = 0;
				}
				else
				{
					s = this->len_ - 2;
				}
				bool f = false;
				for(; s != size_t(-1) && this->data_[s] != '/'; --s)
				{
					if(this->data_[s] == '.')
					{
						f = true;
						if(*__beg == '.')
						{
							++__beg;
							--len;
						}
						break;
					}
				}
				if(!f && *__beg != '.')
				{
					++len;
					this->reserve_with_magnifier(this->len_ + len);
					this->data_[s++] = '.';
				}
				else
				{
					this->reserve_with_magnifier(this->len_ + len);
				}
				this->__input_copy(iterator(this->data_ + s), __beg, __end);
				this->data_[s + len] = '\0';
				this->len_					+= (this->len_ - s - 1);
				return s;
			}

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
			path(
			 align_val_t __align			= ALIGN_NO_ALIGN,
			 _Magnifier &&__magnifier = _Magnifier(),
			 _Allocator &&__allocator = _Allocator()) pf_attr_noexcept
				: u8string<_Magnifier, _Allocator>(__align, std::move(__magnifier), std::move(__allocator))
			{}
			pf_decl_constexpr
			path(
			 u8char_t __val,
			 size_t __count,
			 align_val_t __align			= ALIGN_NO_ALIGN,
			 _Magnifier &&__magnifier = _Magnifier(),
			 _Allocator &&__allocator = _Allocator())
				: u8string<_Magnifier, _Allocator>(__val, __count, __align, std::move(__magnifier), std::move(__allocator))
			{}
			pf_decl_constexpr
			path(
			 u8code_t __code,
			 size_t __count,
			 align_val_t __align			= ALIGN_NO_ALIGN,
			 _Magnifier &&__magnifier = _Magnifier(),
			 _Allocator &&__allocator = _Allocator())
				: u8string<_Magnifier, _Allocator>(__code, __count, __align, std::move(__magnifier), std::move(__allocator))
			{}
			template<typename _IteratorIn>
			pf_decl_constexpr
			path(
			 _IteratorIn __beg,
			 _IteratorIn __end,
			 align_val_t __align			= ALIGN_NO_ALIGN,
			 _Magnifier &&__magnifier = _Magnifier(),
			 _Allocator &&__allocator = _Allocator())
				requires(is_iterator_v<_IteratorIn>)
				: u8string<_Magnifier, _Allocator>(__beg, __end, __align, std::move(__magnifier), std::move(__allocator))
			{
				pf_assert(*(__end - 1) != '/', "Path string shouldn't terminate with end /!");
			}
			template<size_t _Num>
			pf_decl_constexpr
			path(
			 const u8char_t (&__arr)[_Num],
			 align_val_t __align			= ALIGN_NO_ALIGN,
			 _Magnifier &&__magnifier = _Magnifier(),
			 _Allocator &&__allocator = _Allocator())
				: path(pul::begin(__arr), pul::end(__arr), __align, std::move(__magnifier), std::move(__allocator))
			{}
			template<size_t _Num>
			pf_decl_constexpr
			path(
			 const u8code_t (&__arr)[_Num],
			 align_val_t __align			= ALIGN_NO_ALIGN,
			 _Magnifier &&__magnifier = _Magnifier(),
			 _Allocator &&__allocator = _Allocator())
				: path(pul::begin(__arr), pul::end(__arr), __align, std::move(__magnifier), std::move(__allocator))
			{}
			template<size_t _Num>
			pf_decl_constexpr
			path(
			 array<u8char_t, _Num> const &__arr,
			 align_val_t __align			= ALIGN_NO_ALIGN,
			 _Magnifier &&__magnifier = _Magnifier(),
			 _Allocator &&__allocator = _Allocator())
				: path(pul::begin(__arr), pul::end(__arr), __align, std::move(__magnifier), std::move(__allocator))
			{}
			template<size_t _Num>
			pf_decl_constexpr
			path(
			 array<u8code_t, _Num> const &__arr,
			 align_val_t __align			= ALIGN_NO_ALIGN,
			 _Magnifier &&__magnifier = _Magnifier(),
			 _Allocator &&__allocator = _Allocator())
				: path(pul::begin(__arr), pul::end(__arr), __align, std::move(__magnifier), std::move(__allocator))
			{}
			template<typename _View>
			pf_decl_constexpr
			path(
			 _View __view,
			 align_val_t __align			= ALIGN_NO_ALIGN,
			 _Magnifier &&__magnifier = _Magnifier(),
			 _Allocator &&__allocator = _Allocator())
				requires(is_view_v<_View>)
				: path(__view.begin(), __view.end(), __align, std::move(__magnifier), std::move(__allocator))
			{}
			pf_decl_constexpr
			path(
			 const u8char_t *__str,
			 size_t __len,
			 align_val_t __align			= ALIGN_NO_ALIGN,
			 _Magnifier &&__magnifier = _Magnifier(),
			 _Allocator &&__allocator = _Allocator())
				: path(iterator(__str), iterator(__str + __len), __align, std::move(__magnifier), std::move(__allocator))
			{}
			pf_decl_constexpr
			path(
			 const u8char_t *__str,
			 align_val_t __align			= ALIGN_NO_ALIGN,
			 _Magnifier &&__magnifier = _Magnifier(),
			 _Allocator &&__allocator = _Allocator())
				: path(__str, utf8::lenof(__str) - 1, __align, std::move(__magnifier), std::move(__allocator))
			{}
			template<typename _MagnifierR, typename _AllocatorR>
			pf_decl_constexpr
			path(
			 const path<_MagnifierR, _AllocatorR> &__r,
			 align_val_t __align,
			 _Magnifier &&__magnifier,
			 _Allocator &&__allocator) pf_attr_noexcept
				: path(__r.begin(), __r.end(), __align, std::move(__magnifier))
			{}
			pf_decl_constexpr
			path(
			 const path<_Magnifier, _Allocator> &__r,
			 align_val_t __align) pf_attr_noexcept
				: path(__r.begin(), __r.end(), __align, _Magnifier(), _Allocator())
			{}
			pf_decl_constexpr
			path(
			 const path<_Magnifier, _Allocator> &__r) pf_attr_noexcept
				: path(__r.begin(), __r.end(), __r.align_, _Magnifier(), _Allocator())
			{}
			pf_decl_constexpr
			path(
			 path<_Magnifier, _Allocator> &&__r) pf_attr_noexcept = default;

			/// Destructor
			pf_decl_inline pf_decl_constexpr ~path() pf_attr_noexcept
			{
				this->clear();
			}

			/// Operator =
			pf_decl_inline pf_decl_constexpr path<_Magnifier, _Allocator> &
			operator=(
			 const u8char_t *__str)
			{
				this->assign(__str);
				return *this;
			}
			template<typename _View>
			pf_decl_inline pf_decl_constexpr path<_Magnifier, _Allocator> &
			operator=(
			 _View __v)
				requires(is_view_v<_View>)
			{
				this->assign(__v);
				return *this;
			}
			template<typename _MagnifierR, typename _AllocatorR>
			pf_decl_inline pf_decl_constexpr path<_Magnifier, _Allocator> &
			operator=(
			 path<_MagnifierR, _AllocatorR> const &__r)
			{
				this->assign(__r);
				return *this;
			}
			template<size_t _Num>
			pf_decl_inline pf_decl_constexpr path<_Magnifier, _Allocator> &
			operator=(
			 const u8char_t (&__arr)[_Num])
			{
				this->assign(__arr);
				return *this;
			}
			template<size_t _Num>
			pf_decl_inline pf_decl_constexpr path<_Magnifier, _Allocator> &
			operator=(
			 const u8code_t (&__arr)[_Num])
			{
				this->assign(__arr);
				return *this;
			}
			template<size_t _Num>
			pf_decl_inline pf_decl_constexpr path<_Magnifier, _Allocator> &
			operator=(
			 array<u8char_t, _Num> const &__arr)
			{
				this->assign(__arr);
				return *this;
			}
			template<size_t _Num>
			pf_decl_inline pf_decl_constexpr path<_Magnifier, _Allocator> &
			operator=(
			 array<u8code_t, _Num> const &__arr)
			{
				this->assign(__arr);
				return *this;
			}
			pf_decl_inline pf_decl_constexpr path<_Magnifier, _Allocator> &
			operator=(
			 path<_Magnifier, _Allocator> &&__r)
			{
				this->assign(std::move(__r));
				return *this;
			}

			/// Operator (View)
			pf_hint_nodiscard pf_decl_inline pf_decl_constexpr
			operator path_view() const pf_attr_noexcept
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
				return this->__path_push_insert_back(__beg, __end);
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

			/// Insert
			template<typename _IteratorIn>
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

			/// Pop
			pf_decl_inline pf_decl_constexpr size_t
			pop_extension()
			{
				return this->__path_pop_remove_extension();
			}
			pf_decl_inline pf_decl_constexpr size_t
			pop_filename()
			{
				return this->__path_pop_remove_filename();
			}

			/// Remove
			pf_decl_inline pf_decl_constexpr iterator_t
			remove_extension()
			{
				return this->begin() + this->__path_pop_remove_extension();
			}
			pf_decl_inline pf_decl_constexpr iterator_t
			remove_filename()
			{
				return this->begin() + this->__path_pop_remove_filename();
			}

			/// Replace
			template<typename _IteratorIn>
			pf_decl_inline pf_decl_constexpr size_t
			replace_extension(
			 _IteratorIn __beg,
			 _IteratorIn __end)
				requires(is_iterator_v<_IteratorIn>)
			{
				return this->__path_replace_reinsert_extension(__beg, __end);
			}
			template<typename _View>
			pf_decl_inline pf_decl_constexpr size_t
			replace_extension(
			 _View __v)
				requires(is_view_v<_View>)
			{
				return this->replace_extension(__v.begin(), __v.end());
			}
			template<size_t _Num>
			pf_decl_inline pf_decl_constexpr size_t
			replace_extension(
			 const u8char_t (&__arr)[_Num])
			{
				return this->replace_extension(pul::begin(__arr), pul::end(__arr));
			}
			template<size_t _Num>
			pf_decl_inline pf_decl_constexpr size_t
			replace_extension(
			 const u8code_t (&__arr)[_Num])
			{
				return this->replace_extension(pul::begin(__arr), pul::end(__arr));
			}
			template<size_t _Num>
			pf_decl_inline pf_decl_constexpr size_t
			replace_extension(
			 array<u8char_t, _Num> const &__arr)
			{
				return this->replace_extension(__arr.begin(), __arr.end());
			}
			template<size_t _Num>
			pf_decl_inline pf_decl_constexpr size_t
			replace_extension(
			 array<u8code_t, _Num> const &__arr)
			{
				return this->replace_extension(__arr.begin(), __arr.end());
			}
			pf_decl_inline pf_decl_constexpr size_t
			replace_extension(
			 const u8char_t *__str,
			 size_t __len)
			{
				return this->replace_extension(iterator(__str), iterator(__str + __len));
			}
			pf_decl_inline pf_decl_constexpr size_t
			replace_extension(
			 const u8char_t *__str)
			{
				return this->replace_extension(iterator(__str), iterator(__str + utf8::lenof(__str) - 1));
			}


			template<typename _IteratorIn>
			pf_decl_inline pf_decl_constexpr size_t
			replace_filename(
			 _IteratorIn __beg,
			 _IteratorIn __end)
				requires(is_iterator_v<_IteratorIn>)
			{
				return this->__path_replace_reinsert_filename(__beg, __end);
			}
			template<typename _View>
			pf_decl_inline pf_decl_constexpr size_t
			replace_filename(
			 _View __v)
				requires(is_view_v<_View>)
			{
				return this->replace_filename(__v.begin(), __v.end());
			}
			template<size_t _Num>
			pf_decl_inline pf_decl_constexpr size_t
			replace_filename(
			 const u8char_t (&__arr)[_Num])
			{
				return this->replace_filename(pul::begin(__arr), pul::end(__arr));
			}
			template<size_t _Num>
			pf_decl_inline pf_decl_constexpr size_t
			replace_filename(
			 const u8code_t (&__arr)[_Num])
			{
				return this->replace_filename(pul::begin(__arr), pul::end(__arr));
			}
			template<size_t _Num>
			pf_decl_inline pf_decl_constexpr size_t
			replace_filename(
			 array<u8char_t, _Num> const &__arr)
			{
				return this->replace_filename(__arr.begin(), __arr.end());
			}
			template<size_t _Num>
			pf_decl_inline pf_decl_constexpr size_t
			replace_filename(
			 array<u8code_t, _Num> const &__arr)
			{
				return this->replace_filename(__arr.begin(), __arr.end());
			}
			pf_decl_inline pf_decl_constexpr size_t
			replace_filename(
			 const u8char_t *__str,
			 size_t __len)
			{
				return this->replace_filename(iterator(__str), iterator(__str + __len));
			}
			pf_decl_inline pf_decl_constexpr size_t
			replace_filename(
			 const u8char_t *__str)
			{
				return this->replace_filename(iterator(__str), iterator(__str + utf8::lenof(__str) - 1));
			}


			template<typename _IteratorIn>
			pf_decl_inline pf_decl_constexpr size_t
			replace_stem(
			 _IteratorIn __beg,
			 _IteratorIn __end)
				requires(is_iterator_v<_IteratorIn>)
			{
				return this->__path_replace_reinsert_stem(__beg, __end);
			}
			template<typename _View>
			pf_decl_inline pf_decl_constexpr size_t
			replace_stem(
			 _View __v)
				requires(is_view_v<_View>)
			{
				return this->replace_stem(__v.begin(), __v.end());
			}
			template<size_t _Num>
			pf_decl_inline pf_decl_constexpr size_t
			replace_stem(
			 const u8char_t (&__arr)[_Num])
			{
				return this->replace_stem(pul::begin(__arr), pul::end(__arr));
			}
			template<size_t _Num>
			pf_decl_inline pf_decl_constexpr size_t
			replace_stem(
			 const u8code_t (&__arr)[_Num])
			{
				return this->replace_stem(pul::begin(__arr), pul::end(__arr));
			}
			template<size_t _Num>
			pf_decl_inline pf_decl_constexpr size_t
			replace_stem(
			 array<u8char_t, _Num> const &__arr)
			{
				return this->replace_stem(__arr.begin(), __arr.end());
			}
			template<size_t _Num>
			pf_decl_inline pf_decl_constexpr size_t
			replace_stem(
			 array<u8code_t, _Num> const &__arr)
			{
				return this->replace_stem(__arr.begin(), __arr.end());
			}
			pf_decl_inline pf_decl_constexpr size_t
			replace_stem(
			 const u8char_t *__str,
			 size_t __len)
			{
				return this->replace_stem(iterator(__str), iterator(__str + __len));
			}
			pf_decl_inline pf_decl_constexpr size_t
			replace_stem(
			 const u8char_t *__str)
			{
				return this->replace_stem(iterator(__str), iterator(__str + utf8::lenof(__str) - 1));
			}


			/// Reinsert
			template<typename _IteratorIn>
			pf_decl_inline pf_decl_constexpr iterator_t
			reinsert_extension(
			 _IteratorIn __beg,
			 _IteratorIn __end)
				requires(is_iterator_v<_IteratorIn>)
			{
				return this->begin() + this->__path_replace_reinsert_extension(__beg, __end);
			}
			template<typename _View>
			pf_decl_inline pf_decl_constexpr iterator_t
			reinsert_extension(
			 _View __v)
				requires(is_view_v<_View>)
			{
				return this->reinsert_extension(__v.begin(), __v.end());
			}
			template<size_t _Num>
			pf_decl_inline pf_decl_constexpr iterator_t
			reinsert_extension(
			 const u8char_t (&__arr)[_Num])
			{
				return this->reinsert_extension(pul::begin(__arr), pul::end(__arr));
			}
			template<size_t _Num>
			pf_decl_inline pf_decl_constexpr iterator_t
			reinsert_extension(
			 const u8code_t (&__arr)[_Num])
			{
				return this->reinsert_extension(pul::begin(__arr), pul::end(__arr));
			}
			template<size_t _Num>
			pf_decl_inline pf_decl_constexpr iterator_t
			reinsert_extension(
			 array<u8char_t, _Num> const &__arr)
			{
				return this->reinsert_extension(__arr.begin(), __arr.end());
			}
			template<size_t _Num>
			pf_decl_inline pf_decl_constexpr iterator_t
			reinsert_extension(
			 array<u8code_t, _Num> const &__arr)
			{
				return this->reinsert_extension(__arr.begin(), __arr.end());
			}
			pf_decl_inline pf_decl_constexpr iterator_t
			reinsert_extension(
			 const u8char_t *__str,
			 size_t __len)
			{
				return this->reinsert_extension(iterator(__str), iterator(__str + __len));
			}
			pf_decl_inline pf_decl_constexpr iterator_t
			reinsert_extension(
			 const u8char_t *__str)
			{
				return this->reinsert_extension(iterator(__str), iterator(__str + utf8::lenof(__str) - 1));
			}


			template<typename _IteratorIn>
			pf_decl_inline pf_decl_constexpr iterator_t
			reinsert_filename(
			 _IteratorIn __beg,
			 _IteratorIn __end)
				requires(is_iterator_v<_IteratorIn>)
			{
				return this->begin() + this->__path_replace_reinsert_filename(__beg, __end);
			}
			template<typename _View>
			pf_decl_inline pf_decl_constexpr iterator_t
			reinsert_filename(
			 _View __v)
				requires(is_view_v<_View>)
			{
				return this->reinsert_filename(__v.begin(), __v.end());
			}
			template<size_t _Num>
			pf_decl_inline pf_decl_constexpr iterator_t
			reinsert_filename(
			 const u8char_t (&__arr)[_Num])
			{
				return this->reinsert_filename(pul::begin(__arr), pul::end(__arr));
			}
			template<size_t _Num>
			pf_decl_inline pf_decl_constexpr iterator_t
			reinsert_filename(
			 const u8code_t (&__arr)[_Num])
			{
				return this->reinsert_filename(pul::begin(__arr), pul::end(__arr));
			}
			template<size_t _Num>
			pf_decl_inline pf_decl_constexpr iterator_t
			reinsert_filename(
			 array<u8char_t, _Num> const &__arr)
			{
				return this->reinsert_filename(__arr.begin(), __arr.end());
			}
			template<size_t _Num>
			pf_decl_inline pf_decl_constexpr iterator_t
			reinsert_filename(
			 array<u8code_t, _Num> const &__arr)
			{
				return this->reinsert_filename(__arr.begin(), __arr.end());
			}
			pf_decl_inline pf_decl_constexpr iterator_t
			reinsert_filename(
			 const u8char_t *__str,
			 size_t __len)
			{
				return this->reinsert_filename(iterator(__str), iterator(__str + __len));
			}
			pf_decl_inline pf_decl_constexpr iterator_t
			reinsert_filename(
			 const u8char_t *__str)
			{
				return this->reinsert_filename(iterator(__str), iterator(__str + utf8::lenof(__str) - 1));
			}

			template<typename _IteratorIn>
			pf_decl_inline pf_decl_constexpr iterator_t
			reinsert_stem(
			 _IteratorIn __beg,
			 _IteratorIn __end)
				requires(is_iterator_v<_IteratorIn>)
			{
				return this->begin() + this->__path_replace_reinsert_stem(__beg, __end);
			}
			template<typename _View>
			pf_decl_inline pf_decl_constexpr iterator_t
			reinsert_stem(
			 _View __v)
				requires(is_view_v<_View>)
			{
				return this->reinsert_stem(__v.begin(), __v.end());
			}
			template<size_t _Num>
			pf_decl_inline pf_decl_constexpr iterator_t
			reinsert_stem(
			 const u8char_t (&__arr)[_Num])
			{
				return this->reinsert_stem(pul::begin(__arr), pul::end(__arr));
			}
			template<size_t _Num>
			pf_decl_inline pf_decl_constexpr iterator_t
			reinsert_stem(
			 const u8code_t (&__arr)[_Num])
			{
				return this->reinsert_stem(pul::begin(__arr), pul::end(__arr));
			}
			template<size_t _Num>
			pf_decl_inline pf_decl_constexpr iterator_t
			reinsert_stem(
			 array<u8char_t, _Num> const &__arr)
			{
				return this->reinsert_stem(__arr.begin(), __arr.end());
			}
			template<size_t _Num>
			pf_decl_inline pf_decl_constexpr iterator_t
			reinsert_stem(
			 array<u8code_t, _Num> const &__arr)
			{
				return this->reinsert_stem(__arr.begin(), __arr.end());
			}
			pf_decl_inline pf_decl_constexpr iterator_t
			reinsert_stem(
			 const u8char_t *__str,
			 size_t __len)
			{
				return this->reinsert_stem(iterator(__str), iterator(__str + __len));
			}
			pf_decl_inline pf_decl_constexpr iterator_t
			reinsert_stem(
			 const u8char_t *__str)
			{
				return this->reinsert_stem(iterator(__str), iterator(__str + utf8::lenof(__str) - 1));
			}
		};




		/// FILESYSTEM: Enum -> Attribute
		enum class attribute : uint32_t
		{
			none			 = 0x00'00,
			dont_exist = 0x00'01,
			readonly	 = 0x00'02,
			hidden		 = 0x00'04,
			system		 = 0x00'08,
			directory	 = 0x00'10,
			archive		 = 0x00'20,
			temporary	 = 0x00'40
		};

		/// FILESYSTEM: Enum -> Attribute -> Type
		class attribute_t
		{
		public:
			/// Constructors
			pf_decl_inline
			attribute_t() pf_attr_noexcept = default;
			pf_decl_inline
			attribute_t(
			 attribute __attributes) pf_attr_noexcept
				: attributes_(__attributes)
			{}
			pf_decl_inline
			attribute_t(
			 attribute_t const &) pf_attr_noexcept = default;

			/// Operator =
			pf_decl_inline attribute_t &
			operator=(attribute_t const &) = default;

			/// Operator |=
			pf_decl_inline attribute_t &
			operator|=(
			 attribute __rattr) pf_attr_noexcept
			{
				this->attributes_ = union_cast<attribute>(union_cast<uint32_t>(this->attributes_) | union_cast<uint32_t>(__rattr));
				return *this;
			}

			/// Operator &=
			pf_decl_inline attribute_t &
			operator&=(
			 attribute __rattr) pf_attr_noexcept
			{
				this->attributes_ = union_cast<attribute>(union_cast<uint32_t>(this->attributes_) & union_cast<uint32_t>(__rattr));
				return *this;
			}

			/// Operator == / !=
			pf_hint_nodiscard pf_decl_inline bool
			operator==(
			 attribute __rattr) const pf_attr_noexcept
			{
				return union_cast<uint32_t>(this->attributes_) == union_cast<uint32_t>(__rattr);
			}
			pf_hint_nodiscard pf_decl_inline bool
			operator!=(
			 attribute __rattr) const pf_attr_noexcept
			{
				return union_cast<uint32_t>(this->attributes_) != union_cast<uint32_t>(__rattr);
			}

			/// Operator bool()
			pf_hint_nodiscard pf_decl_inline
			operator bool() const pf_attr_noexcept
			{
				return union_cast<uint32_t>(this->attributes_);
			}

			/// Operator attribute()
			pf_hint_nodiscard pf_decl_inline
			operator attribute() const pf_attr_noexcept
			{
				return this->attributes_;
			}

		private:
			attribute attributes_;
		};

		/// Operator |
		pf_hint_nodiscard pf_decl_inline attribute_t
		operator|(
		 attribute __lopt,
		 attribute __ropt) pf_attr_noexcept
		{
			attribute_t tmp(__lopt);
			return tmp |= __ropt;
		}

		/// Operator &
		pf_hint_nodiscard pf_decl_inline attribute_t
		operator&(
		 attribute __lopt,
		 attribute __ropt) pf_attr_noexcept
		{
			attribute_t tmp(__lopt);
			return tmp &= __ropt;
		}

		/// FILESYSTEM: Type -> File Info
		struct file_info_t
		{
			tm_t creationTime;
			tm_t lastAccessTime;
			tm_t lastWriteTime;
			uint64_t fileSize;
			attribute_t attributes;
		};

		/// FILESYSTEM: Enum -> Permission
		enum class perm : uint32_t
		{
			none				= 0x00'00,
			owner_read	= 0x00'01,
			owner_write = 0x00'02,
			owner_exec	= 0x00'04,
			owner_all		= owner_read | owner_write | owner_exec,
			users_read	= 0x00'08,
			users_write = 0x00'10,
			users_exec	= 0x00'20,
			users_all		= users_read | users_write | users_exec,
			guest_read	= 0x00'40,
			guest_write = 0x00'80,
			guest_exec	= 0x01'00,
			guest_all		= guest_read | guest_write | guest_exec,
			all					= owner_all | users_all | guest_all,
		};
		enum class perm_opt : uint32_t
		{
			add,
			replace,
			remove
		};
		using perm_opt_t = perm_opt;

		/// FILESYSTEM: Enum -> Permission Type
		class perm_t
		{
		public:
			/// Constructors
			pf_decl_inline
			perm_t() pf_attr_noexcept = default;
			pf_decl_inline
			perm_t(
			 perm __perms) pf_attr_noexcept
				: perms_(__perms)
			{}
			pf_decl_inline
			perm_t(
			 perm_t const &) pf_attr_noexcept = default;

			/// Operator =
			pf_decl_inline perm_t &
			operator=(perm_t const &) pf_attr_noexcept = default;

			/// Operator |=
			pf_decl_inline perm_t &
			operator|=(
			 perm_t __ropt) pf_attr_noexcept
			{
				this->perms_ = union_cast<perm>(union_cast<uint32_t>(this->perms_) | union_cast<uint32_t>(__ropt.perms_));
				return *this;
			}

			/// Operator &=
			pf_decl_inline perm_t &
			operator&=(
			 perm_t __ropt) pf_attr_noexcept
			{
				this->perms_ = union_cast<perm>(union_cast<uint32_t>(this->perms_) & union_cast<uint32_t>(__ropt.perms_));
				return *this;
			}

			/// Operator ~
			pf_decl_inline perm_t
			operator~() const pf_attr_noexcept
			{
				return union_cast<perm>(~union_cast<uint32_t>(this->perms_));
			}

			/// Operator == / !=
			pf_hint_nodiscard pf_decl_inline bool
			operator==(
			 perm_t __ropt) const pf_attr_noexcept
			{
				return union_cast<uint32_t>(this->perms_) == union_cast<uint32_t>(__ropt.perms_);
			}
			pf_hint_nodiscard pf_decl_inline bool
			operator!=(
			 perm_t __ropt) const pf_attr_noexcept
			{
				return union_cast<uint32_t>(this->perms_) != union_cast<uint32_t>(__ropt.perms_);
			}

			/// Operator bool()
			pf_hint_nodiscard pf_decl_inline
			operator bool()
			{
				return union_cast<uint32_t>(this->perms_);
			}

			/// Operator perm()
			pf_hint_nodiscard pf_decl_inline
			operator perm() const pf_attr_noexcept
			{
				return this->perms_;
			}

		private:
			perm perms_;
		};

		/// Operator |
		pf_hint_nodiscard pf_decl_inline perm_t
		operator|(
		 perm __lopt,
		 perm __ropt) pf_attr_noexcept
		{
			return perm_t(__lopt) |= __ropt;
		}

		/// Operator &
		pf_hint_nodiscard pf_decl_inline perm_t
		operator&(
		 perm __lopt,
		 perm __ropt) pf_attr_noexcept
		{
			return perm_t(__lopt) &= __ropt;
		}


		/// FILESYSTEM: Enum -> Operation
		enum class operation
		{
			replace,
			add,
			remove
		};


		// Pathing
		pulsar_api pf_hint_nodiscard path<>
		absolute(
		 path_view __v);
		pulsar_api pf_hint_nodiscard path<>
		relative(
		 path_view __v);
		pf_hint_nodiscard pf_decl_inline path<>
		parents(
		 path_view __v)
		{
			path p = absolute(__v);
			p.remove_filename();
			return p;
		}
		pulsar_api pf_hint_nodiscard attribute_t
		attributes(
		 path_view __v);

		pulsar_api pf_hint_nodiscard file_info_t
		information(
		 path_view __v);

		// Other
		pf_hint_nodiscard pf_decl_inline bool
		has_parent(
		 path_view __v)
		{
			path p = absolute(__v);
			if(p.is_empty()) return false;
			for(auto rb = p.rbegin(), re = p.rend(); rb != re; ++rb)
			{
				if(*rb == '/') return rb != p.begin();
			}
			return false;
		}
		pf_hint_nodiscard pf_decl_inline bool
		has_extension(
		 path_view __v)
		{
			if(__v.is_empty()) return false;
			auto rb = __v.rbegin(), re = __v.rend();
			++rb;
			if(rb == re) return false;
			for(; rb != re && *rb != '/'; ++rb)
			{
				if(*rb == '.') return true;
			}
			return false;
		}
		pf_hint_nodiscard pf_decl_inline bool
		has_filename(
		 path_view __v)
		{
			if(__v.is_empty()) return false;
			auto rb = __v.rbegin();
			if(*rb == '/') return false;
			return true;
		}
		pf_hint_nodiscard pf_decl_inline bool
		has_stem(
		 path_view __v)
		{
			bool dotfound = false;
			auto rb = __v.rbegin(), re = __v.rend();
			auto r = rb;
			for(; r != re && *r != '/'; ++r)
			{
				if(*r == '.')
				{
					dotfound = true;
					break;
				}
			}
			if(dotfound)
			{
				// Look for /
				++r;
				if(r == re || *r == '/') return false;
				++r;
				for(; r != re; ++r)
				{
					if(*r == '/') return true;
				}
				return false;
			}
			return r != rb;
		}

		pf_hint_nodiscard pf_decl_inline path_view
		extension_of(
		 path_view __v)
		{
			for(auto rb = __v.rbegin(), re = __v.rend(); rb != re && *rb != '/'; ++rb)
			{
				if(*rb == '.') return { --rb, distof(rb.get(), __v.end().get()) };
			}
			return { __v.end().get(), 0 };
		}
		pf_hint_nodiscard pf_decl_inline path_view
		filename_of(
		 path_view __v)
		{
			bool dotfound = false;
			auto rb = __v.rbegin(), re = __v.rend();
			for(; rb != re && *rb != '/'; ++rb)
			{
				if(*rb == '.')
				{
					dotfound = true;
					break;
				}
			}
			if(dotfound)
			{
				++rb;
				if(rb == re || *rb == '/') return { __v.end().get(), 0 };
				++rb;
				for(; rb != re; ++rb)
				{
					if(*rb == '/') return { --rb, distof(rb.get(), re.get()) };
				}
				return { __v.begin().get(), __v.length() - 1 };
			}
			if(rb == re) return { __v.end().get(), 0 };
			return { rb.get(), distof(rb.get(), __v.end().get()) };
		}
		pf_hint_nodiscard pf_decl_inline path_view
		stem_of(
		 path_view __v)
		{
			bool dotfound = false;
			auto rb = __v.rbegin(), re = __v.rend();
			for(; rb != re && *rb != '/'; ++rb)
			{
				if(*rb == '.')
				{
					dotfound = true;
					break;
				}
			}
			if(!dotfound) return { __v.begin().get(), 0 };
			++rb;
			auto e = rb;
			if(rb == re || *rb == '/') return { __v.begin().get(), 0 };
			for(; rb != re; ++rb)
			{
				if(*rb == '/') return { --rb, distof(rb, e) };
			}
			return { __v.begin().get(), distof(__v.begin(), e) };
		}


		/// FILESYSTEM: Drives
		class drive_enumeration
		{
		private:
			pf_decl_friend drive_enumeration
			drive_enumerate();

			/// Constructors
			pf_decl_inline
			drive_enumeration(
			 u8char_t *__data,
			 size_t __totallen) pf_attr_noexcept
				: data_(__data)
				, totallen_(__totallen)
			{}
			drive_enumeration(drive_enumeration const &) = delete;
			drive_enumeration(drive_enumeration &&)			 = delete;

		public:
			/// Destructor
			pf_decl_inline ~drive_enumeration() pf_attr_noexcept
			{
				destroy_delete_s<u8char_t[]>(this->data_);
			}

			/// Operator =
			drive_enumeration &
			operator=(drive_enumeration const &) = delete;
			drive_enumeration &
			operator=(drive_enumeration &&) = delete;

			/// Iterator
			struct iterator_t
			{
			public:
				/// Constructors
				pf_decl_inline
				iterator_t() pf_attr_noexcept = default;
				pf_decl_inline
				iterator_t(
				 const u8char_t *__data,
				 size_t __len)
					: data_(__data)
					, len_(__len)
				{}
				pf_decl_inline
				iterator_t(
				 iterator_t const &) = default;

				/// Destructor
				pf_decl_inline ~iterator_t() pf_attr_noexcept = default;

				/// Operator =
				pf_decl_inline iterator_t &
				operator=(iterator_t const &) = default;

				/// Operator ++
				pf_decl_inline iterator_t &
				operator++() pf_attr_noexcept
				{
					this->data_ += this->len_;
					return *this;
				}

				/// Operator ==
				pf_hint_nodiscard pf_decl_inline bool
				operator==(
				 iterator_t const &__it) pf_attr_noexcept
				{
					return this->data_ == __it.data_;
				}
				pf_hint_nodiscard pf_decl_inline bool
				operator!=(
				 iterator_t const &__it) pf_attr_noexcept
				{
					return this->data_ != __it.data_;
				}

				/// Operator *
				pf_hint_nodiscard pf_decl_inline path_view
				operator*() pf_attr_noexcept
				{
					return path_view { this->data_, this->len_ - 1 };
				}

			private:
				const u8char_t *data_;
				size_t len_;
			};

			/// Begin
			pf_hint_nodiscard iterator_t
			begin() const pf_attr_noexcept
			{
				return iterator_t(this->data_, utf8::lenof(this->data_));
			}

			/// End
			pf_hint_nodiscard iterator_t
			end() const pf_attr_noexcept
			{
				return iterator_t(this->data_ + this->totallen_, 0);
			}

		private:
			/// Store
			u8char_t *data_;
			size_t totallen_;
		};
		enum class drive_type
		{
			unknown,
			invalid,
			removable,
			fixed,
			remote,
			cdrom,
			ramdisk
		};

		/// FILESYSTEM: Drive
		pf_hint_nodiscard pulsar_api drive_enumeration
		drive_enumerate();
		pf_hint_nodiscard pulsar_api drive_type
		drive_get_type(
		 path_view __path);
		pf_hint_nodiscard pulsar_api path<>
		drive_by_path(
		 path_view __path);


		/// FILESYSTEM: Enum -> Copy Options
		enum class copy_option : uint32_t
		{
			skip_existing			 = 0x00'01,
			update_existing		 = 0x00'02,
			recursive					 = 0x00'04,
			overwrite_existing = 0x00'08,
			directories_only	 = 0x00'10,
			copy_symlinks			 = 0x00'20,				 // copy symlink as symlink
			skip_symlinks			 = 0x00'40,				 // skip symlinks
			create_symlinks		 = copy_symlinks,	 // symlink => file, symlink => directory
			create_hard_links	 = 0x00'80				 // hardlink => file, junction => directory
		};

		/// FILESYSTEM: Enum -> Copy Options Type
		class copy_option_t
		{
		public:
			/// Constructors
			pf_decl_inline
			copy_option_t() pf_attr_noexcept = default;
			pf_decl_inline
			copy_option_t(
			 copy_option __options)
				: options_(__options)
			{}
			pf_decl_inline
			copy_option_t(
			 copy_option_t const &) = default;

			/// Operator =
			pf_decl_inline copy_option_t &
			operator=(copy_option_t const &) = default;

			/// Operator |=
			pf_hint_nodiscard pf_decl_inline copy_option_t &
			operator|=(
			 copy_option_t __ropt) pf_attr_noexcept
			{
				this->options_ = union_cast<copy_option>(union_cast<uint32_t>(this->options_) | union_cast<uint32_t>(__ropt.options_));
				return *this;
			}

			/// Operator &=
			pf_hint_nodiscard pf_decl_inline copy_option_t &
			operator&=(
			 copy_option_t __ropt) pf_attr_noexcept
			{
				this->options_ = union_cast<copy_option>(union_cast<uint32_t>(this->options_) & union_cast<uint32_t>(__ropt.options_));
				return *this;
			}

			/// Operator == / !=
			pf_hint_nodiscard pf_decl_inline bool
			operator==(
			 copy_option_t __ropt) const pf_attr_noexcept
			{
				return union_cast<uint32_t>(this->options_) == union_cast<uint32_t>(__ropt.options_);
			}
			pf_hint_nodiscard pf_decl_inline bool
			operator!=(
			 copy_option_t __ropt) const pf_attr_noexcept
			{
				return union_cast<uint32_t>(this->options_) != union_cast<uint32_t>(__ropt.options_);
			}

			/// Operator bool
			pf_hint_nodiscard pf_decl_inline
			operator bool() const pf_attr_noexcept
			{
				return union_cast<uint32_t>(this->options_);
			}

			/// Operator copy_option()
			pf_hint_nodiscard pf_decl_inline
			operator copy_option() const pf_attr_noexcept
			{
				return this->options_;
			}

		private:
			copy_option options_;
		};

		/// Operator |
		pf_hint_nodiscard pf_decl_inline copy_option_t
		operator|(
		 copy_option __lopt,
		 copy_option __ropt) pf_attr_noexcept
		{
			return copy_option_t(__lopt) |= __ropt;
		}

		/// Operator &
		pf_hint_nodiscard pf_decl_inline copy_option_t
		operator&(
		 copy_option __lopt,
		 copy_option __ropt) pf_attr_noexcept
		{
			return copy_option_t(__lopt) &= __ropt;
		}

		/// FILESYSTEM: Enum -> Known
		enum class known
		{
			process_root,
			shared,
			local,
			user,
			temporary
		};
		using known_t = known;

		/// FILESYSTEM: Directory
		pf_hint_nodiscard pulsar_api size_t
		directory_size(
		 path_view __p);
		pf_hint_nodiscard pulsar_api bool
		directory_is_empty(
		 path_view __p);
		pulsar_api void
		directory_create(
		 path_view __p,
		 perm_t __perms);
		pulsar_api void
		directory_create_rec(
		 path_view __p,
		 perm_t __perms);
		pulsar_api void
		directory_copy(
		 path_view __from,
		 path_view __dest,
		 copy_option_t __options,
		 perm_t __perms);
		pulsar_api void
		directory_remove(
		 path_view __p);
		pulsar_api void
		directory_create_symlink(
		 path_view __link,
		 path_view __target);
		pulsar_api void
		directory_copy_symlink(
		 path_view __from,
		 path_view __dest,
		 bool __overwrite = true);
		pulsar_api void
		directory_create_hardlink(
		 path_view __link,
		 path_view __target);
		pf_hint_nodiscard pulsar_api path<>
		directory_known(
		 known_t __known);
		pulsar_api void
		set_current_directory(
		 path_view __p);


		/// FILESYSTEM: Enum -> Mode
		enum class io_mode
		{
			open,
			overwrite,
			truncate
		};

		/// FILESYSTEM: Enum -> Mode Type
		using io_mode_t = io_mode;

		/// FILESYSTEM: Enum -> Access
		enum class io_access
		{
			none								= 0x00'00,
			read								= 0x00'01,
			write								= 0x00'02,
			execute							= 0x00'04,
			shared_read					= 0x00'08,
			shared_write				= 0x00'10,
			random_access_cache = 0x00'20	 // against sequential_access_cache (.json vs .jpeg)
		};

		/// FILESYSTEM: Enum -> Mode Type
		class io_access_t
		{
		public:
			/// Constructors
			pf_decl_inline
			io_access_t() pf_attr_noexcept = default;
			pf_decl_inline
			io_access_t(
			 io_access __access) pf_attr_noexcept
				: access_(__access)
			{}
			io_access_t(io_access_t const &) = default;

			/// Destructor
			pf_decl_inline ~io_access_t() pf_attr_noexcept = default;

			/// Operator =
			pf_decl_inline io_access_t &
			operator=(io_access_t const &) = default;

			/// Operator |=
			pf_decl_inline io_access_t &
			operator|=(
			 io_access_t __rhint) pf_attr_noexcept
			{
				this->access_ = union_cast<io_access>(union_cast<uint32_t>(this->access_) | union_cast<uint32_t>(__rhint.access_));
				return *this;
			}

			/// Operator &=
			pf_decl_inline io_access_t &
			operator&=(
			 io_access_t __rhint) pf_attr_noexcept
			{
				this->access_ = union_cast<io_access>(union_cast<uint32_t>(this->access_) & union_cast<uint32_t>(__rhint.access_));
				return *this;
			}

			/// Operator == / !=
			pf_hint_nodiscard pf_decl_inline bool
			operator==(
			 io_access_t __rhint) const pf_attr_noexcept
			{
				return this->access_ == __rhint.access_;
			}
			pf_hint_nodiscard pf_decl_inline bool
			operator!=(
			 io_access_t __rhint) const pf_attr_noexcept
			{
				return this->access_ != __rhint.access_;
			}

			/// Operator bool()
			pf_hint_nodiscard pf_decl_inline
			operator bool() const pf_attr_noexcept
			{
				return union_cast<uint32_t>(this->access_);
			}

			/// Operator io_access()
			pf_hint_nodiscard pf_decl_inline
			operator io_access() const pf_attr_noexcept
			{
				return this->access_;
			}

		private:
			io_access access_;
		};

		// Operator |
		pf_hint_nodiscard pf_decl_inline io_access_t
		operator|(
		 io_access __lhint,
		 io_access __rhint) pf_attr_noexcept
		{
			return io_access_t(__lhint) |= __rhint;
		}

		// Operator &
		pf_hint_nodiscard pf_decl_inline io_access_t
		operator&(
		 io_access __lhint,
		 io_access __rhint) pf_attr_noexcept
		{
			return io_access_t(__lhint) &= __rhint;
		}


		/// FILESYSTEM: File
		/// Enum -> Pointer Mode
		enum class ptr_mode
		{
			begin,
			current,
			end
		};

		/// Enum -> Pointer Mode Type
		using ptr_mode_t = ptr_mode;

		/// Enum -> Lock Mode
		enum class lock_mode
		{
			exclusive,
			shared
		};

		/// Enum -> Lock Mode Type
		using lock_mode_t = lock_mode;

		// Using pathing
		pulsar_api void
		file_create(
		 path_view __p,
		 perm_t __perms);
		pulsar_api void
		file_remove(
		 path_view __p);
		pulsar_api void
		file_copy(
		 path_view __from,
		 path_view __dest,
		 bool __overwrite = true);
		pulsar_api void
		file_create_symlink(
		 path_view __link,
		 path_view __target);
		pulsar_api void
		file_copy_symlink(
		 path_view __from,
		 path_view __dest,
		 bool __overwrite = true);
		pulsar_api void
		file_create_hardlink(
		 path_view __link,
		 path_view __target,
		 perm_t __perms);


		/// FILESYSTEM: Common
		pf_hint_nodiscard pulsar_api perm_t
		perms(
		 path_view __p);
		pf_hint_nodiscard pulsar_api perm_t
		symlink_perms(
		 path_view __link);
		pulsar_api perm_t
		set_perms(
		 path_view __p,
		 perm_t __perms,
		 perm_opt_t __opt = perm_opt::replace);
		pulsar_api perm_t
		set_symlink_perms(
		 path_view __link,
		 perm_t __perms,
		 perm_opt_t __opt = perm_opt::replace);
		pulsar_api void
		move(
		 path_view __from,
		 path_view __dest,
		 bool __overwrite = true);
		pulsar_api void
		copy(
		 path_view __from,
		 path_view __dest,
		 bool __overwrite = true);
		pulsar_api void
		remove(
		 path_view __p);
		pf_hint_nodiscard pulsar_api uint64_t
		size(
		 path_view __p);
		pulsar_api void
		create_symlink(
		 path_view __link,
		 path_view __target);
		pulsar_api void
		copy_symlink(
		 path_view __from,
		 path_view __dest,
		 bool __overwrite = true);
		pulsar_api void
		create_hardlink(
		 path_view __link,
		 path_view __target);

		/// File -> Base
		// Internal
		pulsar_api void
		__file_close(
		 void *__handle) pf_attr_noexcept;
		pulsar_api void
		__file_flush_buffers(
		 void *__handle);
		pf_hint_nodiscard pulsar_api path<>
		__file_get_path(
		 void *__handle);

		// Type -> Base
		class __file_base
		{
			template<lock_mode _LockMode>
			pf_decl_friend class __file_lock_base;

		public:
			/// Constructors
			pf_decl_inline
			__file_base(
			 void *__handle) pf_attr_noexcept
				: handle_(__handle)
			{}
			__file_base(__file_base const &) = delete;
			pf_decl_inline
			__file_base(__file_base &&__other) pf_attr_noexcept
				: handle_(__other.handle_)
			{
				__other.handle_ = nullptr;
			}

			/// Destructor
			pf_decl_inline ~__file_base() pf_attr_noexcept
			{
				// Close
				this->close();
			}

			/// Operator =
			__file_base &
			operator=(__file_base const &) = delete;
			pf_decl_inline __file_base &
			operator=(__file_base &&__other) pf_attr_noexcept
			{
				if(pf_likely(this != &__other))
				{
					this->close();
					this->handle_		= __other.handle_;
					__other.handle_ = nullptr;
				}
				return *this;
			}

			/// Is Open
			pf_hint_nodiscard pf_decl_inline bool
			is_open() const pf_attr_noexcept
			{
				return this->handle_;
			}

			/// Path
			pf_hint_nodiscard pf_decl_inline path<>
			path() const pf_attr_noexcept
			{
				return __file_get_path(this->handle_);
			}

			/// Close
			pf_decl_inline void
			close() pf_attr_noexcept
			{
				if(this->handle_)
				{
					__file_close(this->handle_);
					this->handle_ = nullptr;
				}
			}

			/// Flush
			pf_decl_inline void
			flush_buffers()
			{
				return __file_flush_buffers(this->handle_);
			}

		protected:
			void *handle_;
		};

		/// Lock -> Internal
		pulsar_api void
		__file_do_lock(
		 void *__handle,
		 lock_mode __mode,
		 uint64_t __where,
		 uint64_t __size);
		pulsar_api void
		__file_do_unlock(
		 void *__handle,
		 uint64_t __where,
		 uint64_t __size);

		/// Lock -> Type (Base)
		template<lock_mode _LockMode>
		class __file_lock_base
		{
		public:
			/// Constructors
			pf_decl_inline
			__file_lock_base(
			 __file_base &__file,
			 uint64_t __where,
			 uint64_t __size)
				: file_(&__file)
				, where_(__where)
				, size_(__size)
			{
				__file_do_lock(__file.handle_, _LockMode, __where, __size);
			}
			__file_lock_base(__file_lock_base const &) = delete;
			pf_decl_inline
			__file_lock_base(
			 __file_lock_base &&__other) pf_attr_noexcept
				: file_(__other.handle_)
				, where_(__other.where_)
				, size_(__other.size_)
			{
				__other.where_ = uint64_t(-1);
				__other.size_	 = 0;
			}

			/// Destructor
			pf_decl_inline ~__file_lock_base() pf_attr_noexcept
			{
				__file_do_unlock(this->file_->handle_, this->where_, this->size_);
			}

			/// Operator =
			__file_lock_base &
			operator=(__file_lock_base const &) = delete;
			pf_decl_inline __file_lock_base &
			operator=(__file_lock_base &&__other) pf_attr_noexcept
			{
				if(pf_likely(this != &__other))
				{
					this->unlock();
					this->handle_	 = __other.handle_;
					this->where_	 = __other.where_;
					__other.where_ = uint64_t(-1);
					this->size_		 = __other.size_;
				}
				return *this;
			}

			/// (Re)Lock
			pf_decl_inline void
			lock(
			 size_t __where,
			 size_t __size)
			{
				if(this->is_locked()) __file_do_unlock(this->file_->handle_, this->where_, this->size_);
				__file_do_lock(this->file_->handle_, _LockMode, __where, __size);
				this->where_ = __where;
				this->size_	 = __size;
			}

			/// Unlock
			pf_decl_inline void
			unlock()
			{
				__file_do_unlock(this->file_->handle_, this->where_, this->size_);
				this->where_ = uint64_t(-1);
			}

			/// Lock
			pf_hint_nodiscard pf_decl_inline bool
			is_locked() const pf_attr_noexcept
			{
				return this->where_ != uint64_t(-1);
			}

		private:
			__file_base *file_;
			uint64_t where_;
			uint64_t size_;
		};

		/// Lock -> Type (Unique)
		using file_lock_unique_t = __file_lock_base<lock_mode::exclusive>;

		/// Lock -> Type (Shared)
		using file_lock_shared_t = __file_lock_base<lock_mode::shared>;

		/// File -> Synchronous
		// Internal
		pf_hint_nodiscard pulsar_api void *
		__file_do_open(
		 path_view __p,
		 io_access_t __access,
		 io_mode_t __mode,
		 bool __async);
		pf_hint_nodiscard pulsar_api void *
		__file_do_reopen(
		 void *__handle,
		 io_access_t __access,
		 bool __async);

		// Internal -> Sync
		pf_hint_nodiscard pulsar_api uint32_t
		__file_sync_do_write(
		 void *__handle,
		 const void *__data,
		 uint32_t __size);
		pf_hint_nodiscard pulsar_api uint32_t
		__file_sync_do_read(
		 void *__handle,
		 void *__data,
		 uint32_t __size);
		pf_hint_nodiscard pulsar_api int64_t
		__file_sync_seek(
		 void *__handle,
		 ptr_mode_t __mode,
		 int64_t __where);
		pulsar_api void
		__file_set_pointer_end_of_file(
		 void *__handle);

		// Type
		class file pf_attr_final : public __file_base
		{
		public:
			/// Constructors
			pf_decl_inline
			file(
			 path_view __p,
			 io_access_t __access,
			 io_mode_t __mode)
				: __file_base(__file_do_open(__p, __access, __mode, false))
			{}
			file(file const &) = delete;
			pf_decl_inline
			file(file &&__file) pf_attr_noexcept = default;

			/// Destructor
			pf_decl_inline ~file() pf_attr_noexcept = default;

			/// Operator =
			file &
			operator=(file const &) = delete;
			pf_decl_inline file &
			operator=(file &&) = default;

			/// Reopen
			pf_decl_inline void
			reopen(
			 io_access __access)
			{
				this->handle_ = __file_do_reopen(this->handle_, __access, false);
			}

			/// IO
			pf_decl_inline uint32_t
			write(
			 const void *__data,
			 uint32_t __size)
			{
				return __file_sync_do_write(this->handle_, __data, __size);
			}
			pf_decl_inline uint32_t
			read(
			 void *__data,
			 uint32_t __size)
			{
				return __file_sync_do_read(this->handle_, __data, __size);
			}

			/// Set Pointer / Get Pointer
			pf_decl_inline int64_t
			seek(
			 ptr_mode_t __mode,
			 int64_t __where)
			{
				return __file_sync_seek(this->handle_, __mode, __where);
			}
			pf_hint_nodiscard pf_decl_inline int64_t
			tell()
			{
				return __file_sync_seek(this->handle_, ptr_mode::current, 0);
			}
			pf_hint_nodiscard pf_decl_inline int64_t
			get_end_of_file()
			{
				return __file_sync_seek(this->handle_, ptr_mode::end, 0);
			}
			pf_decl_inline void
			set_end_of_file()
			{
				return __file_set_pointer_end_of_file(this->handle_);
			}
		};

		/// File -> Asynchronous
		// Callback
		using file_completion_callback_t = fun_ptr<void(uint64_t __where, uint32_t __written, void *__userdata)>;

		// Internal
		pulsar_api void
		__file_future_wait_and_close(
		 void *__handle) pf_attr_noexcept;
		pf_hint_nodiscard pulsar_api bool
		__file_future_is_finished(
		 void *__handle);
		pf_hint_nodiscard pulsar_api bool
		__file_future_wait(
		 void *__handle);
		pf_hint_nodiscard pulsar_api uint32_t
		__file_future_retrieve_written(
		 void *__handle);

		// Future
		class file_future_io_t pf_attr_final
		{
			/// Close
			pf_decl_inline void
			__wait_and_close() pf_attr_noexcept
			{
				if(this->handle_)
				{
					__file_future_wait_and_close(this->handle_);
					this->handle_ = nullptr;
				}
			}

		public:
			/// Constructors
			pf_decl_inline
			file_future_io_t(
			 void *__handle) pf_attr_noexcept
				: handle_(__handle)
			{}
			file_future_io_t(
			 file_future_io_t const &) = delete;
			pf_decl_inline
			file_future_io_t(
			 file_future_io_t &&__other) pf_attr_noexcept
				: handle_(__other.handle_)
			{
				__other.handle_ = nullptr;
			}

			/// Destructor
			pf_decl_inline ~file_future_io_t() pf_attr_noexcept
			{
				this->__wait_and_close();
			}

			/// Operator =
			file_future_io_t &
			operator=(file_future_io_t const &) = delete;
			pf_decl_inline file_future_io_t &
			operator=(file_future_io_t &&__other) pf_attr_noexcept
			{
				if(pf_likely(this != &__other))
				{
					this->__wait_and_close();
					this->handle_ = __other.handle_;
				}
				return *this;
			}

			/// Is Finished
			pf_hint_nodiscard pf_decl_inline bool
			is_finished() const pf_attr_noexcept
			{
				return __file_future_is_finished(this->handle_);
			}

			/// Wait
			pf_decl_inline bool
			wait() const pf_attr_noexcept
			{
				return __file_future_wait(this->handle_);
			}

			/// Written
			pf_hint_nodiscard pf_decl_inline uint32_t
			written() const pf_attr_noexcept
			{
				return __file_future_retrieve_written(this->handle_);
			}

		private:
			void *handle_;
		};

		// Internal
		pf_hint_nodiscard pulsar_api file_future_io_t
		__file_async_do_write_with_future(
		 void *__handle,
		 uint64_t __where,
		 const void *__data,
		 uint32_t __size,
		 file_completion_callback_t __callback,
		 void *__userdata);
		pf_hint_nodiscard pulsar_api file_future_io_t
		__file_async_do_read_with_future(
		 void *__handle,
		 uint64_t __where,
		 void *__data,
		 uint32_t __size,
		 file_completion_callback_t __callback,
		 void *__userdata);
		pulsar_api void
		__file_async_do_write(
		 void *__handle,
		 uint64_t __where,
		 const void *__data,
		 uint32_t __size,
		 file_completion_callback_t __callback,
		 void *__userdata);
		pulsar_api void
		__file_async_do_read(
		 void *__handle,
		 uint64_t __where,
		 void *__data,
		 uint32_t __size,
		 file_completion_callback_t __callback,
		 void *__userdata);

		// Optional parameter
		pf_decl_inline void
		file_no_completion_callback(uint64_t, uint32_t, void *) pf_attr_noexcept
		{}

		// Type
		class async_file pf_attr_final : public __file_base
		{
		public:
			/// Constructors
			pf_decl_inline
			async_file(
			 path_view __p,
			 io_access_t __access,
			 io_mode_t __mode)
				: __file_base(__file_do_open(__p, __access, __mode, true))
			{}
			async_file(async_file const &) = delete;
			pf_decl_inline
			async_file(async_file &&__other) pf_attr_noexcept = default;

			/// Destructor
			pf_decl_inline ~async_file() pf_attr_noexcept = default;

			/// Operator =
			async_file &
			operator=(async_file const &) = delete;
			pf_decl_inline async_file &
			operator=(async_file &&) = default;

			/// Reopen
			pf_decl_inline void
			reopen(
			 io_access_t __access)
			{
				this->handle_ = __file_do_reopen(this->handle_, __access, true);
			}

			/// IO
			pf_hint_nodiscard pf_decl_inline file_future_io_t
			future_write(
			 uint64_t __where,
			 const void *__data,
			 uint32_t __size,
			 file_completion_callback_t __callback = file_no_completion_callback,
			 void *__userdata											 = nullptr)
			{
				return __file_async_do_write_with_future(this->handle_, __where, __data, __size, __callback, __userdata);
			}
			pf_hint_nodiscard pf_decl_inline file_future_io_t
			future_read(
			 uint64_t __where,
			 void *__data,
			 uint32_t __size,
			 file_completion_callback_t __callback = file_no_completion_callback,
			 void *__userdata											 = nullptr)
			{
				return __file_async_do_read_with_future(this->handle_, __where, __data, __size, __callback, __userdata);
			}
			pf_decl_inline void
			write(
			 uint64_t __where,
			 const void *__data,
			 uint32_t __size,
			 file_completion_callback_t __callback = file_no_completion_callback,
			 void *__userdata											 = nullptr)
			{
				return __file_async_do_write(this->handle_, __where, __data, __size, __callback, __userdata);
			}
			pf_decl_inline void
			read(
			 uint64_t __where,
			 void *__data,
			 uint32_t __size,
			 file_completion_callback_t __callback = file_no_completion_callback,
			 void *__userdata											 = nullptr)
			{
				return __file_async_do_read(this->handle_, __where, __data, __size, __callback, __userdata);
			}
		};


		/// FILESYSTEM: Type -> Find Info
		struct find_info_t : file_info_t
		{
			path_view path;
		};

		/// FILESYSTEM: Type -> Entry
		class entry_t
		{
		public:
			/// Constructors
			pf_decl_inline
			entry_t() pf_attr_noexcept = default;
			pf_decl_inline
			entry_t(
			 find_info_t const &__p) pf_attr_noexcept
				: data_(__p)
			{}
			pf_decl_inline
			entry_t(
			 entry_t const &) pf_attr_noexcept = default;

			/// Destructor
			pf_decl_inline ~entry_t() = default;

			/// Operator =
			pf_decl_inline entry_t &
			operator=(entry_t const &) = default;

			/// Time Creation
			pf_hint_nodiscard pf_decl_inline const tm_t &
			time_creation() const pf_attr_noexcept
			{
				return this->data_.creationTime;
			}

			/// Time Last Access
			pf_hint_nodiscard pf_decl_inline const tm_t &
			time_last_access() const pf_attr_noexcept
			{
				return this->data_.lastAccessTime;
			}

			/// Time Last Write
			pf_hint_nodiscard pf_decl_inline const tm_t &
			time_last_write() const pf_attr_noexcept
			{
				return this->data_.lastWriteTime;
			}

			/// Size
			pf_hint_nodiscard pf_decl_inline uint64_t
			size() const pf_attr_noexcept
			{
				if(this->data_.attributes & attribute::directory)
				{
					return directory_size(this->data_.path);
				}
				return this->data_.fileSize;
			}

			/// Attribs
			pf_hint_nodiscard pf_decl_inline attribute_t
			attributes() const pf_attr_noexcept
			{
				return this->data_.attributes;
			}

			/// Path
			pf_hint_nodiscard pf_decl_inline path_view
			path() const pf_attr_noexcept
			{
				return this->data_.path;
			}

		private:
			find_info_t data_;
		};


		/// FILESYSTEM: Directory -> Iteration
		// Internal
		pulsar_api void *
		__iterate_initialize(
		 path_view __v,
		 entry_t &__entry);
		pulsar_api void *
		__iterate_next(
		 void *__ctrl,
		 entry_t &__entry);
		pulsar_api void
		__iterate_close(
		 void *__ctrl) pf_attr_noexcept;

		// Iterator
		class directory_iterator_t
		{
			pf_decl_friend class directory_viewer;

		public:
			/// Constructors
			pf_decl_inline
			directory_iterator_t() pf_attr_noexcept = default;
			pf_decl_inline
			directory_iterator_t(
			 void *__ctrl)
				: ctrl_(__ctrl)
			{}
			pf_decl_inline
			directory_iterator_t(
			 void *__ctrl,
			 entry_t const &__entry)
				: ctrl_(__ctrl)
				, entry_(__entry)
			{}
			pf_decl_inline
			directory_iterator_t(
			 directory_iterator_t const &) = default;

			/// Destructor
			pf_decl_inline ~directory_iterator_t() pf_attr_noexcept
			{
				if(pf_likely(this->ctrl_)) __iterate_close(this->ctrl_);
			}

			/// Operator ++
			pf_decl_inline directory_iterator_t &
			operator++() pf_attr_noexcept
			{
				this->ctrl_ = __iterate_next(this->ctrl_, this->entry_);
				return *this;
			}

			/// Operator *
			pf_hint_nodiscard pf_decl_inline const entry_t &
			operator*() const pf_attr_noexcept
			{
				return this->entry_;
			}

			/// Operator ->
			pf_hint_nodiscard pf_decl_inline const entry_t *
			operator->() const pf_attr_noexcept
			{
				return &this->entry_;
			}

			/// Operator ==
			pf_hint_nodiscard pf_decl_inline bool
			operator==(
			 directory_iterator_t const &__r) const pf_attr_noexcept
			{
				return this->ctrl_ == __r.ctrl_;
			}
			pf_hint_nodiscard pf_decl_inline bool
			operator!=(
			 directory_iterator_t const &__r) const pf_attr_noexcept
			{
				return this->ctrl_ != __r.ctrl_;
			}

		private:
			void *ctrl_;
			entry_t entry_;
		};

		// Iterable
		class directory_viewer
		{
		public:
			/// Constructor
			pf_decl_inline
			directory_viewer(
			 path_view __v)
				: path_(__v)
			{
				attribute_t attrs = attributes(__v);
				pf_throw_if(
				 attrs == attribute::dont_exist,
				 dbg_category_generic(),
				 dbg_code::runtime_error,
				 0,
				 "Could'nt iterate through inexisting directory at path={}",
				 __v.data());
				pf_throw_if(
				 !(attrs & attribute::directory),
				 dbg_category_generic(),
				 dbg_code::runtime_error,
				 0,
				 "Trying to iterate at path={} that isn't a directory!",
				 __v.data());
			}
			directory_viewer(directory_viewer const &) = delete;
			directory_viewer(directory_viewer &&)			 = delete;

			/// Destructor
			pf_decl_inline ~directory_viewer() pf_attr_noexcept = default;

			/// Operator =
			directory_viewer &
			operator=(directory_viewer const &) = delete;
			directory_viewer &
			operator=(directory_viewer &&) = delete;

			/// Begin
			pf_hint_nodiscard pf_decl_inline directory_iterator_t
			begin() const
			{
				directory_iterator_t it;
				it.ctrl_ = __iterate_initialize(this->path_, it.entry_);
				return it;
			}

			/// End
			pf_hint_nodiscard pf_decl_inline directory_iterator_t
			end() const pf_attr_noexcept
			{
				return directory_iterator_t(nullptr);
			}

		private:
			path_view path_;
		};

		// Maker
		pf_hint_nodiscard pf_decl_inline directory_viewer
		make_dir_viewer(
		 path_view __v)
		{
			return directory_viewer(__v);
		}


		/// FILESYSTEM: Iterator -> Recursive
		// Internal
		pulsar_api void *
		__iterate_rec_initialize(
		 path_view __v,
		 entry_t &__entry);
		pulsar_api void *
		__iterate_rec_next(
		 void *__ctrl,
		 entry_t &__entry);
		pulsar_api void
		__iterate_rec_close(
		 void *__ctrl) pf_attr_noexcept;

		// Iterator
		class directory_rec_iterator_t
		{
			pf_decl_friend class directory_rec_viewer;

		public:
			/// Constructors
			pf_decl_inline
			directory_rec_iterator_t() pf_attr_noexcept = default;
			pf_decl_inline
			directory_rec_iterator_t(
			 void *__ctrl)
				: ctrl_(__ctrl)
			{}
			pf_decl_inline
			directory_rec_iterator_t(
			 void *__ctrl,
			 entry_t const &__entry)
				: ctrl_(__ctrl)
				, entry_(__entry)
			{}

			/// Destructor
			pf_decl_inline ~directory_rec_iterator_t() pf_attr_noexcept
			{
				if(pf_likely(this->ctrl_)) __iterate_rec_close(this->ctrl_);
			}

			/// Operator ++
			pf_decl_inline directory_rec_iterator_t &
			operator++() pf_attr_noexcept
			{
				this->ctrl_ = __iterate_rec_next(this->ctrl_, this->entry_);
				return *this;
			}

			/// Operator *
			pf_hint_nodiscard pf_decl_inline const entry_t &
			operator*() const pf_attr_noexcept
			{
				return this->entry_;
			}

			/// Operator ->
			pf_hint_nodiscard pf_decl_inline const entry_t *
			operator->() const pf_attr_noexcept
			{
				return &this->entry_;
			}

			/// Operator ==
			pf_hint_nodiscard pf_decl_inline bool
			operator==(
			 directory_rec_iterator_t const &__r) const pf_attr_noexcept
			{
				return this->ctrl_ == __r.ctrl_;
			}
			pf_hint_nodiscard pf_decl_inline bool
			operator!=(
			 directory_rec_iterator_t const &__r) const pf_attr_noexcept
			{
				return this->ctrl_ != __r.ctrl_;
			}

		private:
			void *ctrl_;
			entry_t entry_;
		};

		// Iterable
		class directory_rec_viewer
		{
		public:
			/// Constructor
			pf_decl_inline
			directory_rec_viewer(
			 path_view __v)
				: path_(__v)
			{
				attribute_t attrs = attributes(__v);
				pf_throw_if(
				 attrs == attribute::dont_exist,
				 dbg_category_generic(),
				 dbg_code::runtime_error,
				 0,
				 "Could'nt recursively iterate through inexisting directory at path={}",
				 __v.data());
				pf_throw_if(
				 !(attrs & attribute::directory),
				 dbg_category_generic(),
				 dbg_code::runtime_error,
				 0,
				 "Trying to recursively iterate at path={} that isn't a directory!",
				 __v.data());
			}
			directory_rec_viewer(directory_rec_viewer const &) = delete;
			directory_rec_viewer(directory_rec_viewer &&)			 = delete;

			/// Destructor
			pf_decl_inline ~directory_rec_viewer() pf_attr_noexcept = default;

			/// Operator =
			directory_rec_viewer &
			operator=(directory_rec_viewer const &) = delete;
			directory_rec_viewer &
			operator=(directory_rec_viewer &&) = delete;

			/// Begin
			pf_hint_nodiscard pf_decl_inline directory_rec_iterator_t
			begin() const pf_attr_noexcept
			{
				directory_rec_iterator_t it;
				it.ctrl_ = __iterate_rec_initialize(this->path_, it.entry_);
				return it;
			}

			/// End
			pf_hint_nodiscard pf_decl_inline directory_rec_iterator_t
			end() const pf_attr_noexcept
			{
				return directory_rec_iterator_t(nullptr);
			}

		private:
			path_view path_;
		};

		// Maker
		pf_hint_nodiscard pf_decl_inline directory_rec_viewer
		make_rec_dir_viewer(
		 path_view __v)
		{
			return directory_rec_viewer(__v);
		}


		/// FILESYSTEM: Archive

		// NOTE: Archive Structure
		// Headers ...
		// Skippers ...
		// Binaries ...

		// NOTE: Archive File Manipulators
		// .pa (Pulsar Archive)
		// .pam (Pulsar Archive Maker)
		// .pas (Pulsar Archive State)
		// .pac (Pulsar Archive Comparator)
		// .pau (Pulsar Archive Updator)

		// NOTE: Archive Making
		// 1. Create .pam (pulsar archive maker)
		// 2. Add files, inputs then destination for each files to make
		// 3. Write the archive (.pa) headers synchronously, then binary files asynchronously

		// NOTE: Archive comparator
		// 1. Create .pas (Pulsar archive state)
		// 2. Specify archive to compare from
		// 3. Fill .pas with archive information
		// 4. Specify archive to compare with
		// 5. Fill another .pas with archive information
		// 6. Create .pac (Pulsar archive comparator) that specify what's new, modificated, removed.

		// NOTE: Archive Updating
		// 1. Create .pau (pulsar archive updator) / check if it already exist (asynchronously add files into it)
		// 2. Add files, that specify operation (update / create / remove...) and io (input files, output in archive)
		// 3. Rewrite entire headers and skipers, beginning from the end, add "holes" for new files (asynchronously?)
		// 4. Asynchronously write to .pa using .paupd, using lock on .pa and .paupd
		// 5. Delete .pau

		// Archive -> Constants
		pf_decl_constexpr uint32_t ARCHIVE_VERSION							 = 100;
		pf_decl_constexpr uint32_t ARCHIVE_IDENTIFIER						 = 0xc0'c4'fc'59;
		pf_decl_constexpr uint32_t ARCHIVE_IDENTIFIER_STATE			 = 0xd0'e4'4a'c1;
		pf_decl_constexpr uint32_t ARCHIVE_IDENTIFIER_COMPARATOR = 0x65'78'37'dc;
		pf_decl_constexpr uint32_t ARCHIVE_IDENTIFIER_UPDATOR		 = 0x5b'8c'e0'48;


		// Archive -> Algorithm -> Enum
		enum class archive_compression_algorithm : uint32_t
		{
			none,
			lz4,
			lz4_hc
		};

		// Archive -> Algorithm -> Enum Type
		using archive_compression_algorithm_t = archive_compression_algorithm;

		// Archive -> Headers
		struct __archive_header_t
		{
			uint32_t identifier = ARCHIVE_IDENTIFIER;
			uint32_t version		= ARCHIVE_VERSION;
			// SHA-3 hashvalue, identifying current file integrity
			archive_compression_algorithm_t compAlg;	// default = lz4
			uint32_t binarySize;											// Complete directory size <=> next
			uint32_t count;														// Number of entries inside this directory
		};
		struct __archive_header_directory_t
		{
			uint64_t binarySize;	// Complete directory size <=> next
			uint64_t parent;			// Parent directory position
			uint64_t prev;				// Previous Directory (== 0, no previous directory)
			uint32_t count;				// Number of entries inside this directory
			uint8_t nameLen;			// Length of name
			u8char_t name[];			// Name data
		};
		struct __archive_header_file_t
		{
			// uint256_t hashVal;				// Hash of the file
			uint64_t time;						// File time
			uint64_t binaryPosition;	// Position in the archive of the file
			uint64_t binarySize;			// Size of the b
			uint64_t directory;				// Directory offset
			uint64_t prev;						// Previous File (== 0, no previous file)
			uint32_t index;						// Index in directory
			uint8_t nameLen;					// Length of name
			u8char_t name[];					// Name data
		};


		// Archive -> Skippers
		struct __archive_skipper_t	// More efficient than trees
		{
			u8char_t val;
			uint32_t skip;				// = 0, no skip  => skip to next layer
			uint32_t child;				// = 0, no child => skip to next child
			uint32_t type	 : 1;		// directory or file?
			uint32_t entry : 31;	// Entry position
		};
		struct __archive_skipper_layer_t
		{
			size_t count;										 // Number of skipper in for 'character' value (dichotomic search)
			__archive_skipper_t skippers[];	 // Skippers
		};


		// Archive -> Iterator
		struct __archive_iterator_t
		{
		private:
			__archive_header_directory_t *dir;
			__archive_header_file_t *cur;
		};
		struct __archive_iterator_rec_t
		{
		private:
			__archive_header_directory_t *dir;
			__archive_header_file_t *cur;
		};

		// Archive -> State -> Header
		struct __archive_state_header_t
		{
			uint32_t identifier = ARCHIVE_IDENTIFIER_STATE;
		};

		// Archive -> Comparator -> Header
		struct __archive_comparator_header_t
		{
			uint32_t identifier = ARCHIVE_IDENTIFIER_COMPARATOR;
		};

		// Archive -> Updator -> Header
		struct __archive_updator_header_t
		{
			uint32_t identifier = ARCHIVE_IDENTIFIER_UPDATOR;
		};

		// Archive -> Entry
		struct __archive_entry_t
		{
			// Data
			path_view from;
			path_view dest;	 // In archive
		};

		// Archive
		class archive
		{
		public:
			/// Constructor

			/// Destructor

			/// Operator =

			/// Read

			/// Reopen

			/// Close

		private:
			file file_;
		};

		// Archive -> Async
		class async_archive
		{
		public:
			/// Constructor

			/// Destructor

			/// Operator =

			/// Read

			/// Reopen

			/// Close

		private:
			async_file file_;
		};

		// Archive -> State
		// TODO: State iterator
		class archive_state
		{
		public:
			/// Constructors

			/// Destructor

			/// Operator =

		private:
			async_file file_;
		};

		// Archive -> Comparator
		// TODO: Comparator iterator
		class archive_comparator
		{
		public:
			/// Constructors

			/// Destructor

			/// Operator =

		private:
			async_file file_;
		};

		// Archive -> Updator
		// TODO: Updator iterator
		class archive_updator
		{
		public:
			/// Constructors
			// TODO

			/// Destructor
			// TODO

			/// Operator =
			// TODO

			/// Add
			// TODO

			/// Remove
			// TODO

			/// Replace / Update
			// TODO

			/// Move
			// TODO

		private:
			async_file file_;
		};

		// Archive -> Functions
		// Maker
		pulsar_api void
		archive_make(
		 path_view __dest,
		 perm_t __perms,
		 archive_compression_algorithm_t __alg,
		 initializer_list<__archive_entry_t> __entries);

		// Extract States
		pulsar_api path<>
		archive_make_state(
		 path_view __archive,
		 path_view __destfolder = "");

		// Compare States
		pulsar_api path<>
		archive_make_comparator(
		 path_view __state1,
		 path_view __state2,
		 path_view __destfolder = "");

		// Updator
		pulsar_api path<>
		archive_make_updater(
		 path_view __archive);
		pulsar_api path<>
		archive_update(
		 path_view __updator,
		 path_view __archive);




		/// FILESYSTEM: Directory -> Listener
		// Sync
		class directory_listener
		{
		public:
			/// Constructors

			/// Callback

		private:
			void *handle_;
			// void *callback_;
		};

		// Async
		class async_directory_listener
		{
		public:
			/// Constructors

			/// Callback

		private:
			void *handle_;
		};

	}	 // namespace fs
}	 // namespace pul

#endif	// PULSAR_FILESYSTEM_HPP
