//
// Created by 甘尧 on 2019-09-24.
//

#pragma once

#include "instruction_fields.h"
#include <map>
#include <vector>

namespace Instruction {
    namespace A64 {

        using MaskValuePair = std::pair<InstrA64, InstrA64>;

        MaskValuePair ParseMaskValuePair(std::string mask_str);

        enum InstrTypeA64 {
            Invalid,
            Unallocated,
            AddSubImmediate,
            MovWide,
            PCRelAdr,
            LogicalImmediate,
            BitField,
            Extract,
            Branches,
            ExpGen,
            System,
            LoadsAndStores,
            DataProcessingRegister,
            DataProcessingSIMDAndFP
        };

#define INST(x, ...) x,
#define Type(x)
        enum class OpcodeA64 : InstrA64 {
            UN_DECODED,
            INVALID,

#include "instructions_table.inl"

            NUM_INSTRUCTIONS,
        };
#undef INST
#undef Type

        struct InstrA64Info {
        public:
            OpcodeA64 opcode;
            MaskValuePair mask_pair;
            std::string name;
            std::vector<FieldA64> fields;
            InstrTypeA64 type;

            bool Test(InstrA64 bits) const;
        };

        class InstructionTableA64 {
        public:

            InstructionTableA64();

            static InstructionTableA64 &Get();

            void init();

            InstrTypeA64 Type(OpcodeA64 opcode);

            const std::map<OpcodeA64, InstrA64Info> &GetInstrTable() const;
            std::map<OpcodeA64, InstrA64Info> &GetInstrTable(InstrTypeA64 type);

            InstrA64Info& GetInstrInfo(OpcodeA64 opcode);

        private:
            InstrTypeA64 cur_type_;
            std::map<OpcodeA64, InstrA64Info> instr_table_;
            std::map<InstrTypeA64, std::map<OpcodeA64, InstrA64Info>> instr_type_table_;
        };

    }
}
