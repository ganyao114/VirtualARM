//
// Created by swift on 2020/3/1.
//

#include "dbi_mmu_arm64.h"

DBI::A64::A64MMU::A64MMU(u8 pageBits, u8 addrWidth, bool tlbPerThread) : MMU(
        pageBits, addrWidth, tlbPerThread) {}

VAddr DBI::A64::A64MMU::GetPageStart(DBI::A64::PTE &pte) {
    return pte.index_ << page_bits_;
}

bool DBI::A64::A64MMU::PageReadable(DBI::A64::PTE &pte) {
    return static_cast<bool>(pte.attrs_ & PageAttrs::Read);
}

bool DBI::A64::A64MMU::PageWritable(DBI::A64::PTE &pte) {
    return static_cast<bool>(pte.attrs_ & PageAttrs::Write);
}

void DBI::A64::A64MMU::HostReadCallback(VAddr host_addr, std::size_t size) {
}

void DBI::A64::A64MMU::HostWriteCallback(VAddr host_addr, std::size_t size) {
}

void DBI::A64::A64MMU::InvalidRead(VAddr vaddr, std::size_t size) {
}

void DBI::A64::A64MMU::InvalidWrite(VAddr vaddr, std::size_t size) {
    abort();
}
