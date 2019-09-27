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
#define PAGE_OFFSET 12
#define A64_PAGE_SIZE (2 << PAGE_OFFSET)

        class InstructionA64 : public Instruction<AArch64Inst> {
        public:
            InstructionA64();

            void SetOpcode(OpcodeA64 opcode);

            OpcodeA64 GetOpcode();

            bool Invalid();

            bool Disassemble(AArch64Inst &t) override;

            bool Assemble() override;

            virtual InstrTypeA64 TypeOfA64() const {
                return InstrTypeA64::Invalid;
            };

        protected:
            AArch64Inst backup_;
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

            XReg GetRn() const;

            void SetRn(XReg rn);

            bool IsAbs() const;

            bool IsLink() const;

            void SetLink(bool link);

            InstrTypeA64 TypeOfA64() const override {
                return InstrTypeA64::Branches;
            };

            bool Disassemble(AArch64Inst &t) override;

            bool Assemble() override;

        protected:
            Condition cond_{AL};
            bool is_abs_ = true;
            bool link_ = false;
            s32 offset_;
            VAddr target_;
            XReg rt_;
            XReg rn_;
        };

        class InstrA64ExpGen : public InstructionA64 {
        public:

            InstrA64ExpGen();

            u16 GetImm() const;

            void SetImm(u16 imm);

            InstrTypeA64 TypeOfA64() const override {
                return InstrTypeA64::ExpGen;
            };

            bool Disassemble(AArch64Inst &t) override;

            bool Assemble() override;

            bool Excutable(ExceptionLevel cur_level);

        protected:
            ExceptionLevel to_exception_level_;
            u16 imm_;
        };


        class InstrA64System : public InstructionA64 {
        public:
            InstrA64System();

            const SystemRegister &GetSystemRegister() const;

            void SetSystemRegister(const SystemRegister &systemRegister);

            XReg GetRt() const;

            void SetRt(XReg rt);

            InstrTypeA64 TypeOfA64() const override {
                return InstrTypeA64::System;
            };

            bool Disassemble(AArch64Inst &t) override;

            bool Assemble() override;

        protected:
            SystemRegister system_register_{};
            XReg rt_;
        };

        class InstrA64PCRelAddr : public InstructionA64 {
        public:

            InstrA64PCRelAddr();

            s32 GetOffset() const;

            void SetOffset(s32 offset);

            VAddr GetTarget();

            bool PageAlign() const;

            InstrTypeA64 TypeOfA64() const override {
                return InstrTypeA64::DataprocessingImmediate;
            };

            bool Disassemble(AArch64Inst &t) override;

            bool Assemble() override;

        private:
            s64 offset_;
            bool page_align_ = false;
        };

        class InstrA64AddSubImm : public InstructionA64 {
        public:

            InstrA64AddSubImm();

            GeneralRegister& GetRd();

            void SetRd(GeneralRegister rd);

            const Operand &GetOperand() const;

            void SetOperand(const Operand &operand);

            bool IsSub() const;

            bool IsUpdateFlag() const;

            bool Is64Bit() const;

            bool Disassemble(AArch64Inst &t) override;

            bool Assemble() override;

        private:
            bool is_sub_ = false;
            bool update_flag_ = false;
            bool is_64bit = false;
            bool shift_ = false;
            GeneralRegister rd_;
            Operand operand_;
        };
    }
}
