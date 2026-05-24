// Gactorio - Energy Drink Factory Simulator
//
// GUI bootstrap. Wires together:
//   - the BE simulation (gactorio::FactoryController)
//   - the View layer (Application + FactoryPanel)
//
// The View consumes BE state strictly through FactorySnapshot (read-only DTO).
// All simulation mutations go through FactoryController commands.

#include "controller/FactoryController.hpp"

#include "views/Application.h"
#include "views/FactoryPanel.h"

#include <memory>

int main() {
    Application app("Gactorio - Energy Drink Factory", 1280, 720);

    gactorio::FactoryController controller;   // constructs default CarbonationFactory

    app.addPanel(std::make_unique<FactoryPanel>(controller));

    app.run();
    return 0;
}
