#ifndef ESUTIL_SDL_H
#define ESUTIL_SDL_H

#include <stdint.h>

#ifndef _MSC_VER
#include <sys/types.h>
#endif

#include <SDL2/SDL.h>

#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <sys/types.h>

#include "EGLUtils.h"

///
//  Includes
//

///#define ESCALLBACK  __cdecl

#ifdef __cplusplus

extern "C" {
#endif

EGLNativeWindowType SDL_CreateDisplaySurface(ESContext *ctx, int width = 320,
                                             int height = 480, int format = 2);

void SDL_WinLoop(ESContext *esContext);

#ifdef __cplusplus
}
#endif

#endif // ESUTIL_SDL_H