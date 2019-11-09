//
// Created by 甘尧 on 2019-11-05.
//

#include "ir_for_a64.h"

using namespace Instruction::A64;

// IR Assembler
#define INST0(name, ret) ret& AssemblerIR::name() { \
    InstrIR &inst = InstrIRPool::Acquire(); \
    inst.opcode_ = static_cast<u8>(OpcodeIRA64::name); \
    ret r(&inst); \
    inst.return_ = r; \
    block_->Emit(inst); \
    return r; \
}
#define INST1(name, ret, arg1) ret& AssemblerIR::name(const arg1& a1) { \
    InstrIR &inst = InstrIRPool::Acquire(); \
    inst.opcode_ = static_cast<u8>(OpcodeIRA64::name); \
    ret r(&inst); \
    inst.return_ = r; \
    inst.SetArg(0, a1); \
    block_->Emit(inst); \
    return r; \
}
#define INST2(name, ret, arg1, arg2) ret& AssemblerIR::name(const arg1& a1, const arg2& a2) { \
    InstrIR &inst = InstrIRPool::Acquire(); \
    inst.opcode_ = static_cast<u8>(OpcodeIRA64::name); \
    ret r(&inst); \
    inst.return_ = r; \
    inst.SetArg(0, a1); \
    inst.SetArg(1, a2); \
    block_->Emit(inst); \
    return r; \
}

#include "opcodes_ir_a64.inl"

#undef INST0
#undef INST1
#undef INST2