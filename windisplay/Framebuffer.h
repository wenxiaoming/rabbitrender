#ifndef FRAMEBUFFER__H
#define FRAMEBUFFER__H

//#include <stdint.h>
#include <stdint.h>
#include <sys/types.h>

#include <EGL/egl.h>
#include <ui/egl/android_natives.h>

#include "EGLUtils.h"

// ---------------------------------------------------------------------------
namespace android {
// ---------------------------------------------------------------------------
#define NUM_FRAME_BUFFERS  2

class Surface;
class NativeBuffer;

// ---------------------------------------------------------------------------

class Framebuffer: public ANativeWindow
{
public:
    Framebuffer(int32_t width, int32_t height, int32_t format);
    virtual ~Framebuffer();

public:
    int32_t getWindowWidth() const { return mWindowWidth; }
    int32_t getWindowHeight() const { return mWindowHeight; }

private:
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

    int32_t mWindowWidth;
    int32_t mWindowHeight;
    int32_t mFormat;
    NativeBuffer* buffers[NUM_FRAME_BUFFERS];
    NativeBuffer* front;
};

// ---------------------------------------------------------------------------
}; // namespace android
// ---------------------------------------------------------------------------


#endif