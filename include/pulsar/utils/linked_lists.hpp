/**
 * @file    linked_lists.hpp
 * @author  Fluffy (noe.louis-quentin@hotmail.fr)
 * @brief
 * @date    14-12-2021
 *
 * @copyright Copyright (c) 2021 - Pulsar Software
 *
 */

#ifndef PULSAR_LINKED_LISTS_HPP
#define PULSAR_LINKED_LISTS_HPP 1

// Include: Pulsar
#include "pulsar/memory/memory_locks.hpp"

// Include: C++
#include <compare>
#include <initializer_list>
#include <type_traits>

// namespace pulsar
namespace pulsar
{

	// Singly Link
	class singlylink
	{
		pf_decl_friend class singlylist;

	public:
		using thread_safe = std::false_type;

		// Constructors
		pf_decl_constexpr pf_decl_explicit singlylink() pf_attr_noexcept : next_(nullptr)
		{}
		pf_decl_constexpr pf_decl_explicit singlylink(singlylink const &r)
		  pf_attr_noexcept															   = delete;
		pf_decl_constexpr pf_decl_explicit singlylink(singlylink &&r) pf_attr_noexcept = delete;

		// Next
		pf_decl_constexpr singlylink *next() pf_attr_noexcept
		{
			return this->next_;
		}

		// Operator=
		pf_decl_constexpr singlylink &operator=(singlylink const &r) pf_attr_noexcept = delete;
		pf_decl_constexpr singlylink &operator=(singlylink &&r) pf_attr_noexcept = delete;

	private:
		singlylink *next_;
	};

	// Singly Link Next of function
	pf_decl_constexpr singlylink *next_of(singlylink *x)
	{
		// TODO: Exception !x

		return x->next();
	}


	// Singly Linked-list
	class singlylist
	{
	public:
		using thread_safe = std::false_type;

		// Constructors
		pf_decl_constexpr pf_decl_explicit singlylist() pf_attr_noexcept
			: front_(nullptr)
			, back_(nullptr)
		{}
		pf_decl_constexpr singlylist(std::initializer_list<singlylink *> list) pf_attr_noexcept
			: singlylist()
		{
			for (auto &it : list) { this->insert_front(it); }
		}
		pf_decl_constexpr pf_decl_explicit singlylist(singlylist const &y)
		  pf_attr_noexcept = delete;
		pf_decl_constexpr pf_decl_explicit singlylist(singlylist &&y) pf_attr_noexcept
			: front_(y.front_)
			, back_(y.back_)
		{
			y.front_ = nullptr;
			y.back_	 = nullptr;
			if (this->front_) this->front_->next_ = nullptr;
		}

		// Destructor
		pf_decl_constexpr ~singlylist() pf_attr_noexcept
		{
			this->clear();
		}

		// Operator=
		pf_decl_constexpr singlylist &operator=(singlylist const &y) pf_attr_noexcept = delete;
		pf_decl_constexpr singlylist &operator=(singlylist &&y) pf_attr_noexcept
		{
			this->front_ = y.front_;
			this->back_	 = y.back_;
			if (this->front_) this->front_->next_ = nullptr;
			return *this;
		}

		// Insert
		pf_decl_constexpr singlylink *insert_front(singlylink *e) pf_attr_noexcept
		{
			// TODO: Exception !e

			if (!this->front_)
			{
				this->front_ = e;
				this->back_	 = e;
			}
			else
			{
				this->front_->next_ = e;
				this->front_		= e;
			}

			e->next_ = nullptr;

			return this->front_;
		}
		pf_decl_constexpr singlylink *insert_back(singlylink *e) pf_attr_noexcept
		{
			// TODO: Exception !e

			if (!this->back_)
			{
				this->back_	 = e;
				this->front_ = e;
			}
			else
			{
				e->next_	= this->back_;
				this->back_ = e;
			}

			e->next_ = nullptr;

			return this->back_;
		}
		pf_decl_constexpr singlylink *insert_after(singlylink *p, singlylink *e) pf_attr_noexcept
		{
			// TODO: Exception !e
			// TODO: Exception !this->back_ && p

			if (!p) { return this->insert_back(e); }

			e->next_ = p->next_;
			p->next_ = e;

			return e;
		}

		// Remove
		pf_decl_constexpr singlylink *remove_front() pf_attr_noexcept
		{
			if (!this->back_) return nullptr;
			singlylink *p = this->back_;
			while (p->next_ != this->front_) { p = p->next_; }
			singlylink *f = this->front_;
			if (p) { p->next_ = nullptr; }
			this->front_ = p;
			return f;
		}
		pf_decl_constexpr singlylink *remove_back() pf_attr_noexcept
		{
			if (!this->back_) return nullptr;
			singlylink *b = this->back_;
			this->back_	  = this->back_->next_;
			return b;
		}
		pf_decl_constexpr singlylink *remove_after(singlylink *p) pf_attr_noexcept
		{
			if (!p) return this->remove_back();

#ifdef pf_debug

				// TODO: Exception !lookup(p) -> p doesn't exist in this list!

#endif // pf_debug

			if (p->next_)
			{
				singlylink *e = p->next_;
				if (e == this->front_) return this->remove_front();
				p->next_ = e->next_;
				return e;
			}

			return nullptr;
		}

		// Look-up
		pf_hint_nodiscard pf_decl_constexpr singlylink *lookup(singlylink *e) const pf_attr_noexcept
		{
			// TODO: Exception !e

			singlylink *p = this->back_;
			while (p)
			{
				if (p == e) return p;
				p = p->next_;
			}

			return nullptr;
		}

		// Size
		pf_hint_nodiscard pf_decl_constexpr size_t size() const pf_attr_noexcept
		{
			size_t i	  = 0;
			singlylink *p = this->back_;
			while (p)
			{
				i++;
				p = p->next_;
			}

			return i;
		}

		// Clear
		pf_decl_constexpr void clear() pf_attr_noexcept
		{
			if (!this->back_) return;

			singlylink *p = this->back_;
			while (p->next_)
			{
				singlylink *e = p->next_;
				this->remove_after(p);
				p = e;
			}

			this->front_ = nullptr;
			this->back_	 = nullptr;
		}

	private:

		singlylink *front_;
		singlylink *back_;
	};

	// Atomic Singly Link
	class atomic_singlylink
	{
		pf_decl_friend class atomic_singlylist;

	public:
		using thread_safe = std::true_type;

		// Constructors
		pf_decl_explicit atomic_singlylink() pf_attr_noexcept : next_(nullptr)
		{}
		pf_decl_explicit atomic_singlylink(atomic_singlylink const &r) pf_attr_noexcept = delete;
		pf_decl_explicit atomic_singlylink(atomic_singlylink &&r) pf_attr_noexcept		= delete;

		// Next
		atomic_singlylink *next() pf_attr_noexcept
		{
			return this->next_.load(memory_order::acquire);
		}

		// Operator=
		atomic_singlylink &operator=(atomic_singlylink const &r) pf_attr_noexcept = delete;
		atomic_singlylink &operator=(atomic_singlylink &&r) pf_attr_noexcept = delete;

	private:
		atomic<atomic_singlylink *> next_;
	};

	// Atomic Singly List
	class atomic_singlylist
	{
	public:
		using thread_safe = std::true_type;

		// Constructors
		pf_decl_constexpr pf_decl_explicit atomic_singlylist() pf_attr_noexcept
			: front_(nullptr)
			, back_(nullptr)
		{}

	private:
		atomic<atomic_singlylink *> front_;
		atomic<atomic_singlylink *> back_;
	};

}; // namespace pulsar

#endif // !PULSAR_LINKED_LISTS_HPP