//
// Created by 甘尧 on 2019-10-28.
//

#include "assembler_ir.h"

using namespace Instruction::IR;


// IR Assembler
#define INST0(name, ret) ret& Assembler::Emit##name() { \
    InstrIR &inst = InstrIRPool::Acquire(); \
    inst.opcode_ = OpcodeIR::name; \
    ret r(&inst); \
    inst.return_ = r; \
    return r; \
}
#define INST1(name, ret, arg1) ret& Assembler::Emit##name(arg1& a1) { \
    InstrIR &inst = InstrIRPool::Acquire(); \
    inst.opcode_ = OpcodeIR::name; \
    ret r(&inst); \
    inst.return_ = r; \
    inst.SetArg(0, a1); \
    return r; \
}
#define INST2(name, ret, arg1, arg2) ret& Assembler::Emit##name(arg1& a1, arg2& a2) { \
    InstrIR &inst = InstrIRPool::Acquire(); \
    inst.opcode_ = OpcodeIR::name; \
    ret r(&inst); \
    inst.return_ = r; \
    inst.SetArg(0, a1); \
    inst.SetArg(1, a2); \
    return r; \
}

#include "opcodes_ir.inl"

#undef INST0
#undef INST1
#undef INST2
