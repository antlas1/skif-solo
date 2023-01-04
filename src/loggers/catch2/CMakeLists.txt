cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

project(logger_catch2)

find_package(Catch2 CONFIG REQUIRED)

set(SOURCES
    catch2logger.cpp
)

add_library (${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} PRIVATE Catch2::Catch2)
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "Skif/loggers")
