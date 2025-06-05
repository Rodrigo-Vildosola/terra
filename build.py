import os
import subprocess
import sys
import shutil

# Config
REQUIRED_DEPS = [
    "external/glad",
    "external/stb",
    "external/nlohmann",
    "external/glfw",
    "external/glm"
]

BUILD_DIR = ".build"
EXECUTABLE_NAME = "Game"  # Update if your target is named differently

# Utility
def run_cmd(cmd, cwd=None, fail_msg=None):
    try:
        print(f"📣 Running: {' '.join(cmd)}")
        subprocess.run(cmd, cwd=cwd, check=True)
    except subprocess.CalledProcessError:
        print(f"❌ {fail_msg or 'Command failed.'}")
        sys.exit(1)

# Dependency handling
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
        # Re-check after init
        missing = [dep for dep in REQUIRED_DEPS if not os.path.exists(dep)]
        if missing:
            print("❌ Still missing after init. Check manually.")
            sys.exit(1)
    else:
        print("✅ All dependencies are present.")

# Build
def configure_cmake():
    print("🔧 Configuring CMake...")
    os.makedirs(BUILD_DIR, exist_ok=True)
    run_cmd(["cmake", "-S", ".", "-B", BUILD_DIR])

def build():
    print("🛠️ Building project...")
    run_cmd(["cmake", "--build", BUILD_DIR])

def clean():
    print("🧹 Cleaning build directory...")
    if os.path.exists(BUILD_DIR):
        shutil.rmtree(BUILD_DIR)
        print(f"🗑️ Removed {BUILD_DIR}/")
    else:
        print(f"⚠️ {BUILD_DIR}/ does not exist. Nothing to clean.")

# Run
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

# CLI
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
