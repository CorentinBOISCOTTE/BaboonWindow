#ifndef __BABOON_WINDOW_H__
#define __BABOON_WINDOW_H__

#include <stdbool.h>
#include <windows.h>
#include <wingdi.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" 
{
#endif

typedef struct BaboonWindow BaboonWindow;

typedef void (*GLProc)(void);

enum Flags
{
	BABOON_NO_API = 0,
	BABOON_OPENGL = 1,

	BABOON_NB_FLAGS
};

typedef void (*resize_callback)(BaboonWindow*, int, int);

BaboonWindow* create_window(int width, int height, const char *title);
void set_resize_callback(resize_callback callback);
void poll_events(void);
void get_window_size(BaboonWindow *window, int *width, int *height);
bool window_should_close(BaboonWindow *window);
void destroy_window(BaboonWindow *window);
void swap_buffers(BaboonWindow *window);
void make_context_current(BaboonWindow* window);
void baboon_terminate(void);
GLProc baboon_get_proc_adress(const char* proc_name);

#ifdef _WIN32
#include <windows.h>
HWND get_win32_window(BaboonWindow* window);
#endif

#ifdef __linux__
// Linux not yet supported
#endif

#ifdef __cplusplus
}
#endif
#endif
