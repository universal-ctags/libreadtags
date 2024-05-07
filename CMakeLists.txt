cmake_minimum_required(VERSION 3.22)
project(
        readtags
        VERSION 0.3.0
        DESCRIPTION "a library for looking up tag entries in tag files"
        HOMEPAGE_URL "https://github.com/universal-ctags/libreadtags"
        LANGUAGES C
)

set(CMAKE_C_STANDARD 99)

option(LIBREADTAGS_BUILD_SHARED "Build shared/static library" OFF)

set(API_VERSION 1)
set(BUILD_VERSION 1.1.2)

if (LIBREADTAGS_BUILD_SHARED)
    message(STATUS "Building shared library")
    add_library(${PROJECT_NAME} SHARED readtags.c)
    set_target_properties(${PROJECT_NAME} PROPERTIES
            VERSION ${BUILD_VERSION}
            SOVERSION ${API_VERSION}
            PUBLIC_HEADER readtags.h
    )
    install(TARGETS ${PROJECT_NAME}
            LIBRARY DESTINATION lib
            PUBLIC_HEADER DESTINATION include
    )
else ()
    message(STATUS "Building static library")
    add_library(${PROJECT_NAME} STATIC readtags.c)
    install(TARGETS ${PROJECT_NAME}
            ARCHIVE DESTINATION lib
            PUBLIC_HEADER DESTINATION include
    )
endif ()

add_library(universal-ctags::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
