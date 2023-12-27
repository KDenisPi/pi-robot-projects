cmake_minimum_required(VERSION 3.12)

#project name
project(pidust)

set(DAEMON ${PROJECT_NAME}d)

set(VER_MJR 1)
set(VER_MIN 0)

#Pi-Robot folder
set(PI_ROBOT_HOME ${PROJECT_SOURCE_DIR}/../../pi-robot-debian)
set(PI_ROBOT_HOME_BUILD ${PI_ROBOT_HOME}/build)
message(STATUS "PI_ROBOT_HOME:  ${PI_ROBOT_HOME}")

IF($ENV{USER} STREQUAL "pi")
    set(REAL_HARDWARE 1)
    set(THIRD_PARTY_HOME "${CMAKE_SOURCE_DIR}/../third-party")
ELSE($ENV{USER} STREQUAL "pi")
    set(REAL_HARDWARE 0)
    set(THIRD_PARTY_HOME "$ENV{HOME}/third-party")
ENDIF($ENV{USER} STREQUAL "pi")

message( STATUS "REAL_HARDWARE: ${REAL_HARDWARE}")
message( STATUS "THIRD_PARTY_HOME: ${THIRD_PARTY_HOME}")

set(MONGOOSE_HOME "${THIRD_PARTY_HOME}/mongoose-5.1")
set(JSONCONS "${THIRD_PARTY_HOME}/jsoncons/include")
set(MOSQUITTO_HOME "${THIRD_PARTY_HOME}/mosquitto")

message( STATUS "Process REAL HARDWARE")

find_library(PIUTILS_LIB pi-utils PATH ${PI_ROBOT_HOME_BUILD}/pi-utils)
find_library(PILIBRARY_LIB pi-library PATH ${PI_ROBOT_HOME_BUILD}/pi-library)
find_library(PISMACHINE_LIB pi-smachine PATH ${PI_ROBOT_HOME_BUILD}/pi-smachine)
find_library(PIHTTP_LIB pi-http PATH ${PI_ROBOT_HOME_BUILD}/pi-http)

set(EXTRA_LIBS ${EXTRA_LIBS} ${PIUTILS_LIB} ${PILIBRARY_LIB} ${PISMACHINE_LIB} ${PIHTTP_LIB} pthread dl curl crypt)

IF($ENV{USER} STREQUAL "pi")
    find_library(BCM_HOST_LIB bcm_host PATH "/opt/vc/lib")
    set(EXTRA_LIBS ${EXTRA_LIBS} ${BCM_HOST_LIB})
ENDIF($ENV{USER} STREQUAL "pi")

message(STATUS "EXTRA_LIBS (proj):  ${EXTRA_LIBS}")

configure_file(version.h.in ${PROJECT_SOURCE_DIR}/version.h)

include_directories(BEFORE
    ${PI_ROBOT_HOME}
    ${PI_ROBOT_HOME}/pi-main
    ${PI_ROBOT_HOME}/pi-utils
    ${PI_ROBOT_HOME}/pi-library
    ${PI_ROBOT_HOME}/pi-smachine
    ${PI_ROBOT_HOME}/pi-http
    ${PI_ROBOT_HOME}/pi-mqtt
    ${MONGOOSE_HOME}
    ${JSONCONS}
    ${MOSQUITTO_HOME}/lib/cpp
    ${MOSQUITTO_HOME}/lib
)

aux_source_directory(${PROJECT_SOURCE_DIR} PIDUST_SOURCES)

add_executable(${DAEMON} ${PIDUST_SOURCES})
target_link_libraries(${DAEMON} ${EXTRA_LIBS})

