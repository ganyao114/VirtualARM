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
#define NO_REG 255
#define CTX_TLS_SLOT 7
#define SUSP_TLS_SLOT 8
#define TMP0 x17
#define TMP1 x16
#define HOST_TLS ({ void** __val; __asm__("mrs %0, tpidr_el0" : "=r"(__val)); __val; })

    class Context {
    public:

        Context(const Register &reg_ctx);
        virtual ~Context();

        const CPU::A64::CPUContext &GetContext() const;

        VAddr GetCurPc() const;
        void SetCurPc(VAddr cur_pc);

        void SetSuspendFlag(bool suspend);

        virtual void GetSp(u8 target) {};
        virtual void GetPc(u8 target) {};

        virtual void CheckSuspend(bool in_context_wrap) {};
        virtual void SavePc(VAddr pc) {};

        //sysreg
        virtual void ReadTPIDR(u8 target) {};
        virtual void ReadTPIDRRO(u8 target) {};

        // context switch
        virtual void SaveContextFull() {};
        virtual void RestoreContextFull() {};

        CPU::A64::CPUContext context_;
        VAddr cur_pc_;

    protected:
        const Register &REG_CTX;
        MacroAssembler masm_;
        u64 *suspend_addr_;
    };

    class ContextNoMemTrace : public Context {
    protected:
        void WrapContext(std::function<void()> wrap);

        void PreDispatch();
        void PostDispatch();

    public:
        ContextNoMemTrace();

        void GetSp(u8 target) override;
        void GetPc(u8 target) override;

        void CheckSuspend(bool in_context_wrap) override;

        void SavePc(VAddr pc) override;

        void ReadTPIDR(u8 target) override;
        void ReadTPIDRRO(u8 target) override;

        void SaveContextFull() override;
        void RestoreContextFull() override;
    };

    class ContextWithMemTrace : public Context {
    public:
        ContextWithMemTrace();

        void LookupFlatPageTable(u8 addr_reg);
        void LookupFlatPageTable(VAddr const_addr, u8 reg);

        VAddr page_tabel_addr_{0};

    protected:
        void PushX(u8 reg1, u8 reg2 = NO_REG);
        void PopX(u8 reg1, u8 reg2 = NO_REG);

        std::pair<u8, u8> PeekTmpRegs(u8 reg_target);
        u8 address_bits_unused_;
    };

}
