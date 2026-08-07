cmake_minimum_required(VERSION 3.21)


add_library(DI_BGM SHARED
    src/main.c
)
target_include_directories(DI_BGM PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/include
)
set_target_properties(DI_BGM PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${BIN_DIR}/DI_BGM
    PREFIX ""
    OUTPUT_NAME "DI_BGM"
)
target_link_options(DI_BGM PRIVATE
    -static
)

add_custom_command(
    OUTPUT ${BIN_DIR}/DI_BGM/config.json
    COMMAND ${CMAKE_COMMAND} -E make_directory ${BIN_DIR}
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_CURRENT_SOURCE_DIR}/config.json
        ${BIN_DIR}/DI_BGM/config.json
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/config.json
    COMMENT "Copying DI_BGM config.json"
)
add_custom_target(DI_BGM_config ALL
    DEPENDS ${BIN_DIR}/DI_BGM/config.json
)
