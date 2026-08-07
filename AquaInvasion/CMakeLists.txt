cmake_minimum_required(VERSION 3.21)


add_library(AquaInvasion SHARED
    src/main.c
)
target_include_directories(AquaInvasion PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/include
)
set_target_properties(AquaInvasion PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${BIN_DIR}/AquaInvasion
    PREFIX ""
    OUTPUT_NAME "AquaInvasion"
)
target_link_options(AquaInvasion PRIVATE
    -static
)

add_custom_command(
    OUTPUT ${BIN_DIR}/AquaInvasion/config.json
    COMMAND ${CMAKE_COMMAND} -E make_directory ${BIN_DIR}
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_CURRENT_SOURCE_DIR}/config.json
        ${BIN_DIR}/AquaInvasion/config.json
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/config.json
    COMMENT "Copying AquaInvasion config.json"
)
add_custom_target(AquaInvasion_config ALL
    DEPENDS ${BIN_DIR}/AquaInvasion/config.json
)
