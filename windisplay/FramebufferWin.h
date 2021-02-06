/*
*	Add by Kevin.Wen
*	It supports displaying opengles's output buffer on windows
*/

#ifndef FRAMEBUFFER_WIN_H
#define FRAMEBUFFER_WIN_H

#include <stdint.h>
#include <sys/types.h>

#include <EGL/egl.h>
#include <ui/egl/android_natives.h>
#include "EGLUtils.h"
#include "EsUtilWin.h"

#define NUM_FRAME_BUFFERS  2

extern "C" EGLNativeWindowType win32_createDisplaySurface(ESContext* ctx,  int width = 320, int height = 480, int format = 2);
extern "C" EGLNativeWindowType win32_createDisplaySurfaceNew(HWND hwnd, int width, int height, int format);

// ---------------------------------------------------------------------------
namespace android {
// ---------------------------------------------------------------------------

class Surface;
class NativeBuffer;

// ---------------------------------------------------------------------------

class FramebufferWin32:
	public ANativeWindow
{
public:
    FramebufferWin32(HWND hwnd,	int width, int height, int format);
private:
    ~FramebufferWin32(); // this class cannot be overloaded
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

    HWND mHwnd;
    int mWindowWidth;
    int mWindowHeight;
    int mFormat;
    NativeBuffer* buffers[NUM_FRAME_BUFFERS];
    NativeBuffer* front;

	//for render
	HDC mHDC;
	HANDLE hloc;
	LPBITMAPINFO BmpInfo;
	void ShowImage(BYTE *lpImage);
};
    
// ---------------------------------------------------------------------------
}; // namespace android
// ---------------------------------------------------------------------------

#endif // FRAMEBUFFER_H

