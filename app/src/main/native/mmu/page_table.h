//
// Created by 甘尧 on 2020-03-04.
//

#pragma once

#include <base/marcos.h>
#include <vector>

namespace MMU {

    template <typename AddrType, u8 page_bits>
    class PageTableWithHostPageAlign {
    public:

        struct PageAddr : AddrType {
            AddrType page_start:sizeof(AddrType) * 8 - page_bits;
            AddrType page_attrs:page_bits;
        };

        PageTableWithHostPageAlign(u8 addr_width) : addr_width_(addr_width) {
            const std::size_t num_page_table_entries = 1ULL
                    << (addr_width_ - page_bits);
            pages_.resize(addr_width);
            pages_.shrink_to_fit();
        }

        VAddr TableAddr() {
            return pages_.data();
        }

    protected:
        u8 addr_width_;
        std::vector<PageAddr> pages_;
    };

    template <typename AddrType, typename AttrType>
    class FlatPageTable {
    public:

        FlatPageTable(u8 page_bits, u8 addr_width) : page_bits_(page_bits), addr_width_(addr_width) {
            const std::size_t num_page_table_entries = 1ULL
                    << (addr_width_ - page_bits);
            pages_.resize(num_page_table_entries);
            pages_.shrink_to_fit();
            attrs_.resize(num_page_table_entries);
            attrs_.shrink_to_fit();
        }

        const std::vector<AddrType> &GetPages() const {
            return pages_;
        }

        const std::vector<AttrType> &GetAttrs() const {
            return attrs_;
        }

    protected:
        const u8 addr_width_;
        const u8 page_bits_;
        std::vector<AddrType> pages_;
        std::vector<AttrType> attrs_;
    };

    template <typename AddrType, typename PTE>
    class MultiLevelPageTable {
    public:

        using Table = void*;
        using FinalTable = PTE*;

        MultiLevelPageTable(u8 page_bits, u8 addr_width) : page_bits_(page_bits), addr_width_(addr_width) {
            pte_bits_ = addr_width_ - page_bits_;
            if (pte_bits_ >= 48 && pte_bits_ % 3 == 0) {
                // 3级页表
                level_ = 3;
            } else {
                level_ = 2;
            }
            pte_bits_ >>= level_;
            pte_size_ = 1ULL << pte_bits_;
            pages_.resize(pte_size_);
        }

        u8 GetPteBits() const;

        u8 GetLevel() const;

        void MapPage(AddrType vaddr, PTE &pte);
        void UnMapPage(AddrType vaddr);

        VAddr TopPageTable() {
            return reinterpret_cast<VAddr>(pages_.data());
        }

    protected:
        const u8 addr_width_;
        const u8 page_bits_;
        u8 pte_bits_;
        std::size_t pte_size_;
        u8 level_;
        std::vector<Table*> pages_;
    };


}
