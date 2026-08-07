cmake_minimum_required(VERSION 3.21)


add_library(SlashSelect SHARED
    src/main.c
)
target_include_directories(SlashSelect PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/include
)
set_target_properties(SlashSelect PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${BIN_DIR}/SlashSelect
    PREFIX ""
    OUTPUT_NAME "SlashSelect"
)
target_link_options(SlashSelect PRIVATE
    -static
)

add_custom_command(
    OUTPUT ${BIN_DIR}/SlashSelect/config.json
    COMMAND ${CMAKE_COMMAND} -E make_directory ${BIN_DIR}
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_CURRENT_SOURCE_DIR}/config.json
        ${BIN_DIR}/SlashSelect/config.json
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/config.json
    COMMENT "Copying SlashSelect config.json"
)
add_custom_target(SlashSelect_config ALL
    DEPENDS ${BIN_DIR}/SlashSelect/config.json
)
