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
    ENCODE_OPCODE;
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

A64Register &InstrA64Branch::GetRt() {
    return rt_;
}

void InstrA64Branch::SetRt(A64Register rt) {
    rt_ = rt;
}

A64Register &InstrA64Branch::GetRn() {
    return rn_;
}

void InstrA64Branch::SetRn(A64Register rn) {
    rn_ = rn;
}


// Exception Generating
InstrA64ExpGen::InstrA64ExpGen() {
    type_ = InstrType::System;
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
    ENCODE_OPCODE;
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
    type_ = InstrType::System;
}

const SystemRegister &InstrA64System::GetSystemRegister() const {
    return system_register_;
}

void InstrA64System::SetSystemRegister(const SystemRegister &systemRegister) {
    system_register_ = systemRegister;
}

A64Register &InstrA64System::GetRt() {
    return rt_;
}

void InstrA64System::SetRt(A64Register rt) {
    rt_ = rt;
}

bool InstrA64System::Disassemble(AArch64Inst &inst) {
    InstructionA64::Disassemble(inst);
    switch (GetOpcode()) {
        case OpcodeA64::MRS: case OpcodeA64::MSR_imm: case OpcodeA64::MSR_reg:
            rt_ = XREG(inst.Rd);
            system_register_ = SystemRegister(static_cast<u16>(inst.system_register));
            break;
        default:
            break;
    }
    return true;
}

bool InstrA64System::Assemble() {
    ENCODE_OPCODE;
    switch (GetOpcode()) {
        case OpcodeA64::MRS: case OpcodeA64::MSR_imm: case OpcodeA64::MSR_reg:
            pc_->Rd = rt_.Code();
            pc_->system_register = system_register_.Value();
            break;
        default:
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
    ENCODE_OPCODE;
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

A64Register &InstrA64AddSubImm::GetRd() {
    return rd_;
}

void InstrA64AddSubImm::SetRd(A64Register rd) {
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

A64Register &InstrA64MovWide::GetRd() {
    return rd_;
}

void InstrA64MovWide::SetRd(A64Register &rd) {
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
    imm_ = static_cast<u16>(inst.imm16);
    shift_ = Shift(inst.hw * 16);
    return true;
}

bool InstrA64MovWide::Assemble() {
    ENCODE_OPCODE;
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
    u64 esize = UINT64_C(1) << length;
    u64 welem = Bitmask(S + 1);
    welem = (welem >> R) | ((welem << (esize - R)) & Bitmask(esize)); // ROR(welem, R)
    u64 ret = 0;
    for (u64 i = 0u; i < 64 / esize; i++) {
        ret |= welem;
        welem <<= esize;
    }
    return ret;
}

static bool IsImmLogical(u64 value,
                             unsigned width,
                             unsigned* n,
                             unsigned* imm_s,
                             unsigned* imm_r) {
    assert((width == kWRegSize) || (width == kXRegSize));

    bool negate = false;
    
    if (value & 1) {
        negate = true;
        value = ~value;
    }

    if (width == kWRegSize) {
        value <<= kWRegSize;
        value |= value >> kWRegSize;
    }
    
    u64 a = LowestSetBit(value);
    u64 value_plus_a = value + a;
    u64 b = LowestSetBit(value_plus_a);
    u64 value_plus_a_minus_b = value_plus_a - b;
    u64 c = LowestSetBit(value_plus_a_minus_b);

    int d, clz_a, out_n;
    u64 mask;

    if (c != 0) {
        clz_a = CountLeadingZeros(a, kXRegSize);
        int clz_c = CountLeadingZeros(c, kXRegSize);
        d = clz_a - clz_c;
        mask = ((UINT64_C(1) << d) - 1);
        out_n = 0;
    } else {
        if (a == 0) {
            return false;
        } else {
            clz_a = CountLeadingZeros(a, kXRegSize);
            d = 64;
            mask = ~UINT64_C(0);
            out_n = 1;
        }
    }

    if (!IsPowerOf2(d)) {
        return false;
    }

    if (((b - a) & ~mask) != 0) {
        return false;
    }

    static const u64 multipliers[] = {
            0x0000000000000001UL,
            0x0000000100000001UL,
            0x0001000100010001UL,
            0x0101010101010101UL,
            0x1111111111111111UL,
            0x5555555555555555UL,
    };
    u64 multiplier = multipliers[CountLeadingZeros(d, kXRegSize) - 57];
    u64 candidate = (b - a) * multiplier;

    if (value != candidate) {
        return false;
    }
    
    int clz_b = (b == 0) ? -1 : CountLeadingZeros(b, kXRegSize);
    int s = clz_a - clz_b;
    
    int r;
    if (negate) {
        s = d - s;
        r = (clz_b + 1) & (d - 1);
    } else {
        r = (clz_a + 1) & (d - 1);
    }
    
    if ((n != nullptr) || (imm_s != nullptr) || (imm_r != nullptr)) {
        *n = out_n;
        *imm_s = ((2 * -d) | (s - 1)) & 0x3f;
        *imm_r = r;
    }

    return true;
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
    unsigned n, imm_s, imm_r;
    pc_->raw = InstructionTableA64::Get().GetInstrInfo(GetOpcode()).mask_pair.second;
    if (IsImmLogical(imm_, rd_.IsX() ? kXRegSize : kWRegSize, &n, &imm_s, &imm_r)) {
        pc_->N = n;
        pc_->imms = imm_s;
        pc_->immr = imm_r;
        pc_->Rd = rd_.Code();
    } else {
        return false;
    }
}

const A64Register &InstrA64LogicalImm::GetRd() const {
    return rd_;
}

void InstrA64LogicalImm::SetRd(const A64Register &rd) {
    rd_ = rd;
}

u64 InstrA64LogicalImm::GetImm() const {
    return imm_;
}

void InstrA64LogicalImm::SetImm(u64 imm) {
    imm_ = imm;
}

bool InstrA64LogicalImm::IsUpdateFlags() const {
    return update_flags_;
}


//BitField
InstrA64BitField::InstrA64BitField() {

}

const A64Register &InstrA64BitField::GetRd() const {
    return rd_;
}

void InstrA64BitField::SetRd(const A64Register &rd) {
    rd_ = rd;
}

const A64Register &InstrA64BitField::GetRn() const {
    return rn_;
}

void InstrA64BitField::SetRn(const A64Register &rn) {
    rn_ = rn;
}

int InstrA64BitField::GetS() const {
    return S;
}

void InstrA64BitField::SetS(int s) {
    S = s;
}

int InstrA64BitField::GetR() const {
    return R;
}

void InstrA64BitField::SetR(int r) {
    R = r;
}

bool InstrA64BitField::Disassemble(AArch64Inst &inst) {
    InstructionA64::Disassemble(inst);
    bool is64 = inst.sf == 1;
    unsigned reg_size = is64 ? kXRegSize : kWRegSize;
    int64_t reg_mask = is64 ? kXRegMask : kWRegMask;
    R = inst.immr;
    S = inst.imms;
    int diff = S - R;
    u64 mask;
    if (diff >= 0) {
        mask = ~UINT64_C(0) >> (64 - (diff + 1));
        mask = (static_cast<unsigned>(diff) < (reg_size - 1)) ? mask : reg_mask;
    } else {
        mask = ~UINT64_C(0) >> (64 - (S + 1));
        mask = RotateRight(mask, R, reg_size);
        diff += reg_size;
    }
    mask_ = mask;
    if (GetOpcode() == OpcodeA64::SBFM) {
        inzero_ = true;
        extend_ = true;
    } else if (GetOpcode() == OpcodeA64::UBFM) {
        inzero_ = true;
    }
    top_bits_ = (diff == 63) ? 0 : (~UINT64_C(0) << (diff + 1));
    if (is64) {
        rd_ = XREG(inst.Rd);
        rn_ = XREG(inst.Rn);
    } else {
        rd_ = WREG(inst.Rd);
        rn_ = WREG(inst.Rn);
    }
}

bool InstrA64BitField::Assemble() {
    ENCODE_OPCODE;
    pc_->immr = static_cast<u32>(R);
    pc_->imms = static_cast<u32>(S);
    pc_->sf = rd_.IsX() ? 1 : 0;
    pc_->Rd = rd_.Code();
    pc_->Rn = rn_.Code();
    return true;
}

u64 InstrA64BitField::GetResult(u64 src, u64 dest) {
    if (inzero_) {
        dest = 0;
    }
    // Rotate source bitfield into place.
    u64 result = RotateRight(src, R, rd_.IsX() ? kXRegSize : kWRegSize);
    // Determine the sign extension.
    u64 signbits = extend_ && ((src >> S) & 1) ? top_bits_ : 0;

    // Merge sign extension, dest/zero and bitfield.
    result = signbits | (result & mask_) | (dest & ~mask_);
    return result;
}


//Extract
InstrA64Extract::InstrA64Extract() {

}

bool InstrA64LoadAndStore::Disassemble(AArch64Inst &inst) {
    size_ = Size(inst.size);
    is_simd_ = inst.is_simd == 1;
    return true;
}

bool InstrA64StoreRegImm::Disassemble(AArch64Inst &inst) {
    InstrA64LoadAndStore::Disassemble(inst);
    operand_.addr_mode_ = AddressMode(inst.addr_mode);
    if (operand_.addr_mode_ == PostIndex) {
        flags_.StoreWriteBack = 1;
        flags_.StorePostIndex = 1;
        flags_.StoreImmSigned = 1;
    } else if (operand_.addr_mode_ == PreIndex) {
        flags_.StoreWriteBack = 1;
        flags_.StoreImmSigned = 1;
    } else if (operand_.addr_mode_ == Offset){
        // 1x - Load/store register (unsigned immediate)
        if (inst.ldrstr_op3 < 2) {
            return false;
        }
    } else {
        return false;
    }
    operand_.base_ = XREG(inst.Rn);
    u64 num_bits = flags_.Store128BitFloat == 1 ? 4 : ConstLog2(UINT64_C(2) << size_);
    u64 imm;
    if (flags_.StoreImmSigned == 1) {
        imm = inst.imm9;
        operand_.offset_ = SignExtend<s32, 9>(imm);
    } else if (flags_.StoreExclusive == 0 && flags_.StoreRelease == 0) {
        imm = inst.dp_imm;
        operand_.offset_ = static_cast<s32>(imm << num_bits);
    }
    return true;
}

bool InstrA64StoreRegImm::Assemble() {
    return true;
}

bool InstrA64StoreRegImm::ShouldUpdateRn() {
    return flags_.StoreWriteBack == 1;
}

InstrA64LoadAndStore::StoreFlags &InstrA64StoreRegImm::GetFlags() {
    return flags_;
}

bool InstrA64LoadRegImm::Disassemble(AArch64Inst &inst) {
    InstrA64LoadAndStore::Disassemble(inst);
    operand_.addr_mode_ = AddressMode(inst.addr_mode);
    if (operand_.addr_mode_ == PostIndex) {
        flags_.LoadWriteBack = 1;
        flags_.LoadPostIndex = 1;
        flags_.LoadImmSigned = 1;
    } else if (operand_.addr_mode_ == PreIndex) {
        flags_.LoadWriteBack = 1;
        flags_.LoadImmSigned = 1;
    } else if (operand_.addr_mode_ == Offset){
        // 1x - Load/store register (unsigned immediate)
        if (inst.ldrstr_op3 < 2) {
            return false;
        }
    } else {
        return false;
    }
    operand_.base_ = XREG(inst.Rn);
    u64 num_bits = flags_.Load128BitFloat == 1 ? 4 : ConstLog2(UINT64_C(2) << size_);
    u64 imm;
    if (flags_.LoadImmSigned == 1) {
        imm = inst.imm9;
        operand_.offset_ = SignExtend<s32, 9>(imm);
    } else if (flags_.LoadExclusive == 0 && flags_.LoadAcquire == 0) {
        imm = inst.dp_imm;
        operand_.offset_ = static_cast<s32>(imm << num_bits);
    }
    return true;
}

bool InstrA64LoadRegImm::Assemble() {
    ENCODE_OPCODE;
    return true;
}


// LDR LIT
InstrA64LoadLiteral::InstrA64LoadLiteral() {}

bool InstrA64LoadLiteral::Disassemble(AArch64Inst &inst) {
    offset_ = DECODE_OFFSET(inst.immhi, 19, 2);
    switch (GetOpcode()) {
        case OpcodeA64::LDR_lit_gen:
        case OpcodeA64::PRFM_lit:
            rt_ = XREG(inst.Rt);
            break;
        case OpcodeA64::LDRSW_lit:
            rt_ = WREG(inst.Rt);
            break;
        case OpcodeA64::LDR_lit_fpsimd:
            rt_ = VREG(inst.Rt);
            break;
    }
    return true;
}

bool InstrA64LoadLiteral::Assemble() {
    ENCODE_OPCODE;
    pc_->immhi = ENCODE_OFFSET(19, 2);
    pc_->Rt = rt_.Code();
    return true;
}

const A64Register &InstrA64LoadLiteral::GetRt() const {
    return rt_;
}

void InstrA64LoadLiteral::SetRt(const A64Register &rt) {
    rt_ = rt;
}

s32 InstrA64LoadLiteral::GetOffset() const {
    return offset_;
}

void InstrA64LoadLiteral::SetOffset(s32 offset) {
    offset_ = offset;
}


//Store Pair
InstrA64StoreRegPair::InstrA64StoreRegPair() {}

bool InstrA64StoreRegPair::Disassemble(AArch64Inst &inst) {

    InstrA64LoadAndStore::Disassemble(inst);

    if (!is_simd_) {
        if ((inst.L == 0 && TestBit<0>(size_)) || size_ == 0b11) {
            return false;
        }
    } else {
        flags_.StoreFloat = 1;
        if (size_ == 0b10) {
            flags_.Store128BitFloat = 1;
        } else if (size_ == 0b11) {
            return false;
        }
    }

    if (inst.wback && (inst.Rt == inst.Rn || inst.Rt2 == inst.Rn) && inst.Rn != 31) {
        return false;
    }

    if (inst.wback) {
        flags_.StoreWriteBack = 1;
    }

    if (!is_simd_) {
        const bool signed_ = TestBit<0>(size_);
        if (signed_) {
            flags_.StoreImmSigned = 1;
        }
    }

    operand_.addr_mode_ = AddressMode(inst.addr_mode);

    operand_.base_ = XREG(inst.Rn);

    if (operand_.addr_mode_ == PostIndex) {
        flags_.StorePostIndex = 1;
    }

    const auto scale = 2 + (flags_.StoreFloat ? size_ : Bit<1>((u32)size_));
    const auto data_size = 8 << scale;
    const auto dbytes = data_size / 8;
    operand_.offset_ = SignExtend<s32, 7>(inst.imm7) << scale;

    if (is_simd_) {
        rt1_ = A64Register::V(data_size, static_cast<u8>(inst.Rt));
        rt2_ = A64Register::V(data_size, static_cast<u8>(inst.Rt2));
    } else {
        rt1_ = A64Register::X(data_size, static_cast<u8>(inst.Rt));
        rt2_ = A64Register::X(data_size, static_cast<u8>(inst.Rt2));
    }

    return true;
}

bool InstrA64StoreRegPair::Assemble() {
    return true;
}


//
InstrA64LoadRegPair::InstrA64LoadRegPair() {

}

bool InstrA64LoadRegPair::Disassemble(AArch64Inst &inst) {
    InstrA64LoadAndStore::Disassemble(inst);
    return true;
}

bool InstrA64LoadRegPair::Assemble() {
    return InstructionA64::Assemble();
}
