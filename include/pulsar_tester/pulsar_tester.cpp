/*! @file   pulsar_tester.cpp
 *  @author Louis-Quentin Noé (noe.louis-quentin@hotmail.fr)
 *  @brief
 *  @date   29-01-2023
 *
 *  @copyright Copyright (c) 2023 - Pulsar Software
 *
 *  @since 0.1.2
 */

// Include: Pulsar
#include "pulsar_tester/pulsar_tester.hpp"

// Pulsar
namespace pul
{
	/// TESTER: Engine -> Variables
	__tester_engine tester_engine;

	/// TESTER: Unit
	// Constructors
	__tester_unit::__tester_unit(
		dbg_u8string_view __name) pf_attr_noexcept
		: name_(__name)
		, next_(nullptr)
	{
		__tester_pack *p = tester_engine.__cur_pack();
		p->__add_unit(this);
	}

	/// TESTER: Exception
	// Constructors
	__tester_exception_require::__tester_exception_require(
		dbg_u8string_view __file,
		uint32_t __line) pf_attr_noexcept
		: file_(__file)
		, line_(__line)
	{}

	/// TESTER: Unit -> Pack
	// Constructors
	__tester_pack::__tester_pack(
		dbg_u8string_view __name) pf_attr_noexcept
		: name_(__name)
		, next_(nullptr)
		, unitCurr_(nullptr)
		, unitHead_(nullptr)
		, unitTail_(nullptr)
		, numTests_(0)
		, numFailed_(0)
	{
		tester_engine.__add_pack(this);
	}

	// Run
	void
	__tester_pack::__run() pf_attr_noexcept
	{
		// 1. Print Pack
		if (this->name_.size() != 1)
		{
			pf_print(
				"Pack <{}>\n",
				fmt::styled(this->name_.data(),
										fmt::fg(fmt::color::steel_blue) | fmt::bg(fmt::color::black)));
		}
		// 2. Run Units
		__tester_unit *u = this->unitHead_;
		while (u)
		{
			try
			{
				this->unitCurr_ = u;
				u->__run();
				u = u->next_;
			}
			catch (__tester_exception_require const &__r)	// Require stops the program!
			{
				this->__add_result(false, __r.file(), __r.line());
			}
		}
		// 3. Results
		if (this->numFailed_ > 0)
		{
			pf_print("{} | {}\n\n",
							 fmt::styled(dbg_format_message("({}) Succeeded", this->numTests_ - this->numFailed_).data(),
													 fmt::fg(fmt::color::green) | fmt::bg(fmt::color::black)),
							 fmt::styled(dbg_format_message("({}) Failed", this->numFailed_).data(),
													 fmt::fg(fmt::color::red) | fmt::bg(fmt::color::black)));
		}
		else
		{
			pf_print(
				"All ({}) succeeded!\n\n",
				fmt::styled(this->numTests_, fmt::fg(fmt::color::green) | fmt::bg(fmt::color::black)));
		}
	}

	// Unit
	void
	__tester_pack::__add_unit(
		__tester_unit *__u) pf_attr_noexcept
	{
		if (!this->unitHead_)
		{
			this->unitHead_ = __u;
			this->unitTail_ = __u;
		}
		this->unitTail_->next_ = __u;
		this->unitTail_				 = __u;
	}
	void
	__tester_pack::__add_result(
		bool __c,
		dbg_u8string_view __file,
		uint32_t __line) pf_attr_noexcept
	{
		++this->numTests_;
		if (!__c)
		{
			++this->numFailed_;
			pf_print(
				"<{}> at {}:{}\n",
				fmt::styled(this->name_, fmt::fg(fmt::color::steel_blue) | fmt::bg(fmt::color::black)),
				fmt::styled(__file.data(), fmt::fg(fmt::color::orange) | fmt::bg(fmt::color::black)),
				fmt::styled(__line, fmt::fg(fmt::color::red) | fmt::bg(fmt::color::black)));
		}
	}

	/// TESTER: Engine
	// Constructors
	__tester_engine::__tester_engine() pf_attr_noexcept
		: unscoppedPack_("")
		, packHead_(nullptr)
		, packTail_(nullptr)
	{}

	// Run
	pf_hint_nodiscard int32_t
	__tester_engine::run() pf_attr_noexcept
	{
		// 1. Info
		pf_print(dbg_type::info, dbg_level::high, "Launching Pulsar Tester...");

		// 2. Running Packs
		__tester_pack *p = this->packHead_;
		size_t nt				 = 0;
		size_t nf				 = 0;
		while (p)
		{
			p->__run();
			nt += p->numTests_;
			nf += p->numFailed_;
		}

		// 3. Print
		if (nt == 0)
		{
			pf_print("Nothing to test.\n");
		}
		else if (nf > 0)
		{
			pf_print("\n{} | {}\n\n",
							 fmt::styled(dbg_format_message("({}) Succeeded", nt - nf).data(),
													 fmt::fg(fmt::color::green) | fmt::bg(fmt::color::black)),
							 fmt::styled(dbg_format_message("({}) Failed", nf).data(),
													 fmt::fg(fmt::color::red) | fmt::bg(fmt::color::black)));
		}
		else
		{
			pf_print("{}",
							 fmt::styled(dbg_format_message("\nAll ({}) test(s) passed!\n\n", nt).data(),
													 fmt::fg(fmt::color::green) | fmt::bg(fmt::color::black)));
		}

		// Fail? Succeeded?
		return nf > 0 ? -1 : 0;
	}

	// Test
	void __tester_engine::__test(
		bool __c,
		dbg_u8string_view __file,
		uint32_t __line) pf_attr_noexcept
	{
		__tester_pack *p = this->__cur_pack();
		p->__add_result(__c, __file, __line);
	}

	// Pack
	void
	__tester_engine::__add_pack(
		__tester_pack *__p) pf_attr_noexcept
	{
		if(!this->packHead_)
		{
			this->packHead_ = __p;
			this->packTail_ = __p;
		}
		this->packTail_->next_ = __p;
		this->packTail_				 = __p;
	}
	__tester_pack*
	__tester_engine::__cur_pack() pf_attr_noexcept
	{
		return this->packTail_;	// TODO: Logic to get current pack
	}

	/// TESTER: Functions

}

// Pulsar -> Main
pul::int32_t
main()
{
	return pul::tester_engine.run();
}