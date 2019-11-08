//
// Created by 甘尧 on 2019-11-08.
//

#include "interpreter_ir.h"

using namespace Backend::IR;

void InterpreterIR::run(void *context, InstrIR &instr) {
    switch (instr.opcode_) {
        case OpcodeIR::Nop:
            break;
        default:
            break;
    }
}
