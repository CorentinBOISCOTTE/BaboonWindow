#ifndef __BABOON_WINDOW_H__
#define __BABOON_WINDOW_H__

#include <stdbool.h>

typedef struct BaboonWindow BaboonWindow;

static void (*g_resize_callback)(BaboonWindow*, int, int);

BaboonWindow* create_window(int width, int height, const char* title);
void set_resize_callback(void (*resize_callback)(BaboonWindow*, int, int));
void poll_events(void);
void get_window_size(BaboonWindow* window, int* width, int* height);
bool window_should_close(BaboonWindow* window);
void destroy_window(BaboonWindow* window);

#endif
