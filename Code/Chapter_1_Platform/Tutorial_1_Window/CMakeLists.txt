project(chapter_1_tutorial_1)

add_executable(${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME} PRIVATE glfw)
target_sources(${PROJECT_NAME}        PRIVATE main.cpp)