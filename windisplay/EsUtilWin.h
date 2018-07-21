//
// Book:      OpenGL(R) ES 2.0 Programming Guide
// Authors:   Aaftab Munshi, Dan Ginsburg, Dave Shreiner
// ISBN-10:   0321502795
// ISBN-13:   9780321502797
// Publisher: Addison-Wesley Professional
// URLs:      http://safari.informit.com/9780321563835
//            http://www.opengles-book.com
//

//
//   API-neutral interface for creating windows.  Implementation needs to be provided per-platform.

#ifndef ESUTIL_WIN_H
#define ESUTIL_WIN_H

#include <GLES2/gl2.h>
#include <EGL/egl.h>

///
//  Includes
//

#define ESCALLBACK  __cdecl

typedef struct  _ESContext
{
	HWND hwnd;
	void* context;
	void (ESCALLBACK *drawFunc) ( void* );

	_ESContext():hwnd(NULL),
                 context(NULL),
                 drawFunc(NULL)
	{

	}
}ESContext;

#ifdef __cplusplus

extern "C" {
#endif

   
///
//  Macros
//

///
// Types
//

///
//  Public Functions
//
 LRESULT WINAPI ESWindowProc ( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

///
//  WinCreate()
//
//      Create Win32 instance and window
//
//GLboolean WinCreate ( ESContext *esContext, const char *title );
HWND WinCreate ( const char *title, int width, int height, ESContext* context);

///
//  WinLoop()
//
//      Start main windows loop
//
void WinLoop ( ESContext *esContext );
///
//  WinTGALoad()
//    
//      TGA loader win32 implementation
//
//int WinTGALoad ( const char *fileName, char **buffer, int *width, int *height );

#ifdef __cplusplus
}
#endif

#endif // ESUTIL_WIN_H