/*! @file   iterable.hpp
 *  @author Louis-Quentin Noé (noe.louis-quentin@hotmail.fr)
 *  @brief  Implementation of iterables.
 *  @date   31-12-2022
 *
 *  @copyright Copyright (c) 2023 - Pulsar Software
 *
 *  @since 0.1.4
 */

#ifndef PULSAR_ITERABLE_HPP
#define PULSAR_ITERABLE_HPP 1

// Include: Pulsar
#include "pulsar/pulsar.hpp"
#include "pulsar/memory.hpp"
#include "pulsar/iterator.hpp"
#include "pulsar/allocator.hpp"
#include "pulsar/algorithm.hpp"

// Pulsar
namespace pul
{
	/// ITERABLE: Concept -> View
	template<typename _Iterable>
	concept __iterable_view_c = !is_iterable_v<_Iterable> && !is_view_v<_Iterable> && is_const_iterable_v<_Iterable>;
	template<typename _Iterable>
	struct is_view : std::false_type
	{};
	template<__iterable_view_c _Iterable>
	struct is_view<_Iterable> : std::true_type
	{};
	template<typename _Iterable>
	pf_decl_static pf_decl_constexpr bool is_view_v = is_view<_Iterable>::value;

	/// ITERABLE: Concept -> Container
	template<typename _Iterable>
	concept __iterable_container_c =
	 is_iterable_v<_Iterable> && !is_view_v<_Iterable>
	 && is_const_iterable_v<_Iterable>
	 && (is_back_insertable_v<_Iterable> || is_front_insertable_v<_Iterable> || is_insertable_v<_Iterable>);

	template<typename _Iterable>
	struct is_container : std::false_type
	{};
	template<__iterable_container_c _Iterable>
	struct is_container<_Iterable> : std::true_type
	{};
	template<typename _Iterable>
	pf_decl_static pf_decl_constexpr bool is_container_v = is_container<_Iterable>::value;


	/// ITERABLE: Begin
	template<typename _Iterable>
	pf_hint_nodiscard pf_decl_inline pf_decl_constexpr typename _Iterable::iterator_t
	begin(
	 _Iterable &__other) pf_attr_noexcept
		requires(is_iterable_v<_Iterable> && !is_view_v<_Iterable>)
	{
		return __other.begin();
	}
	template<typename _Iterable>
	pf_hint_nodiscard pf_decl_inline pf_decl_constexpr typename _Iterable::const_iterator_t
	begin(
	 const _Iterable &__other) pf_attr_noexcept
		requires(is_const_iterable_v<_Iterable>)
	{
		return __other.begin();
	}
	template<typename _Iterable>
	pf_hint_nodiscard pf_decl_inline pf_decl_constexpr typename _Iterable::const_iterator_t
	cbegin(
	 const _Iterable &__other) pf_attr_noexcept
		requires(is_const_iterable_v<_Iterable>)
	{
		return __other.cbegin();
	}
	template<typename _Ty>
	pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const_iterator<_Ty>
	begin(
	 initializer_list<_Ty> __list) pf_attr_noexcept
	{
		return iterator(__list.begin());
	}
	template<typename _Ty>
	pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const_iterator<_Ty>
	cbegin(
	 initializer_list<_Ty> __list) pf_attr_noexcept
	{
		return iterator(__list.begin());
	}
	template<typename _Ty, size_t _Num>
	pf_hint_nodiscard pf_decl_inline pf_decl_constexpr iterator<_Ty>
	begin(
	 _Ty (&__arr)[_Num]) pf_attr_noexcept
	{
		return &__arr[0];
	}
	template<typename _Ty, size_t _Num>
	pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const_iterator<_Ty>
	begin(
	 const _Ty (&__arr)[_Num]) pf_attr_noexcept
	{
		return &__arr[0];
	}
	template<typename _Ty, size_t _Num>
	pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const_iterator<_Ty>
	cbegin(
	 const _Ty (&__arr)[_Num]) pf_attr_noexcept
	{
		return &__arr[0];
	}

	/// ITERABLE: End
	template<typename _Iterable>
	pf_hint_nodiscard pf_decl_inline pf_decl_constexpr typename _Iterable::iterator_t
	end(
	 _Iterable &__other) pf_attr_noexcept
		requires(is_iterable_v<_Iterable> && !is_view_v<_Iterable>)
	{
		return __other.end();
	}
	template<typename _Iterable>
	pf_hint_nodiscard pf_decl_inline pf_decl_constexpr typename _Iterable::const_iterator_t
	end(
	 const _Iterable &__other) pf_attr_noexcept
		requires(is_const_iterable_v<_Iterable>)
	{
		return __other.end();
	}
	template<typename _Iterable>
	pf_hint_nodiscard pf_decl_inline pf_decl_constexpr typename _Iterable::const_iterator_t
	cend(
	 const _Iterable &__other) pf_attr_noexcept
		requires(is_const_iterable_v<_Iterable>)
	{
		return __other.cend();
	}
	template<typename _Ty>
	pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const_iterator<_Ty>
	end(
	 initializer_list<_Ty> __list) pf_attr_noexcept
	{
		return iterator(__list.end());
	}
	template<typename _Ty>
	pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const_iterator<_Ty>
	cend(
	 initializer_list<_Ty> __list) pf_attr_noexcept
	{
		return iterator(__list.end());
	}
	template<typename _Ty, size_t _Num>
	pf_hint_nodiscard pf_decl_inline pf_decl_constexpr iterator<_Ty>
	end(
	 _Ty (&__arr)[_Num]) pf_attr_noexcept
	{
		return (&__arr[0] + _Num);
	}
	template<typename _Ty, size_t _Num>
	pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const_iterator<_Ty>
	end(
	 const _Ty (&__arr)[_Num]) pf_attr_noexcept
	{
		return (&__arr[0] + _Num);
	}
	template<typename _Ty, size_t _Num>
	pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const_iterator<_Ty>
	cend(
	 const _Ty (&__arr)[_Num]) pf_attr_noexcept
	{
		return (&__arr[0] + _Num);
	}

	/// ITERABLE: Reverse Begin
	template<typename _Iterable>
	pf_hint_nodiscard pf_decl_inline pf_decl_constexpr typename _Iterable::reverse_iterator_t
	rbegin(
	 _Iterable &__other) pf_attr_noexcept
		requires(is_reverse_iterable_v<_Iterable>)
	{
		return __other.rbegin();
	}
	template<typename _Iterable>
	pf_hint_nodiscard pf_decl_inline pf_decl_constexpr typename _Iterable::const_reverse_iterator_t
	rbegin(
	 const _Iterable &__other) pf_attr_noexcept
		requires(is_const_reverse_iterable_v<_Iterable>)
	{
		return __other.rbegin();
	}
	template<typename _Iterable>
	pf_hint_nodiscard pf_decl_inline pf_decl_constexpr typename _Iterable::const_reverse_iterator_t
	crbegin(
	 const _Iterable &__other) pf_attr_noexcept
		requires(is_const_reverse_iterable_v<_Iterable>)
	{
		return __other.crbegin();
	}
	template<typename _Ty>
	pf_hint_nodiscard pf_decl_inline pf_decl_constexpr reverse_iterator<const_iterator<_Ty>>
	rbegin(
	 initializer_list<_Ty> __list) pf_attr_noexcept
	{
		return reverse_iterator(iterator(__list.rbegin()));
	}
	template<typename _Ty>
	pf_hint_nodiscard pf_decl_inline pf_decl_constexpr reverse_iterator<const_iterator<_Ty>>
	crbegin(
	 initializer_list<_Ty> __list) pf_attr_noexcept
	{
		return reverse_iterator(iterator(__list.rbegin()));
	}
	template<typename _Ty, size_t _Num>
	pf_hint_nodiscard pf_decl_inline pf_decl_constexpr reverse_iterator<iterator<_Ty>>
	rbegin(
	 _Ty (&__arr)[_Num]) pf_attr_noexcept
	{
		return (&__arr[0] + (_Num - 1));
	}
	template<typename _Ty, size_t _Num>
	pf_hint_nodiscard pf_decl_inline pf_decl_constexpr reverse_iterator<const_iterator<_Ty>>
	rbegin(
	 const _Ty (&__arr)[_Num]) pf_attr_noexcept
	{
		return (&__arr[0] + (_Num - 1));
	}
	template<typename _Ty, size_t _Num>
	pf_hint_nodiscard pf_decl_inline pf_decl_constexpr reverse_iterator<const_iterator<_Ty>>
	crbegin(
	 const _Ty (&__arr)[_Num]) pf_attr_noexcept
	{
		return (&__arr[0] + (_Num - 1));
	}

	/// ITERABLE: Reverse End
	template<typename _Iterable>
	pf_hint_nodiscard pf_decl_inline pf_decl_constexpr typename _Iterable::reverse_iterator_t
	rend(
	 _Iterable &__other) pf_attr_noexcept
		requires(is_reverse_iterable_v<_Iterable>)
	{
		return __other.rend();
	}

	template<typename _Iterable>
	pf_hint_nodiscard pf_decl_inline pf_decl_constexpr typename _Iterable::const_reverse_iterator_t
	rend(
	 const _Iterable &__other) pf_attr_noexcept
		requires(is_const_reverse_iterable_v<_Iterable>)
	{
		return __other.rend();
	}
	template<typename _Iterable>
	pf_hint_nodiscard pf_decl_inline pf_decl_constexpr typename _Iterable::const_reverse_iterator_t
	crend(
	 const _Iterable &__other) pf_attr_noexcept
		requires(is_const_reverse_iterable_v<_Iterable>)
	{
		return __other.crend();
	}
	template<typename _Ty>
	pf_hint_nodiscard pf_decl_inline pf_decl_constexpr reverse_iterator<const_iterator<_Ty>>
	rend(
	 initializer_list<_Ty> __list) pf_attr_noexcept
	{
		return reverse_iterator(iterator(__list.rend()));
	}
	template<typename _Ty>
	pf_hint_nodiscard pf_decl_inline pf_decl_constexpr reverse_iterator<const_iterator<_Ty>>
	crend(
	 initializer_list<_Ty> __list) pf_attr_noexcept
	{
		return reverse_iterator(iterator(__list.rend()));
	}
	template<typename _Ty, size_t _Num>
	pf_hint_nodiscard pf_decl_inline pf_decl_constexpr reverse_iterator<iterator<_Ty>>
	rend(
	 _Ty (&__arr)[_Num]) pf_attr_noexcept
	{
		return (&__arr[0] - 1);
	}
	template<typename _Ty, size_t _Num>
	pf_hint_nodiscard pf_decl_inline pf_decl_constexpr reverse_iterator<const_iterator<_Ty>>
	rend(
	 const _Ty (&__arr)[_Num]) pf_attr_noexcept
	{
		return (&__arr[0] - 1);
	}
	template<typename _Ty, size_t _Num>
	pf_hint_nodiscard pf_decl_constexpr reverse_iterator<const_iterator<_Ty>>
	crend(
	 const _Ty (&__arr)[_Num]) pf_attr_noexcept
	{
		return (&__arr[0] - 1);
	}

	/// ITERABLE: Data
	template<typename _Iterable>
	pf_hint_nodiscard pf_decl_inline pf_decl_constexpr typename _Iterable::value_t *
	data(
	 _Iterable &__other) pf_attr_noexcept
		requires(is_mappable_v<_Iterable>)
	{
		return __other.data();
	}
	template<typename _Iterable>
	pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const typename _Iterable::value_t *
	data(
	 const _Iterable &__other) pf_attr_noexcept
		requires(is_mappable_v<_Iterable>)
	{
		return __other.data();
	}
	template<typename _Ty, size_t _Num>
	pf_hint_nodiscard pf_decl_inline pf_decl_constexpr _Ty *
	data(
	 _Ty (&__arr)[_Num]) pf_attr_noexcept
	{
		return &__arr[0];
	}
	template<typename _Ty, size_t _Num>
	pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const _Ty *
	data(
	 const _Ty (&__arr)[_Num]) pf_attr_noexcept
	{
		return &__arr[0];
	}

	/// ITERABLE: Array
	template<typename _Ty, size_t _Num>
	class array
	{
		pf_assert_static(!std::is_const_v<_Ty> && !std::is_void_v<_Ty>);

	public:
		using value_t									 = _Ty;
		using iterator_t							 = iterator<_Ty>;
		using const_iterator_t				 = iterator<const _Ty>;
		using reverse_iterator_t			 = reverse_iterator<_Ty>;
		using const_reverse_iterator_t = reverse_iterator<const _Ty>;

		/// Constructors
		pf_decl_inline pf_decl_constexpr
		array() pf_attr_noexcept
			requires(std::is_default_constructible_v<_Ty>)
		{}
		pf_decl_inline pf_decl_constexpr
		array(
		 const _Ty &__val) pf_attr_noexcept
			requires(std::is_copy_constructible_v<_Ty>)
		{
			fill(this->begin(), this->end(), __val);
		}
		template<size_t _NumIn>
		pf_decl_inline pf_decl_constexpr
		array(
		 const _Ty (&__arr)[_NumIn])
			requires(_NumIn <= _Num)
		{
			copy_forward(pul::begin(__arr), pul::end(__arr), this->begin());
		}
		template<size_t _NumIn>
		pf_decl_inline pf_decl_constexpr
		array(
		 _Ty (&&__arr)[_NumIn])
			requires(_NumIn <= _Num)
		{
			move_forward(pul::begin(__arr), pul::end(__arr), this->begin());
		}
		template<typename... _Args>
		pf_decl_inline pf_decl_constexpr
		array(
		 _Args &&...__args) pf_attr_noexcept
			requires(sizeof...(_Args) > 1 && sizeof...(_Args) <= _Num && (std::is_constructible_v<_Ty, _Args> && ...))
			: store_ { std::forward<_Args>(__args)... }
		{}
		pf_decl_inline pf_decl_constexpr
		array(
		 const array<_Ty, _Num> &__other) pf_attr_noexcept
			requires(std::is_copy_constructible_v<_Ty>)
		{
			copy_forward(__other.begin(), __other.end(), this->begin());
		}
		pf_decl_inline pf_decl_constexpr
		array(
		 array<_Ty, _Num> &&__other) pf_attr_noexcept
			requires(std::is_move_constructible_v<_Ty>)
		{
			move_forward(__other.begin(), __other.end(), this->begin());
		}

		/// Operator=
		pf_decl_inline pf_decl_constexpr array<_Ty, _Num> &
		operator=(
		 const _Ty &__val)
			requires(std::is_copy_assignable_v<_Ty>)
		{
			fill(this->begin(), this->end(), __val);
		}
		pf_decl_inline pf_decl_constexpr array<_Ty, _Num> &
		operator=(
		 const _Ty (&__arr)[_Num])
			requires(std::is_copy_assignable_v<_Ty>)
		{
			copy_forward(pul::begin(__arr), pul::end(__arr), this->begin());
			return *this;
		}
		pf_decl_inline pf_decl_constexpr array<_Ty, _Num> &
		operator=(
		 _Ty (&&__arr)[_Num])
			requires(std::is_move_assignable_v<_Ty>)
		{
			move_forward(pul::begin(__arr), pul::end(__arr), this->begin());
			return *this;
		}
		pf_decl_inline pf_decl_constexpr array<_Ty, _Num> &
		operator=(
		 array<_Ty, _Num> const &__other) pf_attr_noexcept
			requires(std::is_copy_assignable_v<_Ty>)
		{
			if(pf_likely(this != &__other))
			{
				copy_forward(__other.begin(), __other.end(), this->begin());
			}
			return *this;
		}
		pf_decl_inline pf_decl_constexpr array<_Ty, _Num> &
		operator=(
		 array<_Ty, _Num> &&__other) pf_attr_noexcept
			requires(std::is_move_assignable_v<_Ty>)
		{
			if(pf_likely(this != &__other))
			{
				move_forward(__other.begin(), __other.end(), this->begin());
			}
			return *this;
		}

		/// Operator[]
		pf_decl_inline pf_hint_nodiscard pf_decl_constexpr _Ty &
		operator[](
		 size_t __index) pf_attr_noexcept
		{
			return this->store_[__index];
		}
		pf_decl_inline pf_hint_nodiscard pf_decl_constexpr const _Ty &
		operator[](
		 size_t __index) const pf_attr_noexcept
		{
			return this->store_[__index];
		}

		/// Swap
		pf_decl_inline pf_decl_constexpr void
		swap(
		 array<_Ty, _Num> &__other) pf_attr_noexcept
		{
			pul::swap(this->begin(), this->end(), __other.begin());
		}

		/// Iterator -> Begin
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr iterator_t
		begin() pf_attr_noexcept
		{
			return &this->store_[0];
		}
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const_iterator_t
		begin() const pf_attr_noexcept
		{
			return &this->store_[0];
		}
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const_iterator_t
		cbegin() const pf_attr_noexcept
		{
			return &this->store_[0];
		}

		/// Iterator -> End
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr iterator_t
		end() pf_attr_noexcept
		{
			return &this->store_[0] + _Num;
		}
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const_iterator_t
		end() const pf_attr_noexcept
		{
			return &this->store_[0] + _Num;
		}
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const_iterator_t
		cend() const pf_attr_noexcept
		{
			return &this->store_[0] + _Num;
		}

		/// Iterator -> Reverse Begin
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr reverse_iterator_t
		rbegin() pf_attr_noexcept
		{
			return &this->store_[0] + (_Num - 1);
		}
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const_reverse_iterator_t
		rbegin() const pf_attr_noexcept
		{
			return &this->store_[0] + (_Num - 1);
		}
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const_reverse_iterator_t
		crbegin() const pf_attr_noexcept
		{
			return &this->store_[0] + (_Num - 1);
		}

		/// Iterator -> Reverse End
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr reverse_iterator_t
		rend() pf_attr_noexcept
		{
			return &this->store_[0] - 1;
		}
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const_reverse_iterator_t
		rend() const pf_attr_noexcept
		{
			return &this->store_[0] - 1;
		}
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const_reverse_iterator_t
		crend() const pf_attr_noexcept
		{
			return &this->store_[0] - 1;
		}

		/// Accessors
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr _Ty *
		data() pf_attr_noexcept
		{
			return &this->store_[0];
		}
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const _Ty *
		data() const pf_attr_noexcept
		{
			return &this->store_[0];
		}

		/// Memory
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr size_t
		size() const pf_attr_noexcept
		{
			return _Num * sizeof(_Ty);
		}
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr size_t
		count() const pf_attr_noexcept
		{
			return _Num;
		}

		/// Operator (value_t*)
		pf_hint_nodiscard pf_decl_explicit pf_decl_inline pf_decl_constexpr
		operator value_t *() pf_attr_noexcept
		{
			return this->data();
		}
		pf_hint_nodiscard pf_decl_explicit pf_decl_inline pf_decl_constexpr
		operator const value_t *() const pf_attr_noexcept
		{
			return this->data();
		}

	private:
		_Ty store_[_Num];
	};

	/// ARRAY: CTAD
	template<typename _Ty, size_t _Num>
	array(const _Ty (&__list)[_Num]) -> array<_Ty, _Num>;
	template<typename _Ty, size_t _Num>
	array(_Ty (&&__list)[_Num]) -> array<_Ty, _Num>;
	template<typename _Ty, typename... _Args>
	array(const _Ty &__arg, _Args &&...__args) -> array<_Ty, 1 + sizeof...(_Args)>;

	/// ARRAY: Test
	pf_assert_static(is_iterable_v<array<int32_t, 6>>);




	/// QUEUE:
	// https://github.com/max0x7ba/atomic_queue/blob/master/include/atomic_queue/atomic_queue.h

	/// QUEUE: MPMC
	template<typename _Ty>
	class mpmc_lifo
	{
	private:
		// Type -> Buffer
		struct __buffer_t
		{
			/// Constructors
			__buffer_t(
			 size_t __maxcount) pf_attr_noexcept
				: head(0)
				, tail(0)
				, writers(0)
				, readers(0)
				, maxcount(__maxcount)
				, seqcount(__maxcount / CCY_NUM_THREADS)
			{
				pf_assert(this->maxcount > CCY_NUM_THREADS * 64, "maxcount_ must be greater than {}. maxcount_={}", CCY_NUM_THREADS * 64, this->maxcount);
				pf_assert(is_power_of_two(this->maxcount), "maxcount_ must be a power of two!");
			}
			__buffer_t(__buffer_t const &) = delete;
			__buffer_t(__buffer_t &&)			 = delete;

			/// Destructor
			~__buffer_t() pf_attr_noexcept = default;

			/// Operator =
			__buffer_t &
			operator=(__buffer_t const &) = delete;
			__buffer_t &
			operator=(__buffer_t &&) = delete;

			/// Shuffle
			size_t
			__shuffle(
			 size_t __index) pf_attr_noexcept
			{
				return (__index % CCY_NUM_THREADS) * this->seqcount + (__index / CCY_NUM_THREADS);
			}

			/// Enqueue
			bool
			__try_enqueue(
			 _Ty *__p) pf_attr_noexcept
			{
				// Initialisation
				size_t h = this->head.load(atomic_order::relaxed) % this->maxcount;
				size_t t = this->tail.load(atomic_order::relaxed) % this->maxcount;
				size_t c = writers.fetch_add(1, atomic_order::relaxed);

				// Verify
				size_t d			 = h <= t ? this->maxcount - h + t : t - h;
				const size_t n = c + 1;
				if((t + n % this->maxcount) > d)
				{
					this->writers.fetch_sub(1, atomic_order::relaxed);
					return false;
				}

				// Add
				t = this->tail.fetch_add(1, atomic_order::relaxed);
				this->writers.fetch_sub(1, atomic_order::relaxed);
				t									= this->__shuffle(t);
				this->elements[t] = __p;
				return false;
			}
			void
			__enqueue(
			 _Ty *__p)
			{
				// Add
				size_t t													 = this->tail.fetch_add(1, atomic_order::relaxed);
				this->elements[this->__shuffle(t)] = __p;
			}

			/// Dequeue
			pf_hint_nodiscard _Ty *
			__try_dequeue() pf_attr_noexcept
			{
				size_t h = this->head.load(atomic_order::relaxed) % this->maxcount;
				size_t t = this->tail.load(atomic_order::relaxed) % this->maxcount;
				if(h == t) return nullptr;
				size_t c = readers.fetch_add(1, atomic_order::relaxed);

				// Verify
				size_t d			 = h <= t ? t - h : this->maxcount - h + t;
				const size_t n = c + 1;
				if((t + n % this->maxcount) > d)
				{
					this->readers.fetch_sub(1, atomic_order::relaxed);
					return nullptr;
				}

				// Add
				h = this->head.fetch_add(1, atomic_order::relaxed);
				this->readers.fetch_sub(1, atomic_order::relaxed);
				h = this->__shuffle(h);
				return this->elements[h];
			}

			/// Empty
			pf_hint_nodiscard bool
			__empty() const pf_attr_noexcept
			{
				return this->head.load(atomic_order::relaxed) == this->tail.load(atomic_order::relaxed);
			}

			/// Store
			// Flexible Arrays -> Disable warning
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"

			pf_alignas(CCY_ALIGN) atomic<size_t> head;
			pf_alignas(CCY_ALIGN) atomic<size_t> tail;
			pf_alignas(CCY_ALIGN) atomic<size_t> writers;
			pf_alignas(CCY_ALIGN) atomic<size_t> readers;
			const size_t maxcount;
			const size_t seqcount;
			pf_alignas(CCY_ALIGN) _Ty *elements[];

			// Flexible Arrays
#pragma GCC diagnostic pop
		};

		/// Buffer -> New
		pf_hint_nodiscard __buffer_t *
		__new_buffer(
		 size_t __maxcount)
		{
			return new_construct_ex<__buffer_t>(sizeof(_Ty *) * __maxcount, __maxcount);
		}

		/// Buffer -> Delete
		void
		__delete_buffer(
		 __buffer_t *__buffer)
		{
			destroy_delete(__buffer);
		}

	public:
		/// Constructors
		mpmc_lifo(
		 size_t __maxcount)
			: buf_(this->__new_buffer(__maxcount))
		{}
		mpmc_lifo(mpmc_lifo<_Ty> const &__other)
			: buf_(this->__new_buffer(__other.buf_->maxcount))
		{}
		mpmc_lifo(mpmc_lifo<_Ty> &&__other)
			: buf_(__other.buf_)
		{
			__other.buf_ = this->__new_buffer(__other.buf_->maxcount);
		}

		/// Destructor
		~mpmc_lifo() pf_attr_noexcept
		{
			this->__delete_buffer(this->buf_);
		}

		/// Operator =
		mpmc_lifo<_Ty> &
		operator=(
		 mpmc_lifo<_Ty> const &__other) pf_attr_noexcept
		{
			if(pf_likely(this != __other))
			{
				pf_assert(this->__empty(), "Deleting not empty buffer!");
				this->__delete_buffer(this->buf_);
				this->buf_ = this->__new_buffer(__other.buf_->seqcount);
			}
			return *this;
		}
		mpmc_lifo<_Ty> &
		operator=(
		 mpmc_lifo<_Ty> &&__other) pf_attr_noexcept
		{
			if(pf_likely(this != __other))
			{
				pf_assert(this->__empty(), "Deleting not empty buffer!");
				this->__delete_buffer(this->buf_);
				this->buf_	 = __other.buf_;
				__other.buf_ = this->__new_buffer(__other.buf_->seqcount);
			}
			return *this;
		}

		/// enqueue
		bool
		try_enqueue(
		 _Ty *__p) pf_attr_noexcept
		{
			return this->buf_->__try_enqueue(__p);
		}
		void
		enqueue(
		 _Ty *__p) pf_attr_noexcept
		{
			return this->buf_->__enqueue(__p);
		}

		/// dequeue
		pf_hint_nodiscard _Ty *
		try_dequeue() pf_attr_noexcept
		{
			return this->buf_->__try_dequeue();
		}

	private:
		// Store
		__buffer_t *buf_;
	};


	/// QUEUE: MPMC 2 (Multi-CAS)
	template<typename _Ty>
	class mpmc_lifo2
	{
	private:
		/// Type -> Header
		struct __header_t
		{
			/// Constructors
			__header_t() pf_attr_noexcept
				: head(0)
				, writer(0)
				, tail(0)
			{}
			__header_t(__header_t const &) = delete;
			__header_t(__header_t &&)			 = delete;

			/// Destructor
			~__header_t() pf_attr_noexcept = default;

			/// Operator =
			__header_t &
			operator=(__header_t const &) = delete;
			__header_t &
			operator=(__header_t &&) = delete;

			/// Store
			pf_alignas(CCY_ALIGN) atomic<uint32_t> head;
			pf_alignas(CCY_ALIGN) atomic<uint32_t> writer;
			pf_alignas(CCY_ALIGN) atomic<uint32_t> tail;
		};

		// [b ][H1][H2][..][HN][L1][L2][..][LN]

		/// Type -> Buffer
		struct __buffer_t
		{
			/// Constructors
			__buffer_t(
			 size_t __seqcount) pf_attr_noexcept
				: seqcount(__seqcount)
			{
				pf_assert(this->seqcount > CCY_NUM_THREADS * 64, "seqcount_ must be greater than {}. seqcount_={}", CCY_NUM_THREADS * 64, this->seqcount);

				// Construct Headers
				for(uint32_t i = 0; i < CCY_NUM_THREADS; ++i)
				{
					construct(this->__get_header(i));
				}
			}
			__buffer_t(__buffer_t const &) = delete;
			__buffer_t(__buffer_t &&)			 = delete;

			/// Destructor
			~__buffer_t() pf_attr_noexcept
			{
				// Destroy Headers
				for(uint32_t i = 0; i < CCY_NUM_THREADS; ++i)
				{
					destroy(this->__get_header(i));
				}
			}

			/// Operator =
			__buffer_t &
			operator=(__buffer_t const &) = delete;
			__buffer_t &
			operator=(__buffer_t &&) = delete;

			/// Get Header
			pf_hint_nodiscard pf_decl_inline __header_t *
			__get_header(
			 uint32_t __k) pf_attr_noexcept
			{
				return union_cast<__header_t *>(
				 &this->store[0] + __k * sizeof(__header_t));
			}

			/// Get list
			pf_hint_nodiscard pf_decl_inline _Ty **
			__get_list(
			 uint32_t __k) pf_attr_noexcept
			{
				return union_cast<_Ty **>(
				 &this->store[0] + CCY_NUM_THREADS * sizeof(__header_t) + __k * this->seqcount * sizeof(_Ty *));
			}

			/// Enqueue
			size_t
			__try_enqueue(
			 _Ty *__ptr) pf_attr_noexcept
			{
				const thread_id_t idx = this_thread::get_idx();
				uint32_t i						= idx;
				do {
					__header_t *c		 = this->__get_header(i);
					const uint32_t h = c->head.load(atomic_order::acquire);
					uint32_t t			 = c->tail.load(atomic_order::acquire);
					if(t == h - 1 || !c->tail.compare_exchange_strong(t, t + 1, atomic_order::release, atomic_order::relaxed))
					{
						i = (i + 1) % CCY_NUM_THREADS;
					}
					else
					{
						auto l								= this->__get_list(i);
						l[t % this->seqcount] = __ptr;
						c->writer.fetch_add(1, atomic_order::relaxed);
						return 1;
					}
				} while(i != idx);
				return 0;
			}
			template<typename _IteratorIn>
			size_t
			__try_enqueue_bulk(
			 _IteratorIn __beg,
			 _IteratorIn __end) pf_attr_noexcept
				requires(is_iterator_v<_IteratorIn> && std::is_same_v<value_type_t<_IteratorIn>, _Ty *>)
			{
				const uint32_t count = countof(__beg, __end);
				if(pf_unlikely(count == 0 || count > this->seqcount)) return 0;
				const thread_id_t idx = this_thread::get_idx();
				uint32_t i						= idx;
				do {
					__header_t *c		 = this->__get_header(i);
					const uint32_t h = c->head.load(atomic_order::acquire);
					uint32_t t			 = c->tail.load(atomic_order::acquire);
					const uint32_t n = (t + count);
					if((n > h - 1 && n > union_cast<uint32_t>(-1) - count) || !c->tail.compare_exchange_strong(t, t + count, atomic_order::release, atomic_order::relaxed))
					{
						i = (i + 1) % CCY_NUM_THREADS;
					}
					else
					{
						auto l = this->__get_list(i);
						for(uint32_t k = 0; k < count; ++k)
						{
							l[(t + k) % this->seqcount] = __beg[k];
						}
						c->writer.fetch_add(count, atomic_order::relaxed);
						return count;
					}
				} while(i != idx);
				return 0;
			}

			/// Dequeue
			pf_hint_nodiscard _Ty *
			__try_dequeue() pf_attr_noexcept
			{
				const thread_id_t idx = this_thread::get_idx();
				uint32_t i						= idx;
				do {
					__header_t *c		 = this->__get_header(i);
					uint32_t h			 = c->head.load(atomic_order::acquire);
					const uint32_t t = c->writer.load(atomic_order::acquire);
					if(h == t || !c->head.compare_exchange_strong(h, h + 1, atomic_order::release, atomic_order::relaxed))
					{
						i = (i + 1) % CCY_NUM_THREADS;
					}
					else
					{
						auto l = this->__get_list(i);
						return l[h % this->seqcount];
					}
				} while(i != idx);
				return nullptr;
			}
			template<typename _IteratorOut>
			pf_hint_nodiscard size_t
			__try_dequeue_bulk(
			 _IteratorOut __beg,
			 _IteratorOut __end) pf_attr_noexcept
				requires(is_iterator_v<_IteratorOut> && std::is_same_v<value_type_t<_IteratorOut>, _Ty *>)
			{
				const thread_id_t idx = this_thread::get_idx();
				const uint32_t count	= union_cast<uint32_t>(countof(__beg, __end));
				if(pf_unlikely(count == 0)) return 0;
				uint32_t i = idx;
				do {
					__header_t *c						 = this->__get_header(i);
					uint32_t h							 = c->head.load(atomic_order::acquire);
					const uint32_t t				 = c->writer.load(atomic_order::acquire);
					const uint32_t available = t - h;
					uint32_t num						 = count > available ? available : count;
					if(num == 0 || !c->head.compare_exchange_strong(h, h + num, atomic_order::release, atomic_order::relaxed))
					{
						i = (i + 1) % CCY_NUM_THREADS;
					}
					else
					{
						auto l = this->__get_list(i);
						for(size_t k = 0; k < num; ++k)
						{
							__beg[k] = l[(h + k) % this->seqcount];
						}
						return num;
					}
				} while(i != idx);
				return 0;
			}

			/// Empty
			pf_hint_nodiscard bool
			__empty() const pf_attr_noexcept
			{
				for(size_t i = 0; i < CCY_NUM_THREADS; ++i)
				{
					__header_t *c = this->__get_header(i);
					uint32_t h		= c->head.load(atomic_order::acquire);
					uint32_t t		= c->tail.load(atomic_order::acquire);
					if(h != t) return false;
				}
				return true;
			}

			/// Store
			// Flexible Arrays -> Disable warning
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"

			const size_t seqcount;
			pf_alignas(CCY_ALIGN) byte_t store[];

			// Flexible Arrays
#pragma GCC diagnostic pop
		};

		/// Buffer -> New
		pf_hint_nodiscard __buffer_t *
		__new_buffer(
		 size_t __seqcount) pf_attr_noexcept
		{
			__buffer_t *b = new_construct_ex<__buffer_t>(CCY_NUM_THREADS * (sizeof(__header_t) + __seqcount * sizeof(_Ty *)), __seqcount);
			std::memset(&b->store[0] + CCY_NUM_THREADS * sizeof(__header_t), 0, __seqcount * sizeof(_Ty *) * CCY_NUM_THREADS);
			return b;
		}

		/// Buffer -> Delete
		void
		__delete_buffer(
		 __buffer_t *__buffer) pf_attr_noexcept
		{
			destroy_delete(__buffer);
		}

	public:
		/// Constructors
		pf_decl_inline
		mpmc_lifo2(
		 size_t __seqcount)
			: buf_(this->__new_buffer(__seqcount))
		{}
		pf_decl_inline
		mpmc_lifo2(
		 mpmc_lifo2<_Ty> const &__other)
			: buf_(this->__new_buffer(__other.buf_->seqcount))
		{}
		pf_decl_inline
		mpmc_lifo2(
		 mpmc_lifo2<_Ty> &&__other)
			: buf_(__other.buf_)
		{
			__other.buf_ = this->__new_buffer(__other.buf_->seqcount);
		}

		/// Operator =
		pf_decl_inline mpmc_lifo2<_Ty> &
		operator=(
		 mpmc_lifo2<_Ty> const &__other)
		{
			if(pf_likely(this != __other))
			{
				pf_assert(this->__empty(), "Deleting not empty buffer!");
				this->__delete_buffer(this->buf_);
				this->buf_ = this->__new_buffer(__other.buf_->seqcount);
			}
			return *this;
		}
		pf_decl_inline mpmc_lifo2<_Ty> &
		operator=(
		 mpmc_lifo2<_Ty> &&__other)
		{
			if(pf_likely(this != __other))
			{
				pf_assert(this->__empty(), "Deleting not empty buffer!");
				this->__delete_buffer(this->buf_);
				this->buf_	 = __other.buf_;
				__other.buf_ = this->__new_buffer(__other.buf_->seqcount);
			}
			return *this;
		}

		/// Destructor
		pf_decl_inline ~mpmc_lifo2() pf_attr_noexcept
		{
			this->__delete_buffer(this->buf_);
		}

		/// Enqueue
		pf_decl_inline bool
		try_enqueue(
		 _Ty *__ptr) pf_attr_noexcept
		{
			return this->buf_->__try_enqueue(__ptr);
		}
		template<typename _IteratorIn>
		pf_decl_inline bool
		try_enqueue_bulk(
		 _IteratorIn __beg,
		 _IteratorIn __end) pf_attr_noexcept
			requires(is_iterator_v<_IteratorIn> && std::is_same_v<value_type_t<_IteratorIn>, _Ty *>)
		{
			return this->buf_->__try_enqueue_bulk(__beg, __end);
		}

		/// Dequeue
		pf_hint_nodiscard pf_decl_inline _Ty *
		try_dequeue() pf_attr_noexcept
		{
			return this->buf_->__try_dequeue();
		}
		template<typename _IteratorOut>
		pf_hint_nodiscard pf_decl_inline size_t
		try_dequeue_bulk(
		 _IteratorOut __beg,
		 _IteratorOut __end) pf_attr_noexcept
			requires(is_iterator_v<_IteratorOut> && std::is_same_v<value_type_t<_IteratorOut>, _Ty *>)
		{
			return this->buf_->__try_dequeue_bulk(__beg, __end);
		}

	private:
		/// Store
		__buffer_t *buf_;
	};

	/// SINGLY: MPSC List
	template<typename _NodeTy>
	class mpsc_singly_lifo
	{
		pf_assert_static(!std::is_const_v<_NodeTy>, "_NodeTy is a constant type!");
		pf_assert_static(!std::is_pointer_v<_NodeTy>, "_NodeTy is a pointer type!");

	private:
		/// Type -> Cache
		struct __list_t
		{
			/// Constructors
			pf_decl_inline
			__list_t() pf_attr_noexcept
				: head(nullptr)
				, tail(nullptr)
			{}
			pf_decl_inline
			__list_t(__list_t const &) = delete;
			pf_decl_inline
			__list_t(__list_t &&) = delete;

			/// Destructor
			pf_decl_inline ~__list_t() pf_attr_noexcept = default;

			/// Operator =
			pf_decl_inline __list_t &
			operator=(__list_t const &) = delete;
			pf_decl_inline __list_t &
			operator=(__list_t &&) = delete;

			/// Store
			pf_alignas(CCY_ALIGN) atomic<_NodeTy *> head;
			pf_alignas(CCY_ALIGN) atomic<_NodeTy *> tail;
			byte_t padd[static_cast<size_t>(CCY_ALIGN) - sizeof(atomic<_NodeTy *>) - sizeof(_NodeTy *)] = { 0 };
		};

		/// Type -> Buffer
		struct __buffer_t
		{
			/// Constructors
			pf_decl_inline
			__buffer_t() pf_attr_noexcept
			{
				for(uint32_t i = 0; i != CCY_NUM_THREADS; ++i)
				{
					auto l = this->__get_list(i);
					construct(l);
				}
			}
			__buffer_t(__buffer_t const &) = delete;
			__buffer_t(__buffer_t &&)			 = delete;

			/// Destructor
			pf_decl_inline ~__buffer_t() pf_attr_noexcept
			{
				for(uint32_t i = 0; i < CCY_NUM_THREADS; ++i)
				{
					destroy(this->__get_list(i));
				}
			}

			/// Operator =
			__buffer_t &
			operator=(
			 __buffer_t const &) = delete;
			__buffer_t &
			operator=(
			 __buffer_t &&) = delete;

			/// Get k cache
			pf_hint_nodiscard pf_decl_inline __list_t *
			__get_list(
			 uint32_t __k) pf_attr_noexcept
			{
				return union_cast<__list_t *>(&this->store[0] + __k * sizeof(__list_t));
			}

			/// Enqueue
			void
			__enqueue_bulk(
			 _NodeTy *__b,
			 _NodeTy *__e) pf_attr_noexcept
			{
				// Initialization
				uint32_t i = this_thread::get_idx();
				auto *l		 = this->__get_list(i);
				_NodeTy *t = l->tail.load(atomic_order::relaxed);

				// Loop
				while(!l->tail.compare_exchange_weak(t, __e, atomic_order::release, atomic_order::relaxed))
					;
				if(t)
				{
					std::atomic_thread_fence(atomic_order::release);
					t->next = __b;
				}
				else
				{
					_NodeTy *h = l->head.load(atomic_order::relaxed);
					while(h != nullptr) h = l->head.load(atomic_order::relaxed);	// SPSC dequeue
					while(!l->head.compare_exchange_weak(h, __b, atomic_order::release, atomic_order::relaxed))
						;
				}
			}


			/// Dequeue
			pf_hint_nodiscard _NodeTy *
			__dequeue_all() pf_attr_noexcept
			{
				// Initialization
				uint32_t i = 0;
				_NodeTy *b = nullptr;
				_NodeTy *e = nullptr;

				// Create 1. list
				while(!b && i < CCY_NUM_THREADS)
				{
					auto *l		 = this->__get_list(i);
					_NodeTy *t = l->tail.load(atomic_order::relaxed);
					while(!l->tail.compare_exchange_weak(t, nullptr, atomic_order::release, atomic_order::relaxed))
						;
					if(t)
					{
						_NodeTy *h = nullptr;
						do {
							h = l->head.exchange(nullptr, atomic_order::relaxed);	 // SPSC dequeue
						} while(!h);
						b = h;
						e = t;
					}
					++i;
				}

				// Link to 1.
				while(i < CCY_NUM_THREADS)
				{
					auto *l		 = this->__get_list(i);
					_NodeTy *t = l->tail.load(atomic_order::relaxed);
					while(!l->tail.compare_exchange_weak(t, nullptr, atomic_order::release, atomic_order::relaxed))
						;
					if(t)
					{
						_NodeTy *h = nullptr;
						do {
							h = l->head.exchange(nullptr, atomic_order::relaxed);	 // SPSC dequeue
						} while(!h);
						std::atomic_thread_fence(atomic_order::release);
						e->next = t;
						e				= h;
					}
					++i;
				}

				// Return 1.
				return b;
			}

			/// Store
			pf_alignas(CCY_ALIGN) byte_t store[1];
		};

	public:
		using node_t = _NodeTy;

		/// Constructors
		pf_decl_inline
		mpsc_singly_lifo()
			: buf_(new_construct_ex<__buffer_t>(sizeof(__list_t) * CCY_NUM_THREADS))
		{}
		pf_decl_inline
		mpsc_singly_lifo(
		 mpsc_singly_lifo<_NodeTy> const &)
			: buf_(new_construct_ex<__buffer_t>(sizeof(__list_t) * CCY_NUM_THREADS))
		{}
		pf_decl_inline
		mpsc_singly_lifo(
		 mpsc_singly_lifo<_NodeTy> &&__other)
			: buf_(__other.buf_)
		{
			__other.buf_ = nullptr;
		}

		/// Destructor
		pf_decl_inline ~mpsc_singly_lifo() pf_attr_noexcept
		{
			if(this->buf_) destroy_delete(this->buf_);
		}

		/// Operator =
		pf_decl_inline mpsc_singly_lifo<_NodeTy> &
		operator=(
		 mpsc_singly_lifo<_NodeTy> const &__other) pf_attr_noexcept
		{
			return *this;
		}
		pf_decl_inline mpsc_singly_lifo<_NodeTy> &
		operator=(
		 mpsc_singly_lifo<_NodeTy> &&__other) pf_attr_noexcept
		{
			if(pf_likely(__other != this))
			{
				this->buf_	 = __other.buf_;
				__other.buf_ = nullptr;
			}
			return *this;
		}

		/// Insert Tail
		pf_decl_inline void
		enqueue_bulk(
		 node_t *__b,
		 node_t *__e) pf_attr_noexcept
		{
#ifdef PF_DEBUG

			node_t *l = __b;
			while(l->next) l = l->next;
			pf_assert(l == __e, "__b to __e isn't well formed!");

#endif	// PF_DEBUG

			return this->buf_->__enqueue_bulk(__b, __e);
		}
		pf_decl_inline void
		enqueue(
		 node_t *__n) pf_attr_noexcept
		{
			this->enqueue_bulk(__n, __n);
		}

		/// Remove Head
		pf_hint_nodiscard pf_decl_inline node_t *
		dequeue() pf_attr_noexcept
		{
			return this->buf_->__dequeue_all();
		}

	private:
		__buffer_t *buf_;
	};



	/// ITERABLE: Sequence -> Types
	template<typename _Ty>
		requires(!std::is_const_v<_Ty> && !std::is_void_v<_Ty>)
	class sequence_view;
	template<
	 typename _Ty,
	 typename _Magnifier = magnifier_default,
	 typename _Allocator = allocator_halloc>
		requires(
		 !std::is_const_v<_Ty> && !std::is_void_v<_Ty>
		 && is_magnifier_v<_Magnifier>
		 && is_allocator_v<_Allocator>)
	class sequence;

	/// ITERABLE: Sequence -> View
	template<typename _Ty>
		requires(!std::is_const_v<_Ty> && !std::is_void_v<_Ty>)
	class sequence_view
	{
	public:
		using value_t									 = _Ty;
		using const_iterator_t				 = const_iterator<_Ty>;
		using const_reverse_iterator_t = reverse_iterator<const_iterator_t>;

		/// Constructors
		pf_decl_inline pf_decl_constexpr
		sequence_view() pf_attr_noexcept
			: data_(nullptr)
			, count_(0)
		{}
		pf_decl_inline pf_decl_constexpr
		sequence_view(
		 const _Ty *__data,
		 size_t __count) pf_attr_noexcept
			: data_(__data)
			, count_(__count)
		{}
		template<size_t _Num>
		pf_decl_inline pf_decl_constexpr
		sequence_view(
		 const _Ty (&__arr)[_Num]) pf_attr_noexcept
			: sequence_view(&__arr[0u], _Num)
		{}
		template<size_t _Num>
		pf_decl_inline pf_decl_constexpr
		sequence_view(
		 const array<_Ty, _Num> &__arr) pf_attr_noexcept
			: sequence_view(&__arr[0u], _Num)
		{}
		template<
		 typename _Magnifier,
		 typename _Allocator>
		pf_decl_inline pf_decl_constexpr
		sequence_view(
		 const sequence<_Ty, _Magnifier, _Allocator> &__s) pf_attr_noexcept
			: sequence_view(__s.data(), __s.count())
		{}
		pf_decl_inline pf_decl_constexpr
		sequence_view(
		 const_iterator_t __beg,
		 const_iterator_t __end) pf_attr_noexcept
			: sequence_view(__beg.get(), countof(__beg, __end))
		{}
		pf_decl_inline pf_decl_constexpr
		sequence_view(const sequence_view<_Ty> &__other) pf_attr_noexcept
			: sequence_view(__other.data(), __other.count())
		{}

		/// Destructor
		pf_decl_inline pf_decl_constexpr ~sequence_view() pf_attr_noexcept = default;

		/// Operator =
		pf_decl_inline pf_decl_constexpr sequence_view<_Ty> &
		operator=(
		 const sequence_view<_Ty> &__other) pf_attr_noexcept
		{
			this->data_	 = __other.data_;
			this->count_ = __other.count_;
			return *this;
		}

		/// Operator []
		pf_decl_inline pf_decl_constexpr const _Ty &
		operator[](
		 size_t __index) const
		{
			pf_throw_if(
			 __index >= this->count_,
			 dbg_category_generic(),
			 dbg_code::invalid_argument,
			 dbg_flags::none,
			 "index is out of sequence view! len={}, index={}",
			 this->count_,
			 __index);
			return *(this->data_ + __index);
		}

		/// Begin
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const_iterator_t
		begin() const pf_attr_noexcept
		{
			return this->data_;
		}
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const_iterator_t
		cbegin() const pf_attr_noexcept
		{
			return this->begin();
		}

		/// End
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const_iterator_t
		end() const pf_attr_noexcept
		{
			return (this->data_ + this->count_);
		}
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const_iterator_t
		cend() const pf_attr_noexcept
		{
			return this->end();
		}

		/// Reverse Begin
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const_iterator_t
		rbegin() const pf_attr_noexcept
		{
			return (this->data_ - 1 + this->count_);
		}
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const_iterator_t
		crbegin() const pf_attr_noexcept
		{
			return this->rbegin();
		}

		/// Reverse End
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const_iterator_t
		rend() const pf_attr_noexcept
		{
			return (this->data_ - 1);
		}
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const_iterator_t
		crend() const pf_attr_noexcept
		{
			return this->rend();
		}

		/// Data
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const _Ty *
		data() const pf_attr_noexcept
		{
			return this->data_;
		}

		/// Count
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr size_t
		count() const pf_attr_noexcept
		{
			return this->count_;
		}

		/// Size
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr size_t
		size() const pf_attr_noexcept
		{
			return this->count_ * sizeof(_Ty);
		}

		/// Is Empty
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr bool
		is_empty() const pf_attr_noexcept
		{
			return !this->data_;
		}

	private:
		const _Ty *data_;
		size_t count_;
	};

	/// ITERABLE: Sequence -> View -> CTAD
	template<
	 typename _IteratorIn>
		requires(is_iterator_v<_IteratorIn>)
	sequence_view(_IteratorIn, _IteratorIn)
	 -> sequence_view<std::remove_const_t<typename _IteratorIn::value_t>>;

	/// ITERABLE: Sequence
	template<
	 typename _Ty,
	 typename _Magnifier,
	 typename _Allocator>
		requires(
		 !std::is_const_v<_Ty> && !std::is_void_v<_Ty>
		 && is_magnifier_v<_Magnifier>
		 && is_allocator_v<_Allocator>)
	class sequence
	{
		template<typename _Uy, typename _MagnifierR, typename _AllocatorR>
			requires(
			 !std::is_const_v<_Uy> && std::is_void_v<_Uy>
			 && is_magnifier_v<_MagnifierR>
			 && is_allocator_v<_AllocatorR>)
		pf_decl_friend class sequence;

		// __reallocate
		/*!
		 * @brief Reallocate
		 *
		 * @param[in] __count The size of the new storage
		 * @param[in] __align The alignment of the new storage
		 *
		 * @return diff_t The difference between the old and new storage size
		 */
		pf_decl_constexpr diff_t
		__reallocate_no_check(
		 size_t __count,
		 align_val_t __align)
		{
			this->data_ = union_cast<_Ty *>(reallocate(
			 this->allocator_,
			 this->data_,
			 __count * sizeof(_Ty),
			 __align));
			pf_throw_if(
			 !this->data_,
			 dbg_category_generic(),
			 dbg_code::bad_alloc,
			 dbg_flags::dump_with_data_segs | dbg_flags::dump_with_handle_data,
			 "Cannot reallocate sequence storage!");
			const diff_t d	= union_cast<diff_t>(__count) - this->capacity_;
			this->capacity_ = __count;
			this->align_		= __align;
			return d;
		}

		// __throw_if*
		pf_decl_inline pf_decl_constexpr void
		__throw_if_incorrect_insert_position(
		 size_t __where)
		{
			pf_throw_if(
			 __where > this->count_,
			 dbg_category_generic(),
			 dbg_code::invalid_argument,
			 dbg_flags::none,
			 "Trying to insert outside of sequence range! count={}, where={}",
			 this->count_,
			 __where);
		}
		pf_decl_inline pf_decl_constexpr void
		__throw_if_incorrect_index_position(
		 size_t __index)
		{
			pf_throw_if(
			 __index >= this->count_,
			 dbg_category_generic(),
			 dbg_code::invalid_argument,
			 dbg_flags::none,
			 "Index is out of sequence range! count={}, index={}",
			 this->count_,
			 __index);
		}
		pf_decl_inline pf_decl_constexpr void
		__throw_if_incorrect_remove_replace_position(
		 size_t __where)
		{
			pf_throw_if(
			 __where >= this->count_,
			 dbg_category_generic(),
			 dbg_code::invalid_argument,
			 dbg_flags::none,
			 "Trying to remove / replace outside of sequence range! count={}, where={}",
			 this->count_,
			 __where);
		}
		pf_decl_inline pf_decl_constexpr void
		__throw_if_incorrect_remove_replace_range(
		 size_t __wbeg,
		 size_t __wend)
		{
			pf_throw_if(
			 __wbeg > __wend,
			 dbg_category_generic(),
			 dbg_code::invalid_argument,
			 dbg_flags::none,
			 "Formed range between where_begin and where_end is ill-formed! where_begin={}, where_end={}",
			 __wbeg,
			 __wend);
			pf_throw_if(
			 __wbeg >= this->count_,
			 dbg_category_generic(),
			 dbg_code::invalid_argument,
			 dbg_flags::none,
			 "Trying to remove / replace outside of sequence range! count={}, where_begin={}, where_end={}",
			 this->count_,
			 __wbeg,
			 __wend);
		}

		// __iterator_to_index
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr size_t
		__iterator_to_index(
		 iterator<_Ty> __it) pf_attr_noexcept
		{
			return countof(this->begin(), __it);
		}

		// __push_insert_back
		template<typename... _Args>
		pf_hint_nodiscard pf_decl_constexpr size_t
		__push_insert_back(
		 _Args &&...__args)
		{
			this->reserve_with_magnifier(this->count_ + 1);
			iterator_t w = this->begin() + this->count_;
			construct(w, std::forward<_Args>(__args)...);
			++this->count_;
			return this->count_;
		}
		pf_hint_nodiscard pf_decl_constexpr size_t
		__push_insert_back(
		 const _Ty &__val,
		 size_t __count)
		{
			if(pf_unlikely(!__count)) return -1;
			this->reserve_with_magnifier(this->count_ + __count);
			iterator_t b = this->begin() + this->count_;
			iterator_t e = this->begin() + this->count_ + __count;
			construct(b, e, __val);
			this->count_ += __count;
			return this->count_;
		}
		template<typename _IteratorIn>
		pf_hint_nodiscard pf_decl_constexpr size_t
		__push_insert_back(
		 _IteratorIn __beg,
		 _IteratorIn __end)
		{
			const size_t c = countof(__beg, __end);
			if(pf_unlikely(!c)) return -1;
			this->reserve_with_magnifier(this->count_ + c);
			iterator_t b = this->begin() + this->count_;
			iterator_t e = this->begin() + this->count_ + c;
			construct(b, e, __beg);
			this->count_ += c;
			return this->count_;
		}

		// __push_insert
		template<typename... _Args>
		pf_hint_nodiscard pf_decl_constexpr size_t
		__push_insert(
		 size_t __where,
		 _Args &&...__args)
		{
			this->__throw_if_incorrect_insert_position(__where);
			this->reserve_with_magnifier(this->count_ + 1);
			auto w = this->begin() + __where;
			memmove(w + 1, w, distof(w, this->end()));
			construct(w, std::forward<_Args>(__args)...);
			++this->count_;
			return __where;
		}
		pf_decl_constexpr size_t
		__push_insert_no_check(
		 size_t __where,
		 const _Ty &__val,
		 size_t __count)
		{
			if(pf_unlikely(!__count)) return -1;
			this->reserve_with_magnifier(this->count_ + __count);
			auto w = this->begin() + __where;
			auto l = w + __count;
			memmove(l, w, distof(w, this->end()));
			construct(w, l, __val);
			this->count_ += __count;
			return __where;
		}
		pf_hint_nodiscard pf_decl_constexpr size_t
		__push_insert(
		 size_t __where,
		 const _Ty &__val,
		 size_t __count)
		{
			this->__throw_if_incorrect_insert_position(__where);
			return this->__push_insert_no_check(__where, __val, __count);
		}
		template<typename _IteratorIn>
		pf_decl_constexpr size_t
		__push_insert_no_check(
		 size_t __where,
		 _IteratorIn __beg,
		 _IteratorIn __end)
		{
			const size_t c = countof(__beg, __end);
			if(pf_unlikely(!c)) return -1;
			this->reserve_with_magnifier(this->count_ + c);
			auto w = this->begin() + __where;
			auto n = w + c;
			memmove(n, w, distof(w, this->end()));
			construct(w, n, __beg);
			this->count_ += c;
			return __where;
		}
		template<typename _IteratorIn>
		pf_hint_nodiscard pf_decl_constexpr size_t
		__push_insert(
		 size_t __where,
		 _IteratorIn __beg,
		 _IteratorIn __end)
		{
			this->__throw_if_incorrect_insert_position(__where);
			return this->__push_insert_no_check(__where, __beg, __end);
		}

		// __pop_remove_back
		pf_hint_nodiscard pf_decl_constexpr size_t
		__pop_remove_back()
		{
			pf_throw_if(
			 this->is_empty(),
			 dbg_category_generic(),
			 dbg_code::runtime_error,
			 dbg_flags::none,
			 "Trying to remove elements from empty sequence!");
			--this->count_;
			destroy(this->end());
			return this->count_ - 1;
		}

		// __pop_remove
		pf_hint_nodiscard pf_decl_constexpr size_t
		__pop_remove(
		 size_t __where)
		{
			this->__throw_if_incorrect_remove_replace_position(__where);
			auto w = this->begin() + __where;
			auto l = w + 1;
			destroy(w);
			memmove(w, l, distof(l, this->end()));
			--this->count_;
			return __where >= this->count_ ? this->count_ - 1 : __where;
		}
		pf_hint_nodiscard pf_decl_constexpr size_t
		__pop_remove(
		 size_t __wbeg,
		 size_t __wend)
		{
			this->__throw_if_incorrect_remove_replace_range(__wbeg, __wend);
			auto wbeg = this->begin() + __wbeg;
			auto wend = this->begin() + __wend;
			destroy(wbeg, wend);
			memcpy(wbeg, wend, distof(wend, this->end()));
			this->count_ -= countof(wbeg, wend);
			return __wbeg >= this->count_ ? this->count_ - 1 : __wbeg;
		}

		// __replace
		template<typename... _Args>
		pf_hint_nodiscard pf_decl_constexpr size_t
		__replace(
		 size_t __where,
		 _Args &&...__args)
		{
			this->__throw_if_incorrect_remove_replace_position(__where);
			auto w = this->begin() + __where;
			pul::assign(w, std::forward<_Args>(__args)...);
			return __where;
		}
		pf_hint_nodiscard pf_decl_constexpr size_t
		__replace(
		 size_t __where,
		 const _Ty &__val,
		 size_t __count)
		{
			if(pf_unlikely(!__count)) return this->count_;
			this->__throw_if_incorrect_remove_replace_position(__where);
			auto w = this->begin() + __where;
			pul::assign(w, __val);
			this->__push_insert_no_check(__where + 1, __val, __count - 1);
			return __where;
		}
		template<typename _IteratorIn>
		pf_hint_nodiscard pf_decl_constexpr size_t
		__replace(
		 size_t __where,
		 _IteratorIn __beg,
		 _IteratorIn __end)
		{
			this->__throw_if_incorrect_remove_replace_position(__where);
			auto w = this->begin() + __where;
			pul::assign(w, *__beg);
			this->__push_insert_no_check(__where + 1, __beg + 1, __end);
			return __where;
		}
		pf_hint_nodiscard pf_decl_constexpr size_t
		__replace(
		 size_t __wbeg,
		 size_t __wend,
		 const _Ty &__val,
		 size_t __count)
		{
			if(pf_unlikely(!__count)) return this->count_;
			this->__throw_if_incorrect_remove_replace_range(__wbeg, __wend);
			const size_t c = __wend - __wbeg;
			if(c < __count)
			{
				const size_t n = __count - c;
				this->reserve_with_magnifier(this->count_ + n);
				auto wbeg = this->begin() + __wbeg;
				auto wend = this->begin() + __wend;
				auto w		= wbeg + __count;
				pul::assign(wbeg, wend, __val);
				memmove(w, wend, distof(wend, this->end()));
				construct(wbeg + c, w, __val);
				this->count_ += n;
			}
			else
			{
				auto wbeg = this->begin() + __wbeg;
				auto wend = this->begin() + __wend;
				auto w		= wbeg + __count;
				pul::assign(wbeg, wend, __val);
				if(c > __count)
				{
					destroy(w, wend);
					memcpy(w, wend, distof(wend, this->end()));
					this->count_ -= (c - __count);
				}
			}
			return __wbeg;
		}
		template<typename _IteratorIn>
		pf_hint_nodiscard pf_decl_constexpr size_t
		__replace(
		 size_t __wbeg,
		 size_t __wend,
		 _IteratorIn __ibeg,
		 _IteratorIn __iend)
		{
			this->__throw_if_incorrect_remove_replace_range(__wbeg, __wend);
			const size_t ic = countof(__ibeg, __iend);
			if(pf_unlikely(!ic)) return this->count_;
			const size_t wc = (__wend - __wbeg);
			if(wc < ic)
			{
				const size_t n = ic - wc;
				this->reserve_with_magnifier(this->count_ + n);
				auto wbeg = this->begin() + __wbeg;
				auto wend = this->begin() + __wend;
				auto last = wbeg + ic;
				pul::assign(wbeg, wend, __ibeg);
				memmove(last, wend, distof(wend, this->end()));
				construct(wbeg + wc, last, __ibeg + wc);
				this->count_ += n;
			}
			else
			{
				auto wbeg = this->begin() + __wbeg;
				auto wend = this->begin() + __wend;
				auto last = wbeg + ic;
				pul::assign(wbeg, wend, __ibeg);
				if(wc > ic)
				{
					destroy(last, wend);
					memcpy(last, wend, distof(wend, this->end()));
					this->count_ -= (wc - ic);
				}
			}
			return __wbeg;
		}

	public:
		using value_t									 = _Ty;
		using iterator_t							 = iterator<_Ty>;
		using const_iterator_t				 = const_iterator<_Ty>;
		using reverse_iterator_t			 = reverse_iterator<iterator_t>;
		using const_reverse_iterator_t = reverse_iterator<const_iterator_t>;
		using view_t									 = sequence_view<_Ty>;

		/// Constructors
		pf_decl_inline pf_decl_constexpr
		sequence(
		 align_val_t __align			= ALIGN_DEFAULT,
		 _Magnifier &&__magnifier = _Magnifier(),
		 _Allocator &&__allocator = _Allocator()) pf_attr_noexcept
			: data_(nullptr)
			, capacity_(0)
			, align_(__align)
			, count_(0)
			, magnifier_(std::move(__magnifier))
			, allocator_(std::move(__allocator))
		{}
		pf_decl_inline pf_decl_constexpr
		sequence(
		 const _Ty &__val,
		 size_t __count,
		 align_val_t __align			= ALIGN_DEFAULT,
		 _Magnifier &&__magnifier = _Magnifier(),
		 _Allocator &&__allocator = _Allocator())
			: sequence(__align, std::move(__magnifier), std::move(__allocator))
		{
			this->__reallocate_no_check(this->magnifier_(__count), __align);
			construct(this->begin(), this->begin() + __count, __val);
			this->count_ = __count;
		}
		template<typename _IteratorIn>
		pf_decl_inline pf_decl_constexpr
		sequence(
		 _IteratorIn __beg,
		 _IteratorIn __end,
		 align_val_t __align			= ALIGN_DEFAULT,
		 _Magnifier &&__magnifier = _Magnifier(),
		 _Allocator &&__allocator = _Allocator())
			requires(is_iterator_v<_IteratorIn>)
			: sequence(__align, std::move(__magnifier), std::move(__allocator))
		{
			const size_t c = countof(__beg, __end);
			this->__reallocate_no_check(this->magnifier_(c), __align);
			construct(this->begin(), this->begin() + c, __beg);
			this->count_ = c;
		}
		template<size_t _Num>
		pf_decl_inline pf_decl_constexpr
		sequence(
		 const _Ty (&__arr)[_Num],
		 align_val_t __align			= ALIGN_DEFAULT,
		 _Magnifier &&__magnifier = _Magnifier(),
		 _Allocator &&__allocator = _Allocator())
			: sequence(pul::begin(__arr), pul::end(__arr), __align, std::move(__magnifier), std::move(__allocator))
		{}
		pf_decl_inline pf_decl_constexpr
		sequence(
		 initializer_list<_Ty> __list,
		 align_val_t __align			= ALIGN_DEFAULT,
		 _Magnifier &&__magnifier = _Magnifier(),
		 _Allocator &&__allocator = _Allocator())
			: sequence(pul::begin(__list), pul::end(__list), __align, std::move(__magnifier), std::move(__allocator))
		{}
		template<typename _Iterable>
		pf_decl_inline pf_decl_constexpr
		sequence(
		 _Iterable const &__iterable,
		 align_val_t __align,
		 _Magnifier &&__magnifier,
		 _Allocator &&__allocator)
			requires(is_iterable_v<_Iterable> && !is_view_v<_Iterable>)
			: sequence(__iterable.begin(), __iterable.end(), __align, std::move(__magnifier), std::move(__allocator))
		{}
		template<typename _View>
		pf_decl_inline pf_decl_constexpr
		sequence(
		 _View __view,
		 align_val_t __align,
		 _Magnifier &&__magnifier,
		 _Allocator &&__allocator)
			requires(is_view_v<_View>)
			: sequence(__view.begin(), __view.end(), __align, std::move(__magnifier), std::move(__allocator))
		{}
		pf_decl_inline pf_decl_constexpr
		sequence(
		 sequence<_Ty, _Magnifier, _Allocator> const &__other,
		 align_val_t __align)
			: sequence(__other.begin(), __other.end(), __align, __other.magnifier_, __other.allocator_)
		{}
		pf_decl_inline pf_decl_constexpr
		sequence(
		 sequence<_Ty, _Magnifier, _Allocator> const &__other)
			: sequence(__other.begin(), __other.end(), __other.align_, __other.magnifier_, __other.allocator_)
		{}
		pf_decl_inline pf_decl_constexpr
		sequence(
		 sequence<_Ty, _Magnifier, _Allocator> &&__other) pf_attr_noexcept
			: data_(__other.data_)
			, capacity_(__other.capacity_)
			, align_(__other.align_)
			, count_(__other.count_)
			, magnifier_(std::move(__other.magnifier_))
			, allocator_(std::move(__other.allocator_))
		{
			__other.data_			= nullptr;
			__other.capacity_ = 0;
			__other.count_		= 0;
		}
		template<typename _MagnifierR>
		pf_decl_inline pf_decl_constexpr
		sequence(
		 sequence<_Ty, _MagnifierR, _Allocator> &&__other,
		 _Magnifier &&__magnifier) pf_attr_noexcept
			: data_(__other.data_)
			, capacity_(__other.capacity_)
			, align_(__other.align_)
			, count_(__other.count_)
			, magnifier_(std::move(__magnifier))
			, allocator_(__other.allocator_)
		{
			__other.data_			= nullptr;
			__other.capacity_ = 0;
			__other.count_		= 0;
		}

		/// Destructor
		pf_decl_inline pf_decl_constexpr ~sequence() pf_attr_noexcept
		{
			this->clear();
		}

		/// Operator =
		template<typename _View>
		pf_decl_inline pf_decl_constexpr sequence<_Ty, _Magnifier, _Allocator> &
		operator=(
		 _View __view)
			requires(is_view_v<_View>)
		{
			this->assign(__view);
			return *this;
		}
		template<typename _Iterable>
		pf_decl_inline pf_decl_constexpr sequence<_Ty, _Magnifier, _Allocator> &
		operator=(
		 _Iterable const &__iterable)
			requires(is_iterable_v<_Iterable> && !is_view_v<_Iterable>)
		{
			this->assign(__iterable);
			return *this;
		}
		pf_decl_inline pf_decl_constexpr sequence<_Ty, _Magnifier, _Allocator> &
		operator=(
		 initializer_list<_Ty> __list)
		{
			this->assign(__list);
			return *this;
		}
		template<size_t _Num>
		pf_decl_inline pf_decl_constexpr sequence<_Ty, _Magnifier, _Allocator> &
		operator=(
		 const _Ty (&__arr)[_Num])
		{
			this->assign(__arr);
			return *this;
		}
		pf_decl_inline pf_decl_constexpr sequence<_Ty, _Magnifier, _Allocator> &
		operator=(
		 sequence<_Ty, _Magnifier, _Allocator> const &__other)
		{
			this->assign(__other);
			return *this;
		}
		template<typename _MagnifierR, typename _AllocatorR>
		pf_decl_inline pf_decl_constexpr sequence<_Ty, _Magnifier, _Allocator> &
		operator=(
		 sequence<_Ty, _MagnifierR, _AllocatorR> const &__other)
			requires(is_magnifier_v<_MagnifierR> && is_allocator_v<_AllocatorR>)
		{
			this->assign(__other);
			return *this;
		}
		pf_decl_inline pf_decl_constexpr sequence<_Ty, _Magnifier, _Allocator> &
		operator=(
		 sequence<_Ty, _Magnifier, _Allocator> &&__other)
		{
			this->assign(std::move(__other));
			return *this;
		}

		/// Operator (View)
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr
		operator sequence_view<_Ty>() const pf_attr_noexcept
		{
			return this->view();
		}

		/// Operator []
		pf_decl_inline pf_decl_constexpr value_t &
		operator[](size_t __index)
		{
			this->__throw_if_incorrect_index_position(__index);
			return this->data_[__index];
		}
		pf_decl_inline pf_decl_constexpr const value_t &
		operator[](size_t __index) const
		{
			this->__throw_if_incorrect_index_position(__index);
			return this->data_[__index];
		}

		/// Assign
		pf_decl_constexpr void
		assign(
		 const _Ty &__val,
		 size_t __count)
		{
			if(this->count_ < __count)
			{
				this->reserve_with_magnifier(__count);
				pul::assign(this->begin(), this->begin() + this->count_, __val);
				construct(this->begin() + this->count_, this->begin() + __count, __val);
			}
			else
			{
				pul::assign(this->begin(), this->begin() + __count, __val);
				destroy(this->begin() + __count, this->end());
			}
			this->count_ = __count;
		}
		template<typename _IteratorIn>
		pf_decl_constexpr void
		assign(
		 _IteratorIn __beg,
		 _IteratorIn __end)
			requires(is_iterator_v<_IteratorIn>)
		{
			const size_t c = countof(__beg, __end);
			if(this->count_ < c)
			{
				this->reserve_with_magnifier(c);
				pul::assign(this->begin(), this->begin() + this->count_, __beg);
				construct(this->begin() + this->count_, this->begin() + c, __beg + this->count_);
			}
			else
			{
				pul::assign(this->begin(), this->begin() + c, __beg);
				destroy(this->begin() + c, this->end());
			}
			this->count_ = c;
		}
		pf_decl_inline pf_decl_constexpr void
		assign(
		 sequence<_Ty, _Magnifier, _Allocator> const &__other)
		{
			if(pf_likely(this != &__other)) this->assign(__other.begin(), __other.end());
		}
		template<typename _MagnifierR, typename _AllocatorR>
		pf_decl_inline pf_decl_constexpr void
		assign(
		 sequence<_Ty, _MagnifierR, _AllocatorR> const &__other)
		{
			this->assign(__other.begin(), __other.end());
		}
		template<size_t _Num>
		pf_decl_inline pf_decl_constexpr void
		assign(
		 const _Ty (&__arr)[_Num])
		{
			this->assign(pul::begin(__arr), pul::end(__arr));
		}
		template<typename _Iterable>
		pf_decl_inline pf_decl_constexpr void
		assign(
		 _Iterable const &__iterable)
			requires(is_iterable_v<_Iterable> && !is_view_v<_Iterable>)
		{
			this->assign(__iterable.begin(), __iterable.end());
		}
		template<typename _View>
		pf_decl_inline pf_decl_constexpr void
		assign(
		 _View __v)
			requires(is_view_v<_View>)
		{
			this->assign(__v.begin(), __v.end());
		}
		pf_decl_inline pf_decl_constexpr void
		assign(
		 initializer_list<_Ty> __list)
		{
			this->assign(iterator(__list.begin()), iterator(__list.end()));
		}
		pf_decl_constexpr void
		assign(
		 sequence<_Ty, _Magnifier, _Allocator> &&__other) pf_attr_noexcept
		{
			if(pf_likely(this->data_ != __other.data_))
			{
				this->clear();
				this->data_				= __other.data_;
				this->capacity_		= __other.capacity_;
				this->align_			= __other.align_;
				this->count_			= __other.count_;
				this->magnifier_	= std::move(__other.magnifier_);
				this->allocator_	= std::move(__other.allocator_);
				__other.data_			= nullptr;
				__other.capacity_ = 0;
				__other.count_		= 0;
			}
		}

		/// Push
		template<typename... _Args>
		pf_decl_inline pf_decl_constexpr size_t
		push_back(
		 _Args &&...__args)
			requires(std::is_constructible_v<_Ty, _Args...>)
		{
			return this->__push_insert_back(std::forward<_Args>(__args)...);
		}
		pf_decl_inline pf_decl_constexpr size_t
		push_back(
		 const _Ty &__val,
		 size_t __count)
		{
			return this->__push_insert_back(__val, __count);
		}
		template<typename _IteratorIn>
		pf_decl_inline pf_decl_constexpr size_t
		push_back(
		 _IteratorIn __beg,
		 _IteratorIn __end)
			requires(is_iterator_v<_IteratorIn>)
		{
			return this->__push_insert_back(__beg, __end);
		}
		template<typename _View>
		pf_decl_inline pf_decl_constexpr size_t
		push_back(
		 _View __v)
			requires(
			 is_view_v<_View>
			 && std::is_same_v<_Ty, value_type_t<_View>>)
		{
			return this->push_back(__v.begin(), __v.end());
		}
		pf_decl_inline pf_decl_constexpr size_t
		push_back(
		 initializer_list<_Ty> __list)
		{
			return this->push_back(iterator_t(__list.begin()), iterator_t(__list.end()));
		}

		template<typename... _Args>
		pf_decl_inline pf_decl_constexpr size_t
		push(
		 size_t __where,
		 _Args &&...__args)
			requires(std::is_constructible_v<_Ty, _Args...>)
		{
			return this->__push_insert(__where, std::forward<_Args>(__args)...);
		}
		pf_decl_inline pf_decl_constexpr size_t
		push(
		 size_t __where,
		 const _Ty &__val,
		 size_t __count)
		{
			return this->__push_insert(__where, __val, __count);
		}
		template<typename _IteratorIn>
		pf_decl_inline pf_decl_constexpr size_t
		push(
		 size_t __where,
		 _IteratorIn __beg,
		 _IteratorIn __end)
			requires(is_iterator_v<_IteratorIn>)
		{
			return this->__push_insert(__where, __beg, __end);
		}
		template<typename _View>
		pf_decl_inline pf_decl_constexpr size_t
		push(
		 size_t __where,
		 _View __v)
			requires(
			 is_view_v<_View>
			 && std::is_same_v<_Ty, value_type_t<_View>>)
		{
			return this->push(__where, __v.begin(), __v.end());
		}
		pf_decl_constexpr size_t
		push(
		 size_t __where,
		 initializer_list<_Ty> __list)
		{
			return this->push(__where, iterator(__list.begin()), iterator(__list.end()));
		}

		/// Insert
		template<typename... _Args>
		pf_decl_inline pf_decl_constexpr iterator_t
		insert_back(
		 _Args &&...__args)
			requires(std::is_constructible_v<_Ty, _Args...>)
		{
			return this->begin() + this->__push_insert_back(std::forward<_Args>(__args)...);
		}
		pf_decl_inline pf_decl_constexpr iterator_t
		insert_back(
		 const _Ty &__val,
		 size_t __count)
		{
			return this->begin() + this->__push_insert_back(__val, __count);
		}
		template<typename _IteratorIn>
		pf_decl_inline pf_decl_constexpr iterator_t
		insert_back(
		 _IteratorIn __beg,
		 _IteratorIn __end)
			requires(is_iterator_v<_IteratorIn>)
		{
			return this->begin() + this->__push_insert_back(__beg, __end);
		}
		template<typename _View>
		pf_decl_inline pf_decl_constexpr iterator_t
		insert_back(
		 _View __v)
			requires(
			 is_view_v<_View>
			 && std::is_same_v<_Ty, value_type_t<_View>>)
		{
			return this->insert_back(__v.begin(), __v.end());
		}
		pf_decl_inline pf_decl_constexpr iterator_t
		insert_back(
		 initializer_list<_Ty> __list)
		{
			return this->insert_back(iterator_t(__list.begin()), iterator_t(__list.end()));
		}

		template<typename... _Args>
		pf_decl_inline pf_decl_constexpr iterator_t
		insert(
		 iterator_t __where,
		 _Args &&...__args)
			requires(std::is_constructible_v<_Ty, _Args...>)
		{
			return this->begin() + this->__push_insert(this->__iterator_to_index(__where), std::forward<_Args>(__args)...);
		}
		pf_decl_inline pf_decl_constexpr iterator_t
		insert(
		 iterator_t __where,
		 const _Ty &__val,
		 size_t __count)
		{
			return this->begin() + this->__push_insert(this->__iterator_to_index(__where), __val, __count);
		}
		template<typename _IteratorIn>
		pf_decl_inline pf_decl_constexpr iterator_t
		insert(
		 iterator_t __where,
		 _IteratorIn __beg,
		 _IteratorIn __end)
			requires(is_iterator_v<_IteratorIn>)
		{
			return this->begin() + this->__push_insert(this->__iterator_to_index(__where), __beg, __end);
		}
		template<typename _Iterable>
		pf_decl_inline pf_decl_constexpr iterator_t
		insert(
		 iterator_t __where,
		 _Iterable const &__iterable)
			requires(is_iterable_v<_Iterable> && !is_view_v<_Iterable>)
		{
			return this->insert(__where, __iterable.cbegin(), __iterable.cend());
		}
		template<typename _View>
		pf_decl_inline pf_decl_constexpr iterator_t
		insert(
		 iterator_t __where,
		 _View __view)
			requires(
			 is_view_v<_View>
			 && std::is_same_v<_Ty, value_type_t<_View>>)
		{
			return this->insert(__where, __view.cbegin(), __view.cend());
		}
		pf_decl_inline pf_decl_constexpr iterator_t
		insert(
		 iterator_t __where,
		 initializer_list<_Ty> __list)
		{
			return this->insert(__where, iterator(__list.begin()), iterator(__list.end()));
		}

		/// Pop
		pf_decl_inline pf_decl_constexpr size_t
		pop_back()
		{
			return this->__pop_remove_back();
		}

		pf_decl_inline pf_decl_constexpr size_t
		pop(
		 size_t __where)
		{
			return this->__pop_remove(__where);
		}
		pf_decl_inline pf_decl_constexpr size_t
		pop(
		 size_t __wbeg,
		 size_t __wend)
		{
			return this->__pop_remove(__wbeg, __wend);
		}

		/// Remove
		pf_decl_inline pf_decl_constexpr iterator_t
		remove_back() pf_attr_noexcept
		{
			return this->begin() + this->__pop_remove_back();
		}

		pf_decl_inline pf_decl_constexpr iterator_t
		remove(
		 iterator_t __where) pf_attr_noexcept
		{
			return this->begin() + this->__pop_remove(this->__iterator_to_index(__where));
		}
		pf_decl_inline pf_decl_constexpr iterator_t
		remove(
		 iterator_t __wb,
		 iterator_t __we) pf_attr_noexcept
		{
			return this->begin() + this->__pop_remove(this->__iterator_to_index(__wb), this->__iterator_to_index(__we));
		}

		/// Replace
		template<typename... _Args>
		pf_decl_inline pf_decl_constexpr size_t
		replace(
		 size_t __where,
		 _Args &&...__args)
			requires(std::is_constructible_v<_Ty, _Args...>)
		{
			return this->__replace(__where, std::forward<_Args>(__args)...);
		}
		pf_decl_inline pf_decl_constexpr size_t
		replace(
		 size_t __where,
		 const _Ty &__val,
		 size_t __count)
		{
			return this->__replace(__where, __val, __count);
		}
		template<typename _IteratorIn>
		pf_decl_inline pf_decl_constexpr size_t
		replace(
		 size_t __where,
		 _IteratorIn __beg,
		 _IteratorIn __end)
			requires(is_iterator_v<_IteratorIn>)
		{
			return this->__replace(__where, __beg, __end);
		}
		template<typename _Iterable>
		pf_decl_inline pf_decl_constexpr size_t
		reinsert(
		 size_t __where,
		 _Iterable const &__iterable)
			requires(
			 is_iterable_v<_Iterable> && !is_view_v<_Iterable>
			 && std::is_same_v<_Ty, value_type_t<_View>>)
		{
			return this->replace(__where, __iterable.begin(), __iterable.end());
		}
		template<typename _View>
		pf_decl_inline pf_decl_constexpr size_t
		replace(
		 size_t __where,
		 _View __v)
			requires(
			 is_view_v<_View>
			 && std::is_same_v<_Ty, value_type_t<_View>>)
		{
			return this->replace(__where, __v.begin(), __v.end());
		}
		pf_decl_inline pf_decl_constexpr size_t
		replace(
		 size_t __where,
		 initializer_list<_Ty> __list)
		{
			return this->replace(__where, iterator(__list.begin()), iterator(__list.end()));
		}
		pf_decl_inline pf_decl_constexpr size_t
		replace(
		 size_t __wbeg,
		 size_t __wend,
		 const _Ty &__val,
		 size_t __count)
		{
			return this->__replace(__wbeg, __wend, __val, __count);
		}
		template<typename _IteratorIn>
		pf_decl_inline pf_decl_constexpr size_t
		replace(
		 size_t __wbeg,
		 size_t __wend,
		 _IteratorIn __ibeg,
		 _IteratorIn __iend)
			requires(is_iterator_v<_IteratorIn>)
		{
			return this->__replace(__wbeg, __wend, __ibeg, __iend);
		}
		template<typename _View>
		pf_decl_inline pf_decl_constexpr size_t
		replace(
		 size_t __wbeg,
		 size_t __wend,
		 _View __v)
			requires(
			 is_view_v<_View>
			 && std::is_same_v<_Ty, value_type_t<_View>>)
		{
			return this->replace(__wbeg, __wend, __v.begin(), __v.end());
		}
		pf_decl_inline pf_decl_constexpr size_t
		replace(
		 size_t __wbeg,
		 size_t __wend,
		 initializer_list<_Ty> __list)
		{
			return this->replace(__wbeg, __wend, iterator_t(__list.begin()), iterator_t(__list.end()));
		}


		/// Reinsert
		template<typename... _Args>
		pf_decl_inline pf_decl_constexpr iterator_t
		reinsert(
		 iterator_t __where,
		 _Args &&...__args)
			requires(std::is_constructible_v<_Ty, _Args...>)
		{
			return this->begin() + this->__replace(this->__iterator_to_index(__where), std::forward<_Args>(__args)...);
		}
		pf_decl_inline pf_decl_constexpr iterator_t
		reinsert(
		 iterator_t __where,
		 const _Ty &__val,
		 size_t __count)
		{
			return this->begin() + this->__replace(this->__iterator_to_index(__where), __val, __count);
		}
		template<typename _IteratorIn>
		pf_decl_inline pf_decl_constexpr iterator_t
		reinsert(
		 iterator_t __where,
		 _IteratorIn __beg,
		 _IteratorIn __end)
			requires(is_iterator_v<_IteratorIn>)
		{
			return this->begin() + this->__replace(this->__iterator_to_index(__where), __beg, __end);
		}
		template<typename _View>
		pf_decl_inline pf_decl_constexpr iterator_t
		reinsert(
		 iterator_t __where,
		 _View __v)
			requires(
			 is_view_v<_View>
			 && std::is_same_v<_Ty, value_type_t<_View>>)
		{
			return this->reinsert(__where, __v.begin(), __v.end());
		}
		pf_decl_inline pf_decl_constexpr iterator_t
		reinsert(
		 iterator_t __where,
		 initializer_list<_Ty> __list)
		{
			return this->reinsert(__where, iterator(__list.begin()), iterator(__list.end()));
		}
		pf_decl_inline pf_decl_constexpr iterator_t
		reinsert(
		 iterator_t __wbeg,
		 iterator_t __wend,
		 const _Ty &__val,
		 size_t __count)
		{
			return this->begin() + this->__replace(this->__iterator_to_index(__wbeg), this->__iterator_to_index(__wend), __val, __count);
		}
		template<typename _IteratorIn>
		pf_decl_inline pf_decl_constexpr iterator_t
		reinsert(
		 iterator_t __wbeg,
		 iterator_t __wend,
		 _IteratorIn __ibeg,
		 _IteratorIn __iend)
			requires(is_iterator_v<_IteratorIn>)
		{
			return this->begin() + this->__replace(this->__iterator_to_index(__wbeg), this->__iterator_to_index(__wend), __ibeg, __iend);
		}
		template<typename _View>
		pf_decl_inline pf_decl_constexpr iterator_t
		reinsert(
		 iterator_t __wbeg,
		 iterator_t __wend,
		 _View __v)
			requires(
			 is_view_v<_View>
			 && std::is_same_v<_Ty, value_type_t<_View>>)
		{
			return this->reinsert(__wbeg, __wend, __v.begin(), __v.end());
		}
		pf_decl_inline pf_decl_constexpr iterator_t
		reinsert(
		 iterator_t __wbeg,
		 iterator_t __wend,
		 initializer_list<_Ty> __list)
		{
			return this->reinsert(__wbeg, __wend, iterator(__list.begin()), iterator(__list.end()));
		}

		/// Shrink
		pf_decl_constexpr size_t
		shrink(
		 size_t __newCount)
		{
			if(pf_unlikely(__newCount < this->count_))
				__newCount = this->count_;
			if(__newCount < this->capacity_)
				return -this->__reallocate_no_check(__newCount, this->align_);
			return 0;
		}
		pf_decl_inline pf_decl_constexpr size_t
		shrink_to_fit()
		{
			return this->shrink(this->count_);
		}
		pf_decl_inline pf_decl_constexpr size_t
		shrink_to_magnifier()
		{
			return this->shrink(this->magnifier_(this->count_));
		}

		/// Reserve
		pf_decl_inline pf_decl_constexpr size_t
		reserve(
		 size_t __count)
		{
			if(__count > this->capacity_)
			{
				return this->__reallocate_no_check(__count, this->align_);
			}
			return 0;
		}
		pf_decl_inline pf_decl_constexpr size_t
		reserve(
		 size_t __count,
		 align_val_t __align)
		{
			if(__count > this->capacity_)
			{
				return this->__reallocate_no_check(__count, __align);
			}
			if(__align != this->align_)
			{
				this->realign(__align);
			}
			return 0;
		}
		pf_decl_inline pf_decl_constexpr size_t
		reserve_with_magnifier(
		 size_t __count)
		{
			if(__count > this->capacity_)
			{
				return this->__reallocate_no_check(this->magnifier_(__count), this->align_);
			}
			return 0;
		}
		pf_decl_inline pf_decl_constexpr size_t
		reserve_with_magnifier(
		 size_t __count,
		 align_val_t __align)
		{
			if(__count > this->capacity_)
			{
				return this->__reallocate_no_check(this->magnifier_(__count), __align);
			}
			if(__align != this->align_)
			{
				this->realign(__align);
			}
			return 0;
		}

		/// Resize
		pf_decl_inline pf_decl_constexpr size_t
		resize(
		 const _Ty &__val,
		 size_t __count)
		{
			if(__count > this->count_)
			{
				const size_t old = this->count_;
				this->reserve(__count);
				construct(this->begin() + this->count_, __val);
				this->count_ = __count;
				return __count - old;
			}
			return 0;
		}
		pf_decl_inline pf_decl_constexpr size_t
		resize_with_magnifier(
		 const _Ty &__val,
		 size_t __count)
		{
			return this->resize(__val, this->magnifier_(__count));
		}

		/// Realign
		pf_decl_inline pf_decl_constexpr bool
		realign(
		 align_val_t __align)
		{
			const _Ty *p = this->data_;
			if(this->align_ != __align)
				this->__reallocate_no_check(this->capacity_, __align);
			return (this->data_ != p);
		}

		/// Clear
		pf_decl_inline pf_decl_constexpr size_t
		clear() pf_attr_noexcept
		{
			const size_t c = this->count_;
			if(this->data_)
			{
				destroy(this->begin(), this->end());
				deallocate(this->allocator_, this->data_);
				this->data_			= nullptr;
				this->capacity_ = 0;
				this->count_		= 0;
			}
			return c;
		}

		/// Front
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr _Ty &
		front() pf_attr_noexcept
		{
			return *this->begin();
		}
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const _Ty &
		front() const pf_attr_noexcept
		{
			return *this->begin();
		}

		/// Back
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr _Ty &
		back() pf_attr_noexcept
		{
			return *(this->begin() - 1 + this->count_);
		}
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const _Ty &
		back() const pf_attr_noexcept
		{
			return *(this->begin() - 1 + this->count_);
		}

		/// Begin
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr iterator_t
		begin() pf_attr_noexcept
		{
			return this->data_;
		}
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const_iterator_t
		begin() const pf_attr_noexcept
		{
			return this->data_;
		}
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const_iterator_t
		cbegin() const pf_attr_noexcept
		{
			return this->begin();
		}

		/// End
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr iterator_t
		end() pf_attr_noexcept
		{
			return (this->data_ + this->count_);
		}
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const_iterator_t
		end() const pf_attr_noexcept
		{
			return (this->data_ + this->count_);
		}
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const_iterator_t
		cend() const pf_attr_noexcept
		{
			return this->end();
		}

		/// Reverse Begin
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr reverse_iterator_t
		rbegin() pf_attr_noexcept
		{
			return (this->data_ - 1 + this->count_);
		}
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const_reverse_iterator_t
		rbegin() const pf_attr_noexcept
		{
			return (this->data_ - 1 + this->count_);
		}
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const_reverse_iterator_t
		crbegin() const pf_attr_noexcept
		{
			return this->rbegin();
		}

		/// Reverse End
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr reverse_iterator_t
		rend() pf_attr_noexcept
		{
			return (this->data_ - 1);
		}
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const_reverse_iterator_t
		rend() const pf_attr_noexcept
		{
			return (this->data_ - 1);
		}
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const_reverse_iterator_t
		crend() const pf_attr_noexcept
		{
			return this->rend();
		}

		/// Data
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr _Ty *
		data() pf_attr_noexcept
		{
			return this->data_;
		}
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const _Ty *
		data() const pf_attr_noexcept
		{
			return this->data_;
		}

		/// Size
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr size_t
		size() const pf_attr_noexcept
		{
			return this->count_ * sizeof(_Ty);
		}

		/// Count
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr size_t
		count() const pf_attr_noexcept
		{
			return this->count_;
		}

		/// Storage Size
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr size_t
		storage_size() const pf_attr_noexcept
		{
			return this->capacity_ * sizeof(_Ty);
		}

		/// Capacity
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr size_t
		capacity() const pf_attr_noexcept
		{
			return this->capacity_;
		}

		/// Align
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr align_val_t
		alignment() const pf_attr_noexcept
		{
			return this->align_;
		}

		/// View
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr view_t
		view() const pf_attr_noexcept
		{
			return view_t(this->begin(), this->end());
		}

		/// Swap
		pf_decl_inline pf_decl_constexpr void
		swap(
		 sequence<_Ty, _Magnifier, _Allocator> &__other)
		{
			if(pf_likely(this != &__other))
			{
				pul::swap(this->data_, __other.data_);
				pul::swap(this->capacity_, __other.capacity_);
				pul::swap(this->align_, __other.align_);
				pul::swap(this->count_, __other.count_);
				pul::swap(this->magnifier_, __other.magnifier_);
				pul::swap(this->allocator_, __other.allocator_);
			}
		}

		/// Magnifier
		pf_hint_nodiscard pf_decl_inline const _Magnifier &
		magnifier() const pf_attr_noexcept
		{
			return this->magnifier_;
		}

		/// Allocator
		pf_hint_nodiscard pf_decl_inline const _Allocator &
		allocator() const pf_attr_noexcept
		{
			return this->allocator_;
		}

		/// Is Allocated
		pf_hint_nodiscard pf_decl_inline bool
		is_allocated() const pf_attr_noexcept
		{
			return this->capacity_ != 0;
		}

		/// Is Empty
		pf_hint_nodiscard pf_decl_inline bool
		is_empty() const pf_attr_noexcept
		{
			return this->count_ == 0;
		}

	private:
		_Ty *data_;
		size_t capacity_;
		align_val_t align_;
		size_t count_;
		pf_hint_nounique_address _Magnifier magnifier_;
		pf_hint_nounique_address _Allocator allocator_;
	};

	/// ITERABLE: Sequence -> CTADs
	// iterator
	template<typename _IteratorIn>
		requires(is_iterator_v<_IteratorIn>)
	sequence(
	 _IteratorIn,
	 _IteratorIn)
	 -> sequence<
		std::remove_const_t<typename _IteratorIn::value_t>,
		magnifier_default,
		allocator_halloc>;
	template<typename _IteratorIn>
		requires(is_iterator_v<_IteratorIn>)
	sequence(
	 _IteratorIn,
	 _IteratorIn,
	 align_val_t)
	 -> sequence<
		std::remove_const_t<typename _IteratorIn::value_t>,
		magnifier_default,
		allocator_halloc>;
	template<typename _IteratorIn, typename _Magnifier>
		requires(is_iterator_v<_IteratorIn> && is_magnifier_v<_Magnifier>)
	sequence(
	 _IteratorIn,
	 _IteratorIn,
	 align_val_t,
	 _Magnifier &&)
	 -> sequence<
		std::remove_const_t<typename _IteratorIn::value_t>,
		_Magnifier,
		allocator_halloc>;
	template<typename _IteratorIn, typename _Magnifier, typename _Allocator>
		requires(is_iterator_v<_IteratorIn> && is_magnifier_v<_Magnifier> && is_allocator_v<_Allocator>)
	sequence(
	 _IteratorIn,
	 _IteratorIn,
	 align_val_t,
	 _Magnifier &&,
	 _Allocator &&)
	 -> sequence<
		std::remove_const_t<typename _IteratorIn::value_t>,
		_Magnifier,
		_Allocator>;

	// initializer_list
	template<typename _Ty>
	sequence(initializer_list<_Ty>) -> sequence<_Ty>;

	/// ITERABLE: Sequence -> Concepts -> Tests
	pf_assert_static(is_view_v<sequence_view<int32_t>>);
	pf_assert_static(is_container_v<sequence<int32_t>>);


	/// ITERABLE: Singly -> Types
	template<typename _Ty>
		requires(!std::is_const_v<_Ty> && !std::is_void_v<_Ty>)
	class singly_view;
	template<
	 typename _Ty,
	 typename _Magnifier = magnifier_default,
	 typename _Allocator = allocator_halloc>
		requires(!std::is_const_v<_Ty> && !std::is_void_v<_Ty>)
	class singly_list;

	/// ITERABLE: Singly -> View
	template<typename _Ty>
		requires(!std::is_const_v<_Ty> && !std::is_void_v<_Ty>)
	class singly_view
	{
	public:
		using value_t					 = _Ty;
		using const_iterator_t = singly_const_iterator<_Ty>;

		/// Constructors
		pf_decl_inline pf_decl_constexpr
		singly_view() pf_attr_noexcept
			: first_(nullptr)
			, count_(0)
		{}
		pf_decl_inline pf_decl_constexpr
		singly_view(
		 nullptr_t) pf_attr_noexcept
			: singly_view()
		{}
		pf_decl_inline pf_decl_constexpr
		singly_view(
		 const singly_node<_Ty> *__first,
		 size_t __count) pf_attr_noexcept
			: first_(__first)
			, count_(__count)
		{}
		pf_decl_inline pf_decl_constexpr
		singly_view(
		 const singly_node<_Ty> *__first) pf_attr_noexcept
			: singly_view(__first, singly_count(__first))
		{}
		template<
		 typename _Magnifier,
		 typename _Allocator>
		pf_decl_inline pf_decl_constexpr
		singly_view(
		 const singly_list<_Ty, _Magnifier, _Allocator> &__s) pf_attr_noexcept
			: singly_view(__s.begin().node(), __s.count())
		{}
		pf_decl_inline pf_decl_constexpr
		singly_view(
		 const singly_view<_Ty> &__other) pf_attr_noexcept
			: first_(__other.first_)
			, count_(__other.count_)
		{}

		/// Destructor
		pf_decl_inline pf_decl_constexpr ~singly_view() pf_attr_noexcept = default;

		/// Operator =
		pf_decl_inline pf_decl_constexpr singly_view<_Ty> &
		operator=(
		 const singly_view<_Ty> &__other) pf_attr_noexcept
		{
			this->first_ = __other.first_;
			this->count_ = __other.count_;
			return *this;
		}

		/// Operator []
		pf_decl_inline pf_decl_constexpr const _Ty &
		operator[](
		 size_t __index) const
		{
			pf_throw_if(
			 __index >= this->count_,
			 dbg_category_generic(),
			 dbg_code::invalid_argument,
			 dbg_flags::none,
			 "index is out of node view! count={}, index={}",
			 this->count_,
			 __index);
			return *(this->begin() += __index);
		}

		/// Begin
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const_iterator_t
		begin() const pf_attr_noexcept
		{
			return this->first_;
		}
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const_iterator_t
		cbegin() const pf_attr_noexcept
		{
			return this->begin();
		}

		/// End
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const_iterator_t
		end() const pf_attr_noexcept
		{
			return nullptr;
		}
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const_iterator_t
		cend() const pf_attr_noexcept
		{
			return nullptr;
		}

		/// Count
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr size_t
		count() const pf_attr_noexcept
		{
			return this->count_;
		}

		/// Size
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr size_t
		size() const pf_attr_noexcept
		{
			return this->count_ * sizeof(_Ty);
		}

		/// Is Empty
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr bool
		is_empty() const pf_attr_noexcept
		{
			return !this->first_;
		}

	private:
		const singly_node<_Ty> *first_;
		size_t count_;
	};

	/// ITERABLE: Singly -> List
	template<
	 typename _Ty,
	 typename _Magnifier,
	 typename _Allocator>
		requires(!std::is_const_v<_Ty> && !std::is_void_v<_Ty>)
	class singly_list
	{
		template<typename _Uy, typename _MagnifierR, typename _AllocatorR>
			requires(
			 !std::is_const_v<_Uy> && std::is_void_v<_Uy>
			 && is_magnifier_v<_MagnifierR>
			 && is_allocator_v<_AllocatorR>)
		pf_decl_friend class singly_list;

		// Types
		struct __node_base_t;
		struct __node_t;
		struct __buffer_t;

		// Type -> Node Base
		struct __node_base_t
		{
			uint32_t type		: 1;	// 0 == Free; 1 == Alloc;
			uint32_t offset : 31;
		};

		// Type -> Node Data
		struct __node_t
			: public __node_base_t
			, public singly_node<_Ty>
		{};

		// Type -> Buffer
		struct __buffer_t
		{
		public:
			/// Constructors
			pf_decl_inline pf_decl_constexpr
			__buffer_t(
			 size_t __count,
			 size_t __elemsize) pf_attr_noexcept
				: count(__count)
				, prev(nullptr)
				, next(nullptr)
			{
				pf_assert(__count != 0, "__count mustn't be null!");

				// Initialize
				union
				{
					__node_t *as_node;
					byte_t *as_byte;
				};
				as_byte = &this->seq[0u];
				while(__count > 0)
				{
					as_node->offset		= distof(this, as_node);
					as_node->as_byte += __elemsize;
					as_node->next			= nullptr;
					--__count;
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

			/// Data
			const size_t count;
			__buffer_t *prev;
			__buffer_t *next;
			byte_t seq[];
		};

		// Internal
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr size_t
		__node_size() const pf_attr_noexcept
		{
			return sizeof(node_t) + paddingof(sizeof(__node_base_t) + sizeof(singly_node<_Ty> *), this->align_);
		}

		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr singly_node<_Ty> *
		__singly_of(
		 __node_t *__node) const f_attr_noexcept
		{
			union
			{
				__node_t *as_node;
				struct
				{
					__node_base_t *as_node_base;
					singly_node<_Ty> *as_singly;
				};
			};
			as_node = __node;
			return as_singly;
		}

		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr singly_node<_Ty> *
		__next_singly_of(
		 __node_t *__node) const pf_attr_noexcept
		{
			return __node->next;
		}
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr __node_t *
		__next_node_of(
		 __node_t *__node) const pf_attr_noexcept
		{
			union
			{
				__node_t *as_node;
				singly_node<_Ty> *as_singly;
				byte_t *as_byte;
			};
			if(pf_unlikely(!__node->next)) return nullptr;
			as_singly = __node->next;
			as_byte	 -= sizeof(__node_base_t);
			return as_node;
		}

		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr __buffer_t *
		__buffer_of(
		 __node_t *__node) const pf_attr_noexcept
		{
			union
			{
				__node_t *as_node;
				byte_t *as_byte;
				__buffer_t *as_buffer;
			};
			as_node	 = __node;
			as_byte -= as_node->offset;
			return as_buffer;
		}

		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr __buffer_t *
		__make_buffer(
		 size_t __count) const pf_attr_noexcept
		{
			return new_construct_ex_aligned_at<__buffer_t>(
			 this->allocator_,
			 __count * this->__node_size(),
			 this->align_,
			 sizeof(__buffer_t) + sizeof(singly_node<_Ty> *),
			 __count,
			 this->__node_size());
		}

		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr __node_t *
		__search_parent(
		 __node_t *__prev,
		 __node_t *__node) const pf_attr_noexcept
		{
			while(__prev && __prev->next != __node)
			{
				__prev = next_node_of(__prev);
			}
			return __prev;
		}
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr __node_t *
		__alloc_parent(
		 __node_t *__node) const pf_attr_noexcept
		{
			if(__node == this->allocHead_) return nullptr_;
			return this->__search_parent(this->allocHead_, __node);
		}
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr __node_t *
		__free_parent(
		 __node_t *__node) const pf_attr_noexcept
		{
			if(__node == this->freeHead_) return nullptr;
			return this->__search_parent(this->freeHead_, __node);
		}
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr __node_t *
		__first_node(
		 __buffer_t *__buf) const pf_attr_noexcept
		{
			return union_cast<__node_t *>(__buf->seq[0u]);
		}
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr __node_t *
		__last_node(
		 __buffer_t *__buf) const pf_attr_noexcept
		{
			union
			{
				byte_t *as_byte;
				__node_t *as_node;
			};
			as_byte	 = &__buf->seq[0u];
			as_byte += this->__node_size() * (__buf->count - 1);
			return as_node;
		}
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr __node_t *
		__seq_prev_node_nocheck(
		 __node_t *__node) const pf_attr_noexcept
		{
			union
			{
				__node_t *as_node;
				byte_t *as_byte;
			};
			as_node	 = __node;
			as_byte -= this->__node_size();
			return as_node;
		}
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr __node_t *
		__seq_next_node_nocheck(
		 __node_t *__node) const pf_attr_noexcept
		{
			union
			{
				__node_t *as_node;
				byte_t *as_byte;
			};
			as_node	 = __node;
			as_byte += this->__node_size();
			return as_node;
		}
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr __node_t *
		__seq_parent(
		 __node_t const *&__head,
		 __node_t *__node) const pf_attr_noexcept
		{
			__buffer_t *buf = this->__buffer_of(__node);
			__node_t *par		= nullptr;
			do
			{
				par						= this->__last_node(buf);
				__node_t *fst = this->__first_node(buf);
				while(par != fst)
				{
					if(par->next == __node) return par;
					par = this->__seq_prev_node_nocheck(par);
				}
				buf = buf->prev;
			} while(par != __head);
			if(par->next == __node) return par;
			return nullptr;
		}
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr __node_t *
		__seq_alloc_parent(
		 __node_t *__node) const pf_attr_noexcept
		{
			return this->__seq_parent(this->allocHead_, __node);
		}
		pf_hint_nodiscard pf_decl_inline pf_decl_constexpr __node_t *
		__seq_free_parent(
		 __node_t *__node) const pf_attr_noexcept
		{
			return this->__seq_parent(this->freeHead_, __node);
		}
		pf_decl_inline pf_decl_constexpr void
		__link_node(
		 __node_t *&__head,
		 __node_t *&__tail,
		 __node_t *__node,
		 uint32_t __type) pf_attr_noexcept
		{
			if(pf_unlikely(!__head))
			{
				__head = __node;
				__tail = __node;
			}
			else
			{
				__buffer_t *buf = this->__buffer_of(__node);
				if(buf == this->__buffer_of(__tail) && __tail < __node)
				{
					__tail->next = __node;
					__tail			 = __node;
				}
				else
				{
					__buffer_t *prv = buf->prev;
					__node_t *fst		= this->__first_node(buf);
					__node_t *par		= this->__seq_prev_node(__node);
					while(prv && par != fst)
					{
						if(par < fst)
						{
							buf = prv;
							prv = buf->prev;
							fst = this->__first_node(buf);
						}
						if(par->type == __type)
						{
							__node->next = par->next;
							par->next		 = __node;
							return;
						}
						par = this->__seq_prev_node(par);
					}

					// par == fst
					__node->next = __head;
					__head			 = __node;
				}
			}
		}
		pf_decl_inline pf_decl_constexpr void
		__link_alloc_node(
		 __node_t *__node) pf_attr_noexcept
		{
			this->__link_node(this->allocHead_, this->allocTail_, __node, 1);
			__node->type = 1;
		}
		pf_decl_inline pf_decl_constexpr void
		__link_free_node(
		 __node_t *__node) pf_attr_noexcept
		{
			this->__link_node(this->freeHead_, this->freeTail_, __node, 0);
			__node->type = 0;
		}
		pf_decl_inline pf_decl_constexpr void
		__unlink_node(
		 __node_t *&__head,
		 __node_t *&__tail,
		 __node_t *__node) pf_attr_noexcept
		{
			if(__node == __head)
			{
				__node_t *next = __head->next;
				__head->next	 = nullptr;
				__head				 = next;
				if(!__head) __tail = nullptr;
			}
			else
			{
				__node_t *parent = this->__seq_parent(__head, __node);
				if(__node == __tail)
				{
					parent->next = nullptr;
					__tail			 = parent;
				}
				else
				{
					parent->next = __node->next;
				}
			}
		}
		pf_decl_inline pf_decl_constexpr void
		__unlink_alloc_node(
		 __node_t *__node) pf_attr_noexcept
		{
			this->__unlink_node(this->allocHead_, this->allocTail_, __node);
		}
		pf_decl_inline pf_decl_constexpr void
		__unlink_free_node(
		 __node_t *__node) pf_attr_noexcept
		{
			this->__unlink_node(this->freeHead_, this->freeTail_, __node);
		}

	public:
		using value_t					 = _Ty;
		using iterator_t			 = singly_iterator<_Ty>;
		using const_iterator_t = singly_iterator<const _Ty>;
		using view_t					 = singly_view<_Ty>;

		/// Constructors
		pf_decl_inline pf_decl_constexpr
		singly_list(
		 size_t __startCount,
		 align_val_t __align			= ALIGN_DEFAULT,
		 _Magnifier &&__magnifier = _Magnifier(),
		 _Allocator &&__allocator = _Allocator()) pf_attr_noexcept
			: allocHead_(nullptr)
			, allocTail_(nullptr)
			, freeHead_(nullptr)
			, freeTail_(nullptr)
			, startCount_(__startCount)
			, count_(0)
			, align_(__align)
			, magnifier_(std::move(__magnifier))
			, allocator_(std::move(__allocator))
		{}
		pf_decl_inline pf_decl_constexpr
		singly_list(
		 const _Ty &__val,
		 size_t __count,
		 size_t __startCount			= 0,	// 0 == __count
		 align_val_t __align			= ALIGN_DEFAULT,
		 _Magnifier &&__magnifier = _Magnifier(),
		 _Allocator &&__allocator = _Allocator()) pf_attr_noexcept
			: singly_list(__startCount == 0 ? __count : __startCount, __align, std::move(__magnifier), std::move(__allocator))
		{
			// Initialise
			size_t grow			= this->startCount_;
			__buffer_t *buf = this->__make_buffer(grow);
			__node_t *par		= this->__first_node(buf);
			__node_t *lst		= this->__last_node(buf);
			__node_t *cur		= par;

			// Construct first node
			pul::construct(&cur->store, __val);
			cur->type = 1;
			cur = this->__next_node(cur);
			++this->count_;
			this->allocHead_ = cur;

			// Iterate
			while(true)
			{
				// Iterate
				if(this->count_ != __count && cur != lst)
				{
					par->next = this->__singly_of(cur);
					par				= cur;
					pul::construct(&cur->store, __val);
					cur->type = 1;
					cur = this->__next_node(cur);
					++this->count_;
				}

				// Push buffer
				if(this->count_ != __count && cur == lst)
				{
					grow = this->magnifier_(grow);
					buf	 = this->__make_buffer(grow);
					lst	 = this->__last_node(buf);
					cur	 = this->__first_node(buf);
				}
				else
				{
					break;
				}
			}

			// Allocation Ends
			this->allocTail_ = cur;

			// Remaining is free
			cur = this->__next_node(cur);
			if(cur != lst)
			{
				this->freeHead_ = cur;
				this->freeTail_ = cur;
				cur							= this->__next_node(cur);
				while(cur != lst)
				{
					this->freeTail_->next = this->__singly_of(cur);
					this->freeTail_				= this->freeTail_->next;
					cur										= this->__next_node(cur);
				}
			}
			template<typename _IteratorIn>
			pf_decl_inline pf_decl_constexpr
			singly_list(
			 _IteratorIn __beg,
			 _IteratorIn __end,
			 size_t __startCount			= 0,	// 0 == countof(__beg, __end)
			 align_val_t __align			= ALIGN_DEFAULT,
			 _Magnifier &&__magnifier = _Magnifier(),
			 _Allocator &&__allocator = _Allocator())
				: singly_list(__startCount == 0 ? countof(__beg, __end) : __startCount, _align, std::move(__magnifier), std::move(__allocator))
			{
				// TODO
			}
			template<size_t _Num>
			pf_decl_inline pf_decl_constexpr
			singly_list(
			 const _Ty(&__arr)[_Num],
			 size_t __startCount			= 0,	// 0 == _Num
			 align_val_t __align			= ALIGN_DEFAULT,
			 _Magnifier &&__magnifier = _Magnifier(),
			 _Allocator &&__allocator = _Allocator())
				: singly_list(pul::begin(__arr), pul::end(__arr), __startCount == 0 ? _Num : __startCount, __align, std::move(__magnifier), std::move(__allocator))
			{}
			pf_decl_inline pf_decl_constexpr
			singly_list(
			 initializer_list<_Ty> __list,
			 size_t __startCount			= 0,	// 0 == __list.size()
			 align_val_t __align			= ALIGN_DEFAULT,
			 _Magnifier &&__magnifier = _Magnifier(),
			 _Allocator &&__allocator = _Allocator())
				: singly_list(pul::begin(__list), pul::end(__list), __startCount == 0 ? __list.size() : __startCount, __align, std::move(__magnifier), std::move(__allocator))
			{}
			template<typename _Iterable>
			pf_decl_inline pf_decl_constexpr
			singly_list(
			 _Iterable const &__iterable,
			 size_t __startCount			= 0,	// 0 == __iterable.count()
			 align_val_t __align			= ALIGN_DEFAULT,
			 _Magnifier &&__magnifier = _Magnifier(),
			 _Allocator &&__allocator = _Allocator())
				requires(is_iterable_v<_Iterable> && !is_view_v<_Iterable>)
				: singly_list(__iterable.begin(), __iterable.end(), __startCount == 0 ? __iterable.count() : __startCount, __align, std::move(__magnifier), std::move(__allocator))
			{}
			template<typename _View>
			pf_decl_inline pf_decl_constexpr
			singly_list(
			 _View __view,									// NOTE: By value
			 size_t __startCount			= 0,	// 0 == __view.count()
			 align_val_t __align			= ALIGN_DEFAULT,
			 _Magnifier &&__magnifier = _Magnifier(),
			 _Allocator &&__allocator = _Allocator())
				requires(is_view_v<_View>)
				: singly_list(__view.begin(), __view.end(), __startCount == 0 ? __view.count() : __startCount, __align, std::move(__magnifier), std::move(__allocator))
			{}
			pf_decl_inline pf_decl_constexpr
			singly_list(
			 singly_list<_Ty, _Magnifier, _Allocator> const &__other,
			 size_t __startCount,
			 align_val_t __align)
				: singly_list(__other.begin(), __other.end(), __startCount == 0 ? __other.start_count() : __startCount, __align, __other.magnifier_, __other.allocator_)
			{}
			pf_decl_inline pf_decl_constexpr
			singly_list(
			 singly_list<_Ty, _Magnifier, _Allocator> const &__other,
			 size_t __startCount)
				: singly_list(__other.begin(), __other.end(), __startCount == 0 ? __other.start_count() : __startCount, __other.align_, __other.magnifier_, __other.allocator_)
			{}
			pf_decl_inline pf_decl_constexpr
			singly_list(
			 singly_list<_Ty, _Magnifier, _Allocator> const &__other)
				: singly_list(__other.begin(), __other.end(), __other.startCount_, __other.align_, __other.magnifier_, __other.allocator_)
			{}
			pf_decl_inline pf_decl_constexpr
			singly_list(
			 singly_list<_Ty, _Magnifier, _Allocator> && __other) pf_attr_noexcept
				: head_(__other.head_)
				, tail_(__other.tail_)
				, startCount_(__other.startCount_)
				, count_(__other.count_)
				, align_(__other.align_)
				, magnifier_(std::move(__other.magnifier_))
				, allocator_(std::move(__other.allocator_))
			{
				__other.head_	 = nullptr;
				__other.tail_	 = nullptr;
				__other.count_ = 0;
			}
			pf_decl_inline pf_decl_constexpr
			singly_list(
			 singly_list<_Ty, magnifier_default, _Allocator> && __other,
			 _Magnifier && __magnifier) pf_attr_noexcept
				: head_(__other.head_)
				, tail_(__other.tail_)
				, startCount_(__other.startCount_)
				, count_(__other.count_)
				, align_(__other.align_)
				, magnifier_(std::move(__magnifier))
				, allocator_(std::move(__other.allocator_))
			{
				__other.head_	 = nullptr;
				__other.tail_	 = nullptr;
				__other.count_ = 0;
			}

			/// Destructor
			pf_decl_inline pf_decl_constexpr ~singly_list() pf_attr_noexcept
			{
				this->clear();
			}

			/// Operator =
			template<typename _View>
			pf_decl_inline pf_decl_constexpr singly_list<_Ty, _Magnifier, _Allocator> &
			operator=(
			 _View __view)
				requires(is_view_v<_View>)
			{
				this->assign(__view);
				return *this;
			}
			template<typename _Iterable>
			pf_decl_inline pf_decl_constexpr singly_list<_Ty, _Magnifier, _Allocator> &
			operator=(
			 _Iterable const &__iterable)
				requires(is_iterable_v<_Iterable> && !is_view_v<_Iterable>)
			{
				this->assign(__iterable);
				return *this;
			}
			pf_decl_inline pf_decl_constexpr singly_list<_Ty, _Magnifier, _Allocator> &
			operator=(
			 initializer_list<_Ty> __list)
			{
				this->assign(__list);
				return *this;
			}
			template<size_t _Num>
			pf_decl_inline pf_decl_constexpr singly_list<_Ty, _Magnifier, _Allocator> &
			operator=(
			 const _Ty(&__arr)[_Num])
			{
				this->assign(__arr);
				return *this;
			}
			pf_decl_inline pf_decl_constexpr singly_list<_Ty, _Magnifier, _Allocator> &
			operator=(
			 singly_list<_Ty, _Magnifier, _Allocator> const &__other)
			{
				this->assign(__other);
				return *this;
			}
			template<typename _MagnifierR, typename _AllocatorR>
			pf_decl_inline pf_decl_constexpr singly_list<_Ty, _Magnifier, _Allocator> &
			operator=(
			 singly_list<_Ty, _MagnifierR, _AllocatorR> const &__other)
				requires(is_magnifier_v<_MagnifierR> && is_allocator_v<_AllocatorR>)
			{
				this->assign(__other);
				return *this;
			}
			pf_decl_inline pf_decl_constexpr singly_list<_Ty, _Magnifier, _Allocator> &
			operator=(
			 singly_list<_Ty, _Magnifier, _Allocator> &&__other)
			{
				this->assign(std::move(__other));
				return *this;
			}

			/// Operator (View)
			pf_hint_nodiscard pf_decl_inline pf_decl_constexpr
			operator singly_view<_Ty>() const pf_attr_noexcept
			{
				return this->view();
			}

			/// Assign
			pf_decl_constexpr void
			assign(
			 const _Ty &__val,
			 size_t __count)
			{
				// TODO
			}
			template<typename _IteratorIn>
			pf_decl_constexpr void
			assign(
			 _IteratorIn __beg,
			 _IteratorIn __end)
				requires(is_iterator_v<_IteratorIn>)
			{
				// TODO
			}
			pf_decl_inline pf_decl_constexpr void
			assign(
			 singly_list<_Ty, _Magnifier, _Allocator> const &__other)
			{
				if(pf_likely(this != &__other)) this->assign(__other.begin(), __other.end());
			}
			template<typename _MagnifierR, typename _AllocatorR>
			pf_decl_inline pf_decl_constexpr void
			assign(
			 singly_list<_Ty, _MagnifierR, _AllocatorR> const &__other)
			{
				this->assign(__other.begin(), __other.end());
			}
			template<size_t _Num>
			pf_decl_inline pf_decl_constexpr void
			assign(
			 const _Ty(&__arr)[_Num])
			{
				this->assign(pul::begin(__arr), pul::end(__arr));
			}
			template<typename _Iterable>
			pf_decl_inline pf_decl_constexpr void
			assign(
			 _Iterable const &__iterable)
				requires(is_iterable_v<_Iterable> && !is_view_v<_Iterable>)
			{
				this->assign(__iterable.begin(), __iterable.end());
			}
			template<typename _View>
			pf_decl_inline pf_decl_constexpr void
			assign(
			 _View __view)
				requires(is_view_v<_View>)
			{
				this->assign(__view.begin(), __view.end());
			}
			pf_decl_inline pf_decl_constexpr void
			assign(
			 initializer_list<_Ty> __list)
			{
				this->assign(iterator(__list.begin()), iterator(__list.end()));
			}
			pf_decl_constexpr void
			assign(
			 singly_list<_Ty, _Magnifier, _Allocator> && __other) pf_attr_noexcept
			{
				if(pf_likely(this != &__other))
				{
					this->head_				= __other.head_;
					this->tail_				= __other.tail_;
					this->startCount_ = __other.startCount_;
					this->count_			= __other.count_;
					this->align_			= __other.align_;
					this->magnifier_	= std::move(__other.magnifier_);
					this->allocator_	= std::move(__other.allocator_);
				}
			}

			/// Insert
			template<typename... _Args>
			pf_decl_inline pf_decl_constexpr iterator_t
			insert_back(
			 _Args && ...__args)
				requires(std::is_constructible_v<_Ty, _Args...>)
			{
				// TODO
				return nullptr;
			}
			template<typename... _Args>
			pf_decl_inline pf_decl_constexpr iterator_t
			insert_front(
			 _Args && ...__args)
				requires(std::is_constructible_v<_Ty, _Args...>)
			{
				// TODO
				return nullptr;
			}
			template<typename... _Args>
			pf_decl_inline pf_decl_constexpr iterator_t
			insert(
			 _Args && ...__args)
				requires(std::is_constructible_v<_Ty, _Args...>)
			{
				// TODO
				return nullptr;
			}
			template<typename... _Args>
			pf_decl_inline pf_decl_constexpr iterator_t
			insert(
			 iterator_t __prev,
			 _Args && ...__args)
				requires(std::is_constructible_v<_Ty, _Args...>)
			{
				// TODO
				return nullptr;
			}

			pf_decl_inline pf_decl_constexpr iterator_t
			insert_back(
			 const _Ty &__val,
			 size_t __count)
			{
				// TODO
				return nullptr;
			}
			pf_decl_inline pf_decl_constexpr iterator_t
			insert_front(
			 const _Ty &__val,
			 size_t __count)
			{
				// TODO
				return nullptr;
			}
			pf_decl_inline pf_decl_constexpr iterator_t
			insert(
			 const _Ty &__val,
			 size_t __count)
			{
				// TODO
				return nullptr;
			}
			pf_decl_inline pf_decl_constexpr iterator_t
			insert(
			 iterator_t __prev,
			 const _Ty &__val,
			 size_t __count)
			{
				// TODO
				return nullptr;
			}

			template<typename _IteratorIn>
			pf_decl_inline pf_decl_constexpr iterator_t
			insert_back(
			 _IteratorIn __beg,
			 _IteratorIn __end)
				requires(is_iterator_v<_IteratorIn>)
			{
				// TODO
				return nullptr;
			}
			template<typename _IteratorIn>
			pf_decl_inline pf_decl_constexpr iterator_t
			insert_front(
			 _IteratorIn __beg,
			 _IteratorIn __end)
				requires(is_iterator_v<_IteratorIn>)
			{
				// TODO
				return nullptr;
			}
			template<typename _IteratorIn>
			pf_decl_inline pf_decl_constexpr iterator_t
			insert(
			 _IteratorIn __beg,
			 _IteratorIn __end)
				requires(is_iterator_v<_IteratorIn>)
			{
				// TODO
				return nullptr;
			}
			template<typename _IteratorIn>
			pf_decl_inline pf_decl_constexpr iterator_t
			insert(
			 iterator_t __prev,
			 _IteratorIn __beg,
			 _IteratorIn __end)
				requires(is_iterator_v<_IteratorIn>)
			{
				// TODO
				return nullptr;
			}

			template<typename _Iterable>
			pf_decl_inline pf_decl_constexpr iterator_t
			insert_back(
			 _Iterable const &__iterable)
				requires(is_iterable_v<_Iterable> && !is_view_v<_Iterable>)
			{
				return this->insert_back(__iterable.cbegin(), __iterable.cend());
			}
			template<typename _Iterable>
			pf_decl_inline pf_decl_constexpr iterator_t
			insert_front(
			 _Iterable const &__iterable)
				requires(is_iterable_v<_Iterable> && !is_view_v<_Iterable>)
			{
				return this->insert_front(__iterable.cbegin(), __iterable.cend());
			}
			template<typename _Iterable>
			pf_decl_inline pf_decl_constexpr iterator_t
			insert(
			 _Iterable const &__iterable)
				requires(is_iterable_v<_Iterable> && !is_view_v<_Iterable>)
			{
				return this->insert(__iterable.cbegin(), __iterable.cend());
			}
			template<typename _Iterable>
			pf_decl_inline pf_decl_constexpr iterator_t
			insert(
			 iterator_t __prev,
			 _Iterable const &__iterable)
				requires(is_iterable_v<_Iterable> && !is_view_v<_Iterable>)
			{
				return this->insert(__prev, __iterable.cbegin(), __iterable.cend());
			}

			template<typename _View>
			pf_decl_inline pf_decl_constexpr iterator_t
			insert_back(
			 _View __view)
				requires(
				 is_view_v<_View>
				 && std::is_same_v<_Ty, value_type_t<_View>>)
			{
				return this->insert_back(__view.cbegin(), __view.cend());
			}
			template<typename _View>
			pf_decl_inline pf_decl_constexpr iterator_t
			insert_front(
			 _View __view)
				requires(
				 is_view_v<_View>
				 && std::is_same_v<_Ty, value_type_t<_View>>)
			{
				return this->insert_front(__view.cbegin(), __view.cend());
			}
			template<typename _View>
			pf_decl_inline pf_decl_constexpr iterator_t
			insert(
			 _View __view)
				requires(
				 is_view_v<_View>
				 && std::is_same_v<_Ty, value_type_t<_View>>)
			{
				return this->insert(__view.cbegin(), __view.cend());
			}
			template<typename _View>
			pf_decl_inline pf_decl_constexpr iterator_t
			insert(
			 iterator_t __prev,
			 _View __view)
				requires(
				 is_view_v<_View>
				 && std::is_same_v<_Ty, value_type_t<_View>>)
			{
				return this->insert(__prev, __view.cbegin(), __view.cend());
			}

			pf_decl_inline pf_decl_constexpr iterator_t
			insert_back(
			 initializer_list<_Ty> __list)
			{
				return this->insert_back(iterator(__list.begin()), iterator(__list.end()));
			}
			pf_decl_inline pf_decl_constexpr iterator_t
			insert_front(
			 initializer_list<_Ty> __list)
			{
				return this->insert_front(iterator(__list.begin()), iterator(__list.end()));
			}
			pf_decl_inline pf_decl_constexpr iterator_t
			insert(
			 initializer_list<_Ty> __list)
			{
				return this->insert(iterator(__list.begin()), iterator(__list.end()));
			}
			pf_decl_inline pf_decl_constexpr iterator_t
			insert(
			 iterator_t __prev,
			 initializer_list<_Ty> __list)
			{
				return this->insert(__prev, iterator(__list.begin()), iterator(__list.end()));
			}

			/// Remove
			pf_decl_inline pf_decl_constexpr iterator_t
			remove_back()
			{
				// TODO
			}
			pf_decl_inline pf_decl_constexpr iterator_t
			remove_front()
			{
				// TODO
			}
			pf_decl_inline pf_decl_constexpr iterator_t
			remove_after(
			 iterator_t __prev)
			{
				// TODO
			}
			pf_decl_inline pf_decl_constexpr iterator_t
			remove(
			 iterator_t __where)
			{
				// TODO
			}

			/// Reinsert
			pf_decl_inline pf_decl_constexpr iterator_t
			reinsert(
			 iterator_t __where,
			 _Args && ...__args)
			{
				// TODO
				return nullptr;
			}
			pf_decl_inline pf_decl_constexpr iterator_t
			reinsert(
			 iterator_t __where,
			 const _Ty &__val,
			 size_t __count)
			{
				// TODO
				return nullptr;
			}
			template<typename _IteratorIn>
			pf_decl_inline pf_decl_constexpr iterator_t
			reinsert(
			 iterator_t __where,
			 _IteratorIn __beg,
			 _IteratorIn __end)
				requires(is_iterator_v<_IteratorIn>)
			{
				// TODO
				return nullptr;
			}
			template<typename _Iterable>
			pf_decl_inline pf_decl_constexpr iterator_t
			reinsert(
			 iterator_t __where,
			 _Iterable const &__iterable)
				requires(
				 is_iterable_v<_Iterable> && !is_view_v<_Iterable>
				 && std::is_same_v<_Ty, value_type_t<_View>>)
			{
				return this->reinsert(__where, __iterable.begin(), __iterable.end());
			}
			template<typename _View>
			pf_decl_inline pf_decl_constexpr iterator_t
			reinsert(
			 iterator_t __where,
			 _View __view)
				requires(
				 is_view_v<_View>
				 && std::is_same_v<_Ty, value_type_t<_View>>)
			{
				return this->reinsert(__where, __view.begin(), __view.end());
			}
			pf_decl_inline pf_decl_constexpr iterator_t
			reinsert(
			 iterator_t __where,
			 initializer_list<_Ty> __list)
			{
				return this->reinsert(__where, iterator(__list.begin()), iterator(__list.end()));
			}

			pf_decl_inline pf_decl_constexpr iterator_t
			reinsert(
			 iterator_t __wbeg,
			 iterator_t __wend,
			 _Args && ...__args)
			{
				// TODO
				return nullptr;
			}
			pf_decl_inline pf_decl_constexpr iterator_t
			reinsert(
			 iterator_t __wbeg,
			 iterator_t __wend,
			 const _Ty &__val,
			 size_t __count)
			{
				// TODO
				return nullptr;
			}
			template<typename _IteratorIn>
			pf_decl_inline pf_decl_constexpr iterator_t
			reinsert(
			 iterator_t __wbeg,
			 iterator_t __wend,
			 _IteratorIn __beg,
			 _IteratorIn __end)
				requires(is_iterator_v<_IteratorIn>)
			{
				// TODO
				return nullptr;
			}
			template<typename _Iterable>
			pf_decl_inline pf_decl_constexpr iterator_t
			reinsert(
			 iterator_t __wbeg,
			 iterator_t __wend,
			 _Iterable const &__iterable)
				requires(
				 is_iterable_v<_Iterable> && !is_view_v<_Iterable>
				 && std::is_same_v<_Ty, value_type_t<_View>>)
			{
				return this->reinsert(__wbeg, __wend, __iterable.begin(), __iterable.end());
			}
			template<typename _View>
			pf_decl_inline pf_decl_constexpr iterator_t
			reinsert(
			 iterator_t __wbeg,
			 iterator_t __wend,
			 _View __view)
				requires(
				 is_view_v<_View>
				 && std::is_same_v<_Ty, value_type_t<_View>>)
			{
				return this->reinsert(__wbeg, __wend, __view.begin(), __view.end());
			}
			pf_decl_inline pf_decl_constexpr iterator_t
			reinsert(
			 iterator_t __wbeg,
			 iterator_t __wend,
			 initializer_list<_Ty> __list)
			{
				return this->reinsert(__wbeg, __wend, iterator(__list.begin()), iterator(__list.end()));
			}

			/// Shrink
			pf_decl_constexpr size_t
			shrink(
			 size_t __nc)
			{
				// TODO
			}
			pf_decl_inline pf_decl_constexpr size_t
			shrink_to_fit()
			{
				return this->shrink(this->count_);
			}
			pf_decl_inline pf_decl_constexpr size_t
			shrink_to_magnifier()
			{
				return this->shrink(this->magnifier_(this->count_));
			}

			/// Repack
			pf_decl_inline pf_decl_constexpr size_t
			repack() pf_attr_noexcept
			{
				// TODO
				return 0;
			}
			pf_decl_inline pf_decl_constexpr size_t
			repack_to_one()
			{
				// TODO
				return 0;
			}

			/// Reverse
			pf_decl_inline pf_decl_constexpr void
			reverse() pf_attr_noexcept
			{
				// TODO
			}

			/// Reserve
			pf_decl_inline pf_decl_constexpr size_t
			reserve(
			 size_t __count)
			{
				// TODO
				return 0;
			}
			pf_decl_inline pf_decl_constexpr size_t
			reserve(
			 size_t __count,
			 align_val_t __align)
			{
				// TODO
				return 0;
			}
			pf_decl_inline pf_decl_constexpr size_t
			reserve_with_magnifier(
			 size_t __count,
			 align_val_t __align)
			{
				// TODO
				return 0;
			}

			/// Resize
			pf_decl_inline pf_decl_constexpr size_t
			resize(
			 const _Ty &__val,
			 size_t __count)
			{
				// TODO
				return 0;
			}
			pf_decl_inline pf_decl_constexpr size_t
			resize_with_magnifier(
			 const _Ty &__val,
			 size_t __count)
			{
				return this->resize(__val, this->magnifier_(__count));
			}

			/// Realign
			pf_decl_inline pf_decl_constexpr bool
			realign(
			 align_val_t __align)
			{
				// TODO
				return false;
			}

			/// Clear
			pf_decl_inline pf_decl_constexpr size_t
			clear() pf_attr_noexcept
			{
				size_t freed = 0;
				while(this->head_)
				{
					freed						+= this->head_->count * sizeof(__node_t) + sizeof(__buffer_t);
					__buffer_t *next = this->head_->next;
					this->allocator_.deallocate(this->head_);
					this->head_ = next;
				}
				return freed;
			}

			/// Front
			pf_hint_nodiscard pf_decl_inline pf_decl_constexpr _Ty &
			front() pf_attr_noexcept
			{
				return this->head_->head->store;
			}
			pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const _Ty &
			front() const pf_attr_noexcept
			{
				return this->head_->head->store;
			}

			/// Back
			pf_hint_nodiscard pf_decl_inline pf_decl_constexpr _Ty &
			back() pf_attr_noexcept
			{
				return this->tail_->tail->store;
			}
			pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const _Ty &
			back() const pf_attr_noexcept
			{
				return this->tail_->tail->store;
			}

			/// Begin
			pf_hint_nodiscard pf_decl_inline pf_decl_constexpr iterator_t
			begin() pf_attr_noexcept
			{
				return this->head_->head;
			}
			pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const_iterator_t
			begin() const pf_attr_noexcept
			{
				// TODO
			}
			pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const_iterator_t
			cbegin() const pf_attr_noexcept
			{
				// TODO
			}

			/// End
			pf_hint_nodiscard pf_decl_inline pf_decl_constexpr iterator_t
			end() pf_attr_noexcept
			{
				// TODO
			}
			pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const_iterator_t
			end() const pf_attr_noexcept
			{
				// TODO
			}
			pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const_iterator_t
			cend() const pf_attr_noexcept
			{
				// TODO
			}

			/// Size
			pf_hint_nodiscard pf_decl_inline pf_decl_constexpr size_t
			size() const pf_attr_noexcept
			{
				return this->count_ * sizeof(_Ty);
			}

			/// Start Count
			pf_hint_nodiscard pf_decl_inline pf_decl_constexpr size_t
			start_count() const pf_attr_noexcept
			{
				return this->startCount_;
			}

			/// Count
			pf_hint_nodiscard pf_decl_inline pf_decl_constexpr size_t
			count() const pf_attr_noexcept
			{
				return this->count_;
			}

			/// Capacity
			pf_hint_nodiscard pf_decl_inline pf_decl_constexpr size_t
			capacity() const pf_attr_noexcept
			{
				size_t num			= 0;
				__buffer_t *buf = this->head_;
				while(buf)
				{
					num += buf->count;
					buf	 = buf->next;
				}
				return 0;
			}

			/// Storage Size
			pf_hint_nodiscard pf_decl_inline pf_decl_constexpr size_t
			storage_size() const pf_attr_noexcept
			{
				return this->capacity() * sizeof(_Ty);
			}

			/// Alignment
			pf_hint_nodiscard pf_decl_inline pf_decl_constexpr align_val_t
			alignment() const pf_attr_noexcept
			{
				return this->align_;
			}

			/// View
			pf_hint_nodiscard pf_decl_inline pf_decl_constexpr view_t
			view() const pf_attr_noexcept
			{
				return view_t(this->begin(), this->end());
			}

			/// Swap
			pf_decl_inline pf_decl_constexpr void
			swap(
			 singly_list<_Ty, __Magnifier, _Allocator> & __other)
			{
				if(pf_likely(this != &__other))
				{
					pul::swap(this->head_, __other.head_);
					pul::swap(this->tail_, __other.tail_);
					pul::swap(this->startCount_, __other.startCount_);
					pul::swap(this->count_, __other.count_);
					pul::swap(this->align_, __other.align_);
					pul::swap(this->magnifier_, __other.magnifier_);
					pul::swap(this->allocator_, __other.allocator_);
				}
				return *this;
			}

			/// Magnifier
			pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const _Magnifier &
			magnifier() const pf_attr_noexcept
			{
				return this->magnifier_;
			}

			/// Allocator
			pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const _Allocator &
			allocator() const pf_attr_noexcept
			{
				return this->allocator_;
			}

			/// Is Allocated
			pf_hint_nodiscard pf_decl_inline pf_decl_constexpr bool
			is_allocated() const pf_attr_noexcept
			{
				return this->head_;
			}

			/// Is Empty
			pf_hint_nodiscard pf_decl_inline pf_decl_constexpr bool
			is_empty() const pf_attr_noexcept
			{
				return this->count_ == 0;
			}

		private:
			__node_t *allocHead_;
			__node_t *allocTail_;
			__node_t *freeHead_;
			__node_t *freeTail_;
			size_t startCount_;
			size_t count_;
			align_val_t align_;
			pf_hint_nounique_address _Magnifier magnifier_;
			pf_hint_nounique_address _Allocator allocator_;
		};

		/// ITERABLE: Singly -> Unbuffered List
		template<
		 typename _Ty,
		 typename _Allocator>
			requires(!std::is_const_v<_Ty> && !std::is_void_v<_Ty>)
		class singly_list<_Ty, magnifier_default, _Allocator>	 // NOTE: Less efficient than buffered version!
		{
			/// Type -> Node
			using __node_t = singly_node<_Ty>;

			/// Internal
			template<typename... _Args>
			pf_hint_nodiscard pf_decl_inline pf_decl_constexpr __node_t *
			__create_node(
			 _Args &&...__args)
			{
				return new_construct_aligned_at_ex<__node_t>(this->allocator_, this->align_, sizeof(singly_node<_Ty> *), std::forward<_Args>(__args)...);
			}
			pf_decl_inline pf_decl_constexpr void
			__generate_nodes(
			 const _Ty &__val,
			 size_t __count,
			 __node_t *&__nbeg,
			 __node_t *&__nlst) pf_attr_noexcept
			{
				__nbeg = this->__create_node(__val);
				__nlst = __nbeg;
				--__count;
				while(__count > 0)
				{
					__nlst->next = this->__create_node(__val);
					__nlst			 = __nlst->next;
					--__count;
				}
			}
			pf_decl_inline pf_decl_constexpr void
			__generate_nodes_reverse(
			 const _Ty &__val,
			 size_t __count,
			 __node_t *&__nbeg,
			 __node_t *&__nlst) pf_attr_noexcept
			{
				__nbeg = this->__create_node(__val);
				__nlst = __nbeg;
				--__count;
				while(__count > 0)
				{
					__node_t *node = this->__create_node(__val);
					node->next		 = __nbeg;
					__nbeg				 = node;
					--__count;
				}
			}
			template<typename _IteratorIn>
			pf_decl_inline pf_decl_constexpr void
			__generate_nodes(
			 _IteratorIn __beg,
			 _IteratorIn __end,
			 size_t &__count,
			 __node_t *&__nbeg,
			 __node_t *&__nlst) pf_attr_noexcept
			{
				__nbeg = this->__create_node(*__beg);
				__nlst = __nbeg;
				++__beg;
				++__count;
				while(__beg != __end)
				{
					__nlst->next = this->__create_node(*__beg);
					__nlst			 = __nlst->next;
					++__beg;
					++__count;
				}
			}
			template<typename _IteratorIn>
			pf_decl_inline pf_decl_constexpr void
			__generate_nodes_reverse(
			 _IteratorIn __beg,
			 _IteratorIn __end,
			 size_t &__count,
			 __node_t *&__nbeg,
			 __node_t *&__nlst) pf_attr_noexcept
			{
				__nbeg = this->__create_node(*__beg);
				__nlst = __nbeg;
				++__beg;
				++__count;
				while(__beg != __end)
				{
					__node_t *node = this->__create_node(*__beg);
					node->next		 = __nbeg;
					__nbeg				 = node;
					++__beg;
					++__count;
				}
			}

			pf_decl_inline pf_decl_constexpr void
			__link_node_back(
			 __node_t *__node) pf_attr_noexcept
			{
				if(pf_unlikely(!this->head_))
				{
					this->head_ = __node;
					this->tail_ = __node;
				}
				else
				{
					this->tail_->next = __node;
					this->tail_				= __node;
				}
			}
			pf_decl_inline pf_decl_constexpr void
			__link_node_back(
			 __node_t *__beg,
			 __node_t *__lst) pf_attr_noexcept
			{
				if(pf_unlikely(!this->head_))
				{
					this->head_ = __beg;
					this->tail_ = __lst;
				}
				else
				{
					this->tail_->next = __beg;
					this->tail_				= __lst;
				}
			}
			pf_decl_inline pf_decl_constexpr void
			__link_node_front(
			 __node_t *__node) pf_attr_noexcept
			{
				if(pf_unlikely(!this->head_))
				{
					this->head_ = __node;
					this->tail_ = __node;
				}
				else
				{
					__node->next = this->head_;
					this->head_	 = __node;
				}
			}
			pf_decl_inline pf_decl_constexpr void
			__link_node_front(
			 __node_t *__beg,
			 __node_t *__lst) pf_attr_noexcept
			{
				if(pf_unlikely(!this->head_))
				{
					this->head_ = __beg;
					this->tail_ = __lst;
				}
				else
				{
					__lst->next = this->head_;
					this->head_ = __beg;
				}
			}
			pf_decl_inline pf_decl_constexpr void
			__link_node_after(
			 __node_t *__prev,
			 __node_t *__node) pf_attr_noexcept
			{
				if(pf_unlikely(!__prev))
				{
					if(pf_unlikely(!this->head_))
					{
						this->head_ = __node;
						this->tail_ = __node;
					}
					else
					{
						this->tail_->next = __node;
						this->tail_				= __node;
					}
				}
				else
				{
					__node->next = __prev->next;
					__prev->next = __node;
					if(this->tail_ == __prev) this->tail_ = __node;
				}
			}
			pf_decl_inline pf_decl_constexpr void
			__link_node_after(
			 __node_t *__prev,
			 __node_t *__beg,
			 __node_t *__lst) pf_attr_noexcept
			{
				if(pf_unlikely(!__prev))
				{
					if(pf_unlikely(!this->head_))
					{
						this->head_ = __beg;
						this->tail_ = __lst;
					}
					else
					{
						this->tail_->next = __beg;
						this->tail_				= __lst;
					}
				}
				else
				{
					__lst->next	 = __prev->next;
					__prev->next = __beg;
					if(this->tail_ == __prev) this->tail_ = __lst;
				}
			}

		public:
			using value_t					 = _Ty;
			using iterator_t			 = singly_iterator<_Ty>;
			using const_iterator_t = singly_iterator<const _Ty>;
			using view_t					 = singly_view<_Ty>;

			/// Constructors
			pf_decl_inline pf_decl_constexpr
			singly_list(
			 align_val_t __align			= pf_alignof(_Ty),
			 _Allocator &&__allocator = _Allocator()) pf_attr_noexcept
				: head_(nullptr)
				, tail_(nullptr)
				, count_(0)
				, elemAlign_(__align)
				, allocator_(std::move(__allocator))
			{}
			pf_decl_inline pf_decl_constexpr
			singly_list(
			 const _Ty &__val,
			 size_t __count,
			 align_val_t __align			= pf_alignof(_Ty),
			 _Allocator &&__allocator = _Allocator())
				: singly_list(__align, std::move(__allocator))
			{
				if(pf_likely(__count > 0))
				{
					this->head_ = this->__create_node(__val);
					this->tail_ = this->head_;
					--__count;
					while(__count > 0)
					{
						this->tail_->next = this->__create_node(__val);
						this->tail_				= this->tail_->next;
						--__count;
					}
					this->count_ = __count;
				}
			}
			template<typename _IteratorIn>
			pf_decl_inline pf_decl_constexpr
			singly_list(
			 _IteratorIn __beg,
			 _IteratorIn __end,
			 align_val_t __align			= pf_alignof(_Ty),
			 _Allocator &&__allocator = _Allocator())
				: singly_list(__align, std::move(__allocator))
			{
				if(pf_likely(__beg != __end))
				{
					__node_t *c = this->__create_node(*__beg);
					this->head_ = c;
					this->tail_ = c;
					++this->count_;
					++__beg;
					while(__beg != __end)
					{
						this->tail_->next = this->__create_node(*__beg);
						this->tail_				= this->tail_->next;
						++this->count_;
						++__beg;
					}
				}
			}
			template<size_t _Num>
			pf_decl_inline pf_decl_constexpr
			singly_list(
			 const _Ty (&__arr)[_Num],
			 align_val_t __align			= pf_alignof(_Ty),
			 _Allocator &&__allocator = _Allocator())
				: singly_list(pul::begin(__arr), pul::end(__arr), __align, std::move(__allocator))
			{}
			pf_decl_inline pf_decl_constexpr
			singly_list(
			 initializer_list<_Ty> __list,
			 align_val_t __align			= pf_alignof(_Ty),
			 _Allocator &&__allocator = _Allocator())
				: singly_list(pul::begin(__list), pul::end(__list), __align, std::move(__allocator))
			{}
			template<typename _Iterable>
				requires(is_iterable_v<_Iterable> && !is_view_v<_Iterable>)
			pf_decl_inline pf_decl_constexpr
			singly_list(
			 _Iterable const &__iterable,
			 align_val_t __align			= pf_alignof(_Ty),
			 _Allocator &&__allocator = _Allocator())
				: singly_list(__iterable.begin(), __iterable.end(), __align, std::move(__allocator))
			{}
			pf_decl_inline pf_decl_constexpr
			singly_list(
			 singly_list<_Ty, magnifier_default, _Allocator> &&__other) pf_attr_noexcept
				: head_(std::move(__other.head_))
				, tail_(std::move(__other.tail_))
				, count_(std::move(__other.count_))
				, allocator_(std::move(__other.allocator_))
			{
				__other.head_	 = nullptr;
				__other.tail_	 = nullptr;
				__other.count_ = 0;
			}

			/// Destructor
			pf_decl_inline pf_decl_constexpr ~singly_list() pf_attr_noexcept
			{
				this->clear();
			}

			/// Operator =
			template<typename _View>
			pf_decl_inline pf_decl_constexpr singly_list<_Ty, magnifier_default, _Allocator> &
			operator=(
			 _View __view)
				requires(is_view_v<_View>)
			{
				this->assign(__view);
				return *this;
			}
			template<typename _Iterable>
			pf_decl_inline pf_decl_constexpr singly_list<_Ty, magnifier_default, _Allocator> &
			operator=(
			 _Iterable const &__iterable)
				requires(is_iterable_v<_Iterable> && !is_view_v<_Iterable>)
			{
				this->assign(__iterable);
				return *this;
			}
			pf_decl_inline pf_decl_constexpr singly_list<_Ty, magnifier_default, _Allocator> &
			operator=(
			 initializer_list<_Ty> __list)
			{
				this->assign(__list);
				return *this;
			}
			template<size_t _Num>
			pf_decl_inline pf_decl_constexpr singly_list<_Ty, magnifier_default, _Allocator> &
			operator=(
			 const _Ty (&__arr)[_Num])
			{
				this->assign(__arr);
				return *this;
			}
			pf_decl_inline pf_decl_constexpr singly_list<_Ty, magnifier_default, _Allocator> &
			operator=(
			 singly_list<_Ty, magnifier_default, _Allocator> const &__other)
			{
				this->assign(__other);
				return *this;
			}
			template<typename _MagnifierR, typename _AllocatorR>
			pf_decl_inline pf_decl_constexpr singly_list<_Ty, _Magnifier, _Allocator> &
			operator=(
			 singly_list<_Ty, _MagnifierR, _AllocatorR> const &__other)
				requires(is_magnifier_v<_MagnifierR> && is_allocator_v<_AllocatorR>)
			{
				this->assign(__other);
				return *this;
			}
			pf_decl_inline pf_decl_constexpr singly_list<_Ty, magnifier_default, _Allocator> &
			operator=(
			 singly_list<_Ty, magnifier_default, _Allocator> &&__other)
			{
				this->assign(std::move(__other));
				return *this;
			}

			/// Operator (View)
			pf_hint_nodiscard pf_decl_inline pf_decl_constexpr
			operator singly_view<_Ty>() const pf_attr_noexcept
			{
				return this->view();
			}

			/// Assign
			pf_decl_constexpr void
			assign(
			 const _Ty &__val,
			 size_t __count)
			{
				if(this->count_ < __count)
				{
					iterator_t cur = this->begin();
					for(size_t i = 0; i < this->count_; ++i, ++cur) pul::assign(cur, __val);
					++cur;

					if(pf_unlikely(!this->head_))
					{
						this->head_ = this->__create_node(__val);
						this->tail_ = this->head_;
						++this->count_;
					}
					while(this->count_ < __count)
					{
						this->tail_->next = this->__create_node(__val);
						this->tail_				= this->tail_->next;
						++this->count_;
					}
				}
				else
				{
					iterator_t cur = this->begin();
					for(size_t i = 0; i < this->count_; ++i, ++cur) pul::assign(cur, __val);
					++cur;

					while(cur)
					{
						iterator_t next = cur->next;
						destroy_delete(this->allocator_, cur);
						cur = next;
					}
				}
				this->count_ = __count;
			}
			template<typename _IteratorIn>
			pf_decl_constexpr void
			assign(
			 _IteratorIn __beg,
			 _IteratorIn __end,
			 align_val_t __align)
				requires(is_iterator_v<_IteratorIn>)
			{
				auto cur = this->begin();
				size_t i = 0;
				for(; __beg != __end && i < this->count_; ++i, ++cur, ++__beg)
				{
					pul::assign(cur, *__beg);
				}
				if(i < this->count_)
				{
					while(cur)
					{
						destroy_delete(this->allocator_, cur);
						cur = cur->next;
					}
					this->count_ = i;
				}
				else	// __beg != __end
				{
					if(pf_unlikely(!this->head_))
					{
						this->head_ = this->__create_node(*__beg);
						this->tail_ = this->head_;
						++this->count_;
						++__beg;
					}
					while(__beg != __end)
					{
						this->tail_->next = this->__create_node(*__beg);
						this->tail_				= this->tail_->next;
						++this->count_;
						++__beg;
					}
				}
			}
			pf_decl_inline pf_decl_constexpr void
			assign(
			 singly_list<_Ty, _Magnifier, _Allocator> const &__other)
			{
				if(pf_likely(this != &__other)) this->assign(__other.begin(), __other.end());
			}
			template<typename _MagnifierR, typename _AllocatorR>
			pf_decl_inline pf_decl_constexpr void
			assign(
			 singly_list<_Ty, _MagnifierR, _AllocatorR> const &__other)
			{
				this->assign(__other.begin(), __other.end());
			}
			template<size_t _Num>
			pf_decl_inline pf_decl_constexpr void
			assign(
			 const _Ty (&__arr)[_Num])
			{
				this->assign(pul::begin(__arr), pul::end(__arr));
			}
			template<typename _Iterable>
			pf_decl_inline pf_decl_constexpr void
			assign(
			 _Iterable const &__iterable)
				requires(is_iterable_v<_Iterable> && !is_view_v<_Iterable>)
			{
				this->assign(__iterable.begin(), __iterable.end());
			}
			template<typename _View>
			pf_decl_inline pf_decl_constexpr void
			assign(
			 _View __view, )
				requires(is_view_v<_View>)
			{
				this->assign(__view.begin(), __view.end());
			}
			pf_decl_inline pf_decl_constexpr void
			assign(
			 initializer_list<_Ty> __list)
			{
				this->assign(iterator(__list.begin()), iterator(__list.end()));
			}
			pf_decl_constexpr void
			assign(
			 singly_list<_Ty, magnifier_default, _Allocator> &&__other) pf_attr_noexcept
			{
				if(pf_likely(this != &__other))
				{
					this->clear();
					this->first_		 = __other.first_;
					this->last_			 = __other.last_;
					this->count_		 = __other.count_;
					this->allocator_ = std::move(__other.allocator_);
					__other.first_	 = nullptr;
					__other.last_		 = nullptr;
					__other.count_	 = 0;
				}
			}

			/// Insert
			template<typename... _Args>
			pf_decl_inline pf_decl_constexpr iterator_t
			insert_back(
			 _Args &&...__args)
				requires(std::is_constructible_v<_Ty, _Args...>)
			{
				__node_t *node = this->__create_node(std::forward<_Args>(__args)...);
				this->__link_node_back(node);
				++this->count_;
				return node;
			}
			template<typename... _Args>
			pf_decl_inline pf_decl_constexpr iterator_t
			insert_front(
			 _Args &&...__args)
				requires(std::is_constructible_v<_Ty, _Args...>)
			{
				__node_t *node = this->__create_node(std::forward<_Args>(__args)...);
				this->__link_node_front(node);
				++this->count_;
				return node;
			}
			template<typename... _Args>
			pf_decl_inline pf_decl_constexpr iterator_t
			insert_after(
			 iterator_t __prev,
			 _Args &&...__args)
				requires(std::is_constructible_v<_Ty, _Args...>)
			{
				__node_t *node = this->__create_node(std::forward<_Args>(__args)...);
				this->__link_node_after(__prev, node);
				++this->count_;
				return node;
			}

			pf_decl_constexpr iterator_t
			insert_back(
			 const _Ty &__val,
			 size_t __count)
			{
				if(pf_likely(__count > 0))
				{
					__node_t *nbeg, *nlst;
					this->__generate_nodes(__val, __count, nbeg, nlst);
					this->__link_node_back(nbeg, nlst);
					this->count_ += __count;
					return nlst;
				}
				return nullptr;
			}
			pf_decl_constexpr iterator_t
			insert_front(
			 const _Ty &__val,
			 size_t __count)
			{
				if(pf_likely(__count > 0))
				{
					__node_t *nbeg, *nlst;
					this->__generate_nodes_reverse(__val, __count, nbeg, nlst);
					this->__link_node_front(nbeg, nlst);
					this->count_ += __count;
					return nbeg;
				}
				return nullptr;
			}
			pf_decl_constexpr iterator_t
			insert_after(
			 iterator_t __prev,
			 const _Ty &__val,
			 size_t __count)
			{
				if(pf_likely(__count > 0))
				{
					__node_t *nbeg, *nlst;
					this->__generate_nodes(__val, __count, nbeg, nlst);
					this->__link_node_after(__prev, nbeg, nlst);
					this->count_ += __count;
					return nlst;
				}
				return nullptr;
			}

			template<typename _IteratorIn>
			pf_decl_constexpr iterator_t
			insert_back(
			 _IteratorIn __beg,
			 _IteratorIn __end)
				requires(is_iterator_v<_IteratorIn>)
			{
				if(pf_unlikely(__beg != __end))
				{
					size_t ncount = 0;
					__node_t *nbeg, *nlst;
					this->__generate_nodes(__beg, __end, ncount, nbeg, nlst);
					this->__link_node_back(nbeg, nlst);
					this->count_ += ncount;
					return nlst;
				}
				return nullptr;
			}
			template<typename _IteratorIn>
			pf_decl_inline pf_decl_constexpr iterator_t
			insert_front(
			 _IteratorIn __beg,
			 _IteratorIn __end)
				requires(is_iterator_v<_IteratorIn>)
			{
				if(pf_unlikely(__beg != __end))
				{
					size_t ncount = 0;
					__node_t *nbeg, *nlst;
					this->__generate_nodes_reverse(__beg, __end, ncount, nbeg, nend);
					this->__link_node_front(nbeg, nlst);
					this->count_ += ncount;
					return nbeg;
				}
				return nullptr;
			}
			template<typename _IteratorIn>
			pf_decl_inline pf_decl_constexpr iterator_t
			insert_after(
			 iterator_t __prev,
			 _IteratorIn __beg,
			 _IteratorIn __end)
				requires(is_iterator_v<_IteratorIn>)
			{
				if(pf_unlikely(__beg != __end))
				{
					size_t ncount = 0;
					__node_t *nbeg, *nlst;
					this->__generate_nodes(__beg, __end, ncount, nbeg, nlst);
					this->__link_node_after(__prev, nbeg, nlst);
					this->count_ += ncount;
					return nlst;
				}
				return nullptr;
			}

			template<typename _Iterable>
			pf_decl_inline pf_decl_constexpr iterator_t
			insert_back(
			 _Iterable const &__iterable)
				requires(is_iterable_v<_Iterable> && !is_view_v<_Iterable>)
			{
				return this->insert_back(__iterable.cbegin(), __iterable.cend());
			}
			template<typename _Iterable>
			pf_decl_inline pf_decl_constexpr iterator_t
			insert_front(
			 _Iterable const &__iterable)
				requires(is_iterable_v<_Iterable> && !is_view_v<_Iterable>)
			{
				return this->insert_front(__iterable.cbegin(), __iterable.cend());
			}
			template<typename _Iterable>
			pf_decl_inline pf_decl_constexpr iterator_t
			insert_after(
			 iterator_t __prev,
			 _Iterable const &__iterable)
				requires(is_iterable_v<_Iterable> && !is_view_v<_Iterable>)
			{
				return this->insert_after(__prev, __iterable.cbegin(), __iterable.cend());
			}

			template<typename _View>
			pf_decl_inline pf_decl_constexpr iterator_t
			insert_back(
			 _View __view)
				requires(
				 is_view_v<_View>
				 && std::is_same_v<_Ty, value_type_t<_View>>)
			{
				return this->insert_back(__view.cbegin(), __view.cend());
			}
			template<typename _View>
			pf_decl_inline pf_decl_constexpr iterator_t
			insert_front(
			 _View __view)
				requires(
				 is_view_v<_View>
				 && std::is_same_v<_Ty, value_type_t<_View>>)
			{
				return this->insert_front(__view.cbegin(), __view.cend());
			}
			template<typename _View>
			pf_decl_inline pf_decl_constexpr iterator_t
			insert_after(
			 iterator_t __prev,
			 _View __view)
				requires(
				 is_view_v<_View>
				 && std::is_same_v<_Ty, value_type_t<_View>>)
			{
				return this->insert_after(__prev, __view.cbegin(), __view.cend());
			}

			pf_decl_inline pf_decl_constexpr iterator_t
			insert_back(
			 initializer_list<_Ty> __list)
			{
				return this->insert_back(iterator(__list.begin()), iterator(__list.end()));
			}
			pf_decl_inline pf_decl_constexpr iterator_t
			insert_front(
			 initializer_list<_Ty> __list)
			{
				return this->insert_front(iterator(__list.begin()), iterator(__list.end()));
			}
			pf_decl_inline pf_decl_constexpr iterator_t
			insert_after(
			 iterator_t __prev,
			 initializer_list<_Ty> __list)
			{
				return this->insert_after(__prev, iterator(__list.begin()), iterator(__list.end()));
			}

			/// Remove
			pf_decl_inline pf_decl_constexpr iterator_t
			remove_back() pf_attr_noexcept
			{
				__node_t *p = nullptr, c = this->head_;
				while(c != this->tail_)
				{
					p = c;
					c = c->next;
				}
				destroy_delete(this->allocator_, c);
				this->tail_ = p;
				return p;
			}
			pf_decl_inline pf_decl_constexpr iterator_t
			remove_front() pf_attr_noexcept
			{
				__node_t *n = this->head_->next;
				destroy_delete(this->allocator_, this->head_);
				this->head_ = n;
				return this->head_;
			}
			pf_decl_inline pf_decl_constexpr iterator_t
			remove_after(
			 iterator_t __prev)
			{
				__node_t *p = __prev;
				pf_throw_if(p == this->tail_, dbg_category_generic(), dbg_code::invalid_argument, dbg_flags::none, "__prev cannot be tail!");
				if(pf_unlikely(!p)) return this->remove_front();
				__node_t *t = p->next;
				__node_t *n = t->next;
				destroy_delete(this->allocator_, t);
				p->next = n;
				return p;
			}
			pf_decl_inline pf_decl_constexpr iterator_t
			remove(
			 iterator_t __where)
			{
				__node_t *p = nullptr, c = this->head_;
				while(c != __where)
				{
					p = c;
					c = c->next;
				}
				if(pf_unlikely(!p)) return this->remove_front();
				__node_t *n = c->next;
				destroy_delete(this->allocator_, c);
				p->next = c->next;
				return p;
			}

			/// Reinsert
			pf_decl_inline pf_decl_constexpr iterator_t
			reinsert(
			 iterator_t __where,
			 _Args &&...__args)
			{
				pul::assign(__where, std::forward<_Args>(__args)...);
				return __where;
			}
			pf_decl_inline pf_decl_constexpr iterator_t
			reinsert(
			 iterator_t __where,
			 const _Ty &__val,
			 size_t __count)
			{
				pul::assign(__where, __val);
				return this->insert_after(__where, __val, __count);
			}
			template<typename _IteratorIn>
			pf_decl_inline pf_decl_constexpr iterator_t
			reinsert(
			 iterator_t __where,
			 _IteratorIn __beg,
			 _IteratorIn __end)
				requires(is_iterator_v<_IteratorIn>)
			{
				pul::assign(__where, __beg, __end);
				return this->insert_after(__where, __beg, __end);
			}
			template<typename _Iterable>
			pf_decl_inline pf_decl_constexpr iterator_t
			reinsert(
			 iterator_t __where,
			 _Iterable const &__iterable)
				requires(
				 is_iterable_v<_Iterable> && !is_view_v<_Iterable>
				 && std::is_same_v<_Ty, value_type_t<_View>>)
			{
				return this->reinsert(__where, __iterable.begin(), __iterable.end());
			}
			template<typename _View>
			pf_decl_inline pf_decl_constexpr iterator_t
			reinsert(
			 iterator_t __where,
			 _View __view)
				requires(
				 is_view_v<_View>
				 && std::is_same_v<_Ty, value_type_t<_View>>)
			{
				return this->reinsert(__where, __view.begin(), __view.end());
			}
			pf_decl_inline pf_decl_constexpr iterator_t
			reinsert(
			 iterator_t __where,
			 initializer_list<_Ty> __list)
			{
				return this->reinsert(__where, iterator(__list.begin()), iterator(__list.end()));
			}

			pf_decl_constexpr iterator_t
			reinsert(
			 iterator_t __wbeg,
			 iterator_t __wend,
			 const _Ty &__val,
			 size_t __count)
			{
				if(pf_likely(__count > 0))
				{
					__node_t *p = nullptr, *b = __wbeg, *l = __wend;
					while(b != l && __count > 0)
					{
						p = b;
						pul::assign(b, __val);
						++b;
						--__count;
					}
					if(b != l)
					{
						__node_t *k = b;
						__node_t *n = nullptr;
						while(b != l)
						{
							n = b->next;
							destroy_delete(this->allocator_, b);
							b = n;
						}
						if(!p)
						{
							__node_t *c = this->head_;
							while(c != __wbeg)
							{
								p = c;
								c = c->next;
							}
							if(p)
							{
								p->next = n;
								if(!n) this->tail_ = p;
							}
						}
						return k;
					}
					else if(__count > 0)
					{
						return this->insert_after(p, __val, __count);
					}
					return p;
				}
				return nullptr;
			}
			template<typename _IteratorIn>
			pf_decl_constexpr iterator_t
			reinsert(
			 iterator_t __wbeg,
			 iterator_t __wend,
			 _IteratorIn __beg,
			 _IteratorIn __end)
				requires(is_iterator_v<_IteratorIn>)
			{
				if(pf_likely(__beg != __end))
				{
					__node_t *p = nullptr, *b = __wbeg, *l = __wend;
					while(b != l && __beg != __end)
					{
						p = b;
						pul::assign(b, *__beg);
						++b;
						++__beg;
					}
					if(b != l)
					{
						__node_t *k = b;
						__node_t *n = nullptr;
						while(b != l)
						{
							n = b->next;
							destroy_delete(this->allocator_, b);
							b = n;
						}
						if(!p)
						{
							__node_t *c = this->head_;
							while(c != __wbeg)
							{
								p = c;
								c = c->next;
							}
							if(p)
							{
								p->next = n;
								if(!n) this->tail_ = p;
							}
						}
						return k;
					}
					else if(__beg != __end)
					{
						return this->insert_after(p, __beg, __end);
					}
					return p;
				}
				return nullptr;
			}
			template<typename _Iterable>
			pf_decl_inline pf_decl_constexpr iterator_t
			reinsert(
			 iterator_t __wbeg,
			 iterator_t __wend,
			 _Iterable const &__iterable)
				requires(
				 is_iterable_v<_Iterable> && !is_view_v<_Iterable>
				 && std::is_same_v<_Ty, value_type_t<_View>>)
			{
				return this->reinsert(__wbeg, __wend, __iterable.begin(), __iterable.end());
			}
			template<typename _View>
			pf_decl_inline pf_decl_constexpr iterator_t
			reinsert(
			 iterator_t __wbeg,
			 iterator_t __wend,
			 _View __view)
				requires(
				 is_view_v<_View>
				 && std::is_same_v<_Ty, value_type_t<_View>>)
			{
				return this->reinsert(__wbeg, __wend, __view.begin(), __view.end());
			}
			pf_decl_inline pf_decl_constexpr iterator_t
			reinsert(
			 iterator_t __wbeg,
			 iterator_t __wend,
			 initializer_list<_Ty> __list)
			{
				return this->reinsert(__wbeg, __wend, iterator(__list.begin()), iterator(__list.end()));
			}

			/// Reverse
			pf_decl_inline pf_decl_constexpr void
			reverse() pf_attr_noexcept
			{
				__node_t *t = this->head_;
				__node_t *c = this->head_->next;
				__node_t *p = this->head_;
				while(c)
				{
					__node_t *n = c->next;
					c->next			= p;
					p						= c;
					c						= n;
				}
				this->head_ = p;
				this->tail_ = t;
			}

			/// Resize
			pf_decl_inline pf_decl_constexpr size_t
			resize(
			 const _Ty &__val,
			 size_t __count)
			{
				if(__count > this->count_)
				{
					size_t num = __count - this->count_;
					this->insert_back(__val, num);
					return num;
				}
				return 0;
			}

			/// Clear
			pf_decl_inline pf_decl_constexpr size_t
			clear() pf_attr_noexcept
			{
				size_t num = 0;
				while(this->head_)
				{
					__node_t *n = this->head_->next;
					destroy_delete(this->allocator_, this->head_);
					this->head_ = n;
					++num;
				}
				return num;
			}

			/// Front
			pf_hint_nodiscard pf_decl_inline pf_decl_constexpr _Ty &
			front() pf_attr_noexcept
			{
				return this->head_->store;
			}
			pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const _Ty &
			front() const pf_attr_noexcept
			{
				return this->head_->store;
			}

			/// Back
			pf_hint_nodiscard pf_decl_inline pf_decl_constexpr _Ty &
			back() pf_attr_noexcept
			{
				return this->tail_->store;
			}
			pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const _Ty &
			back() const pf_attr_noexcept
			{
				return this->tail_->store;
			}

			/// Begin
			pf_hint_nodiscard pf_decl_inline pf_decl_constexpr iterator_t
			begin() pf_attr_noexcept
			{
				return this->head_;
			}
			pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const_iterator_t
			begin() const pf_attr_noexcept
			{
				return this->head_;
			}
			pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const_iterator_t
			cbegin() const pf_attr_noexcept
			{
				return this->head_;
			}

			/// End
			pf_hint_nodiscard pf_decl_inline pf_decl_constexpr iterator_t
			end() pf_attr_noexcept
			{
				return nullptr;
			}
			pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const_iterator_t
			end() const pf_attr_noexcept
			{
				return nullptr;
			}
			pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const_iterator_t
			cend() const pf_attr_noexcept
			{
				return nullptr;
			}

			/// Parent
			pf_hint_nodiscard pf_decl_inline pf_decl_constexpr iterator_t
			parent(
			 iterator_t __it) pf_attr_noexcept
			{
				if(__it == this->begin()) return nullptr;
				if(__it == this->end()) return this->tail_;
				iterator_t par = __it;
				while(par->next != __it)
				{
					par = par->next;
				}
				return par;
			}
			pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const_iterator_t
			parent(
			 const_iterator_t __it) pf_attr_noexcept
			{
				if(__it == this->begin()) return nullptr;
				if(__it == this->end()) return this->tail_;
				const_iterator_t par = __it;
				while(par->next != __it)
				{
					par = par->next;
				}
				return par;
			}

			/// Size
			pf_hint_nodiscard pf_decl_inline pf_decl_constexpr size_t
			size() const pf_attr_noexcept
			{
				return this->count_ * sizeof(_Ty);
			}

			/// Count
			pf_hint_nodiscard pf_decl_inline pf_decl_constexpr size_t
			start_count() const pf_attr_noexcept
			{
				return this->startCount_;
			}

			/// Storage Size
			pf_hint_nodiscard pf_decl_inline pf_decl_constexpr size_t
			storage_size() const pf_attr_noexcept
			{
				return this->size();
			}

			/// Capacity
			pf_hint_nodiscard pf_decl_inline pf_decl_constexpr size_t
			capacity() const pf_attr_noexcept
			{
				return this->capacity();
			}

			/// Alignment
			pf_hint_nodiscard pf_decl_inline pf_decl_constexpr align_val_t
			alignment() const pf_attr_noexcept
			{
				return this->elemAlign_;
			}

			/// View
			pf_hint_nodiscard pf_decl_inline pf_decl_constexpr view_t
			view() const pf_attr_noexcept
			{
				return view_t(this->begin(), this->end());
			}

			/// Swap
			pf_decl_inline pf_decl_constexpr void
			swap(
			 singly_list<_Ty, magnifier_default, _Allocator> &__other)
			{
				if(pf_likely(this != &__other))
				{
					pul::swap(this->head_, __other.head_);
					pul::swap(this->tail_, __other.tail_);
					pul::swap(this->count_, __other.count_);
					pul::swap(this->allocator_, __other.allocator_);
				}
				return *this;
			}

			/// Magnifier
			pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const magnifier_default &
			magnifier() const pf_attr_noexcept
			{
				return this->magnifier_;
			}

			/// Allocator
			pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const _Allocator &
			allocator() const pf_attr_noexcept
			{
				return this->allocator_;
			}

			/// Is Allocated
			pf_hint_nodiscard pf_decl_inline pf_decl_constexpr bool
			is_allocated() const pf_attr_noexcept
			{
				return this->head_;
			}

			/// Is Empty
			pf_hint_nodiscard pf_decl_inline pf_decl_constexpr bool
			is_empty() const pf_attr_noexcept
			{
				return this->head_;
			}

		private:
			__node_t *head_;
			__node_t *tail_;
			size_t count_;
			align_val_t elemAlign_;
			pf_hint_nounique_address magnifier_default magnifier_;
			pf_hint_nounique_address _Allocator allocator_;
		};

		/// ITERABLE: Singly -> Alias
		template<
		 typename _Ty,
		 typename _Allocator>
		class singly_unbuffered_list = singly_list<_Ty, magnifier_default, _Allocator>;
		template<typename _Ty>
		class singly_const_iterator = singly_iterator<const _Ty>;

		/// ITERABLE: Singly -> CTADs
		template<typename _IteratorIn>
			requires(is_iterator_v<_IteratorIn>)
		singly_list(
		 _IteratorIn,
		 _IteratorIn)
		 -> singly_list<
			std::remove_const_t<typename _IteratorIn::value_t>,
			magnifier_default,
			allocator_halloc>;
		template<typename _IteratorIn>
			requires(is_iterator_v<_IteratorIn>)
		singly_list(
		 _IteratorIn,
		 _IteratorIn,
		 align_val_t)
		 -> singly_list<
			std::remove_const_t<typename _IteratorIn::value_t>,
			magnifier_default,
			allocator_halloc>;
		template<typename _IteratorIn, typename _Magnifier>
			requires(is_iterator_v<_IteratorIn> && is_magnifier_v<_Magnifier>)
		singly_list(
		 _IteratorIn,
		 _IteratorIn,
		 align_val_t,
		 _Magnifier &&)
		 -> singly_list<
			std::remove_const_t<typename _IteratorIn::value_t>,
			_Magnifier,
			allocator_halloc>;
		template<typename _IteratorIn, typename _Magnifier, typename _Allocator>
			requires(is_iterator_v<_IteratorIn> && is_magnifier_v<_Magnifier> && is_allocator_v<_Allocator>)
		singly_list(
		 _IteratorIn,
		 _IteratorIn,
		 align_val_t,
		 _Magnifier &&,
		 _Allocator &&)
		 -> singly_list<
			std::remove_const_t<typename _IteratorIn::value_t>,
			_Magnifier,
			_Allocator>;

		/// ITERABLE: Singly -> Concepts -> Tests
		pf_assert_static(is_view_v<singly_view<int32_t>>);
		pf_assert_static(is_container_v<singly_list<int32_t>>);


		/// ITERABLE: Doubly -> Types
		template<typename _Ty>
			requires(!std::is_const_v<_Ty> && !std::is_void_v<_Ty>)
		class doubly_view;
		template<
		 typename _Ty,
		 typename _Magnifier = magnifier_default,
		 typename _Allocator = allocator_halloc>
			requires(!std::is_const_v<_Ty> && !std::is_void_v<_Ty>)
		class doubly_list;


		/// ITERABLE: Doubly -> View
		template<typename _Ty>
			requires(!std::is_const_v<_Ty> && !std::is_void_v<_Ty>)
		class doubly_view
		{
		public:
			using value_t									 = _Ty;
			using const_iterator_t				 = doubly_const_iterator<_Ty>;
			using const_reverse_iterator_t = reverse_iterator<const_iterator_t>;

			/// Constructors
			pf_decl_inline pf_decl_constexpr
			doubly_view() pf_attr_noexcept
				: first_(nullptr)
				, last_(nullptr)
				, count_(0)
			{}
			pf_decl_inline pf_decl_constexpr
			doubly_view(
			 nullptr_t) pf_attr_noexcept
				: doubly_view()
			{}
			pf_decl_inline pf_decl_constexpr
			doubly_view(
			 const doubly_node<_Ty> *__first,
			 const doubly_node<_Ty> *__last,
			 size_t __count) pf_attr_noexcept
				: first_(__first)
				, last_(__last)
				, count_(__count)
			{}
			pf_decl_inline pf_decl_constexpr
			doubly_view(
			 const doubly_node<_Ty> *__first,
			 const doubly_node<_Ty> *__last) pf_attr_noexcept
				: doubly_view(__first, __last, doubly_count(__first))
			{}
			pf_decl_inline pf_decl_constexpr
			doubly_view(
			 const doubly_node<_Ty> *__first)
				: first_(__first)
				, last_(__first)
				, count_(1)
			{
				while(this->last_->next)
				{
					this->last_ = this->last_->next;
					++this->count_;
				}
				this->last_ = this->last_;
			}
			template<
			 typename _Magnifier,
			 typename _Allocator>
			pf_decl_inline pf_decl_constexpr
			doubly_view(
			 const doubly_list<_Ty, _Magnifier, _Allocator> &__s) pf_attr_noexcept
				: doubly_view(__s.begin().node(), __s.count())
			{}
			pf_decl_inline pf_decl_constexpr
			doubly_view(
			 doubly_view<_Ty> const &__other) pf_attr_noexcept
				: first_(__other.first_)
				, last_(__other.last_)
				, count_(__other.count_)
			{}

			/// Destructor
			pf_decl_inline pf_decl_constexpr ~doubly_view() pf_attr_noexcept = default;

			/// Operator =
			pf_decl_inline pf_decl_constexpr doubly_view<_Ty> &
			operator=(
			 doubly_view<_Ty> const &__other) pf_attr_noexcept
			{
				this->first_ = __other.first_;
				this->last_	 = __other.last_;
				this->count_ = __other.count_;
				return *this;
			}

			/// Operator []
			pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const _Ty &
			operator[](
			 size_t __index) const
			{
				pf_throw_if(
				 __index >= this->count_,
				 dbg_category_generic(),
				 dbg_code::invalid_argument,
				 dbg_flags::none,
				 "index is out of node view! count={}, index={}",
				 this->count_,
				 __index);
				return *(this->begin() += __index);
			}

			/// Begin
			pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const_iterator_t
			begin() const pf_attr_noexcept
			{
				return this->first_;
			}
			pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const_iterator_t
			cbegin() const pf_attr_noexcept
			{
				return this->first_;
			}

			/// End
			pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const_iterator_t
			end() const pf_attr_noexcept
			{
				return nullptr;
			}
			pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const_iterator_t
			cend() const pf_attr_noexcept
			{
				return nullptr;
			}

			/// Reverse Begin
			pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const_reverse_iterator_t
			rbegin() const pf_attr_noexcept
			{
				return this->last_;
			}
			pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const_reverse_iterator_t
			crbegin() const pf_attr_noexcept
			{
				return this->last_;
			}

			/// Reverse End
			pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const_reverse_iterator_t
			rend() const pf_attr_noexcept
			{
				return nullptr;
			}
			pf_hint_nodiscard pf_decl_inline pf_decl_constexpr const_reverse_iterator_t
			crend() const pf_attr_noexcept
			{
				return nullptr;
			}

			/// Count
			pf_hint_nodiscard pf_decl_inline pf_decl_constexpr size_t
			count() const pf_attr_noexcept
			{
				return this->count_;
			}

			/// Size
			pf_hint_nodiscard pf_decl_inline pf_decl_constexpr size_t
			size() const pf_attr_noexcept
			{
				return this->count_ * sizeof(_Ty);
			}

			/// Is Empty
			pf_hint_nodiscard pf_decl_inline pf_decl_constexpr bool
			is_empty() const pf_attr_noexcept
			{
				return !this->first_;
			}

		private:
			doubly_node<_Ty> *first_;
			doubly_node<_Ty> *last_;
			size_t count_;
		};

		/// ITERABLE: Doubly -> List
		template<
		 typename _Ty,
		 typename _Magnifier,
		 typename _Allocator>
			requires(!std::is_const_v<_Ty> && !std::is_void_v<_Ty>)
		class doubly_list
		{
			template<typename _Uy, typename _MagnifierR, typename _AllocatorR>
				requires(
				 !std::is_const_v<_Uy> && std::is_void_v<_Uy>
				 && is_magnifier_v<_MagnifierR>
				 && is_allocator_v<_AllocatorR>)
			pf_decl_friend class doubly_list;

		public:
			using value_t					 = _Ty;
			using iterator_t			 = doubly_iterator<_Ty>;
			using const_iterator_t = doubly_iterator<const _Ty>;
			using view_t					 = doubly_view<_Ty>;

			/// Constructors
			pf_decl_inline pf_decl_constexpr
			doubly_list(
			 align_val_t __align,
			 _Magnifier &&__magnifier = _Magnifier(),
			 _Allocator &&__allocator = _Allocator()) pf_attr_noexcept
			{}

			/// Destructor

			/// Operator =

			/// Operator (View)

			/// Assign

			/// Insert Front

			/// Insert Back

			/// Insert Pool (Most efficient way)

			/// Insert

			/// Remove

			/// Reinsert

			/// Shrink

			/// Reserve

			/// Resize

			/// Realign

			/// Clear

			/// Front

			/// Back

			/// Begin

			/// End

			/// Reverse Begin

			/// Reverse End

			/// Size

			/// Count

			/// Storage Size

			/// Capacity

			/// Align

			/// View

			/// Swap

			/// Magnifier

			/// Allocator

			/// Is Allocated

			/// Is Empty

		private:
			__buffer_t *head_;
			__buffer_t *tail_;
			size_t startCount_;
			size_t count_;
			align_val_t align_;
			pf_hint_nounique_address _Magnifier magnifier_;
			pf_hint_nounique_address _Allocator allocator_;
		};

		/// ITERABLE: Doubly -> Unbuffered List
		template<
		 typename _Ty,
		 typename _Allocator>
			requires(!std::is_const_v<_Ty> && !std::is_void_v<_Ty>)
		class doubly_list<_Ty, magnifier_default, _Allocator>
		{
			/// Type -> Node
			using __node_t = doubly_node<_Ty>;

		public:
			using value_t					 = _Ty;
			using iterator_t			 = doubly_iterator<_Ty>;
			using const_iterator_t = doubly_iterator<const _Ty>;
			using view_t					 = doubly_view<_Ty>;

			/// Constructors

			/// Destructor

			/// Operator =

			/// Operator (View)

			/// Assign

			/// Insert

			/// Remove

			/// Reinsert

			/// Shrink

			/// Reserve

			/// Resize

			/// Realign

			/// Clear

			/// Front

			/// Back

			/// Begin

			/// End

			/// Reverse Begin

			/// Reverse End

			/// Size

			/// Count

			/// Storage Size

			/// Capacity

			/// Align

			/// View

			/// Swap

			/// Magnifier

			/// Allocator

			/// Is Allocated

			/// Is Empty

		private:
			__node_t *head_;
			__node_t *tail_;
			size_t count_;
			pf_hint_nounique_address _Allocator allocator_;
		};

		/// ITERABLE: Singly -> Alias
		template<
		 typename _Ty,
		 typename _Allocator>
		class doubly_unbuffered_list = doubly_list<_Ty, magnifier_default, _Allocator>;

		/// ITERABLE: Doubly -> CTADs
		// TODO

		/// ITERABLE: Doubly -> Concepts -> Tests
		// TODO doubly_tests

	}			// namespace pul

#endif	// !PULSAR_CONTAINER_HPP
