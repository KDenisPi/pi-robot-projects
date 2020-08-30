cmake_minimum_required(VERSION 3.7)

#project name
project(pidust)

set(DAEMON ${PROJECT_NAME}d)

set(VER_MJR 1)
set(VER_MIN 0)

#Pi-Robot folder
set(PI_ROBOT_HOME ${PROJECT_SOURCE_DIR}/../../pi-robot-dll)
message(STATUS "PI_ROBOT_HOME:  ${PI_ROBOT_HOME}")

#set(EXTRA_LIBS ${EXTRA_LIBS} pi-utils pi-library pi-smachine pthread dl curl crypt)
set(EXTRA_LIBS ${EXTRA_LIBS} pthread dl curl crypt)
message(STATUS "EXTRA_LIBS (proj):  ${EXTRA_LIBS}")

include_directories(BEFORE
    ${PI_ROBOT_HOME}/pi-utils
    ${PI_ROBOT_HOME}/pi-library
    ${PI_ROBOT_HOME}/pi-smachine
    ${PI_ROBOT_HOME}/pi-http
)

aux_source_directory(${PROJECT_SOURCE_DIR} PIDUST_SOURCES)

#-${VER_MJR}-${VER_MIN}
add_executable(${DAEMON} ${PIDUST_SOURCES})
target_link_libraries(${DAEMON} ${EXTRA_LIBS})

configure_file(version.h.in ${PROJECT_SOURCE_DIR}/version.h)
