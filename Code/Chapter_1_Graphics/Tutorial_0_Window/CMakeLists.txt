project(chapter_1_tutorial_0)

add_executable(${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME} PRIVATE glfw)
target_sources(${PROJECT_NAME}        PRIVATE main.cpp)