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

GeneralRegister::GeneralRegister() {
    type_ = Type::U;
}

GeneralRegister::GeneralRegister(XReg xreg) : xreg_(xreg) {
    type_ = Type::X;
}

GeneralRegister::GeneralRegister(WReg wreg) : wreg_(wreg) {
    type_ = Type::W;
}

GeneralRegister::GeneralRegister(VReg vreg) : vreg_(vreg) {
    type_ = Type::V;
}

GeneralRegister::GeneralRegister(QReg qreg) : qreg_(qreg) {
    type_ = Type::Q;
}

GeneralRegister::GeneralRegister(DReg dreg) : dreg_(dreg) {
    type_ = Type::D;
}

GeneralRegister GeneralRegister::X(u32 data_size, u8 code) {
    if (data_size <= 32) {
        return WREG(code);
    } else {
        return XREG(code);
    }
}

GeneralRegister GeneralRegister::V(u32 data_size, u8 code) {
    if (data_size <= 32) {
        return QREG(code);
    } else if (data_size <= 64) {
        return DREG(code);
    } else {
        return VREG(code);
    }
}
