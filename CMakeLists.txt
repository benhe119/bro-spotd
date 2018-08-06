cmake_minimum_required(VERSION 2.8.4)
project(bro-spotd2)

set(CMAKE_SOURCE_DIR ${PROJECT_SOURCE_DIR}/src)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

include_directories("${PROJECT_SOURCE_DIR}/include")
include_directories(/usr/include/iniparser)
include_directories(/usr/include/libspot)
include_directories(/usr/include/dbus-1.0)
include_directories(/usr/lib/x86_64-linux-gnu/dbus-1.0/include/)

add_executable(bro-spotd ${CMAKE_SOURCE_DIR}/main.cpp
        ${CMAKE_SOURCE_DIR}/MonitoredFeature.cpp
        ${CMAKE_SOURCE_DIR}/Dispotcher.cpp
        ${CMAKE_SOURCE_DIR}/Dbuspotcher.cpp)
target_link_libraries(bro-spotd spot broccoli pthread iniparser)