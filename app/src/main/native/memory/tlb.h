//
// Created by 甘尧 on 2020-03-06.
//

#pragma once

// Translation Lookaside Buffer
#include <base/marcos.h>
#include <vector>

namespace Memory {

    template <typename AddrType, typename PTE>
    struct TLBEntry {
        AddrType page_index_{};
        PTE pte_{};
    };

    template <typename AddrType, typename PTE>
    class TLB : public BaseObject {
    public:

        TLB(const u8 page_bits, const u8 tlb_bits) : tlb_bits_(tlb_bits), page_bits_(page_bits) {
            tlb_table_.resize(1U << tlb_bits);
            tlb_table_.shrink_to_fit();
        }

        void CachePage(AddrType vaddr, PTE &pte) {
            assert(vaddr % (1 << page_bits_) == 0);
            auto index = BitRange<AddrType>(vaddr, page_bits_, page_bits_ + tlb_bits_ - 1);
            tlb_table_[index] = {vaddr >> page_bits_, pte};
        }

        PTE GetPage(AddrType vaddr) {
            assert(vaddr % (1 << page_bits_) == 0);
            auto index = BitRange<AddrType>(vaddr, page_bits_, page_bits_ + tlb_bits_ - 1);
            auto &tlb_entry = tlb_table_[index];
            if (tlb_entry.page_index_ == vaddr >> page_bits_) {
                return tlb_entry.pte_;
            } else {
                return {};
            }
        }

        void ClearPageCache(AddrType vaddr) {
            assert(vaddr % (1 << page_bits_) == 0);
            auto index = BitRange<AddrType>(vaddr, page_bits_, page_bits_ + tlb_bits_ - 1);
            tlb_table_[index] = {};
        }

        VAddr TLBTablePtr() {
            return reinterpret_cast<VAddr>(tlb_table_.data());
        }

        u8 TLBBits() const {
            return tlb_bits_;
        }

    protected:
        const u8 tlb_bits_;
        const u8 page_bits_;
        std::vector<TLBEntry<AddrType, PTE>> tlb_table_;
    };

}
