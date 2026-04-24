#ifndef __WIN32_WINDOW_H__
#define __WIN32_WINDOW_H__

#include <windows.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct Win32Window
{
	HWND hwnd;
	HDC hdc;
	HINSTANCE hinstance;
	bool should_close;
	int width;
	int height;

} Win32Window;

#ifdef __cplusplus
}
#endif
#endif
