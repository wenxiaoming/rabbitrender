#include "Framebuffer.h"

using namespace android;
EGLNativeWindowType createDisplaySurface(ESContext* ctx,  int width, int height, int format)
{
#ifndef _MSC_VER
    return SDL_CreateDisplaySurface(ctx, width, height, format);
#else
    return win32_createDisplaySurface(ctx, width, height, format);
#endif
}
