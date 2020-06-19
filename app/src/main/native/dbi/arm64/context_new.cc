//
// Created by SwiftGan on 2020/6/17.
//

#include "context_new.h"

using namespace DBI::A64;

#define __ masm_.

const XRegister &RegisterAllocator::ContextPtr() {
    if (!context_ptr_) {
        context_ptr_ = const_cast<XRegister *>(&context_->LoadContextPtr());
        MarkInUsed(*context_ptr_);
    }
    return *context_ptr_;
}

void RegisterAllocator::ClearContext() {
    if (context_ptr_) {
        MarkInUsed(*context_ptr_, false);
        context_->ClearContextPtr(*context_ptr_);
    }
}

const XRegister &RegisterAllocator::AcquireTempX() {
    for (int i = 0; i < 31; ++i) {
        if (!in_used_[i]) {
//            return XRegister.GetXRegFromCode(i);
        }
    }
    abort();
}

const XRegister &RegisterAllocator::ReleaseTempX(const XRegister &x) {
    return x0;
}

void RegisterAllocator::MarkInUsed(const Register &x, bool in_used) {
    in_used_[x.GetCode()] = in_used;
}

bool RegisterAllocator::InUsed(const Register &x) {
    return in_used_[x.GetCode()];
}

void RegisterAllocator::Reset() {
    std::memset(reinterpret_cast<void *>(in_used_[0]), 0, sizeof(in_used_));
}

void BaseContext::Set(const XRegister &x, u64 value) {
    if (!register_alloc_.InUsed(x)) {
        __ Mov(x, value);
    } else {
        register_alloc_.MarkInUsed(x);
        auto tmp = register_alloc_.AcquireTempX();
        __ Mov(tmp, value);
        __ Str(tmp, MemOperand(register_alloc_.ContextPtr(), 8 * x.GetCode()));
        register_alloc_.ReleaseTempX(tmp);
        register_alloc_.MarkInUsed(x, false);
    }
}

void BaseContext::Set(const WRegister &w, u32 value) {
    if (!register_alloc_.InUsed(w)) {
        __ Mov(w, value);
    } else {
        register_alloc_.MarkInUsed(w);
        auto tmp = register_alloc_.AcquireTempX();
        __ Mov(tmp.W(), value);
        __ Str(tmp, MemOperand(register_alloc_.ContextPtr(), 8 * w.GetCode()));
        register_alloc_.ReleaseTempX(tmp);
        register_alloc_.MarkInUsed(w, false);
    }
}

void BaseContext::Push(const Register &reg) {
    if (reg.IsFPRegister()) {
        __ Str(reg, MemOperand(register_alloc_.ContextPtr(), 8 * reg.GetCode()));
    } else if (reg.IsSP()) {
        auto &tmp = register_alloc_.AcquireTempX();
        __ Mov(tmp, reg);
        __ Str(reg, MemOperand(register_alloc_.ContextPtr(), 8 * reg.GetCode()));
        register_alloc_.ReleaseTempX(tmp);
    } else {
        register_alloc_.MarkInUsed(reg);
        __ Str(reg, MemOperand(register_alloc_.ContextPtr(), 16 * reg.GetCode()));
        register_alloc_.MarkInUsed(reg, false);
    }
}

void BaseContext::Pop(const Register &reg) {
    if (reg.IsFPRegister()) {
        __ Ldr(reg, MemOperand(register_alloc_.ContextPtr(), 8 * reg.GetCode()));
    } else if (reg.IsSP()) {
        auto &tmp = register_alloc_.AcquireTempX();
        __ Ldr(tmp, MemOperand(register_alloc_.ContextPtr(), 8 * reg.GetCode()));
        __ Mov(reg, tmp);
        register_alloc_.ReleaseTempX(tmp);
    } else {
        __ Ldr(reg, MemOperand(register_alloc_.ContextPtr(), 16 * reg.GetCode()));
    }
}

void BaseContext::MarkPC(u64 value) {
    auto tmp = register_alloc_.AcquireTempX();
    __ Mov(tmp, value);
    __ Str(tmp, MemOperand(register_alloc_.ContextPtr(), OFFSET_CTX_A64_PC));
    register_alloc_.ReleaseTempX(tmp);
}

void BaseContext::Push(const Register &reg1, const Register &reg2) {
    assert(reg2.GetCode() - reg1.GetCode() == 1);
    assert(!reg1.IsSP() && !reg2.IsSP());
}

void BaseContext::Pop(const Register &reg1, const Register &reg2) {
    assert(reg2.GetCode() - reg1.GetCode() == 1);
    assert(!reg1.IsSP() && !reg2.IsSP());
}

const XRegister &QuickContext::LoadContextPtr() {
    const auto &context_reg = x17;
    __ Push(context_reg);
    __ Mrs(context_reg, TPIDR_EL0);
    __ Ldr(context_reg, MemOperand(context_reg, CTX_TLS_SLOT * 8));
    // save tmp0, tmp1
    __ Str(x16, MemOperand(context_reg, OFFSET_CTX_A64_CPU_REG + x16.GetCode() * 8));
    __ Pop(x16);
    __ Str(x16, MemOperand(context_reg, OFFSET_CTX_A64_CPU_REG + context_reg.GetCode() * 8));
    // restore tmp0
    __ Ldr(x16, MemOperand(context_reg, OFFSET_CTX_A64_CPU_REG + x16.GetCode() * 8));
    register_alloc_.MarkInUsed(context_reg);
    return context_reg;
}

void QuickContext::ClearContextPtr(const XRegister &context) {
    __ Ldr(context, MemOperand(context, 8 * context.GetCode()));
    register_alloc_.MarkInUsed(context, false);
}

#undef __