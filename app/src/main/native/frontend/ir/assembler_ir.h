//
// Created by 甘尧 on 2019-10-28.
//

#pragma once


#include "argument_ir.h"
#include "block_ir.h"

using namespace Code::IR;

namespace Instruction::IR {

    class Assembler {
    public:
        // IR Assembler
#define INST0(name, ret) ret& name();
#define INST1(name, ret, arg1) ret& name(const arg1& a1);
#define INST2(name, ret, arg1, arg2) ret& name(const arg1& a1, const arg2& a2);

#include "opcodes_ir.inl"

#undef INST0
#undef INST1
#undef INST2
    protected:
        CodeBlock *block_;
    };

}
