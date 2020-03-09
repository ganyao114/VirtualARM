//
// Created by 甘尧 on 2020-03-08.
//

#pragma once

namespace DBI::A64 {
    extern "C" void CodeCacheDispatcherTrampoline();
    extern "C" void PageMissTrampoline();
    extern "C" void CallSvcTrampoline();
    extern "C" void SpecTrampoline();
}
