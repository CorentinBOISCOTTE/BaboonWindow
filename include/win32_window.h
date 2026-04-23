#ifndef __WIN32_WINDOW_H__
#define __WIN32_WINDOW_H__

#include <windows.h>
#include <stdbool.h>

typedef struct
{
	HWND hwnd;
	HDC hdc;
	HINSTANCE hInstance;
	bool should_close;
} Win32Window;

#endif