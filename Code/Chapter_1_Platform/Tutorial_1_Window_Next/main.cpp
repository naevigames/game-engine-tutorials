#include "platform_manager.hpp"
#include "window_manager.hpp"

#ifdef GLFW_PLATFORM
#include "glfw/platform_factory.hpp"
#endif

int32_t main()
{
    #ifdef GLFW_PLATFORM
    glfw::PlatformFactory platform_factory;
    #endif

    auto& platform_manager = PlatformManager::instance();
    auto& window_manager   = WindowManager::instance();

    if (!platform_manager.init(&platform_factory))
    {
        return -1;
    }

    window_manager.init(&platform_factory, { "chapter_1_tutorial_1_next", { 800, 600 } });

    while (window_manager.is_active())
    {
        window_manager.update();
        platform_manager.update();
    }

    window_manager.release();
    platform_manager.release();

    return 0;
}