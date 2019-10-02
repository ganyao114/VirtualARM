//
// Created by 甘尧 on 2019-09-20.
//

#include <jni.h>
#include "virtual_arm.h"
#include "frontend/arm64/instruction_decode.h"

using namespace Instruction::A64;

extern "C"
JNIEXPORT void JNICALL
load_test(JNIEnv *env, jobject instance) {
    FastBranchDecoder decoder;
    InstrA64Ref instr = decoder.Decode(reinterpret_cast<InstrA64 *>((VAddr)rename + 24));
}

static bool registerNativeMethods(JNIEnv *env, const char *className, JNINativeMethod *jniMethods, int methods) {
    jclass clazz = env->FindClass(className);
    if (clazz == nullptr) {
        return false;
    }
    return env->RegisterNatives(clazz, jniMethods, methods) >= 0;
}

static JNINativeMethod jniMethods[] = {
        {
                "launch",
                "()V",
                (void *) load_test
        }
};

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved) {

    const char* CLASS_VIRTUAL_ARM = "com/swift/virtualarm/VirtualARM";

    int jniMethodSize = sizeof(JNINativeMethod);

    JNIEnv *env = nullptr;

    if (vm->GetEnv((void **) &env, JNI_VERSION_1_6) != JNI_OK) {
        return -1;
    }

    if (!registerNativeMethods(env, CLASS_VIRTUAL_ARM, jniMethods, sizeof(jniMethods) / jniMethodSize)) {
        return -1;
    }

    return JNI_VERSION_1_6;
}

