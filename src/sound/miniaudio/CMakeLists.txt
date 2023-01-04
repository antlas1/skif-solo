cmake_minimum_required(VERSION 3.16)

project(sound_miniaudio)

# all source files
set(SOURCES 
    miniaudio_sound.cpp)

# define the sockets target
add_library(${PROJECT_NAME} ${SOURCES})
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "Skif/sound")
