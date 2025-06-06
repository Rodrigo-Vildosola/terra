import subprocess
import argparse
import shutil
import sys
import os

import tools.config as config

REQUIRED_DEPS = [
    "external/glad",
    "external/stb",
    "external/nlohmann",
    "external/glfw",
    "external/glm"
]

# Use config
BUILD_DIR = config.BUILD_DIR
EXECUTABLE_NAME = config.GAME_NAME

def generate_cmake_args(build_type="Debug"):
    defines = {
        "ENGINE_NAME": config.ENGINE_NAME,
        "GAME_NAME": config.GAME_NAME,
        "CMAKE_CXX_STANDARD": config.CXX_STANDARD,
        "CMAKE_BUILD_TYPE": build_type,
        "TR_ENABLE_ASSERTS": "ON" if config.ENABLE_ASSERTS else "OFF",
        "TR_ENABLE_DEBUG_LOGGING": "ON" if config.ENABLE_DEBUG_LOGGING else "OFF",
    }

    cmake_args = ["cmake", "-S", ".", "-B", config.BUILD_DIR]
    cmake_args += [f"-D{k}={v}" for k, v in defines.items()]
    return cmake_args

def run_cmd(cmd, cwd=None, fail_msg=None):
    try:
        print(f"📣 Running: {' '.join(cmd)}")
        subprocess.run(cmd, cwd=cwd, check=True)
    except subprocess.CalledProcessError as e:
        print(f"❌ {fail_msg or 'Command failed.'}")
        print("🔍 Error details:")
        print(e)
        sys.exit(1)

def init_submodules():
    print("🔄 Initializing git submodules...")
    run_cmd(["git", "submodule", "update", "--init", "--recursive"])

def check_dependencies():
    print("🔍 Checking for required dependencies...")
    missing = [dep for dep in REQUIRED_DEPS if not os.path.exists(dep)]
    if missing:
        print("❌ Missing dependencies:")
        for m in missing:
            print(f"  - {m}")
        print("\n💡 Attempting to auto-init submodules...")
        init_submodules()
        # Re-check
        missing = [dep for dep in REQUIRED_DEPS if not os.path.exists(dep)]
        if missing:
            print("❌ Still missing after init. Check manually.")
            sys.exit(1)
    else:
        print("✅ All dependencies are present.")

def configure_cmake():
    print("🔧 Configuring CMake...")
    os.makedirs(BUILD_DIR, exist_ok=True)
    cmake_args = generate_cmake_args()
    run_cmd(cmake_args)

def build(parallel=True, verbose=False):
    print("🛠️ Building project...")
    build_cmd = ["cmake", "--build", BUILD_DIR]
    if parallel:
        build_cmd += ["--parallel", str(os.cpu_count())]
    if verbose:
        build_cmd += ["--verbose"]
    run_cmd(build_cmd)

def clean():
    print("🧹 Cleaning build directory...")
    if os.path.exists(BUILD_DIR):
        shutil.rmtree(BUILD_DIR)
        print(f"🗑️ Removed {BUILD_DIR}/")
    else:
        print(f"⚠️ {BUILD_DIR}/ does not exist. Nothing to clean.")

def run_executable():
    exe_path = os.path.join(BUILD_DIR, EXECUTABLE_NAME)
    if os.name == 'nt':
        exe_path += ".exe"
    if not os.path.isfile(exe_path):
        print(f"❌ Executable not found: {exe_path}")
        print("💡 Try running: python build.py build")
        sys.exit(1)
    print(f"🚀 Running {exe_path}...")
    run_cmd([exe_path])

def main():
    if len(sys.argv) < 2:
        print("📋 Usage: python build.py [init | check | build | clean | run | all]")
        sys.exit(0)

    command = sys.argv[1].lower()

    if command == "init":
        init_submodules()
    elif command == "check":
        check_dependencies()
    elif command == "build":
        check_dependencies()
        configure_cmake()
        build()
    elif command == "clean":
        clean()
    elif command == "run":
        run_executable()
    elif command == "all":
        check_dependencies()
        configure_cmake()
        build()
        run_executable()
    else:
        print(f"❓ Unknown command: {command}")
        print("📋 Usage: python build.py [init | check | build | clean | run | all]")

if __name__ == "__main__":
    main()
