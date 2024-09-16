# Name: CMakeLists.txt
# Micras Team
# Brief: CMake configuration file for the project
# 04/2023

cmake_minimum_required(VERSION 3.22)

###############################################################################
## CMake Configuration
###############################################################################

set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD 20)

set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

###############################################################################
## Project Configuration
###############################################################################

# The .ioc file used to generate the project will be PROJECT_RELEASE.ioc

set(CMAKE_PROJECT_NAME maze_solver)

###############################################################################
## Toolchain Configuration
###############################################################################

set(LAUNCH_JSON_PATH ${CMAKE_CURRENT_SOURCE_DIR}/.vscode/launch.json)
set(DEBUG_FILE_NAME ${CMAKE_PROJECT_NAME})

include(cmake/config_validation.cmake)

project(${CMAKE_PROJECT_NAME} CXX)

include(cmake/workspace.cmake)
include(cmake/targets.cmake)
include(cmake/linter.cmake)

###############################################################################
## Input files
###############################################################################

file(GLOB_RECURSE PROJECT_SOURCES CONFIGURE_DEPENDS "src/*.c*")
file(GLOB_RECURSE PROJECT_HEADERS CONFIGURE_DEPENDS "include/*.h*")
targets_generate_format_target(PROJECT_SOURCES PROJECT_HEADERS)

###############################################################################
## Main executable target
###############################################################################

add_executable(${PROJECT_NAME}
    ${PROJECT_SOURCES}
)

target_include_directories(${PROJECT_NAME} PUBLIC
    include
)

targets_generate_vsfiles_target(${PROJECT_NAME})
