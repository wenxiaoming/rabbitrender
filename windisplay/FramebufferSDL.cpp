/*
*	Add by Kevin.Wen
*	It supports displaying opengles's output buffer with sdl
*/

//#include <unistd.h>
#ifndef _MSC_VER
#include </usr/include/unistd.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <EGL/egl.h>
#include "FramebufferSDL.h"

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

FramebufferSDL::FramebufferSDL(SDL_Window* window, int width, int height, int format) 
    : mUpdateOnDemand(false)
	, mSDLWindow(window)
	, mWindowWidth(width)
	, mWindowHeight(height)
	, mFormat(format)
{
	if(window)
	{
		SDL_Renderer* sdlRenderer = SDL_CreateRenderer(window, -1, 0);  

		Uint32 pixformat = SDL_PIXELFORMAT_BGR888;//SDL_PIXELFORMAT_BGR888;  

		SDL_Texture* sdlTexture = SDL_CreateTexture(sdlRenderer, pixformat, SDL_TEXTUREACCESS_STREAMING, width, height);

		mSDLRenderer = sdlRenderer;
		mSDLTexture = sdlTexture;
	}

	int i;

    ANativeWindow::setSwapInterval = setSwapInterval;
    ANativeWindow::dequeueBuffer = dequeueBuffer;
    ANativeWindow::lockBuffer = lockBuffer;
    ANativeWindow::queueBuffer = queueBuffer;
    ANativeWindow::query = query;
    ANativeWindow::perform = perform;

	mUpdateOnDemand = false;
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
		buffers[i]->handle->data[0] = (uintptr_t)((uint8_t* )malloc(mWindowWidth* mWindowHeight* 4)); 
		buffers[i]->stride = mWindowWidth;

	}
}

FramebufferSDL::~FramebufferSDL() 
{
    int i = 0;
    for (i = 0; i < mNumBuffers; i++)
    {
       free((uint8_t*)(buffers[i]->handle->data[0]));
       free(buffers[i]->handle);
    }

	if (mSDLWindow)
	{
		SDL_DestroyTexture(mSDLTexture);
    	SDL_DestroyRenderer(mSDLRenderer);
    	SDL_DestroyWindow(mSDLWindow);
    	SDL_Quit();
	}
}

int FramebufferSDL::setSwapInterval(
	ANativeWindow* window, int interval) 
{
	return 0;
}

int FramebufferSDL::dequeueBuffer(ANativeWindow* window, 
        ANativeWindowBuffer** buffer)
{
	FramebufferSDL* self = (FramebufferSDL*)window;
	int index = self->mBufferHead++;
	if (self->mBufferHead >= self->mNumBuffers)
	    self->mBufferHead = 0;	

	//// wait for a free buffer
	while (!self->mNumFreeBuffers) {
	    sleep(1);
	}
	// get this buffer
	self->mNumFreeBuffers--;
	self->mCurrentBufferIndex = index;

	*buffer = self->buffers[index];

    return 0;
}

int FramebufferSDL::lockBuffer(ANativeWindow* window, 
        ANativeWindowBuffer* buffer)
{
 	FramebufferSDL* self = (FramebufferSDL*)window;

	const int index = self->mCurrentBufferIndex;

	// wait that the buffer we're locking is not front anymore
	while (self->front == buffer) {
		sleep(1);
	}
    return NO_ERROR;
}

void FramebufferSDL::showImage(uint8_t* buffer)
{
	if (buffer == NULL)
		return;

    SDL_UpdateTexture(mSDLTexture, NULL, buffer, mWindowWidth * 4);  

    // todo, handle it when window size is changed.
	SDL_Rect sdlRect; 
    sdlRect.x = 0;  
    sdlRect.y = 0;  
    sdlRect.w = mWindowWidth;  
    sdlRect.h = mWindowHeight;  

    SDL_RenderClear(mSDLRenderer);   
    SDL_RenderCopy(mSDLRenderer, mSDLTexture, NULL, &sdlRect);  
    SDL_RenderPresent(mSDLRenderer);  
}

int FramebufferSDL::queueBuffer(ANativeWindow* window, 
        ANativeWindowBuffer* buffer)
{
	FramebufferSDL* self = (FramebufferSDL*)window;

	const int index = self->mCurrentBufferIndex;

	if (0)
	{//dump output framebuffer to file
		FILE* file = fopen("datadump.bin", "ab");
		if (file)
		{
			fwrite((uint8_t*)(buffer->handle->data[0]), 1, buffer->height*buffer->width*4, file);
			fclose(file);
		}
	}

	self->showImage((uint8_t*)(buffer->handle->data[0]));

	self->front = static_cast<NativeBuffer*>(buffer);
	self->mNumFreeBuffers++;

    return 0;
}

int FramebufferSDL::query(const ANativeWindow* window,
        int what, int* value) 
{
	FramebufferSDL* self = (FramebufferSDL*)window;
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

int FramebufferSDL::perform(ANativeWindow* window,
        int operation, ...)
{ 
    return 0;
}

// ----------------------------------------------------------------------------
}; // namespace android
// ----------------------------------------------------------------------------

using namespace android;
EGLNativeWindowType SDL_CreateDisplaySurface(ESContext* ctx,  int width, int height, int format)
{

    SDL_Window* window = SDLWinowCreate ( "Kevin.Wen listream@126.com", width, height, ctx);

    FramebufferSDL* w;
    w = new FramebufferSDL(window, width, height, format);

	ctx->context = w;
    ctx->window = window;
    return (EGLNativeWindowType)w;
}