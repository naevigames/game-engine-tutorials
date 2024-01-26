project(chapter_2_tutorial_1_vulkan_next)

find_package(Vulkan)

add_executable(${PROJECT_NAME})

target_compile_features(${PROJECT_NAME}    PRIVATE cxx_std_23)
target_link_libraries(${PROJECT_NAME}      PRIVATE platform graphics Vulkan::Vulkan)
target_sources(${PROJECT_NAME}             PRIVATE main.cpp)