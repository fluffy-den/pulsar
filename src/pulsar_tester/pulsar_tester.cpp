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
				dbg_styled(this->name_.data(),
									 dbg_style_fg(dbg_color::steel_blue)));
		}
		// 2. Run Units
		__tester_unit *u = this->unitHead_;
		if (!u && this->name_.size() != 0)
		{
			pf_print(
				"/{}/ No unit to run!\n",
				dbg_styled('T', dbg_style_fg(dbg_color::green)));
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
			// Initialisation
			high_resolution_point_t start = high_resolution_clock_t::now();
			__tester_benchmark *p					= this->benchHead_;

			// Format
			pf_print(
				"/{}/ Launching benchmark(s)\n"
				"{: <32} {: <12} {: <16} {: <12} {: <12} {: <12} {: <12} {: <12} {: <12} {: <12} {: <12} {: <16}\n",
				dbg_styled('B', dbg_style_fg(dbg_color::golden_rod)),
				"benchmark", "threads", "num", "min", "max", "avg", "var", "dev", "Q1", "Q2", "Q3", "total");

			// Print Results
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

			// Print time
			nanoseconds_t dur = high_resolution_clock_t::now() - start;
			pf_print(
				"/{}/ finished after {}!\n",
				dbg_styled('B', dbg_style_fg(dbg_color::golden_rod)),
				dur);
		}
		// 4. Results
		if (this->numFailed_ > 0)
		{
			pf_print(
				"/{}/ {} | {}\n",
				dbg_styled('T', dbg_style_fg(dbg_color::green_yellow)),
				dbg_styled(dbg_u8format("({}) Succeeded", this->numTests_ - this->numFailed_).data(),
									 dbg_style_fg(dbg_color::green)),
				dbg_styled(dbg_u8format("({}) Failed", this->numFailed_).data(),
									 dbg_style_fg(dbg_color::red)));
		}
		else
		{
			if (this->name_.size() != 0)
			{
				if (this->numTests_ > 0)
				{
					pf_print(
						"/{}/ ({}) Succeeded\n",
						dbg_styled('T', dbg_style_fg(dbg_color::green_yellow)),
						dbg_styled(this->numTests_, dbg_style_fg(dbg_color::green)));
				}
				else
				{
					pf_print(
						"/{}/ No test!\n",
						dbg_styled('T', dbg_style_fg(dbg_color::green_yellow)));
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
					dbg_styled('A', dbg_style_fg(dbg_color::red)),
					dbg_styled(__file.data(), dbg_style_fg(dbg_color::orange)),
					dbg_styled(__line, dbg_style_fg(dbg_color::red)));
			}
			else
			{
				pf_print(
					"/{}/ <{}> at {}:{}\n",
					dbg_styled('A', dbg_style_fg(dbg_color::red)),
					dbg_styled(this->name_.data(), dbg_style_fg(dbg_color::steel_blue)),
					dbg_styled(__file.data(), dbg_style_fg(dbg_color::orange)),
					dbg_styled(__line, dbg_style_fg(dbg_color::red)));
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
		nanoseconds_t min = __rts[0];
		nanoseconds_t max = __rts[0];
		nanoseconds_t avg = __rts[0];
		for (size_t i = 1; i < __c; ++i)
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
		std::sort(union_cast<int64_t*>(&__rts[0]), union_cast<int64_t*>(&__rts[0] + __c));// TODO: TSORT
		nanoseconds_t q1 = __rts[__c / 4];
		nanoseconds_t q2 = __rts[__c / 2];
		nanoseconds_t q3 = __rts[__c * 3 / 4];

		// 2. Print
		pf_print(
			"{: <32} {: <12} {: <16} {: <12} {: <12} {: <12} {: <12} {: <12} {: <12} {: <12} {: <12} {: <16}\n",
			dbg_styled(this->name().data(), dbg_emphasis::bold | dbg_style_fg(dbg_color::pale_golden_rod)),
			this->num_threads(), this->num_iterations(), min, max, avg, var, ect, q1, q2, q3, total);
	}
	/// TESTER: Engine
	// Constructors
	pulsar_api __tester_engine::__tester_engine() pf_attr_noexcept
		: unscoppedPack_(nullptr)
		, packHead_(nullptr)
		, packTail_(nullptr)
		, packCurr_(nullptr)
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
		this->packCurr_ = this->packHead_;
		size_t nt = 0;
		size_t nf = 0;
		while (this->packCurr_ )
		{
			this->packCurr_->__run();
			nt						 += this->packCurr_->numTests_;
			nf						 += this->packCurr_->numFailed_;
			this->packCurr_ = this->packCurr_->next_;
		}

		// 3. Print
		if (nt == 0)
		{
			pf_print(dbg_type::info, dbg_level::high, "Tester -> Nothing to test.\n");
		}
		else if (nf > 0)
		{
			pf_print(dbg_type::info, dbg_level::high, "Tester -> {} | {}\n",
							 dbg_styled(dbg_u8format("({}) Succeeded", nt - nf).data(),
													dbg_style_fg(dbg_color::green)),
							 dbg_styled(dbg_u8format("({}) Failed", nf).data(),
													dbg_style_fg(dbg_color::red)));
		}
		else
		{
			pf_print(dbg_type::info, dbg_level::high, "Tester -> {}",
							 dbg_styled(dbg_u8format("({}) Succeeded\n", nt).data(),
													dbg_style_fg(dbg_color::green)));
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
		this->packCurr_ = __p;
	}
	pulsar_api __tester_pack*
	__tester_engine::__cur_pack() pf_attr_noexcept
	{
		return this->packCurr_;
	}
}