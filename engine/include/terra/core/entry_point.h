#pragma once

#include "terra/core/logger.h"
#include "terra/core/application.h"
#include "terra/debug/profiler.h"

#ifdef TR_PLATFORM_MACOS

extern terra::Application* terra::create_application(terra::CommandLineArgs args);

int main(int argc, char** argv) {
    terra::logger::init();

    PROFILE_BEGIN_SESSION("TerraInit", "terra_init.json");
    auto app = terra::create_application({ argc, argv });
    PROFILE_END_SESSION();

    PROFILE_BEGIN_SESSION("TerraRuntime", "terra_runtime.json");
    app->run();
    PROFILE_END_SESSION();

    PROFILE_BEGIN_SESSION("TerraDestruction", "terra_destruction.json");
    delete app;
    PROFILE_END_SESSION();
}

#endif
