cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

project(skif)
set(CMAKE_CXX_STANDARD 20)
enable_testing()
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

option(BUILD_PLATFORM_CONSOLE "Build console mode" ON)
option(BUILD_PLATFORM_TELNET "Build telnet mode. Must have SFML library" ON)
option(BUILD_PLATFORM_HTTP "Build http mode. Must have cpp-httplib library" ON)
option(BUILD_PLATFORM_GUI "Build GUI mode. Must have GTK library" ON)

option(BUILD_SOUND_SFML "Build hardware sound support. Must have SFML library" ON)
option(BUILD_SOUND_MUD "Build MUD sound protocol support. https://www.zuggsoft.com/zmud/msp.htm" ON)
option(BUILD_SOUND_MINIAUDIO "Build miniaudio. Can select deffirent backends" ON)

#Для студии ставим флаг, что кодировка исходников UTF-8
if(MSVC)
add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
endif()

find_package(Catch2 CONFIG QUIET)
if ( Catch2_FOUND )
message("SKIF: Build unittest")
endif()
   
add_subdirectory(engine)

#Выбираем директории для сборки, по настройкам платформ
if (BUILD_PLATFORM_CONSOLE)
    add_subdirectory(platform/console)
endif()
if (BUILD_PLATFORM_TELNET)
    add_subdirectory(platform/telnet)
endif()
if (BUILD_PLATFORM_HTTP)
    add_subdirectory(platform/http)
endif()
if (BUILD_PLATFORM_GUI)
    add_subdirectory(platform/gui)
endif()

#звуковые системы
add_subdirectory(sound/none)
if(BUILD_SOUND_SFML)
    add_subdirectory(sound/sfml)
endif()
if(BUILD_SOUND_MUD)
    add_subdirectory(sound/mud_protocol)
endif()
if(BUILD_SOUND_MINIAUDIO)
    add_subdirectory(sound/miniaudio)
endif()

#логгеры
add_subdirectory(loggers/none)
add_subdirectory(loggers/simple)
add_subdirectory(loggers/file)
if ( Catch2_FOUND )
add_subdirectory(loggers/catch2)
endif()

find_package(Catch2 CONFIG QUIET)
if ( Catch2_FOUND )
message("SKIF: Build unittest")
add_subdirectory(unittest)
endif()

set(HEADERS
    initargs.h
    soundcontroller.h
    textgame.h
    logger.h
)

add_library(${PROJECT_NAME} INTERFACE ${HEADERS})
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "Skif")
