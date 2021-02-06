#ifndef FRAMEBUFFER__H
#define FRAMEBUFFER__H

//#include <stdint.h>
#include <sys/types.h>

#include <EGL/egl.h>
//#include <ui/egl/android_natives.h>

#include "EGLUtils.h"

#ifndef _MSC_VER
#include "EsUtilSDL.h"
#include "FramebufferSDL.h"
#else
#include "EsUtilWin.h"
#include "FramebufferWin.h"
#endif

extern "C" EGLNativeWindowType createDisplaySurface(ESContext* ctx,  int width = 320, int height = 480, int format = 2);


#endif