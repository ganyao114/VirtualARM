//
// Created by swift on 2020/3/1.
//

#pragma once

#include <base/marcos.h>
#include "memory/mmu.h"

using namespace Memory;

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

    class A64MMU : public MMU<VAddr, PTE> {
    public:
        A64MMU(u8 pageBits, u8 addrWidth, bool tlbPerThread);
        VAddr GetPageStart(PTE &pte) override;

        bool PageReadable(PTE &pte) override;

        bool PageWritable(PTE &pte) override;

        void HostReadCallback(VAddr host_addr, std::size_t size) override;

        void HostWriteCallback(VAddr host_addr, std::size_t size) override;

        void InvalidRead(VAddr vaddr, std::size_t size) override;

        void InvalidWrite(VAddr vaddr, std::size_t size) override;
    };



}
