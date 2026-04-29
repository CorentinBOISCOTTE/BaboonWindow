#include <stdio.h>
#include <baboon/baboon_window.h>
#include <baboon/baboon_window_vulkan.h>
#include "renderer.h"

static void size_callback(BaboonWindow *window, int width, int height)
{
    (void)window;
    printf("(Callback) Width: %d, height: %d\n", width, height);
}

static void mouse_click_callback(BaboonWindow *window, int button, int action, int modifier)
{
    (void)window;
    printf("(Callback) Button: %d, action: %d, modifier: %d\n", button, action, modifier);
}

static void mouse_pos_callback(BaboonWindow *window, double x_pos, double y_pos)
{
    (void)window;
    printf("(Callback) Mouse pos: x = %f, y = %f\n", x_pos, y_pos);
}

static void mouse_enter_callback(BaboonWindow *window, bool entered)
{
    (void)window;
    if (entered)
        printf("(Callback) Entered\n");
    else
        printf("(Callback) Exited\n");
}

int main(void)
{
    BaboonWindow *window = create_window(800, 600, "Baboon Window");
    if (!window) 
    {
        printf("Failed to create window\n");
        return -1;
    }

    printf("Window created successfully!\n");

    set_resize_callback(window, size_callback);
    set_mouse_click_callback(window, mouse_click_callback);
    set_mouse_pos_callback(window, mouse_pos_callback);
    set_mouse_enter_callback(window, mouse_enter_callback);

    int width, height;
    get_window_size(window, &width, &height);

    printf("Width: %d, height: %d\n", width, height);

    Renderer *renderer = create_renderer(window);

    if (!renderer)
    {
        printf("Failed to create renderer\n");
        return -1;
    }

    while (!window_should_close(window)) 
    {
        poll_events();

        renderer_clear(renderer, 1.0f, 0.0f, 0.0f, 1.0f);

        renderer_present(renderer, window);

        if (get_mouse_button(window, BABOON_MB_MIDDLE) == BABOON_RELEASE)
            set_window_should_close(window, true);

        if (get_mouse_button(window, BABOON_MB_RIGHT) == BABOON_PRESS)
        {
            double x, y;
            get_cursor_pos(window, &x, &y);
            printf("Mouse pos: x = %f, y = %f\n", x, y);
        }
    }

    destroy_renderer(renderer);

    printf("Closing window...\n");

    destroy_window(window);

    baboon_terminate();

    printf("Program exited successfully\n");
    return 0;
}
