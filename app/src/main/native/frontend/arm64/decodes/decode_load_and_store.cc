//
// Created by 甘尧 on 2019-10-15.
//

#include "frontend/arm64/instruction_decode.h"

using namespace Instruction::A64;

#define STORE_REG_IMM(x) instruction = SharedPtr<InstrA64StoreRegImm>(new InstrA64StoreRegImm()); \
instruction->SetOpcode(OpcodeA64::x);
#define LOAD_REG_IMM(x) instruction = SharedPtr<InstrA64LoadRegImm>(new InstrA64LoadRegImm()); \
instruction->SetOpcode(OpcodeA64::x);

#define CAST_STORE_REG_IMM DynamicObjectCast<InstrA64StoreRegImm>(instruction)
#define CAST_LOAD_REG_IMM DynamicObjectCast<InstrA64LoadRegImm>(instruction)
#define SET_FLAG_STR_REGIMM(x) CAST_STORE_REG_IMM->GetFlags().x = 1;


InstrA64Ref DecodeLoadStoreSIMDMultipleStructs(AArch64Inst &instr) {

}

InstrA64Ref DecodeLoadStoreExclusive(AArch64Inst &instr) {

}

InstrA64Ref DecodeLoadRegLiteral(AArch64Inst &instr) {

}

InstrA64Ref DecodeLoadStorePair(AArch64Inst &instr) {

}

InstrA64Ref DecodeLoadStoreRegImm(AArch64Inst &instr) {

    bool is_simd = instr.is_simd == 1;

    if ((instr.size != 0) && is_simd == 1 && (instr.ldrstr_opc > 1)) {
        return nullptr;
    }

    InstrA64Ref instruction;

    if (is_simd) {
        if (instr.size == InstrA64LoadAndStore::Size8) {
            switch (instr.ldrstr_opc) {
                case 0:
                    STORE_REG_IMM(STR_float)
                    SET_FLAG_STR_REGIMM(StoreFloat)
                    break;
                case 1:
                    LOAD_REG_IMM(LDR_float)
                    break;
                case 2:
                    STORE_REG_IMM(STR128)
                    SET_FLAG_STR_REGIMM(Store128BitFloat)
                    SET_FLAG_STR_REGIMM(StoreFloat)
                    break;
                case 3:
                    LOAD_REG_IMM(LDR128)
                    break;
            }
        } else {
            switch (instr.ldrstr_opc) {
                case 0:
                    STORE_REG_IMM(STR_float)
                    SET_FLAG_STR_REGIMM(StoreFloat)
                    break;
                case 1:
                    LOAD_REG_IMM(LDR_float)
                    break;
                default:
                    return nullptr;
            }
        }
    } else {
        switch (instr.size) {
            case InstrA64LoadAndStore::Size8:
                switch (instr.ldrstr_opc) {
                    case 0:
                        STORE_REG_IMM(STRB)
                        break;
                    case 1:
                        LOAD_REG_IMM(LDRB)
                        break;
                    case 2:
                        LOAD_REG_IMM(LDRSB)
                        break;
                    case 3:
                        LOAD_REG_IMM(LDRSB)
                        break;
                }
                break;
            case InstrA64LoadAndStore::Size16:
                switch (instr.ldrstr_opc) {
                    case 0:
                    STORE_REG_IMM(STRH)
                        break;
                    case 1:
                    LOAD_REG_IMM(LDRH)
                        break;
                    case 2:
                    LOAD_REG_IMM(LDRSH)
                        break;
                    case 3:
                    LOAD_REG_IMM(LDRSH)
                        break;
                }
                break;
            case InstrA64LoadAndStore::Size32:
                switch (instr.ldrstr_opc) {
                    case 0:
                        STORE_REG_IMM(STR)
                        break;
                    case 1:
                        LOAD_REG_IMM(LDR)
                        break;
                    case 2:
                        LOAD_REG_IMM(LDRSW)
                        break;
                    default:
                        return nullptr;
                }
                break;
            case InstrA64LoadAndStore::Size64:
                switch (instr.ldrstr_opc) {
                    case 0:
                        STORE_REG_IMM(STR)
                        break;
                    case 1:
                        LOAD_REG_IMM(LDR)
                        break;
                    default:
                        return nullptr;
                }
                break;
            default:
                return nullptr;
        }
    }

    return instruction;

}

InstrA64Ref DecodeLoadStoreRegOff(AArch64Inst &instr) {

}

InstrA64Ref DecodeLoadStoreUnprivileged(AArch64Inst &instr) {

}

InstrA64Ref DecodeLoadStoreRegUnscaled(AArch64Inst &instr) {

}

InstrA64Ref FastBranchDecoder::DecodeLoadAndStore(InstrA64 instr_bits) {
    AArch64Inst inst(instr_bits);
    InstrA64Ref instruction;
    switch (inst.ldrstr_op1) {
        case 0:
            // 00
            if (inst.ldrstr_op2 == 1) {
                if (inst.ldrstr_op0 == 1) {
                    // Unallocated
                    return nullptr;
                } else {
                    // op0 == 0
                    switch (inst.ldrstr_op3) {
                        case 0:
                            if (inst.ldrstr_op4 == 0) {
                                // 000000 - Advanced SIMD load/store multiple structures on page C4-203
                                instruction = DecodeLoadStoreSIMDMultipleStructs(inst);
                            } else {
                                // x1xxxx & xx1xxx & xxx1xx & xxxx1x & xxxxx1 - Unallocated
                                return nullptr;
                            }
                            break;
                        case 1:
                            if (inst.ldrstr_op4 < 32) {
                                // 0xxxxx - Advanced SIMD load/store multiple structures (post-indexed)
                                //decodeLoadStoreSIMDMultipleStructsPostIndex(bits, inst);
                            } else {
                                // 1xxxxx - Unallocated
                                return nullptr;
                            }
                            break;
                        case 2:
                            if ((inst.ldrstr_op4 == 0) || (inst.ldrstr_op4 == 32)) {
                                // x00000 - Advanced SIMD load/store single structure
                                //decodeLoadStoreSIMDSingleStruct(bits, inst);
                            } else {
                                // x1xxxx & xx1xxx & xxx1xx & xxxx1x & xxxxx1 - Unallocated
                                return nullptr;
                            }
                            break;
                        case 3:
                            // 11 - Advanced SIMD load/store single structure (post-indexed)
                            //decodeLoadStoreSIMDSingleStructPostIndex(bits, inst);
                            break;
                        default:
                            return nullptr;
                    }
                }
            } else {
                // op2 == 0
                if (inst.ldrstr_op3 < 2) {
                    // 0x - Load/store exclusive
                    instruction = DecodeLoadStoreExclusive(inst);
                } else {
                    // 1x - Unallocated
                    return nullptr;
                }
            }
            break;
        case 1:
            // 01
            if (inst.ldrstr_op3 < 2) {
                // 0x - Load register (literal)
                instruction = DecodeLoadRegLiteral(inst);
            } else {
                // 1x - Unallocated
                return nullptr;
            }
            break;
        case 2:
            // 10
            instruction = DecodeLoadStorePair(inst);
            break;
        case 3:
            // 11
            if (inst.ldrstr_op3 < 2) {
                // 0x
                if (inst.ldrstr_op4 < 32) {
                    // 0xxxxx
                    switch (inst.addr_mode) {
                        case Offset:
                            // 00 - Load/store register (unscaled immediate)
                            instruction = DecodeLoadStoreRegUnscaled(inst);
                            break;
                        case PostIndex:
                            // 01 - Load/store register (immediate post-indexed)
                        case PreIndex:
                            // 11 - Load/store register (immediate pre-indexed)
                            instruction = DecodeLoadStoreRegImm(inst);
                            break;
                        case NonAddrMode:
                            // 10 - Load/store register (unprivileged)
                            instruction = DecodeLoadStoreUnprivileged(inst);
                            break;
                        default:
                            return nullptr;
                    }
                } else {
                    // 1xxxxx
                    if (inst.addr_mode == NonAddrMode) {
                        // 10 - Load/store register (register offset)
                        instruction = DecodeLoadStoreRegOff(inst);
                    } else {
                        // != 10 - Unallocated
                        return nullptr;
                    }
                }
            } else {
                // 1x - Load/store register (unsigned immediate)
                instruction = DecodeLoadStoreRegImm(inst);
            }
            break;
        default:
            return nullptr;
    }
    return instruction;
}