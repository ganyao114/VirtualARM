//
// Created by 甘尧 on 2020-03-04.
//

#pragma once

#include <base/marcos.h>
#include <base/hash_table.h>

using namespace Base;

// 页表 + Hash 表
namespace Code {

    template <typename AddrType>
    class FindTable {
    public:

        FindTable(const u8 addr_width) : addr_width_(addr_width) {

        }

        void FillCodeAddress(AddrType vaddr, AddrType target);
    protected:
        const u8 addr_width_;
    };

}
