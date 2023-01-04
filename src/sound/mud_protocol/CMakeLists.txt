cmake_minimum_required(VERSION 3.16)

project(sound_mud_protocol)

add_library(${PROJECT_NAME} soundmud.cpp)
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "Skif/sound")
