//
// Created by 甘尧 on 2019-10-29.
//

#pragma once

#include "includes/instruction.h"

using namespace Instruction;

using Cond = Condition;
using Offset = s32;

namespace Assembler::A64 {

    class AssemblerA64 {
    public:

#define INST(x, name, regs, mask, args, ...) void x(args);
#define ARG1(arg1) arg1 &a1
#define ARG2(arg1, arg2) arg1 &a1, arg2 &a2
#define Type(x)

#include "instructions_table.inl";

#undef INST
#undef ARG1
#undef ARG2
#undef Type

    };
}
