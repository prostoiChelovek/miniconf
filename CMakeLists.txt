cmake_minimum_required(VERSION 2.8)

project(miniconf)

add_library(miniconf INTERFACE)
target_sources(miniconf INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/src/miniconf.cpp)
target_include_directories(miniconf INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/src)

if(CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
    set(CMAKE_C_COMPILER gcc)
    set(CMAKE_CXX_COMPILER g++)
    set(CMAKE_CXX_FLAGS "-std=c++0x ${CMAKE_CXX_FLAGS} -O0 -g -Wall -Werror")

    add_executable(miniconf_example1 examples/miniconf_example1.cpp)
    add_executable(miniconf_example2 examples/miniconf_example2.cpp)

    target_link_libraries(miniconf_example1 miniconf)
    target_link_libraries(miniconf_example2 miniconf)
endif()