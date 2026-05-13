#include "Application.h"

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <GLFW/glfw3.h>

#include <stdexcept>
#include <utility>

// -----------------------------------------------------------------------------
// 생성: GLFW init → 창/컨텍스트 생성 → ImGui init
// -----------------------------------------------------------------------------
Application::Application(const std::string& title, int width, int height) {
    if (!glfwInit()) {
        throw std::runtime_error("glfwInit failed");
    }

    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    m_window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    if (!m_window) {
        glfwTerminate();
        throw std::runtime_error("glfwCreateWindow failed");
    }
    glfwMakeContextCurrent(m_window);
    glfwSwapInterval(1);  // vsync

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(m_window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);
}

// -----------------------------------------------------------------------------
// 정리: 생성의 역순
// -----------------------------------------------------------------------------
Application::~Application() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    if (m_window) glfwDestroyWindow(m_window);
    glfwTerminate();
}

// -----------------------------------------------------------------------------
// 패널 등록 (AppUI로 위임)
// -----------------------------------------------------------------------------
void Application::addPanel(std::unique_ptr<UIComponent> panel) {
    m_ui.addPanel(std::move(panel));
}

// -----------------------------------------------------------------------------
// 메인 루프
// -----------------------------------------------------------------------------
void Application::run() {
    while (!glfwWindowShouldClose(m_window)) {
        glfwPollEvents();
        if (glfwGetWindowAttrib(m_window, GLFW_ICONIFIED)) {
            ImGui_ImplGlfw_Sleep(10);   // 최소화 상태에서는 CPU 점유 낮춤
            continue;
        }

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        m_ui.renderAll();   // 등록된 UIComponent 모두 그리기

        ImGui::Render();
        int w, h;
        glfwGetFramebufferSize(m_window, &w, &h);
        glViewport(0, 0, w, h);
        glClearColor(0.10f, 0.10f, 0.12f, 1.00f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(m_window);
    }
}
