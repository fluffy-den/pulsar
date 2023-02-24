/*! @file   fs_path.hpp
 *  @author Louis-Quentin Noé (noe.louis-quentin@hotmail.fr)
 *  @brief
 *  @date   02-01-2023
 *
 *  @copyright Copyright (c) 2023 - Pulsar Software
 *
 *  @since 0.1.3
 */

#ifndef PULSAR_FS_PATH_HPP
#define PULSAR_FS_PATH_HPP 1

// Include: Pulsar
#include "pulsar/pulsar.hpp"

// Pulsar
namespace pul
{
	/// FILESYSTEM: Path
	class fspath
	{
	public:

		/// Operator/=
		pf_decl_constexpr fspath &
		operator /=(
			u8string_span_t __r) pf_attr_noexcept;

		/// Has Extension
		pf_decl_constexpr bool
		has_extension() const pf_attr_noexcept;

	private:
	};

	/// FILESYSTEM: Path -> Operator/
	fspath
	operator /(
		fspath const &__l,
		u8string_span_t __r) pf_attr_noexcept;

	/// FILESYSTEM: Path -> Function
	pulsar_api fspath
	current_path() pf_attr_noexcept;
	pulsar_api bool
	exists(
		fspath const &__p) pf_attr_noexcept;
	pulsar_api bool
	create_directories(
		fspath const &__p) pf_attr_noexcept;
}

#endif // !PULSAR_FS_PATH_HPP