#pragma once

#include "UIComponent.h"

#include <memory>
#include <vector>

// AppUI는 UI panel 컴포지터. 여러 패널을 보유하고 매 프레임 모두 render한다.
// 새 패널 추가는 addPanel()로 — AppUI 코드 수정 없이 확장 가능 (OCP).
class AppUI {
public:
    void addPanel(std::unique_ptr<UIComponent> panel);
    void renderAll();

private:
    std::vector<std::unique_ptr<UIComponent>> m_panels;
};
