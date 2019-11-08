//
// Created by 甘尧 on 2019-10-28.
//

#include "assembler_ir.h"

using namespace Instruction::IR;


// IR Assembler
#define INST0(name, ret) ret& Assembler::name() { \
    InstrIR &inst = InstrIRPool::Acquire(); \
    inst.opcode_ = OpcodeIR::name; \
    ret r(&inst); \
    inst.return_ = r; \
    block_->Emit(inst); \
    return r; \
}
#define INST1(name, ret, arg1) ret& Assembler::name(arg1& a1) { \
    InstrIR &inst = InstrIRPool::Acquire(); \
    inst.opcode_ = OpcodeIR::name; \
    ret r(&inst); \
    inst.return_ = r; \
    inst.SetArg(0, a1); \
    block_->Emit(inst); \
    return r; \
}
#define INST2(name, ret, arg1, arg2) ret& Assembler::name(arg1& a1, arg2& a2) { \
    InstrIR &inst = InstrIRPool::Acquire(); \
    inst.opcode_ = OpcodeIR::name; \
    ret r(&inst); \
    inst.return_ = r; \
    inst.SetArg(0, a1); \
    inst.SetArg(1, a2); \
    block_->Emit(inst); \
    return r; \
}

#include "opcodes_ir.inl"

#undef INST0
#undef INST1
#undef INST2
