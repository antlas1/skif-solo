cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

project(unittests)

find_package(Catch2 CONFIG REQUIRED)

set(SOURCES
    test_parser.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} PRIVATE skif_engine logger_catch2 Catch2::Catch2 Catch2::Catch2WithMain)
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "Skif")

include(CTest)
include(Catch)
catch_discover_tests(${PROJECT_NAME})
