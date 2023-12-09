cmake_minimum_required(VERSION 3.12)

#project name
project(stream-info)
enable_testing()

include_directories(BEFORE
    ${PROJECT_SOURCE_DIR}/../../
)

add_executable(srm_info stream_info.cpp)
target_link_libraries(srm_info ${EXTRA_LIBS})
