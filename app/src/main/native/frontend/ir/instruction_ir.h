//
// Created by 甘尧 on 2019-10-25.
//

#pragma once

#include <base/marcos.h>
#include "opcode_ir.h"
#include "argument_ir.h"
#include <boost/intrusive/slist_hook.hpp>

namespace Instruction::IR {

    using namespace boost::intrusive;

    constexpr size_t max_arg_count = 3;

    class InstrIR : public slist_base_hook<> {
    public:

        InstrIR() = default;

        InstrIR(OpcodeIR opcode, size_t useCount, const std::array<Argument, max_arg_count> &args);

        void SetArg(int pos, Argument &argument);

        OpcodeIR opcode_;
        u32 use_count = 0;
        std::array<Argument, max_arg_count> args_;
        Return return_;
    };

    class InstrIRPool {
    public:
        static InstrIR &Acquire();
        static void Release(InstrIR &instr);
    };

}