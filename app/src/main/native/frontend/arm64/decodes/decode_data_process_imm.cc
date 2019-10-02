//
// Created by 甘尧 on 2019-09-27.
//

#include "frontend/arm64/instruction_decode.h"

using namespace Instruction::A64;

InstrA64Ref DecodePCRel(AArch64Inst& inst) {
    SharedPtr<InstrA64PCRelAddr> instr(new InstrA64PCRelAddr());
    if (inst.adr_page == 1) {
        instr->SetOpcode(OpcodeA64::ADRP);
    } else {
        instr->SetOpcode(OpcodeA64::ADR);
    }
    return instr;
}

InstrA64Ref DecodeAddSubImm(AArch64Inst& inst) {
    if (inst.shift > 1) {
        return nullptr;
    }
    SharedPtr<InstrA64AddSubImm> instr(new InstrA64AddSubImm());
    // b31 - is64
    // b30 - isSub
    // b29 - updatesFlags
    switch (inst.addsub_imm_op) {
        case 0:
            instr->SetOpcode(OpcodeA64::ADD_32_imm);
            break;
        case 1:
            instr->SetOpcode(OpcodeA64::ADDS_32_imm);
            break;
        case 2:
            instr->SetOpcode(OpcodeA64::SUB_32_imm);
            break;
        case 3:
            instr->SetOpcode(OpcodeA64::SUBS_32_imm);
            break;
        case 4:
            instr->SetOpcode(OpcodeA64::ADD_64_imm);
            break;
        case 5:
            instr->SetOpcode(OpcodeA64::ADDS_64_imm);
            break;
        case 6:
            instr->SetOpcode(OpcodeA64::SUB_64_imm);
            break;
        case 7:
            instr->SetOpcode(OpcodeA64::SUBS_64_imm);
            break;
        default:
            return nullptr;
    }
    return instr;
}

InstrA64Ref DecodeMovWide(AArch64Inst& inst) {
    bool is64 = inst.addsub_imm_update_64bit == 1;
    auto opc = inst.mov_imm_opc;
    auto hw = inst.mov_imm_hw;
    if (((!is64) && (hw > 1)) || (opc == 1)) {
        return nullptr;
    }
    SharedPtr<InstrA64MovWide> instr(new InstrA64MovWide());
    switch (opc) {
        case 0:
            // MOVN
            instr->SetOpcode(OpcodeA64::MOVN);
            break;
        case 2:
            // MOVZ
            instr->SetOpcode(OpcodeA64::MOVZ);
            break;
        case 3:
            // MOVK
            instr->SetOpcode(OpcodeA64::MOVK);
            break;
    }
    return instr;
}

InstrA64Ref DecodeLogicalImm(AArch64Inst& inst) {

}

InstrA64Ref FastBranchDecoder::DecodeDPImm(InstrA64 instr_bits) {
    AArch64Inst inst(instr_bits);
    switch (inst.dp_type) {
        case 0:
        case 1:
            // 00x - PC-rel. addressing
            return DecodePCRel(inst);
        case 2:
        case 3:
            // 01x - Add/subtract (immediate)
            return DecodeAddSubImm(inst);
        case 4:
            // 100 - Logical (immediate)
            return DecodeLogicalImm(inst);
        case 5:
            // 101 - Move wide (immediate)
            return DecodeMovWide(inst);
//        case 6: {
//            // 110 - Bitfield
//            DecodeBitfield(bits, inst);
//        }
//        case 7: {
//            // 111 - Extract
//            DecodeExtract(bits, inst);
//        }
        default:
            return nullptr;
    }
}


