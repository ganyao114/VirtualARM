//
// Created by 甘尧 on 2020-02-29.
//

#pragma once

#include "asm/arm64/instruction_table.h"

using namespace Instruction::A64;

namespace DBI::A64 {

    struct Visitor : public BaseVisitor {
        using instruction_return_type = void;
#define Type(...)
#define FIELD1(f1) u32 f1
#define FIELD2(f1, f2) u32 f1, u32 f2
#define FIELD3(f1, f2, f3) u32 f1, u32 f2, u32 f3
#define FIELD4(f1, f2, f3, f4) u32 f1, u32 f2, u32 f3, u32 f4
#define INST(code, name, fields, ...) \
    void code(fields);

#include "asm/arm64/instructions_table.inl"

#undef FIELD1
#undef FIELD2
#undef FIELD3
#undef FIELD4
#undef INST
#undef Type
    };

}
