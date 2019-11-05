//
// Created by 甘尧 on 2019-09-25.
//

#pragma once

#include "base/marcos.h"

namespace CPU::A64 {


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
        Reg cpu_registers[31];
        u64 sp;
        u64 pc;
        PSTATE pstate;
        Vec vector_registers[32];
        u32 fpcr;
        u32 fpsr;
        u64 tpidr;
    };
}
