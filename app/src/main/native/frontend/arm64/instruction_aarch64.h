//
// Created by 甘尧 on 2019-09-24.
//

#pragma once

#include <includes/instruction.h>
#include "base/marcos.h"
#include "instruction_fields.h"
#include "instruction_table.h"
#include "cpu_arm64.h"

namespace Instruction {
    namespace A64 {

#define DECODE_OFFSET(val, bits, ext) offset_ = SignExtend<s32, (bits + ext)>(val << ext)
#define ENCODE_OFFSET(bits, ext) TruncateSTo<bits>(GetOffset() >> ext)

        using namespace CPU::A64;

        class InstructionA64 : public Instruction<AArch64Inst> {
        public:
            InstructionA64();

            void SetOpcode(OpcodeA64 opcode);

            OpcodeA64 GetOpcode();

            bool Invalid();
        };

        using InstrA64Ref = SharedPtr<InstructionA64>;


        class InstrA64Branch : public InstructionA64 {
        public:

            InstrA64Branch();

            Condition GetCond() const;

            void SetCond(Condition cond);

            bool HasCond() const;

            VAddr GetTarget() const;

            void SetTarget(VAddr target);

            s32 GetOffset() const;

            void SetOffset(s32 offset);

            XReg GetRt() const;

            void SetRt(XReg rt);

            bool IsAbs() const;

            bool IsLink() const;

            void SetLink(bool link);

            bool Disassemble(AArch64Inst &t) override;

            bool Assemble() override;

        protected:
            Condition cond_{AL};
            bool is_abs_ = true;
            bool link_ = false;
            s32 offset_;
            VAddr target_;
            XReg rt_;
        };

        class InstrA64ExpGen : public InstructionA64 {
        public:

            InstrA64ExpGen();

            u16 GetImm() const;

            void SetImm(u16 imm);

            bool Disassemble(AArch64Inst &t) override;

            bool Assemble() override;

            bool Excutable(ExceptionLevel cur_level);

        protected:
            ExceptionLevel to_exception_level_;
            u16 imm_;
        };

    }
}
