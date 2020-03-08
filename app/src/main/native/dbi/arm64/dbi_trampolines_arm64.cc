//
// Created by 甘尧 on 2020-03-08.
//

#include "dbi_trampolines_arm64.h"
#include "dbi_context_arm64.h"

void DBI::A64::CodeCacheMissTrampoline() {
    auto context = Context::Current()->GetCPUContext();
    auto forward = context.forward;
}
