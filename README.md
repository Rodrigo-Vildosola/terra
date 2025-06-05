# TestGL

This project was generated using the quickpp tool.

## Prerequisites

- [vcpkg](https://github.com/microsoft/vcpkg) is required for managing dependencies. Make sure you have vcpkg installed and set up before starting.
- Set the `VCPKG_ROOT` environment variable to the root directory of your vcpkg installation.

## Building the Project

To build the project, follow these steps:

1. **Navigate to the project directory:**
    ```sh
    cd OPENGL
    ```

2. **Install the required packages using `vcpkg`:**
    ```sh
    vcpkg add port [package_name]
    vcpkg install
    ```

3. **Build the project using the build script:**
    ```sh
    ./build.sh build
    ```

4. **Run the project using the build script:**
    ```sh
    ./build.sh run
    ```

5. **Clean the build directory using the build script:**
    ```sh
    ./build.sh clean
    ```

## Adding a Library

To add a new library to your project, follow these steps:

1. **Install the library using `vcpkg`:**
    ```sh
    vcpkg add port [package_name]
    vcpkg install
    ```

2. **Include the library in your `CMakeLists.txt` file:**
    ```cmake
    find_package([library_name] CONFIG REQUIRED)
    target_include_directories(TestGL PRIVATE ${[library_name]_INCLUDE_DIRS})
    target_link_libraries(TestGL PRIVATE [library_name])
    ```

3. **Rebuild the project using the build script:**
    ```sh
    ./build.sh build
    ```

Replace `[library_name]` with the name of the library you want to add.
