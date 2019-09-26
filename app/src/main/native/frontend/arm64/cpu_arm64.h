//
// Created by 甘尧 on 2019-09-25.
//

#pragma once

#include "base/marcos.h"

namespace CPU {
    namespace A64 {


#define AARCH64_REGISTER_CODE_LIST(R)                                          \
  R(0)  R(1)  R(2)  R(3)  R(4)  R(5)  R(6)  R(7)                               \
  R(8)  R(9)  R(10) R(11) R(12) R(13) R(14) R(15)                              \
  R(16) R(17) R(18) R(19) R(20) R(21) R(22) R(23)                              \
  R(24) R(25) R(26) R(27) R(28) R(29) R(30) R(31)                              \



#define XREG(n) X##n,
        enum class XReg {
            AARCH64_REGISTER_CODE_LIST(XREG)
            LR = static_cast<int>(X30),
            SP = static_cast<int>(X31),
            ZR = static_cast<int>(X31)
        };
#undef XREG

#define WREG(n) W##n,
        enum class WReg {AARCH64_REGISTER_CODE_LIST(WREG)};
#undef WREG

#define DREG(n) D##n,
        enum class DReg {AARCH64_REGISTER_CODE_LIST(DREG)};
#undef DREG

#define QREG(n) Q##n,
        enum class QReg {AARCH64_REGISTER_CODE_LIST(QREG)};
#undef QREG

#define VREG(n) V##n,
        enum class VReg {AARCH64_REGISTER_CODE_LIST(VREG)};
#undef VREG


        union HalfFloat {
            u16 bits;
            s16 bits_signed; // TODO: is this guaranteed to be two's complement?
            struct {
                u16 mantissa : 10;
                u16 exponent : 5;
                u16 sign : 1;
            };

            struct {
                u16 : 9;
                u16 is_quiet : 1;
                u16 : 6;
            };

            static const u64 max_exp = 0x1F;
        };

        union SingleFloat {
            u32 bits;
            s32 bits_signed; // TODO: is this guaranteed to be two's complement?
            float val;
            struct {
                u32 mantissa : 23;
                u32 exponent : 8;
                u32 sign : 1;
            };

            struct {
                u32 : 22;
                u32 is_quiet : 1;
                u32 : 9;
            };

            static const u64 max_exp = 0xFF;
        };

        union Double {
            u64 bits;
            s64 bits_signed; // TODO: is this guaranteed to be two's complement?
            double val;
            struct {
                u64 mantissa : 52;
                u64 exponent : 11;
                u64 sign : 1;
            };

            struct {
                u64 : 51;
                u64 is_quiet : 1;
                u64 : 12;
            };


            static const u64 maxExp = 0x7FF;
        };

        typedef union {
            u64 Q;
            u64 D[2];
            u32 S[4];
            u16 H[8];
            u8 B[16];

            HalfFloat float_h;
            SingleFloat float_s;
            Double float_d;
        } Vec;

        typedef union {
            u64 X;
            struct {
                u64 W : 32;
                u64 R: 32;
            };
        } Reg;

        struct PSTATE {
            bool N; // Negative
            bool Z; // Zero
            bool C; // Carry
            bool V; // Overflow
            int SS;
            int IL;
            int nRW;
            int EL;
            int SP;
            int D;
            int A, I, F;
        };

        struct CPUContext {
            std::array<Reg, 31> cpu_registers;
            u64 sp;
            u64 pc;
            PSTATE pstate;
            std::array<Vec, 32> vector_registers;
            u32 fpcr;
            u32 fpsr;
            u64 tpidr;
        };
    }
}
