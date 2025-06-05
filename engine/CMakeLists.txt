file(GLOB_RECURSE ENGINE_SRC CONFIGURE_DEPENDS
    src/*.cpp
    include/*.h
)

add_library(${ENGINE_NAME} STATIC ${ENGINE_SRC})

target_include_directories(${ENGINE_NAME} PUBLIC include)

target_link_libraries(${ENGINE_NAME}
    glfw
    glad
    # nlohmann_json::nlohmann_json
    # stb
)

target_include_directories(${ENGINE_NAME} PUBLIC external/glm)
