//
// Created by 甘尧 on 2019-09-25.
//

#include "instruction_decode.h"

using namespace Instruction::A64;

OpcodeA64 DefaultDecoder::DecodeOpCode(InstrA64 instr_bits, InstrTypeA64 type) {
    const auto& instr_map = type == Invalid ? InstructionTableA64::Get().GetInstrTable() : InstructionTableA64::Get().GetInstrTable(type);
    for (const auto item:instr_map) {
        if (item.second.Test(instr_bits)) {
            return item.first;
        }
    }
    return OpcodeA64::INVALID;
}

InstrA64Ref DefaultDecoder::Decode(InstrA64 *instr_bits) {
    InstrA64Ref instruction(new InstructionA64());
    InstructionTableA64::Get().Type(DecodeOpCode(*instr_bits));
    return instruction;
}

InstrA64Ref FastBranchDecoder::Decode(InstrA64 *instr_bits) {
    AArch64Inst *inst = ForceCast<AArch64Inst*>(instr_bits);
    InstrA64Ref instruction;
    switch (inst->instr_type_1) {
        case 0: case 1: case 2: case 3:
            break;
        case 8: case 9:
            // 100x - Data processing - immediate
            instruction = DecodeDPImm(*instr_bits);
            break;
        case 5: case 13:
            // x101 - Data processing - register
            instruction = DecodeDPReg(*instr_bits);
        case 10:case 11:
            // 101x - Branches, exception generating and system instructions
            instruction = DecodeSystemAndBranch(*instr_bits);
            break;
    }
    if (instruction) {
        if (!instruction->Disassemble(*inst) || instruction->Invalid()) {
            return nullptr;
        }
        instruction->SetPC(inst);
    }
    return instruction;
}

InstrA64Ref FastBranchDecoder::DecodeDPReg(InstrA64 instr_bits) {
    return nullptr;
}

InstrA64Ref FastBranchDecoder::DecodeSMID(InstrA64 instr_bits) {
    return nullptr;
}

InstrA64Ref FastBranchDecoder::DecodeFloat(InstrA64 instr_bits) {
    return nullptr;
}
