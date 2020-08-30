cmake_minimum_required(VERSION 3.7)

#project name
project(pidust)

set(DAEMON ${PROJECT_NAME}d)

set(VER_MJR 1)
set(VER_MIN 0)

#Pi-Robot folder
set(PI_ROBOT_HOME ${PROJECT_SOURCE_DIR}/../../pi-robot-dll)
message(STATUS "PI_ROBOT_HOME:  ${PI_ROBOT_HOME}")

find_library(PIUTILS_LIB pi-utils PATH ${PI_ROBOT_HOME}/bin)
find_library(PILIBRARY_LIB pi-library PATH ${PI_ROBOT_HOME}/bin)
find_library(PISMACHINE_LIB pi-smachine PATH ${PI_ROBOT_HOME}/bin)

IF($ENV{USER} STREQUAL "pi")
    set(REAL_HARDWARE 1)
    set(THIRD_PARTY_HOME "${CMAKE_SOURCE_DIR}/../third-party")
ELSE($ENV{USER} STREQUAL "pi")
    set(REAL_HARDWARE 0)
    set(THIRD_PARTY_HOME "$ENV{HOME}/third-party")
ENDIF($ENV{USER} STREQUAL "pi")

message( STATUS "THIRD_PARTY_HOME: ${THIRD_PARTY_HOME}")

set(MONGOOSE_HOME "${THIRD_PARTY_HOME}/mongoose-5.1")
set(JSONCONS "${THIRD_PARTY_HOME}/jsoncons/include")
set(MOSQUITTO_HOME "${THIRD_PARTY_HOME}/mosquitto")

#set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)


#set(EXTRA_LIBS ${EXTRA_LIBS} pi-utils pi-library pi-smachine pthread dl curl crypt)
set(EXTRA_LIBS ${EXTRA_LIBS} ${PIUTILS_LIB} ${PILIBRARY_LIB} ${PISMACHINE_LIB} pthread dl curl crypt)
message(STATUS "EXTRA_LIBS (proj):  ${EXTRA_LIBS}")

include_directories(BEFORE
    ${PI_ROBOT_HOME}
    ${PI_ROBOT_HOME}/pi-utils
    ${PI_ROBOT_HOME}/pi-library
    ${PI_ROBOT_HOME}/pi-smachine
    ${PI_ROBOT_HOME}/pi-http
    ${PI_ROBOT_HOME}/pi-mqtt
    ${MONGOOSE_HOME}
    ${JSONCONS}
    ${MOSQUITTO_HOME}
)

aux_source_directory(${PROJECT_SOURCE_DIR} PIDUST_SOURCES)

#-${VER_MJR}-${VER_MIN}
add_executable(${DAEMON} ${PIDUST_SOURCES})
target_link_libraries(${DAEMON} ${EXTRA_LIBS})

configure_file(version.h.in ${PROJECT_SOURCE_DIR}/version.h)
