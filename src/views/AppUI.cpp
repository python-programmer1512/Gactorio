#include "AppUI.h"

#include <utility>

void AppUI::addPanel(std::unique_ptr<UIComponent> panel) {
    m_panels.push_back(std::move(panel));
}

void AppUI::renderAll() {
    for (auto& panel : m_panels) {
        panel->render();
    }
}
