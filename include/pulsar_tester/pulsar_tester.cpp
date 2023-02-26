/*! @file   pulsar_tester.cpp
 *  @author Louis-Quentin Noé (noe.louis-quentin@hotmail.fr)
 *  @brief
 *  @date   25-02-2023
 *
 *  @copyright Copyright (c) 2023 - Pulsar Software
 *
 *  @since 0.1.3
 */

// Include: Pulsar -> Tester
#include "pulsar_tester/pulsar_tester.hpp"

// Pulsar Tester
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
		if (this->name_.size() != 0)
		{
			pf_print(
				"Pack <{}>\n",
				fmt::styled(this->name_.data(),
										fmt::fg(fmt::color::steel_blue)));
		}
		// 2. Run Units
		__tester_unit *u = this->unitHead_;
		if (!u && this->name_.size() != 0)
		{
			pf_print("No unit to run!\n");
		}
		else
		{
			while (u)
			{
				try
				{
					u->__run();
					u = u->next_;
				}
				catch (__tester_exception_require const &__r)	// Require stops the program!
				{
					this->__add_result(false, __r.file(), __r.line());
					u = u->next_;
				}
			}
		}
		// 3. Benchmarks
		if (this->benchHead_)
		{
			pf_print("\nLaunching benchmark(s)...\n");
			high_resolution_point_t start = high_resolution_clock_t::now();
			__tester_benchmark *p					= this->benchHead_;

			// A. Format
			pf_print(
				"{: <32} {: <16} {: <12} {: <12} {: <12} {: <12} {: <12} {: <12} {: <12} {: <12} {: <16}\n",
				"benchmark", "num", "min", "max", "avg", "var", "dev", "Q1", "Q2", "Q3", "total");

			// B. Print Results
			p = this->benchHead_;
			while (p)
			{
				p->process(*p);
				p = p->next_;
			}

			// C. End
			nanoseconds_t dur = high_resolution_clock_t::now() - start;
			pf_print("\nFinished after {}.\n", dur);
		}
		// 4. Results
		if (this->numFailed_ > 0)
		{
			pf_print("{} | {}\n\n",
							 fmt::styled(dbg_format_message("({}) Succeeded", this->numTests_ - this->numFailed_).data(),
													 fmt::fg(fmt::color::green)),
							 fmt::styled(dbg_format_message("({}) Failed", this->numFailed_).data(),
													 fmt::fg(fmt::color::red)));
		}
		else
		{
			if (this->name_.size() != 0)
			{
				if (this->numTests_ > 0)
				{
					pf_print(
						"All ({}) succeeded!\n\n",
						fmt::styled(this->numTests_, fmt::fg(fmt::color::green)));
				}
				else
				{
					pf_print("No test in pack.\n\n");
				}
			}
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
		else
		{
			this->unitTail_->next_ = __u;
			this->unitTail_				 = __u;
		}
	}
	void
	__tester_pack::__add_benchmark(
		__tester_benchmark *__b) pf_attr_noexcept
	{
		if (!this->benchHead_)
		{
			this->benchHead_ = __b;
			this->benchTail_ = __b;
		}
		else
		{
			this->benchTail_->next_ = __b;
			this->benchTail_				= __b;
		}
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
				"/{}/ <{}> at {}:{}\n",
				fmt::styled('A', fmt::fg(fmt::color::red)),
				fmt::styled(this->name_.data(), fmt::fg(fmt::color::steel_blue)),
				fmt::styled(__file.data(), fmt::fg(fmt::color::orange)),
				fmt::styled(__line, fmt::fg(fmt::color::red)));
		}
	}

	/// TESTER: Benchmark
	// Constructors
	__tester_benchmark::__tester_benchmark(
		dbg_u8string_view __name,
		size_t __itc,
		size_t __ntt) pf_attr_noexcept
		: next_(nullptr)
		, name_(__name)
		, itc_(__itc)
		, ntt_(__ntt)
	{
		if (this->ntt_ == 0) this->ntt_ = 1;
		else if (this->ntt_ >= 128) this->ntt_ = 128;
		__tester_pack *p = tester_engine.__cur_pack();
		p->__add_benchmark(this);
	}

	// Display
	void
	__tester_benchmark::__display_results(
		__results_t const &__r) pf_attr_noexcept
	{
		pf_print(
			"{} {: <16} {: <12} {: <12} {: <12} {: <12} {: <12} {: <12} {: <12} {: <12} {: <16}\n",
			dbg_format_message("{: <32}", fmt::styled(this->name().data(), fmt::fg(fmt::color::steel_blue))).data(),
			this->num_iterations(), __r.min, __r.max, __r.avg, __r.var, __r.ect, __r.q1, __r.q2, __r.q3, __r.total);
	}
	/// TESTER: Engine
	// Constructors
	__tester_engine::__tester_engine() pf_attr_noexcept
		: unscoppedPack_(nullptr)
		, packHead_(nullptr)
		, packTail_(nullptr)
	{
		this->__add_pack(&this->unscoppedPack_);
	}

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
			p		= p->next_;
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
													 fmt::fg(fmt::color::green)),
							 fmt::styled(dbg_format_message("({}) Failed", nf).data(),
													 fmt::fg(fmt::color::red)));
		}
		else
		{
			pf_print("{}",
							 fmt::styled(dbg_format_message("\nAll ({}) test(s) passed!\n\n", nt).data(),
													 fmt::fg(fmt::color::green)));
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
		else
		{
			this->packTail_->next_ = __p;
			this->packTail_				 = __p;
		}
	}
	__tester_pack*
	__tester_engine::__cur_pack() pf_attr_noexcept
	{
		return this->packTail_;
	}
}