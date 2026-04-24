#ifndef __BABOON_WINDOW_VULKAN_H__
#define __BABOON_WINDOW_VULKAN_H__

#ifdef BABOON_VULKAN
#include <vulkan/vulkan.h>
#include "baboon/baboon_window.h"

#ifdef __cplusplus
extern "C"
{
#endif

VkSurfaceKHR create_vulkan_surface(VkInstance instance, BaboonWindow *window);
const char** get_vulkan_extensions(uint32_t *count);

#ifdef __cplusplus
}
#endif
#endif
#endif
