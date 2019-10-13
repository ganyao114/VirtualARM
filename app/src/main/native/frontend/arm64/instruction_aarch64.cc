//
// Created by 甘尧 on 2019-09-24.
//

#include "instruction_aarch64.h"
#include "instruction_decode.h"

using namespace Instruction::A64;

InstructionA64::InstructionA64() : backup_({}) {
    SetOpcode(OpcodeA64::UN_DECODED);
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

bool InstructionA64::Disassemble(AArch64Inst &inst) {
    if (GetOpcode() == OpcodeA64::UN_DECODED) {
        SetOpcode(DefaultDecoder::DecodeOpCode(inst.raw, TypeOfA64()));
    } else {
        backup_ = inst;
    }
    return true;
}

bool InstructionA64::Assemble() {
    *pc_ = backup_;
    return true;
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
    InstructionA64::Disassemble(inst);
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
        case OpcodeA64::CBNZ_64: case OpcodeA64::CBZ_64: case OpcodeA64::CBZ_32: case OpcodeA64::CBNZ_32:
            DECODE_OFFSET(inst.bch_ucond_offset, 26, 2);
            rt_ = XREG(inst.Rt);
            break;
        case OpcodeA64::BLR:
            link_ = true;
        case OpcodeA64::BR:
            rn_ = XREG(inst.Rn);
            break;
        default:
            return false;
    }
    return true;
}

bool InstrA64Branch::Assemble() {
    *pc_ = InstructionTableA64::Get().GetInstrInfo(GetOpcode()).mask_pair.second;
    switch (GetOpcode()) {
        case OpcodeA64::B_cond:
            pc_->cond = cond_;
            pc_->bch_cond_offset = ENCODE_OFFSET(19, 2);
            break;
        case OpcodeA64::BL: case OpcodeA64::B:
            pc_->bch_ucond_offset = ENCODE_OFFSET(26, 2);
            break;
        case OpcodeA64::RET:
            break;
        case OpcodeA64::CBNZ_64: case OpcodeA64::CBZ_64: case OpcodeA64::CBZ_32: case OpcodeA64::CBNZ_32:
            pc_->Rt = rt_.Code();
            pc_->bch_cond_offset = ENCODE_OFFSET(19, 2);
            break;
        case OpcodeA64::BLR: case OpcodeA64::BR:
            pc_->Rn = rn_.Code();
            break;
        case OpcodeA64::TBZ: case OpcodeA64::TBNZ:
            break;
        default:
            return false;
    }
    return true;
}

GeneralRegister &InstrA64Branch::GetRt() {
    return rt_;
}

void InstrA64Branch::SetRt(GeneralRegister rt) {
    rt_ = rt;
}

GeneralRegister &InstrA64Branch::GetRn() {
    return rn_;
}

void InstrA64Branch::SetRn(GeneralRegister rn) {
    rn_ = rn;
}


// Exception Generating
InstrA64ExpGen::InstrA64ExpGen() {
    type_ = System;
}

bool InstrA64ExpGen::Disassemble(AArch64Inst &inst) {
    InstructionA64::Disassemble(inst);
    to_exception_level_ = ExceptionLevel(inst.exp_gen_ll);
    SetImm(static_cast<u16>(inst.imm16));
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
    pc_->imm16 = GetImm();
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

InstrA64System::InstrA64System() {
    type_ = System;
}

const SystemRegister &InstrA64System::GetSystemRegister() const {
    return system_register_;
}

void InstrA64System::SetSystemRegister(const SystemRegister &systemRegister) {
    system_register_ = systemRegister;
}

GeneralRegister &InstrA64System::GetRt() {
    return rt_;
}

void InstrA64System::SetRt(GeneralRegister rt) {
    rt_ = rt;
}

bool InstrA64System::Disassemble(AArch64Inst &inst) {
    InstructionA64::Disassemble(inst);
    switch (GetOpcode()) {
        case OpcodeA64::MRS: case OpcodeA64::MSR_imm: case OpcodeA64::MSR_reg:
            rt_ = XREG(inst.Rd);
            system_register_ = SystemRegister(static_cast<u16>(inst.system_register));
            break;
    }
    return true;
}

bool InstrA64System::Assemble() {
    *pc_ = InstructionTableA64::Get().GetInstrInfo(GetOpcode()).mask_pair.second;
    switch (GetOpcode()) {
        case OpcodeA64::MRS: case OpcodeA64::MSR_imm: case OpcodeA64::MSR_reg:
            pc_->Rd = rt_.Code();
            pc_->system_register = system_register_.Value();
            break;
    }
    return true;
}

// PC Relate Addressing
InstrA64PCRelAddr::InstrA64PCRelAddr() {
    type_ = NormalCalculations;
}

s32 InstrA64PCRelAddr::GetOffset() const {
    return offset_;
}

void InstrA64PCRelAddr::SetOffset(s32 offset) {
    offset_ = offset;
}

bool InstrA64PCRelAddr::PageAlign() const {
    return page_align_;
}

VAddr InstrA64PCRelAddr::GetTarget() {
    VAddr base = reinterpret_cast<VAddr>(GetPC());
    if (PageAlign()) {
        base = RoundDown(base, PAGE_SIZE);
    }
    return base + GetOffset();
}

bool InstrA64PCRelAddr::Disassemble(AArch64Inst &inst) {
    InstructionA64::Disassemble(inst);
    switch (GetOpcode()) {
        case OpcodeA64::ADRP:
            page_align_ = true;
        case OpcodeA64::ADR:
            u64 imm = (inst.immhi << 2) | inst.immlo;
            DECODE_OFFSET(imm, 21, 0);
            if (page_align_) {
                offset_ <<= A64_PAGE_SIZE;
            }
            break;
    }
    return true;
}

bool InstrA64PCRelAddr::Assemble() {
    return InstructionA64::Assemble();
}



//Add & Sub Immediate
InstrA64AddSubImm::InstrA64AddSubImm() : rd_() {}

bool InstrA64AddSubImm::Disassemble(AArch64Inst &inst) {
    InstructionA64::Disassemble(inst);
    is_sub_ = inst.addsub_imm_update_sub == 1;
    is_64bit = inst.addsub_imm_update_64bit == 1;
    update_flag_ = inst.addsub_imm_update_flag == 1;
    shift_ = inst.shift == 1;
    operand_.shift_extend_imm_ = static_cast<s32>(static_cast<u64>(inst.dp_imm)
            << ((shift_) ? PAGE_OFFSET : 0));
    if (is_64bit) {
        rd_ = XREG(inst.Rd);
        operand_.reg_ = XREG(inst.Rn);
    } else {
        rd_ = WREG(inst.Rd);
        operand_.reg_ = WREG(inst.Rn);
    }
    return true;
}

bool InstrA64AddSubImm::Assemble() {
    *pc_ = InstructionTableA64::Get().GetInstrInfo(GetOpcode()).mask_pair.second;
    pc_->addsub_imm_update_sub = is_sub_ ? 1 : 0;
    pc_->addsub_imm_update_64bit = is_64bit ? 1 : 0;
    pc_->addsub_imm_update_flag = update_flag_ ? 1 : 0;
    pc_->Rd = rd_.Code();
    pc_->Rn = operand_.reg_.Code();
    return true;
}

bool InstrA64AddSubImm::IsSub() const {
    return is_sub_;
}

bool InstrA64AddSubImm::IsUpdateFlag() const {
    return update_flag_;
}

bool InstrA64AddSubImm::Is64Bit() const {
    return is_64bit;
}

GeneralRegister &InstrA64AddSubImm::GetRd() {
    return rd_;
}

void InstrA64AddSubImm::SetRd(GeneralRegister rd) {
    rd_ = rd;
}

const Operand &InstrA64AddSubImm::GetOperand() const {
    return operand_;
}

void InstrA64AddSubImm::SetOperand(const Operand &operand) {
    operand_ = operand;
}


//Mov Wide
InstrA64MovWide::InstrA64MovWide() {}

GeneralRegister &InstrA64MovWide::GetRd() {
    return rd_;
}

void InstrA64MovWide::SetRd(GeneralRegister &rd) {
    rd_ = rd;
}

u16 InstrA64MovWide::GetImm() const {
    return imm_;
}

void InstrA64MovWide::SetImm(u16 imm) {
    imm_ = imm;
}

bool InstrA64MovWide::Disassemble(AArch64Inst &inst) {
    InstructionA64::Disassemble(inst);
    bool is64 = inst.addsub_imm_update_64bit == 1;
    if (is64) {
        rd_ = XREG(inst.Rd);
    } else {
        rd_ = WREG(inst.Rd);
    }
    imm_ = inst.imm16;
    shift_ = Shift(inst.hw * 16);
    return true;
}

bool InstrA64MovWide::Assemble() {
    *pc_ = InstructionTableA64::Get().GetInstrInfo(GetOpcode()).mask_pair.second;
    pc_->imm16 = imm_;
    pc_->hw = shift_ / 16;
    pc_->Rd = rd_.Code();
    pc_->sf = rd_.IsX() ? 1 : 0;
    return true;
}

InstrA64MovWide::Shift InstrA64MovWide::GetShift() const {
    return shift_;
}

void InstrA64MovWide::SetShift(InstrA64MovWide::Shift shift) {
    shift_ = shift;
}

u64 InstrA64MovWide::GetValue(u64 old_value) {
    u64 value = static_cast<u64>(imm_) << shift_;
    if (GetOpcode() == OpcodeA64::MOVN) {
        // MOVN
        value = ~value;
    } else if (GetOpcode() == OpcodeA64::MOVK) {
        // MOVK
        u64 mask = ~(0xFFFFllu << shift_);
        old_value &= mask;
        value = old_value | value;
    }
    if (rd_.IsW()) {
        value &= Bitmask<32, u64>();
    }
    return value;
}

//LogicalImm
InstrA64LogicalImm::InstrA64LogicalImm() {}

inline u64 GetLogicalImmediate(AArch64Inst& inst) {
    u64 size = (static_cast<u64>(inst.N) << 6) | ((~static_cast<u64>(inst.imms)) & Bitmask<6, u64>());
    u64 length = 0;
    for (; size > 1; length++, size >>= 1);
    // TODO assert for length?
    u64 levels = Bitmask(length);
    u64 S = inst.imms & levels;
    u64 R = inst.immr & levels;
    u64 esize = 1llu << length;
    u64 welem = Bitmask(S + 1);
    welem = (welem >> R) | ((welem << (esize - R)) & Bitmask(esize)); // ROR(welem, R)
    u64 ret = 0;
    for (u64 i = 0u; i < 64 / esize; i++) {
        ret |= welem;
        welem <<= esize;
    }
    return ret;
}

bool InstrA64LogicalImm::Disassemble(AArch64Inst &inst) {
    InstructionA64::Disassemble(inst);
    imm_ = GetLogicalImmediate(inst);
    bool is64 = inst.sf == 1;
    if (is64) {
        rd_ = XREG(inst.Rd);
    } else {
        rd_ = WREG(inst.Rd);
    }
    update_flags_ = GetOpcode() == OpcodeA64::ANDS_imm;
    return true;
}

bool InstrA64LogicalImm::Assemble() {
    return InstructionA64::Assemble();
}


//BitField
InstrA64BitField::InstrA64BitField() {}


//Extract
InstrA64Extract::InstrA64Extract() {

}
