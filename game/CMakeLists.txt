file(GLOB_RECURSE GAME_SRC CONFIGURE_DEPENDS
    src/*.cpp
)

add_executable(Game ${GAME_SRC})

target_link_libraries(Game engine)

target_include_directories(Game PRIVATE
    ${CMAKE_SOURCE_DIR}/game/src
)
