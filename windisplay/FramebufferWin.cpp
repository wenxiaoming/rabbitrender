/*
*	Add by Kevin.Wen
*	It supports displaying opengles's output buffer on windows
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <EGL/egl.h>
#include "FramebufferWin.h"

// ----------------------------------------------------------------------------
namespace android {
// ----------------------------------------------------------------------------

class NativeBuffer 
    : public ANativeWindowBuffer
{
public:
    NativeBuffer(int w, int h, int f, int u) {
        ANativeWindowBuffer::width  = w;
        ANativeWindowBuffer::height = h;
        ANativeWindowBuffer::format = f;
        ANativeWindowBuffer::usage  = u;
    }
private:
    ~NativeBuffer() { }; // this class cannot be overloaded
};

FramebufferWin32::FramebufferWin32(HWND hwnd, int width, int height, int format) 
    : mUpdateOnDemand(false)
	, mHwnd(hwnd)
	, mWindowWidth(width)
	, mWindowHeight(height)
{
	if(IsWindow(hwnd))
	{
		RECT   rect;
		GetWindowRect(hwnd,&rect); 

		mWindowWidth = width;//rect.right - rect.left;
		mWindowHeight = height;//rect.bottom - rect.top;

		mFormat = format;

		mHDC = GetDC(mHwnd); 

		hloc = GlobalAlloc(GMEM_ZEROINIT | GMEM_MOVEABLE,
			sizeof(BITMAPINFOHEADER) + (sizeof(RGBQUAD) * 256));
		BmpInfo = (LPBITMAPINFO) GlobalLock(hloc);

		//init BmpInfo
		int i;
		HANDLE hloc1;
		RGBQUAD *argbq;

		hloc1 = LocalAlloc(LMEM_ZEROINIT | LMEM_MOVEABLE,(sizeof(RGBQUAD) * 256));
		argbq = (RGBQUAD *) LocalLock(hloc1);

		for(i=0;i<256;i++) {
			argbq[i].rgbBlue=i;
			argbq[i].rgbGreen=i;
			argbq[i].rgbRed=i;
			argbq[i].rgbReserved=0;
		}

		BmpInfo->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		BmpInfo->bmiHeader.biPlanes = 1;
		
		BmpInfo->bmiHeader.biBitCount = 32;
		BmpInfo->bmiHeader.biCompression = BI_RGB;
		BmpInfo->bmiHeader.biWidth = mWindowWidth;
		BmpInfo->bmiHeader.biHeight = -mWindowHeight;

		memcpy(BmpInfo->bmiColors, argbq, sizeof(RGBQUAD) * 256);

		LocalUnlock(hloc1);
		LocalFree(hloc1);
	}

	int i;

    ANativeWindow::setSwapInterval = setSwapInterval;
    ANativeWindow::dequeueBuffer = dequeueBuffer;
    ANativeWindow::lockBuffer = lockBuffer;
    ANativeWindow::queueBuffer = queueBuffer;
    ANativeWindow::query = query;
    ANativeWindow::perform = perform;

	mUpdateOnDemand = FALSE;
    mCurrentBufferIndex = 0;

	// initialize the buffer FIFO
	mNumBuffers = NUM_FRAME_BUFFERS;
	mNumFreeBuffers = NUM_FRAME_BUFFERS;
	mBufferHead = mNumBuffers-1;

	for (i = 0; i < mNumBuffers; i++)
	{
		buffers[i] = new NativeBuffer(
			mWindowWidth, mWindowHeight, mFormat, GRALLOC_USAGE_HW_FB);
	}

	for (i = 0; i < mNumBuffers; i++)
	{

		buffers[i]->handle = (buffer_handle_t)malloc(sizeof(native_handle_t));
		buffers[i]->handle->data[0] = (uint32_t)((uint8_t* )malloc(mWindowWidth* mWindowHeight* 4)); 
		buffers[i]->stride = mWindowWidth;

	}
}

FramebufferWin32::~FramebufferWin32() 
{
    int i = 0;
    for (i = 0; i < mNumBuffers; i++)
    {
       free((uint8_t*)(buffers[i]->handle->data[0]));
       free(buffers[i]->handle);
    }

	if (mHwnd)
	{
		DestroyWindow(mHwnd);
	}
}

int FramebufferWin32::setSwapInterval(
	ANativeWindow* window, int interval) 
{
	return 0;
}

int FramebufferWin32::dequeueBuffer(ANativeWindow* window, 
        ANativeWindowBuffer** buffer)
{
	FramebufferWin32* self = (FramebufferWin32*)window;
	int index = self->mBufferHead++;
	if (self->mBufferHead >= self->mNumBuffers)
	    self->mBufferHead = 0;	

	//// wait for a free buffer
	while (!self->mNumFreeBuffers) {
	    Sleep(1);
	}
	// get this buffer
	self->mNumFreeBuffers--;
	self->mCurrentBufferIndex = index;

	*buffer = self->buffers[index];

    return 0;
}

int FramebufferWin32::lockBuffer(ANativeWindow* window, 
        ANativeWindowBuffer* buffer)
{
 	FramebufferWin32* self = (FramebufferWin32*)window;

	const int index = self->mCurrentBufferIndex;

	// wait that the buffer we're locking is not front anymore
	while (self->front == buffer) {
		Sleep(1);
	}
    return NO_ERROR;
}

void FramebufferWin32::ShowImage(BYTE *lpImage)
{
	int i = 0;
	int j = 0;
	int bpp = BmpInfo->bmiHeader.biBitCount/8;
	unsigned char* tempBuffer = lpImage;
	for (i=0; i < mWindowHeight; i++)
	{
		for (j=0; j < mWindowWidth*bpp; j+=bpp)
		{
			unsigned char temp = *(tempBuffer+j);
			*(tempBuffer+j) = *(tempBuffer+j+2);
			*(tempBuffer+j+2) = temp;
		}
		tempBuffer += mWindowWidth*bpp;
	}

	SetStretchBltMode(mHDC, STRETCH_DELETESCANS);
	StretchDIBits(mHDC,0,0, mWindowWidth, mWindowHeight,
		0,0,mWindowWidth,mWindowHeight,
		lpImage, BmpInfo, DIB_RGB_COLORS,SRCCOPY);

}

int FramebufferWin32::queueBuffer(ANativeWindow* window, 
        ANativeWindowBuffer* buffer)
{
	FramebufferWin32* self = (FramebufferWin32*)window;

	const int index = self->mCurrentBufferIndex;

	if (0)
	{//dump output framebuffer to file
		FILE* file = fopen("c:\\datadump.bin", "ab");
		if (file)
		{
			fwrite((uint8_t*)(buffer->handle->data[0]), 1, buffer->height*buffer->width*4, file);
			fclose(file);
		}
	}

	self->ShowImage((uint8_t*)(buffer->handle->data[0]));

	self->front = static_cast<NativeBuffer*>(buffer);
	self->mNumFreeBuffers++;

    return 0;
}

int FramebufferWin32::query(const ANativeWindow* window,
        int what, int* value) 
{
	FramebufferWin32* self = (FramebufferWin32*)window;
	switch(what)
	{
	    case NATIVE_WINDOW_WIDTH:
	        *value = self->mWindowWidth;
	        return NO_ERROR;
	    case NATIVE_WINDOW_HEIGHT:
	        *value = self->mWindowHeight;
	        return NO_ERROR;
	    case NATIVE_WINDOW_FORMAT:
            *value = self->mFormat;
            return NO_ERROR;
	}
 
	return 0;
}

int FramebufferWin32::perform(ANativeWindow* window,
        int operation, ...)
{ 
    return 0;
}

// ----------------------------------------------------------------------------
}; // namespace android
// ----------------------------------------------------------------------------

using namespace android;
EGLNativeWindowType win32_createDisplaySurface(ESContext* ctx,  int width, int height, int format)
{

    HWND hwnd = WinCreate ( "Kevin.Wen listream@126.com", width, height, ctx);

    FramebufferWin32* w;
    w = new FramebufferWin32(hwnd, width, height, format);

	ctx->context = w;
    ctx->hwnd = hwnd;
    return (EGLNativeWindowType)w;
}

EGLNativeWindowType win32_createDisplaySurfaceNew(HWND hwnd, int width, int height, int format)
{
	FramebufferWin32* w;
	w = new FramebufferWin32(hwnd, width, height, format);
	return (EGLNativeWindowType)w;
}
