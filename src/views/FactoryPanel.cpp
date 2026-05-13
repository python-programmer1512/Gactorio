#include "FactoryPanel.h"

#include <imgui.h>

#include <string>

FactoryPanel::FactoryPanel(FactoryController& ctrl) : m_ctrl(ctrl) {}

void FactoryPanel::render() {
    ImGui::Begin("Factory");

    for (std::size_t i = 0; i < m_ctrl.machineCount(); ++i) {
        const Machine& m = m_ctrl.machineAt(i);

        ImGui::PushID(static_cast<int>(i));  // 같은 라벨 충돌 방지
        ImGui::SeparatorText(m.displayName().c_str());

        // 상태 표시
        ImGui::Text("HP     : %d / %d", m.hp(), m.maxHp());
        ImGui::Text("Queue  : %d / %d", m.queueSize(), m.queueCapacity());
        ImGui::Text("Active : %d / %d", m.activeCount(), m.parallelSlots());
        ImGui::Text("Power  : %s", m.isOn() ? "ON" : "OFF");

        // 액션 버튼
        if (ImGui::Button(m.isOn() ? "Turn OFF" : "Turn ON")) {
            m_ctrl.onTogglePower(i);
        }
        ImGui::SameLine();
        if (ImGui::Button("Enqueue")) {
            m_ctrl.onEnqueue(i);
        }
        ImGui::SameLine();
        if (ImGui::Button("Clear Queue")) {
            m_ctrl.onClearQueue(i);
        }

        ImGui::PopID();
    }

    ImGui::End();
}
