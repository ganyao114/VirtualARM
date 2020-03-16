//
// Created by 甘尧 on 2020-03-03.
//

#pragma once

#include "asm/arm64/instruction_table.h"

namespace Instructions::A64 {

    struct DecodeVisitor : public BaseVisitor {
        using instruction_return_type = void;

        enum BrunchFlags {
            Link    = 1 << 0,
            TestBit = 1 << 1,
            Comp    = 1 << 2,
            Negate  = 1 << 3,
        };

#define Type(...)
#define FIELD1(f1) u32 f1
#define FIELD2(f1, f2) u32 f1, u32 f2
#define FIELD3(f1, f2, f3) u32 f1, u32 f2, u32 f3
#define FIELD4(f1, f2, f3, f4) u32 f1, u32 f2, u32 f3, u32 f4
#define INST(code, name, fields, ...) \
    void code(fields);

#include "asm/arm64/instructions_table_all.inl"

#undef FIELD1
#undef FIELD2
#undef FIELD3
#undef FIELD4
#undef INST
#undef Type

        template <unsigned flags = 0>
        void BrunchImm(s32 offset = 0, Cond condition = NO_COND) {}

        template <unsigned flags = 0>
        void BrunchReg(u8 reg_target, Cond condition = NO_COND) {}
    };

}
