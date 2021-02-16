/*
 *	Add by Kevin.Wen
 *	It supports displaying opengles's output buffer with sdl
 */

//#include <unistd.h>
#ifndef _MSC_VER
#include </usr/include/unistd.h>
#endif

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "FramebufferSDL.h"
#include <EGL/egl.h>

// ----------------------------------------------------------------------------
namespace android {
// ----------------------------------------------------------------------------

FramebufferSDL::FramebufferSDL(SDL_Window *window, int width, int height,
                               int format)
    : mSDLWindow(window), Framebuffer(width, height, format) {
    if (window) {
        SDL_Renderer *sdlRenderer = SDL_CreateRenderer(window, -1, 0);

        Uint32 pixformat = SDL_PIXELFORMAT_BGR888; // SDL_PIXELFORMAT_BGR888;

        SDL_Texture *sdlTexture = SDL_CreateTexture(
            sdlRenderer, pixformat, SDL_TEXTUREACCESS_STREAMING, width, height);

        mSDLRenderer = sdlRenderer;
        mSDLTexture = sdlTexture;
    }
    ANativeWindow::wait = wait;
    ANativeWindow::showImage = showImage;
}

FramebufferSDL::~FramebufferSDL() {
    if (mSDLWindow) {
        SDL_DestroyTexture(mSDLTexture);
        SDL_DestroyRenderer(mSDLRenderer);
        SDL_DestroyWindow(mSDLWindow);
        SDL_Quit();
    }
}

void FramebufferSDL::showImage(ANativeWindow *window, uint8_t *buffer) {
    if (buffer == NULL)
        return;

    FramebufferSDL *self = (FramebufferSDL *)window;

    SDL_UpdateTexture(self->mSDLTexture, NULL, buffer,
                      self->getWindowWidth() * 4);

    // todo, handle it when window size is changed.
    SDL_Rect sdlRect;
    sdlRect.x = 0;
    sdlRect.y = 0;
    sdlRect.w = self->getWindowWidth();
    sdlRect.h = self->getWindowHeight();

    SDL_RenderClear(self->mSDLRenderer);
    SDL_RenderCopy(self->mSDLRenderer, self->mSDLTexture, NULL, &sdlRect);
    SDL_RenderPresent(self->mSDLRenderer);
}

void FramebufferSDL::wait(ANativeWindow *window, int32_t sec) {
    sleep(sec * 10000);
}

// ----------------------------------------------------------------------------
}; // namespace android
// ----------------------------------------------------------------------------