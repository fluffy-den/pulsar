/*! @file   internal.cpp
 *  @author Louis-Quentin Noé (noe.louis-quentin@hotmail.fr)
 *  @brief
 *  @date   26-03-2023
 *
 *  @copyright Copyright (c) 2023 - Pulsar Software
 *
 *  @since 0.1.5
 */

// Include: Pulsar
#include "pulsar/internal.hpp"

// Pulsar
namespace pul
{
	/// INTERNAL:
	// Constructor
	__internal_t::__internal_t()
		: cmem(ALLOCATOR_CACHE_SIZE_0, ALLOCATOR_CACHE_SIZE)
		, smem(ALLOCATOR_STACK_SIZE_0, ALLOCATOR_STACK_SIZE, magnifier_linear(ALLOCATOR_STACK_SIZE))
	{
		/// We won't use C print at all!
		std::ios_base::sync_with_stdio(false);

		/// Debug
		dbg_u8string_view logo =
		 R"(
        ██████╗ ██╗   ██╗██╗     ███████╗ █████╗ ██████╗ 
        ██╔══██╗██║   ██║██║     ██╔════╝██╔══██╗██╔══██╗
        ██████╔╝██║   ██║██║     ███████╗███████║██████╔╝
        ██╔═══╝ ██║   ██║██║     ╚════██║██╔══██║██╔══██╗
        ██║     ╚██████╔╝███████╗███████║██║  ██║██║  ██║
        ╚═╝      ╚═════╝ ╚══════╝╚══════╝╚═╝  ╚═╝╚═╝  ╚═╝  )";
		dbg_u8print("{}", logo.begin());
		char_t buf[64] = { '\0' };
		dbg_u8format_to(
		 &buf[0], "v{0}.{1}.{2}\n\n", PULSAR_VERSION_MAJOR, PULSAR_VERSION_MINOR, PULSAR_VERSION_PATCH);
		dbg_u8print("{}", &buf[0]);
	}

	// Instance
	__internal_t __internal;
}	 // namespace pul
