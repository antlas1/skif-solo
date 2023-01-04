cmake_minimum_required(VERSION 3.16)

project(sound_none)

add_library(${PROJECT_NAME} soundnone.cpp)
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "Skif/sound")