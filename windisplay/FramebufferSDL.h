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
#include <SDL2/SDL.h>

#define NUM_FRAME_BUFFERS  2

extern "C" EGLNativeWindowType SDL_CreateDisplaySurface(ESContext* ctx,  int width = 320, int height = 480, int format = 2);

// ---------------------------------------------------------------------------
namespace android {
// ---------------------------------------------------------------------------

class Surface;
class NativeBuffer;

// ---------------------------------------------------------------------------

class FramebufferSDL:
	public ANativeWindow
{
public:
    FramebufferSDL(SDL_Window* window, int width, int height, int format);
private:
    ~FramebufferSDL(); // this class cannot be overloaded
    static int setSwapInterval(ANativeWindow* window, int interval);
    static int dequeueBuffer(ANativeWindow* window, ANativeWindowBuffer** buffer);
    static int lockBuffer(ANativeWindow* window, ANativeWindowBuffer* buffer);
    static int queueBuffer(ANativeWindow* window, ANativeWindowBuffer* buffer);
    static int query(const ANativeWindow* window, int what, int* value);
    static int perform(ANativeWindow* window, int operation, ...);
    
    int32_t mNumBuffers;
    int32_t mNumFreeBuffers;
    int32_t mBufferHead;
    int32_t mCurrentBufferIndex;
    bool mUpdateOnDemand;


    SDL_Window* mSDLWindow = NULL;
    SDL_Renderer* mSDLRenderer = NULL;
    SDL_Texture* mSDLTexture = NULL;
    SDL_Event mSDLEvent;
    SDL_Rect mSDLRect;

    int mWindowWidth;
    int mWindowHeight;
    int mFormat;
    NativeBuffer* buffers[NUM_FRAME_BUFFERS];
    NativeBuffer* front;

	void showImage(uint8_t* buffer);
};
    
// ---------------------------------------------------------------------------
}; // namespace android
// ---------------------------------------------------------------------------

#endif // FRAMEBUFFER_H

