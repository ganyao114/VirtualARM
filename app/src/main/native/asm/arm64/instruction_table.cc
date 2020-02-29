//
// Created by 甘尧 on 2019-09-24.
//

#include "instruction_table.h"

using namespace Instruction;
using namespace Instruction::A64;

bool InstrA64Info::Test(InstrA64 bits) const {
    return (bits & mask_pair_.first) == mask_pair_.second;
}

InstrA64Info::InstrA64Info() {}

MaskValuePair A64::ParseMaskValuePair(std::string mask_str) {
    InstrA64 mask = 0, value = 0;
    for (size_t i = 0; i < mask_str.size(); i++) {
        mask |= ((mask_str[i] == 'x') ? 0 : 1) << i;
        value |= ((mask_str[i] == '1') ? 1 : 0) << i;
    }
    return std::make_pair(mask, value);
}

void InstructionTableA64::init() {
#define INST(x, name, regs, mask, ...) \
    instr_table_[OpcodeA64::x] = InstrA64Info( \
        OpcodeA64::x, \
        ParseMaskValuePair(mask), \
        name, \
        {}, \
        cur_type_ \
    );
#define Type(x) cur_type_ = InstrTypeA64::x;

    instr_table_[OpcodeA64::INVALID] = InstrA64Info(OpcodeA64::INVALID, {},"INVALID", {}, Unallocated);
#include "instructions_table.inl"

    instr_table_[OpcodeA64::NUM_INSTRUCTIONS] = InstrA64Info(OpcodeA64::NUM_INSTRUCTIONS, {}, "NUM_INSTRUCTIONS", {}, Unallocated);

#undef INST
#undef ARG_LIST
#undef Type

    for (const auto &item:instr_table_) {
        OpcodeA64 opcode = item.first;
        const auto &info = item.second;
        if (instr_type_table_.find(info.type_) == instr_type_table_.end()) {
            instr_type_table_[info.type_] = {};
        }
        instr_type_table_[info.type_][opcode] = info;
    }

}

InstructionTableA64 &InstructionTableA64::Get() {
    static InstructionTableA64 inst;
    return inst;
}

InstructionTableA64::InstructionTableA64() {
    init();
}

const std::map<OpcodeA64, InstrA64Info> &InstructionTableA64::GetInstrTable() const {
    return instr_table_;
}

InstrTypeA64 InstructionTableA64::Type(OpcodeA64 opcode) {
    return Unallocated;
}

InstrA64Info &InstructionTableA64::GetInstrInfo(OpcodeA64 opcode) {
    return instr_table_[opcode];
}

std::map<OpcodeA64, InstrA64Info> &
InstructionTableA64::GetInstrTable(InstrTypeA64 type) {
    return instr_type_table_[type];
}
