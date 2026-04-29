#include "opengl_renderer.h"

#include <stdio.h>
#include "glad/glad.h"

Renderer* create_renderer(BaboonWindow *window)
{
    make_context_current(window);

    if (!gladLoadGLLoader((GLADloadproc)baboon_get_proc_address))
    {
        printf("Failed to initialize GLAD\n");
        baboon_terminate();
        return NULL;
    }

    return malloc(1);
}

void renderer_clear(Renderer* renderer, float r, float g, float b, float a)
{
    (void)renderer;
    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void renderer_present(Renderer* renderer, BaboonWindow* window)
{
    (void)renderer;
    swap_buffers(window);
}

void destroy_renderer(Renderer* renderer)
{
    free(renderer);
}
