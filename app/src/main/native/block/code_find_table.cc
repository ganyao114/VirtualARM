//
// Created by 甘尧 on 2020-03-04.
//

#include "code_find_table.h"

using namespace Code;

template<typename AddrType>
void FindTable<AddrType>::FillCodeAddress(AddrType vaddr, VAddr target) {
    AddrType index = BitRange<AddrType>(vaddr >> align_bits_, CODE_CACHE_HASH_BITS + redun_bits, sizeof(AddrType) - addr_width_ - 1);
    SharedPtr<Table> table = tables_[index];
    if (table == nullptr) {
        table = SharedPtr<Table>(new Table(addr_width_));
        tables_[index] = table;
        table_entries_[index] = table->GetHashEntryPtr();
    }
    table->Add(vaddr, target);
}