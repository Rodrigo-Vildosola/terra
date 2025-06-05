#pragma once

#include "terra/core/logger.h"
#include "terra/core/application.h"

#ifdef TR_PLATFORM_MACOS

extern terra::Application* terra::create_application(terra::CommandLineArgs args);

int main(int argc, char** argv) {
    terra::logger::init();
    auto app = terra::create_application({ argc, argv });
    app->init();
    app->run();
    delete app;
}

#endif
