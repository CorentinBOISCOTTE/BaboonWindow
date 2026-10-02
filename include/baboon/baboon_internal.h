#ifndef __BABOON_INTERNAL_H__
#define __BABOON_INTERNAL_H__

#include "baboon_window.h"

#ifdef __cplusplus
extern "C"
{
#endif

extern BaboonWindow *cursor_disabled_window;
extern BaboonWindow *cursor_captured_window;

typedef struct _BaboonWindow
{
	double		virtual_x_pos, virtual_y_pos;
	double		last_x_pos, last_y_pos;
	double		restore_cursor_pos_x, restore_cursor_pos_y;

	int			cursor_mode;

	BaboonBool	cursor_tracked;

	char		mouse_buttons[BABOON_MB_LAST + 1];

	BaboonBool	should_close;
	int			width, height;

	struct
	{
		baboon_resize_callback		resize;
		baboon_mb_callback			mouse_button;
		baboon_mouse_pos_callback	mouse_pos;
		baboon_mouse_enter_callback	mouse_enter;
	} callbacks;
} _BaboonWindow;

_BaboonWindow* initialize_internal_window(void);
void baboon_input_mouse_click(_BaboonWindow *window, void *abstract_window, int button, int action, int modifiers);
void baboon_input_mouse_pos(_BaboonWindow *window, void *abstract_window, double x_pos, double y_pos);
void baboon_input_mouse_enter(_BaboonWindow *window, void *abstract_window, BaboonBool entered);

void baboon_set_cursor_mode(BaboonWindow *window, int value);

void capture_cursor(BaboonWindow *window);
void release_cursor(void);

_BaboonWindow* get_internal_window(BaboonWindow *window);

#ifdef __cplusplus
}
#endif
#endif
