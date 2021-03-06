//
// Created by 甘尧 on 2019-10-25.
//

#pragma once

#include <base/marcos.h>

namespace Instructions::IR {
#define INST0(x, ...) x,
#define INST1(x, ...) x,
#define INST2(x, ...) x,
#define Type(x)
    enum class OpcodeIR : u8 {
        UN_DECODED,
        INVALID,

#include "opcodes_ir.inl"

        NUM_INSTRUCTIONS,
    };
#undef INST0
#undef INST1
#undef INST2
#undef Type
}