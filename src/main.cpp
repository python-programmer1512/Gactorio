#include "controller/FactoryController.hpp"

#include <iostream>

int main() {
    gactorio::FactoryController controller;
    controller.tick(1.0);
    controller.tick(2.0);

    const auto snapshot = controller.snapshot();

    std::cout << "Gactorio backend example\n";
    std::cout << "Simulation time: " << snapshot.timeSeconds() << "s\n";
    std::cout << "Production lines: " << snapshot.productionLines().size() << "\n";
    std::cout << "Events: " << snapshot.events().size() << "\n";

    return 0;
}

