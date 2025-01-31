/*! @file   debug.hpp
 *  @author Louis-Quentin Noé (noe.louis-quentin@hotmail.fr)
 *  @brief
 *  @date   26-03-2023
 *
 *  @copyright Copyright (c) 2023 - Pulsar Software
 *
 *  @since 0.1.4
 */

#ifndef PULSAR_SRC_DEBUG_HPP
#define PULSAR_SRC_DEBUG_HPP 1

// Include: Pulsar
#include "pulsar/allocator.hpp"
#include "pulsar/malloc.hpp"
#include "pulsar/memory.hpp"
#include "pulsar/chrono.hpp"

// Include: Pulsar -> Src
#include "pulsar/debug/debug_win.hpp"

// Pulsar
namespace pul
{
	/// DEBUG: Constants
	pf_decl_constexpr uint32_t ALLOCATOR_CACHE_SIZE_0 = 2'097'152;
	pf_decl_constexpr uint32_t ALLOCATOR_CACHE_SIZE		= 524'288;
	pf_decl_constexpr uint32_t ALLOCATOR_STACK_SIZE_0 = 4 * 65'536;
	pf_decl_constexpr uint32_t ALLOCATOR_STACK_SIZE		= 65'536;

	// NOTE: size_0 = 2359296
	// 			 size_+ = 589814
	// 			 For 16 = 11796320 ~ 12 Mo of cache

	/// DEBUG: Type
	class __dbg_logger_t
	{
	public:
		/// Constructors
		__dbg_logger_t()
			: start_(high_resolution_clock_t::now())
			, filter_(dbg_level::low)
			, callback_(nullptr)
		{}
		__dbg_logger_t(__dbg_logger_t const &) = delete;
		__dbg_logger_t(__dbg_logger_t &&)			 = delete;

		/// Destructor
		~__dbg_logger_t() pf_attr_noexcept
		{}

		/// Operator =
		__dbg_logger_t &
		operator=(__dbg_logger_t const &) = delete;
		__dbg_logger_t &
		operator=(__dbg_logger_t &&) = delete;

		/// Get*
		pf_hint_nodiscard pf_decl_inline nanoseconds_t
		__get_elapsed_time() const pf_attr_noexcept
		{
			return high_resolution_clock_t::now() - this->start_;
		}
		pf_hint_nodiscard pf_decl_inline dbg_level
		__get_filter() const pf_attr_noexcept
		{
			return this->filter_;
		}
		pf_hint_nodiscard pf_decl_inline dbg_logger_callback_t
		__get_callback() const pf_attr_noexcept
		{
			return this->callback_;
		}

		/// Set*
		pf_decl_inline void
		__set_filter(
		 dbg_level __level) pf_attr_noexcept
		{
			this->filter_ = __level;
		}
		pf_decl_inline void
		__set_callback(
		 dbg_logger_callback_t __callback) pf_attr_noexcept
		{
			this->callback_ = __callback;
		}

		/// Write
		pf_decl_static pf_decl_inline void
		__write_task(
		 dbg_u8string_view __str) pf_attr_noexcept
		{
			dbg_u8print("{}", __str.begin());
		}
		pf_decl_inline void
		__write(
		 dbg_u8string &&__str) pf_attr_noexcept
		{
			this->pool_.submit_task(this->__write_task, std::move(__str));
		}
		pf_decl_inline void
		__write(
		 dbg_level __level,
		 dbg_u8string &&__str) pf_attr_noexcept
		{
			if(__level >= this->filter_) this->__write(std::move(__str));
		}

	private:
		/// Store
		high_resolution_point_t start_;
		dbg_level filter_;
		dbg_logger_callback_t callback_;
		task_pool_t pool_;
	};
}	 // namespace pul
#endif	// !PULSAR_DEBUG_HPP
