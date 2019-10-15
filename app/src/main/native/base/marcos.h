//
// Created by 甘尧 on 2019-09-24.
//

#pragma once

#include <cstdint>
#include <mutex>
#include <array>
#include "compiler.h"

using u8 = std::uint8_t;   ///< 8-bit unsigned byte
using u16 = std::uint16_t; ///< 16-bit unsigned short
using u32 = std::uint32_t; ///< 32-bit unsigned word
using u64 = std::uint64_t; ///< 64-bit unsigned int
using u128 = std::array<std::uint64_t, 2>;

using s8 = std::int8_t;   ///< 8-bit signed byte
using s16 = std::int16_t; ///< 16-bit signed short
using s32 = std::int32_t; ///< 32-bit signed word
using s64 = std::int64_t; ///< 64-bit signed int

using f32 = float;  ///< 32-bit floating point
using f64 = double; ///< 64-bit floating point

using VAddr = std::size_t;    ///< Represents a pointer in the userspace virtual address space.

using LockGuard = std::lock_guard<std::mutex>;

struct Base {
};

#define CONCAT2(x, y) DO_CONCAT2(x, y)
#define DO_CONCAT2(x, y) x##y

#define FORCE_INLINE inline __attribute__((always_inline))

#define BitField32(name, offset) u32 name: offset;
#define BitField16(name, offset) u16 name: offset;

template<typename U, typename T>
U ForceCast(T *x) {
    return (U) (uintptr_t) x;
}

template<typename U, typename T>
U ForceCast(T &x) {
    return *(U *) &x;
}

template<typename T>
struct Identity {
    using type = T;
};


template<typename T>
constexpr T RoundDown(T x, typename Identity<T>::type n) {
    return (x & -n);
}

template<typename T>
constexpr T RoundUp(T x, typename std::remove_reference<T>::type n) {
    return RoundDown(x + n - 1, n);
}


namespace pointer {

    template<typename T = void *>
    constexpr intptr_t as_intptr(T in) {
        return reinterpret_cast<intptr_t>(in);
    }

    template<typename R, typename T>
    constexpr R as_pointer(T in) {
        return reinterpret_cast<R>(in);
    }


};  // end namespace pinter

template<typename T>
constexpr bool TestBit(T val, size_t pos) {
    return (val & (static_cast<T>(1) << pos)) != 0;
}

template<size_t pos, typename T>
constexpr bool TestBit(T val) {
    return (val & (static_cast<T>(1) << (pos))) != 0;
}

template<typename T>
constexpr T Bitmask(T size) {
    return (size == sizeof(T) * 8)? std::numeric_limits<T>::max() : (static_cast<T>(1) << size) - 1;
}

template<size_t size, typename T>
constexpr T Bitmask() {
    return (static_cast<T>(1) << (size)) - 1;
}

template<>
constexpr uint32_t Bitmask<32, uint32_t>() {
    return 0xFFFFFFFF;
}

template<>
constexpr uint64_t Bitmask<64, uint64_t>() {
    return 0xFFFFFFFFFFFFFFFF;
}

template<typename T>
constexpr T BitRange(T val, T start, T end) {
    return (val >> start) & Bitmask(end - start + 1);
}

template<size_t start, size_t end, typename T>
constexpr T BitRange(T val) {
    return (val >> start) & Bitmask<end - start + 1, T>();
}

template <typename T>
constexpr T SignExtendX(u8 size, T val) {
    T mask = (T(2) << (size - 1)) - T(1);
    val &= mask;
    T sign_bits = -((val >> (size - 1)) << size);
    val |= sign_bits;
    return val;
}

template <typename T, u8 size>
constexpr T SignExtend(T val) {
    T mask = (T(2) << (size - 1)) - T(1);
    val &= mask;
    T sign_bits = -((val >> (size - 1)) << size);
    val |= sign_bits;
    return val;
}

constexpr u64 TruncateToUintN(unsigned n, u64 x) {
    return static_cast<u64>(x) & ((UINT64_C(1) << n) - 1);
}

template<unsigned size>
constexpr u32 TruncateUTo(u64 val) {
    return static_cast<u32>(TruncateToUintN(size, val));
}

template<unsigned size>
constexpr u32 TruncateSTo(s64 val) {
    return static_cast<u32>(TruncateToUintN(size, val));
}

template<typename T>
constexpr T ConstLog2(T num) {
    return (num == 1)? 0 : 1 + ConstLog2(num >> 1);
}

constexpr u64 LowestSetBit(u64 value) { return value & -value; }

template <typename V>
constexpr bool IsPowerOf2(V value) {
    return (value != 0) && ((value & (value - 1)) == 0);
}

int CountLeadingZerosFallBack(uint64_t value, int width);

template <typename V>
constexpr int CountLeadingZeros(V value, int width = (sizeof(V) * 8)) {
#if COMPILER_HAS_BUILTIN_CLZ
    if (width == 32) {
    return (value == 0) ? 32 : __builtin_clz(static_cast<unsigned>(value));
  } else if (width == 64) {
    return (value == 0) ? 64 : __builtin_clzll(value);
  }
#endif
    return CountLeadingZerosFallBack(value, width);
}

constexpr u64 RotateRight(u64 value,
                            unsigned int rotate,
                            unsigned int width) {
    assert((width > 0) && (width <= 64));
    u64 width_mask = ~UINT64_C(0) >> (64 - width);
    rotate &= 63;
    if (rotate > 0) {
        value &= width_mask;
        value = (value << (width - rotate)) | (value >> rotate);
    }
    return value & width_mask;
}


//share ptr

#include <boost/smart_ptr/intrusive_ptr.hpp>
template<typename T>
using SharedPtr = boost::intrusive_ptr<T>;

class BaseObject {
private:

    friend void intrusive_ptr_add_ref(BaseObject *);

    friend void intrusive_ptr_release(BaseObject *);

    std::atomic<u32> ref_count{0};
    std::atomic<u32> object_id{0};
};

template<typename T>
inline SharedPtr<T> DynamicObjectCast(SharedPtr<BaseObject> object) {
    if (object != nullptr) {
        return boost::static_pointer_cast<T>(object);
    }
    return nullptr;
}

inline void intrusive_ptr_add_ref(BaseObject *object) {
    object->ref_count.fetch_add(1, std::memory_order_relaxed);
}

inline void intrusive_ptr_release(BaseObject *object) {
    if (object->ref_count.fetch_sub(1, std::memory_order_acq_rel) == 1) {
        delete object;
    }
}

using ObjectRef = SharedPtr<BaseObject>;

#define ARG_LIST(...) __VA_ARGS__