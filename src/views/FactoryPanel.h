#pragma once

#include "UIComponent.h"

#include "controller/FactoryController.hpp"
#include "dto/FactorySnapshot.hpp"

#include <optional>

// FactoryPanel renders the BE simulation state via a FactorySnapshot.
// It pulls a fresh snapshot every frame (update) and renders it (render).
// The View never touches Model objects directly — only Snapshots through the
// Controller. This honors the backend's architecture.md contract.
class FactoryPanel : public UIComponent {
public:
    explicit FactoryPanel(gactorio::FactoryController& ctrl);

    void update(double deltaTime) override;
    void render() override;

private:
    gactorio::FactoryController&            m_ctrl;
    std::optional<gactorio::FactorySnapshot> m_snapshot;
    bool                                    m_simRunning  = true;
    double                                  m_speed       = 1.0;
};
