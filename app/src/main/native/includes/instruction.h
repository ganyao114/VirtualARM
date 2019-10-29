//
// Created by 甘尧 on 2019-09-24.
//

#pragma once

#include "base/marcos.h"
#include <vector>

namespace Instruction {

    enum class Condition {
        EQ, NE, CS, CC, MI, PL, VS, VC, HI, LS, GE, LT, GT, LE, AL, NV,
        HS = static_cast<int>(CS), LO = static_cast<int>(CC),
    };

    struct RegisterDescription {
        enum Type : u8 {
            R64,
            R32,
            R16,
            V32,
            V64,
            V128
        };
        Type type;
        u16 code;
    };

    enum class DataSize : u16 {
        Void = 0,
        U1 = 1 << 0,
        U8 = 1 << 3,
        U16 = 1 << 4,
        U32 = 1 << 5,
        U64 = 1 << 6,
        U128 = 1 << 7
    };

    constexpr DataSize operator|(DataSize a, DataSize b) {
        return static_cast<DataSize>(static_cast<u16>(a) | static_cast<u16>(b));
    }

    constexpr DataSize operator&(DataSize a, DataSize b) {
        return static_cast<DataSize>(static_cast<u16>(a) & static_cast<u16>(b));
    }

    class Argument {
    public:
        enum Type {
            Unknown,
            Cond,
            Register,
            Imme,
            RetValue,
            Shift,
            Oprand,
            MemOprand
        };

        virtual Type ArgType() {
            return Unknown;
        };

        FORCE_INLINE bool IsCond() {
            return ArgType() == Cond;
        }

        FORCE_INLINE bool IsUnknown() {
            return ArgType() == Unknown;
        }

        FORCE_INLINE bool IsReg() {
            return ArgType() == Register;
        }

        FORCE_INLINE bool IsImm() {
            return ArgType() == Imme;
        }

        union {
            RegisterDescription reg;
            bool imm_u1;
            u8 imm_u8;
            u16 imm_u16;
            u32 imm_u32;
            u64 imm_u64;
            Condition imm_cond;
        } value_;
    };

    template <DataSize bit_size>
    class Imm : public Argument {
    public:
        static constexpr DataSize bit_size_ = bit_size;

        explicit Imm(u16 value) {
            value_.imm_u16 = value;
        }

        explicit Imm(u32 value) {
            value_.imm_u32 = value;
        }

        explicit Imm(u64 value) {
            value_.imm_u64 = value;
        }

        virtual Type ArgType() {
            return Imme;
        };

        template <typename T = u32>
        T ZeroExtend() const {
            return static_cast<T>(value_.imm_u32);
        }

        template <typename T = s32>
        T SignExtend() const {
            return SignExtendX(bit_size_, value_.imm_u32);
        }

        bool operator==(Imm other) const {
            return value_.imm_u32 == other.value_.imm_u32;
        }

        bool operator!=(Imm other) const {
            return !operator==(other);
        }

        bool operator<(Imm other) const {
            return value_.imm_u32 < other.value_.imm_u32;
        }

        bool operator<=(Imm other) const {
            return value_.imm_u32 <= other.value_.imm_u32;
        }

        bool operator>(Imm other) const {
            return value_.imm_u32 > other.value_.imm_u32;
        }

        bool operator>=(Imm other) const {
            return value_.imm_u32 >= other.value_.imm_u32;
        }

    };

    template <DataSize reg_size>
    class Register : public Argument {
        virtual Type ArgType() {
            return Type::Register;
        };
    };

    using ArgRef = SharedPtr<Argument>;

    enum InstrType {
        NormalCalculations,
        MemoryAccess,
        System,
        ContextSetAndGet,
        Saturated,
        Packed,
        CRC,
        AES,
        SMID4,
        Vector,
        FloatingCalculation,
        FloatingConversion,
        FloatingVector,
        Branch
    };

    template <typename T>
    class Instruction : public BaseObject {
    public:

        Instruction() {}

        void SetPC(T *pc) {
            pc_ = pc;
        }

        T *GetPC() const {
            return pc_;
        }

        virtual bool Disassemble(T& t) {
            return false;
        }
        virtual bool Assemble() {
            return false;
        }

    protected:
        InstrType type_;
        u32 opcode_ = 0;
        T* pc_;
    };

    using InstructionRef = SharedPtr<Instruction<Base>>;

    class Location {

    };

}