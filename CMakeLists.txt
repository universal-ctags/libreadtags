cmake_minimum_required(VERSION 3.25)
project(libreadtags)

set(CMAKE_C_STANDARD 11)

add_library(${PROJECT_NAME} SHARED readtags.c)
add_library(universal-ctags::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
