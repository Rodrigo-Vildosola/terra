file(GLOB_RECURSE EDITOR_SRC CONFIGURE_DEPENDS
    src/*.cpp
)

add_executable(${EDITOR_NAME} ${EDITOR_SRC})

target_link_libraries(${EDITOR_NAME} engine)

target_include_directories(${EDITOR_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/editor/src
)

target_compile_definitions(${EDITOR_NAME} PUBLIC EDITOR_ASSETS_DIR="${EDITOR_ASSETS_DIR}")
