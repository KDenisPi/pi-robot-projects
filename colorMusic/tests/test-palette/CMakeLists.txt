cmake_minimum_required(VERSION 3.12)

#project name
project(test-palette)
enable_testing()

include_directories(BEFORE
    ${PROJECT_SOURCE_DIR}/../../
)

add_executable(test_pal test_palette.cpp)
target_link_libraries(test_pal ${EXTRA_LIBS})

#add_test(NAME Gpio_Simple COMMAND sudo test_gpio)
