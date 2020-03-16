//
// Created by 甘尧 on 2019-11-08.
//

#pragma once

#include <frontend/ir/instruction_ir.h>

using namespace Instructions::IR;

namespace Backend::IR {


    class InterpreterIR {
        void run(void *context, InstrIR &instr);
    };

}