#include <stdio.h>
#include "baboon_window.h"

static void size_callback(BaboonWindow* window, int width, int height)
{
    (void)window;
    printf("Width: %d, height: %d\n", width, height);
}

int main(void)
{
    BaboonWindow* window = create_window(800, 600, "Baboon Window");
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

    while (!window_should_close(window)) 
    {
        poll_events();
    }

    printf("Closing window...\n");

    destroy_window(window);

    printf("Program exited successfully\n");
    return 0;
}
