//
// Created by 甘尧 on 2020-02-29.
//

#pragma once

#include <aarch64/macro-assembler-aarch64.h>
#include "asm/arm64/cpu_arm64.h"
#include <base/marcos.h>

using namespace vixl::aarch64;

namespace DBI::A64 {

#define __ masm_.
#define CTX_TLS_SLOT 7
#define SUSP_TLS_SLOT 8
#define TMP0 x17
#define TMP1 x16
#define REG_CTX TMP1
#define HOST_TLS ({ void** __val; __asm__("mrs %0, tpidr_el0" : "=r"(__val)); __val; })

    class Context {
    public:

        Context();

        virtual ~Context();

        void SetSuspendFlag(bool suspend);

        const CPU::A64::CPUContext &GetContext() const;

        VAddr GetCurPc() const;
        void SetCurPc(VAddr cur_pc);

        CPU::A64::CPUContext context_;
        VAddr cur_pc_;
        VAddr page_tabel_addr_{0};

    protected:

        void WrapContext(std::function<void()> wrap);
        void SaveSp();
        void SavePc(VAddr pc);
        void GetSp(u8 target);
        void GetPc(u8 target);

        void LookupPageTable(u8 addr_reg);
        void LookupPageTable(VAddr const_addr, u8 reg);

        void CheckSuspend(bool in_context_wrap);

        //sysreg
        void ReadTpidr(u8 target);
        void ReadTpidrro(u8 target);

        MacroAssembler masm_;
        u64 *suspend_addr_;
    };

    class ContextSwitch : public Context {
        void SaveContextFull();
        void RestoreContextFull();
    };

}
