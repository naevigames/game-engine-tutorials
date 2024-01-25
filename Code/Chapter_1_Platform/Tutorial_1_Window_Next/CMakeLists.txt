project(chapter_1_tutorial_1_next)

add_executable(${PROJECT_NAME})

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_23)
target_link_libraries(${PROJECT_NAME}   PRIVATE platform)
target_sources(${PROJECT_NAME}          PRIVATE main.cpp)