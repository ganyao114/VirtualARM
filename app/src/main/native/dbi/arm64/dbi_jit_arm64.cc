//
// Created by 甘尧 on 2020-02-29.
//

#include "dbi_jit_arm64.h"

using namespace DBI::A64;

// Brunch

static constexpr s32 CondOffset(u32 imm) {
    return SignExtend<s32, 28>(imm << 2);
}

static constexpr s32 UnCondOffset(u32 imm) {
    return SignExtend<s32, 21>(imm << 2);
}

void Visitor::B_cond(u32 cond, u32 bch_cond_offset) {
    BrunchImm(CondOffset(bch_cond_offset), Cond(cond));
}

void Visitor::B(u32 bch_ucond_offset) {
    BrunchImm(UnCondOffset(bch_ucond_offset));
}

void Visitor::BL(u32 bch_ucond_offset) {
    BrunchImm<Link>(UnCondOffset(bch_ucond_offset));
}

void Visitor::BR(u32 Rn) {
    BrunchReg(Rn);
}

void Visitor::BLR(u32 Rn) {
    BrunchReg<Link>(Rn);
}

void Visitor::CBZ(u32 Rt, u32 bch_ucond_offset) {
    BrunchImm<Comp>(UnCondOffset(bch_ucond_offset));
}

void Visitor::CBNZ(u32 Rt, u32 bch_ucond_offset) {
    BrunchImm<Comp | Negate>(UnCondOffset(bch_ucond_offset));
}

void Visitor::TBZ(u32 Rt, u32 b5, u32 b40, u32 tbz_offset) {
    BrunchImm<TestBit | Negate>(UnCondOffset(tbz_offset));
}

void Visitor::TBNZ(u32 Rt, u32 b5, u32 b40, u32 tbz_offset) {
    BrunchImm<TestBit | Negate>(UnCondOffset(tbz_offset));
}

void Visitor::RET() {
    BrunchReg(static_cast<s32>(XReg::LR));
}

// System

void Visitor::BRK(u32 imm16) {
    Exception<Brk>(static_cast<u16>(imm16));
}

void Visitor::SVC(u32 imm16) {
    Exception<Svc>(static_cast<u16>(imm16));
}

// Mrs Msr

void Visitor::MRS(u32 system_register, u32 Rd) {
    u16 reg_code = static_cast<u16>(system_register);
    if (SysRegs::TPIDR_EL0 == reg_code) {
        context_->ReadTPIDR(static_cast<u8>(Rd));
    } else if(SysRegs::TPIDRRO_EL0 == reg_code) {
        context_->ReadTPIDRRO(static_cast<u8>(reg_code));
    } else {
        context_->Emit(inst->raw);
    }
}

void Visitor::MSR_reg(u32 system_register, u32 Rd) {

}

// PC-rel. addressing

void Visitor::ADR(u32 Rd, u32 immhi, u32 immlo) {
    Addressing<false>(static_cast<u8>(Rd), immhi, immlo);
}

void Visitor::ADRP(u32 Rd, u32 immhi, u32 immlo) {
    Addressing<true>(static_cast<u8>(Rd), immhi, immlo);
}

// Load Store Literal

void Visitor::LDR_lit(u32 Rt, u32 lit_offset) {
    switch (inst->size) {
        case 0:
            LoadRegLiteral<u32>(static_cast<u8>(Rt), lit_offset);
            break;
        case 1:
            LoadRegLiteral<u64>(static_cast<u8>(Rt), lit_offset);
            break;
        default:
            // error
            break;
    }
}

void Visitor::LDRSW_lit(u32 Rt, u32 lit_offset) {
    LoadRegLiteral<u32, LoadExtendResult>(static_cast<u8>(Rt), lit_offset);
}

void Visitor::LDR_lit_fp(u32 Rt, u32 lit_offset) {
    switch (inst->size) {
        case 0:
            LoadRegLiteral<u32, LoadFloat>(static_cast<u8>(Rt), lit_offset);
            break;
        case 1:
            LoadRegLiteral<u64, LoadFloat>(static_cast<u8>(Rt), lit_offset);
            break;
        case 2:
            LoadRegLiteral<u128, LoadFloat | Load128BitFloat>(static_cast<u8>(Rt), lit_offset);
            break;
        default:
            // error
            break;
    }
}

void Visitor::PRFM_lit(u32 Rt, u32 lit_offset) {
    context_->Emit(inst->raw);
}
