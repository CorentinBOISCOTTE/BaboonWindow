#ifndef __WIN32_WINDOW_H__
#define __WIN32_WINDOW_H__

#include <windows.h>
#include <stdbool.h>

#include "baboon_internal.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct Win32Window
{
	_BaboonWindow* internal_window;

	HWND hwnd;
	HDC hdc;
	HINSTANCE hinstance;

} Win32Window;

#ifdef __cplusplus
}
#endif
#endif
