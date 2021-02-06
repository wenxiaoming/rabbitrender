#ifndef ESUTIL_WIN_H
#define ESUTIL_WIN_H

#include <GLES2/gl2.h>
#include <EGL/egl.h>

#include "EGLUtils.h"


#ifdef __cplusplus

extern "C" {
#endif

 LRESULT WINAPI ESWindowProc ( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );


HWND WinCreate ( const char *title, int width, int height, ESContext* context);

void WinLoop ( ESContext *esContext );

#ifdef __cplusplus
}
#endif

#endif // ESUTIL_WIN_H