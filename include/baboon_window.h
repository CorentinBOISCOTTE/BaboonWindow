#ifndef __BABOON_WINDOW_H__
#define __BABOON_WINDOW_H__

#include <stdbool.h>

typedef struct BaboonWindow BaboonWindow;

BaboonWindow* create_window(int width, int height, const char* title);
void poll_events();
bool window_should_close(BaboonWindow* window);
void destroy_window(BaboonWindow* window);

#endif