#include "model/Inventory.hpp"
#include "model/Product.hpp"

#include <cassert>
#include <vector>

int main() {
    constexpr gactorio::ProductId SodaCanProductId = 101;
    constexpr gactorio::ProductId EnergyDrinkProductId = 103;

    gactorio::Inventory inventory;
    inventory.addItem(gactorio::ItemType::Water, 2);
    inventory.addItem(gactorio::ItemType::Can, 3);

    const std::vector<gactorio::ItemRequirement> tooExpensive = {
        gactorio::ItemRequirement(gactorio::ItemType::Water, 2),
        gactorio::ItemRequirement(gactorio::ItemType::Can, 4),
    };

    assert(!inventory.hasEnough(tooExpensive));
    assert(!inventory.consume(tooExpensive));
    assert(inventory.getQuantity(gactorio::ItemType::Water) == 2);
    assert(inventory.getQuantity(gactorio::ItemType::Can) == 3);

    const std::vector<gactorio::ItemRequirement> affordable = {
        gactorio::ItemRequirement(gactorio::ItemType::Water, 1),
        gactorio::ItemRequirement(gactorio::ItemType::Can, 2),
    };

    assert(inventory.hasEnough(affordable));
    assert(inventory.consume(affordable));
    assert(inventory.getQuantity(gactorio::ItemType::Water) == 1);
    assert(inventory.getQuantity(gactorio::ItemType::Can) == 1);
    assert(inventory.getQuantity(gactorio::ItemType::Label) == 0);

    inventory.addProduct(SodaCanProductId, 2);
    const auto savedState = inventory.exportState();

    inventory.addItem(gactorio::ItemType::Water, 10);
    inventory.addItem(gactorio::ItemType::Label, 5);
    inventory.addProduct(EnergyDrinkProductId, 1);

    assert(inventory.getQuantity(gactorio::ItemType::Water) == 11);
    assert(inventory.products().at(SodaCanProductId) == 2);
    assert(inventory.products().at(EnergyDrinkProductId) == 1);

    inventory.restoreState(savedState);

    assert(inventory.getQuantity(gactorio::ItemType::Water) == 1);
    assert(inventory.getQuantity(gactorio::ItemType::Can) == 1);
    assert(inventory.getQuantity(gactorio::ItemType::Label) == 0);
    assert(inventory.products().at(SodaCanProductId) == 2);
    assert(inventory.products().count(EnergyDrinkProductId) == 0);

    return 0;
}
