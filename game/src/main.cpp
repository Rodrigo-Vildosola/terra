#include <terra/core/application.h>

class ExampleGame : public terra::Application {
public:
    ExampleGame(terra::CommandLineArgs args)
        : terra::Application("Example Game", args) {}

    void on_init() override {
        TR_CORE_INFO("ExampleGame initialized");
    }

    void on_update() override {
        // Update logic
    }

    void on_render() override {
        // Render logic
    }

    void on_shutdown() override {
        TR_CORE_INFO("ExampleGame shutdown");
    }
};

terra::Application* terra::create_application(terra::CommandLineArgs args) {
    return new ExampleGame(args);
}

