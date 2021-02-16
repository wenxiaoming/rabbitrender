/*
*	Add by Kevin.Wen
*	It supports displaying opengles's output buffer with sdl
*/

#ifndef FRAMEBUFFER_SDL_H
#define FRAMEBUFFER_SDL_H

#include <stdint.h>
#include <sys/types.h>

#include <EGL/egl.h>
#include <ui/egl/android_natives.h>
#include "EGLUtils.h"
#include "EsUtilSDL.h"
#include "Framebuffer.h"
#include <SDL2/SDL.h>

// ---------------------------------------------------------------------------
namespace android {
// ---------------------------------------------------------------------------

class Surface;
class NativeBuffer;

// ---------------------------------------------------------------------------
class FramebufferSDL: public Framebuffer
{
public:
    FramebufferSDL(SDL_Window* window, int width, int height, int format);
    virtual ~FramebufferSDL();

private:
    static void wait(ANativeWindow* window, int32_t sec);
    static void showImage(ANativeWindow* window, uint8_t* buffer);
    
    SDL_Window* mSDLWindow = NULL;
    SDL_Renderer* mSDLRenderer = NULL;
    SDL_Texture* mSDLTexture = NULL;
    SDL_Event mSDLEvent;
    SDL_Rect mSDLRect;
};
    
// ---------------------------------------------------------------------------
}; // namespace android
// ---------------------------------------------------------------------------

#endif // FRAMEBUFFER_H

