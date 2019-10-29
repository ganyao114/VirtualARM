//
// Created by 甘尧 on 2019-09-27.
//

#pragma once

#include <base/marcos.h>
#include "instruction.h"
#include "entry_point.h"

using namespace Instruction;

namespace Code {

    enum class EntryType {
        Interpreter,
        Jit
    };

    class CodeBlock {
    public:
        u32 id_{0};
        Location code_start_;
        Location code_end_;
        union {
            EntryPointFromJitToJit entry_jit_to_jit_;
            EntryPointFromInterpreterToInterpreter entry_iter_to_inter_;
        } entry_;
        EntryType type;
        /// Conditional to pass in order to execute this block
        Condition cond_;
        u32 cycle_count_ = 0;
        bool static_jump_ {false};
    };

}