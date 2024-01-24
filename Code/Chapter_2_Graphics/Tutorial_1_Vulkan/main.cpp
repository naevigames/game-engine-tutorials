#include "platform_manager.hpp"
#include "window_manager.hpp"

#include "glfw/platform_factory.hpp"

int32_t main()
{
    glfw::PlatformFactory platform_factory;

    auto& platform_manager = PlatformManager::instance();
    auto& window_manager   = WindowManager::instance();

    if (!platform_manager.init(&platform_factory))
    {
        return -1;
    }

    window_manager.init(&platform_factory, { "chapter_2_tutorial_1_vulkan", { 800, 600 } });

    while (window_manager.is_active())
    {
        window_manager.update();
        platform_manager.update();
    }

    window_manager.release();
    platform_manager.release();

    return 0;
}