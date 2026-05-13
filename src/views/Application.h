#pragma once

#include "AppUI.h"
#include "UIComponent.h"

#include <memory>
#include <string>

// 전방 선언으로 GLFW 헤더를 .cpp로 미룬다 — main.cpp가 GLFW를 직접 알 필요 없음.
struct GLFWwindow;

// Application은 GLFW/OpenGL/ImGui 초기화와 메인 루프를 캡슐화한다.
// 사용자(main)는 addPanel()로 UI를 등록하고 run() 호출만 하면 된다.
class Application {
public:
    Application(const std::string& title, int width, int height);
    ~Application();

    // 복사/대입 금지 — 창과 GL 컨텍스트는 단일 소유
    Application(const Application&)            = delete;
    Application& operator=(const Application&) = delete;

    void addPanel(std::unique_ptr<UIComponent> panel);
    void run();

private:
    GLFWwindow* m_window = nullptr;
    AppUI       m_ui;
};
