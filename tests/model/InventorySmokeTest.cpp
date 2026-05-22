#include "model/Inventory.hpp"
#include "model/Product.hpp"

#include <cassert>
#include <vector>

int main() {
    gactorio::Inventory inventory;
    inventory.addItem(gactorio::ItemType::MetalPlate, 2);
    inventory.addItem(gactorio::ItemType::Screw, 3);

    const std::vector<gactorio::ItemRequirement> tooExpensive = {
        gactorio::ItemRequirement(gactorio::ItemType::MetalPlate, 2),
        gactorio::ItemRequirement(gactorio::ItemType::Screw, 4),
    };

    assert(!inventory.hasEnough(tooExpensive));
    assert(!inventory.consume(tooExpensive));
    assert(inventory.getQuantity(gactorio::ItemType::MetalPlate) == 2);
    assert(inventory.getQuantity(gactorio::ItemType::Screw) == 3);

    const std::vector<gactorio::ItemRequirement> affordable = {
        gactorio::ItemRequirement(gactorio::ItemType::MetalPlate, 1),
        gactorio::ItemRequirement(gactorio::ItemType::Screw, 2),
    };

    assert(inventory.hasEnough(affordable));
    assert(inventory.consume(affordable));
    assert(inventory.getQuantity(gactorio::ItemType::MetalPlate) == 1);
    assert(inventory.getQuantity(gactorio::ItemType::Screw) == 1);
    assert(inventory.getQuantity(gactorio::ItemType::Paint) == 0);

    return 0;
}
