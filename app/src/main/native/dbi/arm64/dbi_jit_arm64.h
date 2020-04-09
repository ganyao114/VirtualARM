//
// Created by 甘尧 on 2020-02-29.
//

#pragma once

#include "asm/arm64/instruction_table.h"
#include "dbi_context_arm64.h"

using namespace Instructions::A64;

namespace DBI::A64 {

#define __ context_->Assembler().

template <unsigned bits, unsigned ext = 0>
static constexpr s32 DecodeOffset(u32 imm) {
    return SignExtend<s32, (bits + ext)>(imm << ext);
}

struct Visitor : public BaseVisitor {
        using instruction_return_type = void;

        SharedPtr<Context> context_;

        SharedPtr<ContextWithMemTrace> ContextMem() const {
            return DynamicObjectCast<ContextWithMemTrace>(context_);
        }

        SharedPtr<ContextNoMemTrace> ContextNoMem() const {
            return DynamicObjectCast<ContextNoMemTrace>(context_);
        }

#define Type(...)
#define FIELD1(f1) u32 f1
#define FIELD2(f1, f2) u32 f1, u32 f2
#define FIELD3(f1, f2, f3) u32 f1, u32 f2, u32 f3
#define FIELD4(f1, f2, f3, f4) u32 f1, u32 f2, u32 f3, u32 f4
#define INST(code, name, fields, ...) \
    void code(fields);

#include "asm/arm64/instructions_table_spec.inl"

#undef FIELD1
#undef FIELD2
#undef FIELD3
#undef FIELD4
#undef INST
#undef Type

        enum BrunchFlags {
            Link    = 1 << 0,
            TestBit = 1 << 1,
            Comp    = 1 << 2,
            Negate  = 1 << 3,
        };

        enum ExceptionFlags {
            Svc     = 1 << 0,
            Brk     = 1 << 1,
            Hvc     = 1 << 2,
        };

        enum LoadFlags {
            LoadWriteBack    = 1 << 0,
            LoadImmSigned    = 1 << 1,
            LoadPostIndex    = 1 << 2,
            LoadExtendResult = 1 << 3,
            LoadExtendTo64   = 1 << 4,
            LoadFloat        = 1 << 5,
            Load128BitFloat  = 1 << 6,
            LoadAcquire      = 1 << 7,
            LoadExclusive    = 1 << 8
        };

        enum StoreFlags {
            StoreWriteBack   = 1 << 0,
            StoreImmSigned   = 1 << 1,
            StorePostIndex   = 1 << 2,
            StoreFloat       = 1 << 3,
            Store128BitFloat = 1 << 4,
            StoreRelease     = 1 << 5,
            StoreExclusive   = 1 << 6
        };

        template <unsigned flags = 0>
        void BrunchImm(s32 offset = 0, Cond condition = NO_COND) {

        }

        template <unsigned flags = 0>
        void BrunchReg(u8 reg_target) {

        }

        template <unsigned flags = 0>
        void Exception(u16 imm) {
            if (flags & Svc) {
                context_->CallSvc(imm);
            } else if (flags & Brk) {

            } else {
                context_->Emit(inst->raw);
            }
        }

        template <bool align_page = false>
        void Addressing(u8 Rd, u32 immhi, u32 immlo) {
            u32 imm = (immhi << 2) | immlo;
            auto offset = DecodeOffset<21>(imm);
            if (align_page) {
                offset <<= PAGE_SIZE;
            }
            auto target = RoundDown(reinterpret_cast<VAddr>(inst), PAGE_SIZE) + offset;

            if (module_base) {
                // has a module base
                auto wrap = [this, Rd, target](std::array<Register, 1> tmp) -> void {
                    context_->LoadPcByModuleOffset(target - module_base, XRegister::GetXRegFromCode(Rd), tmp[0]);
                    context_->SetRegisterX(Rd, target);
                };
                context_->WrapContext<1>(wrap, {XRegister::GetXRegFromCode(Rd)});
            } else {
                context_->SetRegisterX(Rd, target);
            }
        }

        template<typename T, unsigned flags = 0>
        void LoadRegLiteral(u8 rt, u32 lit_offset) {
            s32 offset = DecodeOffset<21, 2>(lit_offset);
            VAddr target = reinterpret_cast<VAddr>(inst) + offset;
            if (flags & LoadFloat) {

            } else {

            }
        }
    };

#undef __
}
