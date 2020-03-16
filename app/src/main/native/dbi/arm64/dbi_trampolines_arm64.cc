//
// Created by 甘尧 on 2020-03-08.
//

#include "dbi_trampolines_arm64.h"
#include "dbi_context_arm64.h"

void DBI::A64::CodeCacheDispatcherTrampoline() {
    auto context = Context::Current()->GetCPUContext();
    auto brunch_vaddr = context.forward;
}

void DBI::A64::PageMissTrampoline() {
    auto context = Context::Current()->GetCPUContext();
    auto vaddr = context.forward;
}

void DBI::A64::ContextSwitchTrampoline() {
    auto context = Context::Current()->GetCPUContext();
    auto action = context.forward;
    auto data = context.forward_ext;
    switch (action) {

    }
}

void DBI::A64::SpecTrampoline() {
    auto context = Context::Current()->GetCPUContext();
    auto addr = context.forward;
}
