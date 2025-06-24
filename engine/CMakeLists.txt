file(GLOB_RECURSE ENGINE_SRC CONFIGURE_DEPENDS
    src/*.cpp
    include/*.h
)

add_library(${ENGINE_NAME} STATIC ${ENGINE_SRC})

target_include_directories(${ENGINE_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/external/glm  # <-- Add this if GLM is local
)

target_link_libraries(${ENGINE_NAME}
    glfw
    spdlog::spdlog
    glm
    imgui
    webgpu
    glfw3webgpu
)

# target_include_directories(${ENGINE_NAME} PUBLIC external/glm)
# target_include_directories(${ENGINE_NAME} PUBLIC external/spdlog/include)

target_compile_definitions(${ENGINE_NAME} PUBLIC IMGUI_DEFINE_MATH_OPERATORS)

target_compile_definitions(${ENGINE_NAME} PUBLIC EDITOR_ASSETS_DIR="${EDITOR_ASSETS_DIR}")
target_compile_definitions(${ENGINE_NAME} PUBLIC ENGINE_ASSETS_DIR="${ENGINE_ASSETS_DIR}")
target_compile_definitions(${ENGINE_NAME} PUBLIC GAME_ASSETS_DIR="${GAME_ASSETS_DIR}")

# target_compile_definitions(${ENGINE_NAME} PUBLIC TR_RELEASE)
