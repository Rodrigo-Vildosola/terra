cmake_minimum_required(VERSION 3.16)

project(TerraProject)

# Use passed-in vars from Python
set(CMAKE_CXX_STANDARD ${CMAKE_CXX_STANDARD})
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# Pass to submodules
set(ENGINE_NAME ${ENGINE_NAME})
set(GAME_NAME ${GAME_NAME})

# Dependencies
add_subdirectory(external/glfw)
add_subdirectory(external/glad)
add_subdirectory(external/stb)
add_subdirectory(external/nlohmann)
add_subdirectory(external/glm)

# Targets
add_subdirectory(engine)
add_subdirectory(game)
