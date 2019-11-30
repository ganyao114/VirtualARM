//
// Created by 甘尧 on 2019-11-05.
//

#include "ir_for_a64.h"

using namespace Instruction::A64;

// IR Assembler
#define INST0(name, ret) ret& AssemblerIR::name() { \
    return block_->Emit<ret>(static_cast<u8>(OpcodeIRA64::name), {}); \
}

#define INST1(name, ret, arg1) ret& AssemblerIR::name(const arg1& a1) { \
    return block_->Emit<ret>(static_cast<u8>(OpcodeIRA64::name), {a1}); \
}

#define INST2(name, ret, arg1, arg2) ret& AssemblerIR::name(const arg1& a1, const arg2& a2) { \
    return block_->Emit<ret>(static_cast<u8>(OpcodeIRA64::name), {a1, a2}); \
}

#include "opcodes_ir_a64.inl"

#undef INST0
#undef INST1
#undef INST2