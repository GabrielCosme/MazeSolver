# Name: targets.cmake
# ThundeRatz Robotics Team
# Brief: This file contains auxiliary custom targets declarations
# 04/2023

###############################################################################
## Auxiliary Targets
###############################################################################

add_custom_target(clean_all
    COMMAND echo "Cleaning all build files..."
    COMMAND rm -rf ${CMAKE_CURRENT_BINARY_DIR}/*
)

add_custom_target(rebuild
    COMMAND ${CMAKE_MAKE_PROGRAM} clean_all
    COMMAND cmake ..
    COMMAND ${CMAKE_MAKE_PROGRAM}
)

function(targets_generate_format_target)
    set(FILES_LIST "")
    foreach(FILE ${ARGV})
        list(APPEND FILES_LIST ${${FILE}})
    endforeach()
    add_custom_target(format
        COMMAND clang-format -style=file -i ${FILES_LIST} --verbose
    )
endfunction()

function(targets_generate_vsfiles_target TARGET)
    if("${TARGET}" STREQUAL "${PROJECT_NAME}")
        set(TARGET_SUFFIX "")
    else()
        set(TARGET_SUFFIX _${TARGET})
    endif()

    set(DEBUG_FILE_NAME ${TARGET})

    set(input_file ${CMAKE_CURRENT_SOURCE_DIR}/cmake/templates/launch.json.in)
    set(ouput_save_file ${CMAKE_CURRENT_BINARY_DIR}/vsfiles/.vsfiles${TARGET_SUFFIX})

    configure_file(${input_file} ${ouput_save_file})

    add_custom_target(debug${TARGET_SUFFIX}
        COMMAND echo "Configuring VS Code files for ${TARGET}"
        COMMAND cat ${ouput_save_file} > ${LAUNCH_JSON_PATH}
    )

    add_dependencies(debug${TARGET_SUFFIX} ${TARGET})
endfunction()
