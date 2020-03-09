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
const VAddr OFFSET_CTX_A64_QUERY_PAGE = OFFSET_OF(CPUContext, forward);
const VAddr OFFSET_CTX_A64_SVC_NUM = OFFSET_OF(CPUContext, forward);
const VAddr OFFSET_CTX_A64_TLB = OFFSET_OF(CPUContext, tlb);
const VAddr OFFSET_CTX_A64_PAGE_TABLE = OFFSET_OF(CPUContext, page_table);
const VAddr OFFSET_CTX_A64_SUSPEND_ADDR = OFFSET_OF(CPUContext, suspend_flag);
const VAddr OFFSET_CTX_A64_DISPATCHER_TABLE = OFFSET_OF(CPUContext, dispatcher_table);
const VAddr OFFSET_CTX_A64_TMP_LR = OFFSET_OF(CPUContext, tmp_lr);
const VAddr OFFSET_CTX_A64_HOST_SP = OFFSET_OF(CPUContext, host_sp);