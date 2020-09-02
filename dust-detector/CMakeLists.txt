cmake_minimum_required(VERSION 3.7)

#project name
project(pidust)

set(DAEMON ${PROJECT_NAME}d)

set(VER_MJR 1)
set(VER_MIN 0)

#Pi-Robot folder
set(PI_ROBOT_HOME ${PROJECT_SOURCE_DIR}/../../pi-robot-dll)
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

#set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)


    message( STATUS "Process REAL HARDWARE")

    find_library(PIUTILS_LIB pi-utils PATH ${PI_ROBOT_HOME}/bin)
    find_library(PILIBRARY_LIB pi-library PATH ${PI_ROBOT_HOME}/bin)
    find_library(PISMACHINE_LIB pi-smachine PATH ${PI_ROBOT_HOME}/bin)
    find_library(PIHTTP_LIB pi-http PATH ${PI_ROBOT_HOME}/bin)

    #pi-utils
    #add_library(pi-utils STATIC IMPORTED GLOBAL)
    #set_target_properties(pi-utils PROPERTIES IMPORTED_LOCATION ${PI_ROBOT_HOME_BUILD}/pi-utils/libpi-utils.a)
    #set_property(TARGET pi-utils PROPERTY IMPORTED_LOCATION ${PI_ROBOT_HOME_BUILD}/pi-utils/)

    #pi-library
    #add_library(pi-library STATIC IMPORTED GLOBAL)
    #set_property(TARGET pi-library PROPERTY IMPORTED_LOCATION ${PI_ROBOT_HOME_BUILD}/pi-library/libpi-library.a)

    #pi-smachine
    #add_library(pi-smachine STATIC IMPORTED GLOBAL)
    #set_property(TARGET pi-smachine PROPERTY IMPORTED_LOCATION ${PI_ROBOT_HOME_BUILD}/pi-smachine/libpi-smachine.a)

    #pi-core
    #add_library(pi-core STATIC IMPORTED GLOBAL)
    #set_target_properties(pi-core PROPERTIES IMPORTED_LOCATION ${PI_ROBOT_HOME_BUILD}/pi-core/libpi-core.a)
    #set_property(TARGET pi-core PROPERTY IMPORTED_LOCATION ${PI_ROBOT_HOME_BUILD}/pi-core/libpi-core.a)

    #pi-http
    #add_library(pi-http STATIC IMPORTED GLOBAL)
    #set_property(TARGET pi-http PROPERTY IMPORTED_LOCATION ${PI_ROBOT_HOME_BUILD}/pi-http/libpi-http.a)
    #find_library(PIHTTP_LIB pi-http PATH ${PI_ROBOT_HOME}/bin)

    find_library(BCM_HOST_LIB bcm_host PATH "/opt/vc/lib")

    #set(EXTRA_LIBS ${EXTRA_LIBS} ${PI_ROBOT_HOME_BUILD}/pi-utils/libpi-utils.a ${PI_ROBOT_HOME_BUILD}/pi-core/libpi-core.a  ${PI_ROBOT_HOME_BUILD}/pi-library/libpi-library.a pi-smachine pi-http pthread ${BCM_HOST_LIB} rt dl curl crypt)
    #set(EXTRA_LIBS ${EXTRA_LIBS} pi-utils pi-library pi-smachine pi-core pi-http pthread ${BCM_HOST_LIB} rt dl curl crypt)

    set(EXTRA_LIBS ${EXTRA_LIBS} ${PIUTILS_LIB} ${PILIBRARY_LIB} ${PISMACHINE_LIB} ${PIHTTP_LIB} pthread dl curl crypt ${BCM_HOST_LIB})

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
