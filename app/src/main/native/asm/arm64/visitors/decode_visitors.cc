//
// Created by 甘尧 on 2020-03-03.
//

#include "decode_visitors.h"

using namespace Instruction::A64;

template<unsigned int flags>
void DecodeVisitor::BrunchImm(s32 offset, Condition condition) {

}

template<unsigned int flags>
void DecodeVisitor::BrunchReg(u8 reg_target, Condition condition) {

}

static constexpr s32 CondOffset(u32 imm) {
    return SignExtend<s32, 28>(imm << 2);
}

static constexpr s32 UnCondOffset(u32 imm) {
    return SignExtend<s32, 21>(imm << 2);
}

void DecodeVisitor::B_cond(u32 cond, u32 bch_cond_offset) {
    BrunchImm(CondOffset(bch_cond_offset), Condition(cond));
}

void DecodeVisitor::B(u32 bch_ucond_offset) {
    BrunchImm(UnCondOffset(bch_ucond_offset));
}

void DecodeVisitor::BL(u32 bch_ucond_offset) {
    BrunchImm<Link>(UnCondOffset(bch_ucond_offset));
}

void DecodeVisitor::BR(u32 Rn) {
    BrunchReg(Rn);
}

void DecodeVisitor::BLR(u32 Rn) {
    BrunchReg<Link>(Rn);
}

void DecodeVisitor::CBZ(u32 Rt, u32 bch_ucond_offset) {
    BrunchImm<Comp>(UnCondOffset(bch_ucond_offset));
}

void DecodeVisitor::CBNZ(u32 Rt, u32 bch_ucond_offset) {
    BrunchImm<Comp | Negate>(UnCondOffset(bch_ucond_offset));
}

void DecodeVisitor::TBZ(u32 Rt, u32 b5, u32 b40, u32 tbz_offset) {
    BrunchImm<TestBit | Negate>(UnCondOffset(tbz_offset));
}

void DecodeVisitor::TBNZ(u32 Rt, u32 b5, u32 b40, u32 tbz_offset) {
    BrunchImm<TestBit | Negate>(UnCondOffset(tbz_offset));
}

void DecodeVisitor::RET() {
    BrunchImm<Ret>();
}