cmake_minimum_required(VERSION 3.4.1)
project(VirtualARM)

set(CMAKE_CXX11_EXTENSION_COMPILE_OPTION "-std=c++17")
ENABLE_LANGUAGE(ASM)

include_directories(.)

include_directories(externals/ext-boost)

add_subdirectory(externals/vixl)
include_directories(externals/vixl)

add_library(virtual_arm SHARED
        virtual_arm.cc
        base/marcos.cc
        base/hash_table.cc
        instruction/instruction.cc
        asm/arm64/instruction_fields.cc
        asm/arm64/instruction_aarch64.cc
        asm/arm64/instruction_table.cc
        asm/arm64/instruction_decode.cc
        asm/arm64/decodes/decode_system_branch.cc
        asm/arm64/decodes/decode_data_process_imm.cc
        asm/arm64/decodes/decode_load_and_store.cc
        asm/arm64/cpu_arm64.cc
        asm/arm64/visitors/decode_visitors.cc
        frontend/ir/opcode_ir.cc
        frontend/ir/argument_ir.cc
        frontend/ir/instruction_ir.cc
        frontend/ir/block_ir.cc
        frontend/ir/assembler_ir.cc
        frontend/arm64/ir_for_a64.cc
        asm/arm64/assembler_a64.cc
        backend/arm64/trampoline_a64.cc
        backend/arm64/trampoline.S
        backend/ir/interpreter_ir.cc
        block/code_block.cc
        dbi/arm64/dbi_arm64.cc
        dbi/arm64/dbi_jit_arm64.cc
        dbi/arm64/dbi_context_arm64.cc
        dbi/arm64/dbi_mmu_arm64.cpp
        dbi/arm64/dbi_trampolines_arm64.cc
        dbi/arm64/dbi_diskcache_loader.cc
        dbi/arm64/context_new.cc
        block/host_code_block.cpp
        block/code_find_table.cc
        memory/mmu.cc
        memory/tlb.cc
)


target_link_libraries(virtual_arm android log)
target_link_libraries(virtual_arm vixl)