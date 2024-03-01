cmake_minimum_required(VERSION 3.16.3)

project(strutils)

add_executable(${PROJECT_NAME} main.c)

set_property(TARGET ${PROJECT_NAME} PROPERTY C_STANDARD 99)

add_subdirectory(test_library)

target_include_directories(${PROJECT_NAME}
	PUBLIC test_library
)

target_link_libraries(${PROJECT_NAME}
	PUBLIC adder_library
)
