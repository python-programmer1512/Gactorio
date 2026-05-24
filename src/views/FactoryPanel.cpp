#include "FactoryPanel.h"

#include "common/Types.hpp"

#include <imgui.h>

#include <cstdint>

namespace {

const char* eventTypeName(gactorio::EventType t) {
    using gactorio::EventType;
    switch (t) {
    case EventType::Info:              return "Info";
    case EventType::TaskEnqueued:      return "TaskEnqueued";
    case EventType::TaskStarted:       return "TaskStarted";
    case EventType::StepCompleted:     return "StepCompleted";
    case EventType::ProductCompleted:  return "ProductCompleted";
    case EventType::MachineBroken:     return "MachineBroken";
    case EventType::MachineRepaired:   return "MachineRepaired";
    case EventType::StateChanged:      return "StateChanged";
    case EventType::InputsConsumed:    return "InputsConsumed";
    case EventType::MachinePaused:     return "MachinePaused";
    }
    return "Unknown";
}

const char* productTypeName(gactorio::ProductType t) {
    using gactorio::ProductType;
    switch (t) {
    case ProductType::ToyCar:     return "Toy Car";
    case ProductType::MetalBox:   return "Metal Box";
    case ProductType::DroneFrame: return "Drone Frame";
    case ProductType::Unknown:    return "Unknown";
    }
    return "Unknown";
}

} // namespace

FactoryPanel::FactoryPanel(gactorio::FactoryController& ctrl) : m_ctrl(ctrl) {}

// -----------------------------------------------------------------------------
// update — drive the BE simulation each frame, then cache a fresh snapshot.
// -----------------------------------------------------------------------------
void FactoryPanel::update(double deltaTime) {
    if (m_simRunning) {
        m_ctrl.tick(deltaTime);
    }
    m_snapshot.emplace(m_ctrl.snapshot());
}

// -----------------------------------------------------------------------------
// render — read-only display of the cached snapshot.
// -----------------------------------------------------------------------------
void FactoryPanel::render() {
    if (!m_snapshot) return;
    const auto& snap = *m_snapshot;

    // -------------------------------------------------------------------------
    // Top-level control panel: simulation clock and global commands
    // -------------------------------------------------------------------------
    ImGui::Begin("Simulation Control");
    ImGui::Text("Simulation time: %.2f s", snap.timeSeconds());
    ImGui::Separator();

    if (ImGui::Button(m_simRunning ? "Pause" : "Resume")) {
        m_simRunning = !m_simRunning;
        if (m_simRunning) m_ctrl.resumeSimulation();
        else              m_ctrl.pauseSimulation();
    }
    ImGui::SameLine();
    if (ImGui::Button("Reset")) {
        m_ctrl.resetSimulation();
        m_simRunning = true;
    }
    ImGui::SameLine();
    if (ImGui::SliderFloat("Speed", reinterpret_cast<float*>(&m_speed), 0.1f, 5.0f, "%.1fx")) {
        m_ctrl.setSimulationSpeed(m_speed);
    }

    ImGui::Separator();
    ImGui::Text("Statistics");
    const auto& stats = snap.statistics();
    ImGui::BulletText("Tasks started   : %d", stats.startedTaskEvents());
    ImGui::BulletText("Steps completed : %d", stats.completedStepEvents());
    ImGui::BulletText("Products done   : %d", stats.completedProductEvents());
    ImGui::BulletText("Machines broken : %d", stats.brokenMachineEvents());
    ImGui::BulletText("Machines repaired: %d", stats.repairedMachineEvents());
    ImGui::BulletText("State changes   : %d", stats.stateChangedEvents());

    ImGui::End();

    // -------------------------------------------------------------------------
    // Production line / machine panel
    // -------------------------------------------------------------------------
    ImGui::Begin("Factory");

    if (snap.productionLines().empty()) {
        ImGui::TextDisabled("(no production lines configured yet)");
    }

    for (const auto& line : snap.productionLines()) {
        ImGui::PushID(static_cast<int>(line.id()));
        ImGui::SeparatorText(line.name().c_str());
        ImGui::Text("Queue length : %zu", line.queueLength());
        if (!line.currentTaskName().empty()) {
            ImGui::Text("Active task  : %s", line.currentTaskName().c_str());
            ImGui::ProgressBar(static_cast<float>(line.currentTaskProgress()), ImVec2(-1, 0));
        } else {
            ImGui::TextDisabled("Active task  : (idle)");
        }

        // Enqueue product buttons
        if (ImGui::SmallButton("+ Toy Car")) {
            m_ctrl.enqueueProduct(line.id(), gactorio::ProductType::ToyCar);
        }
        ImGui::SameLine();
        if (ImGui::SmallButton("+ Metal Box")) {
            m_ctrl.enqueueProduct(line.id(), gactorio::ProductType::MetalBox);
        }
        ImGui::SameLine();
        if (ImGui::SmallButton("+ Drone Frame")) {
            m_ctrl.enqueueProduct(line.id(), gactorio::ProductType::DroneFrame);
        }

        // Machines on this line
        if (ImGui::BeginTable("machines", 5,
                              ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
            ImGui::TableSetupColumn("Name");
            ImGui::TableSetupColumn("Type");
            ImGui::TableSetupColumn("State");
            ImGui::TableSetupColumn("Progress");
            ImGui::TableSetupColumn("Actions");
            ImGui::TableHeadersRow();

            for (const auto& mac : line.machines()) {
                ImGui::TableNextRow();

                ImGui::TableSetColumnIndex(0);
                ImGui::Text("%s", mac.name().c_str());

                ImGui::TableSetColumnIndex(1);
                ImGui::TextDisabled("%s", mac.typeName().c_str());

                ImGui::TableSetColumnIndex(2);
                ImGui::Text("%s  (HP %.0f)", mac.stateName().c_str(), mac.health());

                ImGui::TableSetColumnIndex(3);
                ImGui::ProgressBar(static_cast<float>(mac.progress()), ImVec2(-1, 0));

                ImGui::TableSetColumnIndex(4);
                ImGui::PushID(static_cast<int>(mac.id()));
                if (ImGui::SmallButton("Break")) {
                    m_ctrl.forceBreak(mac.id());
                }
                ImGui::SameLine();
                if (ImGui::SmallButton("Repair")) {
                    m_ctrl.repairMachine(mac.id());
                }
                ImGui::PopID();
            }
            ImGui::EndTable();
        }

        ImGui::PopID();
    }

    ImGui::End();

    // -------------------------------------------------------------------------
    // Event log panel — recent events from the Observer pattern
    // -------------------------------------------------------------------------
    ImGui::Begin("Event Log");
    const auto& events = snap.events();
    if (events.empty()) {
        ImGui::TextDisabled("(no events yet)");
    } else {
        ImGui::Text("Total events: %zu", events.size());
        ImGui::Separator();
        if (ImGui::BeginChild("event_scroll", ImVec2(0, 0), false,
                              ImGuiWindowFlags_AlwaysVerticalScrollbar)) {
            // Show most recent first (reverse)
            for (auto it = events.rbegin(); it != events.rend(); ++it) {
                ImGui::Text("[%7.2fs]  %-18s  %s",
                            it->timeSeconds(),
                            eventTypeName(it->type()),
                            it->message().c_str());
            }
        }
        ImGui::EndChild();
    }
    ImGui::End();

    // -------------------------------------------------------------------------
    // Inventory panel
    // -------------------------------------------------------------------------
    ImGui::Begin("Inventory");
    const auto& inv = snap.inventory();
    if (inv.items().empty()) {
        ImGui::TextDisabled("(empty)");
    } else {
        if (ImGui::BeginTable("inv", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
            ImGui::TableSetupColumn("Id");
            ImGui::TableSetupColumn("Qty");
            ImGui::TableHeadersRow();
            for (const auto& e : inv.items()) {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0); ImGui::Text("%s", e.id().c_str());
                ImGui::TableSetColumnIndex(1); ImGui::Text("%d", e.quantity());
            }
            ImGui::EndTable();
        }
    }
    ImGui::End();
}
