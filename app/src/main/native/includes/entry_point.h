//
// Created by 甘尧 on 2019-09-25.
//

//Entry point for code blocks

#pragma once

namespace CodeCache {

    using EntryPoint = void*;
    using EntryPointFromJitToJit = void(*)();
    using EntryPointFromJitToInterpreter = void(*)();
    using EntryPointFromInterpreterToInterpreter = void(*)();

}
