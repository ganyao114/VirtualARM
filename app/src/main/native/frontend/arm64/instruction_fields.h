//
// Created by 甘尧 on 2019-09-24.
//

#pragma once

#include "base/marcos.h"

namespace Instruction {
    namespace A64 {

        enum SHIFT_TYPE {
            LSL = 0,
            LSR = 1,
            ASR = 2,
            ROR = 3
        };


        enum Condition {
            EQ, NE, CS, CC, MI, PL, VS, VC, HI, LS, GE, LT, GT, LE, AL, NV,
            HS = CS, LO = CC,
        };

        enum ExceptionLevel {
            EL0 = 0,
            EL1 = 1,
            EL2 = 2,
            EL3 = 3
        };

        enum Extend {
            NO_EXTEND = -1,
            UXTB      = 0,
            UXTH      = 1,
            UXTW      = 2,
            UXTX      = 3,
            SXTB      = 4,
            SXTH      = 5,
            SXTW      = 6,
            SXTX      = 7
        };

        using InstrA64 = u32;

        enum FieldA64 {
            NONE,
            cond_branch,
            cond_comp,
            Wt, Xt, Bt, Ht, St, Dt, Qt,
            Wt2, Xt2, Bt2, Ht2, St2, Dt2, Qt2,
            Wn, Xn, Bn, Hn, Sn, Dn, Qn,
            Wn_WSP, Xn_SP,
            Wm, Xm, Bm, Hm, Sm, Dm, Qm,
            Wa, Xa, Ba, Ha, Sa, Da, Qa,
            Wd, Xd, Bd, Hd, Sd, Dd, Qd,
            Wd_WSP, Xd_SP,
            Ws,
            imm_tbz,
            imm_exceptions,
            imm_hint,
            imm_clrex,
            imm_adr,
            imm_adrp,
            imm_addsub,
            imm_logical,
            imm_mov,
            imm_bitfield,
            imm_extract,
            imm_compare,
            imm_shift,
            label_cond_branch,
            label_branch,
            label_tbz,
            label_loadstore,
            prefetch_op,
            barrier_type,
            shifted_reg,
            W_extended_reg,
            X_extended_reg,
            cond_comp_flags,
            loadstore_no_off,
            loadstore_post_off,
            loadstore_pre_off,
            loadstore_unscaled_off,
            loadstore_unsigned_off_8,
            loadstore_unsigned_off_16,
            loadstore_unsigned_off_32,
            loadstore_unsigned_off_64,
            loadstore_unsigned_off_128,
            loadstore_post_off_pair_32,
            loadstore_post_off_pair_64,
            loadstore_post_off_pair_128,
            loadstore_pre_off_pair_32,
            loadstore_pre_off_pair_64,
            loadstore_pre_off_pair_128,
            loadstore_off_pair_32,
            loadstore_off_pair_64,
            loadstore_off_pair_128,
            loadstore_reg,
            float_zero_imm,
            TODO,
        };

        struct AArch64Inst {
            union {
                u32 raw;

#define FIELD(name, from, to) \
        struct { \
            u32 : (from); \
            u32 name : ((to) - (from) + 1); \
            u32 : (31 - (to)); \
        };

                //cond
                FIELD(cond, 0, 3); // condition for branch
                FIELD(cond_comp, 12, 15); // condition for conditional compare

                //L1 Type
                FIELD(instr_type_1, 25, 28);

                //system & branch
                FIELD(sys_bch_op0, 29, 31);
                FIELD(sys_bch_op1, 22, 25);
                FIELD(bch_cond_type, 22, 22);
                FIELD(bch_ucond_opc, 21, 24);
                FIELD(bch_ucond_op2, 16, 20);
                FIELD(bch_ucond_op3, 10, 15);
                FIELD(bch_ucond_op4, 0, 4);
                FIELD(bch_ucond_offset, 0, 25);
                FIELD(bch_cond_offset, 5, 23);
                FIELD(exp_gen_ll, 0, 1);
                FIELD(exp_gen_num, 5, 20);

                FIELD(_5_11, 5, 11);
                FIELD(_5_18, 5, 18);
                FIELD(_8_11, 8, 11);
                FIELD(_10_12, 10, 12); // immediate value for some DPReg operations
                FIELD(_10_15, 10, 15); // immediate value for some DPReg operations
                FIELD(_10_21, 10, 21); // immediate value for some DPImm operations
                FIELD(_12_20, 12, 20);
                FIELD(_13_20, 13, 20); // immediate value for some float operations
                FIELD(_15_21, 15, 21); // immediate value for some Load/Store pair operations
                FIELD(_16_20, 16, 20); // immediate value for some DPReg operations
                FIELD(_22_23, 22, 23); // size value for some SIMD operations
                FIELD(o0, 19, 19); // System register?
                FIELD(op1, 16, 18); // System register?
                FIELD(CRn, 12, 15); // System register?
                FIELD(CRm, 8, 11); // System register? Also used for barriers
                FIELD(op2, 5, 7); // System register?
                FIELD(option, 13, 15); // extend option for extened register instructions
                FIELD(hw, 21, 22); // shift amount for mov instructions
                FIELD(opc, 29, 30); // additional opcode for instructions
                FIELD(shift, 22, 23); // shift type

                //Registers
                FIELD(Rt, 0, 4); // register to test against/store
                FIELD(Rd, 0, 4); // destination register for data processing operations
                FIELD(Rn, 5, 9); // input register 1
                FIELD(Rm, 16, 20); // input register 2
                FIELD(Ra, 10, 14); // input register 3
                FIELD(Rt2, 10, 14); // 2nd register to store
                FIELD(Rs, 16, 20); // status register for load/store exclusive

                //imme
                FIELD(immlo, 29, 30); // low portion of immediate for PC-rel. addressing
                FIELD(immhi, 5, 23); // high portion of immediate for PC-rel. addressing
                FIELD(imms, 10, 15); // part of immediate for DPImm operations
                FIELD(immr, 16, 21); // part of immediate for DPImm operations
                FIELD(N, 22, 22); // part of immediate for DPImm operations
                FIELD(b40, 19, 23); // immediate for TB(N)Z
                FIELD(b5, 31, 31); // immediate for TB(N)Z
                FIELD(nzcv, 0, 3); // flags set for cond compare
#undef FIELD
            };

            AArch64Inst(u32 raw);
        };
    }
}
