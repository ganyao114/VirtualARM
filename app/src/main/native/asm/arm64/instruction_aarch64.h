//
// Created by 甘尧 on 2019-09-24.
//

#pragma once

#include <includes/instruction.h>
#include "base/marcos.h"
#include "instruction_fields.h"
#include "instruction_table.h"
#include "cpu_arm64.h"

namespace Instruction::A64 {

#define DECODE_OFFSET(val, bits, ext) offset_ = SignExtend<s32, (bits + ext)>(val << ext)
#define ENCODE_OFFSET(bits, ext) TruncateSTo<bits>(GetOffset() >> ext)
#define ENCODE_OPCODE pc_->raw = InstructionTableA64::Get().GetInstrInfo(GetOpcode()).mask_pair.second
#define PAGE_OFFSET 12
#define A64_PAGE_SIZE (2 << PAGE_OFFSET)

    const unsigned kWRegSize = 32;
    const unsigned kXRegSize = 64;
    const u64 kWRegMask = UINT64_C(0xffffffff);
    const u64 kXRegMask = UINT64_C(0xffffffffffffffff);

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

        A64Register &GetRt();

        void SetRt(A64Register rt);

        A64Register &GetRn();

        void SetRn(A64Register rn);

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
        A64Register rt_;
        A64Register rn_;
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

        A64Register &GetRt();

        void SetRt(A64Register rt);

        InstrTypeA64 TypeOfA64() const override {
            return InstrTypeA64::System;
        };

        bool Disassemble(AArch64Inst &t) override;

        bool Assemble() override;

    protected:
        SystemRegister system_register_{};
        A64Register rt_;
    };

    class InstrA64PCRelAddr : public InstructionA64 {
    public:

        InstrA64PCRelAddr();

        s32 GetOffset() const;

        void SetOffset(s32 offset);

        VAddr GetTarget();

        bool PageAlign() const;

        InstrTypeA64 TypeOfA64() const override {
            return InstrTypeA64::PCRelAdr;
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

        A64Register &GetRd();

        void SetRd(A64Register rd);

        const Operand &GetOperand() const;

        void SetOperand(const Operand &operand);

        bool IsSub() const;

        bool IsUpdateFlag() const;

        bool Is64Bit() const;

        InstrTypeA64 TypeOfA64() const override {
            return InstrTypeA64::AddSubImmediate;
        };

        bool Disassemble(AArch64Inst &t) override;

        bool Assemble() override;

    private:
        bool is_sub_ = false;
        bool update_flag_ = false;
        bool is_64bit = false;
        bool shift_ = false;
        A64Register rd_;
        Operand operand_;
    };

    class InstrA64MovWide : public InstructionA64 {
    public:

        enum Shift : u8 {
            Shift0 = 0,
            Shift1 = 16,
            Shift2 = 32,
            Shift3 = 48
        };

        InstrA64MovWide();

        A64Register &GetRd();

        void SetRd(A64Register &rd);

        u16 GetImm() const;

        void SetImm(u16 imm);

        Shift GetShift() const;

        void SetShift(Shift shift);

        u64 GetValue(u64 old_value);

        InstrTypeA64 TypeOfA64() const override {
            return InstrTypeA64::MovWide;
        };

        bool Disassemble(AArch64Inst &t) override;

        bool Assemble() override;

    private:
        A64Register rd_;
        u16 imm_;
        Shift shift_;
    };


    class InstrA64LogicalImm : public InstructionA64 {
    public:

        InstrA64LogicalImm();

        InstrTypeA64 TypeOfA64() const override {
            return InstrTypeA64::LogicalImmediate;
        };

        const A64Register &GetRd() const;

        void SetRd(const A64Register &rd);

        u64 GetImm() const;

        void SetImm(u64 imm);

        bool IsUpdateFlags() const;

        bool Disassemble(AArch64Inst &t) override;

        bool Assemble() override;

    private:
        bool update_flags_;
        A64Register rd_;
        u64 imm_;
    };


    class InstrA64BitField : public InstructionA64 {
    public:
        InstrA64BitField();

        InstrTypeA64 TypeOfA64() const override {
            return InstrTypeA64::BitField;
        };

        const A64Register &GetRd() const;

        void SetRd(const A64Register &rd);

        const A64Register &GetRn() const;

        void SetRn(const A64Register &rn);

        int GetS() const;

        void SetS(int s);

        int GetR() const;

        void SetR(int r);

        bool Disassemble(AArch64Inst &t) override;

        bool Assemble() override;

        u64 GetResult(u64 src, u64 dest);

    private:
        A64Register rd_, rn_;
        int S, R;
        u64 top_bits_;
        u64 mask_;
        bool extend_ = false;
        bool inzero_ = false;
    };


    class InstrA64Extract : public InstructionA64 {
    public:
        InstrA64Extract();

        InstrTypeA64 TypeOfA64() const override {
            return InstrTypeA64::Extract;
        };
    };


    class InstrA64LoadAndStore : public InstructionA64 {
    public:

        struct StoreFlags {
            //update Rn
            u8 StoreWriteBack:1;
            //use updated Rn
            u8 StorePostIndex:1;
            u8 StoreImmSigned:1;
            u8 StoreFloat:1;
            u8 Store128BitFloat:1;
            u8 StoreRelease:1;
            u8 StoreExclusive:1;
        };

        struct LoadFlags {
            u16 LoadWriteBack:1;
            u16 LoadImmSigned:1;
            u16 LoadPostIndex:1;
            u16 LoadExtendResult:1;
            u16 LoadExtendTo64:1;
            u16 LoadFloat:1;
            u16 Load128BitFloat:1;
            u16 LoadAcquire:1;
            u16 LoadExclusive:1;
        };

        enum Size : u8 {
            Size8 = 0,
            Size16,
            Size32,
            Size64
        };

        bool Disassemble(AArch64Inst &t) override;

    protected:
        Size size_;
        bool is_simd_;
    };


    class InstrA64StoreRegImm : public InstrA64LoadAndStore {
    public:

        InstrA64StoreRegImm() {}

        InstrTypeA64 TypeOfA64() const override {
            return InstrTypeA64::StoreRegImm;
        }

        bool ShouldUpdateRn();

        StoreFlags &GetFlags();

        bool Disassemble(AArch64Inst &t) override;

        bool Assemble() override;

    private:
        StoreFlags flags_{0};
        MemOperand operand_{};
        A64Register rt_;
    };

    class InstrA64LoadRegImm : public InstrA64LoadAndStore {
    public:

        InstrTypeA64 TypeOfA64() const override {
            return InstrTypeA64::LoadRegImm;
        };

        bool Assemble() override;

        bool Disassemble(AArch64Inst &t) override;

    private:
        LoadFlags flags_{0};
        MemOperand operand_;
        A64Register rd_;
    };

    class InstrA64LoadLiteral : public InstructionA64 {
    public:

        InstrA64LoadLiteral();

        InstrTypeA64 TypeOfA64() const override {
            return InstrTypeA64::LoadLiteral;
        };

        const A64Register &GetRt() const;

        void SetRt(const A64Register &rt);

        s32 GetOffset() const;

        void SetOffset(s32 offset);

        bool Disassemble(AArch64Inst &t) override;

        bool Assemble() override;

    private:
        A64Register rt_;
        s32 offset_;
    };

    class InstrA64StoreRegPair : public InstrA64LoadAndStore {
    public:

        InstrA64StoreRegPair();

        bool Disassemble(AArch64Inst &t) override;

        bool Assemble() override;

    private:
        A64Register rt1_, rt2_;
        StoreFlags flags_{0};
        MemOperand operand_;
    };


    class InstrA64LoadRegPair : public InstrA64LoadAndStore {
    public:

        InstrA64LoadRegPair();

        bool Disassemble(AArch64Inst &inst) override;

        bool Assemble() override;

    private:
        A64Register rd1_, rd2_;
        LoadFlags flags_{0};
        MemOperand operand_;
    };

}
