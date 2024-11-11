cmake_minimum_required(VERSION 3.22)

project(driver)

set(DRIVER_NAME ti-basic++)

add_executable(${DRIVER_NAME})

target_compile_options(${DRIVER_NAME} PRIVATE -Wall -Wextra -Werror -pedantic)

file(GLOB_RECURSE DRIVER_SOURCES src/*.c)
target_sources(${DRIVER_NAME} PRIVATE ${DRIVER_SOURCES})

target_include_directories(${DRIVER_NAME} PRIVATE include)

target_link_libraries(${DRIVER_NAME} PRIVATE compiler-ti-basic++)

install(TARGETS ${DRIVER_NAME})

