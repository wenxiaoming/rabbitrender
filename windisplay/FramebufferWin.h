/*
 *	Add by Kevin.Wen
 *	It supports displaying opengles's output buffer on windows
 */

#ifndef FRAMEBUFFER_WIN_H
#define FRAMEBUFFER_WIN_H

#include <stdint.h>
#include <sys/types.h>

#include "EGLUtils.h"
#include "EsUtilWin.h"
#include <EGL/egl.h>
#include <ui/egl/android_natives.h>

#define NUM_FRAME_BUFFERS 2

extern "C" EGLNativeWindowType win32_createDisplaySurface(ESContext *ctx,
                                                          int width = 320,
                                                          int height = 480,
                                                          int format = 2);
extern "C" EGLNativeWindowType
win32_createDisplaySurfaceNew(HWND hwnd, int width, int height, int format);

// ---------------------------------------------------------------------------
namespace android {
// ---------------------------------------------------------------------------

class Surface;
class NativeBuffer;

// ---------------------------------------------------------------------------

class FramebufferWin32 : public Framebuffer {
  public:
    FramebufferWin32(HWND hwnd, int width, int height, int format);
    ~FramebufferWin32(); // this class cannot be overloaded

  private:
    static void wait(ANativeWindow *window, int32_t sec);
    static void showImage(ANativeWindow *window, uint8_t *buffer);

    HWND mHwnd;

    // for render
    HDC mHDC;
    HANDLE hloc;
    LPBITMAPINFO mBmpInfo;
    void ShowImage(BYTE *lpImage);
};

// ---------------------------------------------------------------------------
}; // namespace android
// ---------------------------------------------------------------------------

#endif // FRAMEBUFFER_H
