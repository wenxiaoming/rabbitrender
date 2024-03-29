#ifndef ESUTIL_WIN_H
#define ESUTIL_WIN_H

#include <EGL/egl.h>
#include <GLES2/gl2.h>

#include "EGLUtils.h"

#ifdef __cplusplus

extern "C" {
#endif

EGLNativeWindowType win32_createDisplaySurface(ESContext *ctx, int width,
                                               int height, int format);

void Windows_WinLoop(ESContext *esContext);

#ifdef __cplusplus
}
#endif

#endif // ESUTIL_WIN_H