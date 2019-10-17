//
// Created by 甘尧 on 2019-09-24.
//

#pragma once

#include "base/marcos.h"
#include "includes/cpu.h"

namespace Instruction {
    namespace A64 {

        enum Shift {
            NO_SHIFT = -1,
            LSL = 0x0,
            LSR = 0x1,
            ASR = 0x2,
            ROR = 0x3,
            MSL = 0x4,
            RRX = 0x4
        };

        enum AddressMode { Offset = 0, PostIndex = 1, PreIndex = 3, NonAddrMode = 2};

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

#define AARCH64_REGISTER_CODE_LIST(R)                                          \
  R(0)  R(1)  R(2)  R(3)  R(4)  R(5)  R(6)  R(7)                               \
  R(8)  R(9)  R(10) R(11) R(12) R(13) R(14) R(15)                              \
  R(16) R(17) R(18) R(19) R(20) R(21) R(22) R(23)                              \
  R(24) R(25) R(26) R(27) R(28) R(29) R(30) R(31)                              \



#define XREG(n) X##n,
        enum class XReg : u8 {
            AARCH64_REGISTER_CODE_LIST(XREG)
            LR = static_cast<int>(X30),
            SP = static_cast<int>(X31),
            ZR = static_cast<int>(X31)
        };
#undef XREG

#define WREG(n) W##n,
        enum class WReg : u8 {AARCH64_REGISTER_CODE_LIST(WREG)};
#undef WREG

#define DREG(n) D##n,
        enum class DReg : u8 {AARCH64_REGISTER_CODE_LIST(DREG)};
#undef DREG

#define QREG(n) Q##n,
        enum class QReg : u8 {AARCH64_REGISTER_CODE_LIST(QREG)};
#undef QREG

#define VREG(n) V##n,
        enum class VReg : u8 {AARCH64_REGISTER_CODE_LIST(VREG)};
#undef VREG

        class A64Register : public CPU::Register {

        };

        class GeneralRegister : public A64Register {
        public:

            enum class Type : u8 {
                U,
                X,
                W,
                V,
                Q
            };

            GeneralRegister();
            GeneralRegister(XReg xreg);
            GeneralRegister(WReg wreg);
            GeneralRegister(VReg vreg);
            GeneralRegister(QReg qreg);
            bool InValid() const {
                return type_ == Type::U;
            }
            bool IsW() const {
                return type_ == Type::W;
            }
            bool IsV() const {
                return type_ == Type::V;
            }
            bool IsQ() const {
                return type_ == Type::V;
            }
            bool IsX() const {
                return type_ == Type::X;
            };
            bool IsSP() const {
                return IsX() && xreg_ == XReg::SP;
            };
            bool IsFP() const {
                switch (type_) {
                    case Type::V:
                    case Type::Q:
                        return true;
                    default:
                        return false;
                }
            };
            u8 Size() const {
                switch (type_) {
                    case Type::V:
                        return 128;
                    case Type::Q:
                        return 64;
                    case Type::X:
                        return 64;
                    case Type::W:
                        return 32;
                    default:
                        return 64;
                }
            };
            u32 Code() const {
                return static_cast<u32>(xreg_);
            }
        private:
            Type type_{Type::X};
            union {
                WReg wreg_;
                XReg xreg_;
                VReg vreg_;
                QReg qreg_;
            };
        };

#define XREG(x) GeneralRegister(XReg(x))
#define WREG(x) GeneralRegister(WReg(x))
#define VREG(x) GeneralRegister(VReg(x))
#define QREG(x) GeneralRegister(QReg(x))
#define UNKNOW_REG GeneralRegister()

        class VRegister : public A64Register {

        };

        class SystemRegister {
        public:

            union SystemReg {
                u16 value;
                struct {
                    u16 op2:3;
                    u16 CRm:4;
                    u16 CRn:4;
                    u16 op1:3;
                    u16 op0:2;
                };
            };

            SystemRegister();

            SystemRegister(u16 value);

            SystemRegister(u16 op0, u16 op1, u16 crn, u16 crm, u16 op2);

            FORCE_INLINE u16 Value() const {
                return value.value;
            }

            bool operator==(const SystemRegister &rhs) const {
                return value.value == rhs.value.value;
            }

            bool operator!=(const SystemRegister &rhs) const {
                return value.value != rhs.value.value;
            }

        protected:
            SystemReg value{};
        };

        class Operand {
        public:
            inline explicit Operand(){};
            inline explicit Operand(s64 imm)
                    : immediate_(imm), shift_(NO_SHIFT), extend_(NO_EXTEND), shift_extend_imm_(0) {}
            inline explicit Operand(GeneralRegister reg, int32_t imm = 0, Shift shift = LSL)
                    : immediate_(0), reg_(reg), shift_(shift), extend_(NO_EXTEND), shift_extend_imm_(imm) {}
            inline explicit Operand(GeneralRegister reg, Extend extend, int32_t imm = 0)
                    : immediate_(0), reg_(reg), shift_(NO_SHIFT), extend_(extend), shift_extend_imm_(imm) {}

            // =====

            bool IsImmediate() const { return false; }
            bool IsShiftedRegister() const { return  (shift_ != NO_SHIFT); }
            bool IsExtendedRegister() const { return (extend_ != NO_EXTEND); }

        public:
            s64 immediate_;
            GeneralRegister reg_{};
            Shift shift_;
            Extend extend_;
            s32 shift_extend_imm_;
        };

        class MemOperand {
        public:
            inline explicit MemOperand() {}
            inline explicit MemOperand(GeneralRegister &base, s32 offset = 0, AddressMode addr_mode = Offset)
                    : base_(base), reg_offset_(UNKNOW_REG), offset_(offset), addr_mode_(addr_mode), shift_(NO_SHIFT),
                      extend_(NO_EXTEND), shift_extend_imm_(0) {}

            inline explicit MemOperand(GeneralRegister &base, GeneralRegister &reg_offset, Extend extend, unsigned extend_imm)
                    : base_(base), reg_offset_(reg_offset), offset_(0), addr_mode_(Offset), shift_(NO_SHIFT), extend_(extend),
                      shift_extend_imm_(extend_imm) {}

            inline explicit MemOperand(GeneralRegister base, GeneralRegister &reg_offset, Shift shift = LSL, unsigned shift_imm = 0)
                    : base_(base), reg_offset_(reg_offset), offset_(0), addr_mode_(Offset), shift_(shift), extend_(NO_EXTEND),
                      shift_extend_imm_(shift_imm) {}

            inline explicit MemOperand(GeneralRegister& base, const Operand &offset, AddressMode addr_mode = Offset)
                    : base_(base), reg_offset_(UNKNOW_REG), addr_mode_(addr_mode) {
                if (offset.IsShiftedRegister()) {
                    reg_offset_        = offset.reg_;
                    shift_            = offset.shift_;
                    shift_extend_imm_ = offset.shift_extend_imm_;

                    extend_ = NO_EXTEND;
                    this->offset_ = 0;
                } else if (offset.IsExtendedRegister()) {
                    reg_offset_        = offset.reg_;
                    extend_           = offset.extend_;
                    shift_extend_imm_ = offset.shift_extend_imm_;

                    shift_  = NO_SHIFT;
                    this->offset_ = 0;
                }
            }

            // =====

            bool IsImmediateOffset() const { return (addr_mode_ == Offset); }
            bool IsRegisterOffset() const { return (addr_mode_ == Offset); }
            bool IsPreIndex() const { return addr_mode_ == PreIndex; }
            bool IsPostIndex() const { return addr_mode_ == PostIndex; }

        public:
            GeneralRegister base_;
            GeneralRegister reg_offset_;
            s32 offset_;
            AddressMode addr_mode_;
            Shift shift_;
            Extend extend_;
            s32 shift_extend_imm_;
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

#include "fields_table.inl"

#undef FIELD
            };

            AArch64Inst(u32 raw);
        };
    }
}
