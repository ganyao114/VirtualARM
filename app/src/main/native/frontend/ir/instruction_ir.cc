//
// Created by 甘尧 on 2019-10-25.
//

#include "instruction_ir.h"

using namespace Instruction::IR;

InstrIR::InstrIR(OpcodeIR opcode, size_t useCount, const std::array<Argument, max_arg_count> &args) {

}

void InstrIR::SetArg(int pos, Argument &argument) {
    args_[pos] = argument;
}

InstrIR &InstrIRPool::Acquire() {
}

void InstrIRPool::Release(InstrIR &instr) {

}
