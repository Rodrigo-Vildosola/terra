file(GLOB_RECURSE ENGINE_SRC CONFIGURE_DEPENDS
    src/*.cpp
    include/*.h
)

add_library(${ENGINE_NAME} STATIC ${ENGINE_SRC})

target_include_directories(${ENGINE_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(${ENGINE_NAME}
    glfw
    glad
    spdlog::spdlog
    # nlohmann_json::nlohmann_json
    # stb
)

target_include_directories(${ENGINE_NAME} PUBLIC external/glm)
# target_include_directories(${ENGINE_NAME} PUBLIC external/spdlog/include)
