#include <glfw/glfw3.h>

#include "glfw/platform.hpp"

int32_t main()
{
    glfw::Platform platform;

    if (platform.init() == GLFW_FALSE)
    {
        return -1;
    }

    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    auto window_handle = glfwCreateWindow(800, 600, "chapter_1_tutorial_1_next", nullptr, nullptr);
    if  (window_handle == nullptr)
    {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window_handle);

    while (glfwWindowShouldClose(window_handle) == GLFW_FALSE)
    {
        glfwSwapBuffers(window_handle);
        platform.poll_events();
    }

    glfwDestroyWindow(window_handle);
    platform.release();

    return 0;
}