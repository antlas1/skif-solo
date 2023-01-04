cmake_minimum_required(VERSION 3.16)

project(platform_http)

#find_package(SFML COMPONENTS system CONFIG REQUIRED)
find_path(CPP_HTTPLIB_INCLUDE_DIRS "httplib.h")

#https://github.com/LiamAbyss/httpServer
# all source files
set(SOURCES 
    http_platform.cpp
 #   httpserver.cpp
#    serverlog.cpp
    )
    
#set(HEADERS
#    httpserver.h
#    serverlog.h)

# define the sockets target
add_library(${PROJECT_NAME} ${SOURCES})
#target_link_libraries(${PROJECT_NAME} sfml-system sfml-network)
target_include_directories(${PROJECT_NAME} PRIVATE ${CPP_HTTPLIB_INCLUDE_DIRS})
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "Skif/platforms")
