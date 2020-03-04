//
// Created by 甘尧 on 2020-03-04.
//

#include <cstdlib>
#include "hash_table.h"

using namespace Utils;

template<typename Key, typename Value>
bool SimpleHashTable<Key, Value>::Add(Key key, Value value) {
    int index = GET_INDEX(key);
    int prev_index;
    bool done = false;

    do {
        if (entries[index].key_ == 0 || entries[index].key_ == key) {
            if (entries[index].key_ == 0) {
                count_++;
            }
            entries[index].key_ = key;
            entries[index].value_ = value;
            done = true;
        } else {
            prev_index = index;
            index++;
            if (index >= size_ -1) {
                abort();
            }
            collisions_++;
        }
    } while(!done && index < (size_ - 1));

    return done;
}

template<typename Key, typename Value>
SimpleHashTable<Key, Value>::SimpleHashTable(int size) : size_(size) {
    for (int i = size-1; i >= 0; i--) {
        entries[i].key_ = 0;
    }
}

template<typename Key, typename Value>
Value &SimpleHashTable<Key, Value>::Get(Key &key) {
    int index = GET_INDEX(key);
    bool found = false;
    Value entry;
    Key c_key;

    do {
        c_key = entries[index].key_;
        if (c_key == key) {
            entry = entries[index].value_;
            found = true;
        } else {
            index++;
        }
    } while(!found && index < (size_ - 1) && c_key != 0);

    return entry;
}

template<typename Key, typename Value>
void SimpleHashTable<Key, Value>::Remove(Key key) {
    int index = GET_INDEX(key);
    int end = index - 1;
    bool found = false;
    Key c_key;

    do {
        c_key = entries[index].key_;
        if (c_key == key) {
            entries[index].key_ = 0;
            found = true;
        } else {
            index = (index + 1) & size_;
        }
    } while(!found && index != end && c_key != 0);
}
