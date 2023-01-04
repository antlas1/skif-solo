cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

project(logger_file)

set(SOURCES
    filelogger.cpp
)

add_library (${PROJECT_NAME} ${SOURCES})
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "Skif/loggers")
