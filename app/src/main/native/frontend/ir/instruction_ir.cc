//
// Created by 甘尧 on 2019-10-25.
//

#include "instruction_ir.h"

using namespace Instruction::IR;

InstrIR::InstrIR(OpcodeIR opcode, size_t useCount, const std::array<Argument, max_arg_count> &args) {

}

void InstrIR::SetArg(int pos, const Argument &argument) {
    args_[pos] = argument;
}


InstrIRPool &InstrIRPool::Get() {
    static InstrIRPool pool;
    return pool;
}

InstrIR &InstrIRPool::Acquire() {
    LockGuard lock(pool_lock_);

}

void InstrIRPool::Release(InstrIR &instr) {
    LockGuard lock(pool_lock_);

}
