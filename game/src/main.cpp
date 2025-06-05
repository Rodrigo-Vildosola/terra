#include <terra/core/application.h>
#include <terra/core/entry_point.h>


class ExampleGame : public terra::Application {
public:
    ExampleGame(terra::CommandLineArgs args)
        : terra::Application("My Example Game", args) {}

    void on_init() override {
        TR_CORE_INFO("Game init");
    }

    void on_update() override {
        // update logic
    }

    void on_render() override {
        // OpenGL test code here
    }

    void on_shutdown() override {
        TR_CORE_INFO("Game shutting down.");
    }
};

terra::Application* terra::create_application(terra::CommandLineArgs args) {
    return new ExampleGame(args);
}
