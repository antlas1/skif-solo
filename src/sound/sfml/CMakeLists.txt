cmake_minimum_required(VERSION 3.16)

project(sound_sfml)

find_package(SFML COMPONENTS system audio CONFIG REQUIRED)

# all source files
set(SOURCES 
    sfml_sound.cpp)

# define the sockets target
add_library(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} sfml-system sfml-audio)
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "Skif/sound")
