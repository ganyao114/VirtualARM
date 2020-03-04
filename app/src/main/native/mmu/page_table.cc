//
// Created by 甘尧 on 2020-03-04.
//

#include "page_table.h"

using namespace MMU;

template<typename AddrType, typename PTE>
void MultiLevelPageTable<AddrType, PTE>::MapPage(AddrType vaddr, PTE &pte) {
    assert(vaddr % (2 << page_bits_) == 0);
    AddrType all_page_bits = BitRange<AddrType>(vaddr, page_bits_, sizeof(AddrType) - addr_width_ - 1);
    AddrType index;
    Table *table = reinterpret_cast<Table *>(pages_.data());
    for (int i = 0; i < level_; ++i) {
        index = BitRange<AddrType>(all_page_bits, pte_bits_ * (level_ - i - 1), pte_bits_ * (level_ - i) - 1);
        if (i < level_ - 2) {
            Table *tmp = &table[index];
            if (tmp == nullptr) {
                tmp = static_cast<Table *>(malloc(sizeof(Table) * pte_size_));
                (&table)[index] = tmp;
            }
            table = tmp;
        } else {
            //Final level
            FinalTable *final_table = table[index];
            if (final_table == nullptr) {
                final_table = malloc(sizeof(PTE) * pte_size_);
                (&table)[index] = final_table;
            }
            auto pte_index = BitRange<AddrType>(all_page_bits, 0, pte_bits_ - 1);
            (&final_table)[pte_index] = pte;
            break;
        }
    }
}

template<typename AddrType, typename PTE>
void MultiLevelPageTable<AddrType, PTE>::UnMapPage(AddrType vaddr) {
    assert(vaddr % (2 << page_bits_) == 0);
    AddrType all_page_bits = BitRange<AddrType>(vaddr, page_bits_, sizeof(AddrType) - addr_width_ - 1);
    AddrType index;
    Table *table = reinterpret_cast<Table *>(pages_.data());
    for (int i = 0; i < level_; ++i) {
        index = BitRange<AddrType>(all_page_bits, pte_bits_ * (level_ - i - 1), pte_bits_ * (level_ - i) - 1);
        if (i < level_ - 2) {
            Table *tmp = &table[index];
            if (tmp == nullptr) {
                break;
            }
            table = tmp;
        } else {
            //Final
            FinalTable *final_table = table[index];
            if (final_table == nullptr) {
                break;
            }
            auto pte_index = BitRange<AddrType>(all_page_bits, 0, pte_bits_ - 1);
            (&final_table)[pte_index] = nullptr;
            break;
        }
    }
}

template<typename AddrType, typename PTE>
u8 MultiLevelPageTable<AddrType, PTE>::GetPteBits() const {
    return pte_bits_;
}

template<typename AddrType, typename PTE>
u8 MultiLevelPageTable<AddrType, PTE>::GetLevel() const {
    return level_;
}

