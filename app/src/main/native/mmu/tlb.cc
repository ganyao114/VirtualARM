//
// Created by 甘尧 on 2020-03-06.
//

#include "tlb.h"

using namespace MMU;


template<typename AddrType, typename PTE>
TLB<AddrType, PTE>::TLB(const u8 page_bits, const u8 tlb_bits) : tlb_bits_(tlb_bits),
                                                                       page_bits_(page_bits) {
    tlb_table_.resize(1U << tlb_bits);
    tlb_table_.shrink_to_fit();
}

template<typename AddrType, typename PTE>
void TLB<AddrType, PTE>::CachePage(AddrType vaddr, PTE &pte) {
    auto index = BitRange<AddrType>(vaddr, page_bits_, page_bits_ + tlb_bits_ - 1);
    tlb_table_[index] = {vaddr >> page_bits_, pte};
}

template<typename AddrType, typename PTE>
void TLB<AddrType, PTE>::ClearPageCache(AddrType vaddr) {
    auto index = BitRange<AddrType>(vaddr, page_bits_, page_bits_ + tlb_bits_ - 1);
    tlb_table_[index].vaddr = {};
}



