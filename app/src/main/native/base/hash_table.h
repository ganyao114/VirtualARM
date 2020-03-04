//
// Created by 甘尧 on 2020-03-04.
//

#pragma once

#include "marcos.h"

#define CODE_CACHE_HASH_BITS 18
#define CODE_CACHE_HASH_SIZE 1 << (CODE_CACHE_HASH_BITS + 1) - 1//14071
#define CODE_CACHE_HASH_OVERP 10

/* Warning, size MUST be (a power of 2) */
#ifdef __arm__
#define GET_INDEX(key) ((key) & (size_ - CODE_CACHE_HASH_OVERP))
#endif
#ifdef __aarch64__
#define GET_INDEX(key) ((key >> 2) & (size_ - CODE_CACHE_HASH_OVERP))
#endif

namespace Utils {

    template <typename Key, typename Value>
    struct HashEntry {
        Key key_;
        Value value_;
    };

    // for code lookup
    template <typename Key, typename Value>
    class SimpleHashTable : public BaseObject {
    public:

        SimpleHashTable(int size);

        bool Add(Key key, Value value);
        void Remove(Key key);
        Value &Get(Key &key);

        constexpr HashEntry<Key, Value>** GetHashEntryPtr() {
            return &entries;
        }

    protected:
        int size_;
        int collisions_{0};
        int count_{0};
        HashEntry<Key, Value> entries[CODE_CACHE_HASH_SIZE + CODE_CACHE_HASH_OVERP];
    };


}
