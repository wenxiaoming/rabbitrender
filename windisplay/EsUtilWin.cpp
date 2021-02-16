#define WIN32_LEAN_AND_MEAN
#ifdef _MSC_VER
#include <windows.h>
#endif
#include <stdio.h>
//#include "esUtil.h"
#include "EsUtilWin.h"

static LRESULT WINAPI ESWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam,
                                   LPARAM lParam) {
    LRESULT lRet = 1;

    switch (uMsg) {
    case WM_CREATE:
        break;

    case WM_PAINT: {
        // ESContext *esContext = (ESContext*)(LONG_PTR) GetWindowLongPtr (
        // hWnd, GWL_USERDATA );
        //
        // if ( esContext && esContext->drawFunc )
        //   esContext->drawFunc ( esContext );

        ESContext *esContext =
            (ESContext *)(LONG_PTR)GetWindowLongPtr(hWnd, GWL_USERDATA);
        if (esContext && esContext->context && esContext->drawFunc) {
            esContext->drawFunc(esContext->context);
        }

        if (esContext && esContext->hwnd)
            ValidateRect(esContext->hwnd, NULL);
    } break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    case WM_CHAR: {
        POINT point;
        // ESContext *esContext = (ESContext*)(LONG_PTR) GetWindowLongPtr (
        // hWnd, GWL_USERDATA );
        //
        // GetCursorPos( &point );

        // if ( esContext && esContext->keyFunc )
        // esContext->keyFunc ( esContext, (unsigned char) wParam,
        //                   (int) point.x, (int) point.y );
    } break;

    default:
        lRet = DefWindowProc(hWnd, uMsg, wParam, lParam);
        break;
    }

    return lRet;
}

static HWND WinCreate(const char *title, int width, int height,
                      ESContext *context) {
    WNDCLASS wndclass = {0};
    DWORD wStyle = 0;
    RECT windowRect;
    HINSTANCE hInstance = GetModuleHandle(NULL);

    wndclass.style = CS_OWNDC;
    wndclass.lpfnWndProc = (WNDPROC)ESWindowProc;
    wndclass.hInstance = hInstance;
    wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wndclass.lpszClassName = "opengles2.0";

    if (!RegisterClass(&wndclass))
        return NULL;

    wStyle = WS_VISIBLE | WS_POPUP | WS_BORDER | WS_SYSMENU | WS_CAPTION;

    // Adjust the window rectangle so that the client area has
    // the correct number of pixels
    windowRect.left = 0;
    windowRect.top = 0;
    windowRect.right = width;
    windowRect.bottom = height;

    AdjustWindowRect(&windowRect, wStyle, FALSE);

    HWND hWnd = CreateWindow(
        "opengles2.0", title, wStyle, 0, 0, windowRect.right - windowRect.left,
        windowRect.bottom - windowRect.top, NULL, NULL, hInstance, NULL);

    // Set the ESContext* to the GWL_USERDATA so that it is available to the
    // ESWindowProc
    SetWindowLongPtr(hWnd, GWL_USERDATA, (LONG)(LONG_PTR)context);

    if (hWnd == NULL)
        return NULL;

    ShowWindow(hWnd, TRUE);

    return hWnd;
}

void WinLoop(ESContext *esContext) {
    MSG msg = {0};
    int done = 0;
    DWORD lastTime = GetTickCount();

    while (!done) {
        int gotMsg = (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) != 0);
        DWORD curTime = GetTickCount();
        float deltaTime = (float)(curTime - lastTime) / 1000.0f;
        lastTime = curTime;

        if (gotMsg) {
            if (msg.message == WM_QUIT) {
                done = 1;
            } else {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        } else
            SendMessage(esContext->hwnd, WM_PAINT, 0, 0);

        // Call update function if registered
        // if ( esContext->updateFunc != NULL )
        //   esContext->updateFunc ( esContext, deltaTime );
    }
}

using namespace android;
EGLNativeWindowType win32_createDisplaySurface(ESContext *ctx, int width,
                                               int height, int format) {

    HWND hwnd = WinCreate("Kevin.Wen listream@126.com", width, height, ctx);

    FramebufferWin32 *w;
    w = new FramebufferWin32(hwnd, width, height, format);

    ctx->context = w;
    ctx->hwnd = hwnd;
    return (EGLNativeWindowType)w;
}

EGLNativeWindowType win32_createDisplaySurfaceNew(HWND hwnd, int width,
                                                  int height, int format) {
    FramebufferWin32 *w;
    w = new FramebufferWin32(hwnd, width, height, format);
    return (EGLNativeWindowType)w;
}
