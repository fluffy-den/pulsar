/*! @file   system.hpp
 *  @author Louis-Quentin Noé (noe.louis-quentin@hotmail.fr)
 *  @brief
 *  @date   30-01-2023
 *
 *  @copyright Copyright (c) 2023 - Pulsar Software
 *
 *  @since 0.1.3
 */

#ifndef PULSAR_SYSTEM_HPP
#define PULSAR_SYSTEM_HPP 1

// Include: Pulsar
#include "pulsar/pulsar.hpp"
#include "pulsar/iterable.hpp"
#include "pulsar/utf8.hpp"

// Pulsar
namespace pul
{
	/*
	 * !                 !
	 *   Instruction Set
	 * !                 !
	 */
	/// INSTRUCTION: Set
	class instruction_set
	{
		/// Vendor
		void
		__retrieve_vendor(char_t *__vendor) const pf_attr_noexcept
		{
			int32_t *p = union_cast<int32_t *>(__vendor);
			*(p + 0)	 = this->vendor_[1];
			*(p + 1)	 = this->vendor_[3];
			*(p + 2)	 = this->vendor_[2];
		}

		/// Name
		void
		__retrieve_brand(char_t *__brand) const pf_attr_noexcept
		{
			iterator<const char_t> beg = union_cast<const char_t *>(this->brand1_.data());
			copy(beg + 00, beg + 16, iterator(__brand));
			copy(beg + 16, beg + 32, iterator(__brand));
			copy(beg + 32, beg + 48, iterator(__brand));
		}

	public:
		/// Constructors
		instruction_set() pf_attr_noexcept
			: brand1_ { 0 }
			, brand2_ { 0 }
			, brand3_ { 0 }
			, vendor_ { 0 }
			, nIDs_(0)
			, nExIDs_(0)
			, f_1_ECX_(0)
			, f_1_EDX_(0)
			, f_7_EBX_(0)
			, f_7_ECX_(0)
			, f_81_ECX_(0)
			, f_81_EDX_(0)
		{
			// https://learn.microsoft.com/fr-fr/cpp/intrinsics/cpuid-cpuidex?view=msvc-170
			// Cpui
			array<int32_t, 4> cpui;
			__cpuid(cpui.data(), 0);
			this->nIDs_ = cpui[0];

			// Vendor
			__cpuidex(cpui.data(), 0, 0);
			this->vendor_ = cpui;

			// load bitset with flags for function 0x00000001
			if(this->nIDs_ >= 1)
			{
				__cpuidex(cpui.data(), 1, 0);
				f_1_ECX_ = cpui[2];
				f_1_EDX_ = cpui[3];
			}

			// load bitset with flags for function 0x00000007
			if(this->nIDs_ >= 7)
			{
				__cpuidex(cpui.data(), 7, 0);
				f_7_EBX_ = cpui[1];
				f_7_ECX_ = cpui[2];
			}

			// CpuiEx
			__cpuid(cpui.data(), 0x80'00'00'00);
			this->nExIDs_ = cpui[0];

			// load bitset with flags for function 0x80000001
			if(this->nExIDs_ >= union_cast<int32_t>(0x80'00'00'01))
			{
				__cpuidex(cpui.data(), 0x80'00'00'01, 0);
				f_81_ECX_ = cpui[2];
				f_81_EDX_ = cpui[3];
			}

			// Brand
			if(this->nExIDs_ >= union_cast<int32_t>(0x80'00'00'04))
			{
				__cpuidex(cpui.data(), 0x80'00'00'02, 0);
				this->brand1_ = cpui;
				__cpuidex(cpui.data(), 0x80'00'00'03, 0);
				this->brand2_ = cpui;
				__cpuidex(cpui.data(), 0x80'00'00'04, 0);
				this->brand3_ = cpui;
			}
		}
		instruction_set(const instruction_set &) pf_attr_noexcept = delete;
		instruction_set(instruction_set &&) pf_attr_noexcept			= delete;

		/// Destructor
		~instruction_set() pf_attr_noexcept = default;

		/// Operator =
		instruction_set &
		operator=(const instruction_set &) pf_attr_noexcept = delete;
		instruction_set &
		operator=(instruction_set &&) pf_attr_noexcept = delete;

		/// f_1_ECX
		pf_hint_nodiscard bool
		sse3() const pf_attr_noexcept
		{
			return this->f_1_ECX_ & 0b1;
		}
		pf_hint_nodiscard bool
		pclmulqdq() const pf_attr_noexcept
		{
			return (this->f_1_ECX_ >> 1) & 0b1;
		}
		pf_hint_nodiscard bool
		monitor() const pf_attr_noexcept
		{
			return (this->f_1_ECX_ >> 3) & 0b1;
		}
		pf_hint_nodiscard bool
		fma() const pf_attr_noexcept
		{
			return (this->f_1_ECX_ >> 9) & 0b1;
		}
		pf_hint_nodiscard bool
		ssse3() const pf_attr_noexcept
		{
			return (this->f_1_ECX_ >> 12) & 0b1;
		}
		pf_hint_nodiscard bool
		cmpxchg16b() const pf_attr_noexcept
		{
			return (this->f_1_ECX_ >> 13) & 0b1;
		}
		pf_hint_nodiscard bool
		sse41() const pf_attr_noexcept
		{
			return (this->f_1_ECX_ >> 19) & 0b1;
		}
		pf_hint_nodiscard bool
		sse42() const pf_attr_noexcept
		{
			return (this->f_1_ECX_ >> 20) & 0b1;
		}
		pf_hint_nodiscard bool
		movbe() const pf_attr_noexcept
		{
			return (this->f_1_ECX_ >> 22) & 0b1;
		}
		pf_hint_nodiscard bool
		popcnt() const pf_attr_noexcept
		{
			return (this->f_1_ECX_ >> 23) & 0b1;
		}
		pf_hint_nodiscard bool
		aes() const pf_attr_noexcept
		{
			return (this->f_1_ECX_ >> 25) & 0b1;
		}
		pf_hint_nodiscard bool
		xsave() const pf_attr_noexcept
		{
			return (this->f_1_ECX_ >> 26) & 0b1;
		}
		pf_hint_nodiscard bool
		osxsave() const pf_attr_noexcept
		{
			return (this->f_1_ECX_ >> 27) & 0b1;
		}
		pf_hint_nodiscard bool
		avx() const pf_attr_noexcept
		{
			return (this->f_1_ECX_ >> 28) & 0b1;
		}
		pf_hint_nodiscard bool
		f16c() const pf_attr_noexcept
		{
			return (this->f_1_ECX_ >> 29) & 0b1;
		}
		pf_hint_nodiscard bool
		rdrand() const pf_attr_noexcept
		{
			return (this->f_1_ECX_ >> 30) & 0b1;
		}

		/// f_1_EDX
		pf_hint_nodiscard bool
		msr() const pf_attr_noexcept
		{
			return (this->f_1_EDX_ >> 5) & 0b1;
		}
		pf_hint_nodiscard bool
		cx8() const pf_attr_noexcept
		{
			return (this->f_1_EDX_ >> 8) & 0b1;
		}
		pf_hint_nodiscard bool
		sep() const pf_attr_noexcept
		{
			return (this->f_1_EDX_ >> 11) & 0b1;
		}
		pf_hint_nodiscard bool
		cmov() const pf_attr_noexcept
		{
			return (this->f_1_EDX_ >> 15) & 0b1;
		}
		pf_hint_nodiscard bool
		clfsh() const pf_attr_noexcept
		{
			return (this->f_1_EDX_ >> 19) & 0b1;
		}
		pf_hint_nodiscard bool
		mmx() const pf_attr_noexcept
		{
			return (this->f_1_EDX_ >> 23) & 0b1;
		}
		pf_hint_nodiscard bool
		fxsr() const pf_attr_noexcept
		{
			return (this->f_1_EDX_ >> 24) & 0b1;
		}
		pf_hint_nodiscard bool
		sse() const pf_attr_noexcept
		{
			return (this->f_1_EDX_ >> 25) & 0b1;
		}
		pf_hint_nodiscard bool
		sse2() const pf_attr_noexcept
		{
			return (this->f_1_EDX_ >> 26) & 0b1;
		}

		/// f_7_EBX
		pf_hint_nodiscard bool
		fsgsbase() const pf_attr_noexcept
		{
			return this->f_7_EBX_ & 0b1;
		}
		pf_hint_nodiscard bool
		bmi1() const pf_attr_noexcept
		{
			return (this->f_7_EBX_ >> 3) & 0b1;
		}
		pf_hint_nodiscard bool
		hle() const pf_attr_noexcept
		{
			return this->is_intel() && (this->f_7_EBX_ >> 4) & 0b1;
		}
		pf_hint_nodiscard bool
		avx2() const pf_attr_noexcept
		{
			return (this->f_7_EBX_ >> 5) & 0b1;
		}
		pf_hint_nodiscard bool
		bmi2() const pf_attr_noexcept
		{
			return (this->f_7_EBX_ >> 8) & 0b1;
		}
		pf_hint_nodiscard bool
		erms() const pf_attr_noexcept
		{
			return (this->f_7_EBX_ >> 9) & 0b1;
		}
		pf_hint_nodiscard bool
		invpcid() const pf_attr_noexcept
		{
			return (this->f_7_EBX_ >> 10) & 0b1;
		}
		pf_hint_nodiscard bool
		rtm() const pf_attr_noexcept
		{
			return this->is_intel() && (this->f_7_EBX_ >> 11) & 0b1;
		}
		pf_hint_nodiscard bool
		avx512f() const pf_attr_noexcept
		{
			return (this->f_7_EBX_ >> 16) & 0b1;
		}
		pf_hint_nodiscard bool
		rdseed() const pf_attr_noexcept
		{
			return (this->f_7_EBX_ >> 18) & 0b1;
		}
		pf_hint_nodiscard bool
		adx() const pf_attr_noexcept
		{
			return (this->f_7_EBX_ >> 19) & 0b1;
		}
		pf_hint_nodiscard bool
		avx512pf() const pf_attr_noexcept
		{
			return (this->f_7_EBX_ >> 26) & 0b1;
		}
		pf_hint_nodiscard bool
		avx512er() const pf_attr_noexcept
		{
			return (this->f_7_EBX_ >> 27) & 0b1;
		}
		pf_hint_nodiscard bool
		avx512cd() const pf_attr_noexcept
		{
			return (this->f_7_EBX_ >> 28) & 0b1;
		}
		pf_hint_nodiscard bool
		sha() const pf_attr_noexcept
		{
			return (this->f_7_EBX_ >> 29) & 0b1;
		}

		/// f_7_ECX
		pf_hint_nodiscard bool
		prefetchwt1() const pf_attr_noexcept
		{
			return this->f_7_ECX_ & 0b1;
		}

		/// f_81_ECX
		pf_hint_nodiscard bool
		lahf() const pf_attr_noexcept
		{
			return this->f_81_ECX_ & 0b1;
		}
		pf_hint_nodiscard bool
		lzcnt() const pf_attr_noexcept
		{
			return this->is_intel() && (this->f_81_ECX_ >> 5) & 0b1;
		}
		pf_hint_nodiscard bool
		abm() const pf_attr_noexcept
		{
			return this->is_amd() && (this->f_81_ECX_ >> 5) & 0b1;
		}
		pf_hint_nodiscard bool
		sse4a() const pf_attr_noexcept
		{
			return this->is_amd() && (this->f_81_ECX_ >> 6) & 0b1;
		}
		pf_hint_nodiscard bool
		xop() const pf_attr_noexcept
		{
			return this->is_amd() && (this->f_81_ECX_ >> 11) & 0b1;
		}
		pf_hint_nodiscard bool
		tbm() const pf_attr_noexcept
		{
			return this->is_amd() && (this->f_81_ECX_ >> 21) & 0b1;
		}

		/// f_81_EDX
		pf_hint_nodiscard bool
		syscall() const pf_attr_noexcept
		{
			return this->is_intel() && (this->f_81_EDX_ >> 11) & 0b1;
		}
		pf_hint_nodiscard bool
		mmxext() const pf_attr_noexcept
		{
			return this->is_amd() && (this->f_81_EDX_ >> 22) & 0b1;
		}
		pf_hint_nodiscard bool
		rdtscp() const pf_attr_noexcept
		{
			return this->is_intel() && (this->f_81_EDX_ >> 27) & 0b1;
		}
		pf_hint_nodiscard bool
		mmx3Dnowext() const pf_attr_noexcept
		{
			return this->is_amd() && (this->f_81_EDX_ >> 30) & 0b1;
		}
		pf_hint_nodiscard bool
		mmx3Dnow() const pf_attr_noexcept
		{
			return this->is_amd() && (this->f_81_EDX_ >> 31) & 0b1;
		}

		/// Is
		pf_hint_nodiscard bool
		is_amd() const pf_attr_noexcept
		{
			array<char_t, 0x20> vendor;
			array<char_t, 0x20> asAMD = { "AuthenticAMD" };
			this->__retrieve_vendor(vendor.data());
			return vendor == asAMD;
		}
		pf_hint_nodiscard bool
		is_intel() const pf_attr_noexcept
		{
			array<char_t, 0x20> vendor;
			array<char_t, 0x20> asIntel = { "GenuineIntel" };
			this->__retrieve_vendor(vendor.data());
			return vendor == asIntel;
		}

		/// Name
		pf_hint_nodiscard string<char_traits::ascii, magnifier_default, allocator_default>
		brand() const pf_attr_noexcept
		{
			array<char_t, 0x40> brand;
			this->__retrieve_brand(brand.data());
			return string<char_traits::ascii, magnifier_default, allocator_default>(brand, 0x40);
		}
		pf_hint_nodiscard string<char_traits::ascii, magnifier_default, allocator_default>
		vendor() const pf_attr_noexcept
		{
			array<char_t, 0x20> vendor;
			this->__retrieve_vendor(vendor.data());
			return string<char_traits::ascii, magnifier_default, allocator_default>(vendor, 0x20);
		}

	private:
		array<int32_t, 4> brand1_;
		array<int32_t, 4> brand2_;
		array<int32_t, 4> brand3_;
		array<int32_t, 4> vendor_;
		int32_t nIDs_;
		int32_t nExIDs_;
		int32_t f_1_ECX_;
		int32_t f_1_EDX_;
		int32_t f_7_EBX_;
		int32_t f_7_ECX_;
		int32_t f_81_ECX_;
		int32_t f_81_EDX_;
	};

	/// SYSTEM: Memory
	struct memory_info_t
	{
		size_t physTotal;
		size_t physAvail;
		size_t pageFileTotal;
		size_t pageFileAvail;
		size_t virtTotal;
		size_t virtAvail;
		size_t virtExtendedAvail;
		uint32_t memoryLoad;
	};

	pf_hint_nodiscard pulsar_api memory_info_t
	get_process_memory_usage() pf_attr_noexcept;	// TODO: Impl of get_process_memory_usage
	pf_hint_nodiscard pulsar_api memory_info_t
	get_system_memory_usage() pf_attr_noexcept;		// TODO: Impl of get_system_memory_usage

	/// SYSTEM: OS
	pf_hint_nodiscard pulsar_api u8string_t
	get_os_name() pf_attr_noexcept;	 // TODO: Impl of get_os_name
}	 // namespace pul

#endif	// !PULSAR_SYSTEM_HPP
