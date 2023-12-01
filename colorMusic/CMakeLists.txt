cmake_minimum_required(VERSION 3.7)

#project name
project(cmusic)

set(DAEMON ${PROJECT_NAME}d)

set(VER_MJR 0)
set(VER_MIN 1)

#
# C++ 14
#
IF(CMAKE_BUILD_TYPE STREQUAL "Debug")
    message("debug mode")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_C_FLAGS_DEBUG} -g -std=c++14 -fexceptions -Wno-psabi -Og")
ELSE(CMAKE_BUILD_TYPE NOT STREQUAL "Debug")
    message("release with debug info mode")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_C_FLAGS_RELWITHDEBINFO} -std=c++14 -fexceptions -Wno-psabi")
ENDIF(CMAKE_BUILD_TYPE STREQUAL "Debug")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

message( STATUS "CMAKE_C_FLAGS_RELWITHDEBINFO is ${CMAKE_C_FLAGS_RELWITHDEBINFO}")
message( STATUS "CMAKE_C_FLAGS_DEBUG is ${CMAKE_C_FLAGS_DEBUG}")

message( STATUS "GCC is ${CMAKE_CXX_COMPILER_VERSION}")
message( STATUS "CMAKE_CXX_FLAGS:  ${CMAKE_CXX_FLAGS}" )

#Pi-Robot folder
set(PI_ROBOT_HOME ${PROJECT_SOURCE_DIR}/../../pi-robot)
set(PI_ROBOT_HOME_BUILD ${PI_ROBOT_HOME}/build)
message(STATUS "PI_ROBOT_HOME:  ${PI_ROBOT_HOME}")

IF(REAL_HARDWARE)
    set(THIRD_PARTY_HOME "${CMAKE_SOURCE_DIR}/../third-party")
ELSE(REAL_HARDWARE)
  IF($ENV{USER} STREQUAL "pi")
    set(REAL_HARDWARE 1)
    set(THIRD_PARTY_HOME "${CMAKE_SOURCE_DIR}/../third-party")
  ELSE($ENV{USER} STREQUAL "pi")
    set(REAL_HARDWARE 0)
    set(THIRD_PARTY_HOME "$ENV{HOME}/third-party")
  ENDIF($ENV{USER} STREQUAL "pi")
ENDIF(REAL_HARDWARE)

message( STATUS "REAL_HARDWARE: ${REAL_HARDWARE}")
#message( STATUS "THIRD_PARTY_HOME: ${THIRD_PARTY_HOME}")

find_library(PIUTILS_LIB pi-utils PATH ${PI_ROBOT_HOME_BUILD})
find_library(PILIBRARY_LIB pi-library PATH ${PI_ROBOT_HOME_BUILD})

set(EXTRA_LIBS ${EXTRA_LIBS} ${PIUTILS_LIB} ${PILIBRARY_LIB} fftw3 pthread dl curl crypt)

#IF($ENV{USER} STREQUAL "pi")
#    find_library(BCM_HOST_LIB bcm_host PATH "/opt/vc/lib")
#    set(EXTRA_LIBS ${EXTRA_LIBS} ${BCM_HOST_LIB})
#ENDIF($ENV{USER} STREQUAL "pi")

message(STATUS "EXTRA_LIBS (proj):  ${EXTRA_LIBS}")

configure_file(version.h.in ${PROJECT_SOURCE_DIR}/version.h)

include_directories(BEFORE
    ${PI_ROBOT_HOME}
    ${PI_ROBOT_HOME}/pi-utils
    ${PI_ROBOT_HOME}/pi-library
)

aux_source_directory(${PROJECT_SOURCE_DIR} CMUSIC_SOURCES)

add_executable(${DAEMON} ${CMUSIC_SOURCES})
target_link_libraries(${DAEMON} ${EXTRA_LIBS})

