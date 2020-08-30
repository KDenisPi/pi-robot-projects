cmake_minimum_required(VERSION 3.7)

#project name
project(pi-robot-projects)

message( STATUS "REAL_HARDWARE:  ${REAL_HARDWARE}" )

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

message( STATUS "CMAKE_C_FLAGS_RELWITHDEBINFO is ${CMAKE_C_FLAGS_RELWITHDEBINFO}")
message( STATUS "CMAKE_C_FLAGS_DEBUG is ${CMAKE_C_FLAGS_DEBUG}")

message( STATUS "GCC is ${CMAKE_CXX_COMPILER_VERSION}")
message( STATUS "CMAKE_CXX_FLAGS:  ${CMAKE_CXX_FLAGS}" )

add_subdirectory(${PROJECT_SOURCE_DIR}/dust-detector)
