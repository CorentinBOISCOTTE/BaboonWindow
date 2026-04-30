#include "baboon/baboon_internal.h"

BaboonWindow *cursor_disabled_window = NULL;
BaboonWindow *cursor_captured_window = NULL;

_BaboonWindow* initialize_internal_window(void)
{
	_BaboonWindow *window = (_BaboonWindow*)malloc(sizeof(_BaboonWindow));

	window->virtual_x_pos = 0.0;
	window->virtual_y_pos = 0.0;
	window->last_x_pos = 0.0;
	window->last_y_pos = 0.0;

	window->cursor_mode = BABOON_NONE;

	window->cursor_tracked = BABOON_FALSE;

	for (int i = 0; i < BABOON_MB_LAST + 1; ++i)
		window->mouse_buttons[i] = (char)BABOON_NONE;

	window->should_close = BABOON_FALSE;
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

void baboon_input_mouse_pos(_BaboonWindow *window, void* abstract_window, double x_pos, double y_pos)
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

void baboon_input_mouse_enter(_BaboonWindow *window, void* abstract_window, BaboonBool entered)
{
	if (!window || !abstract_window)
		return;

	if (window->callbacks.mouse_enter)
		window->callbacks.mouse_enter((BaboonWindow*)abstract_window, entered);
}

void set_input_mode(BaboonWindow *window, int mode, int value)
{
	if (!window)
		return;

	_BaboonWindow *internal_window = get_internal_window(window);

	switch (mode)
	{
		case BABOON_CURSOR:
		{
			if (value != BABOON_CURSOR_NORMAL && value != BABOON_CURSOR_HIDDEN && value != BABOON_CURSOR_DISABLED && value != BABOON_CURSOR_CAPTURED)
				return;

			if (internal_window->cursor_mode == value)
				return;

			internal_window->cursor_mode = value;

			get_cursor_pos(window, &internal_window->virtual_x_pos, &internal_window->virtual_y_pos);
			baboon_set_cursor_mode(window, value);
			return;
		}
	}
}

void center_cursor(BaboonWindow *window)
{
	int width, height;

	get_window_size(window, &width, &height);
	set_cursor_pos(window, (double)width * 0.5, (double)height * 0.5);
}
