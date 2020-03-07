//
// Created by 甘尧 on 2020-02-29.
//

#include <sys/mman.h>
#include "dbi_context_arm64.h"

using namespace DBI::A64;

Context::Context(const Register &reg_ctx) : masm_{}, REG_CTX{reg_ctx} {
    suspend_addr_ = reinterpret_cast<u64>(static_cast<u64 *>(mmap(0, PAGE_SIZE, PROT_READ,
                                                                  MAP_PRIVATE | MAP_ANONYMOUS |
                                                                  MAP_FIXED,
                                                                  -1, 0)));
    context_.suspend_flag = suspend_addr_;
    code_find_table_ = SharedPtr<FindTable<VAddr>>(new FindTable<VAddr>(48, 2));
}

Context::~Context() {
    munmap(reinterpret_cast<void*>(suspend_addr_), PAGE_SIZE);
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
    mprotect(reinterpret_cast<void*>(suspend_addr_), PAGE_SIZE, suspend ? PROT_NONE : PROT_READ);
}


void Context::PushX(Register reg1, Register reg2) {
    assert(reg1.GetCode() != REG_CTX.GetCode() && reg2.GetCode() != REG_CTX.GetCode());
    if (reg2.GetCode() == NoReg.GetCode()) {
        __ Str(reg1, MemOperand(REG_CTX, OFFSET_CTX_A64_CPU_REG + reg1.GetCode() * 8));
    } else {
        if (reg2.GetCode() - reg1.GetCode() == 1) {
            __ Stp(reg1, reg2, MemOperand(REG_CTX, OFFSET_CTX_A64_CPU_REG + reg1.GetCode() * 8));
        } else {
            __ Str(reg1, MemOperand(REG_CTX, OFFSET_CTX_A64_CPU_REG + reg1.GetCode() * 8));
            __ Str(reg2, MemOperand(REG_CTX, OFFSET_CTX_A64_CPU_REG + reg2.GetCode() * 8));
        }
    }
}

void Context::PopX(Register reg1, Register reg2) {
    assert(reg1.GetCode() != REG_CTX.GetCode() && reg2.GetCode() != REG_CTX.GetCode());
    if (reg2.GetCode() == NoReg.GetCode()) {
        __ Ldr(reg1, MemOperand(REG_CTX, OFFSET_CTX_A64_CPU_REG + reg1.GetCode() * 8));
    } else {
        if (reg2.GetCode() - reg1.GetCode() == 1) {
            __ Ldp(reg1, reg2, MemOperand(REG_CTX, OFFSET_CTX_A64_CPU_REG + reg1.GetCode() * 8));
        } else {
            __ Ldr(reg1, MemOperand(REG_CTX, OFFSET_CTX_A64_CPU_REG + reg1.GetCode() * 8));
            __ Ldr(reg2, MemOperand(REG_CTX, OFFSET_CTX_A64_CPU_REG + reg2.GetCode() * 8));
        }
    }
}

void Context::CheckSuspend(Register tmp) {
    __ Ldr(tmp, MemOperand(REG_CTX, OFFSET_CTX_A64_SUSPEND_ADDR));
    // if suspend, trigger 11 signal
    __ Ldr(tmp, MemOperand(TMP0));
}

void Context::SavePc(VAddr pc, Register tmp) {
    __ Mov(tmp, pc);
    __ Str(tmp, MemOperand(REG_CTX, OFFSET_CTX_A64_PC));
}

void Context::LoadFromContext(Register rt, VAddr offset) {
    if (REG_CTX.GetCode() == rt.GetCode()) {
        auto wrap = [this, rt, offset](std::array<Register, 1> tmp) -> void {
            __ Ldr(tmp[0], MemOperand(REG_CTX, offset));
            __ Str(tmp[0], MemOperand(REG_CTX, OFFSET_CTX_A64_CPU_REG + rt.GetCode() * 8));
        };
        WrapContext<1>(wrap, {rt});
    } else {
        auto wrap = [this, rt, offset](std::array<Register, 0> tmp) -> void {
            __ Ldr(rt, MemOperand(REG_CTX, offset));
        };
        WrapContext<0>(wrap, {rt});
    }
}

void Context::ReadTPIDR(u8 target) {
    LoadFromContext(XRegister::GetXRegFromCode(target), OFFSET_CTX_A64_TPIDR);
}

void Context::ReadTPIDRRO(u8 target) {
    LoadFromContext(XRegister::GetXRegFromCode(target), OFFSET_CTX_A64_TPIDRRO);
}

void Context::FindForwardTarget(u8 reg_target) {
    auto rt = XRegister::GetXRegFromCode(reg_target);
    auto wrap = [this, rt](std::array<Register, 2> tmp) -> void {
        Label miss_target, label_loop, label_end, gen_code;
        // Find hash table
        __ Ldr(tmp[0], MemOperand(REG_CTX, OFFSET_CTX_A64_DISPATCHER_TABLE));
        __ Mov(tmp[1], Operand(rt, LSR, CODE_CACHE_HASH_BITS));
        __ Bfc(tmp[1], code_find_table_->TableBits(), sizeof(VAddr) * 8 - code_find_table_->TableBits());
        __ Ldr(tmp[0], MemOperand(tmp[0], tmp[1], LSL, 3));
        __ Cbz(tmp[0], &miss_target);
        __ And(tmp[1], rt, (CODE_CACHE_HASH_SIZE - CODE_CACHE_HASH_OVERP) << 2);
        // 2 + 2 = 4 = 16字节 = Entry 大小
        __ Adds(tmp[0], tmp[0], Operand(tmp[1], LSL, 2));
        __ Bind(&label_loop);
        __ Ldr(tmp[1], MemOperand(tmp[0], 16, PostIndex));
        __ Cbz(tmp[1], &miss_target);
        __ Sub(tmp[1], tmp[1], rt);
        __ Cbnz(tmp[1], &label_loop);
        // find target
        __ Ldr(rt, MemOperand(tmp[0], -8, PreIndex));
        __ B(&label_end);
        // can not found table
        __ Bind(&miss_target);
        __ Str(rt, MemOperand(REG_CTX, OFFSET_CTX_A64_FORWARD));
        PopX<2>(tmp);
        __ Bl(&gen_code);
        __ Ldr(rt, MemOperand(REG_CTX, OFFSET_CTX_A64_FORWARD));
        __ Bind(&label_end);
    };
    WrapContext<2>(wrap, {rt});
}

void Context::FindForwardTarget(VAddr const_target) {
}


ContextNoMemTrace::ContextNoMemTrace() : Context(TMP1) {
    HOST_TLS[CTX_TLS_SLOT] = &context_;
}

void ContextNoMemTrace::GetSp(u8 target) {
    auto rt = XRegister::GetXRegFromCode(target);
    __ Ldr(rt, MemOperand(REG_CTX, OFFSET_CTX_A64_SP));
}

void ContextNoMemTrace::GetPc(u8 target) {
    auto rt = XRegister::GetXRegFromCode(target);
    __ Ldr(rt, MemOperand(REG_CTX, OFFSET_CTX_A64_PC));
}


void ContextNoMemTrace::SaveContextFull() {
    auto wrap = [this](std::array<Register, 1> tmp) -> void {
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
    WrapContext<1>(wrap, {});
}

void ContextNoMemTrace::RestoreContextFull() {
    auto wrap = [this](std::array<Register, 1> tmp) -> void {
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
    WrapContext<1>(wrap, {});
}

void ContextNoMemTrace::PreDispatch() {
    __ Push(TMP0, TMP1);
}

void ContextNoMemTrace::PostDispatch() {
    __ Pop(TMP0, TMP1);
}

void ContextNoMemTrace::LoadContext() {
    __ Push(REG_CTX);
    __ Mrs(REG_CTX, TPIDR_EL0);
    __ Ldr(REG_CTX, MemOperand(REG_CTX, CTX_TLS_SLOT * 8));
    // save tmp0, tmp1
    __ Str(TMP0, MemOperand(REG_CTX, OFFSET_CTX_A64_CPU_REG + TMP0.GetCode() * 8));
    __ Pop(TMP0);
    __ Str(TMP0, MemOperand(REG_CTX, OFFSET_CTX_A64_CPU_REG + REG_CTX.GetCode() * 8));
    // restore tmp0
    __ Ldr(TMP0, MemOperand(REG_CTX, OFFSET_CTX_A64_CPU_REG + TMP0.GetCode() * 8));
}

void ContextNoMemTrace::ClearContext() {
    __ Ldr(REG_CTX, MemOperand(REG_CTX, OFFSET_CTX_A64_CPU_REG + REG_CTX.GetCode() * 8));
}

ContextWithMemTrace::ContextWithMemTrace(SharedPtr<PageTable> page_table) : Context(TMP1), page_table_(page_table) {
    // Need keep CTX_REG, so rewrite all instructions used CTX_REG
    page_bits_ = page_table->GetPageBits();
    address_bits_unused_ = page_table->GetUnusedBits();
    tlb_bits_ = page_table->Tbl()->TLBBits();
}

void ContextWithMemTrace::LookupFlatPageTable(u8 reg_addr) {
    auto rt = XRegister::GetXRegFromCode(reg_addr);
    auto wrap = [this, rt](std::array<Register, 2> tmp) -> void {
        auto tmp1 = tmp[0];
        auto tmp2 = tmp[1];
        __ Mov(tmp1, Operand(rt, LSR, page_bits_));
        __ Bfc(tmp1, sizeof(VAddr) * 8 - address_bits_unused_ - page_bits_, address_bits_unused_ + page_bits_);
        __ Ldr(tmp2, MemOperand(REG_CTX, OFFSET_CTX_A64_PAGE_TABLE));
        __ Add(tmp1, tmp2, Operand(tmp1, LSL, 3));
        __ Ldr(rt, MemOperand(tmp1));
    };
    WrapContext<2>(wrap, {rt});
}

void ContextWithMemTrace::LookupFlatPageTable(VAddr const_addr, u8 reg) {
    auto rt = XRegister::GetXRegFromCode(reg);
    auto wrap = [this, rt, const_addr](std::array<Register, 1> tmp) -> void {
        auto tmp1 = tmp[0];
        PushX(tmp1);
        __ Ldr(rt, MemOperand(REG_CTX, OFFSET_CTX_A64_PAGE_TABLE));
        __ Mov(tmp1 ,BitRange<VAddr>(const_addr, page_bits_, sizeof(VAddr) * 8 - address_bits_unused_ - 1) * 8);
        __ Add(rt, rt, tmp1);
        __ Ldr(rt, MemOperand(rt));
    };
    WrapContext<1>(wrap, {rt});
}

void ContextWithMemTrace::LookupTLB(u8 reg_addr) {
    auto rt = XRegister::GetXRegFromCode(reg_addr);
    auto wrap = [this, rt](std::array<Register, 3> tmp) -> void {
        Label label_hit, label_end, label_lookup_page_table;
        auto tmp1 = tmp[0];
        auto tmp2 = tmp[1];
        auto tmp3 = tmp[2];
        __ Mov(tmp1, Operand(rt, LSR, page_bits_));
        __ Bfc(tmp1, tlb_bits_, sizeof(VAddr) * 8 - tlb_bits_);
        __ Ldr(tmp2, MemOperand(REG_CTX, OFFSET_CTX_A64_TLB));
        // PTE size of a64 = 8, key = 8,so size of tlb entry = 16
        __ Add(tmp1, tmp2, Operand(tmp1, LSL, 4));
        __ Ldr(tmp3, MemOperand(tmp1));
        __ Mov(tmp2, Operand(rt, LSR, page_bits_));
        __ Sub(tmp3, tmp3, tmp2);
        __ Cbz(tmp3, &label_hit);
        // miss cache
        __ Str(tmp2, MemOperand(REG_CTX, OFFSET_CTX_A64_QUERY_PAGE));
        PopX<3>(tmp);
        __ Bl(&label_lookup_page_table);
        __ Ldr(rt, MemOperand(REG_CTX, OFFSET_CTX_A64_QUERY_PAGE));
        __ B(&label_end);
        // hit, load pte
        __ Bind(&label_hit);
        __ Ldr(rt, MemOperand(tmp1, 8));
        __ Bind(&label_end);
    };
    WrapContext<3>(wrap, {rt});
}

void ContextWithMemTrace::LookupMultiLevelPageTable(u8 addr_reg) {

}

void ContextWithMemTrace::CheckReadSpec(Register pte_reg, Register offset_reg) {
    Label label_skip, label_hook;
    __ Tbz(pte_reg, READ_SPEC_BITS, &label_skip);
    // go hook
    __ Bl(&label_hook);
    __ Bind(&label_skip);
}

void ContextWithMemTrace::CheckWriteSpec(Register pte_reg, Register offset_reg) {
    Label label_skip, label_hook;
    __ Tbz(pte_reg, WRITE_SPEC_BITS, &label_skip);
    // go hook
    __ Bl(&label_hook);
    __ Bind(&label_skip);
}
