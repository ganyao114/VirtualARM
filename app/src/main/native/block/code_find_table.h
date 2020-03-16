//
// Created by 甘尧 on 2020-03-04.
//

#pragma once

#include <base/marcos.h>
#include <base/hash_table.h>
#include <vector>

using namespace Utils;

// 页表 + Hash 表
namespace CodeCache {

    template <typename AddrType>
    class FindTable : public BaseObject {
    public:

        constexpr static u8 redun_bits = 10;
        using Table = SimpleHashTable<AddrType, VAddr>;

        FindTable(const u8 addr_width, const u8 align_bits = 0) : addr_width_(addr_width), align_bits_(align_bits) {
            table_bits_ = static_cast<u8>(addr_width - CODE_CACHE_HASH_BITS - redun_bits);
            table_count_ = 1U << table_bits_;
            table_entries_.resize(table_count_);
            table_entries_.shrink_to_fit();
            tables_.resize(table_count_);
        }

        void FillCodeAddress(AddrType vaddr, VAddr target) {
            AddrType index = BitRange<AddrType>(vaddr >> align_bits_, CODE_CACHE_HASH_BITS + redun_bits, sizeof(AddrType) - addr_width_ - 1);
            LockGuard guard(lock_);
            SharedPtr<Table> table = tables_[index];
            if (table == nullptr) {
                table = SharedPtr<Table>(new Table(addr_width_));
                tables_[index] = table;
                table_entries_[index] = table->GetHashEntryPtr();
            }
            table->Add(vaddr, target);
        }

        VAddr TableEntryPtr() {
            return reinterpret_cast<VAddr>(table_entries_.data());
        }

        u8 TableBits() {
            return table_bits_;
        }

    protected:
        const u8 addr_width_;
        u8 table_bits_;
        u32 table_count_;
        u8 align_bits_;
        std::vector<HashEntry<AddrType, VAddr>*> table_entries_;
        std::vector<SharedPtr<Table>> tables_;
        std::mutex lock_;
    };

}
