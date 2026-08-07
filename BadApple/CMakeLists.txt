cmake_minimum_required(VERSION 3.21)


add_library(BadApple SHARED
    src/main.c
)
target_include_directories(BadApple PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/include
)
set_target_properties(BadApple PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${BIN_DIR}/BadApple
    PREFIX ""
    OUTPUT_NAME "BadApple"
)
target_link_options(BadApple PRIVATE
    -static
)

add_custom_command(
    OUTPUT ${BIN_DIR}/BadApple/config.json
    COMMAND ${CMAKE_COMMAND} -E make_directory ${BIN_DIR}
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_CURRENT_SOURCE_DIR}/config.json
        ${BIN_DIR}/BadApple/config.json
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/config.json
    COMMENT "Copying BadApple config.json"
)
add_custom_target(BadApple_config ALL
    DEPENDS ${BIN_DIR}/BadApple/config.json
)
