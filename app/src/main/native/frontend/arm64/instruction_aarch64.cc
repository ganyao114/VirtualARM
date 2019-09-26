//
// Created by 甘尧 on 2019-09-24.
//

#include "instruction_aarch64.h"
#include "opcodes_table.inl"

using namespace Instruction::A64;

InstructionA64::InstructionA64() {
    SetOpcode(OpcodeA64::INVALID);
}

void InstructionA64::SetOpcode(OpcodeA64 opcode) {
    opcode_ = static_cast<u32>(opcode);
}

OpcodeA64 InstructionA64::GetOpcode() {
    return OpcodeA64(opcode_);
}

bool InstructionA64::Invalid() {
    return GetOpcode() == OpcodeA64::INVALID;
}


//Branch
InstrA64Branch::InstrA64Branch() {
    type_ = Branch;
}

Condition InstrA64Branch::GetCond() const {
    return cond_;
}

void InstrA64Branch::SetCond(Condition cond) {
    cond_ = cond;
}

bool InstrA64Branch::HasCond() const {
    return cond_ != AL;
}

VAddr InstrA64Branch::GetTarget() const {
    return target_;
}

void InstrA64Branch::SetTarget(VAddr target) {
    target_ = target;
}

bool InstrA64Branch::IsAbs() const {
    return is_abs_;
}

s32 InstrA64Branch::GetOffset() const {
    return offset_;
}

void InstrA64Branch::SetOffset(s32 offset) {
    is_abs_ = false;
    offset_ = offset;
}

bool InstrA64Branch::IsLink() const {
    return link_;
}

void InstrA64Branch::SetLink(bool link) {
    link_ = link;
}

bool InstrA64Branch::Disassemble(AArch64Inst &inst) {
    switch (GetOpcode()) {
        case OpcodeA64::B_cond:
            SetCond(Condition(inst.cond));
            DECODE_OFFSET(inst.bch_cond_offset, 19, 2);
            break;
        case OpcodeA64::BL:
            link_ = true;
        case OpcodeA64::B:
            DECODE_OFFSET(inst.bch_ucond_offset, 26, 2);
            break;
        case OpcodeA64::RET:
            break;
        case CBNZ_64:
        case CBZ_64:
        case CBZ_32:
        case CBNZ_32:
            DECODE_OFFSET(inst.bch_ucond_offset, 26, 2);
            rt_ = XREG(inst.Rt);
        default:
            return false;
    }
    return true;
}

bool InstrA64Branch::Assemble() {
    *pc_ = InstructionTableA64::Get().GetInstrInfo(GetOpcode()).mask_pair.second;
    switch (GetOpcode()) {
        bool is_64 = false;
        case OpcodeA64::B_cond:
            pc_->cond = cond_;
            pc_->bch_cond_offset = ENCODE_OFFSET(19, 2);
            break;
        case OpcodeA64::BL: case OpcodeA64::B:
            pc_->bch_ucond_offset = ENCODE_OFFSET(26, 2);
            break;
        case OpcodeA64::RET:
            break;
        case CBNZ_64:
        case CBZ_64:
            is_64 = true;
        case CBZ_32:
        case CBNZ_32:
            pc_->Rt = static_cast<u32>(rt_);
            pc_->bch_cond_offset = ENCODE_OFFSET(19, 2);
        default:
            return false;
    }
    return true;
}

XReg InstrA64Branch::GetRt() const {
    return rt_;
}

void InstrA64Branch::SetRt(XReg rt) {
    rt_ = rt;
}


// Exception Generating
InstrA64ExpGen::InstrA64ExpGen() {
    type_ = System;
}

bool InstrA64ExpGen::Disassemble(AArch64Inst &inst) {
    to_exception_level_ = ExceptionLevel(inst.exp_gen_ll);
    SetImm(static_cast<u16>(inst.exp_gen_num));
    switch (GetOpcode()) {
        case OpcodeA64::SVC:
            break;
        case OpcodeA64::HVC:
        case OpcodeA64::SMC:
            break;
        default:
            break;
    }
    return true;
}

bool InstrA64ExpGen::Assemble() {
    *pc_ = InstructionTableA64::Get().GetInstrInfo(GetOpcode()).mask_pair.second;
    pc_->exp_gen_num = GetImm();
    return true;
}

bool InstrA64ExpGen::Excutable(ExceptionLevel cur_lel) {
    return to_exception_level_ - cur_lel == 1;
}

u16 InstrA64ExpGen::GetImm() const {
    return imm_;
}

void InstrA64ExpGen::SetImm(u16 imm) {
    imm_ = imm;
}


//System

