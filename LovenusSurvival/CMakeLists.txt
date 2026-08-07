cmake_minimum_required(VERSION 3.21)


add_library(LovenusSurvival SHARED
    src/main.c
)
target_include_directories(LovenusSurvival PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/include
)
set_target_properties(LovenusSurvival PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${BIN_DIR}/LovenusSurvival
    PREFIX ""
    OUTPUT_NAME "LovenusSurvival"
)
target_link_options(LovenusSurvival PRIVATE
    -static
)

add_custom_command(
    OUTPUT ${BIN_DIR}/LovenusSurvival/config.json
    COMMAND ${CMAKE_COMMAND} -E make_directory ${BIN_DIR}
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_CURRENT_SOURCE_DIR}/config.json
        ${BIN_DIR}/LovenusSurvival/config.json
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/config.json
    COMMENT "Copying LovenusSurvival config.json"
)
add_custom_target(LovenusSurvival_config ALL
    DEPENDS ${BIN_DIR}/LovenusSurvival/config.json
)
