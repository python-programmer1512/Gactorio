#pragma once

#include "UIComponent.h"

#include <memory>
#include <vector>

// AppUI is the UI panel compositor. It holds many panels and drives them
// each frame: update(dt) for all, then render() for all.
// Adding a new panel does NOT require modifying AppUI (OCP).
class AppUI {
public:
    void addPanel(std::unique_ptr<UIComponent> panel);

    void updateAll(double deltaTime);
    void renderAll();

private:
    std::vector<std::unique_ptr<UIComponent>> m_panels;
};
