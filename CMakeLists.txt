cmake_minimum_required(VERSION 3.22)

if("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
  message(FATAL_ERROR "In-source builds are not allowed.")
endif()

option(BUILD_TESTS "Build tests" OFF)

# Language standards
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# Build type
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Debug")
endif()

# Project name
set(CMAKE_PROJECT_NAME compiler-ti-basic++)

# Enable compile commands
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# Core project settings
project(${CMAKE_PROJECT_NAME})
message("Build type: " ${CMAKE_BUILD_TYPE})

enable_language(C)

# Compiler library
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
  add_library(${CMAKE_PROJECT_NAME} SHARED)
else()
  add_library(${CMAKE_PROJECT_NAME} STATIC)
endif()

# All the warnings
target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE -Wall -Wextra -Werror -pedantic)

# Include directories
target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC include)

# Sources
file(GLOB_RECURSE LIB_SOURCES lib/*.c)
target_sources(${CMAKE_PROJECT_NAME} PRIVATE ${LIB_SOURCES})

# Add stb library
add_subdirectory(thirdparty/stb)

target_link_libraries(${CMAKE_PROJECT_NAME} PUBLIC stb)

# Tools
add_subdirectory(tools)

# Unit tests
if(BUILD_TESTS)
  add_subdirectory(tests)
endif()

