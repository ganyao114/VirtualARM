//
// Created by 甘尧 on 2019-09-24.
//

#pragma once

#include "base/marcos.h"
#include <vector>

namespace Instructions {

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