cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

project(platform_console)

set(SOURCES
    console_platform.cpp
)

add_library (${PROJECT_NAME} ${SOURCES})
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "Skif/platforms")
