cmake_minimum_required(VERSION 3.21)


add_library(DITimer SHARED
    src/main.c
)
target_include_directories(DITimer PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/include
)
set_target_properties(DITimer PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${BIN_DIR}/DITimer
    PREFIX ""
    OUTPUT_NAME "DITimer"
)
target_link_options(DITimer PRIVATE
    -static
)

add_custom_command(
    OUTPUT ${BIN_DIR}/DITimer/config.json
    COMMAND ${CMAKE_COMMAND} -E make_directory ${BIN_DIR}
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_CURRENT_SOURCE_DIR}/config.json
        ${BIN_DIR}/DITimer/config.json
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/config.json
    COMMENT "Copying DITimer config.json"
)
add_custom_target(DITimer_config ALL
    DEPENDS ${BIN_DIR}/DITimer/config.json
)
