# Terra Engine

**Terra** is a **research-focused**, **work-in-progress** C++ game engine built for **learning high-performance rendering architectures**. It uses **WebGPU via Dawn**, supports **cross-platform rendering**, and emphasizes **data-oriented design (DOD)**, modular abstractions, and efficient multi-pass rendering.

> ⚠️ Not production-ready — Terra is a personal exploration tool for experimenting with real-time rendering and modern engine design.


## Features

* **Cross-Platform Rendering** with [Dawn (WebGPU)](https://dawn.googlesource.com/)
* **Data-Oriented Design** — minimal indirection, better cache locality
* **Abstracted Renderer Architecture** — pipelines, materials, passes, instances
* **ImGui** UI integration
* **Layer System** for modular game logic
* **JSON Profiling** to Chrome Tracing
* **High-performance** instancing and batching support


## Build Instructions

### Prerequisites

* Python 3.6+
* CMake ≥ 3.16
* C++20-compatible compiler (tested with Clang and MSVC)
* Git (with submodule support)

### Dependencies (included via submodules)

* [GLFW](https://www.glfw.org/)
* [ImGui](https://github.com/ocornut/imgui)
* [glm](https://github.com/g-truc/glm)
* [spdlog](https://github.com/gabime/spdlog)
* [WebGPU (Dawn)](https://dawn.googlesource.com/dawn)
* [glfw3webgpu](https://github.com/eliemichel/glfw3webgpu) for native GLFW surface creation with WebGPU


### Setup & Build

```bash
# 1. Clone and initialize submodules
git clone https://github.com/Rodrigo-Vildosola/terra.git
cd terra
python build.py init

# 2. Build the engine (Debug by default)
python build.py build

# 3. Run the example game
python build.py run
```

You can also use:

```bash
python build.py all
```

To check dependencies, build, and run the game in one go.


## WebGPU Distribution

Terra uses the [**WebGPU-distribution**](https://github.com/eliemichel/WebGPU-distribution) system by [@eliemichel](https://github.com/eliemichel) to simplify native WebGPU integration with CMake:

<div align="center">
  <picture>
    <source media="(prefers-color-scheme: dark)" srcset="https://raw.githubusercontent.com/eliemichel/LearnWebGPU/main/images/webgpu-dark.svg">
    <source media="(prefers-color-scheme: light)" srcset="https://raw.githubusercontent.com/eliemichel/LearnWebGPU/main/images/webgpu-light.svg">
    <img alt="WebGPU Logo" src="images/webgpu-dark.svg" width="200">
  </picture>

<a href="https://github.com/eliemichel/WebGPU-distribution">WebGPU-distribution</a>  |  <a href="https://github.com/eliemichel/WebGPU-Cpp">WebGPU-C++</a>  |  <a href="https://github.com/eliemichel/glfw3webgpu">glfw3webgpu</a> <br/> <a href="https://discord.gg/2Tar4Kt564"> <img src="https://img.shields.io/static/v1?label=Discord&message=Join%20us!&color=blue&logo=discord&logoColor=white" alt="Discord | Join us!"/> </a>

</div>

> Terra explicitly uses the **Dawn** backend with **precompiled binaries**, though source builds are supported.

See [`external/webgpu`](external/webgpu) for full setup and integration.


## Project Structure

```
terra/
├── engine/               # Core engine modules (rendering, math, input, etc.)
├── game/                 # Your actual game or sandbox
├── external/             # Git submodules and 3rd-party libraries
├── tools/                # Python tools for build/config/formatting
├── build.py              # Python-based CMake driver
└── CMakeLists.txt        # Root CMake config
```


## Profiling & Performance

Terra supports **Chrome Tracing**-compatible profiling via its `terra/debug/profiler.h`. Use `PROFILE_FUNCTION()` or `PROFILE_SCOPE("name")` to annotate code.

To visualize:

1. Run your app
2. Look for the generated `profile.json` file
3. Open it in `chrome://tracing/`

> On macOS, use **Xcode Instruments** or `dtrace` for deeper, system-level profiling.


## Inspirations & Credits

Terra is built as a **learning engine**, inspired by two major educational resources:

* 🎓 [**Learn WebGPU** by @eliemichel](https://github.com/eliemichel/LearnWebGPU): for its **modern C++** bindings, **native WebGPU setup**, and **cross-platform rendering approach**.
* 🛠️ [**TheCherno’s Game Engine Series**](https://www.youtube.com/c/TheCherno): for its **layered architecture**, **logging/assertion systems**, and general **engine structure patterns**.

This engine combines the **data-driven rendering concepts** from *Learn WebGPU* with the **game-layered application model** taught by *TheCherno*, building a hybrid designed for both **experimentation** and **high-performance graphics learning**.

### Other Resources

* [WebGPU-distribution](https://github.com/eliemichel/WebGPU-distribution)
* [WebGPU-C++](https://github.com/eliemichel/WebGPU-Cpp)
* [glfw3webgpu](https://github.com/eliemichel/glfw3webgpu)



## License

This project is open-source and available under the MIT License.
