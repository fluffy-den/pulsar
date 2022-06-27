/*! @file   singly_cds_list.hpp
 *  @author Fluffy (noe.louis-quentin@hotmail.fr)
 *  @brief
 *  @date   18-06-2022
 *
 *  @copyright Copyright (c) 2022 - Pulsar Software
 *
 *  @since 0.1.2
 */

#ifndef PULSAR_SINGLY_CDS_LINKED_LIST_HPP
#define PULSAR_SINGLY_CDS_LINKED_LIST_HPP 1

// Include: Pulsar
#include "pulsar/linked_list/singly_list.hpp"

// Include: C++
#include <atomic>

// Pulsar
namespace pul
{
	/// SINGLY: Types
	template <typename _Ty>
		requires(!std::is_void_v<_Ty>)
	class singly_node;
	template <typename _Ty>
	class singly_iterator;

	// h == t == nullptr -> Empty
	// !h && !t -> Empty
	// 1. h && !t -> 2. h = nullptr (removal)
	// 1. !h && t -> 2. h = t				(insertion)

	/// SINGLY: CDS Linked LIFO
	template <typename _Ty>
	class singly_cds_linked_lifo
	{
	public:
		using node					 = singly_node<_Ty>;
		using iterator			 = singly_iterator<_Ty>;
		using const_iterator = singly_const_iterator<_Ty>;

		/// Constructors
		pf_decl_constexpr pf_decl_explicit singly_cds_linked_lifo() pf_attr_noexcept
				: cacheline1_{ '\0' }
				, head_(nullptr)
				, cacheline2_{ '\0' }
				, tail_(nullptr)
				, cacheline3_{ '\0' }
		{}
		singly_cds_linked_lifo(
				singly_cds_linked_lifo const &) = delete;
		pf_decl_explicit singly_cds_linked_lifo(
				singly_cds_linked_lifo &&__r)
				: cacheline1_{ '\0' }
				, head_(__r.clear())
				, cacheline2_{ '\0' }
				, tail_(singly_last(this->head_.load(std::memory_order::relaxed)))
				, cacheline3_{ '\0' }
		{}

		/// Operator=
		singly_cds_linked_lifo &operator=(
				singly_cds_linked_lifo const &) = delete;
		singly_cds_linked_lifo &operator=(
				singly_cds_linked_lifo &&__r) pf_attr_noexcept
		{
			this->head_ = __r.clear();
			this->tail_ = singly_last(this->head_.load(std::memory_order::relaxed));
			return *this;
		}

		/// Insert
		node *insert_tail(
				node *__n) pf_attr_noexcept
		{
			// Tail
			node *t = this->tail_.load(std::memory_order::relaxed);
			while (!this->tail_.compare_exchange_weak(
					t,
					__n,
					std::memory_order::relaxed,
					std::memory_order::consume))
				;
			// Barrier
			std::atomic_thread_fence(std::memory_order::release);
			if (t)
			{
				t->next_ = __n;
			}
			// Head
			node *h = this->head_.load(std::memory_order::relaxed);
			if (!h)
			{
				this->head_.compare_exchange_strong(
						h,
						__n,
						std::memory_order::release,
						std::memory_order::relaxed);
			}
			return t;
		}
		template <typename _InIterator>
		node *insert_tail_bulk(
				_InIterator __beg,
				_InIterator __end) pf_attr_noexcept
		{
			// Tail
			node *e = singly_link(__beg, __end);
			node *t = this->tail_.load(std::memory_order::relaxed);
			while (!this->tail_.compare_exchange_weak(
					t,
					e,
					std::memory_order::relaxed,
					std::memory_order::consume))
				;
			// Barrier
			std::atomic_thread_fence(std::memory_order::release);
			if (t)
			{
				t->next_ = &(*__beg);
			}
			// Head
			node *h = this->head_.load(std::memory_order::relaxed);
			if (!h)
			{
				this->head_.compare_exchange_strong(
						h,
						&(*__beg),
						std::memory_order::release,
						std::memory_order::relaxed);
			}
			return t;
		}

		/// Remove
		pf_hint_nodiscard node *remove_head() pf_attr_noexcept
		{
			// Head
			node *h = this->head_.load(std::memory_order::relaxed);
			do
			{
				if (!h)
				{
					return nullptr;
				}
			} while (!this->head_.compare_exchange_weak(
					h,
					h->next_,
					std::memory_order::relaxed,
					std::memory_order::consume));
			// Barrier
			std::atomic_thread_fence(std::memory_order::acquire);
			// Tail
			if (!h->next_)
			{
				node *t = h;
				this->tail_.compare_exchange_strong(
						t,
						nullptr,
						std::memory_order::release,
						std::memory_order::relaxed);
			}
			return h;
		}

		/// Clear
		node *clear() pf_attr_noexcept
		{
			// Head
			node *h = this->head_.load(std::memory_order::relaxed);
			do
			{
				if (!h)
				{
					return nullptr;
				}
			} while (!this->head_.compare_exchange_weak(
					h,
					nullptr,
					std::memory_order::relaxed,
					std::memory_order::consume));
			// Tail
			node *t = this->tail_.load(std::memory_order::relaxed);
			this->tail_.compare_exchange_strong(
					t,
					nullptr,
					std::memory_order::release,
					std::memory_order::relaxed);
			return h;
		}

		/// Begin / End
		pf_hint_nodiscard iterator begin() pf_attr_noexcept
		{
			return this->head_.load(std::memory_order::relaxed);
		}
		pf_hint_nodiscard const_iterator begin() const pf_attr_noexcept
		{
			return this->head_.load(std::memory_order::relaxed);
		}
		pf_hint_nodiscard const_iterator cbegin() const pf_attr_noexcept
		{
			return this->head_.load(std::memory_order::relaxed);
		}
		pf_hint_nodiscard iterator end() pf_attr_noexcept
		{
			return nullptr;
		}
		pf_hint_nodiscard const_iterator end() const pf_attr_noexcept
		{
			return nullptr;
		}
		pf_hint_nodiscard const_iterator cend() const pf_attr_noexcept
		{
			return nullptr;
		}

		/// Empty
		pf_hint_nodiscard bool empty() pf_attr_noexcept
		{
			node *h = this->head_.load(std::memory_order::relaxed);
			if (!h)
				return true;
			return false;
		}

	private:
		byte_t cacheline1_[64];
		std::atomic<node *> head_;
		byte_t cacheline2_[64];
		std::atomic<node *> tail_;
		byte_t cacheline3_[64];
	};


	/// SINGLY: CDS Linked FIFO
	template <typename _Ty>
	struct singly_cds_linked_fifo
	{
	public:
		using node					 = singly_node<_Ty>;
		using iterator			 = singly_iterator<_Ty>;
		using const_iterator = singly_const_iterator<_Ty>;

		/// Constructors
		pf_decl_explicit pf_decl_constexpr singly_cds_linked_fifo() pf_attr_noexcept
				: cacheline1_{ '\0' }
				, head_(nullptr)
				, cacheline2_{ '\0' }
		{}
		singly_cds_linked_fifo(
				singly_cds_linked_fifo const &__r) = delete;
		pf_decl_explicit singly_cds_linked_fifo(
				singly_cds_linked_fifo &&__r) pf_attr_noexcept
				: cacheline1_{ '\0' }
				, head_(__r.clear())
				, cacheline2_{ '\0' }
		{}

		/// Operator=
		singly_cds_linked_fifo &operator=(
				singly_cds_linked_fifo const &) = delete;
		singly_cds_linked_fifo &operator=(
				singly_cds_linked_fifo &&__r) pf_attr_noexcept
		{
			this->head_ = __r.clear();
			return *this;
		}

		/// Insert
		node *insert_head(
				node *__n) pf_attr_noexcept
		{
			node *h = this->head_.load(std::memory_order::relaxed);
			do
			{
				__n->next_ = h;
			} while (!this->head_.compare_exchange_weak(
					h,
					__n,
					std::memory_order::release,
					std::memory_order::consume));
			return h;
		}
		template <typename _InIterator>
		node *insert_head_bulk(
				_InIterator __beg,
				_InIterator __end)
		{
			singly_link_range(__beg, __end);
			node *b = *__beg, *e = *__end;
			node *h = this->head_.load(std::memory_order::relaxed);
			do
			{
				e->next_ = h;
			} while (!this->head_.compare_exchange_weak(
					h,
					b,
					std::memory_order::release,
					std::memory_order::consume));
			return h;
		}

		/// Remove
		pf_hint_nodiscard node *remove_head() pf_attr_noexcept
		{
			node *h = this->head_.load(std::memory_order::relaxed);
			while (!this->head_.compare_exchange_weak(
					h,
					h ? h->next_ : nullptr,
					std::memory_order::release,
					std::memory_order::consume))
				;
			return h;
		}

		/// Clear
		node *clear() pf_attr_noexcept
		{
			return this->head_.exchange(
					nullptr,
					std::memory_order::release);
		}

		/// Begin / End
		pf_hint_nodiscard iterator begin() pf_attr_noexcept
		{
			return this->head_.load(std::memory_order::relaxed);
		}
		pf_hint_nodiscard const_iterator begin() const pf_attr_noexcept
		{
			return this->head_.load(std::memory_order::relaxed);
		}
		pf_hint_nodiscard const_iterator cbegin() const pf_attr_noexcept
		{
			return this->head_.load(std::memory_order::relaxed);
		}
		pf_hint_nodiscard iterator end() pf_attr_noexcept
		{
			return nullptr;
		}
		pf_hint_nodiscard const_iterator end() const pf_attr_noexcept
		{
			return nullptr;
		}
		pf_hint_nodiscard const_iterator cend() const pf_attr_noexcept
		{
			return nullptr;
		}

		/// Empty
		pf_hint_nodiscard bool empty() const pf_attr_noexcept
		{
			return !this->head_.load(std::memory_order::relaxed);
		}

	private:
		byte_t cacheline1_[64];
		std::atomic<node *> head_;
		byte_t cacheline2_[64];
	};
}

#endif // !PULSAR_SINGLY_CDS_LINKED_LIST_HPP