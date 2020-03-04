//
// Created by 甘尧 on 2020-03-04.
//

#pragma once

#include <base/marcos.h>
#include <base/hash_table.h>
#include <vector>

using namespace Utils;

// 页表 + Hash 表
namespace Code {

    template <typename AddrType>
    class FindTable {
    public:

        constexpr static u8 redun_bits = 10;
        using Table = SimpleHashTable<AddrType, VAddr>;

        FindTable(const u8 addr_width) : addr_width_(addr_width) {
            table_count_ = 1U << (addr_width - CODE_CACHE_HASH_BITS - redun_bits);
            table_entries_.resize(table_count_);
            table_entries_.shrink_to_fit();
            tables_.resize(table_count_);
        }

        void FillCodeAddress(AddrType vaddr, VAddr target);

    protected:
        const u8 addr_width_;
        u32 table_count_;
        std::vector<HashEntry<AddrType, VAddr>**> table_entries_;
        std::vector<SharedPtr<Table>> tables_;
    };

}
