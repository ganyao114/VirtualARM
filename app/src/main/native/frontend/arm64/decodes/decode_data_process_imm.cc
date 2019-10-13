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
    switch (inst.opc) {
        case 0:
            instr->SetOpcode(OpcodeA64::ADD_imm);
            break;
        case 1:
            instr->SetOpcode(OpcodeA64::ADDS_imm);
            break;
        case 2:
            instr->SetOpcode(OpcodeA64::SUB_imm);
            break;
        case 3:
            instr->SetOpcode(OpcodeA64::SUBS_imm);
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
        default:
            return nullptr;
    }
    return instr;
}

InstrA64Ref DecodeLogicalImm(AArch64Inst& inst) {

    if (inst.sf != 1 && (inst.N == 1)) {
        return nullptr;
    }

    SharedPtr<InstrA64LogicalImm> instr(new InstrA64LogicalImm());

    switch (inst.opc) {
        case 0:
            // AND (immediate)
            instr->SetOpcode(OpcodeA64::AND_imm);
            break;
        case 1:
            // ORR (immediate)
            instr->SetOpcode(OpcodeA64::ORR_imm);
            break;
        case 2:
            // EOR (immediate)
            instr->SetOpcode(OpcodeA64::EOR_imm);
            break;
        case 3:
            // ANDS (immediate)
            instr->SetOpcode(OpcodeA64::ANDS_imm);
            break;
        default:
            return nullptr;
    }

    return instr;
}

InstrA64Ref DecodeBitField(AArch64Inst& inst) {
    if ((inst.sf != inst.N) || (inst.opc == 3)) {
        return nullptr;
    }
    if ((inst.sf == 0) && ((inst.immr > 31) || (inst.imms > 31))) {
        return nullptr;
    }

    SharedPtr<InstrA64BitField> instr(new InstrA64BitField());

    switch (inst.opc) {
        case 0:
            // SBFM
            instr->SetOpcode(OpcodeA64::SBFM);
            break;
        case 1:
            // BFM
            instr->SetOpcode(OpcodeA64::BFM);
            break;
        case 2:
            // UBFM
            instr->SetOpcode(OpcodeA64::UBFM);
            break;
        default:
            return nullptr;
    }

    return instr;
}

InstrA64Ref DecodeExtract(AArch64Inst& inst) {
    auto op21 = BitRange<29, 30>(inst.raw);
    if ((inst.sf == 1) && (op21 == 0) && (inst.o0 == 0)) {
        SharedPtr<InstrA64Extract> instr(new InstrA64Extract());
        instr->SetOpcode(OpcodeA64::EXTR);
        return instr;
    } else {
        return nullptr;
    }
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
        case 6:
            // 110 - Bitfield
            return DecodeBitField(inst);
        case 7:
            // 111 - Extract
            return DecodeExtract(inst);
        default:
            return nullptr;
    }
}


