//
// Created by 甘尧 on 2019-09-25.
//

#include "frontend/arm64/instruction_decode.h"

using namespace Instruction::A64;

InstrA64Ref FastBranchDecoder::DecodeSystemAndBranch(InstrA64 instr_bits) {
    constexpr static InstrA64 B_COND = 0b010;
    constexpr static InstrA64 BR_EXP = 0b110;
    AArch64Inst inst(instr_bits);
    auto op0 = inst.sys_bch_op0;
    auto op1 = inst.sys_bch_op1;
    InstrA64Ref ret;
    switch (op0) {
        case B_COND:
            if (inst.bch_cond_type == 1) {
                // 1xxx - Unallocated
                return nullptr;
            }
            // 0xxx - Conditional branch (immediate)
            if (TestBit<4>(instr_bits) || TestBit<24>(instr_bits)) {
                return nullptr;
            } else {
                SharedPtr<InstrA64Branch> instr(new InstrA64Branch());
                ret = instr;
                instr->SetOpcode(OpcodeA64::B_cond);
                instr->Disassemble(inst);
            }
            break;
        case BR_EXP:
            // 110
            if (op1 > 7) {
                // 1xxx - Unconditional branch (register)
                auto opc = inst.bch_ucond_opc;
                auto op2 = inst.bch_ucond_op2;
                auto op3 = inst.bch_ucond_op3;
                auto Rn = inst.Rn;
                auto op4 = inst.bch_ucond_op4;

                if ((op2 != 31) || (op3 != 0) || (op4 != 0)) {
                    return nullptr;
                }

                SharedPtr<InstrA64Branch> instr(new InstrA64Branch());
                ret = instr;
                switch (opc) {
                    case 0:
                        instr->SetOpcode(OpcodeA64::BR);
                        break;
                    case 1:
                        instr->SetOpcode(OpcodeA64::BLR);
                        break;
                    case 2:
                        instr->SetOpcode(OpcodeA64::RET);
                        break;
                    case 4:
                        if (Rn == 31) {
                            instr->SetOpcode(OpcodeA64::ERET);
                        } else {
                            return nullptr;
                        }
                        break;
                    case 5:
                        if (Rn == 31) {
                            instr->SetOpcode(OpcodeA64::DRPS);
                        } else {
                            return nullptr;
                        }
                        break;
                    default:
                        return nullptr;
                }
            } else if (op1 < 4) {
                // 00xx - Exception generation
                ret = SharedPtr<InstrA64ExpGen>(new InstrA64ExpGen());
            } else if (op1 == 4) {
                // 0100 - System
                ret = SharedPtr<InstrA64System>(new InstrA64System());
            } else {
                // 0101 & 011x - Unallocated
                return nullptr;
            }
            break;
        case 0:case 4:
        {
            // x00 - Unconditional branch (immediate)
            SharedPtr<InstrA64Branch> instr(new InstrA64Branch());
            ret = instr;
            auto link = TestBit<31>(instr_bits);
            if (link) {
                instr->SetOpcode(OpcodeA64::BL);
            } else {
                instr->SetOpcode(OpcodeA64::B);
            }
            break;
        }
        case 1:case 5:
        {
            // x01
            SharedPtr<InstrA64Branch> instr(new InstrA64Branch());
            ret = instr;
            if (op1 < 8) {
                // 0xxx - Compare & branch (immediate)
                bool is_64 = TestBit<31>(instr_bits);
                bool negate = TestBit<24>(instr_bits);
                if (is_64) {
                    if (negate) {
                        instr->SetOpcode(OpcodeA64::CBNZ_64);
                    } else {
                        instr->SetOpcode(OpcodeA64::CBZ_64);
                    }
                } else {
                    if (negate) {
                        instr->SetOpcode(OpcodeA64::CBNZ_32);
                    } else {
                        instr->SetOpcode(OpcodeA64::CBZ_32);
                    }
                }
            } else {
                // 1xxx - Test & branch (immediate)
                bool negate = TestBit<24>(instr_bits);
                if (negate) {
                    instr->SetOpcode(OpcodeA64::TBNZ);
                } else {
                    instr->SetOpcode(OpcodeA64::TBZ);
                }
            }
            break;
        }
        default:
            break;
    }
    return ret;
}
