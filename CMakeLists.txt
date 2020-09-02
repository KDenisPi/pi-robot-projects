cmake_minimum_required(VERSION 3.7)

#project name
project(pi-robot-projects)

message( STATUS "REAL_HARDWARE:  ${REAL_HARDWARE}" )


#
# C++ 11
#
IF(CMAKE_BUILD_TYPE STREQUAL "Debug")
    message("debug mode")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_C_FLAGS_DEBUG} -g -std=c++11 -fexceptions -Wno-psabi -Og")
ELSE(CMAKE_BUILD_TYPE NOT STREQUAL "Debug")
    message("release with debug info mode")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_C_FLAGS_RELWITHDEBINFO} -std=c++11 -fexceptions -Wno-psabi")
ENDIF(CMAKE_BUILD_TYPE STREQUAL "Debug")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

message( STATUS "CMAKE_C_FLAGS_RELWITHDEBINFO is ${CMAKE_C_FLAGS_RELWITHDEBINFO}")
message( STATUS "CMAKE_C_FLAGS_DEBUG is ${CMAKE_C_FLAGS_DEBUG}")

message( STATUS "GCC is ${CMAKE_CXX_COMPILER_VERSION}")
message( STATUS "CMAKE_CXX_FLAGS:  ${CMAKE_CXX_FLAGS}" )

add_subdirectory(${PROJECT_SOURCE_DIR}/dust-detector)
