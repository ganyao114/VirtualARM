//
// Created by swift on 2020/3/3.
//

#pragma once

#include <base/marcos.h>
#include <map>

namespace Code {

#define MAX_BUFFER UINT16_MAX
#define MAX_BLOCK_BIT 26

    struct BlockHeader {
        char magic_[4];
        u32 arch_;
        u32 version_;
        u32 timestamp_;
        u16 block_count_;
        u32 size_;
    };

    struct Buffer {
        u16 id_;
        struct {
            u32 offset_:24;
            u32 version_:8;
        };
        // 4 *
        u16 size_;
        VAddr source_;
    };

    class BaseBlock {
    public:

        BaseBlock(VAddr start, VAddr size);

        VAddr GetBufferStart(Buffer &buffer);
        VAddr GetBufferEnd(Buffer &buffer);

        Buffer &GetBuffer(u16 id);

        virtual Buffer &AllocCodeBuffer(VAddr source, u32 size);
        virtual bool SaveToDisk(std::string path);
    protected:
        VAddr start_;
        VAddr size_;
        std::mutex lock_;
        u16 current_buffer_id_{0};
        u32 current_offset_{0};
        std::array<Buffer, MAX_BUFFER> buffers_;
        std::map<VAddr, u16> buffers_map_;
    };

    namespace A64 {

        struct Dispatcher {
            // B label
            u32 instr_direct_branch_;
        };

        struct DispatcherTable {
            Dispatcher dispatchers_[MAX_BUFFER];
        };

        class CodeBlock : public BaseBlock {
        public:
            CodeBlock();
            virtual ~CodeBlock();

            void GenDispatcher(Buffer &buffer);

        protected:
            DispatcherTable *dispatcher_table_;
        };

    }

}
