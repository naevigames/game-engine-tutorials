project(chapter_2_tutorial_1_vulkan)

find_package(Vulkan)

add_executable(${PROJECT_NAME})

target_compile_features(${PROJECT_NAME}    PRIVATE cxx_std_23)
target_compile_definitions(${PROJECT_NAME} PRIVATE VK_USE_PLATFORM_WIN32_KHR)
target_link_libraries(${PROJECT_NAME}      PRIVATE platform Vulkan::Vulkan glfw)
target_sources(${PROJECT_NAME}             PRIVATE main.cpp)