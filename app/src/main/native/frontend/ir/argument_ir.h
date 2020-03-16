//
// Created by 甘尧 on 2019-10-25.
//

#pragma once

#include <includes/instruction.h>

namespace Instructions::IR {

    class InstrIR;

    struct FrontedReg {
        u8 type;
        u8 code;
    };

    class Argument {
    public:

        enum Type {
            Unknown,
            Cond,
            RegisterFromRet,
            RegisterFronted,
            RegisterState,
            StateFromRet,
            Imme,
            RetValue,
            Shift,
            Oprand,
            MemOprand
        };

        Argument() {}

        explicit Argument(InstrIR *inst) : type_(RegisterFromRet) {
            value_.instr = inst;
        }

        explicit Argument(FrontedReg &reg) : type_(RegisterFronted) {
            value_.fronted = reg;
        }

        explicit Argument(bool imm) : type_(Imme), size_(DataSize::U1) {
            value_.imm_u1 = imm;
        }

        explicit Argument(u8 imm) : type_(Imme), size_(DataSize::U8) {
            value_.imm_u8 = imm;
        }

        explicit Argument(u16 imm) : type_(Imme), size_(DataSize::U16) {
            value_.imm_u16 = imm;
        }

        explicit Argument(u32 imm) : type_(Imme), size_(DataSize::U32) {
            value_.imm_u32 = imm;
        }

        explicit Argument(u64 imm) : type_(Imme), size_(DataSize::U64) {
            value_.imm_u64 = imm;
        }

        explicit Argument(u128 imm) : type_(Imme), size_(DataSize::U128) {
            value_.imm_u128 = imm;
        }


        Type ArgType() const {
            return Unknown;
        };

        FORCE_INLINE bool IsCond() const {
            return ArgType() == Cond;
        }

        FORCE_INLINE bool IsUnknown() const {
            return ArgType() == Unknown;
        }

        FORCE_INLINE bool IsFrontedReg() const {
            return ArgType() == RegisterFronted;
        }

        FORCE_INLINE bool IsImm() const {
            return ArgType() == Imme;
        }

        Type type_;

        DataSize size_;

        union {
            RegisterDescription reg;
            bool imm_u1;
            u8 imm_u8;
            u16 imm_u16;
            u32 imm_u32;
            u64 imm_u64;
            u128 imm_u128;
            Condition cond;
            InstrIR *instr;
            FrontedReg fronted;
        } value_;

    };

    class Return {
    public:
        Return() = default;

        Return(InstrIR *inst) : instr(inst) {}

        DataSize size_;
        InstrIR *instr;
    };


    template<DataSize data_size>
    class ReturnType : public Return {
    public:
        ReturnType() = default;
        ReturnType(InstrIR *inst) : Return(inst) {
            size_ = data_size;
        }
    };

    class State : public Argument {
    public:
        State() = default;
        State(Return &ret) : Argument(ret.instr) {
            type_ = StateFromRet;
        }
    };


    template<DataSize data_size>
    class RegisterIR : public Argument {
    public:

        RegisterIR() = default;

        RegisterIR(FrontedReg &reg) : Argument(reg) {
            size_ = data_size;
        }

        RegisterIR(ReturnType<data_size> &ret) : Argument(ret.instr) {
            size_ = data_size;
        }

        RegisterIR(State &state) : Argument(state.value_.instr) {
            size_ = data_size;
            type_ = RegisterState;
        }
    };

    template<typename D>
    class Imm : public Argument {
    public:
        Imm() = default;
        Imm(D d) : Argument(d) {}
    };


    using RetVoid = ReturnType<DataSize::Void>;
    using RetU1 = ReturnType<DataSize::U1>;
    using RetU8 = ReturnType<DataSize::U8>;
    using RetU16 = ReturnType<DataSize::U16>;
    using RetU32 = ReturnType<DataSize::U32>;
    using RetU64 = ReturnType<DataSize::U64>;
    using RetU128 = ReturnType<DataSize::U128>;
    using RetU16U32U64 = ReturnType<DataSize::U16 | DataSize::U32 | DataSize::U64>;
    using RetAny = ReturnType<
            DataSize::U1 | DataSize::U8 | DataSize::U16 | DataSize::U32 | DataSize::U64 |
            DataSize::U128>;


    using RegU1 = RegisterIR<DataSize::U1>;
    using RegU8 = RegisterIR<DataSize::U8>;
    using RegU16 = RegisterIR<DataSize::U16>;
    using RegU32 = RegisterIR<DataSize::U32>;
    using RegU64 = RegisterIR<DataSize::U64>;
    using RegU128 = RegisterIR<DataSize::U128>;
    using RegU16U32U64 = RegisterIR<DataSize::U16 | DataSize::U32 | DataSize::U64>;
    using RegAny = RegisterIR<
            DataSize::U1 | DataSize::U8 | DataSize::U16 | DataSize::U32 | DataSize::U64 |
            DataSize::U128>;
    using RegAddr = RegisterIR<DataSize(sizeof(VAddr) * 8)>;

    using Imm1 = Imm<bool>;
    using Imm8 = Imm<u8>;
    using Imm16 = Imm<u16>;
    using Imm32 = Imm<u32>;
    using Imm64 = Imm<u64>;
    using Imm128 = Imm<u128>;
}
