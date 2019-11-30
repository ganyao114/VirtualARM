//
// Created by 甘尧 on 2019-10-28.
//

#include "assembler_ir.h"

using namespace Instruction::IR;


// IR Assembler
#define INST0(name, ret) ret& Assembler::name() { \
    return block_->Emit<ret>(static_cast<u8>(OpcodeIR::name), {}); \
}

#define INST1(name, ret, arg1) ret& Assembler::name(const arg1& a1) { \
    return block_->Emit<ret>(static_cast<u8>(OpcodeIR::name), {a1}); \
}

#define INST2(name, ret, arg1, arg2) ret& Assembler::name(const arg1& a1, const arg2& a2) { \
    return block_->Emit<ret>(static_cast<u8>(OpcodeIR::name), {a1, a2}); \
}

#include "opcodes_ir.inl"

#undef INST0
#undef INST1
#undef INST2
