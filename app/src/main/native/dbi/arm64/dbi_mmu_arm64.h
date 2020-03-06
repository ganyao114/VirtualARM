//
// Created by swift on 2020/3/1.
//

#pragma once

#include <base/marcos.h>
#include "mmu/page_table.h"

using namespace MMU;

namespace DBI::A64 {

#define PAGE_BITS 12

    enum PageAttrs {
        Read        = 1 << 0,
        Write       = 1 << 1,
        Execute     = 1 << 2,
        WriteSpec   = 1 << 3
    };

    union PTE {
        VAddr target_;
        struct {
            VAddr index_:sizeof(VAddr) * 8 - PAGE_BITS;
            VAddr attrs_:PAGE_BITS;
        };
    };

    using PageTable = MultiLevelPageTable<VAddr, PTE>;

}
