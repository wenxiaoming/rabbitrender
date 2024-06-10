// Copyright 2013 The Flutter Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FLUTTER_FML_PLATFORM_ANDROID_JNI_UTIL_H_
#define FLUTTER_FML_PLATFORM_ANDROID_JNI_UTIL_H_

#include <jni.h>

#include <vector>

#include "macros.h"
#include "platform/android/scoped_java_ref.h"

namespace fml {
namespace jni {

void InitJavaVM(JavaVM* vm);

// Returns a JNI environment for the current thread.
// Attaches the thread to JNI if needed.
JNIEnv* AttachCurrentThread();

void DetachFromVM();

std::string JavaStringToString(JNIEnv* env, jstring string);

ScopedJavaLocalRef<jstring> StringToJavaString(JNIEnv* env,
                                               const std::string& str);

std::vector<std::string> StringArrayToVector(JNIEnv* env, jobjectArray jargs);

ScopedJavaLocalRef<jobjectArray> VectorToStringArray(
    JNIEnv* env,
    const std::vector<std::string>& vector);

bool HasException(JNIEnv* env);

bool ClearException(JNIEnv* env);

std::string GetJavaExceptionInfo(JNIEnv* env, jthrowable java_throwable);

}  // namespace jni
}  // namespace fml

#endif  // FLUTTER_FML_PLATFORM_ANDROID_JNI_UTIL_H_