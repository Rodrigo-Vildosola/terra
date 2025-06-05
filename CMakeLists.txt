cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(TerraProject)

add_subdirectory(external/glfw)
add_subdirectory(external/glad)
add_subdirectory(external/stb)
add_subdirectory(external/nlohmann)
add_subdirectory(external/glm)

# Engine, Editor, Game
add_subdirectory(engine)
add_subdirectory(game)
# add_subdirectory(editor)  # Enable later if needed
