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
	pulsar_api __tester_engine tester_engine;

	/// TESTER: Unit
	// Constructors
	pulsar_api __tester_unit::__tester_unit(
		dbg_u8string_view __name) pf_attr_noexcept
		: name_(__name)
		, next_(nullptr)
	{
		__tester_pack *p = tester_engine.__cur_pack();
		p->__add_unit(this);
	}

	/// TESTER: Unit -> Pack
	// Constructors
	pulsar_api __tester_pack::__tester_pack(
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
	pulsar_api void
	__tester_pack::__run() pf_attr_noexcept
	{
		// 1. Print Pack
		if (this->name_.size() != 0)
		{
			pf_print(
				dbg_type::info, dbg_level::high,
				"Tester -> Pack <{}>",
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
					u = u->next_;
				}
			}
		}
		// 3. Benchmarks
		if (this->benchHead_)
		{
			pf_print("\nLaunching benchmark(s)...\n\n");
			high_resolution_point_t start = high_resolution_clock_t::now();
			__tester_benchmark *p					= this->benchHead_;

			// A. Format
			pf_print(
				"{: <32} {: <12} {: <16} {: <12} {: <12} {: <12} {: <12} {: <12} {: <12} {: <12} {: <12} {: <16}\n",
				"benchmark", "threads", "num", "min", "max", "avg", "var", "dev", "Q1", "Q2", "Q3", "total");

			// B. Print Results
			p = this->benchHead_;
			while (p)
			{
				try
				{
					p->process(*p);
					p = p->next_;
				}
				catch (__tester_exception_require const &__r)	// Require stops the program!
				{
					p = p->next_;
				}
			}

			// C. End
			nanoseconds_t dur = high_resolution_clock_t::now() - start;
			pf_print("\nFinished after {}.\n\n", dur);
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
					pf_print(dbg_type::info, dbg_level::high, "Tester -> All ({}) succeeded!\n\n",
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
	pulsar_api void
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
	pulsar_api void
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
	pulsar_api void
	__tester_pack::__add_result(
		bool __c,
		dbg_u8string_view __file,
		uint32_t __line) pf_attr_noexcept
	{
		++this->numTests_;
		if (!__c)
		{
			++this->numFailed_;
			if (this->name_.data() == nullptr)
			{
				pf_print(
					"/{}/ at {}:{}\n",
					fmt::styled('A', fmt::fg(fmt::color::red)),
					fmt::styled(__file.data(), fmt::fg(fmt::color::orange)),
					fmt::styled(__line, fmt::fg(fmt::color::red)));
			}
			else
			{
				pf_print(
					"/{}/ <{}> at {}:{}\n",
					fmt::styled('A', fmt::fg(fmt::color::red)),
					fmt::styled(this->name_.data(), fmt::fg(fmt::color::steel_blue)),
					fmt::styled(__file.data(), fmt::fg(fmt::color::orange)),
					fmt::styled(__line, fmt::fg(fmt::color::red)));
			}
		}
	}

	/// TESTER: Benchmark
	// Constructors
	pulsar_api __tester_benchmark::__tester_benchmark(
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
	pulsar_api void
	__tester_benchmark::__display_measures(
		const nanoseconds_t *__rts,
		const size_t __c) pf_attr_noexcept
	{
		// 1. Convert
		nanoseconds_t min = nanoseconds_t(union_cast<size_t>(-1));
		nanoseconds_t max = nanoseconds_t(0);
		nanoseconds_t avg = nanoseconds_t(0);
		for (size_t i = 0; i < __c; ++i)
		{
			if (__rts[i] < min) min = __rts[i];
			if (__rts[i] > max) max = __rts[i];
			avg += __rts[i];
		}
		nanoseconds_t total = avg;
		avg = nanoseconds_t(avg.count() / __c);
		nanoseconds_t var = nanoseconds_t(0);
		for (size_t i = 0; i < __c; ++i)
		{
			const nanoseconds_t k = (__rts[i] - avg);
			var += nanoseconds_t(k.count() * k.count());
		}
		var = nanoseconds_t(var.count() / __c);
		nanoseconds_t ect = nanoseconds_t(static_cast<int64_t>(std::sqrt(static_cast<float64_t>(var.count()))));
		nanoseconds_t q1	= __rts[__c / 4];
		nanoseconds_t q2	= __rts[__c / 2];
		nanoseconds_t q3	= __rts[__c * 3 / 4];

		// 2. Print
		pf_print(
			"{} {: <12} {: <16} {: <12} {: <12} {: <12} {: <12} {: <12} {: <12} {: <12} {: <12} {: <16}\n",
			dbg_format_message("{: <32}", fmt::styled(this->name().data(), fmt::fg(fmt::color::steel_blue))).data(),
			this->num_threads(), this->num_iterations(), min, max, avg, var, ect, q1, q2, q3, total);
	}
	/// TESTER: Engine
	// Constructors
	pulsar_api __tester_engine::__tester_engine() pf_attr_noexcept
		: unscoppedPack_(nullptr)
		, packHead_(nullptr)
		, packTail_(nullptr)
	{
		this->__add_pack(&this->unscoppedPack_);
	}

	// Run
	pf_hint_nodiscard pulsar_api int32_t
	__tester_engine::run() pf_attr_noexcept
	{
		// 1. Info
		pf_print(dbg_type::info, dbg_level::high, "Launching Tester...");

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
			pf_print(dbg_type::info, dbg_level::high, "Tester -> Nothing to test.\n");
		}
		else if (nf > 0)
		{
			pf_print(dbg_type::info, dbg_level::high, "Tester -> {} | {}\n",
							 fmt::styled(dbg_format_message("({}) Succeeded", nt - nf).data(),
													 fmt::fg(fmt::color::green)),
							 fmt::styled(dbg_format_message("({}) Failed", nf).data(),
													 fmt::fg(fmt::color::red)));
		}
		else
		{
			pf_print(dbg_type::info, dbg_level::high, "Tester -> {}",
							 fmt::styled(dbg_format_message("All ({}) test(s) passed!\n", nt).data(),
													 fmt::fg(fmt::color::green)));
		}

		// Fail? Succeeded?
		return nf > 0 ? -1 : 0;
	}

	// Test
	pulsar_api void __tester_engine::__test(
		bool __c,
		dbg_u8string_view __file,
		uint32_t __line) pf_attr_noexcept
	{
		__tester_pack *p = this->__cur_pack();
		p->__add_result(__c, __file, __line);
	}

	// Pack
	pulsar_api void
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
	pulsar_api __tester_pack*
	__tester_engine::__cur_pack() pf_attr_noexcept
	{
		return this->packTail_;
	}
}