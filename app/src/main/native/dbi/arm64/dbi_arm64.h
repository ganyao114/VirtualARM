//
// Created by 甘尧 on 2020-02-29.
//
#pragma once

#include <base/marcos.h>

namespace DBI::A64 {

    class CallBack {
        virtual void OnSpecWrite64(VAddr addr) {};
        virtual void OnSpecWrite32(VAddr addr) {};
        virtual void OnSpecWrite16(VAddr addr) {};
        virtual void OnSpecWrite8(VAddr addr) {};
    };

}
