//
// Created by 甘尧 on 2020-03-04.
//

#pragma once

#define CODE_CACHE_HASH_SIZE 0x7FFFF//14071
#define CODE_CACHE_HASH_OVERP 10

/* Warning, size MUST be (a power of 2) */
#ifdef __arm__
#define GET_INDEX(key) ((key) & (table->size - CODE_CACHE_HASH_OVERP))
#endif
#ifdef __aarch64__
#define GET_INDEX(key) ((key >> 2) & (table->size - CODE_CACHE_HASH_OVERP))
#endif

namespace Base {

    // for code lookup
    template <typename Key, typename Value>
    class SimpleHashTable {
    public:
        typedef struct {
            Key key;
            Value value;
        } HashEntry;
    protected:
        int size;
        int collisions;
        int count;
        HashEntry entries[CODE_CACHE_HASH_SIZE + CODE_CACHE_HASH_OVERP];
    };

}
