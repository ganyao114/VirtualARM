//
// Created by 甘尧 on 2019-09-24.
//

#include "instruction_fields.h"

using namespace Instruction::A64;

AArch64Inst::AArch64Inst(u32 raw) : raw(raw) {}

SystemRegister::SystemRegister(u16 op0, u16 op1, u16 crn, u16 crm, u16 op2) {
    value.CRm = crm;
    value.CRn = crn;
    value.op0 = op0;
    value.op1 = op1;
    value.op2 = op2;
}

SystemRegister::SystemRegister() {
}

SystemRegister::SystemRegister(u16 value) {
    this->value.value = value;
}

A64Register::A64Register() {
    type_ = Type::U;
}

A64Register::A64Register(XReg xreg) : xreg_(xreg) {
    type_ = Type::X;
}

A64Register::A64Register(WReg wreg) : wreg_(wreg) {
    type_ = Type::W;
}

A64Register::A64Register(VReg vreg) : vreg_(vreg) {
    type_ = Type::V;
}

A64Register::A64Register(QReg qreg) : qreg_(qreg) {
    type_ = Type::Q;
}

A64Register::A64Register(DReg dreg) : dreg_(dreg) {
    type_ = Type::D;
}

A64Register A64Register::X(u32 data_size, u8 code) {
    if (data_size <= 32) {
        return WREG(code);
    } else {
        return XREG(code);
    }
}

A64Register A64Register::V(u32 data_size, u8 code) {
    if (data_size <= 32) {
        return QREG(code);
    } else if (data_size <= 64) {
        return DREG(code);
    } else {
        return VREG(code);
    }
}

const std::function<u32(AArch64Inst&)> Instruction::A64::GetAArch64FieldGetFunc(AArch64Fields::Fields field) {
    static const std::function<u32(AArch64Inst&)> functions[] = {
#define FIELD(name, from, to) \
    [] (AArch64Inst &inst) -> u32 { \
            return inst.name; \
    },
    #include "fields_table.inl"
#undef FIELD
    };
    return functions[static_cast<u8>(field)];
}

u32 Instruction::A64::GetAArch64Field(AArch64Fields::Fields type, AArch64Inst &inst) {
    return GetAArch64FieldGetFunc(type)(inst);
}
