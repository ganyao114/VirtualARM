//
// Created by 甘尧 on 2019-09-25.
//

#include "cpu_arm64.h"

using namespace CPU::A64;

const VAddr OFFSET_CTX_A64_CPU_REG = OFFSET_OF(CPUContext, cpu_registers);
const VAddr OFFSET_CTX_A64_SP = OFFSET_OF(CPUContext, sp);
const VAddr OFFSET_CTX_A64_PC = OFFSET_OF(CPUContext, pc);
const VAddr OFFSET_CTX_A64_PSTATE = OFFSET_OF(CPUContext, pstate);
const VAddr OFFSET_CTX_A64_VEC_REG = OFFSET_OF(CPUContext, vector_registers);
const VAddr OFFSET_CTX_A64_FPCR = OFFSET_OF(CPUContext, fpcr);
const VAddr OFFSET_CTX_A64_FPSR = OFFSET_OF(CPUContext, fpsr);
const VAddr OFFSET_CTX_A64_TPIDR = OFFSET_OF(CPUContext, tpidr);
const VAddr OFFSET_CTX_A64_TPIDRRO = OFFSET_OF(CPUContext, tpidrro);
const VAddr OFFSET_CTX_A64_FORWARD = OFFSET_OF(CPUContext, forward);