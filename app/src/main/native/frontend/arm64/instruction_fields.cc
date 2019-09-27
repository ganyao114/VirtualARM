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
}

GeneralRegister::GeneralRegister(XReg xreg) : xreg_(xreg) {
    is_w_ = false;
}

GeneralRegister::GeneralRegister(WReg wreg) : wreg_(wreg) {
    is_w_ = true;
}
