cmake_minimum_required(VERSION 3.22)
project(libreadtags)

set(CMAKE_C_STANDARD 99)

add_library(${PROJECT_NAME} STATIC readtags.c)
add_library(universal-ctags::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
