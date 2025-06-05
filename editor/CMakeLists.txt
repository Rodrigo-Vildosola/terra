file(GLOB_RECURSE EDITOR_SRC CONFIGURE_DEPENDS
    src/*.cpp
)

add_executable(EditorExecutable ${EDITOR_SRC})

target_link_libraries(EditorExecutable engine)

target_include_directories(EditorExecutable PRIVATE
    ${CMAKE_SOURCE_DIR}/editor/src
)
