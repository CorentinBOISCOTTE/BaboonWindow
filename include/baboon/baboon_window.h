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

#define BABOON_NONE				-1

#define BABOON_RELEASE			0
#define BABOON_PRESS			1

#define BABOON_MB_1				0
#define BABOON_MB_2				1
#define BABOON_MB_3				2
#define BABOON_MB_4				3
#define BABOON_MB_5				4
#define BABOON_MB_6				5
#define BABOON_MB_7				6
#define BABOON_MB_8				7
#define BABOON_MB_LAST			BABOON_MB_8
#define BABOON_MB_LEFT			BABOON_MB_1
#define BABOON_MB_RIGHT			BABOON_MB_2
#define BABOON_MB_MIDDLE		BABOON_MB_3

#define BABOON_CURSOR_NORMAL	0
#define BABOON_CURSOR_HIDDEN	1
#define BABOON_CURSOR_DISABLED	2
#define BABOON_CURSOR_CAPTURED	3

#define BABOON_MOD_CONTROL		0x0001
#define BABOON_MOD_SHIFT		0x0002
#define BABOON_MOD_ALT			0x0004
#define BABOON_MOD_CAPS_LOCK	0x0008
#define BABOON_MOD_NUM_LOCK		0x0010
#define BABOON_MOD_SUPER		0x0020

#define BABOON_MOD_MASK			(BABOON_MOD_CONTROL | BABOON_MOD_SHIFT | BABOON_MOD_ALT | BABOON_MOD_CAPS_LOCK | BABOON_MOD_NUM_LOCK | BABOON_MOD_SUPER)

typedef struct BaboonWindow BaboonWindow;

typedef void (*baboon_resize_callback)(BaboonWindow *window, int x_size, int y_size);
typedef void (*baboon_mb_callback)(BaboonWindow *window, int button, int action, int mods);
typedef void (*baboon_mouse_pos_callback)(BaboonWindow *window, double x_pos, double y_pos);
typedef void (*baboon_mouse_enter_callback)(BaboonWindow *window, bool entered);

typedef void (*gl_proc)(void);

BaboonWindow* create_window(int width, int height, const char *title);
void destroy_window(BaboonWindow* window);
void baboon_terminate(void);

void set_resize_callback(BaboonWindow *window, baboon_resize_callback callback);
void set_mouse_click_callback(BaboonWindow *window, baboon_mb_callback callback);
void set_mouse_pos_callback(BaboonWindow *window, baboon_mouse_pos_callback callback);
void set_mouse_enter_callback(BaboonWindow *window, baboon_mouse_enter_callback callback);

void get_window_size(BaboonWindow *window, int *width, int *height);
int get_mouse_button(BaboonWindow *window, int button);
void get_cursor_pos(BaboonWindow *window, double *x_pos, double *y_pos);

void make_context_current(BaboonWindow* window);
void poll_events(void);
void wait_events(void);
bool window_should_close(BaboonWindow *window);
void set_window_should_close(BaboonWindow* window, bool should_close);
void swap_buffers(BaboonWindow *window);
gl_proc baboon_get_proc_address(const char* proc_name);

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
