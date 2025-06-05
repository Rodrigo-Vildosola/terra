project(nlohmann)

file(GLOB_RECURSE nlohmann_SRC
    "src/*.cpp"
)

add_library(nlohmann STATIC ${nlohmann_SRC})

target_include_directories(nlohmann PUBLIC include)
