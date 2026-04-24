#include "baboon/baboon_window_vulkan.h"

#ifdef _WIN32
#include <vulkan/vulkan_win32.h>
#include "baboon/win32_window.h"

VkSurfaceKHR create_vulkan_surface(VkInstance instance, BaboonWindow *window)
{
    Win32Window *win32_window = (Win32Window*)window;

    VkWin32SurfaceCreateInfoKHR surface_create_info = 
    {
        .sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
        .hinstance = win32_window->hinstance,
        .hwnd = win32_window->hwnd
    };

    VkSurfaceKHR surface;
    if (vkCreateWin32SurfaceKHR(instance, &surface_create_info, NULL, &surface) != VK_SUCCESS)
        return VK_NULL_HANDLE;

    return surface;
}

const char** get_vulkan_extensions(uint32_t *count)
{
    static const char *extensions[] = 
    {
        VK_KHR_SURFACE_EXTENSION_NAME,
        VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
    };

    *count = 2;
    return extensions;
}

#endif
