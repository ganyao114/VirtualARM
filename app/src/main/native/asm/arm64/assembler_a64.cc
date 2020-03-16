//
// Created by 甘尧 on 2019-10-29.
//

#include "assembler_a64.h"
#include "instruction_aarch64.h"

using namespace Assembler::A64;
using namespace Instructions::A64;

void AssemblerA64::B_cond(Cond &a1, Offset &a2) {
    StoreRegImm<DataSize::U1, StoreImmSigned | StorePostIndex>();
}

template<DataSize size, u8 flags>
void AssemblerA64::StoreRegImm() {

}


