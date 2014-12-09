cmake_minimum_required(VERSION 2.6)
project(twitchCursed)

# Get all source files
FILE(GLOB_RECURSE PLUGIN_SRC
    "${CMAKE_SOURCE_DIR}/plugins/*.cc"
)

include_directories (${CMAKE_SOURCE_DIR})


add_executable(twitchCursed main.cpp configFile.cpp twitch.cpp ${PLUGIN_SRC} pluginBase.cpp)
target_link_libraries(twitchCursed ncurses menu curl jansson)

install(TARGETS twitchCursed RUNTIME DESTINATION bin)
