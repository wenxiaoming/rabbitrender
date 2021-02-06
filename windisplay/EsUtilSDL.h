#ifndef ESUTIL_SDL_H
#define ESUTIL_SDL_H

#include <stdint.h>

#ifndef _MSC_VER
#include <sys/types.h>
#endif

#include <SDL2/SDL.h>

#include <sys/types.h>
#include <GLES2/gl2.h>
#include <EGL/egl.h>

#include "EGLUtils.h"

///
//  Includes
//

///#define ESCALLBACK  __cdecl

#ifdef __cplusplus

extern "C" {
#endif


SDL_Window* SDLWinowCreate ( const char *title, int width, int height, ESContext* context);


void WinLoop ( ESContext *esContext );

#ifdef __cplusplus
}
#endif

#endif // ESUTIL_SDL_H