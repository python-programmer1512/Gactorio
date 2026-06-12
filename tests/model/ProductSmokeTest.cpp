#include "model/Product.hpp"
#include "model/ProductCatalog.hpp"

#include <cassert>

int main() {
    const gactorio::VoltzClassic voltz;
    const gactorio::HyperBolt    hyper;
    const gactorio::AuroraZero   aurora;

    const auto& catalog = gactorio::productDefinitions();
    assert(catalog.size() == 3);
    assert(gactorio::createProduct(gactorio::ProductType::HyperBolt)->getName() == "Hyper Bolt");

    // Every drink has a distinct product id.
    assert(voltz.getProductId() != hyper.getProductId());
    assert(hyper.getProductId() != aurora.getProductId());

    // All drinks share the same 5-item bill of materials shape.
    assert(voltz.getRequirements().size()  == 5);
    assert(hyper.getRequirements().size()  == 5);
    assert(aurora.getRequirements().size() == 5);

    // All drinks have a four-step route: Mixing -> Quality -> Bottling -> Packaging.
    assert(voltz.getRoute().size()  == 4);
    assert(hyper.getRoute().size()  == 4);
    assert(aurora.getRoute().size() == 4);

    assert(voltz.getRequirements().front().itemType() == gactorio::ItemType::Ingredient);
    assert(voltz.getRoute().front().requiredRole()    == gactorio::MachineRole::Mixing);
    assert(voltz.getRoute().back().requiredRole()     == gactorio::MachineRole::Packaging);

    // Durations match data/factory_config.json.
    assert(voltz.getRoute()[0].baseDurationSeconds()  == 13.0);
    assert(hyper.getRoute()[0].baseDurationSeconds()  == 18.0);
    assert(aurora.getRoute()[3].baseDurationSeconds() == 11.0);

    return 0;
}
