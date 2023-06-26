/*
 *	Add by Kevin.Wen
 *	It supports displaying opengles's output buffer on windows
 */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "FramebufferWin.h"
#include <EGL/egl.h>

// ----------------------------------------------------------------------------
namespace android {
// ----------------------------------------------------------------------------

FramebufferWin32::FramebufferWin32(HWND hwnd, int width, int height,
                                   int format)
    : mHwnd(hwnd), Framebuffer(width, height, format) {
    if (IsWindow(hwnd)) {
        RECT rect;
        GetWindowRect(hwnd, &rect);

        mHDC = GetDC(mHwnd);

        hloc = GlobalAlloc(GMEM_ZEROINIT | GMEM_MOVEABLE,
                           sizeof(BITMAPINFOHEADER) + (sizeof(RGBQUAD) * 256));
        mBmpInfo = (LPBITMAPINFO)GlobalLock(hloc);

        // init mBmpInfo
        int i;
        HANDLE hloc1;
        RGBQUAD *argbq;

        hloc1 =
            LocalAlloc(LMEM_ZEROINIT | LMEM_MOVEABLE, (sizeof(RGBQUAD) * 256));
        argbq = (RGBQUAD *)LocalLock(hloc1);

        for (i = 0; i < 256; i++) {
            argbq[i].rgbBlue = i;
            argbq[i].rgbGreen = i;
            argbq[i].rgbRed = i;
            argbq[i].rgbReserved = 0;
        }

        mBmpInfo->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        mBmpInfo->bmiHeader.biPlanes = 1;

        mBmpInfo->bmiHeader.biBitCount = 32;
        mBmpInfo->bmiHeader.biCompression = BI_RGB;
        mBmpInfo->bmiHeader.biWidth = width;
        mBmpInfo->bmiHeader.biHeight = height;

        memcpy(mBmpInfo->bmiColors, argbq, sizeof(RGBQUAD) * 256);

        LocalUnlock(hloc1);
        LocalFree(hloc1);
    }
    ANativeWindow::wait = wait;
    ANativeWindow::showImage = showImage;
}

FramebufferWin32::~FramebufferWin32() {
    if (mHwnd) {
        DestroyWindow(mHwnd);
    }
}

void FramebufferWin32::showImage(ANativeWindow *window, uint8_t *buffer) {
    if (buffer == NULL)
        return;

    FramebufferWin32 *self = (FramebufferWin32 *)window;

    int i = 0;
    int j = 0;
    int bpp = self->mBmpInfo->bmiHeader.biBitCount / 8;
    unsigned char *tempBuffer = buffer;
    for (i = 0; i < self->getWindowHeight(); i++) {
        for (j = 0; j < self->getWindowWidth() * bpp; j += bpp) {
            unsigned char temp = *(tempBuffer + j);
            *(tempBuffer + j) = *(tempBuffer + j + 2);
            *(tempBuffer + j + 2) = temp;
        }
        tempBuffer += self->getWindowWidth() * bpp;
    }

    SetStretchBltMode(self->mHDC, STRETCH_DELETESCANS);
    StretchDIBits(self->mHDC, 0, 0, self->getWindowWidth(), self->getWindowHeight(), 0,
                  0, self->getWindowWidth(), self->getWindowHeight(), buffer,
                  self->mBmpInfo, DIB_RGB_COLORS, SRCCOPY);
}

void FramebufferWin32::wait(ANativeWindow *window, int32_t sec) { Sleep(sec); }

// ----------------------------------------------------------------------------
}; // namespace android
// ----------------------------------------------------------------------------