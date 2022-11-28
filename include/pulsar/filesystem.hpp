/*! @file   filesystem.hpp
 *  @author Louis-Quentin NOE (noe.louis-quentin@hotmail.fr)
 *  @brief
 *  @date   17-09-2022
 *
 *  @copyright Copyright (c) 2022 - Pulsar Software
 *
 *  @since 0.1.2
 */

#ifndef PULSAR_FILESYSTEM_HPP
#define PULSAR_FILESYSTEM_HPP 1

// Include: Pulsar
#include "pulsar/pulsar.hpp"

// Pulsar
namespace pul
{

	/// FILESYSTEM: Handle
	class fs_handle
	{};

	/// FILESYSTEM: Path
	class fs_path
	{
	public:

		/// Operator/=
		pf_decl_constexpr fs_path &
		operator /=(
			c8string_span_t __r) pf_attr_noexcept;

		/// Has Extension
		pf_decl_constexpr bool
		has_extension() const pf_attr_noexcept;

	private:
	};

	/// FILESYSTEM: Path -> Operator/
	fs_path
	operator /(
		fs_path const &__l,
		c8string_span_t __r) pf_attr_noexcept;

	/// FILESYSTEM: Path -> Function
	pulsar_api fs_path
	current_path() pf_attr_noexcept;
	pulsar_api bool
	exists(
		fs_path const &__p) pf_attr_noexcept;
	pulsar_api bool
	create_directories(
		fs_path const &__p) pf_attr_noexcept;

	/// FILESYSTEM: Package
	class fs_package
	{};

	/// FILESYSTEM: Signal
	class fs_signal
	{};

	/// FILESYSTEM: Iterator
	class fs_iterator
	{};
}
// TODO: fs_handle: // https://docs.microsoft.com/en-us/windows/win32/fileio/i-o-concepts
// TODO: fs_package
// TODO: fs_path
// TODO: fs_signal: // https://www.codeproject.com/Articles/4692/How-to-get-a-notification-if-change-occurs-in-a-sp
// https://github.com/apetrone/simplefilewatcher
// TODO: fs_iterator

#endif // !PULSAR_FILESYSTEM_HPP