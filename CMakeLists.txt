cmake_minimum_required(VERSION 3.16)

project(TerraProject)

# Use passed-in vars from Python
set(CMAKE_CXX_STANDARD ${CMAKE_CXX_STANDARD})
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

set(EDITOR_ASSETS_DIR "${CMAKE_SOURCE_DIR}/editor/assets")
set(GAME_ASSETS_DIR "${CMAKE_SOURCE_DIR}/game/assets")


# Pass to submodules
set(ENGINE_NAME ${ENGINE_NAME})
set(GAME_NAME ${GAME_NAME})

# Dependencies
add_subdirectory(external/glfw)
add_subdirectory(external/stb)
add_subdirectory(external/nlohmann)
add_subdirectory(external/glm)
add_subdirectory(external/spdlog)
add_subdirectory(external/webgpu)
add_subdirectory(external/glfw3webgpu)


file(GLOB IMGUI_SRC
    external/imgui/*.cpp
    external/imgui/*.h
)

add_library(imgui STATIC ${IMGUI_SRC})

target_include_directories(imgui PUBLIC external/imgui)

# Targets
add_subdirectory(engine)
add_subdirectory(game)

# Add optimization flags for GCC and Clang
if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-O2 -Wall -Wextra -Wpedantic)
endif()

# Add optimization flags for MSVC
if (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    add_compile_options(/O2 /W4)
endif()



if (TR_ENABLE_ASSERTS)
    target_compile_definitions(${ENGINE_NAME} PUBLIC TR_ENABLE_ASSERTS)
endif()

if (TR_ENABLE_DEBUG_LOGGING)
    target_compile_definitions(${ENGINE_NAME} PUBLIC TR_ENABLE_DEBUG_LOGGING)
endif()



string(TOUPPER "${CMAKE_BUILD_TYPE}" BUILD_TYPE_UPPER)

if (BUILD_TYPE_UPPER STREQUAL "RELEASE" OR BUILD_TYPE_UPPER STREQUAL "RELWITHDEBINFO")
    target_compile_definitions(${ENGINE_NAME} PUBLIC TR_RELEASE=1)
elseif (BUILD_TYPE_UPPER STREQUAL "TESTING")
    target_compile_definitions(${ENGINE_NAME} PUBLIC TR_TESTING=1)
endif()
