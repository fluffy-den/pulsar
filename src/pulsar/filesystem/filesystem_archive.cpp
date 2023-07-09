/*! @file   filesystem_archive.cpp
 *  @author Louis-Quentin Noé (noe.louis-quentin@hotmail.fr)
 *  @brief
 *  @date   28-06-2023
 *
 *  @copyright Copyright (c) 2023 - Pulsar Software
 *
 *  @since 0.1.6
 */

// Include: Pulsar
#include "pulsar/internal.hpp"

// Include: Lz4
#include "lz4.h"
#include "lz4hc.h"

// Pulsar
namespace pul
{
	// Filesystem
	namespace fs
	{
		// Builder
		struct __archive_builder_file_t
		{
			/// Constructors

			/// Operator =

			/// Destructor

			/// Data
			uint8_t len;
			u8char_t name[255];
		};
		struct __archive_builder_directory_t
		{
			using allocator_t = allocator_shared_ptr<allocator_linear<magnifier_linear>, deleter_halloc<shared_store<allocator_linear<magnifier_linear>>>>;
			using list_t			= singly_list<__archive_builder_file_t, magnifier_linear, allocator_t>;

			/// Constructors

			/// Operator =

			/// Destructor

			/// Data
			list_t files;
		};
		struct __archive_builder_t
		{
			using allocator_t = typename __archive_builder_directory_t::allocator_t;
			using list_t			= singly_list<__archive_builder_directory_t, magnifier_linear, allocator_t>;

			/// Constructors
			pf_decl_inline pf_decl_constexpr
			__archive_builder_t() pf_attr_noexcept
			// : all(make_unique<allocator_linear<magnifier_linear>>(65'536, align_val_t(8), magnifier_linear(65'536)))
			// , root()
			{
				// TODO
			}

			/// Operator =

			/// Destructor

			/// Find

			/// Insert

			/// Data
			// allocator_t all;
			// list_t root;
		};

		// Maker
		pulsar_api void
		archive_make(
		 path_view __dest,
		 perm_t __perms,
		 archive_compression_algorithm_t __alg,
		 initializer_list<__archive_entry_t> __entries)
		{
			// Create
			file_create(__dest, __perms);

			// Open
			file f(__dest, io_access::write, io_mode::open);
			size_t numDirs	= 0;
			size_t numFiles = 0;

			// Builder
			// __archive_builder_t builder;

			// Calculate total needed size for archive
			for(auto &e : __entries)
			{
				auto attrs = attributes(e.from);

				// File?
				if(attrs & attribute::directory)
				{
				}
				else
				{
					++numFiles;
				}
			}
		}

		// State

		// Compare

		// Updator

	}	 // namespace fs

}	 // namespace pul
