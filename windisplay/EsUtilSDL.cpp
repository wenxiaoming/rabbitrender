#define WIN32_LEAN_AND_MEAN
#ifdef _MSC_VER
#include <windows.h>
#endif
#include <stdio.h>
//#include "esUtil.h"
#include "EsUtilSDL.h"

SDL_Window* SDLWinowCreate ( const char *title, int width, int height, ESContext* context)
{
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window* window = SDL_CreateWindow(title,
                              SDL_WINDOWPOS_UNDEFINED,
                              SDL_WINDOWPOS_UNDEFINED,
                              width,
                              height,
                              SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);// 创建窗口

    if (!window) {
        return NULL;
    }

    return window;
}

// Refresh Event
#define REFRESH_EVENT  (SDL_USEREVENT + 1)
static int thread_quit = 0;
int refresh_render(void *opaque){
    while (thread_quit==0) {
        SDL_Event event;
        event.type = REFRESH_EVENT;
        SDL_PushEvent(&event);
        SDL_Delay(1);
    }
    return 0;
}

void WinLoop ( ESContext *esContext)
{
   SDL_Thread *refresh_thread = SDL_CreateThread(refresh_render,NULL,NULL);
   while (!thread_quit)
   {
        SDL_Event event;
        SDL_PollEvent(&event);
        switch (event.type) {
            case REFRESH_EVENT:
                //SDL_Log("REFRESH_EVENT");
                {
                    if (esContext && esContext->context && esContext->drawFunc)
                    {
                        esContext->drawFunc(esContext->context);
                    }
                }
                break;
            case SDL_QUIT:
                SDL_Log("quit");
                thread_quit = 1;
                break;
            default:
                //SDL_Log("event type:%d", event.type);
                break;
        }
   }

   SDL_DetachThread(refresh_thread);
}
