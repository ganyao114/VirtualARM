//
// Created by 甘尧 on 2019-10-28.
//

#pragma once


#include "argument_ir.h"

namespace Instruction {
    namespace IR {

        class Assembler {

            // IR Assembler
#define INST0(name, ret) ret& Emit##name();
#define INST1(name, ret, arg1) ret& Emit##name(arg1& a1);
#define INST2(name, ret, arg1, arg2) ret& Emit##name(arg1& a1, arg2& a2);
#include "opcodes_ir.inl"
#undef INST0
#undef INST1
#undef INST2

        };

    }
}
