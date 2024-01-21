project(chapter_1_tutorial_1_next)

add_executable(${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME} PRIVATE platform glfw)
target_sources(${PROJECT_NAME}        PRIVATE main.cpp)