//
// Created by 甘尧 on 2020-03-06.
//

#pragma once

// Translation Lookaside Buffer
#include <base/marcos.h>
#include <vector>

namespace MMU {

    template <typename AddrType, typename PTE>
    struct TLBEntry {
        AddrType page_index_;
        PTE pte_;
    };

    template <typename AddrType, typename PTE>
    class TLB : public BaseObject {
    public:
        TLB(const u8 page_bits, const u8 tlb_bits);

        void CachePage(AddrType vaddr, PTE &pte);
        void ClearPageCache(AddrType vaddr);

        VAddr TLBTablePtr() {
            return tlb_table_.data();
        }

    protected:
        const u8 tlb_bits_;
        const u8 page_bits_;
        std::vector<TLBEntry<AddrType, PTE>> tlb_table_;
    };

}
