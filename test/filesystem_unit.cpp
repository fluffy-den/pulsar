/*! @file filesystem_unit.cpp
 *  @author Louis-Quentin Noé (noe.louis-quentin@hotmail.fr)
 *  @brief
 *  @date 28-05-2023
 *
 *  @copyright Copyright (c) 2023 - Pulsar Software
 *
 *  @since 0.1
 */

// Include: Pulsar
#include "pulsar/filesystem.hpp"

// Include: Pulsar -> Tester
#include "pulsar_tester/pulsar_tester.hpp"

// Include: C++
#include <filesystem>

// Pulsar
namespace pul
{


	// filesystem -> path
	pt_pack(filesystem_pack)
	{
		// -> String
		pt_unit(path_unit)
		{
			fs::path p1;
			p1.insert_back("C:/games/idy/modules/core.ppck");
			p1.remove_extension();
			p1.remove_filename();
			fs::path_view v1 = "C:/games/idy/modules";
			pt_check(equal(p1.begin(), p1.end(), v1.begin()));

			fs::path p2;
			p2.insert_back("C:/games/idy/modules/core.ppck");
			p2.replace_extension(".pach");
			p2.replace_stem("glory");
			fs::path_view v2 = "C:/games/idy/modules/glory.pach";
			pt_check(equal(p2.begin(), p2.end(), v2.begin()));

			fs::path p3;
			p3.insert_back("C:/games/idy/modules/core.ppck");
			p3.replace_filename("glory.pach");
			pt_check(equal(p3.begin(), p3.end(), v2.begin()));
		}

		// -> Iterate
		pt_unit(iterate_unit)
		{
			auto v = fs::make_dir_viewer("C:/Users/Fluffy/Documents/Mount&Blade Warband");
			for(auto &it : v)
			{
				pf_print("{}\n", it.path().data());
			}
		}

		// -> Iterate Rec
		pt_unit(iterate_rec_unit)
		{
			auto vpul = fs::make_rec_dir_viewer("C:/Users/Fluffy/Documents/Mount&Blade Warband");
			for(auto &it : vpul)
			{
				pf_print("{}\n", it.path().data());
			}
		}

		// -> Drive
		pt_unit(drive_unit)
		{
			auto em = fs::drive_enumerate();
			for(auto it : em)
			{
				pf_print("{}\n", it.data());
			}
			auto type = fs::drive_get_type("C:/");
			pt_check(type == fs::drive_type::fixed);
			fs::path<> str = "C:/Users/Fluffy/Appdata/Roaming/Factorio";
			pt_check(str.view() != "Blah");
			pt_check(fs::drive_by_path("C:/Users/Fluffy/Appdata/Roaming/Factorio").view() == "C:/");
		}

		// -> File
		pt_unit(file_unit)
		{
			// Sync
			u8char_t str[] = "C:/Users/Fluffy/AppData/LocalLow/Pulsar/Test.plk";
			fs::file_create(str, fs::perm::owner_all);
			fs::file f(str, fs::io_access::write | fs::io_access::read, fs::io_mode::overwrite);
			pt_check(f.is_open());
			auto path = f.path();
			pt_check(equal(begin(str), end(str), begin(path)));
			{
				fs::file_lock_unique_t lock(f, 0, 1'024);
				pt_check(lock.is_locked());
				byte_t data[] = "This is some sync writing testing.";
				f.write(&data[0u], sizeof(data));
				auto k = f.tell();
				pt_check(k == 35);
			}
			f.close();

			// Async
			fs::async_file a(str, fs::io_access::write | fs::io_access::read, fs::io_mode::overwrite);
			pt_check(a.is_open());
			path = a.path();
			pt_check(equal(begin(str), end(str), begin(path)));
			{
				byte_t data[] = "This is some async writing testing.";
				auto fut			= a.future_write(0, &data[0u], sizeof(data));
			}
			a.close();
		}
	}
}	 // namespace pul
