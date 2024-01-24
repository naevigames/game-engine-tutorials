#include "platform_manager.hpp"
#include "window_manager.hpp"

#include "glfw/platform_factory.hpp"

#include <glfw/glfw3.h>
#include <glad/glad.h>

int32_t main()
{
    glfw::PlatformFactory platform_factory;

    auto& platform_manager = PlatformManager::instance();
    auto& window_manager   = WindowManager::instance();

    if (!platform_manager.init(&platform_factory))
    {
        return -1;
    }

    window_manager.init(&platform_factory, { "chapter_2_tutorial_1_opengl", { 800, 600 } });

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        platform_manager.release();
        return -1;
    }

    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);

    while (window_manager.is_active())
    {
        glClear(GL_COLOR_BUFFER_BIT);

        window_manager.update();
        platform_manager.update();
    }

    window_manager.release();
    platform_manager.release();

    return 0;
}