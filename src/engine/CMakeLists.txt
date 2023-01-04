cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

project(skif_engine)

set(SOURCES
    mainloop.cpp
    programoptions.cpp
    encodecheck.cpp
    logger.cpp
)

set(HEADERS
    programoptions.h
    encodecheck.h
)

add_library (${PROJECT_NAME} ${SOURCES} ${HEADERS})
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "Skif")
