#include <glfw/glfw3.h>

int32_t main()
{
    if (glfwInit() == GLFW_FALSE)
    {
        return -1;
    }

    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    auto window_handle = glfwCreateWindow(800, 600, "", nullptr, nullptr);
    if  (window_handle == nullptr)
    {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window_handle);

    while (glfwWindowShouldClose(window_handle) == GLFW_FALSE)
    {
        glfwSwapBuffers(window_handle);
        glfwPollEvents();
    }

    glfwDestroyWindow(window_handle);
    glfwTerminate();

    return 0;
}