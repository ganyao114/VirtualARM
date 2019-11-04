//
// Created by 甘尧 on 2019-09-25.
//

#pragma once

#include "instruction_aarch64.h"
#include "instruction_table.h"

namespace Instruction::A64 {
    class Decoder {
    public:
        virtual InstrA64Ref Decode(InstrA64 *instr_bits) = 0;
    };

    class DefaultDecoder : public Decoder {
    public:
        InstrA64Ref Decode(InstrA64 *instr_bits) override;

        static OpcodeA64 DecodeOpCode(InstrA64 instr_bits, InstrTypeA64 type = Invalid);
    };

    class FastBranchDecoder : public DefaultDecoder {
    public:
        InstrA64Ref Decode(InstrA64 *instr_bits) override;

        InstrA64Ref DecodeDPImm(InstrA64 instr_bits);

        InstrA64Ref DecodeDPReg(InstrA64 instr_bits);

        InstrA64Ref DecodeSMID(InstrA64 instr_bits);

        InstrA64Ref DecodeSystemAndBranch(InstrA64 instr_bits);

        InstrA64Ref DecodeFloat(InstrA64 instr_bits);

        InstrA64Ref DecodeLoadAndStore(InstrA64 instr_bits);

    };

}
