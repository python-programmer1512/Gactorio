#include "model/Product.hpp"

#include <cassert>

int main() {
    const gactorio::SodaCan sodaCan;
    const gactorio::SparklingWater sparklingWater;
    const gactorio::EnergyDrink energyDrink;

    assert(sodaCan.getProductId() != sparklingWater.getProductId());
    assert(sparklingWater.getProductId() != energyDrink.getProductId());

    assert(sodaCan.getRequirements().size() == 5);
    assert(sparklingWater.getRequirements().size() == 4);
    assert(energyDrink.getRequirements().size() == 6);

    assert(sodaCan.getRoute().size() == 4);
    assert(sparklingWater.getRoute().size() == 4);
    assert(energyDrink.getRoute().size() == 4);

    assert(sodaCan.getRequirements().front().itemType() == gactorio::ItemType::Water);
    assert(sodaCan.getRoute().front().requiredRole() == gactorio::MachineRole::Carbonator);
    assert(energyDrink.getRoute().front().requiredRole() == gactorio::MachineRole::Carbonator);

    return 0;
}
