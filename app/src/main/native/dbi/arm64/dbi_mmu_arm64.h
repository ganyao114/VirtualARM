//
// Created by swift on 2020/3/1.
//

#pragma once

#include <base/marcos.h>
#include "mmu/page_table.h"

using namespace MMU;

namespace DBI::A64 {

#define PAGE_BITS 12
#define WRITE_SPEC_BITS 3
#define READ_SPEC_BITS 4

    enum PageAttrs {
        Read        = 1 << 0,
        Write       = 1 << 1,
        Execute     = 1 << 2,
        WriteSpec   = 1 << WRITE_SPEC_BITS,
        ReadSpec    = 1 << READ_SPEC_BITS
    };

    union PTE {
        PTE() {
           target_ = 0;
        }
        VAddr target_;
        struct {
            VAddr attrs_:sizeof(VAddr) * 8 - PAGE_BITS;
            VAddr index_:PAGE_BITS;
        };
        bool operator==(const PTE& rhs) const {
            return rhs.target_ == target_;
        }
        bool operator!=(const PTE& rhs) const {
            return !operator==(rhs);
        }
    };

    class PageTable : public MultiLevelPageTable<VAddr, PTE> {
    public:
        PageTable(u8 pageBits, u8 addrWidth, bool tlbPerThread);
        VAddr GetPageStart(PTE &pte) override;
    };



}
