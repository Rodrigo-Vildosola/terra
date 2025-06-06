#include <terra/terra.h>

void processInput(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}


class ExampleGame : public terra::Application {
public:
    ExampleGame(terra::CommandLineArgs args)
        : terra::Application("My Example Game", args) {}

    void on_init() override {
        TR_INFO("Game init");
    }

    void on_update() override {
        // update logic
        processInput(this->get_window());
    }

    void on_render() override {
        // OpenGL test code here
    }

    void on_shutdown() override {
        TR_INFO("Game shutting down.");
    }
};

terra::Application* terra::create_application(terra::CommandLineArgs args) {
    return new ExampleGame(args);
}
