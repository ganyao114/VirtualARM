//
// Created by 甘尧 on 2020-03-08.
//

#include "dbi_trampolines_arm64.h"
#include "dbi_context_arm64.h"

void DBI::A64::CodeCacheDispatcherTrampoline() {
    auto context = Context::Current()->GetCPUContext();
    auto brunch_vaddr = context.forward;
}

void DBI::A64::PageLookupTrampoline() {
    auto context = Context::Current()->GetCPUContext();
    auto vaddr = context.forward;
}

void DBI::A64::CallSvcTrampoline() {
    auto context = Context::Current()->GetCPUContext();
    auto svc_num = context.forward;
}

void DBI::A64::SpecTrampoline() {
    auto context = Context::Current()->GetCPUContext();
    auto addr = context.forward;
}
