//
// Created by 甘尧 on 2019-11-05.
//

#pragma once

#include <frontend/ir/argument_ir.h>
#include "frontend/ir/assembler_ir.h"
#include "frontend/ir/argument_ir.h"

using namespace Instruction::IR;

namespace Instruction::A64 {

#define INST0(x, ...) x,
#define INST1(x, ...) x,
#define INST2(x, ...) x,
#define Type(x)
    enum class OpcodeIRA64 {
        A64Start = static_cast<u8>(OpcodeIR::NUM_INSTRUCTIONS) + 1,
#include "opcodes_ir_a64.inl"
    };
#undef INST0
#undef INST1
#undef INST2
#undef Type

    enum class RegisterType : u8 {
        X,
        W,
        V,
        D,
        Q
    };

    class A64XReg : public RegU64 {
    public:
        A64XReg(u8 code) {
            size_ = DataSize::U64;
            type_ = Argument::RegisterFronted;
            value_.fronted = {
                    .type = static_cast<u8>(RegisterType::X),
                    .code = code
            };
        }
    };

    class A64WReg : public RegU32 {
    public:
        A64WReg(u8 code) {
            size_ = DataSize::U32;
            type_ = Argument::RegisterFronted;
            value_.fronted = {
                    .type = static_cast<u8>(RegisterType::W),
                    .code = code
            };
        }
    };

    class A64VReg : public RegU128 {
    public:
        A64VReg(u8 code) {
            size_ = DataSize::U128;
            type_ = Argument::RegisterFronted;
            value_.fronted = {
                    .type = static_cast<u8>(RegisterType::V),
                    .code = code
            };
        }
    };

    class AssemblerIR {
    public:
        // IR Assembler
#define INST0(name, ret) ret& name();
#define INST1(name, ret, arg1) ret& name(const arg1& a1);
#define INST2(name, ret, arg1, arg2) ret& name(const arg1& a1, const arg2& a2);

#include "opcodes_ir_a64.inl"

#undef INST0
#undef INST1
#undef INST2

    private:
        CodeBlock *block_;
    };

}
