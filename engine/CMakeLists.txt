file(GLOB_RECURSE ENGINE_SRC CONFIGURE_DEPENDS
    src/*.cpp
    include/*.h
)

add_library(engine STATIC ${ENGINE_SRC})

target_include_directories(engine PUBLIC include)

target_link_libraries(engine
    glfw
    glad
    # stb
)

# Include GLM manually (header-only)
target_include_directories(engine PUBLIC external/glm)
