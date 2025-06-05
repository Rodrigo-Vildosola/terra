import sys
import platform

# Project names
ENGINE_NAME = "terra"
GAME_NAME = "example"
EDITOR_NAME = "lithos"

# Build
BUILD_DIR = ".build"
CXX_STANDARD = 20

# Feature flags
ENABLE_ASSERTS = True
ENABLE_DEBUG_LOGGING = True

# Platform flags
PLATFORM = platform.system()

TR_PLATFORM_MACOS = PLATFORM == "Darwin"
TR_PLATFORM_WINDOWS = PLATFORM == "Windows"
TR_PLATFORM_LINUX = PLATFORM == "Linux"
