/*
 * Copyright (C) 2009 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


#ifndef ANDROID_UI_EGLUTILS_H
#define ANDROID_UI_EGLUTILS_H

#include <utils/Errors.h>
#include <ui/PixelFormat.h>
#include <EGL/egl.h>

#include <stdint.h>

#ifndef _MSC_VER
#include <sys/types.h>
#endif

#ifndef _MSC_VER
#include <SDL2/SDL.h>
#endif

typedef struct  _ESContext
{
#ifdef _MSC_VER
        HWND hwnd;
#else
	SDL_Window *window = NULL;
#endif
	void* context;
	void (*drawFunc) ( void* );

	_ESContext():
#ifdef _MSC_VER
	hwnd(NULL),
#else
	window(NULL),
#endif
        context(NULL),
        drawFunc(NULL)
	{

	}
} ESContext;


//typedef int         status_t;
//typedef int32_t PixelFormat;

// ----------------------------------------------------------------------------
namespace android {
// ----------------------------------------------------------------------------

class EGLUtils
{
public:

    static const char *strerror(EGLint err);

    static status_t selectConfigForPixelFormat(
            EGLDisplay dpy,
            EGLint const* attrs,
            PixelFormat format,
            EGLConfig* outConfig);

    static status_t selectConfigForNativeWindow(
            EGLDisplay dpy,
            EGLint const* attrs,
            EGLNativeWindowType window,
            EGLConfig* outConfig);
};

// ----------------------------------------------------------------------------
}; // namespace android
// ----------------------------------------------------------------------------

#endif /* ANDROID_UI_EGLUTILS_H */
