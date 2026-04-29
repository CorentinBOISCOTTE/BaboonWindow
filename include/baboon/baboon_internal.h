#ifndef __BABOON_INTERNAL_H__
#define __BABOON_INTERNAL_H__

#include "baboon_window.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct _BaboonWindow
{
	double	virtual_x_pos, virtual_y_pos;
	double	last_x_pos, last_y_pos;

	int		cursor_mode;

	bool	cursor_tracked;

	char	mouse_buttons[BABOON_MB_LAST + 1];

	bool	should_close;
	int		width, height;

	struct
	{
		baboon_resize_callback		resize;
		baboon_mb_callback			mouse_button;
		baboon_mouse_pos_callback	mouse_pos;
		baboon_mouse_enter_callback	mouse_enter;
	} callbacks;
} _BaboonWindow;

_BaboonWindow* initialize_internal_window(void)
{
	_BaboonWindow *window = (_BaboonWindow*)malloc(sizeof(_BaboonWindow));

	window->virtual_x_pos = 0.0;
	window->virtual_y_pos = 0.0;
	window->last_x_pos = 0.0;
	window->last_y_pos = 0.0;

	window->cursor_mode = BABOON_NONE;

	window->cursor_tracked = false;

	for (int i = 0; i < BABOON_MB_LAST + 1; ++i)
		window->mouse_buttons[i] = (char)BABOON_NONE;

	window->should_close = false;
	window->width = 0;
	window->height = 0;

	window->callbacks.resize = NULL;
	window->callbacks.mouse_button = NULL;
	window->callbacks.mouse_pos = NULL;
	window->callbacks.mouse_enter = NULL;

	return window;
}

void baboon_input_mouse_click(_BaboonWindow *window, void *abstract_window, int button, int action, int modifiers)
{
	if (!window || !abstract_window || button < 0 || !(action == BABOON_RELEASE || action == BABOON_PRESS) || modifiers != (modifiers & BABOON_MOD_MASK))
		return;

	if (button <= BABOON_MB_LAST)
		window->mouse_buttons[button] = (char)action;

	if (window->callbacks.mouse_button)
		window->callbacks.mouse_button((BaboonWindow*)abstract_window, button, action, modifiers);
}

void baboon_input_mouse_pos(_BaboonWindow *window, void *abstract_window, double x_pos, double y_pos)
{
	if (!window || !abstract_window)
		return;

	if (window->virtual_x_pos == x_pos && window->virtual_y_pos == y_pos)
		return;

	window->virtual_x_pos = x_pos;
	window->virtual_y_pos = y_pos;

	if (window->callbacks.mouse_pos)
		window->callbacks.mouse_pos((BaboonWindow*)abstract_window, x_pos, y_pos);
}

void baboon_input_mouse_enter(_BaboonWindow *window, void *abstract_window, bool entered)
{
	if (!window || !abstract_window)
		return;

	if (window->callbacks.mouse_enter)
		window->callbacks.mouse_enter((BaboonWindow*)abstract_window, entered);
}

#ifdef __cplusplus
}
#endif
#endif
