#include <stdio.h>
#include "glad/glad.h"
#include <baboon/baboon_window.h>
#include <baboon/baboon_window_vulkan.h>

static void size_callback(BaboonWindow *window, int width, int height)
{
    (void)window;
    printf("Width: %d, height: %d\n", width, height);
}

int main(void)
{
    BaboonWindow *window = create_window(800, 600, "Baboon Window");
    if (!window) 
    {
        printf("Failed to create window\n");
        return 1;
    }

    printf("Window created successfully!\n");

    set_resize_callback(size_callback);

    int width, height;
    get_window_size(window, &width, &height);

    printf("Width: %d, height: %d\n", width, height);

    make_context_current(window);

    if (!gladLoadGLLoader((GLADloadproc)baboon_get_proc_adress))
    {
        printf("Failed to initialize GLAD\n");
        baboon_terminate();
        return 1;
    }

    while (!window_should_close(window)) 
    {
        poll_events();

        glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        swap_buffers(window);
    }

    printf("Closing window...\n");

    destroy_window(window);

    baboon_terminate();

    printf("Program exited successfully\n");
    return 0;
}
