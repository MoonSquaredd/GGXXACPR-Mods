cmake_minimum_required(VERSION 3.21)


add_library(GoodMorning SHARED
    src/main.c
)
target_link_libraries(GoodMorning PRIVATE
    winmm
)
target_include_directories(GoodMorning PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/include
)
set_target_properties(GoodMorning PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${BIN_DIR}/GoodMorning
    PREFIX ""
    OUTPUT_NAME "GoodMorning"
)
target_link_options(GoodMorning PRIVATE
    -static
)

add_custom_command(
    OUTPUT ${BIN_DIR}/GoodMorning/config.json
    COMMAND ${CMAKE_COMMAND} -E make_directory ${BIN_DIR}
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_CURRENT_SOURCE_DIR}/config.json
        ${BIN_DIR}/GoodMorning/config.json
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/config.json
    COMMENT "Copying GoodMorning config.json"
)
add_custom_target(GoodMorning_config ALL
    DEPENDS ${BIN_DIR}/GoodMorning/config.json
)
