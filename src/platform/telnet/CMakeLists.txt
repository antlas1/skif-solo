cmake_minimum_required(VERSION 3.16)

project(platform_telnet)

find_package(SFML COMPONENTS system CONFIG REQUIRED)

# all source files
set(SOURCES 
    sfml_tcp_platform.cpp)

# define the sockets target
add_library(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} sfml-system sfml-network)
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "Skif/platforms")
