#ifndef __RENDERER_H__
#define __RENDERER_H__

#include <baboon/baboon_window.h>

typedef struct Renderer Renderer;

Renderer* create_renderer(BaboonWindow *window);
void renderer_clear(Renderer *renderer, float r, float g, float b, float a);
void renderer_present(Renderer *renderer, BaboonWindow *window);
void destroy_renderer(Renderer *renderer);

#endif
