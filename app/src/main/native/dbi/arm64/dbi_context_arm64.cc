//
// Created by 甘尧 on 2020-02-29.
//

#include <sys/mman.h>
#include "dbi_context_arm64.h"

using namespace DBI::A64;

Context::Context() : masm_{} {
    HOST_TLS[CTX_TLS_SLOT] = &context_;
    suspend_addr_ = static_cast<u64 *>(mmap(0, PAGE_SIZE, PROT_READ,
                                            MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED,
                                            -1, 0));
    HOST_TLS[SUSP_TLS_SLOT] = suspend_addr_;
}

Context::~Context() {
    munmap(suspend_addr_, PAGE_SIZE);
}

void Context::WrapContext(std::function<void()> wrap) {
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

void Context::GetSp(u8 target) {
    auto rt = XRegister::GetXRegFromCode(target);
    __ Ldr(rt, MemOperand(REG_CTX, OFFSET_CTX_A64_SP));
}

void Context::GetPc(u8 target) {
    auto rt = XRegister::GetXRegFromCode(target);
    __ Ldr(rt, MemOperand(REG_CTX, OFFSET_CTX_A64_PC));
}

void Context::SavePc(VAddr pc) {
    __ Mov(TMP0, pc);
    __ Str(TMP0, MemOperand(REG_CTX, OFFSET_CTX_A64_PC));
}

void Context::SaveSp() {
    __ Str(sp, MemOperand(REG_CTX, OFFSET_CTX_A64_SP));
}

void Context::LookupPageTable(u8 reg_addr) {
    // TODO pick tmp regs
    __ Mov(TMP1, page_tabel_addr_);
}

void Context::LookupPageTable(VAddr const_addr, u8 reg) {

}

void Context::SetSuspendFlag(bool suspend) {
    mprotect(suspend_addr_, PAGE_SIZE, suspend ? PROT_NONE : PROT_READ);
}

void Context::CheckSuspend(bool in_context_wrap) {
    if (in_context_wrap) {
        __ Mrs(TMP0, TPIDR_EL0);
        __ Ldr(TMP0, MemOperand(TMP0, SUSP_TLS_SLOT * 8));
        // if suspend, trigger 11 signal
        __ Ldr(TMP0, MemOperand(TMP0));
    }
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

void Context::ReadTpidr(u8 target) {
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

void Context::ReadTpidrro(u8 target) {
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


void ContextSwitch::SaveContextFull() {
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

void ContextSwitch::RestoreContextFull() {
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
