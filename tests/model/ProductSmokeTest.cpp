#include "model/Product.hpp"

#include <cassert>

int main() {
    const gactorio::ToyCar toyCar;
    const gactorio::MetalBox metalBox;
    const gactorio::DroneFrame droneFrame;

    assert(toyCar.getProductId() != metalBox.getProductId());
    assert(metalBox.getProductId() != droneFrame.getProductId());

    assert(toyCar.getRequirements().size() == 3);
    assert(metalBox.getRequirements().size() == 2);
    assert(droneFrame.getRequirements().size() == 3);

    assert(toyCar.getRoute().size() == 5);
    assert(metalBox.getRoute().size() == 2);
    assert(droneFrame.getRoute().size() == 3);

    assert(toyCar.getRequirements().front().itemType() == gactorio::ItemType::MetalPlate);
    assert(toyCar.getRoute().front().requiredRole() == gactorio::MachineRole::Processor);
    assert(droneFrame.getRoute().front().requiredRole() == gactorio::MachineRole::Processor);

    return 0;
}
