//
// Created by 甘尧 on 2019-10-28.
//

#include "block_ir.h"

using namespace CodeCache::IR;

void CodeBlock::Emit(InstrIR &instr) {
    instrs_.push_back(instr);
}

void CodeBlock::Terminal(If &ter) {
    terminal_type_ = IF;
    if_ = ter;
}

void CodeBlock::Terminal(CheckBit &ter) {
    terminal_type_ = CHECK_BIT;
    check_bit_ = ter;
}
