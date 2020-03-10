//
// Created by swift on 2020/3/3.
//

#include <sys/mman.h>
#include "host_code_block.h"

using namespace Code;

bool BaseBlock::SaveToDisk(std::string path) {
    return false;
}

Buffer &BaseBlock::AllocCodeBuffer(VAddr source) {
    LockGuard lck(lock_);
    Buffer &buffer = buffers_[current_buffer_id_];
    buffer.id_ = current_buffer_id_;
    buffer.source_ = source;
    buffer.version_ = 1;
    buffers_map_[source] = current_buffer_id_;
    current_buffer_id_++;
    return buffer;
}

void BaseBlock::FlushCodeBuffer(Buffer &buffer, u32 size) {
    assert(size <= (UINT16_MAX << 2));
    buffer.size_ = static_cast<u16>(size >> 2);
    LockGuard lck(lock_);
    buffer.offset_ = current_offset_ += buffer.size_;
}

VAddr BaseBlock::GetBufferStart(Buffer &buffer) {
    return start_ + buffer.offset_ << 2;
}

VAddr BaseBlock::GetBufferStart(u16 id) {
    return GetBufferStart(GetBuffer(id));
}

VAddr BaseBlock::GetBufferEnd(Buffer &buffer) {
    return GetBufferStart(buffer) + buffer.size_ << 2;
}

BaseBlock::BaseBlock(VAddr start, VAddr size) : start_(start), size_(size) {

}

Buffer &BaseBlock::GetBuffer(u16 id) {
    return buffers_[id];
}

void BaseBlock::Align(u32 size) {
    LockGuard lck(lock_);
    current_offset_ = RoundUp(current_offset_, size);
}

u16 BaseBlock::GetCurrentId() const {
    return current_buffer_id_;
}

std::mutex &BaseBlock::Lock() {
    return lock_;
}

static VAddr MapCodeMemory(u32 size) {
    return reinterpret_cast<VAddr>(mmap(0, size, PROT_READ | PROT_WRITE | PROT_EXEC,
                                        MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED,
                                        -1, 0));
}

static void UnMapCodeMemory(VAddr start, u32 size) {
    munmap(reinterpret_cast<void *>(start), size);
}

A64::CodeBlock::CodeBlock(u32 forward_reg_rec_size, u32 block_size) : BaseBlock(
        MapCodeMemory(block_size), block_size), forward_reg_rec_size_(forward_reg_rec_size) {
    // init dispatcher table
    dispatcher_count_ = std::min<u32>(block_size >> 8, UINT16_MAX);
    dispatcher_table_ = reinterpret_cast<DispatcherTable *>(start_);
    current_offset_ = (sizeof(Dispatcher) * dispatcher_count_) >> 2;
}

A64::CodeBlock::~CodeBlock() {
    UnMapCodeMemory(start_, size_);
}

void A64::CodeBlock::GenDispatcher(Buffer &buffer) {
    assert(buffer.id_ < dispatcher_count_);
    auto delta = GetBufferStart(buffer) -
                 reinterpret_cast<VAddr>(&dispatcher_table_->dispatchers_[buffer.id_].go_without_pop_forward_);
    // B offset
    dispatcher_table_->dispatchers_[buffer.id_].go_without_pop_forward_ =
            0x14000000 | (0x03ffffff & (static_cast<u32>(delta) >> 2));
    dispatcher_table_->dispatchers_[buffer.id_].go_with_pop_forward_ =
            0x14000000 | (0x03ffffff & (static_cast<u32>(delta + forward_reg_rec_size_) >> 2));
}

VAddr A64::CodeBlock::GetDispatcherAddr(Buffer &buffer, bool with_pop_forward) {
    if (with_pop_forward) {
        return reinterpret_cast<VAddr>(&dispatcher_table_->dispatchers_[buffer.id_].go_with_pop_forward_);
    } else {
        return reinterpret_cast<VAddr>(&dispatcher_table_->dispatchers_[buffer.id_].go_without_pop_forward_);
    }
}

VAddr A64::CodeBlock::GetDispatcherOffset(Buffer &buffer, bool with_pop_forward) {
    return start_ - GetDispatcherAddr(buffer, with_pop_forward);
}

void A64::CodeBlock::FlushCodeBuffer(Buffer &buffer, u32 size) {
    BaseBlock::FlushCodeBuffer(buffer, size);
    GenDispatcher(buffer);
}
