project(chapter_2_tutorial_1_opengl)

add_executable(${PROJECT_NAME})

target_compile_definitions(${PROJECT_NAME} PRIVATE GLFW_INCLUDE_NONE)
target_link_libraries(${PROJECT_NAME}      PRIVATE platform glfw glad)
target_sources(${PROJECT_NAME}             PRIVATE main.cpp)