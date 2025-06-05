file(GLOB_RECURSE GAME_SRC CONFIGURE_DEPENDS
    src/*.cpp
)

add_executable(${GAME_NAME} ${GAME_SRC})

target_link_libraries(${GAME_NAME} ${ENGINE_NAME})

target_include_directories(${GAME_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/game/src
)
