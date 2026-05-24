#pragma once

#include "AppUI.h"
#include "UIComponent.h"

#include <memory>
#include <string>

// Forward-declare so we don't pull GLFW into clients of this header.
struct GLFWwindow;

// Application encapsulates GLFW/OpenGL/ImGui initialization and the main loop.
// Clients (main) register UI panels with addPanel(), then call run().
class Application {
public:
    Application(const std::string& title, int width, int height);
    ~Application();

    // Single ownership of window & GL context.
    Application(const Application&)            = delete;
    Application& operator=(const Application&) = delete;

    void addPanel(std::unique_ptr<UIComponent> panel);
    void run();

private:
    GLFWwindow* m_window = nullptr;
    AppUI       m_ui;
};
