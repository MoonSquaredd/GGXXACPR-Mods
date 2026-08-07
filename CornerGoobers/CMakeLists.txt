cmake_minimum_required(VERSION 3.21)


add_library(CornerGoobers SHARED
    src/main.c
)
target_include_directories(CornerGoobers PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/include
)
set_target_properties(CornerGoobers PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${BIN_DIR}/CornerGoobers
    PREFIX ""
    OUTPUT_NAME "CornerGoobers"
)
target_link_options(CornerGoobers PRIVATE
    -static
)

add_custom_command(
    OUTPUT ${BIN_DIR}/CornerGoobers/config.json
    COMMAND ${CMAKE_COMMAND} -E make_directory ${BIN_DIR}
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_CURRENT_SOURCE_DIR}/config.json
        ${BIN_DIR}/CornerGoobers/config.json
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/config.json
    COMMENT "Copying CornerGoobers config.json"
)
add_custom_target(CornerGoobers_config ALL
    DEPENDS ${BIN_DIR}/CornerGoobers/config.json
)
