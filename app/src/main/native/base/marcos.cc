//
// Created by 甘尧 on 2019-09-24.
//

#include "marcos.h"


int CountLeadingZerosFallBack(uint64_t value, int width) {
    assert(IsPowerOf2(width) && (width <= 64));
    if (value == 0) {
        return width;
    }
    int count = 0;
    value = value << (64 - width);
    if ((value & UINT64_C(0xffffffff00000000)) == 0) {
        count += 32;
        value = value << 32;
    }
    if ((value & UINT64_C(0xffff000000000000)) == 0) {
        count += 16;
        value = value << 16;
    }
    if ((value & UINT64_C(0xff00000000000000)) == 0) {
        count += 8;
        value = value << 8;
    }
    if ((value & UINT64_C(0xf000000000000000)) == 0) {
        count += 4;
        value = value << 4;
    }
    if ((value & UINT64_C(0xc000000000000000)) == 0) {
        count += 2;
        value = value << 2;
    }
    if ((value & UINT64_C(0x8000000000000000)) == 0) {
        count += 1;
    }
    count += (value == 0);
    return count;
}