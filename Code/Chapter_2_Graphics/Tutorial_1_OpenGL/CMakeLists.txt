project(chapter_2_tutorial_1)

add_executable(${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME} PRIVATE platform)
target_sources(${PROJECT_NAME}        PRIVATE main.cpp)