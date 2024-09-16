# Name: config_validation.cmake file
# ThundeRatz Robotics Team
# Brief: This file contains the existence checks of the declared variables
# 04/2023

###############################################################################
## Existence checks
###############################################################################

# Check if CMake build type is correctly configured
if(NOT (BUILD_TYPE STREQUAL "Release"        OR BUILD_TYPE STREQUAL "Debug" OR
        BUILD_TYPE STREQUAL "RelWithDebInfo" OR BUILD_TYPE STREQUAL "MinSizeRel"))
    set(BUILD_TYPE "RelWithDebInfo")
endif()
set(CMAKE_BUILD_TYPE ${BUILD_TYPE})
message(STATUS "Build type: " ${CMAKE_BUILD_TYPE})
