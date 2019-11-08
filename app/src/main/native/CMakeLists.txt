cmake_minimum_required(VERSION 3.4.1)
project(VirtualARM)

set(CMAKE_CXX11_EXTENSION_COMPILE_OPTION "-std=c++17")
ENABLE_LANGUAGE(ASM)

include_directories(.)

include_directories(externals/ext-boost)

add_library(virtual_arm SHARED
        virtual_arm.cc
        base/marcos.cc
        instruction/instruction.cc
        asm/arm64/instruction_fields.cc
        asm/arm64/instruction_aarch64.cc
        asm/arm64/instruction_table.cc
        asm/arm64/instruction_decode.cc
        asm/arm64/decodes/decode_system_branch.cc
        asm/arm64/decodes/decode_data_process_imm.cc
        asm/arm64/decodes/decode_load_and_store.cc
        asm/arm64/cpu_arm64.cc
        frontend/ir/opcode_ir.cc
        frontend/ir/argument_ir.cc
        frontend/ir/instruction_ir.cc
        frontend/ir/block_ir.cc
        frontend/ir/assembler_ir.cc
        frontend/arm64/ir_adapter_a64.cc
        asm/arm64/assembler_a64.cc
        backend/arm64/trampoline_a64.cc
        backend/arm64/trampoline.S
        backend/ir/interpreter_ir.cc
        block/code_block.cc
)


target_link_libraries(virtual_arm android log)