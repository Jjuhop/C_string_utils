cmake_minimum_required(VERSION 3.16.3)

project(strut)

add_executable(${PROJECT_NAME} main.c tests.c)

set_property(TARGET ${PROJECT_NAME} PROPERTY C_STANDARD 99)