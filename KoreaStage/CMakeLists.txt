cmake_minimum_required(VERSION 3.21)


add_library(KoreaStage SHARED
    src/main.c
)
target_include_directories(KoreaStage PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/include
)
set_target_properties(KoreaStage PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${BIN_DIR}/KoreaStage
    PREFIX ""
    OUTPUT_NAME "KoreaStage"
)
target_link_options(KoreaStage PRIVATE
    -static
)

add_custom_command(
    OUTPUT ${BIN_DIR}/KoreaStage/config.json
    COMMAND ${CMAKE_COMMAND} -E make_directory ${BIN_DIR}
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_CURRENT_SOURCE_DIR}/config.json
        ${BIN_DIR}/KoreaStage/config.json
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/config.json
    COMMENT "Copying KoreaStage config.json"
)
add_custom_target(KoreaStage_config ALL
    DEPENDS ${BIN_DIR}/KoreaStage/config.json
)
