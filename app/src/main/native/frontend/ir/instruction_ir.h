//
// Created by 甘尧 on 2019-10-25.
//

#pragma once

#include <base/marcos.h>
#include "opcode_ir.h"
#include "argument_ir.h"

namespace Instruction::IR {

    constexpr size_t max_arg_count = 4;

    class InstrIR {
    public:
        OpcodeIR opcode_;
        size_t use_count = 0;
        std::array<Argument, max_arg_count> args_;
    };

}