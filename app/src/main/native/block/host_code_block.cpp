//
// Created by swift on 2020/3/3.
//

#include <sys/mman.h>
#include "host_code_block.h"

using namespace Code;

bool BaseBlock::SaveToDisk(std::string path) {
    return false;
}

Buffer &BaseBlock::AllocCodeBuffer(VAddr source, u32 size) {
    assert(size <= UINT16_MAX << 2);
    LockGuard lck(lock_);
    Buffer &buffer = buffers_[current_buffer_id_];
    buffer.id_ = current_buffer_id_;
    buffer.size_ = static_cast<u16>(size >> 2);
    buffer.source_= source;
    buffer.version_ = 1;
    buffer.offset_ = current_offset_ += buffer.size_;
    buffers_map_[source] = current_buffer_id_;
    current_buffer_id_ ++;
    return buffer;
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

static VAddr MapCodeMemory(u32 size) {
    return reinterpret_cast<VAddr>(mmap(0, size, PROT_READ | PROT_WRITE | PROT_EXEC,
                                        MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED,
                                        -1, 0));
}

static void UnMapCodeMemory(VAddr start, u32 size) {
    munmap(reinterpret_cast<void *>(start), size);
}

A64::CodeBlock::CodeBlock(u32 block_size) : BaseBlock(MapCodeMemory(block_size), block_size) {
    // init dispatcher table
    dispatcher_count_ = std::min<u32>(block_size >> 8, UINT16_MAX);
    dispatcher_table_ = reinterpret_cast<DispatcherTable *>(start_);
    current_offset_ = sizeof(DispatcherTable) >> 2;
}

A64::CodeBlock::~CodeBlock() {
    UnMapCodeMemory(start_, size_);
}

void A64::CodeBlock::GenDispatcher(Buffer &buffer) {
    assert(buffer.id_ < dispatcher_count_);
    auto delta = GetBufferStart(buffer) - reinterpret_cast<VAddr>(&dispatcher_table_->dispatchers_[buffer.id_].instr_direct_branch_);
    // B offset
    dispatcher_table_->dispatchers_[buffer.id_].instr_direct_branch_ = 0x14000000 | (0x03ffffff & (static_cast<u32>(delta) >> 2));
}

VAddr A64::CodeBlock::GetDispatcherAddr(Buffer &buffer) {
    return reinterpret_cast<VAddr>(&dispatcher_table_->dispatchers_[buffer.id_].instr_direct_branch_);
}

Buffer &A64::CodeBlock::AllocCodeBuffer(VAddr source, u32 size) {
    auto buffer = BaseBlock::AllocCodeBuffer(source, size);
    GenDispatcher(buffer);
    return buffer;
}
