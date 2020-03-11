//
// Created by swift on 2020/3/1.
//

#include "dbi_mmu_arm64.h"

DBI::A64::PageTable::PageTable(u8 pageBits, u8 addrWidth, bool tlbPerThread) : MultiLevelPageTable(
        pageBits, addrWidth, tlbPerThread) {}

VAddr DBI::A64::PageTable::GetPageStart(DBI::A64::PTE &pte) {
    return pte.index_ << page_bits_;
}
