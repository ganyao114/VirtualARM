//
// Created by 甘尧 on 2020-02-29.
//

#include <sys/mman.h>
#include "dbi_context_arm64.h"

using namespace DBI::A64;

Context::Context(const Register &reg_ctx) : masm_{}, REG_CTX{reg_ctx} {
    suspend_addr_ = static_cast<u64 *>(mmap(0, PAGE_SIZE, PROT_READ,
                                            MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED,
                                            -1, 0));
}

Context::~Context() {
    munmap(suspend_addr_, PAGE_SIZE);
}

const CPU::A64::CPUContext &Context::GetContext() const {
    return context_;
}

VAddr Context::GetCurPc() const {
    return cur_pc_;
}

void Context::SetCurPc(VAddr cur_pc) {
    cur_pc_ = cur_pc;
}

void Context::SetSuspendFlag(bool suspend) {
    mprotect(suspend_addr_, PAGE_SIZE, suspend ? PROT_NONE : PROT_READ);
}

ContextNoMemTrace::ContextNoMemTrace() : Context(TMP1) {
    HOST_TLS[CTX_TLS_SLOT] = &context_;
    HOST_TLS[SUSP_TLS_SLOT] = suspend_addr_;
}

void ContextNoMemTrace::WrapContext(std::function<void()> wrap) {
    __ Push(REG_CTX);
    __ Mrs(REG_CTX, TPIDR_EL0);
    __ Ldr(REG_CTX, MemOperand(REG_CTX, CTX_TLS_SLOT * 8));
    // save tmp0, tmp1
    __ Str(TMP0, MemOperand(REG_CTX, OFFSET_CTX_A64_CPU_REG + 17 * 8));
    __ Pop(TMP0);
    __ Str(TMP0, MemOperand(REG_CTX, OFFSET_CTX_A64_CPU_REG + 16 * 8));
    wrap();
    // restore tmp0, tmp1
    __ Ldp(TMP1, TMP0, MemOperand(REG_CTX, OFFSET_CTX_A64_CPU_REG + 16 * 8));
}

void ContextNoMemTrace::GetSp(u8 target) {
    auto rt = XRegister::GetXRegFromCode(target);
    __ Ldr(rt, MemOperand(REG_CTX, OFFSET_CTX_A64_SP));
}

void ContextNoMemTrace::GetPc(u8 target) {
    auto rt = XRegister::GetXRegFromCode(target);
    __ Ldr(rt, MemOperand(REG_CTX, OFFSET_CTX_A64_PC));
}

void ContextNoMemTrace::SavePc(VAddr pc) {
    __ Mov(TMP0, pc);
    __ Str(TMP0, MemOperand(REG_CTX, OFFSET_CTX_A64_PC));
}

void ContextNoMemTrace::CheckSuspend(bool in_context_wrap) {
    if (in_context_wrap) {
        __ Mrs(TMP0, TPIDR_EL0);
        __ Ldr(TMP0, MemOperand(TMP0, SUSP_TLS_SLOT * 8));
        // if suspend, trigger 11 signal
        __ Ldr(TMP0, MemOperand(TMP0));
    }
}

void ContextNoMemTrace::ReadTPIDR(u8 target) {
    auto wrap = [this, target]() -> void {
        if (TMP0.GetCode() == target || TMP1.GetCode() == target) {
            __ Ldr(TMP0, MemOperand(REG_CTX, OFFSET_CTX_A64_TPIDR));
            __ Str(TMP0, MemOperand(REG_CTX, OFFSET_CTX_A64_CPU_REG + target * 8));
        } else {
            __ Ldr(XRegister::GetXRegFromCode(target), MemOperand(REG_CTX, OFFSET_CTX_A64_TPIDR));
        }
    };
    WrapContext(wrap);
}

void ContextNoMemTrace::ReadTPIDRRO(u8 target) {
    auto wrap = [this, target]() -> void {
        if (TMP0.GetCode() == target || TMP1.GetCode() == target) {
            __ Ldr(TMP0, MemOperand(REG_CTX, OFFSET_CTX_A64_TPIDRRO));
            __ Str(TMP0, MemOperand(REG_CTX, OFFSET_CTX_A64_CPU_REG + target * 8));
        } else {
            __ Ldr(XRegister::GetXRegFromCode(target), MemOperand(REG_CTX, OFFSET_CTX_A64_TPIDRRO));
        }
    };
    WrapContext(wrap);
}


void ContextNoMemTrace::SaveContextFull() {
    auto wrap = [this]() -> void {
        // XRegs
        for (int i = 0; i < 32; ++i) {
            if (i == 16) {
                continue;
            }
            __ Stp(XRegister::GetXRegFromCode(i), XRegister::GetXRegFromCode(i + 1), MemOperand(REG_CTX, 16 * i));
        }
        // Sysregs
        __ Mrs(TMP0, NZCV);
        __ Str(TMP0.W(), MemOperand(REG_CTX, OFFSET_CTX_A64_PSTATE));
        __ Mrs(TMP0, FPCR);
        __ Str(TMP0.W(), MemOperand(REG_CTX, OFFSET_CTX_A64_FPCR));
        __ Mrs(TMP0, FPSR);
        __ Str(TMP0.W(), MemOperand(REG_CTX, OFFSET_CTX_A64_FPSR));
        // Sp
        __ Str(sp, MemOperand(REG_CTX, OFFSET_CTX_A64_SP));
        // Pc
        __ Mov(TMP0, cur_pc_);
        __ Str(TMP0, MemOperand(REG_CTX, OFFSET_CTX_A64_PC));
        // VRegs
        __ Add(TMP0, REG_CTX, OFFSET_CTX_A64_VEC_REG);
        for (int i = 0; i < 33; ++i) {
            __ Stp(VRegister::GetVRegFromCode(i), XRegister::GetXRegFromCode(i + 1), MemOperand(TMP0, 32 * i));
        }
    };
    WrapContext(wrap);
}

void ContextNoMemTrace::RestoreContextFull() {
    auto wrap = [this]() -> void {
        // XRegs
        for (int i = 0; i < 32; ++i) {
            if (i == 16) {
                continue;
            }
            __ Ldp(XRegister::GetXRegFromCode(i), XRegister::GetXRegFromCode(i + 1), MemOperand(REG_CTX, 16 * i));
        }
        // Sysregs
        __ Ldr(TMP0.W(), MemOperand(REG_CTX, OFFSET_CTX_A64_PSTATE));
        __ Msr(NZCV, TMP0);
        __ Ldr(TMP0.W(), MemOperand(REG_CTX, OFFSET_CTX_A64_FPCR));
        __ Msr(FPCR, TMP0);
        __ Ldr(TMP0.W(), MemOperand(REG_CTX, OFFSET_CTX_A64_FPSR));
        __ Msr(FPSR, TMP0);
        // Sp
        __ Ldr(sp, MemOperand(REG_CTX, OFFSET_CTX_A64_SP));
        // VRegs
        __ Add(TMP0, REG_CTX, OFFSET_CTX_A64_VEC_REG);
        for (int i = 0; i < 33; ++i) {
            __ Ldp(VRegister::GetVRegFromCode(i), XRegister::GetXRegFromCode(i + 1), MemOperand(TMP0, 32 * i));
        }
    };
    WrapContext(wrap);
}

void ContextNoMemTrace::PreDispatch() {
    __ Push(TMP0, TMP1);
}

void ContextNoMemTrace::PostDispatch() {
    __ Pop(TMP0, TMP1);
}

ContextWithMemTrace::ContextWithMemTrace() : Context(TMP1) {
    // Need keep CTX_REG, so rewrite all instructions used CTX_REG
}

void ContextWithMemTrace::LookupFlatPageTable(u8 reg_addr) {
    constexpr static int PAGE_BITS = 12;
    auto rt = XRegister::GetXRegFromCode(reg_addr);
    auto [tmp_reg1, tmp_reg2] = PeekTmpRegs(reg_addr);
    auto tmp1 = XRegister::GetXRegFromCode(tmp_reg1);
    auto tmp2 = XRegister::GetXRegFromCode(tmp_reg2);
    PushX(tmp_reg1, tmp_reg2);
    __ Mov(tmp1, Operand(XRegister::GetXRegFromCode(reg_addr), LSR, PAGE_BITS));
    __ Bfc(tmp1, sizeof(VAddr) * 8 - address_bits_unused_ - PAGE_BITS, address_bits_unused_ + PAGE_BITS);
    __ Mov(tmp2, page_tabel_addr_);
    __ Add(tmp1, tmp2, Operand(tmp1, LSL, 3));
    __ Ldr(rt, MemOperand(tmp1));
    PopX(tmp_reg1, tmp_reg2);
}

void ContextWithMemTrace::LookupFlatPageTable(VAddr const_addr, u8 reg) {
    constexpr static int PAGE_BITS = 12;
    auto rt = XRegister::GetXRegFromCode(reg);
    __ Mov(rt ,page_tabel_addr_ + BitRange<VAddr>(const_addr, PAGE_BITS, sizeof(VAddr) * 8 - address_bits_unused_ - 1) * 8);
    __ Ldr(rt, MemOperand(rt));
}

void ContextWithMemTrace::PushX(u8 reg1, u8 reg2) {
    assert(reg1 != REG_CTX.GetCode() && reg2 != REG_CTX.GetCode());
    if (reg2 == NO_REG) {
        auto rt = XRegister::GetXRegFromCode(reg1);
        __ Str(rt, MemOperand(REG_CTX, OFFSET_CTX_A64_CPU_REG + rt.GetCode() * 8));
    } else {
        auto rt1 = XRegister::GetXRegFromCode(reg1);
        auto rt2 = XRegister::GetXRegFromCode(reg2);
        if (reg2 - reg1 == 1) {
            __ Stp(rt1, rt2, MemOperand(REG_CTX, OFFSET_CTX_A64_CPU_REG + rt1.GetCode() * 8));
        } else {
            __ Str(rt1, MemOperand(REG_CTX, OFFSET_CTX_A64_CPU_REG + rt1.GetCode() * 8));
            __ Str(rt2, MemOperand(REG_CTX, OFFSET_CTX_A64_CPU_REG + rt2.GetCode() * 8));
        }
    }
}

void ContextWithMemTrace::PopX(u8 reg1, u8 reg2) {
    assert(reg1 != REG_CTX.GetCode() && reg2 != REG_CTX.GetCode());
    if (reg2 == NO_REG) {
        auto rt = XRegister::GetXRegFromCode(reg1);
        __ Ldr(rt, MemOperand(REG_CTX, OFFSET_CTX_A64_CPU_REG + rt.GetCode() * 8));
    } else {
        auto rt1 = XRegister::GetXRegFromCode(reg1);
        auto rt2 = XRegister::GetXRegFromCode(reg2);
        if (reg2 - reg1 == 1) {
            __ Ldp(rt1, rt2, MemOperand(REG_CTX, OFFSET_CTX_A64_CPU_REG + rt1.GetCode() * 8));
        } else {
            __ Ldr(rt1, MemOperand(REG_CTX, OFFSET_CTX_A64_CPU_REG + rt1.GetCode() * 8));
            __ Ldr(rt2, MemOperand(REG_CTX, OFFSET_CTX_A64_CPU_REG + rt2.GetCode() * 8));
        }
    }
}

std::pair<u8, u8> ContextWithMemTrace::PeekTmpRegs(u8 reg_target) {
    u8 r1 = 0;
    for (int i = 0; i < 30; ++i) {

    }
    return std::make_pair(0, 1);
}
