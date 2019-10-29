//
// Created by 甘尧 on 2019-10-25.
//

#pragma once

#include <includes/instruction.h>

class InstrIR;

namespace Instruction {
    namespace IR {

        //description return of some instructions
        template <DataSize reg_size>
        class Return : public Argument {
        public:
            Return() {}
            Return(const SharedPtr<InstrIR> &instr) : instr_(instr) {}
        private:
            union {
                SharedPtr<InstrIR> instr_;
            };
        };

        struct FrontedReg {
            u8 type;
            u8 code;
        };

        template <DataSize reg_size>
        class RegisterIR : public Register<reg_size> {
        public:

            //wrap return value with register
            RegisterIR(const Return<reg_size> &ret):return_(ret){
                for_ret_ = true;
            }

            RegisterIR(const FrontedReg &fronted) : fronted_(fronted) {
                fronted_reg_ = true;
            }

        private:
            bool for_ret_ {false};
            bool fronted_reg_ {false};
            union {
                Return<reg_size> return_ {};
                FrontedReg fronted_;
            };
        };


        class InstrIR : public BaseObject {
        private:
        };


        using RetVoid = Return<DataSize::Void>;
        using RetU1 = Return<DataSize::U1>;
        using RetU8 = Return<DataSize::U8>;
        using RetU16 = Return<DataSize::U16>;
        using RetU32 = Return<DataSize::U32>;
        using RetU64 = Return<DataSize::U64>;
        using RetU128 = Return<DataSize::U128>;
        using RetU16U32U64 = Return<DataSize::U16 | DataSize::U32 | DataSize::U64>;
        using RetAny = Return<DataSize::U1 | DataSize::U8 | DataSize::U16 | DataSize::U32 | DataSize::U64 | DataSize::U128>;


        using RegU1 = RegisterIR<DataSize::U1>;
        using RegU8 = RegisterIR<DataSize::U8>;
        using RegU16 = RegisterIR<DataSize::U16>;
        using RegU32 = RegisterIR<DataSize::U32>;
        using RegU64 = RegisterIR<DataSize::U64>;
        using RegU128 = RegisterIR<DataSize::U128>;
        using RegU16U32U64 = RegisterIR<DataSize::U16 | DataSize::U32 | DataSize::U64>;
        using RegAny = RegisterIR<DataSize::U1 | DataSize::U8 | DataSize::U16 | DataSize::U32 | DataSize::U64 | DataSize::U128>;

        using Imm1 = Imm<DataSize::U1>;
        using Imm8 = Imm<DataSize::U8>;
        using Imm16 = Imm<DataSize::U16>;
        using Imm32 = Imm<DataSize::U32>;
        using Imm64 = Imm<DataSize::U64>;
        using Imm128 = Imm<DataSize::U128>;
    }
}
