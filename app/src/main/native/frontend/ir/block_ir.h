//
// Created by 甘尧 on 2019-10-28.
//

#pragma once

#include <externals/ext-boost/boost/intrusive/slist.hpp>
#include "includes/instruction.h"
#include "instruction_ir.h"

using namespace Instruction;
using namespace boost::intrusive;
using namespace Instruction::IR;

namespace Code::IR {


    struct Terminal {
        u32 block_id_;
    };

    struct If {
        If() = default;
        If(Condition if_, Terminal then_, Terminal else_) : if_(if_), then_(std::move(then_)),
                                                            else_(std::move(else_)) {}
        Condition if_;
        Terminal then_;
        Terminal else_;
    };

    struct CheckBit {
        CheckBit() = default;
        CheckBit(Terminal then_, Terminal else_) : then_(std::move(then_)),
                                                   else_(std::move(else_)) {}
        Terminal then_;
        Terminal else_;
    };

    class CodeBlock {
    public:

        enum TerminalType : u8 {
            IF,
            CHECK_BIT
        };

        void Emit(InstrIR &instr);
        void Terminal(If &ter);
        void Terminal(CheckBit &ter);
    private:
        slist<InstrIR, cache_last<true>> instrs_;
        TerminalType terminal_type_;
        union {
            If if_;
            CheckBit check_bit_;
        };
        u32 exe_count_ {0};
    };

}