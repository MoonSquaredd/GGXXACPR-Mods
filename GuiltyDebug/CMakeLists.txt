cmake_minimum_required(VERSION 3.21)


add_library(GuiltyDebug SHARED
    src/main.c
)
target_include_directories(GuiltyDebug PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/include
)
set_target_properties(GuiltyDebug PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${BIN_DIR}/GuiltyDebug
    PREFIX ""
    OUTPUT_NAME "GuiltyDebug"
)
target_link_options(GuiltyDebug PRIVATE
    -static
)

add_custom_command(
    OUTPUT ${BIN_DIR}/GuiltyDebug/config.json
    COMMAND ${CMAKE_COMMAND} -E make_directory ${BIN_DIR}
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_CURRENT_SOURCE_DIR}/config.json
        ${BIN_DIR}/GuiltyDebug/config.json
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/config.json
    COMMENT "Copying GuiltyDebug config.json"
)
add_custom_target(GuiltyDebug_config ALL
    DEPENDS ${BIN_DIR}/GuiltyDebug/config.json
)
