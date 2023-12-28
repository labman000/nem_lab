cmake_minimum_required(VERSION 3.16)
project(nem_lab C)

set(CMAKE_C_STANDARD 11)

add_subdirectory(nem_com)

add_executable(nem_lab main.c)
