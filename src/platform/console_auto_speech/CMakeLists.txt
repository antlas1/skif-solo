cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

project(platform_console_speech)

set(SOURCES
    console_auto_speech.cpp
)

add_library (${PROJECT_NAME} ${SOURCES})
