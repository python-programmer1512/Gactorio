#include "model/DefaultProducts.hpp"
#include "model/Product.hpp"
#include "model/ProductCatalog.hpp"

#include <cassert>

int main() {
    constexpr gactorio::ProductId SodaCanProductId = 101;
    constexpr gactorio::ProductId SparklingWaterProductId = 102;
    constexpr gactorio::ProductId EnergyDrinkProductId = 103;

    gactorio::ProductCatalog catalog;
    gactorio::registerDefaultProducts(catalog);

    const auto sodaCan = catalog.createProduct(SodaCanProductId);
    const auto sparklingWater = catalog.createProduct(SparklingWaterProductId);
    const auto energyDrink = catalog.createProduct(EnergyDrinkProductId);
    assert(sodaCan != nullptr);
    assert(sparklingWater != nullptr);
    assert(energyDrink != nullptr);

    assert(sodaCan->getProductId() != sparklingWater->getProductId());
    assert(sparklingWater->getProductId() != energyDrink->getProductId());

    assert(sodaCan->getRequirements().size() == 5);
    assert(sparklingWater->getRequirements().size() == 4);
    assert(energyDrink->getRequirements().size() == 6);

    assert(sodaCan->getRoute().size() == 4);
    assert(sparklingWater->getRoute().size() == 4);
    assert(energyDrink->getRoute().size() == 4);

    assert(sodaCan->getRequirements().front().itemType() == gactorio::ItemType::Water);
    assert(sodaCan->getRoute().front().requiredRole() == gactorio::MachineRole::Carbonator);
    assert(energyDrink->getRoute().front().requiredRole() == gactorio::MachineRole::Carbonator);

    return 0;
}
