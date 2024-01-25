#include "platform_manager.hpp"
#include "window_manager.hpp"

#include "glfw/platform_factory.hpp"

#include <glfw/glfw3.h>

#include <vulkan/vulkan.h>

#include <array>

int32_t main()
{
    glfw::PlatformFactory platform_factory;

    auto& platform_manager = PlatformManager::instance();
    auto& window_manager   = WindowManager::instance();

    if (!platform_manager.init(&platform_factory))
    {
        return -1;
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    window_manager.init(&platform_factory, { "chapter_2_tutorial_1_vulkan", { 800, 600 } });

    VkInstance   vk_instance;
    VkSurfaceKHR vk_surface;

    std::array<const char*, 2> vk_instance_extensions
    {
        "VK_KHR_surface",
        "VK_KHR_win32_surface"
    };

    VkApplicationInfo vk_application_info
    {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pApplicationName = "Vulkan Tutorial",
        .pEngineName      = "Vulkan Engine",
        .apiVersion       = VK_API_VERSION_1_0
    };

    VkInstanceCreateInfo vk_instance_create_info
    {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pApplicationInfo        = &vk_application_info,
        .enabledLayerCount       = 0,
        .enabledExtensionCount   = (int32_t)vk_instance_extensions.size(),
        .ppEnabledExtensionNames = vk_instance_extensions.data()
    };

    vkCreateInstance(&vk_instance_create_info, nullptr, &vk_instance);

    VkWin32SurfaceCreateInfoKHR vk_surface_create_info
    {
        .sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
        .hinstance = GetModuleHandle(nullptr),
        .hwnd      = std::any_cast<HWND>(window_manager.handle())
    };

    vkCreateWin32SurfaceKHR(vk_instance, &vk_surface_create_info, nullptr, &vk_surface);

    while (window_manager.is_active())
    {
        window_manager.update();
        platform_manager.update();
    }

    vkDestroySurfaceKHR(vk_instance, vk_surface, nullptr);
    vkDestroyInstance(vk_instance, nullptr);

    window_manager.release();
    platform_manager.release();

    return 0;
}