#include "model/Inventory.hpp"
#include "model/Product.hpp"

#include <cassert>
#include <vector>

int main() {
    gactorio::Inventory inventory;
    inventory.addItem(gactorio::ItemType::Ingredient, 2);
    inventory.addItem(gactorio::ItemType::Water, 3);

    const std::vector<gactorio::ItemRequirement> tooExpensive = {
        gactorio::ItemRequirement(gactorio::ItemType::Ingredient, 2),
        gactorio::ItemRequirement(gactorio::ItemType::Water, 4),
    };

    assert(!inventory.hasEnough(tooExpensive));
    assert(!inventory.consume(tooExpensive));
    assert(inventory.getQuantity(gactorio::ItemType::Ingredient) == 2);
    assert(inventory.getQuantity(gactorio::ItemType::Water) == 3);

    const std::vector<gactorio::ItemRequirement> affordable = {
        gactorio::ItemRequirement(gactorio::ItemType::Ingredient, 1),
        gactorio::ItemRequirement(gactorio::ItemType::Water, 2),
    };

    assert(inventory.hasEnough(affordable));
    assert(inventory.consume(affordable));
    assert(inventory.getQuantity(gactorio::ItemType::Ingredient) == 1);
    assert(inventory.getQuantity(gactorio::ItemType::Water) == 1);
    assert(inventory.getQuantity(gactorio::ItemType::Label) == 0);

    return 0;
}
