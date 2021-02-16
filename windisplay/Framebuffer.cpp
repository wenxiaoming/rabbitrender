#include "Framebuffer.h"

// ----------------------------------------------------------------------------
namespace android {
// ----------------------------------------------------------------------------

class NativeBuffer : public ANativeWindowBuffer
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

Framebuffer::Framebuffer(int32_t width, int32_t height, int32_t format)
	: mWindowWidth(width)
	, mWindowHeight(height)
	, mFormat(format)
{
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

	int i;
    for (i = 0; i < mNumBuffers; i++)
    {
        buffers[i] = new NativeBuffer(
            mWindowWidth, mWindowHeight, mFormat, GRALLOC_USAGE_HW_FB);
    }

    for (i = 0; i < mNumBuffers; i++)
    {
        buffers[i]->handle = (buffer_handle_t)malloc(sizeof(native_handle_t));
        buffers[i]->handle->data[0] = (uintptr_t)((uint8_t* )malloc(mWindowWidth* mWindowHeight * 4));
        buffers[i]->stride = mWindowWidth;
    }
}

Framebuffer::~Framebuffer()
{
    int i = 0;
    for (i = 0; i < mNumBuffers; i++)
    {
       free((uint8_t*)(buffers[i]->handle->data[0]));
       free(buffers[i]->handle);
    }
}

int Framebuffer::setSwapInterval(
	ANativeWindow* window, int interval)
{
	return 0;
}

int Framebuffer::dequeueBuffer(ANativeWindow* window,
        ANativeWindowBuffer** buffer)
{
    Framebuffer* self = (Framebuffer*)window;
	int index = self->mBufferHead++;
	if (self->mBufferHead >= self->mNumBuffers)
	    self->mBufferHead = 0;

	//// wait for a free buffer
	while (!self->mNumFreeBuffers) {
	    window->wait(window, 1);
	}
	// get this buffer
	self->mNumFreeBuffers--;
	self->mCurrentBufferIndex = index;

	*buffer = self->buffers[index];

    return 0;
}

int Framebuffer::lockBuffer(ANativeWindow* window,
        ANativeWindowBuffer* buffer)
{
    Framebuffer* self = (Framebuffer*)window;

	const int index = self->mCurrentBufferIndex;

	// wait that the buffer we're locking is not front anymore
	while (self->front == buffer) {
		window->wait(window, 1);
	}
    return NO_ERROR;
}

int Framebuffer::queueBuffer(ANativeWindow* window,
        ANativeWindowBuffer* buffer)
{
	Framebuffer* self = (Framebuffer*)window;

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

	window->showImage(window, (uint8_t*)(buffer->handle->data[0]));

	self->front = static_cast<NativeBuffer*>(buffer);
	self->mNumFreeBuffers++;

    return 0;
}

int Framebuffer::query(const ANativeWindow* window,
        int what, int* value)
{
	Framebuffer* self = (Framebuffer*)window;
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

int Framebuffer::perform(ANativeWindow* window,
        int operation, ...)
{
    return 0;
}

// ----------------------------------------------------------------------------
}; // namespace android
// ----------------------------------------------------------------------------