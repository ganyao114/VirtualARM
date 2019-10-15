//
// Created by 甘尧 on 2019-10-15.
//

#include "frontend/arm64/instruction_decode.h"

using namespace Instruction::A64;

InstrA64Ref FastBranchDecoder::DecodeLoadAndStore(InstrA64 instr_bits) {
    AArch64Inst inst(instr_bits);

}