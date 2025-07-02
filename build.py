import argparse
from pathlib import Path
import subprocess
import shutil
import sys
import os
import logging

import tools.config as config
import tools.fmtgen as fmtgen

WEBGPU_CPP_PRINT_PATH = Path(".build/_deps/dawn-7187-macos-aarch64-release-src/include/dawn/webgpu_cpp_print.h")
FORMATTER_OUTPUT_PATH = Path("engine/include/terra/helpers/webgpu_fmt_formatters.h")

REQUIRED_DEPS = [
    "external/stb",
    "external/nlohmann",
    "external/glfw",
    "external/glm",
    "external/imgui",
    "external/spdlog",
    "external/webgpu"
]

# Config
BUILD_DIR = config.BUILD_DIR
EXECUTABLE_NAME = config.GAME_NAME
EDITOR_NAME = config.EDITOR_NAME

# Logging setup
class LogColor:
    HEADER = "\033[95m"
    OKBLUE = "\033[94m"
    OKGREEN = "\033[92m"
    WARNING = "\033[93m"
    FAIL = "\033[91m"
    ENDC = "\033[0m"

logging.basicConfig(
    format='[%(levelname)s] %(message)s',
    level=logging.INFO
)

def colored(msg, color):
    return f"{color}{msg}{LogColor.ENDC}"

def generate_cmake_args(build_type="Debug"):
    defines = {
        "ENGINE_NAME": config.ENGINE_NAME,
        "GAME_NAME": config.GAME_NAME,
        "EDITOR_NAME": config.EDITOR_NAME,
        "CMAKE_CXX_STANDARD": config.CXX_STANDARD,
        "CMAKE_BUILD_TYPE": build_type,
        "WEBGPU_BACKEND": "DAWN",
        "WEBGPU_BUILD_FROM_SOURCE": "OFF",
        "TR_ENABLE_ASSERTS": "ON" if config.ENABLE_ASSERTS else "OFF",
        "TR_ENABLE_DEBUG_LOGGING": "ON" if config.ENABLE_DEBUG_LOGGING else "OFF",
    }

    cmake_args = ["cmake", "-S", ".", "-B", config.BUILD_DIR]
    cmake_args += [f"-D{k}={v}" for k, v in defines.items()]
    return cmake_args

def run_cmd(cmd, cwd=None, fail_msg=None):
    try:
        logging.info(colored(f"Running: {' '.join(cmd)}", LogColor.OKBLUE))
        subprocess.run(cmd, cwd=cwd, check=True)
    except subprocess.CalledProcessError as e:
        logging.error(colored(fail_msg or 'Command failed.', LogColor.FAIL))
        print(colored(str(e), LogColor.FAIL))
        sys.exit(1)

def init_submodules():
    logging.info("Initializing git submodules...")
    run_cmd(["git", "submodule", "update", "--init", "--recursive"])

def check_dependencies():
    logging.info("Checking for required dependencies...")
    missing = [dep for dep in REQUIRED_DEPS if not os.path.exists(dep)]
    if missing:
        logging.error("Missing dependencies:")
        for m in missing:
            print(colored(f"  - {m}", LogColor.FAIL))
        logging.info("Attempting to auto-init submodules...")
        init_submodules()
        missing = [dep for dep in REQUIRED_DEPS if not os.path.exists(dep)]
        if missing:
            logging.error("Still missing after init. Please check manually.")
            sys.exit(1)
    else:
        logging.info(colored("All dependencies are present.", LogColor.OKGREEN))

def configure_cmake(build_type):
    logging.info(f"Configuring CMake ({build_type})...")
    os.makedirs(BUILD_DIR, exist_ok=True)
    cmake_args = generate_cmake_args(build_type)
    run_cmd(cmake_args)

def build(parallel=True, verbose=False):
    logging.info("Building project...")
    build_cmd = ["cmake", "--build", BUILD_DIR]
    if parallel:
        build_cmd += ["--parallel", str(os.cpu_count())]
    if verbose:
        build_cmd += ["--verbose"]

    if not fmtgen.generate_fmt_formatters(WEBGPU_CPP_PRINT_PATH, FORMATTER_OUTPUT_PATH):
        logging.warning("Failed to generate WebGPU formatters.")

    run_cmd(build_cmd)

def clean():
    logging.info("Cleaning build directory...")
    if os.path.exists(BUILD_DIR):
        shutil.rmtree(BUILD_DIR)
        logging.info(f"Removed {BUILD_DIR}/")
    else:
        logging.warning(f"{BUILD_DIR}/ does not exist. Nothing to clean.")

def run_executable():
    exe_path = os.path.join(BUILD_DIR, "bin", EXECUTABLE_NAME)
    if os.name == 'nt':
        exe_path += ".exe"
    if not os.path.isfile(exe_path):
        logging.error(f"Executable not found: {exe_path}")
        logging.info("Try running: python build.py build")
        sys.exit(1)
    logging.info(f"Running {exe_path}...")
    run_cmd([exe_path])

def main():
    parser = argparse.ArgumentParser(description="CMake-based build system for your game engine.")
    subparsers = parser.add_subparsers(dest="command", required=True)

    subparsers.add_parser("init", help="Initialize git submodules")
    subparsers.add_parser("check", help="Check for required dependencies")
    subparsers.add_parser("clean", help="Remove build directory")
    subparsers.add_parser("run", help="Run the built executable")

    build_parser = subparsers.add_parser("build", help="Configure and build the project")
    build_parser.add_argument("--no-parallel", action="store_true", help="Disable parallel build")
    build_parser.add_argument("--verbose", action="store_true", help="Enable verbose output")
    build_parser.add_argument("--config", default="Debug", help="Build configuration (Debug/Release)")

    all_parser = subparsers.add_parser("all", help="Check, configure, build and run")
    all_parser.add_argument("--no-parallel", action="store_true")
    all_parser.add_argument("--verbose", action="store_true")
    all_parser.add_argument("--config", default="Debug")

    args = parser.parse_args()

    if args.command == "init":
        init_submodules()
    elif args.command == "check":
        check_dependencies()
    elif args.command == "clean":
        clean()
    elif args.command == "run":
        run_executable()
    elif args.command == "build":
        check_dependencies()
        configure_cmake(args.config)
        build(parallel=not args.no_parallel, verbose=args.verbose)
    elif args.command == "all":
        check_dependencies()
        configure_cmake(args.config)
        build(parallel=not args.no_parallel, verbose=args.verbose)
        run_executable()

if __name__ == "__main__":
    main()
