cmake_minimum_required(VERSION 3.14)

project(platform_gui)
find_package(wxWidgets CONFIG REQUIRED)

set(SOURCES
 platform_gui.cpp
 mainframe.cpp
 settingsdialog.cpp
)

set(HEADERS 
 mainframe.h
 settingsdialog.h
)

#add_executable (${PROJECT_NAME} WIN32 ${SOURCES} ${HEADERS})
add_library (${PROJECT_NAME} ${SOURCES} ${HEADERS})
target_link_libraries (${PROJECT_NAME} wx::core wx::base)
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "Skif/platforms")