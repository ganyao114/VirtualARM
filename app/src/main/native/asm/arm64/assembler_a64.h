//
// Created by 甘尧 on 2019-10-29.
//

#pragma once

#include "instruction_fields.h"
#include "includes/instruction.h"
#include "instruction_aarch64.h"

using namespace Instruction;

using Cond = A64::Condition;
using Offset = A64::Offset;

namespace Assembler::A64 {

    class AssemblerA64 {
    public:

        template <DataSize size, u8 flags = 0>
        void StoreRegImm();

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
