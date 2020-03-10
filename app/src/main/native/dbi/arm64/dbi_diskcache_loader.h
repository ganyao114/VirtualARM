//
// Created by 甘尧 on 2020-03-10.
//

#pragma once

#include <base/marcos.h>

namespace DBI::A64 {
    class DiskCacheLoader {
    protected:
        void RewriteBrunchInstruction(VAddr origin_target, VAddr new_target, bool link = false);
    };
}
